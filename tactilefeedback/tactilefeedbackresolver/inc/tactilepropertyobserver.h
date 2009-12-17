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
* Description:  Callback interface for getting a notification about
*                property changes.
* Part of:      Tactile Feedback.
*
*/


#ifndef M_TACTILEPROPERTYOBSERVER_H
#define M_TACTILEPROPERTYOBSERVER_H



#include <e32std.h>


/**
 *  Callback interface used from CTactilePropertyWatcher.
 *
 *  @lib N/A
 *  @since S60 v5.0
 */
class MTactilePropertyObserver
    {
public:
    /**
     * This gets called when a subscribed property is changed.
     *
     * @param aCategory - The UID of the changed property
     * @param aKey      - The key (i.e. identifier) of the changed property
     */
    virtual void PropertyChangedL( const TUid aCategory, 
                                   const TUint aKey ) = 0;
    };



#endif // M_TACTILEPROPERTYOBSERVER_H
