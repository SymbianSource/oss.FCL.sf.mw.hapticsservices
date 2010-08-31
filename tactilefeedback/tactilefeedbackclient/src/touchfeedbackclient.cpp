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
* Description:  Communication with server side implementation of
*                Area Registry.
* Part of:      Tactile Feedback.
*
*/

#include <e32std.h>
#include <eikenv.h>

#include <tactileinternaldatatypes.h>
#include <tactilefeedbacktrace.h>

#include "touchfeedbackclient.h"
#include "touchfeedbackimpl.h"
#include "touchfeedbackregistry.h"
#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "touchfeedbackclientTraces.h"
#endif

// Minimun and maximum sized of one shared memory chunk.
const TInt KTactileChunkInitialSize = 4096; // 4kB
const TInt KTactileChunkMaxSize     = 262144; // 256kB
const TUid KTactileClickPluginUid   = { 0x2000B493 };
// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
CTouchFeedbackClient::CTouchFeedbackClient( CTouchFeedbackImpl& aFeedback ):
    iClickPlugin ( CEikonEnv::Static()->WsSession()),
    iFeedback( aFeedback )
    {
    }

// ---------------------------------------------------------------------------
// We do all the initializations in the construction phase. Other option
// would be to postpone these until the first registry entry has been added.
//
// #1 Connect to our ClickPlugin
// #2 Create a shared chunk, which is used for storing this application
//    process' area registry, so that it can be accessed by the window
//    server.
// #3 Set window count to zero in the new chunk
// #4 Try to create a global semaphore to have mutual exclusion on chunks
//    If semaphore already exists, then try to open it.
// #5 Put our chunk name to package buffer, and send it to click plugin
//    so that it can open a handle to our chunk and thus access the shared
//    memory.
// #6 Create a CIdle object that is used for updating the registry in shared
//    memory.
//
// Notice that we will leave in case anything fails. This has to be handled
// on higher level, so that whole application launch will not fail in case
// touch feedback does not work.
// ---------------------------------------------------------------------------
//
void CTouchFeedbackClient::ConstructL()
    {
    TRACE("CTouchFeedbackClient::ConstructL - Begin");
    
    // #1 Connect to click plugin
    User::LeaveIfError( iClickPlugin.Construct( KTactileClickPluginUid ) );
        
    TBool changable = EFalse;
    
    TBool loaded = iClickPlugin.IsLoaded( changable );  
    
    if ( !loaded )
        {
        User::Leave( KErrNotSupported );
        }

    // #2 Create shared chunk 
    RThread me;

    TTactileFeedbackConnectData data;
    
    // Set window group identifier
    data.iWindowGroupId = CCoeEnv::Static()->RootWin().Identifier();
           
    // We use our own thread name as name for the chunk       
    data.iChunkName.Copy( me.Name().Right( KMaxKernelName ) );
    
    // Now create the chunk
    TInt err = iChunk.CreateGlobal( data.iChunkName, 
                                    KTactileChunkInitialSize, 
                                    KTactileChunkMaxSize );     
        
    // We have to take into account, that the chunk may already exist.
    // This may happen in case we have been started (and crashed)
    // once already.
    if ( err == KErrAlreadyExists )
        {
        User::LeaveIfError( iChunk.OpenGlobal( data.iChunkName, EFalse ) );
        }
    else if( err != KErrNone )
        {
        User::LeaveIfError( err );
        }
    
    // #3 Set window count to zero
    TInt* chunkPtr = reinterpret_cast<TInt*>( iChunk.Base() );
    
    *chunkPtr = 0; // No entries in the chunk in the beginning
    
    // #4 Try to create/open global semaphore
    err = iSemaphore.CreateGlobal( KTouchFeedbackSemaphore, 1 );
    
    if ( err == KErrAlreadyExists )
        {
        User::LeaveIfError( iSemaphore.OpenGlobal( KTouchFeedbackSemaphore ) );
        }
    else if( err != KErrNone )
        {
        User::LeaveIfError( err );
        }
       
    // #5 Send chunk name to window server
    TPckgC<TTactileFeedbackConnectData> dataPkg ( data );
    
    User::LeaveIfError( iClickPlugin.CommandReply( ETactileOpCodeConnect, 
                                                   dataPkg ) );
    iConnected = ETrue;
    
    iFeedbackTimer = CPeriodic::NewL( CActive::EPriorityStandard );
        
    User::LeaveIfError( iFbClient.Connect() );
            
    // #6 We use Standard priority so that we won't disturb the normal 
    //    functionality of the application, but can get registry updated
    //    fastly enough in case the application does very plenty of 
    //    background prosessing with active objects.
    iIdle = CIdle::NewL( CActive::EPriorityStandard );
    
    TRACE("CTouchFeedbackClient::ConstructL - End");
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
CTouchFeedbackClient* CTouchFeedbackClient::NewL( CTouchFeedbackImpl& aFeedback )
    {
    CTouchFeedbackClient* self = 
        new( ELeave ) CTouchFeedbackClient ( aFeedback );
    CleanupStack::PushL( self );
    self->ConstructL();    
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
CTouchFeedbackClient::~CTouchFeedbackClient()
    {
    iChunk.Close();
    iSemaphore.Close();

    if ( iFeedbackTimer )
        {
        iFeedbackTimer->Cancel();
        delete iFeedbackTimer;    
        }
    
    iFbClient.Close();
   
    if ( iConnected )
        {
        TTactileFeedbackDisconnectData data;
        
        data.iWindowGroupId = CCoeEnv::Static()->RootWin().Identifier();
        
        TPckgC<TTactileFeedbackDisconnectData> dataPkg ( data );
    
        iClickPlugin.CommandReply( ETactileOpCodeDisconnect, dataPkg );    
        }
    
    iClickPlugin.Close();  
    
    delete iIdle;  
    }
   
    
// ---------------------------------------------------------------------------
// In this function we just activate CIdle. The motivation is that this
// way we avoid updating the registry multiple times. For e.g. Calculator
// application launch would cause around 20 registry updates in case we would
// always make changes to shared memory immediately after registry update.
// ---------------------------------------------------------------------------
//
void CTouchFeedbackClient::RegistryChanged( )
    {
    if ( iIdle && !iIdle->IsActive() )
        {
        iIdle->Start( TCallBack( IdleCallbackL, this ) ); 
        }
    }

// ---------------------------------------------------------------------------
// Callback for CIdle.
//
// We check the current enabled/disabled status of tactile feedback for
// this application, and either UpdateRegistryToChunkL for updating whole
// registry to shared chunk, or ClearWholeChunkL for clearing all feedback
// areas from the shared chunk.
// ---------------------------------------------------------------------------
//
TInt CTouchFeedbackClient::IdleCallbackL( TAny* aPtr )
    {
    if ( aPtr )
        {
        CTouchFeedbackClient* self =
            static_cast<CTouchFeedbackClient*> ( aPtr );

        if ( self->iFeedback.FeedbackEnabledForThisApp() )
            {
            self->UpdateRegistryToChunkL();
            }
        else
            {
            self->ClearWholeChunkL();
            }
        }
    
    return KErrNone;
    }

// ---------------------------------------------------------------------------
// Area registry is written to shared memory in following format
//
// + Header (four bytes) = Number of windows as TInt
// + Window entries 
// + Area registry entries
// 
// Window entries: For each window:
//   [four bytes] = Window handle 
//   [four bytes] = Number of registry entries in this window 
//   [four bytes] = Offset of registry data from beginning of chunk
// Area registry entries:
//   - Each entry written as TFeedbackChunkAreaEntry straight into memory
//
//
// Steps taken in this function:
//
// #1 Calculate the size needed for area registry data
// #2 Mutual exclusion with global semaphore
// #3 Increase Chunk size in case necessary
// #4 Update Chunk contents. This is now done simply by writing the whole
//    area registry into the shared memory.
// #5 Release semaphore
// ---------------------------------------------------------------------------
//
void CTouchFeedbackClient::UpdateRegistryToChunkL()
    {
    TRACE( "CTouchFeedbackClient::UpdateRegistryToChunkL - Begin" );
    
    RPointerArray<CTouchFeedbackRegistry>* registry =
        iFeedback.RegistryArray();

    // Do cleanup so that empty registry instances are not hanging anymore
    TInt windowIndex = 0;
    TInt cleanupAreaCount = 0;
    TInt cleanupWindowCount = 0;
    // value is returned in parameter variable  
    iFeedback.GetAreaCount( cleanupAreaCount, cleanupWindowCount );  
    
    while ( windowIndex < cleanupWindowCount )
        {
        TBool emptyWindow = ( (*registry)[windowIndex]->AreaCount() == 0 ); 
        if ( emptyWindow )
            {
            // Remove empty/possibly destroyed registry instance that possibly 
            // may refer to destroyed control/window.
            CTouchFeedbackRegistry* feedbackRegistry = (*registry)[windowIndex];
            registry->Remove( windowIndex );
            delete feedbackRegistry;
            cleanupWindowCount--;
            }
        else
            {
            windowIndex++; // to next window
            }
        }
        
    // #1 Calculate the size needed    
    TInt windowCount(0);
    TInt areaCount(0);
    iFeedback.GetAreaCount( areaCount, windowCount );    
        
    // Header contains only number of windows in this registry
    TInt headerSize = sizeof( TInt );     
        
    // There is a list of windows in this registry in the beginning, with
    // data offsets in the chunk    
    TInt windowItemSize = 
        sizeof( TInt ) + // Window handle
        sizeof( TInt ) + // Area count
        sizeof( TInt );  // Offset to area registry entries
    
    // Size of one area registry entry
    TInt areaItemSize = sizeof( TFeedbackChunkAreaEntry );    
           
    TInt sizeNeeded =     
        headerSize + 
        windowCount * windowItemSize + 
        areaCount * areaItemSize;
        
    // #2 Mutual exclusion    
    iSemaphore.Wait();

     // We use cleanup item for making sure that semaphore will not stay
    // reserved in case there is a leave.
    CleanupStack::PushL( TCleanupItem( CleanupSemaphore, &iSemaphore ) );
       
    // #3 Increase chunk (committed) size in case needed    
    if ( iChunk.Size() < sizeNeeded )
        {
        TInt err = iChunk.Adjust( sizeNeeded );
        
        if ( err != KErrNone )
            {
            // In case the current registry does not fit to the chunk, then
            // we'll set window count to zero, in case that is possible
            // (it is not possible if chunk size is zero).
            // This means that there won't be any area registry based 
            // feedback for this application on the moment.
            if ( iChunk.Size() > 0 )
                {
                TInt* chunkPtr = reinterpret_cast<TInt*>( iChunk.Base() );
                    
                // Set window count to zero   
                *chunkPtr = 0;           
                }
                
            // We will leave anyway
            User::Leave( err );
            }
        }
        
        
    // #4 Update chunk contents    
    TInt* chunkPtr = reinterpret_cast<TInt*>( iChunk.Base() );
        
    // Set window count in the beginning of chunk    
    *chunkPtr = windowCount; 
    chunkPtr++;
            
    TInt areaOffset = headerSize + windowCount * windowItemSize;
    
    // This is pointer to the first area registry entry in the chunk
    TFeedbackChunkAreaEntry* chunkEntryPtr 
       = reinterpret_cast<TFeedbackChunkAreaEntry*> ( iChunk.Base() + areaOffset );
    
    // Check if audio or vibra is disabled for this application.
    TBool audioEnabled = iFeedback.FeedbackEnabledForThisApp( ETouchFeedbackAudio );
    TBool vibraEnabled = iFeedback.FeedbackEnabledForThisApp( ETouchFeedbackVibra );
            
    // One loop round for each window where we have area registry entries.
    for ( TInt windowIndex = 0; windowIndex < windowCount; windowIndex++ )
        {
        RArray<TFeedbackEntry>* windowRegistry = 
            (*registry)[windowIndex]->WindowRegistry();
        
        // Write handle of this window
        *chunkPtr = (*registry)[windowIndex]->WindowHandle();
        chunkPtr++;
        // Store the address where we shall write count later
        // (We don't know the amount of areas yet, as we shall
        // only add areas of visible controls).
        TInt* countPointer = chunkPtr;
        chunkPtr++;
        // Write offset of the area registry entries of this window
        *chunkPtr = ((TInt)chunkEntryPtr-(TInt)iChunk.Base());
        chunkPtr++;
        
        // Write all areas of visible controls in this window 
        // to the shared memory. Invisible controls' areas are left out so
        // that they won't mess up the feedback for overlapping visible
        // controls.
        TInt visibleAreaCount = 0;
        for ( TInt areaIndex = windowRegistry->Count()-1; areaIndex >= 0; areaIndex-- )
            {
            TFeedbackEntry entry = (*windowRegistry)[areaIndex];
            
            if ( entry.iVisible ) 
                {
            chunkEntryPtr->iRect = entry.iRect;
            
            TInt feedback = ( entry.iFeedbackTypeUp << 10 );
            feedback |= entry.iFeedbackTypeDown;            

            // Add audio and vibra information to feedback type
            if ( entry.iVibraEnabled && vibraEnabled )
                {
                //feedback |= KTactileVibraBit;    
                if ( entry.iFeedbackDown & ETouchFeedbackVibra )
                    {
                    feedback |= KTactileVibraBitDown;    
                    }
                if ( entry.iFeedbackUp & ETouchFeedbackVibra )
                    {
                    feedback |= KTactileVibraBitUp;    
                    }
                
                }
            if ( entry.iAudioEnabled && audioEnabled )
                {
                //feedback |= KTactileAudioBit;
                if ( entry.iFeedbackDown & ETouchFeedbackAudio )
                    {
                    feedback |= KTactileAudioBitDown;    
                    }
                if ( entry.iFeedbackUp & ETouchFeedbackAudio )
                    {
                    feedback |= KTactileAudioBitUp;    
                    } 
                }
                
            chunkEntryPtr->iFeedbackType = static_cast<TTouchLogicalFeedback>( feedback );
            
            chunkEntryPtr->iEventType    = entry.iEventType;    
            
            chunkEntryPtr++;        
                visibleAreaCount++;      
                }            
            }
        // Now store area count as we know the amount of visible areas
        // for this window.    
        *countPointer = visibleAreaCount;     
        }   
        
    // #5 Release semaphore so that other processes can access the chunks again    
    // This calls "Signal" on the semaphore
    CleanupStack::PopAndDestroy(&iSemaphore);

    TRACE( "CTouchFeedbackClient::UpdateRegistryToChunkL - End" );
    }

// ---------------------------------------------------------------------------
// Despite the function name, we don't actually clear whole chunk here
// as that is not necessary. It is enough to set the window count to zero
// at beginning of chunk, as then server will consider this chunk empty.
//
// #1 Mutual exclusion
// #2 Make sure chunk is not of zero -size
// #3 Set window count to zero at beginning of chunk
// #4 Release mutual exclusion
// ---------------------------------------------------------------------------
//
void CTouchFeedbackClient::ClearWholeChunkL()
    {
    TRACE( "CTouchFeedbackClient::ClearWholeChunkL - Begin" );
    
    // #1 Mutual exclusion    
    iSemaphore.Wait();
    
    // We use cleanup item for making sure that semaphore will not stay
    // reserved in case there is a leave.
    CleanupStack::PushL( TCleanupItem( CleanupSemaphore, &iSemaphore ) );
    
    // We only need four bytes for the data, as we are only going to store
    // one number to the chunk. But we count the space just in case anyway, 
    // because at some point chunks initial size may be change so that it is
    // zero.
    TInt sizeNeeded = sizeof( TInt );
        
    // #2 Increase chunk (committed) size in case needed    
    if ( iChunk.Size() < sizeNeeded )
        {
        User::LeaveIfError( iChunk.Adjust( sizeNeeded ) );
        }
  
      // #3 Update chunk contents    
    TInt* chunkPtr = reinterpret_cast<TInt*>( iChunk.Base() );

    // Set window count to zero (this is enough for the server to think
    // that there are no feedback areas at all).
    *chunkPtr = 0; 

    // #4 Release semaphore
    CleanupStack::PopAndDestroy(&iSemaphore);         
    
    TRACE( "CTouchFeedbackClient::ClearWholeChunkL - End" );
    }

// ---------------------------------------------------------------------------
// This is a cleanup function for releasing a semaphore in case there will
// be a leave after calling RSemaphore::Wait (otherwise there could easily
// be a deadlock in the system).
// ---------------------------------------------------------------------------
//
void CTouchFeedbackClient::CleanupSemaphore( TAny* aPtr )
    {
    TRACE( "CTouchFeedbackClient::CleanupSemaphore" );
    
    if ( aPtr )
        {
        RSemaphore* sem = static_cast<RSemaphore*> ( aPtr );
        
        sem->Signal();
        }    
    }

// ---------------------------------------------------------------------------
// Here we just send the logical feedback type to server.
// ---------------------------------------------------------------------------
//
void CTouchFeedbackClient::ImmediateFeedback( TTouchLogicalFeedback aType,  
    TBool aVibraOn, TBool aAudioOn  )
    {
    OstTrace0( TACTILE_PERFORMANCE, TACTILE_CLIENT_INSTANT_FEEDBACK_1, "e_TACTILE_CLIENT_INSTANT_FEEDBACK 1");
    
    if ( aVibraOn || aAudioOn )
        {
        iFbClient.PlayFeedback( aType, aVibraOn, aAudioOn );
        }
    
    OstTrace0( TACTILE_PERFORMANCE, TACTILE_CLIENT_INSTANT_FEEDBACK_0, "e_TACTILE_CLIENT_INSTANT_FEEDBACK 0");
    }

// ---------------------------------------------------------------------------
// Check that we really have something to update, and then do the update.
// ---------------------------------------------------------------------------
//
void CTouchFeedbackClient::FlushUpdates()
    {
    if ( iIdle && iIdle->IsActive() )
        {
        // Do the updates
        TRAP_IGNORE( IdleCallbackL( this ) );
        
        // Cancel pending update request (no need to keep it active since
        // registry is now up-to-date).
        iIdle->Cancel();
        }
    }
    
// ----------------------------------------------------------------------------
// 
// ----------------------------------------------------------------------------
//
void CTouchFeedbackClient::StartFeedback( TUint32 aClientHandle,
                                          TTouchContinuousFeedback aType,
                                          TInt aIntensity,
                                          TTimeIntervalMicroSeconds32 aTimeout )
    {
    TRACE("CTouchFeedbackClient::StartFeedback - Begin");

    if ( !iFeedbackTimer->IsActive() )
        {
        iPreviousIntensity = aIntensity;
        iFbClient.StartFeedback( aType, aIntensity );
        iClientHandle = aClientHandle; // Control, which started the feedback.
        }
    if ( aTimeout != TTimeIntervalMicroSeconds32(0) )
        {
        StartFeedbackTimer( aTimeout );
        
        if ( aIntensity != iPreviousIntensity )
            {
            ModifyFeedback( aClientHandle, aIntensity );
            }
        }
    
    TRACE("CTouchFeedbackClient::StartFeedback - End");
    }
    
// ----------------------------------------------------------------------------
// 
// ----------------------------------------------------------------------------
//
void CTouchFeedbackClient::ModifyFeedback( TUint32 aClientHandle, 
                                           TInt aIntensity )
    {
    // Modification is allowed only for the same control, which started 
    // the feedback.
    if ( aClientHandle == iClientHandle &&
         aIntensity != iPreviousIntensity )
        {
        iPreviousIntensity = aIntensity;
        iFbClient.ModifyFeedback( aIntensity );    
        }    
    }
                             
// ----------------------------------------------------------------------------
// 
// ----------------------------------------------------------------------------
//
void CTouchFeedbackClient::StopFeedback( const TUint32 aClientHandle )
    {
    TRACE("CTouchFeedbackClient::StopFeedback - Begin");
    if ( aClientHandle == iClientHandle )
        {
        iFbClient.StopFeedback();
        iFeedbackTimer->Cancel();
        // Clear also client handle to indicate there's no ongoing feedback.
        iClientHandle = 0; 
        }
    TRACE("CTouchFeedbackClient::StopFeedback - End");
    }    

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TInt CTouchFeedbackClient::SetFeedbackEnabledForDevice( TTouchFeedbackType aFeedbackType )
    {
    return iFbClient.SetFeedbackEnabledForDevice( aFeedbackType );
    }

// ----------------------------------------------------------------------------
// 
// ----------------------------------------------------------------------------
//
TTouchFeedbackType CTouchFeedbackClient::FeedbackEnabledForDevice()
    {
    TTouchFeedbackType feedbackEnabled;
    iFbClient.FeedbackEnabledForDevice( feedbackEnabled );
    return feedbackEnabled;
    }

// ----------------------------------------------------------------------------
// 
// ----------------------------------------------------------------------------
//
void CTouchFeedbackClient::StartFeedbackTimer( TTimeIntervalMicroSeconds32 aTimeout )
    {
    iFeedbackTimer->Cancel();
    TCallBack callback( StopFeedbackCallback, this );
    iFeedbackTimer->Start( aTimeout, 0, callback );
    }    

// ----------------------------------------------------------------------------
// 
// ----------------------------------------------------------------------------
//
void CTouchFeedbackClient::StopFeedbackByTimeout()
    {
    iFeedbackTimer->Cancel();
    iFbClient.StopFeedback();
    iClientHandle = 0; 
    }

// ----------------------------------------------------------------------------
// CAknSlider::StopFeedbackCallback
// ----------------------------------------------------------------------------
//
TInt CTouchFeedbackClient::StopFeedbackCallback( TAny* aThis )
    {
    TRACE("CTouchFeedbackClient::StopFeedbackCallback - Begin");
    static_cast<CTouchFeedbackClient*>( aThis )->StopFeedbackByTimeout();
    TRACE("CTouchFeedbackClient::StopFeedbackCallback - End");
    return KErrNone;
    }    

// End of File
