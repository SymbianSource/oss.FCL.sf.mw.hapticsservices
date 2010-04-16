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
* Description: Client interface to tacticon server
* Part of    : Tacticon Server
*/

#include <tacticon.h>
#include <featmgr.h>
#include "tacticontrace.h"
#include "tacticondefs.h"
#include "OstTraceDefinitions.h" 
#ifdef OST_TRACE_COMPILER_IN_USE 
#include "tacticonTraces.h"
#endif

//------------------------------------------------------------
// Starts the server
//------------------------------------------------------------
//
TInt StartServer()
    {
    TRACE("Tacticon client StartServer() - Begin");
    TInt res = KErrNone;

    // DLL launch
    RProcess server;
    res = server.Create( KTacticonServerName, KNullDesC );

    // Loading failed.
    if ( res != KErrNone )
        {
        return res;
        }
    
    TRequestStatus status;
    server.Rendezvous( status );

    if ( status != KRequestPending )
        {
        TRACE( "Tacticon server StartServer() - Rendezvous failed" );
        server.Kill( 0 );       // abort startup
        server.Close();
        return KErrGeneral;
        }
    else
        {
        TRACE( "Tacticon server StartServer() - Resume" );
        server.Resume();    // Logon OK - start the server.
        }
        
    User::WaitForRequest( status );
    server.Close();
    
    TRACE("Tacticon client StartServer() - End");
    return status.Int();
    }

// ---------------------------------------------------------
// Default constructor
// ---------------------------------------------------------
//
EXPORT_C RTacticon::RTacticon()
    {
    }

// ---------------------------------------------------------
// Creates connection to tacticon server
// ---------------------------------------------------------
//
EXPORT_C TInt RTacticon::Connect()
    { 
    TRACE("RTacticon::Connect() - Begin");
    // Check if Tacticon feature is supported or not
    TRAPD( ret, FeatureManager::InitializeLibL() );
    if ( KErrNone == ret )
        {
        TBool supported = 
            FeatureManager::FeatureSupported( KFeatureIdFfTacticons );
        FeatureManager::UnInitializeLib();    
    
        ret = KErrNotSupported;
        if ( supported )          
            {
            TRACE("RTacticon::Connect() - CreateSession");
            ret = CreateSession( KTacticonServerName, Version() );
            if ( ret != KErrNone )
                {
                ret = StartServer();
                if ( ret == KErrNone )
                    {
                    TRACE2("RTacticon::Connect() - StartServer(%d)", ret);
                    ret = CreateSession( KTacticonServerName, Version() );
                    }
               }
            }
        else
            {
            TRACE("RTacticon::Connect() - Feature not supported");
            }
        }
    else
        {
        TRACE2("RTacticon::Connect() - FeatureManager init leaves (%d)", ret );
        }    
    TRACE2("RTacticon::Connect() - End(%d)", ret);
    return ret;    
    }
    
// ---------------------------------------------------------
// Closes connection to tacticon server
// ---------------------------------------------------------
//      
EXPORT_C void RTacticon::Close()
    {   
    if ( iHandle )
        {
        RSessionBase::Close();
        }
    }
        
// --------------------------------------------------------------------------
// Plays tacticon
// --------------------------------------------------------------------------
//     
EXPORT_C void RTacticon::PlayTacticon( TTacticonType aTacticonType )
    {
    if ( iHandle )
        {
        TRACE("RTacticon::PlayTacticon - Begin");
        OstTrace0( TACTICON_PERFORMANCE, TACTICON_CLIENT_PLAY_TACTICON_1,
                   "e_TACTICON_CLIENT_PLAY_TACTICON 1" );
        SendReceive( EPlayTacticon, TIpcArgs( aTacticonType ) );   
        OstTrace0( TACTICON_PERFORMANCE, TACTICON_CLIENT_PLAY_TACTICON_0,
                   "e_TACTICON_CLIENT_PLAY_TACTICON 0" );
        TRACE("RTacticon::PlayTacticon - End");
        }
    }
    
// --------------------------------------------------------------------------
// Stops playing current tacticon
//
// --------------------------------------------------------------------------
//     
EXPORT_C void RTacticon::StopTacticon()
    {
    if ( iHandle )
        {
        TRACE("RTacticon::StopTacticon() - Begin");
        OstTrace0( TACTICON_PERFORMANCE, TACTICON_CLIENT_STOP_TACTICON_1,
                   "e_TACTICON_CLIENT_STOP_TACTICON 1" );
        SendReceive( EStopTacticon, TIpcArgs() );   
        OstTrace0( TACTICON_PERFORMANCE, TACTICON_CLIENT_STOP_TACTICON_0,
                   "e_TACTICON_CLIENT_STOP_TACTICON 0" );
        TRACE("RTacticon::StopTacticon() - End");
        }
    }   

// --------------------------------------------------------------------------
// Enable/disable tacticon playback.
//
// --------------------------------------------------------------------------
//     
EXPORT_C TInt RTacticon::EnableTacticons( TBool aEnabled )
    {
    TInt ret( KErrSessionClosed );
    if ( iHandle )
        {
        TRACE("RTacticon::EnableTacticons() - Begin");
        OstTrace0( TACTICON_PERFORMANCE, TACTICON_CLIENT_ENABLE_TACTICON_1,
                   "e_TACTICON_CLIENT_ENABLE_TACTICON 1" );

        ret = SendReceive( EEnableTacticons, TIpcArgs( aEnabled ) );

        OstTrace0( TACTICON_PERFORMANCE, TACTICON_CLIENT_ENABLE_TACTICON_0,
                   "e_TACTICON_CLIENT_ENABLE_TACTICON 0" );
        TRACE("RTacticon::EnableTacticons() - End");
        }
    return ret;
    }      

// --------------------------------------------------------------------------
// Returns boolean value whether tacticons are enabled or disabled.
//
// --------------------------------------------------------------------------
//     
EXPORT_C TBool RTacticon::TacticonsEnabled()
    {
    TBool enabled( EFalse );
    if ( iHandle )
        {
        TRACE("RTacticon::TacticonsEnabled() - Begin");
        TPckg<TBool> enabledBuf( enabled );
        SendReceive( ETacticonsEnabled, TIpcArgs( &enabledBuf ) );

        TRACE("RTacticon::TacticonsEnabled() - End");
        }
    return enabled;
    }   

// ---------------------------------------------------------
// Returns tacticon server version
// ---------------------------------------------------------
//      
TVersion RTacticon::Version() const
    {
    return TVersion( KTacticonServerMajor, 
                     KTacticonServerMinor,
                     KTacticonServerBuild );
    }


// End of File
