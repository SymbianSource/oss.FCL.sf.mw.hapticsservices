/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  A plug-in for disabling/enabling tacticons.
* Part of:      Tacticon Server.
*
*/


#ifndef C_TACTICONACTIONPLUGIN_H
#define C_TACTICONACTIONPLUGIN_H

// INCLUDES
#include <cfactionplugin.h> 
#include <e32base.h>
#include <tacticon.h>

/**
* Class CTacticonActionPlugIn.
*
* Disable/enable/play tacticons when needed.
* Triggered when the conditions from the rule file are valid.
*/
class CTacticonActionPlugIn : public CCFActionPlugIn
    {
public:

    /**
     * Two phased constructor.
     * @since S60 5.2     
     */
    static CTacticonActionPlugIn* NewL();

    /**
     * Two phased constructor.
     * @since S60 5.2     
     */
    static CTacticonActionPlugIn* NewLC();

    /**
     * Destructor.
     * @since S60 5.2
     */
    virtual ~CTacticonActionPlugIn();

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
    CTacticonActionPlugIn();
    
private: // data
    /**
     * Tacticon server interface.
     * Own.    
     */    
    RTacticon iTacticon;

    /**
     * Boolean value, which determines if tacticons are enabled 
     * during voice call.
     */
    TBool iEnabledDuringCall;
    
    /**
     * Tacticon settings bitmap, which determines Tacticon playback 
     * when voice call is connected/disconnected.
     */
    TInt iTacticonCallSettings;
    
    /**
     * Flag for proximity detection.
     */
    TBool iProximityDiscernible;
    
    /**
     * Flag for playing 'call connected' tacticon only when call
     * was initiated by this device.
     */
    TBool iCallInitiated;
    
    };

#endif // C_TACTICONACTIONPLUGIN_H
