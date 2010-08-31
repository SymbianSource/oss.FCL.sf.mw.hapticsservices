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
* Description:  Class for producing audio feedback.
* Part of:      Tactile Feedback.
*
*/


#include <e32debug.h>

#include <centralrepository.h>
#include <ecom/implementationproxy.h>

#include <AudioPreference.h>
#include <mda/common/audio.h>
#include <mdaaudiooutputstream.h>

#include "tactilefeedbackprivatecrkeys.h"
#include "tactilefeedbacktrace.h"

#include "tactileaudioplayer.h"
#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "tactileaudioplayerTraces.h"
#endif

const TInt KRiffHeaderSize = 44;
_LIT8( KRiff, "RIFF" );
_LIT8( KWave, "WAVE" );
_LIT8( KFmt,  "fmt " );
_LIT8( KData, "data" );

// ---------------------------------------------------------------------------
// Constructor.
// ---------------------------------------------------------------------------
//
CTactileAudioPlayer::CTactileAudioPlayer( CRepository& aRepository ):
    iRepository( aRepository )
    {
    }

// ---------------------------------------------------------------------------
// 2nd phase constructor.
// ---------------------------------------------------------------------------
//
void CTactileAudioPlayer::ConstructL()
    {
    TRACE("CTactileAudioPlayer::ConstructL()");
    
    ReadSettingsL();

    CreateWavPlayerL();

    iCenRepNotifier = CCenRepNotifyHandler::NewL( *this, 
                                                  iRepository, 
                                                  CCenRepNotifyHandler::EIntKey,
                                                  KTactileFeedbackAudioVolume );
    iCenRepNotifier->StartListeningL(); 

    TRACE("CTactileAudioPlayer::ConstructL() - end");
    }

// ---------------------------------------------------------------------------
// 2-phased constructor.
// ---------------------------------------------------------------------------
//
CTactileAudioPlayer* CTactileAudioPlayer::NewL( CRepository& aRepository )
    {
    CTactileAudioPlayer* self = 
        new ( ELeave ) CTactileAudioPlayer( aRepository );
    CleanupStack::PushL( self );
    self->ConstructL( );
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// Destructor.
// ---------------------------------------------------------------------------
//
CTactileAudioPlayer::~CTactileAudioPlayer()
    {
    if ( iAudioPlayer )
        {
        iAudioPlayer->Stop();
        delete iAudioPlayer;
        }
    
    delete iCenRepNotifier;
    delete iSensitiveSample;
    delete iBasicSample;
    delete iSensitiveFileName;
    delete iBasicFileName;
    
    iVolumeLevels.Close();
    }


// ---------------------------------------------------------------------------
// From class CTactilePlayer
//
// Currently we only select volume level according to logical feedback type
// (i.e. the .wav -file cannot be selected separately for each logical
// feedback type).
//
// We don't do anything in case volume level has been set to zero for the
// given feedback type (this way e.g. dragging feedback can be disabled
// totally if that is wanted).
//
// Notice that currently the ETouchFeedbackSensitive -feedback is a bit
// troublesome: Even though we have our own thread for feedback playing,
// it still lags behind when something is dragged on screen. Final solution
// may be to configure audio .wav dragging feedback OFF, and use audio
// tone for dragging instead.
// ---------------------------------------------------------------------------
//
TInt CTactileAudioPlayer::PlayFeedback( TTouchLogicalFeedback aFeedback )
    {
    TRACE2( "CTactileAudioPlayer::PlayFeedback( %d )", aFeedback );
    TInt volumeIndex(0);
    
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
            volumeIndex = 0;
            break;
        case ETouchFeedbackSensitive:           // flow through
        case ETouchFeedbackSensitiveButton:     // flow through
        case ETouchFeedbackSensitiveItem:      
        case ETouchFeedbackSensitiveKeypad:
        case ETouchFeedbackSensitiveSlider:
            volumeIndex = 1;
            break;
        default:
            // should not be there at all
            TRACE2( "CTactileAudioPlayer::PlayFeedback - %d is not a feedback type - returning", aFeedback );
            break;
        }
    
    
    if ( volumeIndex <= iVolumeLevels.Count() &&
         iVolumeLevels[volumeIndex] > 0 &&
         iAudioPlayer )
        {
        switch ( iState )
            {
            case ETactileAudioInitialising:
                // can't play anything yet
                break;
            case ETactileAudioPlaying: // fall trough
            case ETactileAudioReady:
                OstTrace1( TACTILE_PERFORMANCE, TACTILE_PLAY_AUDIO_FEEDBACK_1,
                                "e_TACTILE_PLAY_AUDIO_FEEDBACK 1 0x%x", aFeedback );

                if ( aFeedback == ETouchFeedbackBasic )
                    {
                    TRAP_IGNORE( iAudioPlayer->WriteL( *iBasicSample ) );
                    }
                else
                    {
                    TRAP_IGNORE( iAudioPlayer->WriteL( *iSensitiveSample ) );
                    }                    
                OstTrace1( TACTILE_PERFORMANCE, TACTILE_PLAY_AUDIO_FEEDBACK_0,
                                "e_TACTILE_PLAY_AUDIO_FEEDBACK 0 0x%x", aFeedback );
                iState = ETactileAudioPlaying;
                break;
            case ETactileAudioError: 
                break;
            default:
                // should not be here
                TRACE("CTactileAudioPlayer::PlayFeedback - error with audio device");
                break;
                    
            }

        }
    else 
        {
        if ( !iAudioPlayer )
            {
            TRACE("CTactileAudioPlayer::PlayFeedback - iAudioPlayer is NULL!");
            }
        else
            {
            TRACE("CTactileAudioPlayer::PlayFeedback - volume levels not ok");
            }
        }
    TRACE2( "CTactileAudioPlayer::PlayFeedback( %d ) - end", aFeedback );
    return KErrNone;
    }
    
// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//    
TInt CTactileAudioPlayer::PlayPreviewFeedback( TInt aLevel, 
                                             TTouchLogicalFeedback aFeedback )    
    {
    TRACE("CTactileAudioPlayer::PlayPreviewFeedback - Begin");
    TInt ret( KErrArgument );
    
    iOriginalVolume = iCurrentVolume;
    iCurrentVolume = aLevel;
    iAudioPlayer->SetVolume( ScaledVolume() );
    ret = PlayFeedback( aFeedback );
    iCurrentVolume = iOriginalVolume;
    iAudioPlayer->SetVolume( ScaledVolume() );
    
    TRACE("CTactileAudioPlayer::PlayPreviewFeedback - End");
    return ret;    
    }    

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CTactileAudioPlayer::MaoscOpenComplete( TInt aError )
    {
    if ( aError == KErrNone ) 
        {
        iCurrentVolume = iVolumeLevels[0];
        iMaxRawVolume = iAudioPlayer->MaxVolume();
        iRepository.Get( KTactileFeedbackAudioVolume, iCurrentVolume );
        iAudioPlayer->SetVolume( ScaledVolume() );
        TRAP_IGNORE( iAudioPlayer->SetDataTypeL( KMMFFourCCCodePCM16 ) );
        iState = ETactileAudioReady;        
        }
    else 
        {
        TRACE2("CTactileAudioPlayer::MaoscOpenComplete( %d ) failed, will not play feedback", aError );
        delete iAudioPlayer;
        iAudioPlayer = NULL;
        
        iState = ETactileAudioError;
        }
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CTactileAudioPlayer::MaoscPlayComplete( TInt aError )
    {
    TRACE("CTactileAudioPlayer::MaoscPlayComplete - Begin");
    iAudioPlayer->Stop();
    if ( aError == KErrCorrupt )
        {
        TRACE2("CTactileAudioPlayer::MaoscPlayComplete( %d ) failed, will not play feedback", aError );
        delete iAudioPlayer;
        iAudioPlayer = NULL;
        
        iState = ETactileAudioError;        
        }
    else
        {   
        iState = ETactileAudioReady;
        }            
    TRACE("CTactileAudioPlayer::MaoscPlayComplete - End");
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//    
void CTactileAudioPlayer::MaoscBufferCopied( TInt aError, const TDesC8& /*aBuffer*/ )
    {
    if ( !aError )
        {
        iState = ETactileAudioReady; // may write another sample
        }
    else
        {
        TRACE2("CTactileAudioPlayer::MaoscPlayComplete( %d ) failed", aError);
        }
    }

// ---------------------------------------------------------------------------
// Settings are now not read completely, as only the .wav -file of basic
// feedback is read, and same file is used for sensitive feedback. 
// Using of separate files for basic and sensitive feedback 
// can be added later in case necessary.
// ---------------------------------------------------------------------------
//
void CTactileAudioPlayer::ReadSettingsL()
    {
    TRACE("CTactileAudioPlayer::ReadSettingsL - Begin");
    
    iVolumeLevels.Reset();
    
    delete iSensitiveFileName;
    iSensitiveFileName = NULL;

    delete iBasicFileName;
    iBasicFileName = NULL;

    iSensitiveFileName = HBufC::NewL( KMaxFileName );    
    TPtr sensitiveFileName = iSensitiveFileName->Des();

    iBasicFileName = HBufC::NewL( KMaxFileName );    
    TPtr basicFileName = iBasicFileName->Des();

    TInt basicVolumeLevel = 0;
    TInt sensitiveVolumeLevel = 0;
    
    // Read volume values for level 2
    iRepository.Get( KTactileAudioWavVolumeBasicLevel2, basicVolumeLevel );

    
    iRepository.Get( KTactileAudioWavVolumeSensitiveLevel2, sensitiveVolumeLevel );

    // Read file names
    iRepository.Get( KTactileAudioWavFileBasicLevel2, basicFileName );

    iRepository.Get( KTactileAudioWavFileSensitiveLevel2, sensitiveFileName );
    iVolumeLevels.AppendL( basicVolumeLevel );
    iVolumeLevels.AppendL( sensitiveVolumeLevel );

    TRACE("CTactileAudioPlayer::ReadSettingsL() - End");
    }


// ---------------------------------------------------------------------------
// ScaledVolume()
// return 100% scaled & sanity checked volume value
// ---------------------------------------------------------------------------
//
TInt CTactileAudioPlayer::ScaledVolume()
    {
    if ( iMaxRawVolume == KErrNotFound )
        {
        TRACE( "CTactileAudioPlayer::ScaledVolume() Audio Hardware is not initialized properly" );
        return 0;
        }
    
    TInt volume = iMaxRawVolume * iCurrentVolume / 100;

    if ( volume > iMaxRawVolume )
        {
        // sanity check, we might get anything from cenrep
        volume = iMaxRawVolume;
        }

    TRACE4( "CTactileAudioPlayer::ScaledVolume() iCurrentVolume:%d scales volume to:%d out of iMaxRawVolume:%d",
        iCurrentVolume, volume, iMaxRawVolume );
        
    return volume;
    }

/*
* Wave file structure: Riff header + Wave format chunk + Wave data chunk
* 
* Riff header:
* ==================================================
* Offset Size Description      Value
* 0x00      4 Chunk ID         "RIFF"
* 0x04      4 Chunk Data Size  ( file size ) - 8
* 0x08      4 RIFF Type        "WAVE"
* 0x0c      *  Wave chunks *
*
* Wave Format Chunk:
* ==================================================
* Offset Size Description      Value
* 0x00      4 Chunk ID         "fmt "
* 0x04      4 Chunk Data Size  16 + extra format bytes ( 0 for normal wav files)
* 0x08      2 Compression code 1...65535               ( 1 for PCM uncompressed)
* 0x0a      2 Nbr of channels  1...65535
* 0x0c      4 Sample rate      1...0xFFFFFFFF
* 0x10      4 Average bytes per sec
* 0x14      2 block align
* 0x16      2 siginificant bits per sample
* 0x18      2 extra format bytes 0...65535
* 0x1a        * extra format bytes, if any *
*
* Wave Data Chunk:
* ==================================================
* Offset Size Description      Value
* 0x00      4 Chunk ID         "data"
* 0x04      4 Chunk data size  
* 0x08        * sample data *
*
*/
void CTactileAudioPlayer::ReadSampleL( RFile& aFile, 
                                       HBufC8*& aDes, 
                                       TUint& aChannels, 
                                       TUint& aSampleRate )
    {
    TRACE("CTactileAudioPlayer::ReadSampleL - Start");
    const TInt fmtOffset  = 0x0c;
    const TInt dataOffset = fmtOffset + 0x18;
    
    TBuf8<KRiffHeaderSize> header;
    TInt err = aFile.Read( header, KRiffHeaderSize );
    if ( err )
        {
        TRACE("CTactileAudioPlayer::ReadSampleL: reading from file failed, aborting");
        User::Leave( err );
        }

    TPtr8 p = header.LeftTPtr( 4 );
    
    if ( p.Compare( KRiff ) )
        {
        TRACE("CTactileAudioPlayer::ReadSampleL: no RIFF header found, aborting" );
        User::Leave( KErrCorrupt );
        }

    p = header.MidTPtr( 0x08, 4 );
    if ( p.Compare( KWave ) )
        {
        TRACE("CTactileAudioPlayer::ReadSampleL:  not a WAVE file, aborting" );
        User::Leave( KErrCorrupt );
        }

    p = header.MidTPtr( fmtOffset + 0x00, 4 );
    if ( p.Compare( KFmt ) )
        {
        TRACE("CTactileAudioPlayer::ReadSampleL:  no 'fmt ' chunk found, aborting" );
        User::Leave( KErrCorrupt );
        }

    p = header.MidTPtr( dataOffset, 4 );
    if ( p.Compare( KData ) )
        {
        TRACE("CTactileAudioPlayer::ReadSampleL:  no 'data' chunk found, aborting" );
        User::Leave( KErrCorrupt );
        }

    TUint8 lo = header[ fmtOffset + 0x08 ];
    TUint8 hi = header[ fmtOffset + 0x08 + 1 ];
    if ( !( lo == 1 && hi == 0) )
        {
        TRACE("CTactileAudioPlayer::ReadSampleL: non PCM wav not supported, aborting" );
        User::Leave( KErrNotSupported );
        }

    lo = header[ fmtOffset + 0x0a ];
    hi = header[ fmtOffset + 0x0a + 1 ];
    aChannels = lo;
    if ( !(aChannels == 1 || aChannels == 2 && hi == 0) )
        {
        TRACE2("CTactileAudioPlayer::ReadSampleL: unsupported number of channels ( %d ), aborting", aChannels );
        User::Leave( KErrNotSupported );
        }

    aSampleRate = 0;
    for ( TInt i = 0; i < 4; i++ )
        {
        lo = header[ fmtOffset + 0x0c + i ];
        TUint32 tmp = lo;
        tmp = tmp << i * 8;

        aSampleRate = aSampleRate | tmp;
        }

    lo = header[ fmtOffset + 0x16 ];
    hi = header[ fmtOffset + 0x16 + 1 ];
    TUint16 bitsPerSample = hi;
    bitsPerSample = bitsPerSample << 8;
    bitsPerSample = bitsPerSample | lo;

    if ( bitsPerSample != 16 )
        {
        TRACE2("CTactileAudioPlayer::ReadSampleL:  %d bits per sample not supported", bitsPerSample );
        User::Leave( KErrNotSupported );
        }

    TUint32 bytesPerSample = bitsPerSample / 8;
    
    // how many bytes for 6 ms
    TUint bytesNeeded = ( aSampleRate * aChannels * bytesPerSample * 6 ) / 1000;

    TInt fsize( 0 );
    if ( aFile.Size( fsize ) == KErrNone && fsize >= bytesNeeded + KRiffHeaderSize )
        {
        aDes = HBufC8::NewL( bytesNeeded );
        TPtr8 des = aDes->Des();
        aFile.Read( des, bytesNeeded );
        }
    else
        {
        TRACE("CTactileAudioPlayer::ReadSampleL: Less than 6ms content in file, aborting" );
        User::Leave( KErrNotSupported );
        }

    TRACE3("CTactileAudioPlayer::ReadSampleL %dHz %dchannel sample read successfully - End", 
        iSampleRate, iChannels);
    }

// ---------------------------------------------------------------------------
// Audio data reading from file and creation of the actual player utility.
//
// Rest of the initializations are done in MapcInitComplete -function.
//
// Notice that CMdaAudioOutputStream usage is not a good idea, as it
// buffers samples until buffer is full and flushes all in a row. Result is 
// no feedback first, then a machine gun burst at some completely 
// unrelated moment. 
// ---------------------------------------------------------------------------
//
void CTactileAudioPlayer::CreateWavPlayerL()
    {
    TRACE("CTactileAudioPlayer::CreateWavPlayerL - Start");
    iState = ETactileAudioInitialising;
    delete iBasicSample;
    iBasicSample = NULL;
    delete iSensitiveSample;
    iSensitiveSample = NULL;

    if ( !( iBasicFileName && iBasicFileName->Length() > 0  && 
            iSensitiveFileName && iSensitiveFileName->Length() > 0 ))
        {
        TRACE("CTactileAudioPlayer::CreateWavPlayerL filenames missing, aborting");
        User::Leave( KErrBadName );
        }

    RFs fs;
    User::LeaveIfError ( fs.Connect() ); 
    CleanupClosePushL( fs );
        
    RFile file;
    TInt err = file.Open( fs, *iBasicFileName, EFileRead );
    if ( err )
        {
        TRACE2("CTactileAudioPlayer::CreateWavPlayerL could not open %S, aborting", iBasicFileName );
        User::Leave( err );
        }

    CleanupClosePushL( file );
    ReadSampleL( file, iBasicSample, iChannels, iSampleRate );
    CleanupStack::PopAndDestroy(&file); 
    
    RFile file2; 
    err = file2.Open( fs, *iSensitiveFileName, EFileRead );
    if ( err )
        {
        TRACE2("CTactileAudioPlayer::CreateWavPlayerL could not open %S, aborting", iSensitiveFileName );
        User::Leave( err );
        }

    CleanupClosePushL( file2 );
    
    TUint sampleRate(0);
    TUint channels(0);
    ReadSampleL( file2, iSensitiveSample, channels, sampleRate );
    CleanupStack::PopAndDestroy( &file2 );  
    CleanupStack::PopAndDestroy( &fs );
    
    if ( channels != iChannels || sampleRate != iSampleRate )
        {
        TRACE("CTactileAudioPlayer::CreateWavPlayerL Sample rates and number of channels must be same for both files, aborting");
        User::Leave( KErrNotSupported );
        }
    
    TInt priority = KAudioPriorityKeyPressNonDTMFWithFeedback; 
    TMdaPriorityPreference pref = static_cast<TMdaPriorityPreference>(KAudioPrefKeyPressNonDTMFWithFeedback);
    
    iAudioPlayer = CMdaAudioOutputStream::NewL( *this ,priority, pref); 
        
    TMdaAudioDataSettings streamSettings;
    switch ( iChannels )
        {
        case 1:
            streamSettings.iChannels = TMdaAudioDataSettings::EChannelsMono;
            break;
        case 2:
            streamSettings.iChannels = TMdaAudioDataSettings::EChannelsStereo;
            TRACE("CTactileAudioPlayer::CreateWavPlayerL Warning: stereo wav for feedback");
            break;
        default:
            TRACE2("CTactileAudioPlayer::CreateWavPlayerL: unsupported number of channels %d, aborting", iChannels );
            User::Leave( KErrNotSupported );
            break;
        }
    
    // there must be easier way to do this...
    switch( iSampleRate )
        {
        case 48000:
            streamSettings.iSampleRate = TMdaAudioDataSettings::ESampleRate48000Hz;
            streamSettings.iCaps = TMdaAudioDataSettings::ESampleRate48000Hz;
            break;
        case 44100:
            TRACE2("CTactileAudioPlayer::CreateWavPlayerL: warning, samplerate %d causes performance problem", iSampleRate );
            streamSettings.iSampleRate = TMdaAudioDataSettings::ESampleRate44100Hz;
            streamSettings.iCaps = TMdaAudioDataSettings::ESampleRate44100Hz;
            break;
        case 32000:
            streamSettings.iSampleRate = TMdaAudioDataSettings::ESampleRate32000Hz;
            streamSettings.iCaps = TMdaAudioDataSettings::ESampleRate32000Hz;
            break;
        case 24000:
            streamSettings.iSampleRate = TMdaAudioDataSettings::ESampleRate24000Hz;
            streamSettings.iCaps = TMdaAudioDataSettings::ESampleRate24000Hz;
            break;
        case 16000:
            streamSettings.iSampleRate = TMdaAudioDataSettings::ESampleRate16000Hz;
            streamSettings.iCaps = TMdaAudioDataSettings::ESampleRate16000Hz;
            break;
        case 12000:
            streamSettings.iSampleRate = TMdaAudioDataSettings::ESampleRate12000Hz;
            streamSettings.iCaps = TMdaAudioDataSettings::ESampleRate12000Hz;
            break;
        case 11025:
            TRACE2("CTactileAudioPlayer::CreateWavPlayerL: warning, samplerate %d causes performance problem",iSampleRate );
            streamSettings.iSampleRate = TMdaAudioDataSettings::ESampleRate11025Hz;
            streamSettings.iCaps = TMdaAudioDataSettings::ESampleRate11025Hz;
            break;
        case 8000:
            streamSettings.iSampleRate = TMdaAudioDataSettings::ESampleRate8000Hz;
            streamSettings.iCaps = TMdaAudioDataSettings::ESampleRate8000Hz;
            break;
        default:
            TRACE2("CTactileAudioPlayer::CreateWavPlayerL: unsupported samplerate %d, aborting",iSampleRate );
            User::Leave( KErrNotSupported );
            break;
        }
    
    iAudioPlayer->Open( &streamSettings );

    TRACE("CTactileAudioPlayer::CreateWavPlayerL - End");
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//    
void CTactileAudioPlayer::HandleNotifyInt( TUint32 aId, TInt aNewValue )    
    {
    TRACE("CTactileTonePlayer::HandleNotifyInt - Begin");
    if ( aId == KTactileFeedbackAudioVolume )
        {
        iCurrentVolume = aNewValue;
        iAudioPlayer->SetVolume( ScaledVolume() );
        }
    TRACE("CTactileTonePlayer::HandleNotifyInt - End");
    }


//---------------------------------------------------------------------------
// ImplementationTable[]
//
//---------------------------------------------------------------------------
//
const TImplementationProxy ImplementationTable[] = 
    {
    IMPLEMENTATION_PROXY_ENTRY( 0x2002133A, CTactileAudioPlayer::NewL )
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
