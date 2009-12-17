/*
* Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  The click maker plugin, which
*                handles key events at window server process.
* Part of:      Tactile Feedback.
*
*/


#ifndef C_TACTILECLICKPLUGIN_H
#define C_TACTILECLICKPLUGIN_H



#include <w32click.h>

#ifdef SYMBIAN_ENABLE_SPLIT_HEADERS
#include <graphics/pointereventdata.h> 
#endif //SYMBIAN_ENABLE_SPLIT_HEADERS

#include <tactilefeedback.h>

#include "tactilefeedbackserver.h"

class RTactileFeedback;
class CTactileAreaRegistry;


/**
 *  The click maker plugin for triggering tactile feedback.
 *
 *  Tactile feedback is generated on each pointer down event, in case the
 *  pointer event hit some area on screen where feedback is enabled.
 *  Feedback triggering has tight latency requirements, and thus this
 *  click maker plugin's purpose is to enable feedback triggering from
 *  window server process (instead of application process when the pointer
 *  event arrives there).
 *
 *  Feedback areas are kept in the Area Registry, and this plug-ins task is
 *  to pass incoming pointer events to the Area registry so that hit tests
 *  can be done to see if any feedback area was hit or not.
 *
 *  The implementation of this plugin relies to the fact that OtherEvent
 *  -function is called with the pointer event + window identifier and
 *  window group identifier as additional parameters (these are Symbian
 *  changes compared to the documented behaviour).
 *
 *  This plug-in is also responsible of providing 
 *  Tactile Feedback AnimDll API for AnimDll plug-ins. That API is 
 *  designed for supporting direct feedback triggering from AnimDll plug-ins,
 *  and it needs to be implemented by this plug-in because this is the only 
 *  component in Tactile Feedback subsystem that is guaranteed to run
 *  in the window server's process (other components have been designed in
 *  such way that they can be moved elsewhere in case necessary).
 *
 *  @lib N/A
 *  @since S60 v5.0
 */
NONSHARABLE_CLASS( CTactileClickPlugin ): 
    public CClickMaker,
    public MTactileFeedbackServer
    {
public:

    static CTactileClickPlugin* NewL();

    virtual ~CTactileClickPlugin();

// from base class CClickMaker

    /**
     * From CClickMaker.
     * This function is called by the window server whenever there is a 
     * key event, to generate the sound.
     *
     * The key events are not used by this plugin actually, because this
     * plug-in's purpose is to handle only pointer events.
     *
     * @since S60 v5.0
     * @param aType  - The type of the key event.
     * @param aEvent - The key event details. 
     */
    void KeyEvent( TEventCode aType,const TKeyEvent& aEvent );

    /**
     * From CClickMaker.
     * This function is called by the window server whenever there is a 
     * pointer event, to generate the sound.
     *
     * Not used by this plug-in because there is not enough information in
     * the pointer event for resolving the actual area and window where
     * pointer event was hit. Intead pointer events are handled in 
     * OtherEvent -function.
     *
     * @since S60 v5.0
     * @param aEvent - The pointer event details.
     */
    void PointerEvent( const TPointerEvent& aEvent );

    /**
     * From CClickMaker.

     * This function is a "future expansion" of the original click maker API,
     * and it has been taken into use by this plugin for getting more 
     * information about the pointer event than what is possible to pass as
     * parameter to PointerEvent -function.
     *
     * @since S60 v5.0
     * @param aType  - The type of the event.
     * @param aParam - Pointer to data package containing the pointer event,
     *                 window identifier where pointer event hit, and window
     *                 group identifier of the window group where the window
     *                 belongs to.
     */
    void OtherEvent( TInt aType, TAny* aParam=NULL );

    /**
     * From CClickMaker.
     * Implemented to enable this plug-in to communicate with the window
     * server client.
     *
     * Area registry management communication (such as creating or closing
     * an application) is communicated through this function.
     *
     * In practice communication is done between this plug-in, and 
     * Tactile Feedback Client API implementation.
     *
     * @since S60 v5.0
     * @param aOpcode - Opcode of the command sent by Client API
     * @param aArgs   - Pointer to the data sent by client.
     */
    TInt CommandReplyL( TInt aOpcode, TAny* aArgs );


// from base class MTactileFeedbackServer

    /**
     * From MTactileFeedbackServer.
     * Direct feedback service for Anim Dll plugins.
     *
     * @since S60 v5.0
     * @param aType - Logical feedback type to play.
     */
    void InstantFeedback( TTouchLogicalFeedback aType );

private:

    CTactileClickPlugin();

    void ConstructL();
    
    /**
     * Handles data parsing when new client connects to this plugin.
     *
     * Actual functionality is implemented in Tactile Area Registry.
     *
     * @param aArgs - Pointer to the data sent by client.
     */
    void HandleConnectL( TAny* aArgs );

    /**
     * Handles data parsing when client disconnects itself. In practice
     * this happens when application is closed.
     *
     * Actual functionality is implemented in Tactile Area Registry.
     *
     * @param aArgs - Pointer to the data sent by client.
     */
    void HandleDisconnect( TAny* aArgs );

    /**
     * Handles direct feedback request from Tactile Client API.
     *
     * In practice this function only parses the data so that the
     * logical feedback type is known. After that feedback type is passed
     * to Tactile Feedback Resolver, which will then play direct feedback
     * (or block the request if feedback is not enabled on the moment).
     *
     * @param aArgs - Pointer to the data sent by client.
     * @return KErrNone if operation succeeded, otherwise one of standand
     *         Symbian OS error codes.
     */
    TInt HandleImmediateFeedback( TAny* aArgs );


private: // data

    /**
     * Access to tactile feedback server, which actually produces 
     * the feedback.
     * Own.  
     */
    RTactileFeedback iFeedback;

    /**
     * Access to tactile area registry, which stores feedback areas and is
     * used for hit testing the incoming pointer events against registry 
     * content.
     * Own.  
     */
    CTactileAreaRegistry* iAreaRegistry;
    
    /**
     * Flag to indicate whether resolver construction has been attempted
     * already. We store this information to avoid unnecessary overhead at
     * boot when something is missing and it just is not possible to 
     * construct resolver.  
     */
    TBool iResolverConstructionAttempted;
    };


#endif // C_TACTILECLICKPLUGIN_H
