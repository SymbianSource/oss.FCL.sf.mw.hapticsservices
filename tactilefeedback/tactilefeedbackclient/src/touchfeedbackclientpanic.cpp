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
* Description:  Panic function for Tactile Feedback Client
* Part of:      Tactile Feedback.
*
*/



#include <e32base.h>

#include "touchfeedbackclientpanic.h"


_LIT( KTouchClientPanic, "Touch FB Client" );


void Panic( TTouchFeedbackClientPanic aPanic )
    {
    User::Panic( KTouchClientPanic, aPanic );
    }

// End of File

    
    


