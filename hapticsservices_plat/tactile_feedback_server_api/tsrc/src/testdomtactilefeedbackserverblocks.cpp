/*
* Copyright (c) 2002 - 2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Test tactilefeedbackserver.h
*
*/



// [INCLUDE FILES]
#include <e32svr.h>
#include <stifparser.h>
#include <stiftestinterface.h>
#include <tactilefeedbackserver.h>

#include "testdomtactilefeedbackserver.h"
#include "testtactilefeedbackserver.h"

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// Ctestdomtactilefeedbackserver::RunMethodL
// Run specified method. Contains also table of test mothods and their names.
// -----------------------------------------------------------------------------
//
TInt Ctestdomtactilefeedbackserver::RunMethodL( CStifItemParser& aItem ) 
    {

    static TStifFunctionInfo const KFunctions[] =
        {  
        // First string is the function name used in TestScripter script file.
        // Second is the actual implementation member function. 
        ENTRY( "TestTactileFdbkServerInstanceL", 
            Ctestdomtactilefeedbackserver::TestTactileFdbkServerInstanceL ),

        // [test cases entries]

        };

    const TInt count = sizeof( KFunctions ) / sizeof( TStifFunctionInfo );

    return RunInternalL( KFunctions, count, aItem );

    }

// -----------------------------------------------------------------------------
// Ctestdomtactilefeedbackserver::TestTactileFdbkServerInstanceL
// -----------------------------------------------------------------------------
//
TInt Ctestdomtactilefeedbackserver::TestTactileFdbkServerInstanceL( CStifItemParser& /*aItem*/ )
    {
    CTestMTactileFeedbackServer* FbServer = new( ELeave ) CTestMTactileFeedbackServer;
    CleanupStack::PushL( FbServer );
    MTactileFeedbackServer* server = FbServer->Instance();
    STIF_ASSERT_NULL( server );
    CleanupStack::PopAndDestroy( FbServer );
    return KErrNone;
    }

//  [End of File]

