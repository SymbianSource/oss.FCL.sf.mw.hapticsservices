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


#ifndef C_TACTILEFEEDBACKRESOLVER_H
#define C_TACTILEFEEDBACKRESOLVER_H

#include <e32base.h>
#include <touchlogicalfeedback.h>
#include <cenrepnotifyhandler.h>

#include "tactilepropertyobserver.h"

class CRepository;
class CTactilePlayer;
class CTactilePropertyWatcher;

/**
 *  Class for procuding physical feedback based on logical
 *  feedback type.
 *
 *  Feedback is generated according to Tactile Feedback settings and
 *  current device state, and it may be that feedback is not even generated
 *  at all.
 *
 *  @lib tactilefeedbackresolver.lib
 *  @since S60 5.2
  */
class CTactileFeedbackResolver: public CBase,
                                public MTactilePropertyObserver,
                                public MCenRepNotifyHandlerCallback
    {
public:

    static CTactileFeedbackResolver* NewL();

    virtual ~CTactileFeedbackResolver();

    /**
     * Produces immediately physical feedback according to given
     * logical feedback type.
     *
     * Actual physical feedback type depends on settings of existing player 
     * plugins and current device state.
     *
     * @param aFeedback  - Logical feedback type.
     * @param aPlayVibra - If EFalse then vibra feedback is not played.
     * @param aPlayAudio - If EFalse then audio feedback is not played.
     */
    void PlayFeedback( TTouchLogicalFeedback aFeedback,
                       TBool aPlayVibra,
                       TBool aPlayAudio );

    /**
     * Starts playing continuous feedback.
     *
     * @since S60 5.2
     * @param aType         - Type of continuous feedback to be played.
     * @param aIntensity    - Intensity of continuous feedback. Value
     *                        range for intensity is 0-100%.
     */ 
    void StartFeedback( TTouchContinuousFeedback aType,
                        TInt aIntensity );
                        
    /**
     * Modifies the intensity of ongoing continuous feedback.
     * @since S60 5.2
     * @param aIntensity    - New intensity for continuous feedback. Value
     *                        range for intensity is 0-100%. 
     */                         
    void ModifyFeedback( TInt aIntensity );
    
    /**
     * Stops the ongoing continuous feedback.
     *
     * @since S60 5.2
     */    
    void StopFeedback();
    
    /**
     * Produces immediate physical feedback according to given
     * logical feedback type and given level. 
     *
     * @since S60 5.2
     * @param aLevel        - Level value defines the strength of playing 
     *                        effect. Maximum level value is player dependent.
     * @param aFeedbackType - Logical feedback type.
     * @param aType         - Type of feedback to be previewed. 
     */    
    void PlayPreviewFeedback( TInt aLevel,
                              TTouchLogicalFeedback aFeedback,
                              TTouchFeedbackType aType );

    /**
     * Starts playing continuous feedback of given type on given level.
     *
     * @since S60 5.2
     * @param aLevel        - Level value defines the strength of playing 
     *                        effect. Maximum level value is player dependent.     
     * @param aFeedback     - Type of continuous feedback to be played.
     * @param aIntensity    - Intensity of continuous feedback. Value
     *                        range for intensity is 0-100% from basic
     *                        strength (aLevel).
     * @param aType         - Type of feedback to be previewed.
     */    
    void StartPreviewFeedback( TInt aLevel,
                               TTouchContinuousFeedback aFeedback,
                               TInt aIntensity,
                               TTouchFeedbackType aType );

    /**
     * Stops the ongoing preview of continuous feedback.
     *
     * @since S60 5.2
     */
    void StopPreviewFeedback();    

private:
    /**
     * Constructor.
     */
    CTactileFeedbackResolver();

    /**
     * 2nd phase constructor..
     */
    void ConstructL();

    /**
     * Central repository related initializations.
     */
    void InitializeCrKeysL();

    /**
     * Publish & Subscribe related initializations.
     */
     void InitializePsKeysL();
     
    /**
     * This function reads the feedback enabled/disabled value from
     * the dedicated P&S key, and stores it into iFeedbackEnabled
     * -member variable.
     *
     * Dedicated fuction used for avoiding duplicate code (in
     * construction and when the P&S key changes)
     */
     void ReadFeedbackEnabledFromPS();
     
// from base class MTactilePropertyObserver

    /**
     * From MTactilePropertyObserver
     *
     * This is called when the property for temporary disabling feedback
     * is updated.
     *
     * @param aCategory - The UID of the changed property
     * @param aKey      - The key (i.e. identifier) of the changed property
     */
    void PropertyChangedL( const TUid aCategory, 
                           const TUint aKey );
                           
    
//from base class MCenRepNotifyHandlerCallback.

    /**
     * From MCenRepNotifyHandlerCallback.
     *
     * This is called when Tactile Feedback settings in Central Repository
     * are changed.
     *
     * @param aId       - The id of the changed Central Repository value.
     */        
    void HandleNotifyGeneric( TUint32 aId );
        
private: 
    /**
     * Handle changes in tactile feedback settings.
     */    
    void DoHandleNotifyGenericL( TUint32 aId );
    
    /**
     * Create haptics player. ECom implementation uid is used for selecting
     * the player plugin. 
     */    
    void CreateHapticsPlayerL( TUid aHapticsUid );
    
    /**
     * Create audio player. ECom implementation uid is used for selecting
     * the player plugin.
     */    
    void CreateAudioPlayerL( TUid aAudioUid );

    /**
     * Return ETrue if aFeedback's priority is higher than current playing
     * feedback, namely iLastFeedback.
     *
     * TODO: Currently, we only take Popup and Button effects into
     * account. Ultimately, a complete priority queue should be set up.
     */
    TBool IsHigherThanPlaying ( TTouchLogicalFeedback aFeedback ) const;
    
private: // data

    /**
     * Access to the P&S property, which is used for temporary disabling
     * the feedback.
     * Own.  
     */
    CTactilePropertyWatcher* iPropertyWatcher;
    
    /**
     * Haptics player.
     * Own.  
     */
    CTactilePlayer*       iHapticsPlayer;
    
    /**
     * Audio player.
     * Own.  
     */    
    CTactilePlayer*       iAudioPlayer;
    
    /**
     * Central repository for reading tactile feedback settings.
     * Own.
     */
    CRepository*          iRepository;
    
    /**
     * Central repository for reading profile settings.
     * Own.
     */
    CRepository*          iProfileRepository;
    
    /**
     * Central repository notifier, which sends notification when tactile 
     * feedback settings has been changed.
     * Own.
     */
    CCenRepNotifyHandler* iCenRepNotifier; 
    
    /**
     * This flag indicates the vibra feedback state according to the P&S key
     * that can be used for globally disabling vibra and/or audio
     * feedback.  
     */    
    TBool iVibraEnabled;
    
    /**
     * This flag indicates the audio feedback state according to the P&S key
     * that can be used for globally disabling vibra and/or audio
     * feedback.  
     */    
    TBool iAudioEnabled;
    
    /**
     * This flag indicates whether the continuous feedback is started or not.
     * 
     */
    TBool iFeedbackStarted;
    
    /**
     * Minimum interval between feedbacks.
     * 
     */
    TTimeIntervalMicroSeconds iMinimumInterval;
    
    /**
     * Timestamp of the last played feedback.
     * 
     */    
    TTime iLastFeedbackTimeStamp;
    
    /**
     * This flag determinates the playing order of feedback types.
     * ETrue  - vibra feedback is played first.
     * EFalse - audio feedback is played first.
     */
    TBool iVibraFirst;

    /**
     * Name of the last played feedback.
     * 
     */
    TTouchLogicalFeedback iLastFeedback;
    };


#endif // C_TACTILEFEEDBACKPLAYER_H
