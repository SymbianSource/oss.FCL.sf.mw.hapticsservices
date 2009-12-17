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
* Description:  Master trace -file for whole Tactile Feedback subsystem.
* Part of:      Tactile Feedback.
*
*/



#ifndef TACTILEFEEDBACKTRACE_H
#define TACTILEFEEDBACKTRACE_H

#include <bldvariant.hrh>


#ifndef _DEBUG

  #define TRACE( x )
  #define TRACE2( x, y )
  #define TRACE3( x, y, z )
  #define TRACE4( x, y, z, zz )
  #define TRACE5( x, y, z, zz, zzz )
  
#else

#include <e32std.h>
#include <e32svr.h> // RDebug

#define TRACE( x )            RDebug::Print( _L( x  ) )
#define TRACE2( x, y )        RDebug::Print( _L( x ), ( y ) )
#define TRACE3( x, y, z )     RDebug::Print( _L( x ), ( y ), ( z ) )
#define TRACE4( x, y, z, zz ) RDebug::Print( _L( x ), ( y ), ( z ), ( zz ) )
#define TRACE5( x, y, z, zz, zzz ) RDebug::Print( _L( x ), ( y ), ( z ), ( zz ), ( zzz ) )
  
#endif // _DEBUG

#endif // TACTILEFEEDBACKTRACE_H

