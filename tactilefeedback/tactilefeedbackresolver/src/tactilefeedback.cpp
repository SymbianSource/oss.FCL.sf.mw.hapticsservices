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
* Description:  Client side of Tactile Feedback server.
* Part of:      Tactile Feedback.
*
*/

#include <e32base.h>
#include <e32std.h>
#include <ecom/ecom.h>
#include <e32debug.h>

#include <tactilefeedback.h>

#include "tactilefeedbacktrace.h"
#include "tactilefeedbacksrv.h"
#include "tactilefeedbackdefs.h"

/**
 * Own active scheduler class. We need this so that we can override the
 * CActiveScheduler::Error -function and avoid panic in case something
 * leaves.
 */
NONSHARABLE_CLASS( CTactileScheduler ): public CActiveScheduler
    {
public:   

    /**
     * This is called when something leaves in our Feedback Player
     * thread.
     */
    void Error( TInt aError ) const;
    };


void CTactileScheduler::Error( TInt aError ) const
    {
    (void) aError; // Remove compiler warning in armv5 urel
    
    TRACE2( "CTactileScheduler::Error, aError = %d", aError );
    }

/**
 * Structure for the parameters, that are passed to Feedback Player Thread
 * when it is created.
 */
struct TTactileThreadParams
    {
    RThread* iOwningThread;
    TInt iQueueHandle;
    };
    
/**
 * Class for creating new thread for Tactile Feedback playing, and
 * for passing feedback requests to the thread via a message queue.
 */
NONSHARABLE_CLASS( CThreadPlayer ): public CBase
    {
public:
    static CThreadPlayer* NewL();
    ~CThreadPlayer();

private:
    CThreadPlayer() {};
    
    void ConstructL();
    
    static void RunThreadL( TAny* aParam );

    static TInt ThreadEntryPoint( TAny* aParam );

    RThread                 iThread;
    };    

