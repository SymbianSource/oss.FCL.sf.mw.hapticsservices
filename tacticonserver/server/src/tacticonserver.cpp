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

#include <e32debug.h>
#include <w32std.h>
#include <centralrepository.h>
#include <featmgr.h>
#include "tacticonserver.h"
#include "tacticonsession.h"
#include "tacticondefs.h"
#include "tacticonshutdown.h"
#include "tacticontrace.h"
#include "OstTraceDefinitions.h" 
#ifdef OST_TRACE_COMPILER_IN_USE 
#include "tacticonserverTraces.h"
#endif

// --------------------------------------------------------------------------
// RunServerL
// Initialize and run the server.
// --------------------------------------------------------------------------
//
static void RunServerL()
    {
    TRACE( "Tacticon server RunServer() - Begin" );
    User::LeaveIfError( RThread().RenameMe( KTacticonServerName ) );

    CActiveScheduler* scheduler = new (ELeave) CActiveScheduler;
    CleanupStack::PushL( scheduler );
    CActiveScheduler::Install( scheduler );

    CTacticonServer* server = CTacticonServer::NewLC();

    RProcess::Rendezvous( KErrNone );

    CActiveScheduler::Start();
    
    // Cleanup server and scheduler 
    CleanupStack::PopAndDestroy(server); 
    CleanupStack::PopAndDestroy(scheduler); 
    TRACE("Tacticon server RunServer() - End");
    }

// --------------------------------------------------------------------------
// E32Main
// Server process entry-point.
// --------------------------------------------------------------------------
//
TInt E32Main()
    {
    __UHEAP_MARK;   
    CTrapCleanup* cleanup = CTrapCleanup::New();
    TInt ret( KErrNoMemory );
    if( cleanup )
        {
        TRAP( ret, RunServerL() );
        delete cleanup;
        }
    __UHEAP_MARKEND;    
    return ret;
    }

// --------------------------------------------------------------------------
// CTacticonServer::NewL
// 2-phased constructor.
// --------------------------------------------------------------------------
//
CTacticonServer* CTacticonServer::NewLC()
    {
    TRACE( "CTacticonServer::NewLC - Begin" );
    CTacticonServer* self = new (ELeave) CTacticonServer();
    CleanupStack::PushL(self);
    self->ConstructL();
    TRACE("CTacticonServer::NewLC - End");
    return self;
    }

// --------------------------------------------------------------------------
// CTacticonServer::CTacticonServer()
// C++ constructor.
// --------------------------------------------------------------------------
//
CTacticonServer::CTacticonServer() 
    : CPolicyServer( EPriorityNormal, 
                     KTacticonPolicy, 
                     ESharableSessions ), 
      iTacticonsEnabled( ETrue )
    {
    }

// --------------------------------------------------------------------------
// CTacticonServer::ConstructL
// 2nd phase constructor.
// --------------------------------------------------------------------------
//
void CTacticonServer::ConstructL()
    {
    TRACE( "CTacticonServer::ConstructL - Begin" );
    StartL( KTacticonServerName);
    TRACE( "CTacticonServer::ConstructL - Server started" );
    iRepository = CRepository::NewL( KCRUidTacticon ); 
    
    TInt pluginUid(0);
    User::LeaveIfError( iRepository->Get( KTacticonPlugin, pluginUid ) );
    TRACE2( "CTacticonServer::ConstructL - got pluginUid: %d", pluginUid );
    
    TRACE("CTacticonServer::ConstructL - creating player");
    // The server is instantiated during boot time. To enable phone to boot up
    // even if player instatiation fails, trap its possible failure.
    TRAPD(err, iPlayer = CTacticonPlayer::NewL( TUid::Uid(pluginUid), *iRepository ) );
    if ( err )
        {
        TRACE2( "CTacticonServer::ConstructL - Creating Player failed with code %d", err );
        iPlayer = NULL; // to ensure that player pointer is not broken
        }        
        
    TRACE( "CTacticonServer::ConstructL - Creating shutdown instance" );
    iShutdown = CTacticonShutdown::NewL();
    TRACE( "CTacticonServer::ConstructL - End" );
    }

