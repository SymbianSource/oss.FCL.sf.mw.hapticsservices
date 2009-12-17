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
* Description:  Class for producing audio feedback.
* Part of:      Tactile Feedback.
*
*/


#ifndef C_TACTILEAUDIOPLAYER_H
#define C_TACTILEAUDIOPLAYER_H


#include <e32base.h>
#include <cenrepnotifyhandler.h>
#include <mdaaudiooutputstream.h> 
#include <touchlogicalfeedback.h>

#include "tactileplayer.h"


/**
 *  Class for procuding audio feedback based on logical
 *  feedback type.
 *
 *  @lib tactileaudioplugin.lib
 *  @since S60 5.2
  */

NONSHARABLE_CLASS( CTactileAudioPlayer ): public CTactilePlayer,
                                          public MCenRepNotifyHandlerCallback,
                                          public MMdaAudioOutputStreamCallback
    {
public:
    /**  Possible states of this player */
    enum TTactileAudioWavState
        {
        ETactileAudioInitialising,
        ETactileAudioError,
        ETactileAudioReady,
        ETactileAudioPlaying
        };

    /**
     * 2-phased constructor.
     * @param aRepository - Reference to Central Repository.
     * @return Instance of CTactileAudioPlayer.
     */
    static CTactileAudioPlayer* NewL( CRepository& aRepository );

    /**
     * Destructor.
     */
    virtual ~CTactileAudioPlayer();

private:  // from base class CTactilePlayer
    /**
     * @see CTactilePlayer.
     */
    TInt PlayFeedback( TTouchLogicalFeedback aFeedback );
    
    /**
     * @see CTactilePlayer.
     */    
    TInt PlayPreviewFeedback( TInt aLevel, TTouchLogicalFeedback aFeedback );
    
private: // From MMdaAudioOutputStreamCallback.

    /**
     * @see MMdaAudioOutputStreamCallback.
     */    
    void MaoscOpenComplete( TInt aError );
    
    /**
     * @see MMdaAudioOutputStreamCallback.
     */    
    void MaoscBufferCopied( TInt aError, const TDesC8 &aBuffer );
    
    /**
     * @see MMdaAudioOutputStreamCallback.
     */    
    void MaoscPlayComplete( TInt aError );

private: // From MCenRepNotifyHandlerCallback.

    /**
     * @see MCenRepNotifyHandlerCallback.
     */
    void HandleNotifyInt( TUint32 aId, TInt aNewValue );

private:
    /**
     * Read new settings values.
     */   
    void ReadSettingsL();

    /**
     * Scale volume.
     */
    TInt ScaledVolume();

    /**
     * Creation of the actual player is handled inside this function. It
     * it assumed that current settings have been read to members 
     * iFileName and iVolumeLevels before calling this function.
     */
    void CreateWavPlayerL();

    /**
     * Read audio sample.
     */
    void ReadSampleL( RFile& aFile, HBufC8*& aDes, TUint& aChannels, TUint& aSampleRate );

private:
    /**
     * Constructor.
     */
    CTactileAudioPlayer( CRepository& aRepository );
    
    /**
     * 2nd phase constructor.
     */
    void ConstructL();
    

private: // data

    /**
    * Descriptor for wav file used for basic feedback.
    * Own.
    */
    HBufC8* iBasicSample;
    
    /**
    * Descriptor for wav file used for sensitive feedback.
    * Own.    
    */    
    HBufC8* iSensitiveSample;

    /**
    * Maximum raw volume value supported by audio hardware
    * interface. Stored to member variable to avoid MMFAudioClient:2
    * panic. Possibly even increases performance, if ever so slightly.
    */
    TInt iMaxRawVolume;
    
    /**
     * Audio channels.
     */ 
    TUint iChannels;
    
    /**
     * Audio sample rate..
     */     
    TUint iSampleRate;
    
    /**
     * Current state of this class
     */
    TTactileAudioWavState iState;

    /**
     * Access to audio player utility
     * Own.  
     */
    CMdaAudioOutputStream* iAudioPlayer;
    
    /**
     * Name of the .wav -file to be used as sensitive effect
     * Own.  
     */
    HBufC* iSensitiveFileName;
    
    /**
     * Name of the .wav -file to be used as basic effect
     * Own.  
     */    
    HBufC* iBasicFileName;
    
    /**
     * Current volume level
     */
    TInt iCurrentVolume;
    
    /**
     * Original volume is saved here in preview playing.
     */    
    TInt iOriginalVolume;
        
    /**
     * Volume levels for different logical feedbacks.
     */
    RArray<TInt> iVolumeLevels;
    
     /**
     * Access to settings.
     * Not own.
     */    
    CRepository& iRepository;
    
    /**
     * Utility for listening to changes in configurable parameters.
     * Own.  
     */
    CCenRepNotifyHandler* iCenRepNotifier;    
    
    };


#endif // C_TACTILEAUDIOPLAYER_H
