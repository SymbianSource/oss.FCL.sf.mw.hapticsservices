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
* Description:  Direct feedback interface for usage from Anim Dll plugins.
* Part of:      Tactile Feedback.
*
*/

#ifndef M_TACTILEFEEDBACKSERVER_H
#define M_TACTILEFEEDBACKSERVER_H


#include <e32std.h>
#include <touchlogicalfeedback.h>

/**
 *  Direct feedback interface for Anim Dll plugins.
 *
 *  This interface can be used for triggering feedback from Anim Dll plugins,
 *  or from any other DLLs which run in window server's process.
 *
 *  Usage example:
 *
 *    MTactileFeedbackServer* feedback = MTactileFeedbackServer::Instance();
 *    if ( feedback ) 
 *        {
 *        feedback->InstantFeedback( ETouchFeedbackBasic );
 *        }
 *
 *  @lib tactileclickplugin.lib
 *  @since S60 v5.0
 */
class MTactileFeedbackServer
    {
public:

    /**
     * Can be used for acquiring a pointer to touch feedback instance.
     *
     * Pointer is retrieved from thread local storage, and thus it is best
     * to store the returned pointer as member variable in case it shall
     * be needed frequently.
     *
     * NULL is returned in case there is no instance. In that case 
     * feedback playing is not possible.
     *
     * @since S60 5.0
     * @return Pointer to tactile feedback instance that can be used for direct
     *         feedback playing.
     */
    IMPORT_C static MTactileFeedbackServer* Instance();


    /**
     * Gives instant feedback.
     *
     * Notice that the feedback might not be actually played, if 
     * for example user has disabled the feedback from the settings.
     *
     * @since S60 5.0
     * @param aType - The logical feedback type to play.
     */
    virtual void InstantFeedback( TTouchLogicalFeedback aType ) = 0;
    };


#endif // M_TACTILEFEEDBACKSERVER_H
