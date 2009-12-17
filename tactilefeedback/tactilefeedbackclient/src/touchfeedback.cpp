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
* Description:  Tactile Feedback client Instance Factory.
* Part of:      Tactile Feedback.
*
*/

#include <eikenv.h>
#include <touchfeedbackadaptation.h>

#include "touchfeedback.h"


// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
EXPORT_C MTouchFeedback* MTouchFeedback::Instance()
    {
    return CTouchFeedbackAdaptation::GetInstance();
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
EXPORT_C MTouchFeedback* MTouchFeedback::CreateInstanceL()
    {
    return CTouchFeedbackAdaptation::CreateInstanceL();
    }
    
// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
EXPORT_C void MTouchFeedback::DestroyInstance()
    {
    CTouchFeedbackAdaptation::DestroyInstance();
    }
