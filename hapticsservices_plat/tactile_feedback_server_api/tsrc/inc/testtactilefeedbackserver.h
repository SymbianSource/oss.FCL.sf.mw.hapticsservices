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

#ifndef C_TESTTACTILEFEEDBACKSERVER_H
#define C_TESTTACTILEFEEDBACKSERVER_H

//INCLUDE
#include <tactilefeedbackserver.h>
#include <e32base.h>

/*
 * This class inherit from MTactileFeedbackServer for testing function
 */
class CTestMTactileFeedbackServer:public CBase, public MTactileFeedbackServer
    {
public:
    /*
     * Constructor
     */
    CTestMTactileFeedbackServer();
    
    /*
     * Destructor
     */
    ~CTestMTactileFeedbackServer();
    
    /*
     * This function is a implement of pure implement function
     */
    void InstantFeedback( TTouchLogicalFeedback aType );
    };

#endif /*C_TESTTACTILEFEEDBACKSERVER_H*/

//Endfile

