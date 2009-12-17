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
* Description: File with debuf trace definitions.
* Part of    : Tacticon Server
*/

#ifndef TACTICONTRACE_H
#define TACTICONTRACE_H

#ifdef _DEBUG

#include <e32debug.h>

#define TRACE( x )            RDebug::Print( _L( x  ) )
#define TRACE2( x, y )        RDebug::Print( _L( x ), ( y ) )
#define TRACE3( x, y, z )     RDebug::Print( _L( x ), ( y ), ( z ) )
#define TRACE4( x, y, z, zz ) RDebug::Print( _L( x ), ( y ), ( z ), ( zz ) )

#else // _DEBUG

#define TRACE( x ) 
#define TRACE2( x, y ) 
#define TRACE3( x, y, z ) 
#define TRACE4( x, y, z, zz ) 

#endif // _DEBUG
   
#endif // TACTICONTRACE_H

