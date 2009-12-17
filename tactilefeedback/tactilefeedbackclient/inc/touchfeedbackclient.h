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
* Description:  Communication from Client API to Tactile Area Registry.
* Part of:      Tactile Feedback.
*
*/



#ifndef C_TOUCHFEEDBACKCLIENT_H
#define C_TOUCHFEEDBACKCLIENT_H

#include <e32std.h>
#include <w32std.h>
#include <e32base.h>
#include <coemain.h>
#include <tactilefeedback.h>

#include "touchlogicalfeedback.h"

class CTouchFeedbackImpl;

/**
 *  The class for sharing area registry with the window server.
 *
 *  @lib touchfeedback.lib
 *  @since S60 v5.0
 */
NONSHARABLE_CLASS( CTouchFeedbackClient ): 
        public CBase
    {
public:
    /**
     * Instantiation method.
     *
     * @since S60 5.0
     * @return New CTouchFeedbackClient instance.
     */
    static CTouchFeedbackClient* NewL( CTouchFeedbackImpl& aFeedback );

    /**
     * Destructor.
     * @since S60 5.0
     */
    virtual ~CTouchFeedbackClient();
        
    /**
     * Should be called whenever registry has changed.
     *
     * Causes starting of a CIdle object, so that registry updates are
     * flushed to server side as soon as possible.
     * @since S60 5.0     
     */
    void RegistryChanged( );

    /**
     * Use to produce direct feedback immediately.
     *
     * @since S60 5.0     
     * @param aType    - The logical feedback type.
     * @param aVibraOn - If ETrue then vibra feedback shall be played
     *                   (if enabled in device).
     * @param aAudioOn - If ETrue then audio feedback shall be played
     *                   (if enabled in device).
     */
    void ImmediateFeedback( TTouchLogicalFeedback aType, 
                            TBool aVibraOn, 
                            TBool aAudioOn );

    /**
     * Use to flush registry updates immediately.
     * @since S60 5.0     
     */
    void FlushUpdates();
    
    /**
     * Starts continuous feedback or just restarts the timeout timer if 
     * feedback is already started. This function can also be used to
     * modify the intensity of ongoing continuous feedback if timeout 
     * and repeated start calls are used. 
     *
     * @since S60 5.2     
     * @param aClientHandle - Handle of a client. Handle is stored to 
     *                        prevent other clients to stop or modify 
     *                        playing feedback.
     * @param aType         - Type of continuous feedback.
     *                   
     * @param aIntensity    - Intensity of continuous feedback. Value
     *                        range is 0-100%.
     * @param aTimeout      - Timeout value, which is used to automatically 
     *                        stop continuous feedback if there's no new 
     *                        start call within the timeout time. 
     */    
    void StartFeedback( TUint32 aClientHandle,
                        TTouchContinuousFeedback aType,
                        TInt aIntensity,
                        TTimeIntervalMicroSeconds32 aTimeout );
                        
    /**
     * Modifies the intensity of continuous feedback.
     *
     * @since S60 5.2     
     * @param aClientHandle - Handle of a client to check if caller started
     *                        the playing continuous feedback. 
     * @param aIntensity    - New intensity of continuous feedback. Value
     *                        range is 0-100%.
     */                            
    void ModifyFeedback( TUint32 aClientHandle,
                         TInt aIntensity );
    
    /**
     * Stops the continuous feedback.
     *
     * @since S60 5.2     
     * @param aClientHandle - Handle of a client to check if caller started
     *                        the playing continuous feedback. 
     */                                
    void StopFeedback( TUint32 aClientHandle );  

    /**
     * Enables or disables audio or/and vibra feedback in whole device.
     * @param aFeedbackType  - Feedback types to be enabled/disabled 
     *                         defined as a bitmask combination of 
     *                         enumeration items from TTouchFeedbackType.
     *
     * @since S60 5.2     
     * @return KErrNone, or one of standard Symbian OS error codes if
     *         enabling/disabling fails.
     *         KErrPermissionDenied - ECapabilityWriteDeviceData required
     *                                for using this function.
     */    
    TInt SetFeedbackEnabledForDevice( TTouchFeedbackType aFeedbackType );  

    /**
     * Used to check enabled feedback types for the device.
     *
     * @since S60 5.2
     * @return TTouchFeedbackType containing enabled feedback types 
     *         as a bitmask.
     */    
    TTouchFeedbackType FeedbackEnabledForDevice();

private:

    /**
     * Start timeout timer for continuous feedback.
     */
    void StartFeedbackTimer( TTimeIntervalMicroSeconds32 aTimeout );
    
    /**
     * Stops continuous feedback and clears timeout timer and client handle.
     * Called by timeout callback function.
     */    
    void StopFeedbackByTimeout();
    
    /**
     * Timeout callback function.
     */
     static TInt StopFeedbackCallback( TAny* aThis );
    
private:

    /**
     * Default constructor.
     */
    CTouchFeedbackClient( CTouchFeedbackImpl& aFeedback );

    /**
     * 2nd phase constructor.
     */    
    void ConstructL();

    /**
     * Callback for CIdle. When this function is called, it is time to update
     * local registry content to shared memory chunk.
     *
     * Depending of whether feedback is actually enabled or disabled for
     * this application, either UpdateRegistryToChunkL or ClearWholeChunkL
     * -function shall be called.
     *
     * @param aPtr - Pointer to this class instance.
     */
    static TInt IdleCallbackL( TAny* aPtr );

    /**
     * Updates whole area registry content to shared chunk. Called from
     * IdleCallbackL when feedback is enabled for this application.
     */
    void UpdateRegistryToChunkL();

    /**
     * Clears whole shared chunk so that there are no feedback areas at
     * all. Called from IdleCallbackL when feedback is disabled for this
     * application.
     */
    void ClearWholeChunkL();

    /**
     * This is a cleanup operation that releases a semaphore in case
     * a leave occurs while we have called Wait on the semaphore. 
     * Failing to release the semaphore (by calling "Signal" -function
     * on it) would lead to deadlock in the system.
     * @param aPtr - Pointer to RSemaphore.
     */
    static void CleanupSemaphore( TAny* aPtr );

private: // data

    /**
     * Connection to window server click maker plugin.
     */
    RSoundPlugIn iClickPlugin;
    
    /**
     * Area registry in shared chunk between this process and
     * window server process.
     */
    RChunk iChunk;
    
    /**
     * Mutual exlusion with window server and other clients.
     */
    RSemaphore iSemaphore;
    
    /**
     * For knowing if we'll need to disconnect when this application
     * is closed.
     */    
    TBool iConnected;
    
    /**
     * For doing registry update as soon as possible, but not
     * immediately.
     * Own.  
     */
    CIdle* iIdle;
    
    /**
     * Access to local area registry
     * Not own.  
     */
    CTouchFeedbackImpl& iFeedback;
    
    /**
     * Client interface to tactile feedback server.
     * Own.
     */
    RTactileFeedback iFbClient;
    
    /**
     * Timer for continuous feedback timeout.
     * Own.
     */
    CPeriodic* iFeedbackTimer;
    
    /**
     * Client handle for checking continuous is not modified or stopped by
     * other clients that the one, which started it.
     */
    TUint32    iClientHandle;
    
    /**
     * Previous intensity value of an ongoing continuous feedback to prevent
     * unnecessary modify calls if there's no change in intensity. Also
     * new start call for ongoing feedback modifies the intensity if value
     * was changed.
     */
    TInt       iPreviousIntensity;
    };


#endif //  C_TOUCHFEEDBACKCLIENT_H