// --------------------------------------------------------------------------
// CTacticonServer::CTacticonServer()
// C++ destructor.
// --------------------------------------------------------------------------
//
CTacticonServer::~CTacticonServer()
    {
    delete iPlayer;
    delete iRepository; 
    delete iShutdown;
    REComSession::FinalClose();
    }

// --------------------------------------------------------------------------
// CTacticonServer::NewSessionL
// from CServer2, creates a new session.
// --------------------------------------------------------------------------
//
CSession2* CTacticonServer::NewSessionL( const TVersion& aVersion, 
                                             const RMessage2& /*aMessage*/ ) const
    {
    TRACE("CTacticonServer::NewSessionL - Begin");
    TVersion version( KTacticonServerMajor, 
                      KTacticonServerMinor, KTacticonServerBuild );
                      
    TBool supported = User::QueryVersionSupported( version, aVersion );
                                                                                                                                                                                                                                                                                                                            
    if( !supported )
        {
        TRACE("CTacticonServer::NewSessionL - Version not supported");
        User::Leave( KErrNotSupported );
        }
        
    CTacticonSession* session = new (ELeave) CTacticonSession(); 
    TRACE("CTacticonServer::NewSessionL - End");
    return session;
    }
    
// --------------------------------------------------------------------------
// CTacticonServer::AddSession
// --------------------------------------------------------------------------
//  
void CTacticonServer::AddSession()  
    {
    TRACE("CTacticonServer::AddSession - Begin");
    iSessionCount++;
    iShutdown->Cancel();
    TRACE("CTacticonServer::AddSession - End");
    }
    
// --------------------------------------------------------------------------
// CTacticonServer::RemoveSession
// --------------------------------------------------------------------------
//    
void CTacticonServer::RemoveSession()
    {
    TRACE( "CTacticonServer::RemoveSession - Begin" );
    iSessionCount--;
    
    // Start the shutdown timer there are no more sessions
    if ( !iSessionCount && !iShutdown->IsActive() )
        {
        iShutdown->Start();
        }       
    TRACE( "CTacticonServer::RemoveSession - End" );
    }

// --------------------------------------------------------------------------
// CTacticonServer::PlayTacticon
// --------------------------------------------------------------------------
//     
void CTacticonServer::PlayTacticon( TTacticonType aTacticonType )
    {
    if ( iPlayer && iTacticonsEnabled )
        {
        OstTrace0( TACTICON_PERFORMANCE, TACTICON_SERVER_PLAY_TACTICON_1,
                   "e_TACTICON_SERVER_PLAY_TACTICON 1" );

        iPlayer->PlayTacticon( aTacticonType );

        OstTrace0( TACTICON_PERFORMANCE, TACTICON_SERVER_PLAY_TACTICON_0,
                   "e_TACTICON_SERVER_PLAY_TACTICON 0" );
        }
    else
        {
        TRACE("CTacticonServer::PlayTacticon - Player not available");
        }
    }

// --------------------------------------------------------------------------
// CTacticonServer::StopTacticon
// --------------------------------------------------------------------------
// 
void CTacticonServer::StopTacticon()
    {
    if ( iPlayer )
        {
        OstTrace0( TACTICON_PERFORMANCE, TACTICON_SERVER_STOP_TACTICON_1,
                   "e_TACTICON_SERVER_STOP_TACTICON 1" );

        iPlayer->StopTacticon();

        OstTrace0( TACTICON_PERFORMANCE, TACTICON_SERVER_STOP_TACTICON_0,
                   "e_TACTICON_SERVER_STOP_TACTICON 0" );
        }
    }
 
// --------------------------------------------------------------------------
// CTacticonServer::EnableTacticons
// --------------------------------------------------------------------------
//     
void CTacticonServer::EnableTacticons( TBool aEnabled )
    {
    iTacticonsEnabled = aEnabled;
    }
    
// --------------------------------------------------------------------------
// CTacticonServer::EnableTacticons
// --------------------------------------------------------------------------
//    
TBool CTacticonServer::TacticonsEnabled()
    {
    return iTacticonsEnabled;
    }

// End of file 
