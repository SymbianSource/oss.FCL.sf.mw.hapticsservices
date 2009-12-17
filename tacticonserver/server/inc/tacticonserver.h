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
* Description: Tacticon server implementation.
* Part of    : Tacticon Server
*/

#ifndef C_TACTICONSERVER_H
#define C_TACTICONSERVER_H

#include <e32base.h>
#include <f32file.h>

#include <tacticon.hrh>

#include "tacticonplayer.h"
#include "tacticonprivatecrkeys.h"
#include "tacticondefs.h"

class CRepository;
class CTacticonShutdown;

/** Total number of ranges */
const TUint KTacticonRangeCount = 1;

/** Definition of the ranges of IPC numbers */
const TInt KTacticonRanges[KTacticonRangeCount] = 
    {
    0,
    }; 

/** Policy to implement for each of the above ranges */       
const TUint8 KTacticonElementsIndex[KTacticonRangeCount] = 
    {
    0,
    };

const CPolicyServer::TPolicyElement KTacticonPolicyElements[] = 
    {
    _INIT_SECURITY_POLICY_C2(ECapability_None,
                               ECapability_None),
                               CPolicyServer::EFailClient
    };

/** Package all the above together into a policy */
const CPolicyServer::TPolicy KTacticonPolicy =
    {
    CPolicyServer::EAlwaysPass,     // all attempts should pass
    KTacticonRangeCount,     // number of ranges
    KTacticonRanges,         // ranges array
    KTacticonElementsIndex,  // elements<->ranges index
    KTacticonPolicyElements, // array of elements
    };
    
/**
*  A server class to initialize server. 
*  @lib tacticonserver.lib
*  @since S60 5.2
*/
class CTacticonServer : public CPolicyServer
    {
public:
    /**
    * 2-phased constructor.
    */
    static CTacticonServer* NewLC();
    
    /**
    * C++ destructor.
    */ 
    virtual ~CTacticonServer();
    
   
public:
    /**
    * From CServer2, creates a new session.
    * @since S60 5.2
    * @param aVersion, Tacticon server version
    * @param aMessage, a message from client     
    */
    CSession2* NewSessionL( const TVersion& aVersion,
                            const RMessage2& aMessage ) const;
                            
public:
    /**
     * Increase session count.
     * @since S60 5.2
     */
    void AddSession();
    
    /**
     * Decrease session count, close server if count is zero.
     * @since S60 5.2
     */
    void RemoveSession();  
    
public:
    /**
     * Plays tacticon according to aTacticonType
     * @since S60 5.2
     * @param aTacticonType, Tacticon type to be played
     */
    void PlayTacticon( TTacticonType aTacticonType ); 
    
    /**
     * Stops playing tacticon
     * @since S60 5.2
     */    
    void StopTacticon();                             

    /**
     * Sets tacticons enabled/disabled.
     * @since S60 5.2
     */    
    void EnableTacticons( TBool aEnabled );
    
    /**
     * Returns boolean value whether tacticons are enabled or disabled.
     * @since S60 5.2
     */        
    TBool TacticonsEnabled();

private:    
    /**
    * C++ constructor.
    */
    CTacticonServer();
    
    /**
    * 2nd phase constructor.
    */
    void ConstructL();

private:
    /**
     * Counter of opened client sessions.
     */
    TInt iSessionCount;

    /**
     * Player plugin. 
     * Own.
     */
    CTacticonPlayer* iPlayer;  
    
     /**
     * Access to settings.
     * Own.
     */    
    CRepository* iRepository;
  
    /**
     * Timer for server shutdown
     * Own.
     */
    CTacticonShutdown* iShutdown; 
    
    /**
     * 
     */    
    TBool   iTacticonsEnabled;

    };


#endif // C_TACTICONSERVER_H

