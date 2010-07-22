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
* Description:  Opcodes and data types used in communication between client 
*                process and window server.
* Part of:      Tactile Feedback.
*
*/


#ifndef T_TACTILEINTERNALDATATYPES_H
#define T_TACTILEINTERNALDATATYPES_H



#include <e32std.h>
#include <e32svr.h>
#include <touchlogicalfeedback.h>


/**
 *  These are used as opcodes (function number) in IPC communication 
 *  between Tactile Feedback Client and the server hosting
 *  Tactile Area Registry (currently window server)
 */
enum TTactileFeedbackOpCodes
    {
    ETactileOpCodeConnect = 200,
    ETactileOpCodeDisconnect = 201,
    ETactileOpCodeImmediateFeedback = 202
    };

/**
 *  This is the data package that must be delivered with opcode 
 *  ETouchOpCodeConnect.
 */
struct TTactileFeedbackConnectData
    {
    TInt   iWindowGroupId;
    TKName iChunkName;
    };

/**
 *  This is the data package that must be delivered with opcode 
 *  ETouchOpCodeDisconnect.
 */
struct TTactileFeedbackDisconnectData
    {
    TInt   iWindowGroupId;
    };

/**
 *  This is the data package that must be delivered with opcode 
 *  ETactileOpCodeImmediateFeedback.
 */
struct TTactileFeedbackImmediateData
    {
    TTouchLogicalFeedback iFeedbackType;
    TBool iPlayVibra;
    TBool iPlayAudio;
    };

    
/**
 *  This struct represents one area registry entry in shared memory chunk.
 * 
 *  Top two bits of iFeedbackType field are used for coding vibra-
 *  and audio feedback status together with feedback type:
 *
 *  0x80000000 --> 1 If vibra feedback is enabled for this area
 *  0x40000000 --> 1 If audio feedback is enabled for this area
 *  
 */    
struct TFeedbackChunkAreaEntry
    {
    TRect iRect;
    TTouchLogicalFeedback iFeedbackType;
    TTouchEventType iEventType;  
    };

const TInt KTactileVibraBitDown = 0x80000000;
const TInt KTactileAudioBitDown = 0x40000000;    
const TInt KTactileVibraBitUp   = 0x20000000;
const TInt KTactileAudioBitUp   = 0x10000000;

_LIT( KTouchFeedbackSemaphore, "SemaphoreTouchFeedback");    
    

#endif // T_TACTILEINTERNALDATATYPES_H
