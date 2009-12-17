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
* Description:  A plug-in for turning tactile feedback on/off.
* Part of:      Tactile Feedback.
*
*/


#ifndef C_TACTILEACTIONPLUGIN_H
#define C_TACTILEACTIONPLUGIN_H

// INCLUDES
#include <cfactionplugin.h> 
#include <e32base.h>

/**
* Class CTactileActionPlugIn.
*
* Turns tactile feedback on/off when needed.
* Triggered when the conditions from the rule file are valid.
*/
class CTactileActionPlugIn : public CCFActionPlugIn
    {
public:

    /**
     * Two phased constructor.
     */
    static CTactileActionPlugIn* NewL();

    /**
     * Two phased constructor.
     */
    static CTactileActionPlugIn* NewLC();

    /**
     * Destructor.
     */
    ~CTactileActionPlugIn();

public: // From CCFActionPlugIn

    /**
     * @see CCFActionPlugIn
     */
    void InitializeL();

    /**
     * @see CCFActionPlugIn
     */
    TExecutionTime ExecuteL(
        CCFActionIndication* aActionIndication );
                
    /**
     * @see CCFActionPlugIn
     */
    void GetActionsL( CDesCArray& aActionList ) const;
    
    /**
     * @see CCFActionPlugIn
     */
    const TSecurityPolicy& SecurityPolicy() const;
    
protected:

    /**
     * C++ constructor
     */
    CTactileActionPlugIn();
    
private:
    TInt iFeedbackInCall;
    
    };

#endif // C_TACTILEACTIONPLUGIN_H
