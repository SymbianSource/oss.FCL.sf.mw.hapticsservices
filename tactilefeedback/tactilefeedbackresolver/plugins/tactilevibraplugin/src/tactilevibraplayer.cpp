/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Class for producing vibra feedback.
* Part of:      Tactile Feedback.
*
*/

#include <e32debug.h>
#include <hwrmvibra.h>
#include <centralrepository.h>
#include <ecom/implementationproxy.h>
#include <ProfileInternal.hrh>

#include "tactilefeedbackprivatecrkeys.h"
#include "tactilefeedbacktrace.h"

#include "tactilevibraplayer.h"
#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "tactilevibraplayerTraces.h"
#endif


// ---------------------------------------------------------------------------
// Constructor.
// ---------------------------------------------------------------------------
//
CTactileVibraPlayer::CTactileVibraPlayer( CRepository& aRepository ):
        iRepository ( aRepository )
    {    
    }

// ---------------------------------------------------------------------------
// 2nd phase constructor.
// ---------------------------------------------------------------------------
//
void CTactileVibraPlayer::ConstructL()
    {
    TRACE("CTactileVibraPlayer::ConstructL - Begin");

    User::LeaveIfError( iRepository.Get( KTactileFeedbackHapticsStrength, 
                                         iVibraLevel ) );
                                         
    if ( iVibraLevel > EProfileTactileFeedbackLevel5 )
        {
        User::Leave( KErrGeneral );
        }
        
    ReadSettings();

    iCenRepNotifier = CCenRepNotifyHandler::NewL( *this, 
                                                  iRepository, 
                                                  CCenRepNotifyHandler::EIntKey,
                                                  KTactileFeedbackHapticsStrength );
    iCenRepNotifier->StartListeningL();     

    TRACE("CTactileVibraPlayer::ConstructL - End");
    }

