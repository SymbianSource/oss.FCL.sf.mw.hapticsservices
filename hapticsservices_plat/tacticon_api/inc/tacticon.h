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
* Description: Client interface for using Tacticon server.
* Part of:     Tacticon Server
*/

#ifndef R_TACTICON_H
#define R_TACTICON_H

// INCLUDES
#include <e32base.h>
#include <tacticon.hrh>

/**
*  Client interface to tacticon server.
*  @lib tacticon.lib
*  @since S60 5.2
*/
class RTacticon : public RSessionBase
    {
public:

    /**
     * C++ default constructor.
     * @since S60 5.2   
     */
    IMPORT_C RTacticon();
    
    /**
     * Creates connection to tacticon server.
     * @since S60 5.2
     * @return KErrNone if successful, or one of standard Symbian OS
     *         error codes if creating connection fails.
     */
    IMPORT_C TInt Connect();
    
    /**
     * Closes connection to tacticon server.
     * @since S60 5.2
     */
    IMPORT_C void Close();
    
public:
    /**
     * Plays tacticon according to a given tacticon type.
     *
     * @since S60 5.2
     * @param aTacticonType - Logical tacticon type.       
     */
    IMPORT_C void PlayTacticon( TTacticonType aTacticonType );
    
    
    /**
     * Stops playing current tacticon
     *
     * @since S60 5.2
     */    
    IMPORT_C void StopTacticon();       
    
    /**
     * Sets tacticon playback enabled/disabled.
     * Client must have Write Device Data capability to use this function.
     * 
     * @since S60 5.2
     * @param aEnabled - ETrue sets tacticon playback enabled.
     *                 - EFalse sets tacticon playback disabled.
     * @return KErrNone if succesful, or one of standard Symbian OS
     *         error codes.
     *         KErrPermissionDenied if client does not have required 
     *         capability.
     */        
    IMPORT_C TInt EnableTacticons( TBool aEnabled );
    
    /**
     * Returns boolean value whether tacticons are enabled or disabled.
     * 
     * @since S60 5.2
     * @return ETrue if tacticons are enabled, EFalse if disabled.  
     */            
    IMPORT_C TBool TacticonsEnabled();

private:
    /*
     * @return Version of Tacticon Server.
     */
    TVersion Version() const;   

    };
    
#endif // R_TACTICON_H

//End of File
    