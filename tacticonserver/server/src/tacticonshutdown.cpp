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

#include "tacticonshutdown.h"
#include "tacticondefs.h"

// ---------------------------------------------------------------------------
// Two phased constructor.
// ---------------------------------------------------------------------------
//
CTacticonShutdown* CTacticonShutdown::NewL()
    {
    CTacticonShutdown* self = new ( ELeave ) CTacticonShutdown();
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// Destructor.
// ---------------------------------------------------------------------------
//
CTacticonShutdown::~CTacticonShutdown()
    {
    }

// ---------------------------------------------------------------------------
// Activates this timer.
// ---------------------------------------------------------------------------
//
void CTacticonShutdown::Start()
    {
    After( KTacticonShutdownDelay );
    }

// ---------------------------------------------------------------------------
// Constructor of the shutdown timer.
// ---------------------------------------------------------------------------
//
CTacticonShutdown::CTacticonShutdown()
    : CTimer( EPriorityStandard )
    {
    // add this active object to the active scheduler
    CActiveScheduler::Add( this );
    }

// ---------------------------------------------------------------------------
// Second phase construction.
// ---------------------------------------------------------------------------
//
void CTacticonShutdown::ConstructL()
    {
    CTimer::ConstructL();
    }

// ---------------------------------------------------------------------------
// Shuts down tacticon server by stopping the active scheduler.
// ---------------------------------------------------------------------------
//
void CTacticonShutdown::RunL()
    {
    CActiveScheduler::Stop();
    }

//  End of File  