// ---------------------------------------------------------------------------
// 2-phased constructor.
// ---------------------------------------------------------------------------
//
CTactileVibraPlayer* CTactileVibraPlayer::NewL( CRepository& aRepository )
    {
    CTactileVibraPlayer* self = 
        new ( ELeave ) CTactileVibraPlayer ( aRepository );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// Destructor.
// ---------------------------------------------------------------------------
//
CTactileVibraPlayer::~CTactileVibraPlayer()
    {
    delete iCenRepNotifier;
    delete iVibra;
    iVibraParams.Close();
    }


// ---------------------------------------------------------------------------
// From class CTactilePlayer
//
// Duration and intensity for the vibra are selected from the array
// according to logical feedback type.
//
// Feedback is not produced in case duration or intensity is zero.
// ---------------------------------------------------------------------------
//
TInt CTactileVibraPlayer::PlayFeedback( TTouchLogicalFeedback aFeedback )
    {
    TInt ret(KErrNotReady);
    if ( iVibraLevel != EProfileTactileFeedbackOff )
        {
        TInt effectIndex = KErrNotFound;
        
        switch ( aFeedback )
            {
            case ETouchFeedbackBasic:               // flow through
            case ETouchFeedbackBasicButton:         // flow through
            case ETouchFeedbackBasicItem:           // flow through
            case ETouchFeedbackBounceEffect:        // flow through
            case ETouchFeedbackBasicSlider:         // flow through
            case ETouchFeedbackEditor:              // flow through
            case ETouchFeedbackLineSelection:       // flow through
            case ETouchFeedbackBlankSelection:      // flow through
            case ETouchFeedbackTextSelection:       // flow through
            case ETouchFeedbackEmptyLineSelection:  // flow through
            case ETouchFeedbackPopUp:               // flow through
            case ETouchFeedbackPopupOpen:           // flow through
            case ETouchFeedbackPopupClose:          // flow through
            case ETouchFeedbackItemScroll:          // flow through
            case ETouchFeedbackCheckbox:            // flow through
            case ETouchFeedbackMultitouchActivate:
            case ETouchFeedbackBasicKeypad:
            case ETouchFeedbackFlick:
            case ETouchFeedbackItemDrop:
            case ETouchFeedbackItemMoveOver:
            case ETouchFeedbackItemPick:
            case ETouchFeedbackMultipleCheckbox:
            case ETouchFeedbackRotateStep:
            case ETouchFeedbackStopFlick:
            case ETouchFeedbackLongPress:
                effectIndex = 0;
                break;
            case ETouchFeedbackSensitive:           // flow through
            case ETouchFeedbackSensitiveButton:     // flow through
            case ETouchFeedbackSensitiveItem:      
            case ETouchFeedbackSensitiveKeypad:
            case ETouchFeedbackSensitiveSlider:
                effectIndex = 1;                    
                break;                              
            default:                                
                effectIndex = 0;                    
                break;                              
            }
        
        if ( effectIndex < iVibraParams.Count() && 
             iVibraParams[effectIndex].iTime > 0 &&
             iVibraParams[effectIndex].iIntensity != 0)
            {
            TRACE2("CTactileVibraPlayer::PlayFeedback %d", aFeedback);  
            TRAP( ret, DoPlayFeedbackL( 
                iVibraParams[effectIndex].iTime, 
                iVibraParams[effectIndex].iIntensity ) ); 

            if (!ret)    
                {
                TRACE2("CTactileVibraPlayer: Vibra playing leave code: %d", ret);  
                }
            }        
        }
    return ret;
    }
    
// ---------------------------------------------------------------------------
// Start feedback.
// ---------------------------------------------------------------------------
//   
TInt CTactileVibraPlayer::StartFeedback( TTouchContinuousFeedback aType,
                                         TInt aIntensity )
    {
    TRACE("CTactileVibraPlayer::StartFeedback - Begin");
    TRAPD( ret, DoStartFeedbackL( aType, aIntensity ) );
    TRACE("CTactileVibraPlayer::StartFeedback - End");
    return ret;
    }
        
// ---------------------------------------------------------------------------
// Modify feedback.
// ---------------------------------------------------------------------------
//                        
TInt CTactileVibraPlayer::ModifyFeedback( TInt /*aIntensity*/ )
    {
    return KErrNotSupported;
    }
    
// ---------------------------------------------------------------------------
// Stop feedback.
// ---------------------------------------------------------------------------
//    
void CTactileVibraPlayer::StopFeedback()
    {
    TRACE("CTactileVibraPlayer::StopFeedback - Begin");
    if ( iVibra )
        {
        TRAP_IGNORE( iVibra->StopVibraL() );
        }    
    TRACE("CTactileVibraPlayer::StopFeedback - End");
    }
  
// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//    
TInt CTactileVibraPlayer::PlayPreviewFeedback( TInt aLevel, 
                                             TTouchLogicalFeedback aFeedback )    
    {
    TRACE("CTactileVibraPlayer::PlayPreviewFeedback - Begin");
    TInt ret( KErrArgument );
    
    if ( aLevel > EProfileTactileFeedbackOff  &&
         aLevel <= EProfileTactileFeedbackLevel5 )
        {
        iOriginalLevel = iVibraLevel;
        iVibraLevel = aLevel;
        ReadSettings();    
        ret = PlayFeedback( aFeedback );
        iVibraLevel = iOriginalLevel;
        ReadSettings();
        }    
    
    TRACE("CTactileVibraPlayer::PlayPreviewFeedback - End");
    return ret;    
    }

// ---------------------------------------------------------------------------
// Read vibra effects from central repository.
// ---------------------------------------------------------------------------
//
void CTactileVibraPlayer::ReadSettings()
    {    
    TRACE("CTactileVibraPlayer::ReadSettings - Begin");
    TTactileVibraParams basicParams;
    
    basicParams.iTime = 0;
    basicParams.iIntensity = 0;

    TTactileVibraParams sensitiveParams = basicParams;
        
    if ( iVibraLevel == EProfileTactileFeedbackLevel1 )
        {
        // Read settings for ETouchFeedbackBasic
        iRepository.Get( KTactileVibraIntensityBasicLevel1, 
                          basicParams.iIntensity );
        iRepository.Get( KTactileVibraDurationBasicLevel1,  
                          basicParams.iTime );
        
        // Read settings for ETouchFeedbackSensitive
        iRepository.Get( KTactileVibraIntensitySensitiveLevel1, 
                          sensitiveParams.iIntensity );
        iRepository.Get( KTactileVibraDurationSensitiveLevel1,  
                          sensitiveParams.iTime );
        }
    else if ( iVibraLevel == EProfileTactileFeedbackLevel2 )
        {
        // Read settings for ETouchFeedbackBasic
        iRepository.Get( KTactileVibraIntensityBasicLevel2, 
                          basicParams.iIntensity );
        iRepository.Get( KTactileVibraDurationBasicLevel2,  
                          basicParams.iTime );
        
        // Read settings for ETouchFeedbackSensitive
        iRepository.Get( KTactileVibraIntensitySensitiveLevel2, 
                          sensitiveParams.iIntensity );
        iRepository.Get( KTactileVibraDurationSensitiveLevel2,  
                          sensitiveParams.iTime );
        }
    else if ( iVibraLevel == EProfileTactileFeedbackLevel3 )
        {
        // Read settings for ETouchFeedbackBasic
        iRepository.Get( KTactileVibraIntensityBasicLevel3, 
                          basicParams.iIntensity );
        iRepository.Get( KTactileVibraDurationBasicLevel3,  
                          basicParams.iTime );
        
        // Read settings for ETouchFeedbackSensitive
        iRepository.Get( KTactileVibraIntensitySensitiveLevel3, 
                          sensitiveParams.iIntensity );
        iRepository.Get( KTactileVibraDurationSensitiveLevel3,  
                          sensitiveParams.iTime );
        }
    else if ( iVibraLevel == EProfileTactileFeedbackLevel4 )
        {
        // Read settings for ETouchFeedbackBasic
        iRepository.Get( KTactileVibraIntensityBasicLevel4, 
                          basicParams.iIntensity );
        iRepository.Get( KTactileVibraDurationBasicLevel4,  
                          basicParams.iTime );
        
        // Read settings for ETouchFeedbackSensitive
        iRepository.Get( KTactileVibraIntensitySensitiveLevel4, 
                          sensitiveParams.iIntensity );
        iRepository.Get( KTactileVibraDurationSensitiveLevel4,  
                          sensitiveParams.iTime );
        }
    else if ( iVibraLevel == EProfileTactileFeedbackLevel5 )
        {
        // Read settings for ETouchFeedbackBasic
        iRepository.Get( KTactileVibraIntensityBasicLevel5, 
                          basicParams.iIntensity );
        iRepository.Get( KTactileVibraDurationBasicLevel5,  
                          basicParams.iTime );
        
        // Read settings for ETouchFeedbackSensitive
        iRepository.Get( KTactileVibraIntensitySensitiveLevel5, 
                          sensitiveParams.iIntensity );
        iRepository.Get( KTactileVibraDurationSensitiveLevel5,  
                          sensitiveParams.iTime );
        }
    
    iVibraParams.Reset();

    TRAP_IGNORE( iVibraParams.AppendL( basicParams ) );
    TRAP_IGNORE( iVibraParams.AppendL( sensitiveParams ) );
    TRACE("CTactileVibraPlayer::ReadSettings - End");      
    }    
    
// ---------------------------------------------------------------------------
// Actually play feedback.
// ---------------------------------------------------------------------------
//
void CTactileVibraPlayer::DoPlayFeedbackL( TInt aTime, TInt aIntensity )
    {
    TRACE("CTactileVibraPlayer::DoPlayFeedbackL - Begin");
    if ( !iVibra )
        {
        // Construct iVibra at first feedback request.
        iVibra = CHWRMVibra::NewL();
        }
    
    TRACE("CTactileVibraPlayer::DoPlayFeedbackL - Starting Vibra");
    
    OstTrace0( TACTILE_PERFORMANCE, TACTILE_PLAY_VIBRA_FEEDBACK_1, "e_TACTILE_PLAY_VIBRA_FEEDBACK 1");
    
    iVibra->PulseVibraL( aTime, aIntensity ); 

    OstTrace0( TACTILE_PERFORMANCE, TACTILE_PLAY_VIBRA_FEEDBACK_0, "e_TACTILE_PLAY_VIBRA_FEEDBACK 0");
    TRACE("CTactileVibraPlayer::DoPlayFeedbackL - End");
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//    
void CTactileVibraPlayer::DoStartFeedbackL( TTouchContinuousFeedback /*aType*/,
                                            TInt /*aIntensity*/ )
    {
    if ( !iVibra )
        {
        // Construct iVibra at first feedback request.
        iVibra = CHWRMVibra::NewL();
        }    

    if ( iVibraParams.Count() )
        {
        TRACE2("CTactileVibraPlayer: StartVibraL  Duration: %d", KHWRMVibraMaxDuration ); 
        TRACE2("CTactileVibraPlayer: StartVibraL  Intensity: %d", iVibraParams[0].iIntensity );
        iVibra->StartVibraL( KHWRMVibraMaxDuration, 
                             iVibraParams[0].iIntensity );
        }    
    }                                            

// ---------------------------------------------------------------------------
// From MCenRepNotifyHandlerCallback.
// ---------------------------------------------------------------------------
//    
void CTactileVibraPlayer::HandleNotifyInt( TUint32 aId, TInt aNewValue )    
    {
    TRACE("CTactileVibraPlayer::HandleNotifyInt - Begin");
    if ( aId == KTactileFeedbackHapticsStrength )
        {
        iVibraLevel = aNewValue;
        if ( iVibraLevel != EProfileTactileFeedbackOff )
            {
            ReadSettings();    
            }
        }
    TRACE("CTactileVibraPlayer::HandleNotifyInt - End");
    }

//---------------------------------------------------------------------------
// ImplementationTable[]
//
//---------------------------------------------------------------------------
//
const TImplementationProxy ImplementationTable[] = 
    {
    IMPLEMENTATION_PROXY_ENTRY( 0x2001CB9B, CTactileVibraPlayer::NewL )
    };

//---------------------------------------------------------------------------
// TImplementationProxy* ImplementationGroupProxy()
//
//---------------------------------------------------------------------------
//
EXPORT_C const TImplementationProxy* ImplementationGroupProxy( TInt& aTableCount )
    {
    aTableCount = sizeof(ImplementationTable) / sizeof(TImplementationProxy);
    return ImplementationTable;
    }
   
// End of file
