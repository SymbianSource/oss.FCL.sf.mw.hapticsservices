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
* Description:  Class for producing haptics feedback.
* Part of:      Tactile Feedback.
*
*/


#include <f32file.h>
#include <e32debug.h>
#include <hwrmhaptics.h>
#include <centralrepository.h>
#include <ecom/implementationproxy.h>
#include <MProfileEngine.h>
#include <MProfile.h>
#include <MProfileExtraSettings.h>
#include <MProfileFeedbackSettings.h>
#include <CProfileChangeNotifyHandler.h>

#include "tactilefeedbackprivatecrkeys.h"
#include "tactilefeedbacktrace.h"

#include "tactilehapticsplayer.h"
#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "tactilehapticsplayerTraces.h"
#endif

// ======== MEMBER FUNCTIONS ========

const TInt KMaxEffectNameLen = 128;

// Pulse feedback's names
_LIT8( KTFBasic,                "TFBasic");
_LIT8( KTFSensitive,            "TFSensitive");
_LIT8( KTFBasicButton,          "TFBasicButton");
_LIT8( KTFSensitiveButton,      "TFSensitiveButton");
_LIT8( KTFBasicItem,            "TFBasicItem");             // Mapped to IVT file version 9.2
_LIT8( KTFSensitiveItem,        "TFSensitiveItem");         // Mapped to IVT file version 9.2
_LIT8( KTFBounceEffect,         "TFBounceEffect");          // Mapped to IVT file version 9.2
_LIT8( KTFBasicSlider,          "TFBasicSlider");           // Mapped to IVT file version 9.2
_LIT8( KTFEditor,               "TFEditor");                // Mapped to IVT file version 9.2
_LIT8( KTFLineSelection,        "TFLineSelection");
_LIT8( KTFBlankSelection,       "TFBlankSelection");
_LIT8( KTFTextSelection,        "TFTextSelection");
_LIT8( KTFEmptyLineSelection,   "TFEmptyLineSelection");
_LIT8( KTFPopUp,                "TFPopUp");
_LIT8( KTFPopupOpen,            "TFPopupOpen");             // Mapped to IVT file version 9.2
_LIT8( KTFPopupClose,           "TFPopupClose");            // Mapped to IVT file version 9.2
_LIT8( KTFItemScroll,           "TFItemScroll");            // Mapped to IVT file version 9.2
_LIT8( KTFCheckbox,             "TFCheckbox");
_LIT8( KTFBasicKeypad,          "TFBasicKeypad");           // Mapped to IVT file version 10.1
_LIT8( KTFSensitiveKeypad,      "TFSensitiveKeypad");       // Mapped to IVT file version 9.2
_LIT8( KTFMultitouchActivate,   "TFMultitouchActivate");    // Mapped to IVT file version 9.2
_LIT8( KTFFlick,                "TFFlick");                 // Mapped to IVT file version 10.1
_LIT8( KTFItemDrop,             "TFItemDrop");              // Mapped to IVT file version 10.1
_LIT8( KTFItemMoveOver,         "TFItemMoveOver");          // Mapped to IVT file version 10.1
_LIT8( KTFItemPick,             "TFItemPick");              // Mapped to IVT file version 10.1
_LIT8( KTFMultipleCheckbox,     "TFMultipleCheckbox");      // Mapped to IVT file version 10.1
_LIT8( KTFRotateStep,           "TFRotateStep");            // Mapped to IVT file version 10.1
_LIT8( KTFSensitiveSlider,      "TFSensitiveSlider");       // Mapped to IVT file version 10.1
_LIT8( KTFStopFlick,            "TFStopFlick");             // Mapped to IVT file version 10.1
_LIT8( KTFLongPress,              "TFLongPress");               // Mapped to IVT file version 10.1

