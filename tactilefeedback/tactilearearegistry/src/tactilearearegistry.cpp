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
* Description:  The class to be used from server side for accessing registry.
*                Hit testing is done using this API.
* Part of:      Tactile Feedback.
*
*/


#include <tactilefeedbacktrace.h>
#include "tactilearearegistry.h"
#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "tactilearearegistryTraces.h"
#endif

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
CTactileAreaRegistry::TTactilePenDownEvent::TTactilePenDownEvent(): 
        iWindowGroupId( -1 ), iWindowHandle( 0 )
    {
    }
    
// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//    
CTactileAreaRegistry::TTactilePenDownEvent::TTactilePenDownEvent( 
    TInt aWindowGroupId, 
    TUint32 aWindowHandle, 
    TRect aFeedbackArea ): 
        iWindowGroupId( aWindowGroupId ), 
        iWindowHandle( aWindowHandle ),
        iFeedbackArea( aFeedbackArea )
    {
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CTactileAreaRegistry::TTactilePenDownEvent::Reset()
    {
    iWindowGroupId = -1 ;
    iWindowHandle = 0;
    }

// ---------------------------------------------------------------------------
// Constructor.
// ---------------------------------------------------------------------------
//
CTactileAreaRegistry::CTactileAreaRegistry()
    {
    }

// ---------------------------------------------------------------------------
// 2-phased constructor.
// ---------------------------------------------------------------------------
//
EXPORT_C CTactileAreaRegistry* CTactileAreaRegistry::NewL()
    {
    CTactileAreaRegistry* self = new ( ELeave ) CTactileAreaRegistry;
    
    // We don't need ConstructL in this class for the moment, so we
    // can just return the created instance right away.
    return self;
    }

// ---------------------------------------------------------------------------
// Destructor.
// ---------------------------------------------------------------------------
//
CTactileAreaRegistry::~CTactileAreaRegistry()
    {
    iChunkArray.Close();
    iWgArray.Close();
    iTactileSemaphore.Close();
    }


// ---------------------------------------------------------------------------
// #1 Find the shared chunk created by client
// #2 Open handle to the found chunk
// #3 Store window group id and connection handle with the chunk, 
//    and add chunk to array
// #4 Open global semaphore in case it is not yet open
// ---------------------------------------------------------------------------
//
EXPORT_C void CTactileAreaRegistry::HandleConnectL( 
    const TTactileFeedbackConnectData& aData )
    {
    TRACE3("CTactileAreaRegistry::HandleConnectL - wgId = %d, Chunk = %S", 
        aData.iWindowGroupId,
        &aData.iChunkName );
    
    // #1
    TFindChunk findChunk( aData.iChunkName );
    
    TFullName chunkFullName;
    
    TInt err = findChunk.Next( chunkFullName );
    
    User::LeaveIfError( err );
       
    // #2
    TTactileAreaChunk newChunk;

    err = newChunk.iChunk.OpenGlobal( chunkFullName, ETrue );

    User::LeaveIfError( err );
    
    CleanupClosePushL( newChunk.iChunk );
    
    // #3      
    newChunk.iWindowGroupId = aData.iWindowGroupId;
    
    newChunk.iConnectionHandle = 
        ConnectionHandleByWgId( aData.iWindowGroupId );
    
    iChunkArray.AppendL( newChunk );
    
    CleanupStack::Pop(); // newChunk.iChunk  
        
    // #4 Open global semaphore in case it is not yet open
    if ( !iTactileSemaphore.Handle() )
        {
        err = iTactileSemaphore.OpenGlobal( KTouchFeedbackSemaphore );
        User::LeaveIfError( err );
        }   
    }
   
    
// ---------------------------------------------------------------------------
// We handle diconnect by finding client's chunk, closing it and then
// removing the entry from chunk array.
//
// #1 Find the correct entry
// #2 Close chunk
// #3 Remove entry from chunk array
// ---------------------------------------------------------------------------
//
EXPORT_C void CTactileAreaRegistry::HandleDisconnect( 
    const TTactileFeedbackDisconnectData& aData )
    {
    // #1
    iTactileSemaphore.Wait();
    
    for ( TInt i=0; i < iChunkArray.Count(); i++ )
        {
        if ( iChunkArray[i].iWindowGroupId == aData.iWindowGroupId )
            {
            // #2
            iChunkArray[i].iChunk.Close();
            
            // #3
            iChunkArray.Remove( i );            
            break;
            }
        }  
    
    iTactileSemaphore.Signal();
    }
    

// ---------------------------------------------------------------------------
// Here we just store information (e.g. connection handle) about the
// newly created window group.
//
// Notice that we ignore possible error on purpose when adding new item to
// the array. Firstly a failure should be almost impossible, and secondly 
// it would only cause feedback to be disabled for the newly created 
// window group.
// ---------------------------------------------------------------------------
//
EXPORT_C void CTactileAreaRegistry::HandleWindowGroupCreated( 
    TInt aIdentifier, TUint aConnectionHandle )
    {
    TTactileWgroupItem newItem;
    
    newItem.iWindowGroupId    = aIdentifier;
    newItem.iConnectionHandle = aConnectionHandle;
    // if append fail just make its action like before
    TRAP_IGNORE( iWgArray.AppendL( newItem ) );
    }
    
    
// ---------------------------------------------------------------------------
// When window group is closed, we remove all information about it from
// the array (we don't destroy chunks because it has been done based
// on the disconnect request already).
// ---------------------------------------------------------------------------
//
EXPORT_C void CTactileAreaRegistry::HandleWindowGroupClosed(
    TInt aIdentifier )
    {
    // Loop down so that deleting of items won't mix up the array
    for ( TInt i = iWgArray.Count()-1; i >= 0; i-- ) 
        {
        if ( iWgArray[i].iWindowGroupId == aIdentifier )
            {
            iWgArray.Remove( i );
            }
        }
    }
    
    
// ---------------------------------------------------------------------------
// This is the implementation of hit testing.
//
// It is assumed that this function is not called unless there is at least
// one connected application, and unless iTactileSemaphore has been opened 
// succesfully.
//
// #1 Check that we have valid handle to global semaphore.
// #2 Find correct chunk based on window group id.
// #3 Do actual hit testing in a separate function.
// #4 Return feedback type based on hit testing results.
// ---------------------------------------------------------------------------
//
EXPORT_C TTouchLogicalFeedback CTactileAreaRegistry::HitTestPointerEvent( 
    const TPointerEvent& aPointerEvent,
    TInt aWgIdentifier,
    TUint32 aWindowHandle )
    {
    TTouchLogicalFeedback feedback(ETouchFeedbackNone);
    
    OstTrace0( TACTILE_PERFORMANCE, TACTILE_REGISTRY_HIT_TEST_1, "e_TACTILE_REGISTRY_HIT_TEST 1");

    // #1
    if ( iTactileSemaphore.Handle() && 
         ( aPointerEvent.iType == TPointerEvent::EButton1Down || 
           aPointerEvent.iType == TPointerEvent::EButton1Up ) )
        {
        // We keep performance trace here instead of beginning of function
        // so that drag events won't confuse performance measurements.    
        
        // #2
        TInt chunkIndex = ChunkIndexByWindowGroupId( aWgIdentifier );
        
        // #3 If we found the window group where pointer event is going to land,
        // then search for correct window (and area) in its shared chunk.
        if ( chunkIndex >= 0 && chunkIndex < iChunkArray.Count() )   
            {
            feedback = HitTestChunk( 
                iChunkArray[chunkIndex].iChunk, 
                aPointerEvent,
                aWgIdentifier,
                aWindowHandle );
            }
         }
    
    OstTrace0( TACTILE_PERFORMANCE, TACTILE_REGISTRY_HIT_TEST_0, "e_TACTILE_REGISTRY_HIT_TEST 0");
    
    // #4
    return feedback;
    }
      
// ---------------------------------------------------------------------------
// In this function we do hit testing for one chunk.
//
// There is currently support for feedback on both
// pen down and up -events.
//
// #1 Reset last pen down information in case this was a pen down -event.
// #2 Call wait on global semaphore for mutual exlusion of shared memory.
// #3 Start from the beginning of chunk, and read number of windows first.
// #4 Go through all windows in a loop
// ---------------------------------------------------------------------------
//
TTouchLogicalFeedback CTactileAreaRegistry::HitTestChunk( 
    RChunk& aChunk, 
    const TPointerEvent& aPointerEvent,
    TInt aWgIdentifier, 
    TUint32 aWindowHandle )
    {
    // #1
    if ( aPointerEvent.iType == TPointerEvent::EButton1Down )
        {
        iLastPenDown.Reset();
        }
    
    TTouchLogicalFeedback feedback = ETouchFeedbackNone;
    
    // #2 Protect shared memory chunks so that nobody can modify
    // them in the middle of hit testing.
    iTactileSemaphore.Wait();

    // #3
    TInt* base = (TInt*) aChunk.Base();  
    
    TInt windowCount = *base;
    base++;
    
    TBool windowFound(EFalse);
    
    // #4 Iterate though windows
    for ( TInt i=0; i < windowCount && !windowFound; i++ )
        {
        // Read handle identifier of this window
        TInt wsHandle = *base;
        base++;
        // Read number of areas registered to this window
        TInt areaCount = *base;
        base++;
        // Read offset to the area data of this window
        TInt offset = *base;
        base++; 
       
        // Check if this window is the one where pointer event hit.
        if ( wsHandle == static_cast<TInt>( aWindowHandle ) ) 
            {
            // Set pointer to the first area belonging to this window.
            TFeedbackChunkAreaEntry* entryPtr = 
                reinterpret_cast<TFeedbackChunkAreaEntry*>( aChunk.Base() + offset );

            TBool matchFound = EFalse;
            
            // Go through all areas and test each one against the pointer
            // event as long as a match is found.
            for ( TInt j = 0; j < areaCount && !matchFound; j++ )
                {
                matchFound = HitTestRegistryEntry( aPointerEvent,
                                                   *entryPtr,
                                                   aWgIdentifier,
                                                   aWindowHandle,
                                                   feedback );
                            
                entryPtr++;
                }
                
            // No need to continue in the loop, 
            // because window found already
            windowFound = ETrue;
            }   
        }
        
    // Release the semaphore so that applications can do updates to
    // registry again.   
    iTactileSemaphore.Signal();  
    
    return feedback;  
    }
    
    
// ---------------------------------------------------------------------------
// Here we analyse the pointer event type against feedback area entry, that
// is located in same window where the pointer event hit.
//
// This functionality is encapsulated into separate function mainly
// because this is the only part of the implementation that needs
// to be modified in case we'll have different kinds of down- and up event
// combinations (there could be different types for e.g. in situations were
// up event is / is not required to match down event for it to trigger 
// feedback).
//
// Here we also record details of pen down events (that trigger feedback), so
// that we can check on pen up event in case it matches the same feedback 
// area where down -event landed.
//
// #1 First see if pointer event even hit the area
//
// #2 Currently pen down event always generates feedback
// 
// #3 We trigger feedback on pen up event only if it matches the same
//    feedback area, where corresponding down event hit.
//    
// ---------------------------------------------------------------------------
//
TBool CTactileAreaRegistry::HitTestRegistryEntry( 
    const TPointerEvent&           aPointerEvent, 
    const TFeedbackChunkAreaEntry& aEntry,
    TInt                           aWgIdentifier, 
    TUint32                        aWindowHandle,
    TTouchLogicalFeedback&         aFeedback )
    {
    TBool matchFound = EFalse;
    
    // #1
    if ( aEntry.iRect.Contains( aPointerEvent.iPosition) )
        {
        TInt enablers     = aEntry.iFeedbackType & ( KTactileVibraBitDown | 
                                                     KTactileAudioBitDown | 
                                                     KTactileVibraBitUp   | 
                                                     KTactileAudioBitUp );
        TInt feedbackDown = aEntry.iFeedbackType & 0x3FF;         // the first 10 bits
        TInt feedbackUp   = aEntry.iFeedbackType & (0x3FF << 10); // next 10 bits
        feedbackUp   = feedbackUp >> 10;
        // #2 Pointer down on the area always triggers feedback     
        if ( ( aPointerEvent.iType == TPointerEvent::EButton1Down ))
            {
            matchFound = ETrue;
            aFeedback = static_cast<TTouchLogicalFeedback>( feedbackDown );
            iLastPenDown = 
                TTactilePenDownEvent(
                    aWgIdentifier, aWindowHandle, aEntry.iRect );
            }
        // #3    
        else if ( ( aPointerEvent.iType == TPointerEvent::EButton1Up ))
            {
            if ( iLastPenDown.iWindowGroupId == aWgIdentifier &&
                 iLastPenDown.iWindowHandle == aWindowHandle && 
                 iLastPenDown.iFeedbackArea == aEntry.iRect )
                {
                matchFound = ETrue;
                aFeedback = static_cast<TTouchLogicalFeedback>( feedbackUp );

                // Can only match agains same pen down event once
                iLastPenDown.Reset();            
                }
            }
        if ( matchFound )
            {
            aFeedback = static_cast<TTouchLogicalFeedback>(aFeedback | enablers);
            }
        }
    return matchFound;
    }
    
    
// ---------------------------------------------------------------------------
// Find the chunk of that application, where pointer event hit.
//
// #1 First determine the window server client connection handle (we use
//    this to indentify chunk instead of window group id, because this way
//    resolving works the same way also for additional window groups)
//
// #2 Go though the chunk array for finding out the correct one.
// ---------------------------------------------------------------------------
//
TInt CTactileAreaRegistry::ChunkIndexByWindowGroupId( TInt aWgIdentifier ) const
    {
    TInt chunkIndex = KErrNotFound;
    
    // #1
    TUint connectionHandle = ConnectionHandleByWgId( aWgIdentifier );

    // #2 Iterate though chunks to find the correct one
    for ( TInt chunk = 0; chunk < iChunkArray.Count() && chunkIndex == KErrNotFound; chunk++ ) 
        {
        if ( iChunkArray[chunk].iConnectionHandle == connectionHandle )
            {
            chunkIndex = chunk;
            }
        }
            
    return chunkIndex;
    }


// ---------------------------------------------------------------------------
// Here we scan through the window group array, and return the corresponding
// connection handle in case a match is found.
// ---------------------------------------------------------------------------
//
TUint CTactileAreaRegistry::ConnectionHandleByWgId( TInt aWgIdentifier ) const
    {
    TUint connectionHandle = 0;
    
    for ( TInt i=0; i < iWgArray.Count() && !connectionHandle; i++ )
        {
        if ( iWgArray[i].iWindowGroupId == aWgIdentifier )
            {
            connectionHandle = iWgArray[i].iConnectionHandle;
            }
        }
        
    return connectionHandle;
    }
   
    
    

