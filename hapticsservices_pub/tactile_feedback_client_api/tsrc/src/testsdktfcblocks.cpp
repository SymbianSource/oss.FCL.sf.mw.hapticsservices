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
* Description:  tactile_feedback_client_api
*
*/



// [INCLUDE FILES]
#include <e32svr.h>
#include <stifparser.h>
#include <stiftestinterface.h>
#include <touchfeedback.h>

#include "testsdktfc.h"


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CTestSDKTFC::Delete
// Delete here all resources allocated and opened from test methods. 
// Called from destructor. 
// -----------------------------------------------------------------------------
//
void CTestSDKTFC::Delete() 
    {

    }

// -----------------------------------------------------------------------------
// CTestSDKTFC::RunMethodL
// Run specified method. Contains also table of test mothods and their names.
// -----------------------------------------------------------------------------
//
TInt CTestSDKTFC::RunMethodL( 
    CStifItemParser& aItem ) 
    {

    static TStifFunctionInfo const KFunctions[] =
        {  
        // First string is the function name used in TestScripter script file.
        // Second is the actual implementation member function. 

        ENTRY( "TestTFBInstance", CTestSDKTFC::TestTFBInstance ),
        ENTRY( "TestTFBCreateInstanceL", CTestSDKTFC::TestTFBCreateInstanceL ),
        ENTRY( "TestTFBDestroyInstance", CTestSDKTFC::TestTFBDestroyInstance ),

        // [test cases entries]

        };

    const TInt count = sizeof( KFunctions ) / sizeof( TStifFunctionInfo );

    return RunInternalL( KFunctions, count, aItem );

    }


// -----------------------------------------------------------------------------
// CTestSDKTFC::TestTFCInstance
// Instance test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CTestSDKTFC::TestTFBInstance( CStifItemParser& /*aItem*/ )
    {
    // Print to UI 
        _LIT( Ktestsdktfc, "testsdktfc" );
        _LIT( KTestTFBInstance, "In TestTFBInstance" );
        TestModuleIf().Printf( 0, Ktestsdktfc, KTestTFBInstance );
        // Print to log file
        iLog->Log( KTestTFBInstance );
    
        MTouchFeedback* ptr = MTouchFeedback::Instance( );
        
        STIF_ASSERT_NOT_NULL( ptr );
        
        return KErrNone;
    
    }

// -----------------------------------------------------------------------------
// CTestSDKTFC::TestTFCInstance
// Instance test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CTestSDKTFC::TestTFBCreateInstanceL( CStifItemParser& /*aItem*/ )
    {
    // Print to UI 
        _LIT( Ktestsdktfc, "testsdktfc" );
        _LIT( KTestTFBCreateInstanceL, "In TestTFBCreateInstanceL" );
        TestModuleIf().Printf( 0, Ktestsdktfc, KTestTFBCreateInstanceL );
        // Print to log file
        iLog->Log( KTestTFBCreateInstanceL );
   
        MTouchFeedback* mtfb = MTouchFeedback::CreateInstanceL( );
        //ptr->CreateInstanceL();
        
        STIF_ASSERT_NOT_NULL( mtfb );
        
        return KErrNone;
    
    }

// -----------------------------------------------------------------------------
// CTestSDKTFC::TestTFCInstance
// Instance test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CTestSDKTFC::TestTFBDestroyInstance( CStifItemParser& /*aItem*/ )
    {
    // Print to UI 
        _LIT( Ktestsdktfc, "testsdktfc" );
        _LIT( KTestTFBDestroyInstance, "In TestTFBDestroyInstance" );
        TestModuleIf().Printf( 0, Ktestsdktfc, KTestTFBDestroyInstance );
        // Print to log file
        iLog->Log( KTestTFBDestroyInstance );
   
       
         TInt err = KErrNone;
         TRAP ( err, MTouchFeedback::DestroyInstance());
        
        
        return err;
    
    }
//  [End of File]