// Continuous feedback's names
_LIT8( KTFContinuousSmooth,     "TFContinuousSmooth");
_LIT8( KTFContinuousSlider,     "TFContinuousSlider");
_LIT8( KTFContinuousInput,      "TFContinuousInput");
_LIT8( KTFContinuousPopup,      "TFContinuousPopup");       // Mapped to IVT file version 10.1
_LIT8( KTFContinuousPinch,      "TFContinuousPinch");       // Mapped to IVT file version 10.1
// ---------------------------------------------------------------------------
// Constructor.
// ---------------------------------------------------------------------------
//
CTactileHapticsPlayer::CTactileHapticsPlayer( CRepository& aRepository ) :
    iRepository(aRepository)
    {
    
    }

// ---------------------------------------------------------------------------
// 2nd phase constructor.
// ---------------------------------------------------------------------------
//
void CTactileHapticsPlayer::ConstructL()
    {
    TRACE("CTactileHapticsPlayer::ConstructL - Begin");
    iHaptics = CHWRMHaptics::NewL(NULL, NULL); 

    TUint32 suppMask( 0 );
    User::LeaveIfError( iHaptics->SupportedActuators( suppMask ) );

    TInt actuatorType( EHWRMLogicalActuatorAny );    
    iRepository.Get( KTactileFeedbackHapticsActuator, actuatorType );
    
    if ( actuatorType & suppMask )
        {
        iHaptics->OpenActuatorL( static_cast<THWRMLogicalActuators>(actuatorType) );    
        }    

    User::LeaveIfError( iHaptics->SetDeviceProperty( 
                                        CHWRMHaptics::EHWRMHapticsLicensekey, 
                                        KNullDesC8() ) );

    TInt strength(0);
    iRepository.Get( KTactileFeedbackHapticsStrength, strength );
    
    // Strength value in settings is multiplied by 100 to scale value 
    // suitable for haptics (0-10000).    
    iStrength = strength * 100;

    // As a temporary fix to EAKH-7LKANT, the strength is (over)read from
    // profiles engine
    InitializeProfilesEngineL();
    
    if ( iVibraLevel > EProfileTactileFeedbackLevel5 )
        {
        User::Leave( KErrGeneral );
        }

    User::LeaveIfError( iHaptics->SetDeviceProperty( 
                                        CHWRMHaptics::EHWRMHapticsStrength, 
                                        iStrength ) );
    TFileName ivtFile;
    if ( iVibraLevel != EProfileTactileFeedbackOff )
        {
        iRepository.Get( IVTFileId(), ivtFile );
        }
    else
        {
        // Load KTactileHapticsLevel1IVTFile as default IVT file
        // to start up if vibra level is EProfileTactileFeedbackOff.
        iRepository.Get( KTactileHapticsLevel1IVTFile, ivtFile );
        }
    HBufC8* ivtBuf = IVTBufAllocL( ivtFile );

    CleanupStack::PushL( ivtBuf );
    User::LeaveIfError( iHaptics->LoadEffectData( *ivtBuf, iIVTHandle ) );
    CleanupStack::PopAndDestroy( ivtBuf );
    
    iCenRepNotifier = CCenRepNotifyHandler::NewL( *this, 
                                                  iRepository );
    iCenRepNotifier->StartListeningL();    
    
    TRACE("CTactileHapticsPlayer::ConstructL - End");
    }

