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
* Description:  Panic codes for Tactile Feedback Client.
* Part of:      Tactile Feedback.
*
*/



#ifndef T_TOUCHFEEDBACKCLIENTPANIC_H
#define T_TOUCHFEEDBACKCLIENTPANIC_H

#include <e32def.h>


// DATA TYPES
enum TTouchFeedbackClientPanic
    {
    // Invalid array index passed to some internal function
    ETouchClientPanicArrayAccess = 0
    };

// FUNCTION PROTOTYPES
GLDEF_C void Panic( TTouchFeedbackClientPanic aPanic );


#endif // T_TOUCHFEEDBACKCLIENTPANIC_H

