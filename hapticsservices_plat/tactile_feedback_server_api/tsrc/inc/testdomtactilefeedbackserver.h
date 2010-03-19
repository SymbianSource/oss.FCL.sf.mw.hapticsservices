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



#ifndef C_TESTDOMTACTILEFEEDBACKSERVER_H
#define C_TESTDOMTACTILEFEEDBACKSERVER_H

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
_LIT( KtestdomtactilefeedbackserverLogPath, "\\logs\\testframework\\testdomtactilefeedbackserver\\" ); 
// Log file
_LIT( KtestdomtactilefeedbackserverLogFile, "testdomtactilefeedbackserver.txt" ); 
_LIT( KtestdomtactilefeedbackserverLogFileWithTitle, "testdomtactilefeedbackserver_[%S].txt" );

/**
*  Ctestdomtactilefeedbackserver test class for STIF Test Framework TestScripter.
*  @since S60 5.0
*/
NONSHARABLE_CLASS( Ctestdomtactilefeedbackserver ) : public CScriptBase
    {
public:  // Constructors and destructor

    /**
    * Two-phased constructor.
    */
    static Ctestdomtactilefeedbackserver* NewL( CTestModuleIf& aTestModuleIf );

    /**
    * Destructor.
    */
    virtual ~Ctestdomtactilefeedbackserver();

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
    Ctestdomtactilefeedbackserver( CTestModuleIf& aTestModuleIf );

    /**
    * By default Symbian 2nd phase constructor is private.
    */
    void ConstructL();
    
private:
    /**
     * TestTactileFdbkServerInstanceL test function for testing 
     * the Instance function
     * @since S60 5.0
     * @param aItem never used
     * @return Symbian OS error code.
     */
    virtual TInt TestTactileFdbkServerInstanceL( CStifItemParser& aItem );
    
    /**
     * Method used to log version of test class
     */
    void SendTestClassVersion();

private:    // Data

    /**
     * ScreenSaver Property
     */
    TInt iOldScreenSaverProperty;

    };

#endif      // C_TESTDOMTACTILEFEEDBACKSERVER_H

// End of File
