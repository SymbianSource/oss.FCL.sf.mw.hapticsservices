/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Class for producing audio tone feedback.
* Part of:      Tactile Feedback.
*
*/

#include <ecom/implementationproxy.h>
#include <centralrepository.h>
#include <AudioPreference.h>
#include <ProfileInternal.hrh>
#include <MProfileEngine.h>
#include <MProfile.h>
#include <MProfileExtraSettings.h>
#include <MProfileFeedbackSettings.h>
#include <CProfileChangeNotifyHandler.h>

#include "tactilefeedbackprivatecrkeys.h"
#include "tactilefeedbacktrace.h"

#include "tactiletoneplayer.h"
#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "tactiletoneplayerTraces.h"
#endif


// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
//  Constructor.
// ---------------------------------------------------------------------------
//
CTactileTonePlayer::CTactileTonePlayer( CRepository& aRepository ):
    iRepository(aRepository)
    {
    }

// ---------------------------------------------------------------------------
// 2nd phase constructor.
// ---------------------------------------------------------------------------
//
void CTactileTonePlayer::ConstructL()
    {
    TRACE("CTactileTonePlayer::ConstructL - Begin");    
    iMdaAudioToneUtility = CMdaAudioToneUtility::NewL( 
        *this,
        NULL,
        KAudioPriorityKeyPress,
        TMdaPriorityPreference( KAudioPrefKeyPressNonDTMF ) );

    User::LeaveIfError( iRepository.Get( KTactileFeedbackAudioVolume, 
                                         iAudioLevel ) );

    // As a fix to EAKH-7LKANT, we're temporarily reading volume level
    // settings from profiles engine
    InitializeProfilesEngineL();
                                         
    if ( iAudioLevel > EProfileAudioFeedbackLevel5 )
        {
        User::Leave( KErrGeneral );
        }
  
    ReadSettings();
    
    iMdaAudioToneUtility->PrepareToPlayTone(
        iSoundParams[0].iFrequency,
        iSoundParams[0].iDuration );
        
    iCenRepNotifier = CCenRepNotifyHandler::NewL( *this, 
                                                  iRepository, 
                                                  CCenRepNotifyHandler::EIntKey,
                                                  KTactileFeedbackAudioVolume );
    iCenRepNotifier->StartListeningL();         
    TRACE("CTactileTonePlayer::ConstructL - End");        
    }

// ---------------------------------------------------------------------------
// 2-phased constructor.
// ---------------------------------------------------------------------------
//
CTactileTonePlayer* CTactileTonePlayer::NewL( CRepository& aRepository )
    {
    TRACE("CTactileTonePlayer::NewL - Begin");    
    CTactileTonePlayer* self = new ( ELeave ) CTactileTonePlayer(aRepository);
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    TRACE("CTactileTonePlayer::NewL - End");    
    return self;
    }

// ---------------------------------------------------------------------------
// Destructor.
// ---------------------------------------------------------------------------
//
CTactileTonePlayer::~CTactileTonePlayer()
    {
    if( iProfileEng )
        {
        iProfileEng->Release();
        iProfileEng = NULL;
        }
    delete iProfileHandler;
    delete iCenRepNotifier;
    delete iMdaAudioToneUtility;
    iSoundParams.Close();
    }


