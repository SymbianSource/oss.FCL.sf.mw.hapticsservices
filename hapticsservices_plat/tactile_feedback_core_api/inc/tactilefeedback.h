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
* Description:  Client interface to tactile feedback server.
* Part of:      Tactile Feedback.
*
*/


#ifndef R_TACTILEFEEDBACK_H
#define R_TACTILEFEEDBACK_H

#include <e32base.h>

#include <touchlogicalfeedback.h>

class CThreadPlayer;

/**
 *  Client interface to tactile feedback server.
 *
 *  @lib tactilefeedbackresolver.lib
 *  @since S60 5.2
  */
class RTactileFeedback : public RSessionBase
    {
public:

    /**
     * C++ default constructor.
     * @since S60 5.2
     */
    IMPORT_C RTactileFeedback();
    
    /**
     * Creates connection to tactile feedback server.
     * @since S60 5.2
     * @return KErrNone if successful, or one of standard Symbian OS
     *         error codes if creating connection fails.
     */
    IMPORT_C TInt Connect();
    
    /**
     * Closes connection to tactile feedback server.
     * @since S60 5.2 
     */
    IMPORT_C void Close();
    
public:
    /**
     * Produces immediately physical feedback according to given
     * logical feedback type.
     *
     * @since S60 5.2
     * @param aFeedbackType - Logical feedback type.
     * @param aPlayVibra    - ETrue to play vibra feedback, EFalse if vibra
     *                        feedback is disabled.
     * @param aPlayAudio    - ETrue to play audio feedback, EFalse if audio
     *                        feedback is disabled.        
     */
    IMPORT_C void PlayFeedback( TTouchLogicalFeedback aFeedbackType,
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
    IMPORT_C void StartFeedback( TTouchContinuousFeedback aType,
                                 TInt aIntensity );
                        
    /**
     * Modifies the intensity of ongoing continuous feedback.
     * @since S60 5.2
     * @param aIntensity    - New intensity for continuous feedback. Value
     *                        range for intensity is 0-100%. 
     */    
    IMPORT_C void ModifyFeedback( TInt aIntensity );
    
    /**
     * Stops the ongoing continuous feedback.
     *
     * @since S60 5.2
     */    
    IMPORT_C void StopFeedback();
    
    /**
     * Enables or disables audio or/and vibra feedback in whole device.
     *
     * @since S60 5.2
     * @param aFeedbackType  - Feedback types to be enabled/disabled 
     *                         defined as a bitmask combination of 
     *                         enumeration items from TTouchFeedbackType.
     * @return KErrNone, or one of standard Symbian OS error codes if
     *         enabling/disabling fails.
     *         KErrPermissionDenied - ECapabilityWriteDeviceData required
     *                                for using this function.
     */    
    IMPORT_C TInt SetFeedbackEnabledForDevice( TTouchFeedbackType aFeedbackType ); 
    
    /**
     * Used to check enabled feedback types for the device.
     * @since S60 5.2
     * @param aFeedbackEnabled - Enabled feedback types are returned to this
     *                           parameter as a bitmask.
     */    
    IMPORT_C void FeedbackEnabledForDevice( TTouchFeedbackType& aFeedbackEnabled );


public:
     /**
     * Produces immediate physical feedback according to given
     * logical feedback type and given level. 
     *
     * Maximum feedback level depends on used player and it is defined in 
     * Central Repository settings (KTactileFeedbackVibraLevels & 
     * KTactileFeedbackAudioLevels). 
     *
     * UID for tactile feedback settings is: 
     * KCRUidTactileFeedback = 0x2000B494;
     *
     * @since S60 5.2
     * @param aLevel        - Level value defines the strength of playing 
     *                        effect. Maximum level value is player dependent.
     * @param aFeedbackType - Logical feedback type.
     * @param aType         - Type of feedback to be previewed. 
     *                        Feedback type in preview mode must be either 
     *                        audio or haptics, not both.
     */
    IMPORT_C void PlayPreviewFeedback( TInt aLevel,
                                       TTouchLogicalFeedback aFeedback,
                                       TTouchFeedbackType aType );
    /**
     * Starts playing continuous feedback of given type on given level.
     *
     * Maximum feedback level depends on used player and it is defined in 
     * Central Repository settings (KTactileFeedbackVibraLevels & 
     * KTactileFeedbackAudioLevels). 
     *
     * UID for tactile feedback settings is: 
     * KCRUidTactileFeedback = 0x2000B494;
     *
     * @since S60 5.2
     * @param aLevel        - Level value defines the strength of playing 
     *                        effect. Maximum level value is player dependent.     
     * @param aFeedback     - Type of continuous feedback to be played.
     * @param aIntensity    - Intensity of continuous feedback. Value
     *                        range for intensity is 0-100% from basic
     *                        strength (see aLevel).
     * @param aType         - Type of feedback to be previewed.
     *                        Feedback type in preview mode must be either 
     *                        audio or haptics, not both.
     */    
    IMPORT_C void StartPreviewFeedback( TInt aLevel,
                                        TTouchContinuousFeedback aFeedback,
                                        TInt aIntensity,
                                        TTouchFeedbackType aType );
    /**
     * Modifies the intensity of ongoing preview of continuous feedback.
     *
     * @since S60 5.2
     * @param aIntensity    - New intensity for continuous feedback. Value
     *                        range for intensity is 0-100% from basic
     *                        strength. 
     */                                        
    IMPORT_C void ModifyPreviewFeedback( TInt aIntensity );

    /**
     * Stops the ongoing preview of continuous feedback.
     *
     * @since S60 5.2
     */      
    IMPORT_C void StopPreviewFeedback();
        

private:
    /**
     * Returns the version of Tactile Feedback Server.
     */
    TVersion Version() const;    
    
private:
    /**
     * Instance of a class, which creates a new thread for the 
     * tactile feedback server 
     * Own.
     */
    CThreadPlayer* iThread;    
    };

#endif // R_TACTILEFEEDBACK_H
