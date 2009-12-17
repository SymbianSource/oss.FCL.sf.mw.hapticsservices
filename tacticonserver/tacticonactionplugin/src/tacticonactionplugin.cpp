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
* Description:  A plug-in for disabling/enabling tacticons.
* Part of:      Tacticon Server.
*
*/

// INCLUDE FILES
#include <badesca.h>
#include <cfactionindication.h>
#include <e32property.h>
#include <centralrepository.h>

#include "tacticonactionplugin.h"
#include "tacticonprivatecrkeys.h"
#include "tacticontrace.h"

// CONSTANTS

// Constants for tactile feedback actions

_LIT( KTacticonAction,    "Tacticon" );
_LIT( KTacticonMode,      "Mode" );

_LIT( KTacticonCallDialling,     "CALL_DIALLING" );
_LIT( KTacticonCallConnected,    "CALL_CONNECTED" );
_LIT( KTacticonCallDisconnected, "CALL_DISCONNECTED" );
_LIT( KProximityOn,              "PROXIMITY_ON" );
_LIT( KProximityOff,             "PROXIMITY_OFF" );

// Security policy
_LIT_SECURITY_POLICY_PASS( KSecurityPolicy );

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CTacticonActionPlugIn::CTacticonActionPlugIn
// C++ default constructor.
// -----------------------------------------------------------------------------
//
CTacticonActionPlugIn::CTacticonActionPlugIn( ) 
 : CCFActionPlugIn(),
    iProximityDiscernible(ETrue), iCallInitiated( EFalse )                                  
    {
    }

// -----------------------------------------------------------------------------
// CTacticonActionPlugIn::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CTacticonActionPlugIn* CTacticonActionPlugIn::NewL( )
    {
    CTacticonActionPlugIn* self = CTacticonActionPlugIn::NewLC();
    CleanupStack::Pop( self );
    return self;
    }
  
// -----------------------------------------------------------------------------
// CTacticonActionPlugIn::NewLC
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CTacticonActionPlugIn* CTacticonActionPlugIn::NewLC( )
    {
    CTacticonActionPlugIn* self = new( ELeave ) CTacticonActionPlugIn();
    CleanupStack::PushL( self );
    return self;
    }
  

// -----------------------------------------------------------------------------
// CTacticonActionPlugIn::~CTacticonActionPlugIn
// Destructor.
// -----------------------------------------------------------------------------
//
CTacticonActionPlugIn::~CTacticonActionPlugIn()
    {
    iTacticon.Close();
    }

//-----------------------------------------------------------------------------
// CTacticonActionPlugIn::InitializeL
// Initializes the plugin, called by the CF framework.
//-----------------------------------------------------------------------------
//
void CTacticonActionPlugIn::InitializeL()
    {
    TRACE("CTacticonActionPlugIn::InitializeL(): start");
    User::LeaveIfError( iTacticon.Connect() );
    
    CRepository* repository = CRepository::NewL( KCRUidTacticon );
    CleanupStack::PushL( repository );

    User::LeaveIfError( repository->Get( KTacticonDuringCall, 
                                         iEnabledDuringCall ) );

    User::LeaveIfError( repository->Get( KTacticonSet, 
                                         iTacticonCallSettings ) );
    CleanupStack::PopAndDestroy( repository );
    TRACE("CTacticonActionPlugIn::InitializeL(): end");
    }

//-----------------------------------------------------------------------------
// CTacticonActionPlugIn::ExecuteL
// Executes an action.
//-----------------------------------------------------------------------------
//
CCFActionPlugIn::TExecutionTime CTacticonActionPlugIn::ExecuteL(
    CCFActionIndication* aActionIndication )
    {
    TRACE("CTacticonActionPlugIn::ExecuteL(): start");
    
    // get action parameters
    const RKeyValueArray& parameters = aActionIndication->Parameters();
    
    for( TInt i = 0; i < parameters.Count(); i++ )
        {
        // get received parameter and its value
        TPtrC param( parameters[i]->Key() );
        TPtrC value( parameters[i]->Value() );
        // check command
        if ( param == KTacticonMode )
            {
            if ( value == KProximityOn )
                {
                TRACE("CTacticonActionPlugIn::ExecuteL(): Proximity ON");
                iProximityDiscernible = ETrue;
                }
            else if ( value == KProximityOff )
                {
                TRACE("CTacticonActionPlugIn::ExecuteL(): Proximity OFF");
                iProximityDiscernible = EFalse;
                }
            else if ( value == KTacticonCallDialling )
                {
                iCallInitiated = ETrue;
                }
            else if ( value == KTacticonCallConnected )
                {
                TRACE("CTacticonActionPlugIn::ExecuteL(): Call Connected");
                if ( iCallInitiated && 
                     !iProximityDiscernible &&
                   ( iTacticonCallSettings & KTacticonSetCallConnected ) )
                    {
                    iTacticon.PlayTacticon( ENeutralTacticon );
                    }                
                if ( !iEnabledDuringCall )
                    {
                    iTacticon.EnableTacticons( EFalse );
                    }
                }
            else if ( value == KTacticonCallDisconnected )
                {
                TRACE("CTacticonActionPlugIn::ExecuteL(): Call Disconnected");
                iTacticon.EnableTacticons( ETrue );
                iCallInitiated = EFalse;
                }                
            }            
        }
    TRACE("CTacticonActionPlugIn::ExecuteL(): end");
    
    // not async operation --> return always ENone
    return CCFActionPlugIn::ENone;
    }

//-----------------------------------------------------------------------------
// CTacticonActionPlugIn::GetActionsL
// Returns all the actions the plugin can perform.
//-----------------------------------------------------------------------------
//
void CTacticonActionPlugIn::GetActionsL( CDesCArray& aActionList ) const
    {
    TRACE("CTacticonActionPlugIn::GetActionsL(): start");

    aActionList.AppendL( KTacticonAction );

    TRACE("CTacticonActionPlugIn::GetActionsL(): end");
    }

//-----------------------------------------------------------------------------
// CTacticonActionPlugIn::SecurityPolicy
// Returns the capabilities required by the plugin.
//-----------------------------------------------------------------------------
//
const TSecurityPolicy& CTacticonActionPlugIn::SecurityPolicy() const
    {
    TRACE("CTacticonActionPlugIn::SecurityPolicy()");

    return KSecurityPolicy;
    }

// End of File
