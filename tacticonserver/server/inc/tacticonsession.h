/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description: Tacticon server session implementation.
* Part of    : Tacticon Server
*/

#ifndef C_TACTICONSESSION_H
#define C_TACTICONSESSION_H

// INCLUDES
#include <e32base.h>

//  FORWARD DECLARATIONS
class CTacticonServer;

/**
*  A session class of Tacticon server.
*
*  @since S60 5.2
*/

class CTacticonSession : public CSession2
    {
public:
    /**
    * C++ constructor.
    */
    CTacticonSession();
    
    /**
     * Passes the request forward to DispatchMessageL. 
     * @param aMessage, a message from client.
     */
    void ServiceL( const RMessage2& aMessage );
    
    /**
     * Handles the request from client. 
     * @param aMessage, a message from client.
     */
    void DispatchMessageL( const RMessage2& aMessage );
    
    /**
     * Adds session to server's counter. 
     */
    void CreateL(); 

private:
    /**
     * Sets tacticons enabled/disabled.
     */    
    void EnableTacticonsL( const RMessage2& aMessage );
        
    /**
     * Returns boolean value whether tacticons are enabled or disabled.
     */           
    void TacticonsEnabledL( const RMessage2& aMessage );

private:

    /**
    * C++ destructor.
    */
    virtual ~CTacticonSession(); 

    /*
    * Reference to Tacticon Server. Ownership is not transferred.
    */
    CTacticonServer& Server();      

    };

#endif // C_TACTICONSESSION_H