// ---------------------------------------------------------------------------
// From class CTactilePlayer
//
// Here we first check if we are ready to play, and then either play
// sound immediately (if loaded parameters are correct), or then load
// the proper parameters in case this feedback type is different than
// the previously played one.
// ---------------------------------------------------------------------------
//
TInt CTactileTonePlayer::PlayFeedback( TTouchLogicalFeedback aFeedback )
    {
    TRACE("CTactileTonePlayer::PlayFeedback- Begin");
    if ( iState == ETactileToneReady )
        {
        TInt effectIndex(0);
        
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
                                                    
        // Check that we have valid effect index    
        if ( effectIndex < iSoundParams.Count() &&
             iAudioLevel != EProfileAudioFeedbackOff )
            {
            if ( effectIndex == iCurrentEffectIndex )
                {
                // Play the sound immediately.
                OstTrace0( TACTILE_PERFORMANCE, TACTILE_PLAY_AUDIO_TONE_FEEDBACK_1, "e_TACTILE_PLAY_AUDIO_TONE_FEEDBACK 1");
                                
                iMdaAudioToneUtility->Play(); 

                OstTrace0( TACTILE_PERFORMANCE, TACTILE_PLAY_AUDIO_TONE_FEEDBACK_0, "e_TACTILE_PLAY_AUDIO_TONE_FEEDBACK 0");
        
                iState = ETactileTonePlaying;
                }
            else
                {      
                // Load new parameters.          
                iMdaAudioToneUtility->PrepareToPlayTone(
                    iSoundParams[effectIndex].iFrequency,
                    iSoundParams[effectIndex].iDuration );

                iCurrentEffectIndex = effectIndex;

                iState = ETactileTonePlayPending;
                }
            }
        }
    TRACE("CTactileTonePlayer::PlayFeedback- End");
    return KErrNone;
    }
    
// ---------------------------------------------------------------------------
// 
// 
// ---------------------------------------------------------------------------
//    
TInt CTactileTonePlayer::PlayPreviewFeedback( TInt aLevel, 
                                             TTouchLogicalFeedback aFeedback )
    {
    TRACE("CTactileTonePlayer::PlayPreviewFeedback - Begin");
    TInt ret( KErrArgument );
    
    if ( aLevel > EProfileAudioFeedbackOff  &&
         aLevel <= EProfileAudioFeedbackLevel5 )
        {
        iOriginalLevel = iAudioLevel;
        iAudioLevel = aLevel;
        ReadSettings();    
        iCurrentEffectIndex = KErrNotFound;
        iPlayPreview = ETrue;
        ret = PlayFeedback( aFeedback );
        }    
    
    TRACE("CTactileTonePlayer::PlayPreviewFeedback - End");
    return ret;
    }

// ---------------------------------------------------------------------------
// From class MMdaAudioToneObserver.
// 
// If initialization was succesfull, then we set the volume according
// to current effect, and after that either start to play (in case we
// have a pending play request), or just set ourselves to ready state.
//
// If initialization failed, then we set ourselves to error state. 
// In practice this means that no sounds will be played at all.
// ---------------------------------------------------------------------------
//
void CTactileTonePlayer::MatoPrepareComplete( TInt aError )
    {
    if ( aError == KErrNone )
        {      
        if ( iCurrentEffectIndex < iSoundParams.Count() )
            {
            iMdaAudioToneUtility->SetVolume( 
                    iSoundParams[iCurrentEffectIndex].iVolume ); 
            }
        if ( iState == ETactileTonePlayPending )
            {
            iMdaAudioToneUtility->Play(); 
            
            if ( iPlayPreview )
                {
                iPlayPreview = EFalse;
                iAudioLevel = iOriginalLevel;
                ReadSettings();    
                iCurrentEffectIndex = KErrNotFound;                
                }
            iState = ETactileTonePlaying;            
            }
        else
            {
            iState = ETactileToneReady;                    
            }        
        }
    else
        {
        iState = ETactileToneError;
        }    
    }


// ---------------------------------------------------------------------------
// From class MMdaAudioToneObserver.
// 
// Any errors are ignored, because they are most likely caused by
// override of higher priority client.
// ---------------------------------------------------------------------------
//
void CTactileTonePlayer::MatoPlayComplete( TInt /*aError*/ )
    {
    iState = ETactileToneReady;
    }


