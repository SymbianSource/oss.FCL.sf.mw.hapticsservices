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

#ifndef C_TACTILETONEPLAYER_H
#define C_TACTILETONEPLAYER_H

#include <e32base.h>
#include <mdaaudiotoneplayer.h> 
#include <cenrepnotifyhandler.h>
#include <touchlogicalfeedback.h>
#include <MProfileChangeObserver.h>

#include "tactileplayer.h"

class CRepository;
class MProfileEngine;
class CProfileChangeNotifyHandler;
/**
 *  Class for procuding audio tone feedback based on logical
 *  feedback type.
 *
 *  @since S60 5.2
 */
NONSHARABLE_CLASS( CTactileTonePlayer ): public CTactilePlayer, 
                                         public MMdaAudioToneObserver,
                                         public MCenRepNotifyHandlerCallback,
                                         public MProfileChangeObserver
    {
    /**
     * Sound params for tone feedback. 
     */
    struct TTactileSoundParams
        {
        TInt iFrequency;
        TTimeIntervalMicroSeconds iDuration;
        TInt iVolume; // 0 = No physical feedback.
        };
        
    /**
     * State of tone playing.
     */
    enum TTactileToneState
        {
        ETactileTonePreparing,
        ETactileToneReady,
        ETactileTonePlaying,
        ETactileTonePlayPending,
        ETactileToneError
        };

public:

    static CTactileTonePlayer* NewL( CRepository& aRepository );


    virtual ~CTactileTonePlayer();

 // from base class CTactilePlayer
    /**
     * @see CTactilePlayer.
     */
    TInt PlayFeedback( TTouchLogicalFeedback aFeedback );
    
    /**
     * @see CTactilePlayer.
     */
    TInt PlayPreviewFeedback( TInt aLevel, TTouchLogicalFeedback aFeedback );
    
private: // From MMdaAudioToneObserver

    /**
     * From MMdaAudioToneObserver
     * Called when audio tone player utility has been configured.
     *
     * @param aError - KErrNone if operation was succesful. Otherwise one of
     *                 the standard Symbian OS error codes.
     */
    void MatoPrepareComplete( TInt aError );
    
    /**
     * From MMdaAudioToneObserver
     * Called when audio tone playing operation has completed.
     *
     * @param aError - KErrNone if operation was succesful. Otherwise one of
     *                 the standard Symbian OS error codes.
     */
    void MatoPlayComplete( TInt aError );

private: // From MCenRepNotifyHandlerCallback.

    /**
     * @see MCenRepNotifyHandlerCallback.
     */
    void HandleNotifyInt( TUint32 aId, TInt aNewValue );

private: // From MProfileChangeObserver
    /**
     * @see MProfileChangeObserver.
     */
    void HandleActiveProfileEventL( TProfileEvent aProfileEvent, 
                                    TInt aProfileId );

private:
    /**
     * Constructror.
     */
    CTactileTonePlayer( CRepository& aRepository );

    /**
     * 2nd phase constructror.
     */
    void ConstructL();
    
    /**
     * Read settings values.
     */    
    void ReadSettings();
    
    /**
     * Scales given volume percentage to 0 - CMdaAudioToneUtility::MaxVolume.
     *
     * @param aPercentages - Volume level in percentages
     * @return Volume level that can be set to audio tone player.
     */
    TInt ScaleVolume( TInt aPercentages );

    /**
     * Profiles engine -related initializations.
     * (User changeable settings are stored in profiles)
     */
    void InitializeProfilesEngineL();

private: // data

    /**
     * Current state of the player.
     */
    TTactileToneState iState;

    /**
     * Audio tone player utility used for producing sound feedback.
     * Own. 
     */
    CMdaAudioToneUtility* iMdaAudioToneUtility;
    
    /**
     * Physical feedback definitions
     */
    RArray<TTactileSoundParams> iSoundParams;
    
    /**
     * Currently loaded sound parameters (index is to iSoundParams array)
     */
    TInt iCurrentEffectIndex;

    /**
     * Reference to CenRep session
     * Not own
     */
    CRepository& iRepository;
    
    /**
     * Utility for listening to changes in configurable parameters.
     * Own.  
     */
    CCenRepNotifyHandler* iCenRepNotifier;    
    
    /**
     * Current audio volume level.
     */
    TInt iAudioLevel;

    /**
     * Original audio volume level is saved here in preview playing.
     */    
    TInt iOriginalLevel;
    
    /**
     * This flag indicates if preview is played. This is needed because original 
     * settings have to be returned after preview.
     */    
    TBool iPlayPreview;
    
    /**
     * Access to user changeable settings (stored into profiles)
     * Own.
     */
     MProfileEngine* iProfileEng;

    /**
     * For listening to changes in profiles.
     * Own.  
     */
    CProfileChangeNotifyHandler* iProfileHandler;
    };


#endif // C_TACTILETONEPLAYER_H
