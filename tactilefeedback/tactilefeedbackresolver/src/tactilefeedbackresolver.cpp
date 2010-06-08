/*
* Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:  Class for resolving physical feedback type based on 
*                tactile feedback settings and current device state.
* Part of:      Tactile Feedback.
*
*/

#include <e32base.h>
#include <e32std.h>
#include <e32debug.h>

#include <centralrepository.h>
#include <ecom/implementationinformation.h>
#include <ProfileEngineInternalCRKeys.h>

#include "tactilefeedbackprivatecrkeys.h"
#include "tactilefeedbackinternalpskeys.h"
#include "tactilefeedbacktrace.h"

#include "tactilefeedbackresolver.h"
#include "tactilepropertywatcher.h"
#include "tactileplayer.h"
#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "tactilefeedbackresolverTraces.h"
#endif


// Security policies for reading and writing to our P&S property
_LIT_SECURITY_POLICY_PASS( KTactileReadPolicy );
_LIT_SECURITY_POLICY_C1(   KTactileWritePolicy, ECapabilityWriteDeviceData );
// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// Consructor.
// ---------------------------------------------------------------------------
//
CTactileFeedbackResolver::CTactileFeedbackResolver() : 
    iFeedbackStarted( EFalse ), 
    iLastFeedback( ETouchFeedbackNone ) 
    {
    }

// ---------------------------------------------------------------------------
// 2nd phase costructor.
// ---------------------------------------------------------------------------
//
void CTactileFeedbackResolver::ConstructL()
    {
    TRACE("CTactileFeedbackResolver::ConstructL - Begin");

    InitializeCrKeysL();

    TInt hapticsUid(0);  
    TInt audioUid(0);    
    User::LeaveIfError( iRepository->Get( KTactileHapticsPlugin, hapticsUid ) );
    User::LeaveIfError( iRepository->Get( KTactileAudioPlugin, audioUid ) );

    if ( hapticsUid )
        {
        CreateHapticsPlayerL( TUid::Uid( hapticsUid ) );
        }

    if ( audioUid )
        {
        CreateAudioPlayerL( TUid::Uid( audioUid ) );
        }    

    InitializePsKeysL();
    
    TRACE("CTactileFeedbackResolver::ConstructL - End");
    }