// ---------------------------------------------------------------------------
// Read all audio configuration options (frequency, duration and volume)
// for each logical feedback into array according to current feedback level, 
// so that they can conveniently be used when feedback needs to be played.
// ---------------------------------------------------------------------------
//
void CTactileTonePlayer::ReadSettings()
    {
    TRACE("CTactileTonePlayer::ReadSettingsL- Begin");    
    TTactileSoundParams basicParams;
    basicParams.iFrequency = 0;
    basicParams.iDuration   = TTimeIntervalMicroSeconds( 0 );
    basicParams.iVolume    = 0;
    
    TTactileSoundParams sensitiveParams = basicParams;
    
    TInt basicDuration = 0;
    TInt sensitiveDuration = 0;
    
    if ( iAudioLevel == EProfileAudioFeedbackLevel1 )
        {
        // Read settings for ETouchFeedbackBasic, level 1
        iRepository.Get( KTactileAudioToneFreqBasicLevel1,     
                         basicParams.iFrequency );
        iRepository.Get( KTactileAudioToneDurationBasicLevel1, 
                         basicDuration );
        iRepository.Get( KTactileAudioToneVolumeBasicLevel1,   
                         basicParams.iVolume );
        
        // Read settings for ETouchFeedbackSensitive, level 1
        iRepository.Get( KTactileAudioToneFreqSensitiveLevel1, 
                         sensitiveParams.iFrequency );
        iRepository.Get( KTactileAudioToneDurationSensitiveLevel1,  
                         sensitiveDuration );
        iRepository.Get( KTactileAudioToneVolumeSensitiveLevel1,  
                         sensitiveParams.iVolume );
        }
    else if ( iAudioLevel == EProfileAudioFeedbackLevel2  )
        {
        // Read settings for ETouchFeedbackBasic, level 2
        iRepository.Get( KTactileAudioToneFreqBasicLevel2,     
                         basicParams.iFrequency );
        iRepository.Get( KTactileAudioToneDurationBasicLevel2, 
                         basicDuration );
        iRepository.Get( KTactileAudioToneVolumeBasicLevel2,   
                         basicParams.iVolume );
        
        // Read settings for ETouchFeedbackSensitive, level 2
        iRepository.Get( KTactileAudioToneFreqSensitiveLevel2, 
                         sensitiveParams.iFrequency );
        iRepository.Get( KTactileAudioToneDurationSensitiveLevel2,  
                         sensitiveDuration );
        iRepository.Get( KTactileAudioToneVolumeSensitiveLevel2,  
                         sensitiveParams.iVolume );        
        }
    else if ( iAudioLevel == EProfileAudioFeedbackLevel3  )
        {
        // Read settings for ETouchFeedbackBasic, level 3
        iRepository.Get( KTactileAudioToneFreqBasicLevel3,     
                         basicParams.iFrequency );
        iRepository.Get( KTactileAudioToneDurationBasicLevel3, 
                         basicDuration );
        iRepository.Get( KTactileAudioToneVolumeBasicLevel3,   
                         basicParams.iVolume );
        
        // Read settings for ETouchFeedbackSensitive, level 3
        iRepository.Get( KTactileAudioToneFreqSensitiveLevel3, 
                         sensitiveParams.iFrequency );
        iRepository.Get( KTactileAudioToneDurationSensitiveLevel3,  
                         sensitiveDuration );
        iRepository.Get( KTactileAudioToneVolumeSensitiveLevel3,  
                         sensitiveParams.iVolume );        
        }
    else if ( iAudioLevel == EProfileAudioFeedbackLevel4  )
        {
        // Read settings for ETouchFeedbackBasic, level 4
        iRepository.Get( KTactileAudioToneFreqBasicLevel4,     
                         basicParams.iFrequency );
        iRepository.Get( KTactileAudioToneDurationBasicLevel4, 
                         basicDuration );
        iRepository.Get( KTactileAudioToneVolumeBasicLevel4,   
                         basicParams.iVolume );
        
        // Read settings for ETouchFeedbackSensitive, level 4
        iRepository.Get( KTactileAudioToneFreqSensitiveLevel4, 
                         sensitiveParams.iFrequency );
        iRepository.Get( KTactileAudioToneDurationSensitiveLevel4,  
                         sensitiveDuration );
        iRepository.Get( KTactileAudioToneVolumeSensitiveLevel4,  
                         sensitiveParams.iVolume );
        }
    else if ( iAudioLevel == EProfileAudioFeedbackLevel5  )
        {
        // Read settings for ETouchFeedbackBasic, level 5
        iRepository.Get( KTactileAudioToneFreqBasicLevel5,     
                         basicParams.iFrequency );
        iRepository.Get( KTactileAudioToneDurationBasicLevel5, 
                         basicDuration );
        iRepository.Get( KTactileAudioToneVolumeBasicLevel5,   
                         basicParams.iVolume );
        
        // Read settings for ETouchFeedbackSensitive, level 5
        iRepository.Get( KTactileAudioToneFreqSensitiveLevel5, 
                         sensitiveParams.iFrequency );
        iRepository.Get( KTactileAudioToneDurationSensitiveLevel5,  
                         sensitiveDuration );
        iRepository.Get( KTactileAudioToneVolumeSensitiveLevel5,  
                         sensitiveParams.iVolume );
        }
     
    // Clear old parameters. Only do it at end, so that we still have the
    // old settings in case something fails when reading these from 
    // Central Repository.
    iSoundParams.Reset();
    
    
    // Duration is in milliseconds in central repository
    // --> Convert to microseconds
    basicParams.iDuration = 
        TTimeIntervalMicroSeconds( basicDuration * 1000 );
    sensitiveParams.iDuration = 
        TTimeIntervalMicroSeconds( sensitiveDuration * 1000 );
        
    // Volume is in precentages in central repository
    // --> Convert to 0-CMdaAudioToneUtility::MaxVolume    
    basicParams.iVolume     = ScaleVolume( basicParams.iVolume );
    sensitiveParams.iVolume = ScaleVolume( sensitiveParams.iVolume );
    
    // Store parameters for different feedback types
    // if append fail just make its action like before
    TRAP_IGNORE( iSoundParams.AppendL( basicParams ) );
    TRAP_IGNORE( iSoundParams.AppendL( sensitiveParams ) );
    TRACE("CTactileTonePlayer::ReadSettingsL - End");        
    }