// ---------------------------------------------------------------------------
// 2-phased constructor.
// ---------------------------------------------------------------------------
//
CTactileHapticsPlayer* CTactileHapticsPlayer::NewL( CRepository& aRepository )
    {
    CTactileHapticsPlayer* self = 
                        new ( ELeave ) CTactileHapticsPlayer( aRepository );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// Destructor.
// ---------------------------------------------------------------------------
//
CTactileHapticsPlayer::~CTactileHapticsPlayer()
    {
    if( iProfileEng )
        {
        iProfileEng->Release();
        iProfileEng = NULL;
        }
    delete iProfileHandler;
    delete iCenRepNotifier;
    delete iHaptics;
    }

// ---------------------------------------------------------------------------
// From class CTactilePlayer
//
// ---------------------------------------------------------------------------
//
TInt CTactileHapticsPlayer::PlayFeedback( TTouchLogicalFeedback aFeedback )
    {
    // TRACE("CTactileHapticsPlayer::PlayFeedback - Start");
    
    TRACE2("CTactileHapticsPlayer::PlayFeedback type: %d", aFeedback );
    TRAPD( err, DoPlayFeedbackL( aFeedback )); 
    if ( err != KErrNone )
        {
        TRACE2("CTactileHapticsPlayer::PlayFeedback error: %d", err );
        }    

    // TRACE("CTactileHapticsPlayer::PlayFeedback - End");
    return err;
    }
    
// ---------------------------------------------------------------------------
// Actually play feedback.
// ---------------------------------------------------------------------------
//
void CTactileHapticsPlayer::DoPlayFeedbackL( TTouchLogicalFeedback aFeedback )
    {
    // Check if hapticts strength is set to zero or
    // vibra level is EProfileTactileFeedbackOff.
    if ( !iStrength || iVibraLevel == EProfileTactileFeedbackOff)
        {
        User::Leave(KErrNotReady);
        }
    
    TBuf8<KMaxEffectNameLen> name;
  
    switch( aFeedback )
        {
        case ETouchFeedbackBasic:
            name = KTFBasic;
            break;
        case ETouchFeedbackSensitive:
            name = KTFSensitive;
            break;
        case ETouchFeedbackBasicButton:
            name = KTFBasicButton;
            break;
        case ETouchFeedbackSensitiveButton:
            name = KTFSensitiveButton;
            break;
        case ETouchFeedbackBasicItem:
            name = KTFBasicItem;
            break;
        case ETouchFeedbackSensitiveItem:
            name = KTFSensitiveItem;
            break;
        case ETouchFeedbackBasicSlider:
            name = KTFBasicSlider;
            break;
        case ETouchFeedbackSensitiveSlider:
            name = KTFSensitiveSlider;
            break;
        case ETouchFeedbackBounceEffect:
            name = KTFBounceEffect;
            break;
        case ETouchFeedbackEditor:
            name = KTFEditor;
            break;
        case ETouchFeedbackLineSelection:
            name = KTFLineSelection;
            break;
        case ETouchFeedbackBlankSelection:
            name = KTFBlankSelection;
            break;
        case ETouchFeedbackTextSelection:
            name = KTFTextSelection;
            break;
        case ETouchFeedbackEmptyLineSelection:
            name = KTFEmptyLineSelection;
            break;                        
        case ETouchFeedbackPopUp:  
            name = KTFPopUp;
            break; 
       case ETouchFeedbackPopupOpen:
            name = KTFPopupOpen;
            break;     
        case ETouchFeedbackPopupClose:
            name = KTFPopupClose;
            break;
        case ETouchFeedbackItemScroll:
            name = KTFItemScroll;
            break;
        case ETouchFeedbackCheckbox:
            name = KTFCheckbox;
            break;
        case ETouchFeedbackBasicKeypad:
            name = KTFBasicKeypad;
            break;
        case ETouchFeedbackSensitiveKeypad:
            name = KTFSensitiveKeypad;
            break;
        case ETouchFeedbackMultitouchActivate:
            name = KTFMultitouchActivate;
            break;
        case ETouchFeedbackFlick:
            name = KTFFlick;
            break;
        case ETouchFeedbackItemDrop:
            name = KTFItemDrop;
            break;
        case ETouchFeedbackItemMoveOver:
            name = KTFItemMoveOver;
            break;
        case ETouchFeedbackItemPick:
            name = KTFItemPick;
            break;
        case ETouchFeedbackMultipleCheckbox:
            name = KTFMultipleCheckbox;
            break;
        case ETouchFeedbackRotateStep:
            name = KTFRotateStep;
            break;
        case ETouchFeedbackStopFlick:
            name = KTFStopFlick;
            break;
        case ETouchFeedbackLongPress:
            name = KTFLongPress;
            break;
        default:
            User::Leave( KErrArgument );
            break;
        }
    TInt effectIndex(0);
    User::LeaveIfError( iHaptics->GetEffectIndexFromName( iIVTHandle, 
                                                          name, 
                                                          effectIndex ) );
    TInt effectHandle(0);
    OstTrace0( TACTILE_PERFORMANCE, TACTILE_PLAY_HAPTICS_FEEDBACK_1, "e_TACTILE_PLAY_HAPTICS_FEEDBACK 1");
    User::LeaveIfError( iHaptics->PlayEffect( iIVTHandle, 
                                              effectIndex, 
                                              effectHandle ) );
    OstTrace0( TACTILE_PERFORMANCE, TACTILE_PLAY_HAPTICS_FEEDBACK_0, "e_TACTILE_PLAY_HAPTICS_FEEDBACK 0");
    }

// ---------------------------------------------------------------------------
// Get IVT file Id via vibra level.
// ---------------------------------------------------------------------------
//
TUint32 CTactileHapticsPlayer::IVTFileId()
    {
    TUint32 fileId = 0;
    switch ( iVibraLevel )
        {
        case EProfileTactileFeedbackLevel1:
            fileId = KTactileHapticsLevel1IVTFile;
            break;
        case EProfileTactileFeedbackLevel2:
            fileId = KTactileHapticsLevel2IVTFile;
            break;
        case EProfileTactileFeedbackLevel3:
            fileId = KTactileHapticsLevel3IVTFile;
            break;
        case EProfileTactileFeedbackLevel4:
            fileId = KTactileHapticsLevel4IVTFile;
            break;
        case EProfileTactileFeedbackLevel5:
            fileId = KTactileHapticsLevel5IVTFile;
            break;
        }
    return fileId;
    }

// ---------------------------------------------------------------------------
// Load IVT file.
// ---------------------------------------------------------------------------
//
HBufC8* CTactileHapticsPlayer::IVTBufAllocL( const TDesC& aFileName )
    {
    TRACE("CTactileHapticsPlayer::IVTBufAllocL - Begin");
    
    RFs fs;
    User::LeaveIfError( fs.Connect() );  
    CleanupClosePushL( fs );
    
    RFile file;
    User::LeaveIfError( file.Open( fs, aFileName, EFileRead ) );
    CleanupClosePushL( file );
    
    TInt fileSize( 0 );
    file.Size( fileSize );
    
    HBufC8* ivtFileBuf = HBufC8::NewLC( fileSize );
    TPtr8 dataBufPtr = ivtFileBuf->Des();
    
    User::LeaveIfError( file.Read( dataBufPtr ) );

    CleanupStack::Pop( ivtFileBuf );    
    CleanupStack::PopAndDestroy( &file );
    CleanupStack::PopAndDestroy( &fs );
    TRACE("CTactileHapticsPlayer::IVTBufAllocL - End");
    return ivtFileBuf;
    }
  
// ---------------------------------------------------------------------------
// Start feedback.
// ---------------------------------------------------------------------------
//   
TInt CTactileHapticsPlayer::StartFeedback( TTouchContinuousFeedback aFeedback,
                                           TInt aIntensity )
    {
    TRACE("CTactileHapticsPlayer::StartFeedback - Start");
    TInt ret(KErrNone);

    // If there's an ongoing feedback, stop it first.
    if ( iEffectHandle )
        {
        StopFeedback();
        }
     
    TBuf8<KMaxEffectNameLen> name;   
    switch( aFeedback )
        {
        case ETouchContinuousSmooth:
            name = KTFContinuousSmooth;
            break;
        case ETouchContinuousSlider:
            name = KTFContinuousSlider;
            break;               
        case ETouchContinuousInput:
            name = KTFContinuousInput;
            break;
        case ETouchContinuousPopup:
            name = KTFContinuousPopup;
            break;
        case ETouchContinuousPinch:
            name = KTFContinuousPinch;
            break;    
        default:
            ret = KErrArgument;
            break;
        }

    if ( ret == KErrNone )
        {
        ret = iHaptics->GetEffectIndexFromName( iIVTHandle, 
                                                name, 
                                                iEffectIndex );        
        }

    TInt effectType(0);
    if ( ret == KErrNone )
        {    
        ret = iHaptics->GetEffectType( iIVTHandle, 
                                       iEffectIndex, 
                                       effectType );
        }           
    
    if ( ret == KErrNone )
        {
        switch ( effectType )                               
            {
            case CHWRMHaptics::EHWRMHapticsTypePeriodic:
                {
                CHWRMHaptics::THWRMHapticsPeriodicEffect periodicDef;

                ret = iHaptics->GetPeriodicEffectDefinition( iIVTHandle,
                                                             iEffectIndex,
                                                             periodicDef );

                // Effect's magnitude value in IVT file is used as max value for 
                // continuous effects and Attack level is used as minimum value
                iMultiplier = ( periodicDef.iMagnitude - periodicDef.iAttackLevel ) / 100;
                periodicDef.iMagnitude = aIntensity * iMultiplier + periodicDef.iAttackLevel;

                if ( ret == KErrNone )
                    {
                    ret = iHaptics->PlayPeriodicEffect( periodicDef,
                                                        iEffectHandle );
                    }
                }
                break;
            case CHWRMHaptics::EHWRMHapticsTypeMagSweep:
                {
                CHWRMHaptics::THWRMHapticsMagSweepEffect magSweepDef;

                ret =iHaptics->GetMagSweepEffectDefinition( iIVTHandle,
                                                            iEffectIndex,
                                                            magSweepDef );
                 
                // Effect's magnitude value in IVT file is used as max value for
                // continuous effects and Attack level is used as minimum value
                iMultiplier = ( magSweepDef.iMagnitude - magSweepDef.iAttackLevel ) / 100;
                magSweepDef.iMagnitude = aIntensity * iMultiplier + magSweepDef.iAttackLevel;
                
                if ( ret == KErrNone )
                    {
                    ret =iHaptics->PlayMagSweepEffect( magSweepDef,
                                                       iEffectHandle );  
                    }
                }
                break;            
            default:
                TRACE("CTactileHapticsPlayer::StartFeedback - Playing effect not found");
                ret = KErrArgument;
                break;
            }            
        }  

    TRACE("CTactileHapticsPlayer::StartFeedback - End");
    return ret;     
    }
        
// ---------------------------------------------------------------------------
// Modify feedback.
// ---------------------------------------------------------------------------
//                        
TInt CTactileHapticsPlayer::ModifyFeedback( TInt aIntensity )
    {
    TRACE("CTactileHapticsPlayer::ModifyFeedback - Start");
    TInt ret(KErrNotReady);
    
    // Check if there's feedback started, do nothing if not.
    if ( iEffectHandle )
        {
        TInt effectType(0);

        ret = iHaptics->GetEffectType( iIVTHandle, 
                                       iEffectIndex, 
                                       effectType );

        TRACE2("CTactileHapticsPlayer::ModifyFeedback - effect type %d", effectType );
        
        if ( ret == KErrNone )
            {
            TInt intensity = aIntensity * iMultiplier;
                 
            switch ( effectType )                               
                {
                case CHWRMHaptics::EHWRMHapticsTypePeriodic:
                    {
                    CHWRMHaptics::THWRMHapticsPeriodicEffect periodicDef;

                    iHaptics->GetPeriodicEffectDefinition( iIVTHandle,
                                                           iEffectIndex,
                                                           periodicDef );
                                                              
                    periodicDef.iMagnitude = intensity + periodicDef.iAttackLevel;
                    
                    iHaptics->ModifyPlayingPeriodicEffect( iEffectHandle, 
                                                           periodicDef );            
                    }
                    break;
                case CHWRMHaptics::EHWRMHapticsTypeMagSweep:
                    {
                    CHWRMHaptics::THWRMHapticsMagSweepEffect magSweepDef;

                    iHaptics->GetMagSweepEffectDefinition( iIVTHandle,
                                                           iEffectIndex,
                                                           magSweepDef );
                                                              
                    magSweepDef.iMagnitude = intensity + magSweepDef.iAttackLevel;
                    
                    iHaptics->ModifyPlayingMagSweepEffect( iEffectHandle, 
                                                           magSweepDef );
                    
                    }
                    break;            
                default:
                    ret = KErrArgument;    
                    break;
                }            
            }             
        }    
   
    TRACE("CTactileHapticsPlayer::ModifyFeedback - End");     
    return ret;                        
    }
    
// ---------------------------------------------------------------------------
// Stop feedback.
// ---------------------------------------------------------------------------
//    
void CTactileHapticsPlayer::StopFeedback()
    {
    TRACE("CTactileHapticsPlayer::StopFeedback - Start");
    iHaptics->StopAllPlayingEffects();
    iEffectHandle = 0;    
    TRACE("CTactileHapticsPlayer::StopFeedback - End");
    }    

// ---------------------------------------------------------------------------
// Check if there's a change in haptics settings and set new values if needed.
// ---------------------------------------------------------------------------
//  
void CTactileHapticsPlayer::DoHandleNotifyGenericL( TUint32 aId )
    {
    TRACE("CTactileHapticsPlayer::DoHandleNotifyGenericL - Begin");
    switch ( aId )
        {
        case KTactileFeedbackHapticsStrength:
            {
            // Strength value in settings is multiplied by 100 to scale value 
            // suitable for haptics.
            TInt strength(0);
            iRepository.Get( KTactileFeedbackHapticsStrength, strength );
            
            iStrength = strength * 100;
            iHaptics->SetDeviceProperty( CHWRMHaptics::EHWRMHapticsStrength, 
                                         iStrength );        
            }
            break;
        case KTactileHapticsLevel1IVTFile:
        case KTactileHapticsLevel2IVTFile:
        case KTactileHapticsLevel3IVTFile:
        case KTactileHapticsLevel4IVTFile:
        case KTactileHapticsLevel5IVTFile:
            {
            TFileName ivtFile;
            iRepository.Get( aId, ivtFile );
            
            HBufC8* ivtBuf = IVTBufAllocL( ivtFile );
            if ( ivtBuf )
                {
                TInt ret = iHaptics->DeleteEffectData( iIVTHandle );
                if ( ret == KErrNone )
                    {
                    iHaptics->LoadEffectData( *ivtBuf, iIVTHandle );
                    }
                delete ivtBuf;
                }
            }
            break;            
        default:
            break;
        }    
    TRACE("CTactileHapticsPlayer::DoHandleNotifyGenericL - End");
    }
    
// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//                              
TInt CTactileHapticsPlayer::PlayPreviewFeedback( TInt aLevel, 
                                            TTouchLogicalFeedback aFeedback )
    {
    TRACE("CTactileHapticsPlayer::PlayPreviewFeedback - Begin");
    // Set preview level (scaled to haptics - multiplied by 100).
    TInt ret = iHaptics->SetDeviceProperty( CHWRMHaptics::EHWRMHapticsStrength, 
                                 aLevel * 100 );
    if ( ret == KErrNone )
        {
        ret = PlayFeedback( aFeedback );    
        }
        
    iHaptics->SetDeviceProperty( CHWRMHaptics::EHWRMHapticsStrength, 
                                 iStrength );
                                 
    TRACE("CTactileHapticsPlayer::PlayPreviewFeedback - End");
    return ret;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//                              
TInt CTactileHapticsPlayer::StartPreviewFeedback( TInt aLevel,
                           TTouchContinuousFeedback aFeedback,
                           TInt aIntensity )
    {
    TRACE("CTactileHapticsPlayer::StartPreviewFeedback - Begin");
    // Set preview level (scaled to haptics - multiplied by 100).
    TInt ret = iHaptics->SetDeviceProperty( CHWRMHaptics::EHWRMHapticsStrength, 
                                            aLevel * 100 );
    if ( ret == KErrNone )
        {
        ret = StartFeedback( aFeedback, aIntensity );
        }
    TRACE("CTactileHapticsPlayer::StartPreviewFeedback - End");
    return ret;
    }
 
// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//                              
void CTactileHapticsPlayer::StopPreviewFeedback()
    {
    TRACE("CTactileHapticsPlayer::StopPreviewFeedback - Begin");
    StopFeedback();
    
    iHaptics->SetDeviceProperty( CHWRMHaptics::EHWRMHapticsStrength, 
                                 iStrength );
    TRACE("CTactileHapticsPlayer::StopPreviewFeedback - Begin");
    }

// ---------------------------------------------------------------------------
// From MCenRepNotifyHandlerCallback.
// ---------------------------------------------------------------------------
//    
void CTactileHapticsPlayer::HandleNotifyGeneric( TUint32 aId )
    {
    TRACE("CTactileHapticsPlayer::HandleNotifyGeneric - Begin");
    TRAP_IGNORE( DoHandleNotifyGenericL( aId ) );
    TRACE("CTactileHapticsPlayer::HandleNotifyGeneric - End");
    }

// ---------------------------------------------------------------------------
// From MProfileChangeObserver.
// ---------------------------------------------------------------------------
//
void CTactileHapticsPlayer::HandleActiveProfileEventL( 
    TProfileEvent /*aProfileEvent*/, 
    TInt /*aProfileId*/ )
    {
    TInt oldVibraLevel = iVibraLevel;
    InitializeProfilesEngineL();
    iHaptics->SetDeviceProperty( CHWRMHaptics::EHWRMHapticsStrength, iStrength );
    if ( iVibraLevel != oldVibraLevel &&
         iVibraLevel != EProfileTactileFeedbackOff )
        {
        HandleNotifyGeneric( IVTFileId() );
        }
    }

// ---------------------------------------------------------------------------
// Profiles engine -related initializations
// ---------------------------------------------------------------------------
//
void CTactileHapticsPlayer::InitializeProfilesEngineL()
    {
    // Create profiles engine, if it does not yet exist
    if ( !iProfileEng )
        {
        iProfileEng = CreateProfileEngineL();        
        }
        
    MProfile* activeProfile  = iProfileEng->ActiveProfileL();
 
    const MProfileExtraSettings& extraSettings = 
     activeProfile->ProfileExtraSettings();
    
    const MProfileFeedbackSettings& feedbackSettings = 
     extraSettings.ProfileFeedbackSettings();
 
    iVibraLevel = feedbackSettings.TactileFeedback();
    iStrength = 100 * ( EProfileTactileFeedbackOff != iVibraLevel ? 100 : 0 );
 
    activeProfile->Release();
    
    // Create listener for profiles changes, if it does not yet exist
    if ( !iProfileHandler ) 
        {
        iProfileHandler = CProfileChangeNotifyHandler::NewL( this );
        }
    }

//---------------------------------------------------------------------------
// ImplementationTable[]
//
//---------------------------------------------------------------------------
//
const TImplementationProxy ImplementationTable[] = 
    {
    IMPLEMENTATION_PROXY_ENTRY( 0x2001CB99, CTactileHapticsPlayer::NewL )
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
