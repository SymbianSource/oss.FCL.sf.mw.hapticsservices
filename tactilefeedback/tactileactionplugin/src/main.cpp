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


// INCLUDE FILES
#include <e32std.h>
#include <ecom/implementationproxy.h>

#include "tactileactionplugin.h"
#include "tactileactionpluginconst.hrh"

// -----------------------------------------------------------------------------
// Provides a key value pair table, which is used to identify the correct 
// construction function for the requested interface.
// -----------------------------------------------------------------------------
//
const TImplementationProxy ImplementationTable[] = 
    {
    IMPLEMENTATION_PROXY_ENTRY( 
        KTactileActionPluginImplementationUid, 
        CTactileActionPlugIn::NewL )
    };

// -----------------------------------------------------------------------------
// Returns an instance of the proxy table.
// -----------------------------------------------------------------------------
//
EXPORT_C const TImplementationProxy* ImplementationGroupProxy( 
    TInt& aTableCount )
    {
    aTableCount 
        = sizeof( ImplementationTable ) / sizeof( TImplementationProxy );
    return ImplementationTable;
    }

// End of File
