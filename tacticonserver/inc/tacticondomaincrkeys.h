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
* Description: Domain level Central Repository keys for Tacticon settings.
* Part of    : Tacticon Server
*/

#ifndef TACTICONDOMAINCRKEYS_H
#define TACTICONDOMAINCRKEYS_H

#include <e32std.h>

const TUid KCRUidTacticon = {0x2001FE4D};

/**
* Base strength of tacticons
*
* Possible values are:
* 0-100
*
* Default value: 50
**/
const TUint32 KTacticonStrength                    = 0x00000001;

/**
* Full path of the IVT file, which is used to play tacticons. 
*
* Possible values are:
* Valid path to IVT file. 
*
* Default value: "Z:\\system\\sounds\\digital\\tacticons.ivt"
**/
const TUint32 KTacticonIVTFile                     = 0x00000002;


#endif  // TACTICONDOMAINCRKEYS_H

// End of File
