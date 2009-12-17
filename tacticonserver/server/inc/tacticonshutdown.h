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
* Description: Tacticon server shutdown timer.
* Part of    : Tacticon Server
*/

#ifndef C_TACTICONSHUTDOWN_H
#define C_TACTICONSHUTDOWN_H

#include <e32base.h>

/**
 *  Tacticon server shutdown timer.
 */
class CTacticonShutdown : public CTimer
    {
public:
    
    /**
     * Two phased constructor.
     *
     * @return Created instance of the shutdown timer.
     */ 
    static CTacticonShutdown* NewL();

    /**
     * Destructor.
     */ 
    virtual ~CTacticonShutdown();
    
    /**
     * Starts the timer.
     */ 
    void Start();

private:

    /**
     * C++ constructor.
     */ 
    CTacticonShutdown();
    
    /**
     * Second phase construction.
     */ 
    void ConstructL();
    
    /**
     * From base class CActive.
     * @see CActive.
     * 
     * Executed when the timer expires; Shuts down
     * the server by closing the active scheduler.
     */ 
    void RunL();
    };

#endif // C_TACTICONSHUTDOWN_H
            
// End of File