// ---------------------------------------------------------------------------
// Here we create and start new thread for Tactile Feedback Playing.
//
// If this function returns succesfully, then the new thread was created ok
// and is now running. Otherwise we leave from here.
//
// ---------------------------------------------------------------------------
//
void CThreadPlayer::ConstructL()
    {
    TRACE("CThreadPlayer::ConstructL - Begin");
    
    RThread thisThread;

    CleanupClosePushL( thisThread );
    
    TTactileThreadParams params;
    params.iOwningThread = &thisThread;
 
    User::LeaveIfError( 
        iThread.Create( 
            KTactileThreadName, 
            ThreadEntryPoint,
            KPlayerThreadStackSize, 
            KPlayerThreadHeapMinSize, 
            KPlayerThreadHeapMaxSize,
            &params, 
            EOwnerProcess ) );       
                        
    // With EPriorityMore feedback player thread operates with the same
    // priority as window server main thread. This seems to be the best
    // choice for now, as this way neither drawing or feedback can cause
    // significant delay for each other.
    iThread.SetPriority( EPriorityMore );
    iThread.Resume();
    
    // We have to wait so that our thread has read the parameters
    TRequestStatus status;
    iThread.Rendezvous( status );
    User::WaitForRequest( status );
    
    // Store thread exit type while we still have handles open
    TExitType exitType = iThread.ExitType(); 

    // Close thread handle to ourself.  
    CleanupStack::PopAndDestroy( &thisThread ); 
    
    // Check that rendezvouz succeeded, and that thread is still alive
    if ( exitType != EExitPending || status != KErrNone )
        {
        TRACE3("CThreadPlayer::ConstructL - Thread creation failed, %d, %d", exitType, status.Int() );
        
        User::Leave( KErrGeneral );
        }
    
    TRACE("CThreadPlayer::ConstructL - End");
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
CThreadPlayer* CThreadPlayer::NewL()
    {
    CThreadPlayer* self = new( ELeave ) CThreadPlayer();
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// We close our player thread by sending an exit request and waiting for
// the thread to close itself. This way all resources get closed cleanly.
//
// In case message queue is full, then we'll just kill the thread.
// ---------------------------------------------------------------------------
//
CThreadPlayer::~CThreadPlayer()
    {
    if ( iThread.Handle() )
        {
        TExitType exitType = iThread.ExitType(); 
        
        if ( exitType == EExitPending )
            {
            iThread.Kill( KErrAbort );
            }
            
        iThread.Close();
        }
    }
    
// ---------------------------------------------------------------------------
// Rest of the actions required for setting Tactile Feedback Player thread
// up and running (this is called from ThreadEntryPoint)
//
// #1 Create and install an active scheduler
// #2 Create server instance.
// #3 Notify main thread that we are ready to run (this allows main thread 
//    to continue).
// #4 Start the scheduler. Excecution will halt here in this functio until
//    somebody stops the scheduler.
//
// #5 Delete server and scheduler (at this point we are exiting our thread 
//    already).
//
// #6 Close ECom session, as we are now exiting the thread (this is on our
//    responsibility even though we are not directly using Ecom).
// ---------------------------------------------------------------------------
//
void CThreadPlayer::RunThreadL( TAny* /*aParam*/ )
    {
    // #1
    CTactileScheduler* s = new(ELeave) CTactileScheduler;
    CleanupStack::PushL(s);
    CActiveScheduler::Install(s);

    // #2
    CTactileFeedbackSrv* svr = CTactileFeedbackSrv::NewL();

    // #3
    RThread me;
    me.Rendezvous( KErrNone ); 
    
    // #4    
    CActiveScheduler::Start();

    // #5
    delete svr;
    CleanupStack::PopAndDestroy( s );

    // #6
    REComSession::FinalClose();
    }
        
// ---------------------------------------------------------------------------
// This is the entry point of Tactile feedback player thread.
//
// Standard operation: Create cleanup stack and call rest of the code under
// trap harness.
// ---------------------------------------------------------------------------
//
TInt CThreadPlayer::ThreadEntryPoint( TAny* aParam )
    {
    __UHEAP_MARK;
    CTrapCleanup* cleanup = CTrapCleanup::New();
    TInt err( KErrNoMemory );
    if ( cleanup )
        {
        TRAP( err, CThreadPlayer::RunThreadL( aParam ) );
        delete cleanup;
        }
    __UHEAP_MARKEND;
    return err;
    }    
    
// --------------------------------------------------------------------------
// Default constructor.
// 
// --------------------------------------------------------------------------
//       
EXPORT_C RTactileFeedback::RTactileFeedback() 
    {
    }

// --------------------------------------------------------------------------
// 
// Creates connection to server
// --------------------------------------------------------------------------
//
EXPORT_C TInt RTactileFeedback::Connect()
    {
    TInt ret = CreateSession( KTactileFeedbackServer, Version() );
    if (  ret != KErrNone )
        {
        TRAP( ret, iThread = CThreadPlayer::NewL() );
        if ( ret == KErrNone )
            {
            ret = CreateSession( KTactileFeedbackServer, Version() );    
            }        
        }
    
    return ret; 
    }
    
// --------------------------------------------------------------------------
// 
// --------------------------------------------------------------------------
//      
EXPORT_C void RTactileFeedback::Close()
    {
    delete iThread;
    iThread = NULL;
    // close session    
    RSessionBase::Close();
    }    

// --------------------------------------------------------------------------
// 
// --------------------------------------------------------------------------
//     
EXPORT_C void RTactileFeedback::PlayFeedback( TTouchLogicalFeedback aFeedbackType,
                                              TBool aPlayVibra,
                                              TBool aPlayAudio )
    {
    SendReceive( EPlayFeedback, TIpcArgs( aFeedbackType, 
                                          aPlayVibra, 
                                          aPlayAudio ) );    
    }

// --------------------------------------------------------------------------
// 
// --------------------------------------------------------------------------
//    
EXPORT_C void RTactileFeedback::StartFeedback( TTouchContinuousFeedback aType,
                                              TInt aIntensity )
    {
    TRACE("RTactileFeedback::StartFeedback");
    SendReceive( EStartFeedback, TIpcArgs( aType, aIntensity ) );
    }
        
// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//                        
EXPORT_C void RTactileFeedback::ModifyFeedback( TInt aIntensity )
    {
    TRACE("RTactileFeedback::ModifyFeedback");
    SendReceive( EModifyFeedback, TIpcArgs( aIntensity ) );
    }
    
// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//    
EXPORT_C void RTactileFeedback::StopFeedback()
    {
    TRACE("RTactileFeedback::StopFeedback");
    SendReceive( EStopFeedback );   
    }
    
// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//    
EXPORT_C TInt RTactileFeedback::SetFeedbackEnabledForDevice( TTouchFeedbackType aFeedbackType )
    {
    TRACE("RTactileFeedback::StopFeedback");
    return SendReceive( ESetFBEnabledDevice, TIpcArgs( aFeedbackType ) );   
    }    
    
// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//        
EXPORT_C void RTactileFeedback::FeedbackEnabledForDevice( TTouchFeedbackType& aFeedbackEnabled )
    {
    TPckg<TTouchFeedbackType> enabled( aFeedbackEnabled );
    SendReceive( EFBEnabledForDevice, TIpcArgs( &enabled ) );
    }

// --------------------------------------------------------------------------
// 
// --------------------------------------------------------------------------
//     
EXPORT_C void RTactileFeedback::PlayPreviewFeedback( TInt aLevel,
                                 TTouchLogicalFeedback aFeedback,
                                 TTouchFeedbackType aType )
    {
    SendReceive( EPlayPreviewFeedback, TIpcArgs( aLevel, 
                                                 aFeedback, 
                                                 aType ) );    
    }

// --------------------------------------------------------------------------
// 
// --------------------------------------------------------------------------
//     
EXPORT_C void RTactileFeedback::StartPreviewFeedback( TInt aLevel,
                                  TTouchContinuousFeedback aFeedback,
                                  TInt aIntensity,
                                  TTouchFeedbackType aType )
    {
    SendReceive( EStartPreviewFeedback, TIpcArgs( aLevel, aFeedback, 
                                                  aIntensity, aType ) );
    }

// --------------------------------------------------------------------------
// 
// --------------------------------------------------------------------------
//     
EXPORT_C void RTactileFeedback::ModifyPreviewFeedback( TInt aIntensity )
    {
    SendReceive( EModifyPreviewFeedback, TIpcArgs( aIntensity ) );
    }

// --------------------------------------------------------------------------
// 
// --------------------------------------------------------------------------
// 
EXPORT_C void RTactileFeedback::StopPreviewFeedback()
    {
    SendReceive( EStopPreviewFeedback );    
    }


// --------------------------------------------------------------------------
// 
// --------------------------------------------------------------------------
//      
TVersion RTactileFeedback::Version() const
    {
    return TVersion( KTactileFeedbackServerMajor, 
                     KTactileFeedbackServerMinor, 
                     KTactileFeedbackServerBuild );
    }    

