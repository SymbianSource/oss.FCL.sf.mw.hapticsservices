/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  A plug-in for turning tactile feedback on/off.
* Part of:      Tactile Feedback.
*
*/


// INCLUDE FILES
#include <badesca.h>
#include <cfactionindication.h>
#include <e32property.h>
#include <centralrepository.h>

#include "tactileactionplugin.h"
#include "tactilefeedbackinternalpskeys.h"
#include "tactilefeedbackprivatecrkeys.h"
#include "tactilefeedbacktrace.h"

// CONSTANTS

// Constants for tactile feedback actions
_LIT( KActionId,         "Tactile" );
_LIT( KActionIdCommand,  "Feedback" );
_LIT( KActionCommandOn,  "ON" );
_LIT( KActionCommandOff, "OFF" );

// Constants used for setting Tactile Feedback while in-call and while
// no call is in progress. See tactilefeedbackinternalpskeys.h for 
// documentation.
const TInt KTactilePSValueWhileInCall = 0x00; // Vibra and audio disabled
const TInt KTactilePSValueWhileInCallAudioOff = 0x01; // Vibra enabled, audio disabled
const TInt KTactilePSValueWhileInCallVibraOff = 0x02; // Vibra disabled, audio enabled
const TInt KTactilePSValueWhileNoCall = 0x03; // Vibra and audio enabled

// Security policy
_LIT_SECURITY_POLICY_PASS( KSecurityPolicy );

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CTactileActionPlugIn::CTactileActionPlugIn
// C++ default constructor.
// -----------------------------------------------------------------------------
//
CTactileActionPlugIn::CTactileActionPlugIn( ) 
 : CCFActionPlugIn()                                  
    {
    }

// -----------------------------------------------------------------------------
// CTactileActionPlugIn::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CTactileActionPlugIn* CTactileActionPlugIn::NewL( )
    {
    CTactileActionPlugIn* self = CTactileActionPlugIn::NewLC();
    CleanupStack::Pop( self );
    return self;
    }
  
// -----------------------------------------------------------------------------
// CTactileActionPlugIn::NewLC
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CTactileActionPlugIn* CTactileActionPlugIn::NewLC( )
    {
    CTactileActionPlugIn* self = new( ELeave ) CTactileActionPlugIn();
    CleanupStack::PushL( self );
    return self;
    }
  

// -----------------------------------------------------------------------------
// CTactileActionPlugIn::~CTactileActionPlugIn
// Destructor.
// -----------------------------------------------------------------------------
//
CTactileActionPlugIn::~CTactileActionPlugIn()
    {
    }

//-----------------------------------------------------------------------------
// CTactileActionPlugIn::InitializeL
// Initializes the plugin, called by the CF framework.
//-----------------------------------------------------------------------------
//
void CTactileActionPlugIn::InitializeL()
    {
    TRACE("CTactileActionPlugIn::InitializeL(): start");
    CRepository* repository = CRepository::NewL( KCRUidTactileFeedback );
    CleanupStack::PushL( repository );
    
    TInt tactileSettings(0);
    repository->Get( KTactileInternalSettings, tactileSettings );
    
    // Clean all other settings values, leave only in-call settings.
    tactileSettings &= ( KTactileInternalSettingsCallDisableVibra | 
                         KTactileInternalSettingsCallDisableAudio );
    
    switch ( tactileSettings )                    
        {
        case KTactileInternalSettingsCallDisableVibra:
            iFeedbackInCall = KTactilePSValueWhileInCallVibraOff;
            break;
        case KTactileInternalSettingsCallDisableAudio:
            iFeedbackInCall = KTactilePSValueWhileInCallAudioOff;
            break;
        case ( KTactileInternalSettingsCallDisableVibra | 
               KTactileInternalSettingsCallDisableAudio ):
            iFeedbackInCall = KTactilePSValueWhileInCall;
            break;  
        default:
            iFeedbackInCall = KTactilePSValueWhileNoCall;
            break;
        }    
    CleanupStack::PopAndDestroy( repository );
    TRACE("CTactileActionPlugIn::InitializeL(): end");
    }

//-----------------------------------------------------------------------------
// CTactileActionPlugIn::ExecuteL
// Executes an action.
//-----------------------------------------------------------------------------
//
CCFActionPlugIn::TExecutionTime CTactileActionPlugIn::ExecuteL(
    CCFActionIndication* aActionIndication )
    {
    TRACE("CTactileActionPlugIn::ExecuteL(): start");
    
    // get action parameters
    const RKeyValueArray& parameters = aActionIndication->Parameters();
    
    for( TInt i = 0; i < parameters.Count(); i++ )
        {
        // get received parameter and its value
        TPtrC param( parameters[i]->Key() );
        TPtrC value( parameters[i]->Value() );

        // check command
        if ( param == KActionIdCommand )
            {
            TInt err = KErrNone;
            
            if ( value == KActionCommandOn )
                {
                // Turn tactile feedback on
                TRACE( "CTactileActionPlugIn::ExecuteL(): Feedback set ON");
                err = RProperty::Set( KPSUidTactileFeedback, 
                                      KTactileFeedbackEnabled, 
                                      KTactilePSValueWhileNoCall );
                }
            else if ( value == KActionCommandOff )
                {
                // Turn tactile feedback off
                TRACE("CTactileActionPlugIn::ExecuteL(): Feedback set OFF");
                err = RProperty::Set( KPSUidTactileFeedback, 
                                      KTactileFeedbackEnabled, 
                                      iFeedbackInCall );
                }
            
            TRACE2("CTactileActionPlugIn::ExecuteL(): Set returned %d", err);
            
            User::LeaveIfError( err );
            }
        }
        
    TRACE("CTactileActionPlugIn::ExecuteL(): end");
    
    // not async operation --> return always ENone
    return CCFActionPlugIn::ENone;
    }

//-----------------------------------------------------------------------------
// CTactileActionPlugIn::GetActionsL
// Returns all the actions the plugin can perform.
//-----------------------------------------------------------------------------
//
void CTactileActionPlugIn::GetActionsL( CDesCArray& aActionList ) const
    {
    TRACE("CTactileActionPlugIn::GetActionsL(): start");

    aActionList.AppendL( KActionId );

    TRACE("CTactileActionPlugIn::GetActionsL(): end");
    }

//-----------------------------------------------------------------------------
// CTactileActionPlugIn::SecurityPolicy
// Returns the capabilities required by the plugin.
//-----------------------------------------------------------------------------
//
const TSecurityPolicy& CTactileActionPlugIn::SecurityPolicy() const
    {
    TRACE("CTactileActionPlugIn::SecurityPolicy()");

    return KSecurityPolicy;
    }
    
// End of File
