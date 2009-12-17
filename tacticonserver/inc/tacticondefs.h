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
* Description: Tacticon server definitions.
* Part of    : Tacticon Server
*/

#ifndef TACTICONDEFS_H
#define TACTICONDEFS_H

_LIT( KTacticonServerName, "TacticonServer" );

const TInt KTacticonServerMinor = 0;
const TInt KTacticonServerMajor = 1;
const TInt KTacticonServerBuild = 0;

const TInt KTacticonShutdownDelay = 2000000; // 2s

enum TTacticonCommands // Server commands.
    {
    EPlayTacticon,
    EStopTacticon,
    EEnableTacticons,
    ETacticonsEnabled
    };

#endif // TACTICONDEFS_H
