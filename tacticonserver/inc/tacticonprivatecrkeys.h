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
* Description: Private Central Repository keys for Tacticon setttings.
* Part of    : Tacticon Server
*/


#ifndef TACTICONPRIVATECRKEYS_H
#define TACTICONPRIVATECRKEYS_H

#include "tacticondomaincrkeys.h"

/**
* Implementation UID of the plugin, which is used to play tacticons.
*
* Possible values are:
* Implementation UID of the plugin.
*
* Default value: 537001553 (0x2001FE51)
**/
const TUint32 KTacticonPlugin                      = 0x00000003;

/**
* Boolean value to define is tacticons disabled during a phone call or not.
*
* Possible values are:
* 0, 1
*
* Default value: 0
**/
const TUint32 KTacticonDuringCall                  = 0x00000004;

/**
* Bitmask to define which tacticons are enabled:
* Bit 1: Call connected tacticon
*
* Possible values are:
* 0-1
*
* Default value: 1
**/
const TUint32 KTacticonSet                         = 0x00000005;

/**
* Defines bit 1 in KTacticonSet. 
* If this bit is 1, tacticon is played when call is connected
* If this bit is 0, tacticon is not played when call is connected.
* By default this is 1.
*/
const TInt KTacticonSetCallConnected = 0x01;  // 2^0

/**
* Actuator type used for playing tacticons
*
* Possible values are defined in hwrmlogicalactuators.h
*
* Default value: 1 (EHWRMLogicalActuatorAny)
**/
const TUint32 KActuatorType                    = 0x00000006;


#endif  // TACTICONPRIVATECRKEYS_H

// End of File