// ---------------------------------------------------------------------------
// 2-phased constructor.
// ---------------------------------------------------------------------------
//
CTactileFeedbackResolver* CTactileFeedbackResolver::NewL()
    {
    CTactileFeedbackResolver* self = new ( ELeave ) CTactileFeedbackResolver;
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// Destructor.
// ---------------------------------------------------------------------------
//
CTactileFeedbackResolver::~CTactileFeedbackResolver()
    {
    delete iCenRepNotifier;
    delete iRepository;
    delete iProfileRepository;
    delete iPropertyWatcher;
    delete iHapticsPlayer;
    delete iAudioPlayer;
    REComSession::FinalClose();
    }

TBool CTactileFeedbackResolver::IsHigherThanPlaying(
    TTouchLogicalFeedback aFeedback ) const
    {
    if( aFeedback == ETouchFeedbackBasicItem &&
        iLastFeedback == ETouchFeedbackSensitiveItem )
        {
        return ETrue;
        }
    
    return ( aFeedback == ETouchFeedbackPopUp || 
            aFeedback == ETouchFeedbackPopupOpen || 
            aFeedback == ETouchFeedbackPopupClose ||
            aFeedback == ETouchFeedbackBounceEffect ) &&
            ( iLastFeedback == ETouchFeedbackBasicButton ||
            iLastFeedback == ETouchFeedbackSensitiveButton ||
            iLastFeedback == ETouchFeedbackSensitiveItem ||
            iLastFeedback == ETouchFeedbackBasicItem ||
            iLastFeedback == ETouchFeedbackCheckbox );
    }

// ---------------------------------------------------------------------------
// We play feedback in case all three conditions are met:
// 
// #1 Either vibra or audio feedback is requested and globally enabled
// #2 Given feedback type is different than "None"
// #3 Player instance exists
// ---------------------------------------------------------------------------
//
void CTactileFeedbackResolver::PlayFeedback( 
    TTouchLogicalFeedback aFeedback,
    TBool aPlayVibra,
    TBool aPlayAudio )
    {
    // TRACE("CTactileFeedbackResolver::PlayFeedback - Begin");
    // Feedback filtering in order to prevent too many feedbacks
    // in a short time (e.g. when doing text selection).    
    TTime now;
    now.UniversalTime();
    
    TTimeIntervalMicroSeconds interval = 
        now.MicroSecondsFrom( iLastFeedbackTimeStamp );

    TBool willPlay = EFalse;
    
    if ( iMinimumInterval == TTimeIntervalMicroSeconds( 0 ) || 
         now < iLastFeedbackTimeStamp ||                       
         iMinimumInterval <= interval )
        {
        willPlay = ETrue;
        }
    else if ( IsHigherThanPlaying( aFeedback ) )
        {
        willPlay = ETrue;
        StopFeedback();
        }

    if ( willPlay )
        {
        // First store the timestamp of this feedback playing moment.
        // This really needs to be done when 
        // actually playing feedback (not when feedback was requested
        // but filtered out).
        iLastFeedbackTimeStamp = now;      

        iLastFeedback = aFeedback;
    
        // Force vibra- and audio feedback off if those are globally disabled
        if ( !iVibraEnabled )
            {
            aPlayVibra = EFalse;
            }
            
        if ( !iAudioEnabled )
            {
            aPlayAudio = EFalse;
            }
        
        // check silent mode, if device is in silent mode, 
        // audio feedback is not allowed.
        TInt err;
        TInt isAudioSupported;
        err = iProfileRepository->Get( KProEngSilenceMode, isAudioSupported );
        if ( KErrNone == err && 1 == isAudioSupported )
            {
            aPlayAudio = EFalse;
            }

        if ( ( aPlayVibra || aPlayAudio ) &&        // #1
               aFeedback != ETouchFeedbackNone )    // #2
            {
            OstTrace0( TACTILE_PERFORMANCE, TACTILE_RESOLVER_PLAY_FEEDBACK_1, "e_TACTILE_RESOLVER_PLAY_FEEDBACK 1");
            
            if ( iVibraFirst )   
                {
                if ( aPlayVibra && iHapticsPlayer )
                    {
                    iHapticsPlayer->PlayFeedback( aFeedback );    
                    }
                if ( aPlayAudio && iAudioPlayer )
                    {
                    iAudioPlayer->PlayFeedback( aFeedback );    
                    }                
                }
            else
                {
                if ( aPlayAudio && iAudioPlayer )
                    {
                    iAudioPlayer->PlayFeedback( aFeedback );    
                    }                
                if ( aPlayVibra && iHapticsPlayer )
                    {
                    iHapticsPlayer->PlayFeedback( aFeedback );    
                    }
                }

            OstTrace0( TACTILE_PERFORMANCE, TACTILE_RESOLVER_PLAY_FEEDBACK_0, "e_TACTILE_RESOLVER_PLAY_FEEDBACK 0");
            }
        }
    // TRACE("CTactileFeedbackResolver::PlayFeedback - End");
    }
    
// ---------------------------------------------------------------------------
// Central Repository related initializations.
// 
// ---------------------------------------------------------------------------
//   
void CTactileFeedbackResolver::InitializeCrKeysL()    
    {
    if ( !iRepository )
        {
        iRepository = CRepository::NewL( KCRUidTactileFeedback );    
        }

    if ( !iProfileRepository )
        {
        iProfileRepository = CRepository::NewL( KCRUidProfileEngine );
        }
    
    TInt minInterval(0);
    // Read and store minimun feedback interfal
    User::LeaveIfError( iRepository->Get( KTactileFeedbackMinimumInterval, 
                                          minInterval ) );
                                          
    // Time is in milliseconds in Central Repository
    iMinimumInterval = TTimeIntervalMicroSeconds( 1000*minInterval );
    
    
    TInt internalSettings(0); 
    
    User::LeaveIfError( iRepository->Get( KTactileInternalSettings, 
                                          internalSettings ) );

    iVibraFirst = internalSettings & KTactileInternalSettingsVibraPlayedFirst;

    // Notifier for Central Repository changes.
    if ( !iCenRepNotifier )
        {
        iCenRepNotifier = CCenRepNotifyHandler::NewL( *this, 
                                                      *iRepository );
        iCenRepNotifier->StartListeningL();             
        }
    }
    
// ---------------------------------------------------------------------------
// P&S -related initializations have been moved here for making
// ConstructL more clear.
// ---------------------------------------------------------------------------
//
void CTactileFeedbackResolver::InitializePsKeysL()
    {
    TInt err = RProperty::Define( 
                KPSUidTactileFeedback,
                KTactileFeedbackEnabled,
                RProperty::EInt,
                KTactileReadPolicy,
                KTactileWritePolicy );

    if ( err != KErrNone && err != KErrAlreadyExists )
        {
        User::Leave( err );
        }
        
    // Enable feedback by default.
    TInt playerType(0);
    playerType |= ETouchFeedbackVibra;
    playerType |= ETouchFeedbackAudio;    
    
    RProperty::Set( KPSUidTactileFeedback, KTactileFeedbackEnabled, playerType );    
    
    // Watcher will start monitoring immediately, i.e. we don't have to
    // start it separately.
    iPropertyWatcher = CTactilePropertyWatcher::NewL(
        *this,
        KPSUidTactileFeedback,
        KTactileFeedbackEnabled,
        CActive::EPriorityStandard );
                
    ReadFeedbackEnabledFromPS();    
    }

// ---------------------------------------------------------------------------
// We only change our bookkeeping in case we succeed in reading the P&S
// property
// ---------------------------------------------------------------------------
//
void CTactileFeedbackResolver::ReadFeedbackEnabledFromPS()
    {
    TInt tmpVal = 0;
    TInt err = iPropertyWatcher->Get( tmpVal );
    
    if ( err == KErrNone )
        {
        iVibraEnabled = EFalse;
        iAudioEnabled = EFalse;
        
        // Check if vibra feedback is enabled (bit 0x01).
        if ( (tmpVal & 0x01) )
            {
            iVibraEnabled = ETrue;
            }
        // Check if audio feedback is enabled (bit 0x02).            
        if ( (tmpVal & 0x02) )
            {
            iAudioEnabled = ETrue;            
            }
        }
    }   
    
// ---------------------------------------------------------------------------
// From class MTactilePropertyObserver
// Called when the property for globally enabling or disabling feedback
// changes
// ---------------------------------------------------------------------------
//
void CTactileFeedbackResolver::PropertyChangedL( const TUid aCategory, 
                                                 const TUint aKey )
    {
    if ( aCategory == KPSUidTactileFeedback && 
         aKey == KTactileFeedbackEnabled )
        {
        ReadFeedbackEnabledFromPS();
        }
    }    
    
// ---------------------------------------------------------------------------
// Start continuous feedback.
// ---------------------------------------------------------------------------
//   
void CTactileFeedbackResolver::StartFeedback( TTouchContinuousFeedback aType,
                                              TInt aIntensity )
    {
    if ( iHapticsPlayer && !iFeedbackStarted && iVibraEnabled )
        {
        iHapticsPlayer->StartFeedback( aType, aIntensity );
        iFeedbackStarted = ETrue;
        }
    }
        
// ---------------------------------------------------------------------------
// Modify continuous feedback.
// ---------------------------------------------------------------------------
//                        
void CTactileFeedbackResolver::ModifyFeedback( TInt aIntensity )
    {
    if ( iHapticsPlayer )
        {
        iHapticsPlayer->ModifyFeedback( aIntensity );
        }    
    }
    
// ---------------------------------------------------------------------------
// Stop continuous feedback.
// ---------------------------------------------------------------------------
//    
void CTactileFeedbackResolver::StopFeedback()
    {
    if ( iHapticsPlayer )
        {
        iHapticsPlayer->StopFeedback();
        iFeedbackStarted = EFalse;
        }  
    }

// ---------------------------------------------------------------------------
// Play preview feedback.
// ---------------------------------------------------------------------------
//    
void CTactileFeedbackResolver::PlayPreviewFeedback( TInt aLevel,
                          TTouchLogicalFeedback aFeedback,
                          TTouchFeedbackType aType )
    {
    if ( aType & ETouchFeedbackVibra && iHapticsPlayer )
        {
        iHapticsPlayer->PlayPreviewFeedback( aLevel, aFeedback );
        }
    if ( aType & ETouchFeedbackAudio && iAudioPlayer )
        {
        iAudioPlayer->PlayPreviewFeedback( aLevel, aFeedback );
        }    
    }

// ---------------------------------------------------------------------------
// Start preview of continuous feedback.
// ---------------------------------------------------------------------------
//
void CTactileFeedbackResolver::StartPreviewFeedback( TInt aLevel,
                           TTouchContinuousFeedback aFeedback,
                           TInt aIntensity,
                           TTouchFeedbackType aType )
    {
    if ( aType & ETouchFeedbackVibra && iHapticsPlayer )
        {
        iHapticsPlayer->StartPreviewFeedback( aLevel, aFeedback, aIntensity );
        }    
    }

// ---------------------------------------------------------------------------
// Stop preview of continuous feedback.
// ---------------------------------------------------------------------------
//
void CTactileFeedbackResolver::StopPreviewFeedback()
    {
    if ( iHapticsPlayer )
        {
        iHapticsPlayer->StopPreviewFeedback();
        }        
    }

// ---------------------------------------------------------------------------
// Create haptics player.
// ---------------------------------------------------------------------------
//    
void CTactileFeedbackResolver::CreateHapticsPlayerL( TUid aHapticsUid )
    {
    RImplInfoPtrArray implArray;
    CleanupClosePushL( implArray );
    CTactilePlayer::ListImplementationsL( ETactilePlayerHaptics, implArray );
    
    if (!implArray.Count())
        {
        TRACE( "CTactileFeedbackResolver::ConstructL - No Haptics plugins found" );
        }
    for ( TInt i(0) ; i < implArray.Count() && !iHapticsPlayer; i++ )
        {
        // Create haptics player. 
        if ( !iHapticsPlayer &&
           ( implArray[i]->ImplementationUid() == aHapticsUid ) )
            {
            TUid pluginUid = implArray[i]->ImplementationUid();
        
            // delete implementation info list contents
            implArray.ResetAndDestroy();
            
            iHapticsPlayer = CTactilePlayer::NewL( pluginUid, *iRepository );
            }
        }
    
    // Make sure that implArray is reseted also if plugin wasn't found.    
    if ( !iHapticsPlayer ) 
        {
        implArray.ResetAndDestroy();
        }
        
    CleanupStack::PopAndDestroy( &implArray );            
    }

// ---------------------------------------------------------------------------
// Create audio player.
// ---------------------------------------------------------------------------
//        
void CTactileFeedbackResolver::CreateAudioPlayerL( TUid aAudioUid )
    {
    RImplInfoPtrArray implArray;
    CleanupClosePushL( implArray );    
    CTactilePlayer::ListImplementationsL( ETactilePlayerAudio, implArray );
    
    if (!implArray.Count())
        {
        TRACE( "CTactileFeedbackResolver::ConstructL - No Audio plugins found" );
        }
    for ( TInt i(0) ; i < implArray.Count() && !iAudioPlayer; i++ )
        {            
        // Create audio player.           
        if ( !iAudioPlayer &&
             ( implArray[i]->ImplementationUid() == aAudioUid ) )
            {
            TUid pluginUid = implArray[i]->ImplementationUid();
        
            // delete implementation info list contents
            implArray.ResetAndDestroy();     
                   
            iAudioPlayer = CTactilePlayer::NewL( pluginUid, *iRepository );
            }                
        }
        
    // Make sure that implArray is reseted also if plugin wasn't found.
    if ( !iAudioPlayer ) 
        {
        implArray.ResetAndDestroy();
        }        
        
    CleanupStack::PopAndDestroy( &implArray );    
    }

// ---------------------------------------------------------------------------
// From MCenRepNotifyHandlerCallback.
// ---------------------------------------------------------------------------
//
void CTactileFeedbackResolver::HandleNotifyGeneric( TUint32 aId )
    {
    TRAP_IGNORE( DoHandleNotifyGenericL(aId ) );
    }

// ---------------------------------------------------------------------------
// Handle changes in tactile feedback settings.
// ---------------------------------------------------------------------------
//
void CTactileFeedbackResolver::DoHandleNotifyGenericL( TUint32 aId )
    {
    switch ( aId )
        {
        case KTactileFeedbackMinimumInterval:
            InitializeCrKeysL();
            break;
        case KTactileHapticsPlugin:
            {
            TInt hapticsUid(0);
            iRepository->Get( KTactileHapticsPlugin, hapticsUid );
            if ( iHapticsPlayer )
                {
                delete iHapticsPlayer;
                iHapticsPlayer = NULL;
                }
            if ( hapticsUid )
                {
                CreateHapticsPlayerL( TUid::Uid( hapticsUid ) );    
                }                    
            }
            break;
        case KTactileAudioPlugin:
            {
            TInt audioUid(0);
            iRepository->Get( KTactileAudioPlugin, audioUid );
            if ( iAudioPlayer )
                {
                delete iAudioPlayer;
                iAudioPlayer = NULL;
                }
            if ( audioUid )
                {
                CreateAudioPlayerL( TUid::Uid( audioUid ) );
                }
            }            
            break;
        default:
            break;
        }
    }
