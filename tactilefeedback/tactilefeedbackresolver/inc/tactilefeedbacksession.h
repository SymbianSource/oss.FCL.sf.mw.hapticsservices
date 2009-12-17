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
* Description:  Session class for Tactile Feedback server.
* Part of:      Tactile Feedback.
*
*/


#ifndef C_TACTILEFEEDBACKSESSION_H
#define C_TACTILEFEEDBACKSESSION_H

#include <e32base.h>

class CTactileFeedbackSrv;

/**
 *  Session class for Tactile Feedback server.
 *
 *  @since S60 5.2
 */
class CTactileFeedbackSession : public CSession2
    {
public:
    /**
     * C++ constructor.
     */
    CTactileFeedbackSession();
    
    /**
     * From CSession2, passes the request forward to DispatchMessageL.
     * @since S60 5.2
     * @param aMessage, a message from client.
     */
    void ServiceL( const RMessage2& aMessage );
    
    /**
     * Handles the request from client.
     * @since S60 5.2
     * @param aMessage, a message from client.
     */
    void DispatchMessageL( const RMessage2& aMessage );
    
    /**
     * From CSession2, adds session to server's counter.
     * @since S60 5.2 
     */
    void CreateL();    

private:
    /**
     * Play feedback.
     */
    void PlayFeedback( const RMessage2& aMessage );
    
    /**
     * Start continuous feedback.
     */    
    void StartFeedback( const RMessage2& aMessage );
    
    /**
     * Modify continuous feedback.
     */    
    void ModifyFeedback( const RMessage2& aMessage );
    
    /**
     * Stop continuous feedback.
     */    
    void StopFeedback( const RMessage2& aMessage );
    
    /**
     * Set feedback enabled/disabled for the device.
     */    
    void SetFBEnabledForDeviceL( const RMessage2& aMessage );
    
    /**
     * Get feedback enabled/disabled state.
     */    
    void FeedbackEnabledForDeviceL( const RMessage2& aMessage );
    
    /**
     * Play preview feedback.
     */    
    void PlayPreviewFeedback( const RMessage2& aMessage );
    
    /**
     * Start preview of continuous feedback.
     */    
    void StartPreviewFeedback( const RMessage2& aMessage );
    
    /**
     * Modify preview of continuous feedback.
     */    
    void ModifyPreviewFeedback( const RMessage2& aMessage );

    /**
     * Stop preview of continuous feedback.
     */
    void StopPreviewFeedback( const RMessage2& aMessage );

private:
    /**
    * C++ destructor.
    */
    virtual ~CTactileFeedbackSession(); 

    /*
    * Reference to Tactile Feedback Server.
    */
    CTactileFeedbackSrv& Server();
        
private:
    /**
     * This flag is used for making sure that continuous feedback 
     * is stopped if the client, which started it, is closed.
     */
    TBool iFeedbackStarted;
    };

#endif // C_TACTILEFEEDBACKSESSION_H

// End of file.
