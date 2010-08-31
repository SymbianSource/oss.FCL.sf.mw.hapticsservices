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



#ifndef C_TESTSDKTFC_H
#define C_TESTSDKTFC_H

//  INCLUDES
#include <stiflogger.h>
#include <testscripterinternal.h>
#include <stiftestmodule.h>
#include <testclassassert.h>

// MACROS
#define TEST_CLASS_VERSION_MAJOR 0
#define TEST_CLASS_VERSION_MINOR 0
#define TEST_CLASS_VERSION_BUILD 0

// Logging path
_LIT( KtestsdktfcLogPath, "\\logs\\testframework\\testsdktfc\\" ); 
// Log file
_LIT( KtestsdktfcLogFile, "testsdktfc.txt" ); 
_LIT( KtestsdktfcLogFileWithTitle, "testsdktfc_[%S].txt" );

/**
*  CTestSDKTFC test class for STIF Test Framework TestScripter.
*  @since S60 5.0
*/
NONSHARABLE_CLASS(CTestSDKTFC) : public CScriptBase
    {
public:  // Constructors and destructor

    /**
    * Two-phased constructor.
    */
    static CTestSDKTFC* NewL( CTestModuleIf& aTestModuleIf );

    /**
    * Destructor.
    */
    virtual ~CTestSDKTFC();

public: // Functions from base classes

    /**
    * From CScriptBase Runs a script line.
    * @since S60 5.0
    * @param aItem Script line containing method name and parameters
    * @return Symbian OS error code
    */
    virtual TInt RunMethodL( CStifItemParser& aItem );

private:

    /**
    * C++ default constructor.
    */
    CTestSDKTFC( CTestModuleIf& aTestModuleIf );

    /**
    * By default Symbian 2nd phase constructor is private.
    */
    void ConstructL();

    /**
    * Frees all resources allocated from test methods.
    * @since S60 5.0
    */
    void Delete();

    
    /**
     * Method used to log version of test class
     */
    void SendTestClassVersion();

    //[TestMethods]
private:// for test the touchfeedback.h
    /**
     * TestTFCInstance test method for test the Instance method.
     * @since S60 5.0
     * @return Symbian OS error code.
     */
    virtual TInt TestTFBInstance( CStifItemParser& aItem );
    /**
     * TestTFCCreateInstanceL test method for test the CreateInstanceL method.
     * @since S60 5.0
     * @return Symbian OS error code.
     */
    virtual TInt TestTFBCreateInstanceL( CStifItemParser& aItem );
    /**
     * TestTFBDestroyInstance test method for test the DestroyInstance method.
     * @since S60 5.0
     * @return Symbian OS error code.
     */
    virtual TInt TestTFBDestroyInstance( CStifItemParser& aItem );
    
private:// for test the 
    
private:    // Data

    /**
     * ScreenSaver Property
     */
    TInt iOldScreenSaverProperty;

    };

#endif      // C_TESTSDKTFC_H

// End of File
