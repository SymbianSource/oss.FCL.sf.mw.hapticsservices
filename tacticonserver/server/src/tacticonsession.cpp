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

#include "tacticonsession.h"
#include "tacticonserver.h"
#include "tacticondefs.h"
#include "tacticontrace.h"

// --------------------------------------------------------------------------
// CTacticonSession::CTacticonSession
// C++ constructor.
// --------------------------------------------------------------------------
//
CTacticonSession::CTacticonSession() 
    {
    }

// --------------------------------------------------------------------------
// CTacticonSession::CTacticonSession
// C++ destructor.
// --------------------------------------------------------------------------
//
CTacticonSession::~CTacticonSession()
    {
    Server().RemoveSession();
    }

// --------------------------------------------------------------------------
// CTacticonSession::ServiceL
// From CSession2, passes the request forward to DispatchMessageL.
// --------------------------------------------------------------------------
//
void CTacticonSession::ServiceL( const RMessage2& aMessage )
    {
    TRACE( "CTacticonSession::ServiceL - Begin" );
    TRAPD( err,DispatchMessageL( aMessage ) );
    aMessage.Complete( err );
        
    TRACE( "CTacticonSession::ServiceL - End" );
    }

// --------------------------------------------------------------------------
// CTacticonSession::DispatchMessageL
// Handles the request from client.
// --------------------------------------------------------------------------
//
void CTacticonSession::DispatchMessageL( const RMessage2& aMessage )
    {
    TRACE( "CTacticonSession::DispatchMessageL - Begin" );
    switch( aMessage.Function() )
        {
        case EPlayTacticon:
            Server().PlayTacticon( ( TTacticonType )aMessage.Int0() );
            break;
        case EStopTacticon:            
            Server().StopTacticon();
            break;        
        case EEnableTacticons:
            EnableTacticonsL( aMessage );
            break;
        case ETacticonsEnabled:
            TacticonsEnabledL( aMessage );
            break;
        default:
            User::Leave( KErrArgument );
            break;
        }
    TRACE( "CTacticonSession::DispatchMessageL - End" );
    }

// --------------------------------------------------------------------------
// CTacticonSession::EnableTacticonsL
// Set tacticons enabled/disabled.
// ECapabilityWriteDeviceData capability is needed to use this function.
// --------------------------------------------------------------------------
//    
void CTacticonSession::EnableTacticonsL( const RMessage2& aMessage )
    {
    if ( aMessage.HasCapability( ECapabilityWriteDeviceData ) )
        {
        Server().EnableTacticons( aMessage.Int0() );
        }
    else
        {
        User::Leave( KErrPermissionDenied );
        }
    }
    
// --------------------------------------------------------------------------
// CTacticonSession::TacticonsEnabled
// Returns boolean value whether tacticons are enabled or disabled.
// --------------------------------------------------------------------------
//    
void CTacticonSession::TacticonsEnabledL( const RMessage2& aMessage )
    {
    TBool enabled = Server().TacticonsEnabled();
    TPckg<TBool> enabledBuf( enabled );    
    aMessage.WriteL( 0, enabledBuf );
    }
        
// --------------------------------------------------------------------------
// CTacticonSession::CreateL
// From CSession2, used to increase server's session count.
// --------------------------------------------------------------------------
//
void CTacticonSession::CreateL()
    {
    TRACE( "CTacticonSession::CreateL - Begin" );
    Server().AddSession();
    TRACE( "CTacticonSession::CreateL - End" );
    }    
    
// --------------------------------------------------------------------------
// CTacticonSession::Server
// Reference to server.
// --------------------------------------------------------------------------
//
CTacticonServer& CTacticonSession::Server()
    {   
    return *static_cast<CTacticonServer*>
           ( const_cast<CServer2*>( CSession2::Server() ) );
    }    
        

// End of file 
