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
* Description:  Tactile Feedback Client Connection point to application 
*                framework.
* Part of:      Tactile Feedback.
*
*/



#ifndef C_TOUCHFEEDBACKADAPTATION_H
#define C_TOUCHFEEDBACKADAPTATION_H



#include <e32std.h>
#include <e32base.h>
#include <coecobs.h>
#include <coemain.h>



class CTouchFeedbackImpl;
class CTouchFeedbackClient;
class MTouchFeedback;


/**
 *  The class for instantiating and accessing Tactile Feedback Client API.
 *
 *  Instance of this class should be created at construction phase
 *  of every new application. This interface is ment to be used only
 *  by the AppUI class for contructing and destroying the Client API, and
 *  for informing the client about layout changes.
 *
 *  @lib touchfeedback.lib
 *  @since S60 v5.0
 */
class CTouchFeedbackAdaptation : 
        public CBase, public MObjectProvider, public MCoeControlStateObserver
    {


public:

    /**
     * Instantiation method.
     *
     * One should not attempt to build more than one instance in each
     * application process.
     *
     * @return New CTouchFeedbackAdaptation instance.
     */
    IMPORT_C static CTouchFeedbackAdaptation* NewL();

    virtual ~CTouchFeedbackAdaptation();
    
    /**
     * Used by MTouchFeedback::Instance
     *
     * @see MTouchFeedback.
     */
    static MTouchFeedback* GetInstance();

    /**
     * Used by MTouchFeedback::CreateInstanceL
     *
     * @see MTouchFeedback.
     */
    static MTouchFeedback* CreateInstanceL();
    
    /**
     * Used by MTouchFeedback::DestroyInstance
     *
     * @see MTouchFeedback.
     */
    static void DestroyInstance();
    
    /**
     * Deprecated. This is still in place for keeping the .def -file
     * unchanged.
     */
    IMPORT_C void ControlVisibilityChanged( const CCoeControl* aControl );

    /**
     * This should be called by the application framework for informing
     * that current layout has been changed. This way Tactile FW can
     * disable all feedback areas in those layouts, where pen usage
     * is not enabled (direct feedback is on UI controls' responsibility).
     */
    IMPORT_C void LayoutChanged();
  
public: // From MObjectProvider
    
    /**
     * We have to implement this because it is pure virtual in
     * MObjectProvider, but we always return TTypeUid::Null.
     *
     * @param aId - Encapsulates the Uid that identifies the type of 
     *              object required. Ignored in this class.
     */
    TTypeUid::Ptr MopSupplyObject( TTypeUid aId );
  
public: // From MCoeControlStateObserver
    
    /**
     * This is implemented for getting notificatons when given control's 
     * state (i.e. visiblity or dimming status) has been changed. 
     * This way Tactile FW knows to disable or enable
     * the feedback for that control again.
     *
     * @param aControl - Pointer to the control, which visiblity 
     *                   or dimming status has changed.
     * @param aState   - Can be used for knowing if it was dimming or
     *                   visibility status that changed.
     */
    TInt HandleControlStateChange( CCoeControl* aControl, TCoeState aState );
  
private:

    CTouchFeedbackAdaptation();

    void ConstructL();


private: // data

    /**
     * The actual feedback instance.
     * Own.  
     */
    CTouchFeedbackImpl* iTouchFeedback;    
    };


#endif //  C_TOUCHFEEDBACKADAPTATION_H
