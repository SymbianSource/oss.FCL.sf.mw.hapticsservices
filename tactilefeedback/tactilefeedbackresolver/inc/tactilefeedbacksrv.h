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
* Description:  Header of tactile feedback server.
* Part of:      Tactile Feedback.
*
*/


#ifndef C_TACTILEFEEDBACKSRV_H
#define C_TACTILEFEEDBACKSRV_H

#include <e32base.h>
#include <f32file.h>

#include "tactilefeedbackresolver.h"
#include "tactilefeedbackdefs.h"

// Total number of ranges
const TUint KTactileFeedbackRangeCount = 1;

// Definition of the ranges of IPC numbers
const TInt KTactileFeedbackRanges[KTactileFeedbackRangeCount] = 
    {
    0,
    }; 

// Policy to implement for each of the above ranges        
const TUint8 KTactileFeedbackElementsIndex[KTactileFeedbackRangeCount] = 
    {
    0,
    };

const CPolicyServer::TPolicyElement KTactileFeedbackPolicyElements[] = 
    {
    { _INIT_SECURITY_POLICY_C2(ECapability_None,
                               ECapability_None),
                               CPolicyServer::EFailClient }
    };

// Package all the above together into a policy
const CPolicyServer::TPolicy KTactileFeedbackPolicy =
    {
    CPolicyServer::EAlwaysPass,     // all attempts should pass
    KTactileFeedbackRangeCount,     // number of ranges
    KTactileFeedbackRanges,         // ranges array
    KTactileFeedbackElementsIndex,  // elements<->ranges index
    KTactileFeedbackPolicyElements, // array of elements
    };

/**
 *  Tactile feedback server.
 *
 *  @since S60 5.2
 */
class CTactileFeedbackSrv : public CPolicyServer
    {
public:
    /**
    * 2-phased constructor.
    *
    * @since S60 5.2
    * @return Instance of CTactileFeedbackSrv.
    */
    static CTactileFeedbackSrv* NewL();
    
    /**
    * C++ destructor.
    */ 
    virtual ~CTactileFeedbackSrv();
  
public:
// from base class CServer2.
    /**
    * From CServer2, creates a new session.
    */
    CSession2* NewSessionL( const TVersion& aVersion, 
                            const RMessage2& aMessage ) const;

    /**
     * Increase session count.
     *
     * @since S60 5.2
     */
    void AddSession();
    
    /**
     * Decrease session count, close server if count is zero.
     *
     * @since S60 5.2
     */
    void RemoveSession();
    
public:    
    /**
     * Play feedback.
     *
     * @since S60 5.2
     * @param aFeedback    - Logical feedback type.
     * @param aPlayHaptics - Boolean value, which indicates if haptics 
     *                       feedback is played.
     * @param aPlayAudio   - Boolean value, which indicates if audio 
     *                       feedback is played.
     */
    void PlayFeedback( TTouchLogicalFeedback aFeedback,
                       TBool aPlayHaptics,
                       TBool aPlayAudio );
    
    /**
     * Start continuous feedback.
     *
     * @since S60 5.2
     * @param aType        - Type of continuous feedback.
     * @param aIntensity   - Intensity of continuous feedback.
     */
    void StartFeedback( TTouchContinuousFeedback aType,
                        TInt aIntensity );

    /**
     * Modify continuous feedback.
     *
     * @since S60 5.2     
     * @param aIntensity   - Intensity of continuous feedback.
     */                        
    void ModifyFeedback( TInt aIntensity );
    
    /**
     * Stop continuous feedback.
     */    
    void StopFeedback();
    
    /**
     * Set feedback enabled/disabled for the device.
     *
     * @since S60 5.2     
     * @param aFeedbackType - Bitmask, which tells which feedback types
     *                        are enabled/disabled. 
     */    
    void EnableFeedbackForDevice(  TTouchFeedbackType aFeedbackType  );
    
    /**
     * Get a bitmask of feedback type, which indicates what feedback types 
     * are enabled/disabled in the device.
     *
     * @since S60 5.2
     * @return Bitmask, which contains currently enabled feedback types.
     */    
    TTouchFeedbackType FeedbackEnabledForDevice();
    
    /**
     * Play preview feedback.
     *
     * @since S60 5.2
     * @param aLevel    - Level value defines the strength of playing 
     *                    effect. Maximum level value is player dependent.
     * @param aFeedback - Logical feedback type.
     * @param aType     - Type of feedback to be previewed. 
     */    
    void PlayPreviewFeedback( TInt aLevel,
                               TTouchLogicalFeedback aFeedback,
                               TTouchFeedbackType aType );

    /**
     * Start preview of continuous feedback.
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
     * Stop preview of continuous feedback.
     *
     * @since S60 5.2
     */
    void StopPreviewFeedback();

private:    
    /**
    * C++ constructor.
    */
    CTactileFeedbackSrv();
    
    /**
    * 2nd phase constructor.
    */
    void ConstructL();

private: // data
    /**
     * Instance of tactile feedback resolver.
     * Own.
     */
    CTactileFeedbackResolver* iResolver;
    
    /**
     * Count of open sessions.
     */     
    TInt iSessionCount;
    
    /**
     * A bitmap, which contains information of currently enabled/disabled
     * feedback types.
     */
    TInt iDeviceFeedbackEnabled;
    };


#endif // C_TACTILEFEEDBACKSRV_H

// End of file.
