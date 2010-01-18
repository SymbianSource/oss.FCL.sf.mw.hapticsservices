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
* Description:  The click maker plugin, which
*               handles key events at window server process.
* Part of:      Tactile Feedback.
*
*/

#include <e32std.h>
#include <tactilearearegistry.h>
#include <tactilefeedbacktrace.h>

#include "tactileclickplugin.h"
#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "tactileclickpluginTraces.h"
#endif

// ======== MEMBER FUNCTIONS ========

CTactileClickPlugin::CTactileClickPlugin()
    {
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CTactileClickPlugin::ConstructL()
    {
    TRACE( "CTactileClickPlugin::ConstructL - Begin" );
    
    // Notice that we intentionally don't connect to feedback server here. 
    // This is  because some players may try to connect to such servers, 
    // which don't exist yet when click plugin is loaded.
    iAreaRegistry = CTactileAreaRegistry::NewL();
    
    // We need to store ourselves to thead local storage, so that
    // Anim Dll plugins can access the feedback functionality
    Dll::SetTls( this );

    TRACE( "CTactileClickPlugin::ConstructL - End" );
    }

// ---------------------------------------------------------------------------
// We really have to trap ConstructL, because construction of click maker 
// plug-in must not fail (otherwise WSERV will panic, resulting in KERN 4 and
// re-boot of whole device).
// ---------------------------------------------------------------------------
//
CTactileClickPlugin* CTactileClickPlugin::NewL()
    {
    CTactileClickPlugin* self = new( ELeave ) CTactileClickPlugin;
    TRAP_IGNORE( self->ConstructL() );
    return self;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
CTactileClickPlugin::~CTactileClickPlugin()
    {
    delete iAreaRegistry;
    iFeedback.Close();
    Dll::SetTls( NULL );
    }

// ---------------------------------------------------------------------------
// From class CClickMaker.
// No implementation needed
// ---------------------------------------------------------------------------
//
void CTactileClickPlugin::KeyEvent( 
    TEventCode /*aType*/, const TKeyEvent& /*aEvent*/ )
    {    
    }
    
// ---------------------------------------------------------------------------
// From class CClickMaker.
// No implementation needed
// ---------------------------------------------------------------------------
//
void CTactileClickPlugin::PointerEvent( const TPointerEvent& /*aEvent*/ )
    {
    }

// ---------------------------------------------------------------------------
// From class CClickMaker.
//
// Notice that we really have to check iFeedback handle and iAreaRegistry 
// pointer here, because click maker construction must not fail 
// (otherwise WSERV will panic) 
//
// We handle three types of events here:
// 
// 1. Pointer events coming from window server: For these we do hit testing,
//    and use RTactileFeedback to play feedback accordingly.
//
// 2. Window group open events: These ones we pass to area registry for
//    bookkeeping.
//
// 3. Window group close events: These ones we also pass to area registry, so
//    that it can update its bookkeeping.
// ---------------------------------------------------------------------------
//
void CTactileClickPlugin::OtherEvent( TInt aType, TAny* aParam )
    {
    // 1. Pointer events 
    if ( aParam && aType == EEventPointer && iFeedback.Handle() && iAreaRegistry )
        {
        TPointerEventData* p = static_cast<TPointerEventData*> ( aParam );
                 
        // Extract pointer event from the data given by wserv               
        TPointerEvent pointerEvent = p->iPointerEvent;
        
        // Handle button down and button up events.
        if ( pointerEvent.iType == TPointerEvent::EButton1Down ||
             pointerEvent.iType == TPointerEvent::EButton1Up )
            {
            OstTrace0( TACTILE_PERFORMANCE, TACTILE_CLICK_PLUGIN_PEN_EVENT_1, "e_TACTILE_CLICK_PLUGIN_PEN_EVENT 1");
            
            // Adjust position so that it is window -relative
            pointerEvent.iPosition = ( p->iCurrentPos - p->iWindowOrigin );
            
            TInt feedback = 
                iAreaRegistry->HitTestPointerEvent( 
                    pointerEvent, 
                    p->iWindowGroupId,
                    p->iClientHandle );  
                
            // There is extra vibra- and audio information in top two bits of
            // feedback type.    
            TBool playVibra( EFalse );
            TBool playAudio( EFalse );
            
            if ( pointerEvent.iType == TPointerEvent::EButton1Down )
                {
                playVibra  = ( ( feedback & KTactileVibraBitDown ) != 0);
                playAudio  = ( ( feedback & KTactileAudioBitDown ) != 0);
                }
            else
                {
                playVibra  = ( ( feedback & KTactileVibraBitUp ) != 0);
                playAudio  = ( ( feedback & KTactileAudioBitUp ) != 0);
                }
                        
            // Clear upper two bytes completely
            feedback &= 0xffff;
             
            if ( feedback != ETouchFeedbackNone )    
                {
                iFeedback.PlayFeedback( 
                            static_cast<TTouchLogicalFeedback>( feedback ),
                            playVibra, 
                            playAudio );
                }

            OstTrace0( TACTILE_PERFORMANCE, TACTILE_CLICK_PLUGIN_PEN_EVENT_0, "e_TACTILE_CLICK_PLUGIN_PEN_EVENT 0");
            }
        }
    // 2. Window group open events    
    else if ( aParam && aType == EEventGroupWindowOpen && iAreaRegistry )
        {
        TGroupWindowOpenData* data = static_cast<TGroupWindowOpenData*>( aParam );
        
        iAreaRegistry->HandleWindowGroupCreated( 
            data->iIdentifier,
            data->iClient );
        }
    // 3. Window group close events.    
    else if ( aType == EEventGroupWindowClose && iAreaRegistry )
        {
        TInt identifier = reinterpret_cast<TInt>( aParam ); 
        
        iAreaRegistry->HandleWindowGroupClosed( 
            identifier );
        }
    }

// ---------------------------------------------------------------------------
// From class CClickMaker.
// This is run when client calls RSoundPlugin::CommandReply -function.
//
// Handling of individual commands has been moved to dedicated functions,
// because data handling requires some effort and relively complex code.
// This is due to the mistake in CClickMaker API design, as the data is
// not delivered in the same format as it was passed to RSoundPlugin on
// client side (client gives data in descriptor, but here the descriptor
// needs to be re-built from TAny* pointer, and in addition the lenght
// of data needs to be known based on command id).
//
// Leaving here causes RSoundPlugin::CommandReply to return with an error
// code on client side. It is thus important that we leave in case 
// command ETactileOpCodeConnect fails, because that way client knows
// that feedback won't work and does not attempt to do updates in vain.
// ---------------------------------------------------------------------------
//
TInt CTactileClickPlugin::CommandReplyL( TInt aOpcode, TAny* aArgs )
    {
    TRACE2( "CTactileClickPlugin::OtherEvent - Begin, opCode = %d", aOpcode );
    
    TInt errCode = KErrNone;
    
    switch ( aOpcode )
        {
        case ETactileOpCodeConnect:
            HandleConnectL( aArgs );
            break;
        case ETactileOpCodeDisconnect:
            HandleDisconnect( aArgs );
            break;
        case ETactileOpCodeImmediateFeedback:
            errCode = HandleImmediateFeedback( aArgs );
            break;
        default:
            break;
        }
       
    TRACE( "CTactileClickPlugin::OtherEvent - End" );    
    
    return errCode;    
    };

// ---------------------------------------------------------------------------
// From class MTactileFeedbackServer
//
// This function handles instant feedback requests originating inside
// window server (i.e. From Anim Dll plugins).
// ---------------------------------------------------------------------------
//
void CTactileClickPlugin::InstantFeedback( TTouchLogicalFeedback aType )
    {
    TRACE( "CTactileClickPlugin::InstantFeedback - Begin" );    
    
    if ( iFeedback.Handle() )
        {
        OstTrace0( TACTILE_PERFORMANCE, TACTILE_CLICK_PLUGIN_INSTANT_FEEDBACK_SERVER_API_1, "e_TACTILE_CLICK_PLUGIN_INSTANT_FEEDBACK_SERVER_API 1");
        
        iFeedback.PlayFeedback( aType, ETrue, ETrue );
        
        OstTrace0( TACTILE_PERFORMANCE, TACTILE_CLICK_PLUGIN_INSTANT_FEEDBACK_SERVER_API_0, "e_TACTILE_CLICK_PLUGIN_INSTANT_FEEDBACK_SERVER_API 0");
        }
    }

// ---------------------------------------------------------------------------
// Handling of connection requests from clients.
//
// #1 Try to connect to Tactile Feedback server in case it is not yet created,
//    and creation has already been attempted (this is an optimization in case
//    there is something wrong in the device so that construction cannot 
//    succeed).
// 
// #2 Convert the data and pass connect .command to Area Registry
//
// #3 In case area registry or server connection does not exist, then leave 
//    so that client knows that feedback is not in use.
// ---------------------------------------------------------------------------
//
void CTactileClickPlugin::HandleConnectL( TAny* aArgs )
    {
    // #1 Try to connct to RTactileFeedback first (if not yet connected)
    if ( !iFeedback.Handle() )
        {
        // Try to connect to Tactile Feedback server now when first client 
        // has connected.
        User::LeaveIfError( iFeedback.Connect() );
        }
    
    // #2
    if ( aArgs && iAreaRegistry && iFeedback.Handle() )
        {
        TInt* tmpInt = reinterpret_cast<TInt*>( aArgs );

        TInt dataSize = sizeof( TTactileFeedbackConnectData );
        
        TPtrC8 tmpConnectData( reinterpret_cast<TUint8*>( tmpInt ), dataSize );
        
        TTactileFeedbackConnectData clientData;
        
        TPckg<TTactileFeedbackConnectData> dataBuf (clientData);
        
        dataBuf.Copy( tmpConnectData );
        
        iAreaRegistry->HandleConnectL( clientData );
        }
    else
        {
        // #3 Fail connection if everything is not in place. This way client
        // will not waste effort doing updates in vain.
        User::Leave( KErrGeneral );
        }
    }
        
// ---------------------------------------------------------------------------
// Handle disconnection requests from clients.
//
// Here we just convert the data and pass command to Area Registry
// ---------------------------------------------------------------------------
//
void CTactileClickPlugin::HandleDisconnect( TAny* aArgs )
    {  
    if ( aArgs && iAreaRegistry )
        {
        TInt* tmpInt = reinterpret_cast<TInt*>( aArgs );

        TInt dataSize = sizeof( TTactileFeedbackDisconnectData );
        
        TPtrC8 tmpDisconnectData( reinterpret_cast<TUint8*>( tmpInt ), dataSize );
        
        TTactileFeedbackDisconnectData clientData;
        
        TPckg<TTactileFeedbackDisconnectData> dataBuf (clientData);
        
        dataBuf.Copy( tmpDisconnectData );
        
        iAreaRegistry->HandleDisconnect( clientData );
        }
    }

// ---------------------------------------------------------------------------
// Handling of immediate feedback, that originates from client application
// (See InstantFeedback function for handling of feedback that originates
//  from inside window server)
//
// We don't need to do any state checks (call in progress) etc. here, 
// because Feedback Server will do that anyway. Notice that we rely
// on the client API so that it does not send direct feedback in case client
// application has disabled feedback from itself.
// ---------------------------------------------------------------------------
//
TInt CTactileClickPlugin::HandleImmediateFeedback( TAny* aArgs )
    {
    if ( aArgs && iFeedback.Handle() )
        {
        TTactileFeedbackImmediateData* data = 
            reinterpret_cast<TTactileFeedbackImmediateData*>( aArgs );
        
        OstTrace0( TACTILE_PERFORMANCE, TACTILE_CLICK_PLUGIN_INSTANT_FEEDBACK_CLIENT_API_1, "e_TACTILE_CLICK_PLUGIN_INSTANT_FEEDBACK_CLIENT_API 1");
        
       iFeedback.PlayFeedback( data->iFeedbackType, 
                                      data->iPlayVibra, 
                                      data->iPlayAudio );  

        OstTrace0( TACTILE_PERFORMANCE, TACTILE_CLICK_PLUGIN_INSTANT_FEEDBACK_CLIENT_API_0, "e_TACTILE_CLICK_PLUGIN_INSTANT_FEEDBACK_CLIENT_API 0");
        }
    
    return KErrNone;
    }

// ======== GLOBAL FUNCTIONS ========

// ---------------------------------------------------------------------------
// Function with this signature needs to be the first exported function
// in click maker plugin DLLs.
//
// Constructs and returns an instance to tactile click maker plugin.
// ---------------------------------------------------------------------------
//
EXPORT_C CClickMaker* CreateClickMakerL()
    {
    TRACE( "CreateClickMakerL" );    
    return CTactileClickPlugin::NewL();
    }