// ---------------------------------------------------------------------------
// Scale the volume, and perform sanity check.
// ---------------------------------------------------------------------------
//
TInt CTactileTonePlayer::ScaleVolume( TInt aPercentages )
    {
    TInt maxVolume = iMdaAudioToneUtility->MaxVolume();

    TInt scaledVolume = ( maxVolume * aPercentages ) / 100;
    
    if ( scaledVolume > maxVolume )
        {
        scaledVolume = maxVolume;
        }
    
    return scaledVolume;    
    }

// ---------------------------------------------------------------------------
// From MCenRepNotifyHandlerCallback.
// ---------------------------------------------------------------------------
//    
void CTactileTonePlayer::HandleNotifyInt( TUint32 aId, TInt aNewValue )    
    {
    TRACE("CTactileTonePlayer::HandleNotifyInt - Begin");
    if ( aId == KTactileFeedbackAudioVolume )
        {
        iAudioLevel = aNewValue;
        
        // No need to read settings if audio feedback is set off.
        if ( iAudioLevel != EProfileAudioFeedbackOff )
            {
            ReadSettings();    
            iCurrentEffectIndex = KErrNotFound;        
            }
        }
    TRACE("CTactileTonePlayer::HandleNotifyInt - End");
    }

// ---------------------------------------------------------------------------
// From MProfileChangeObserver.
// ---------------------------------------------------------------------------
//
void CTactileTonePlayer::HandleActiveProfileEventL( 
    TProfileEvent /*aProfileEvent*/, 
    TInt /*aProfileId*/ )
    {
    InitializeProfilesEngineL();
    ReadSettings();    
    iCurrentEffectIndex = KErrNotFound;
    }

// ---------------------------------------------------------------------------
// Profiles engine -related initializations
// ---------------------------------------------------------------------------
//
void CTactileTonePlayer::InitializeProfilesEngineL()
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
 
    iAudioLevel = feedbackSettings.AudioFeedback();
 
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
    IMPLEMENTATION_PROXY_ENTRY( 0x2001CB9D, CTactileTonePlayer::NewL )
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
