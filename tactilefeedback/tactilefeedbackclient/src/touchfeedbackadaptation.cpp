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
* Description:  This class is an adaptation class that is instantiated
*                by the application framework, and that instantiates the
*                actual Tactile Feedback Client implementation.
* Part of:      Tactile Feedback.
*
*/

#include <e32std.h>
#include <eikenv.h>
#include <coecntrl.h>

#include <tactilefeedbacktrace.h>
#include <touchfeedbackadaptation.h>

#include "touchfeedbackclient.h"
#include "touchfeedbackimpl.h"

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// Constructor.
// ---------------------------------------------------------------------------
//
CTouchFeedbackAdaptation::CTouchFeedbackAdaptation()
    {
    }

// ---------------------------------------------------------------------------
// 2nd phase constructor.
// ---------------------------------------------------------------------------
//
void CTouchFeedbackAdaptation::ConstructL()
    {
    TRACE("CTouchFeedbackAdaptation::ConstructL - Begin");

    iTouchFeedback = CTouchFeedbackImpl::New();    

    TRACE("CTouchFeedbackAdaptation::ConstructL - End");
    }


// ---------------------------------------------------------------------------
// We store ourselves to the thread local storage here. The instance is 
// accessible to clients via Instance -function.
// ---------------------------------------------------------------------------
//
EXPORT_C CTouchFeedbackAdaptation* CTouchFeedbackAdaptation::NewL()
    {
    CTouchFeedbackAdaptation* self = new( ELeave ) CTouchFeedbackAdaptation;
    CleanupStack::PushL( self );
    self->ConstructL();

    // Store ourselves to thread local storage
    User::LeaveIfError( Dll::SetTls( self ) );        

    CleanupStack::Pop( self );    
    return self;
    }

// ---------------------------------------------------------------------------
// Destructor.
// ---------------------------------------------------------------------------
//
CTouchFeedbackAdaptation::~CTouchFeedbackAdaptation()
    {
    delete iTouchFeedback;
    Dll::SetTls( NULL );
    }

// ---------------------------------------------------------------------------
// Retrieve instance from thread local storage
// ---------------------------------------------------------------------------
//
MTouchFeedback* CTouchFeedbackAdaptation::GetInstance()
    {
    TAny* selfPtr = Dll::Tls();

    CTouchFeedbackAdaptation* self = 
        static_cast<CTouchFeedbackAdaptation*>( selfPtr );
    
    if ( self )
        {
        return self->iTouchFeedback;
        }
    else
        {
        return NULL;
        }
    }

// ---------------------------------------------------------------------------
// Create new instance and return it to caller
//
// We must have check for the case where client calls this even though
// instance exists already (in this case we only return the existing 
// instance).
// ---------------------------------------------------------------------------
//
MTouchFeedback* CTouchFeedbackAdaptation::CreateInstanceL()
    {   
    // Use current instance if it exists
    MTouchFeedback* feedback = GetInstance();

    // If current instance does not exist, then create a new one
    if ( !feedback )
        {
        CTouchFeedbackAdaptation* tmp = NewL();
        
        feedback = tmp->GetInstance();
        }
        
    return feedback;
    }

// ---------------------------------------------------------------------------
// Destroy ourselves if we are found at thread local storage (tls)
//
// Notice that tls content does not need to be zeroed because that is already
// done in the destructor.
// ---------------------------------------------------------------------------
//
void CTouchFeedbackAdaptation::DestroyInstance()
    {
    TAny* selfPtr = Dll::Tls();

    CTouchFeedbackAdaptation* self = 
        static_cast<CTouchFeedbackAdaptation*>( selfPtr );
    
    if ( self )
        {
        delete self;
        }        
    }    
    
// ---------------------------------------------------------------------------
// Empty implementation, as this function is no longer in use.
// (Control visibility changes now handled in HandleControlStateChange)
// ---------------------------------------------------------------------------
//
EXPORT_C void CTouchFeedbackAdaptation::ControlVisibilityChanged( 
    const CCoeControl* /*aControl*/ )
    {
    }

// ---------------------------------------------------------------------------
// Nothing to do here, just pass the information to implementation.
// ---------------------------------------------------------------------------
//
EXPORT_C void CTouchFeedbackAdaptation::LayoutChanged( )
    {
    if ( iTouchFeedback )
        {
        iTouchFeedback->LayoutChanged();
        }
    }
    
// ---------------------------------------------------------------------------
// From class MObjectProvider.
// We have to implement this because it is pure virtual in base class.
// Return Null as we are not actually part of object provider hierarchy.
// ---------------------------------------------------------------------------
TTypeUid::Ptr CTouchFeedbackAdaptation::MopSupplyObject( TTypeUid /*aId*/ )
    {
    return TTypeUid::Null();
    }
        
// ---------------------------------------------------------------------------
// From class MCoeControlStateObserver.
// Here we just pass the information about control state change ahead to
// actual feedback implementation.
// ---------------------------------------------------------------------------
TInt CTouchFeedbackAdaptation::HandleControlStateChange( 
    CCoeControl* aControl,
    TCoeState /*aState*/ )
    {
    if ( iTouchFeedback )
        {
        iTouchFeedback->ControlVisibilityChanged( aControl );
        }
        
    return KErrNone;
    }
    


