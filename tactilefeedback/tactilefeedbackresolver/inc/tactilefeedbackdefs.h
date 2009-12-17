/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Tactile Feedback server constants definitions.
* Part of:      Tactile Feedback.
*
*/


#ifndef TACTILEFEEDBACKDEFS_H
#define TACTILEFEEDBACKDEFS_H

_LIT( KTactileThreadName, "FeedbackPlayerThread");
_LIT( KTactileFeedbackServer, "TactileFeedbackServer" );

const TInt KTactileFeedbackServerMinor = 0;
const TInt KTactileFeedbackServerMajor = 1;
const TInt KTactileFeedbackServerBuild = 0;

const TInt KPlayerThreadStackSize   = 0x4000;   // 16kB
const TInt KPlayerThreadHeapMinSize = 0x1000;   // 4kB
const TInt KPlayerThreadHeapMaxSize = 0x100000; // 1MB


enum TTactileFeedbackCmd // Server commands.
    {
    EPlayFeedback,
    EStartFeedback,
    EModifyFeedback,
    EStopFeedback,
    ESetFBEnabledDevice,
    EFBEnabledForDevice,
    EPlayPreviewFeedback,
    EStartPreviewFeedback,
    EModifyPreviewFeedback,
    EStopPreviewFeedback
    };

#endif // TACTILEFEEDBACKDEFS_H

// End of file.
