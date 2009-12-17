/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Session class of Tactile Feedback server.
* Part of:      Tactile Feedback.
*
*/

#include <s32mem.h>

// #include "tactilefeedbacktrace.h"
#include "tactilefeedbacksrv.h"
#include "tactilefeedbacksession.h"

// --------------------------------------------------------------------------
// CTactileFeedbackSession::CTactileFeedbackSession
// C++ constructor.
// --------------------------------------------------------------------------
//
CTactileFeedbackSession::CTactileFeedbackSession() 
    {
    
    }

// --------------------------------------------------------------------------
// CTactileFeedbackSession::~CTactileFeedbackSession
// C++ destructor.
// --------------------------------------------------------------------------
//
CTactileFeedbackSession::~CTactileFeedbackSession()
    {
    // Stop continuous feedback, if it is started in this session. 
    if ( iFeedbackStarted )
        {
        Server().StopFeedback();
        }
    Server().RemoveSession();
    }

// --------------------------------------------------------------------------
// CTactileFeedbackSession::CreateL
// From CSession2, used to increase server's session count.
// --------------------------------------------------------------------------
//
void CTactileFeedbackSession::CreateL()
    {
    // TRACE("CTactileFeedbackSession::CreateL - Begin");
    Server().AddSession();
    // TRACE("CTactileFeedbackSession::CreateL - End");    
    }

// --------------------------------------------------------------------------
// CTactileFeedbackSession::Server
// Reference to server.
// --------------------------------------------------------------------------
//
CTactileFeedbackSrv& CTactileFeedbackSession::Server()
    {   
    return *static_cast<CTactileFeedbackSrv*>
           ( const_cast<CServer2*>( CSession2::Server() ) );
    }
    
// --------------------------------------------------------------------------
// CTactileFeedbackSession::ServiceL
// From CSession2, passes the request forward to DispatchMessageL.
// --------------------------------------------------------------------------
//
void CTactileFeedbackSession::ServiceL( const RMessage2& aMessage )
    {
    TRAPD(err,DispatchMessageL(aMessage));
    if ( err != KErrNone )
        {
        aMessage.Complete( err );
        }    
    }

// --------------------------------------------------------------------------
// CTactileFeedbackSession::DispatchMessageL
// Handles the request from client.
// --------------------------------------------------------------------------
//
void CTactileFeedbackSession::DispatchMessageL( const RMessage2& aMessage )
    {
    switch( aMessage.Function() )
        {
        case EPlayFeedback:
            PlayFeedback( aMessage );
            break;
        case EStartFeedback:
            StartFeedback( aMessage );
            break;
        case EModifyFeedback:
            ModifyFeedback( aMessage );
            break;
        case EStopFeedback:            
            StopFeedback( aMessage );
            break;            
        case ESetFBEnabledDevice:            
            SetFBEnabledForDeviceL( aMessage );
            break;
        case EFBEnabledForDevice:
            FeedbackEnabledForDeviceL( aMessage );
            break;
        case  EPlayPreviewFeedback:
            PlayPreviewFeedback( aMessage );
            break;
        case  EStartPreviewFeedback:
            StartPreviewFeedback( aMessage );
            break;
        case  EModifyPreviewFeedback:
            ModifyPreviewFeedback( aMessage );
            break;
        case  EStopPreviewFeedback:
            StopPreviewFeedback( aMessage );
            break;            
        default:
            aMessage.Complete( KErrArgument );
            break;
        }
    }

// --------------------------------------------------------------------------
// CTactileFeedbackSession::PlayFeedbackL
//
// --------------------------------------------------------------------------
//     
void CTactileFeedbackSession::PlayFeedback( const RMessage2& aMessage )
    {
    Server().PlayFeedback( static_cast<TTouchLogicalFeedback>(aMessage.Int0()),
                           aMessage.Int1(),
                           aMessage.Int2() );

    aMessage.Complete( KErrNone );
    }

// --------------------------------------------------------------------------
// CTactileFeedbackSession::StartFeedbackL
//
// --------------------------------------------------------------------------
//
void CTactileFeedbackSession::StartFeedback( const RMessage2& aMessage )
    {
    // TRACE("CTactileFeedbackSession::StartFeedbackL - Begin");
    Server().StartFeedback( 
                     static_cast<TTouchContinuousFeedback>( aMessage.Int0() ),
                     aMessage.Int1() );
    iFeedbackStarted = ETrue;
    aMessage.Complete( KErrNone );
    // TRACE("CTactileFeedbackSession::StartFeedbackL - End");
    }
    
// --------------------------------------------------------------------------
// CTactileFeedbackSession::ModifyFeedbackL
//
// --------------------------------------------------------------------------
//
void CTactileFeedbackSession::ModifyFeedback( const RMessage2& aMessage )
    {
    // TRACE("CTactileFeedbackSession::ModifyFeedbackL - Begin");
    Server().ModifyFeedback( aMessage.Int0() );
    aMessage.Complete( KErrNone );
    // TRACE("CTactileFeedbackSession::ModifyFeedbackL - End");
    }
    
// --------------------------------------------------------------------------
// CTactileFeedbackSession::StopFeedbackL
//
// --------------------------------------------------------------------------
//
void CTactileFeedbackSession::StopFeedback( const RMessage2& aMessage )
    {
    // TRACE("CTactileFeedbackSession::StopFeedbackL - Begin");
    Server().StopFeedback();
    iFeedbackStarted = EFalse;
    aMessage.Complete( KErrNone );
    // TRACE("CTactileFeedbackSession::StopFeedbackL - End");
    }

// --------------------------------------------------------------------------
// CTactileFeedbackSession::SetFBEnabledForDeviceL
//
// --------------------------------------------------------------------------
//
void CTactileFeedbackSession::SetFBEnabledForDeviceL( const RMessage2& aMessage )
    {
    if ( aMessage.HasCapability( ECapabilityWriteDeviceData ) )
        {
        Server().EnableFeedbackForDevice( 
                        static_cast<TTouchFeedbackType>( aMessage.Int0() )  );
        aMessage.Complete( KErrNone );
        }
    else
        {
        User::Leave( KErrPermissionDenied );
        }
    }

// --------------------------------------------------------------------------
// CTactileFeedbackSession::FeedbackEnabledForDeviceL
//
// --------------------------------------------------------------------------
//
void CTactileFeedbackSession::FeedbackEnabledForDeviceL( const RMessage2& aMessage )
    {
    TTouchFeedbackType feedbackEnabled = Server().FeedbackEnabledForDevice();
    
    TPckg<TTouchFeedbackType> enabled(feedbackEnabled);
    
    aMessage.WriteL( 0, enabled );
    aMessage.Complete( KErrNone );
    }

// --------------------------------------------------------------------------
// CTactileFeedbackSession::PlayPreviewFeedbackL
//
// --------------------------------------------------------------------------
//
void CTactileFeedbackSession::PlayPreviewFeedback( const RMessage2& aMessage )
    {
    Server().PlayPreviewFeedback( aMessage.Int0(),
                         static_cast<TTouchLogicalFeedback>(aMessage.Int1() ),
                         static_cast<TTouchFeedbackType>(aMessage.Int2() ) );
    aMessage.Complete( KErrNone );    
    
    }

// --------------------------------------------------------------------------
// CTactileFeedbackSession::StartPreviewFeedbackL
//
// --------------------------------------------------------------------------
//
void CTactileFeedbackSession::StartPreviewFeedback( const RMessage2& aMessage )
    {
    Server().StartPreviewFeedback( 
                     aMessage.Int0(),
                     static_cast<TTouchContinuousFeedback>( aMessage.Int1() ),
                     aMessage.Int2(),
                     static_cast<TTouchFeedbackType>( aMessage.Int3() ) );
    iFeedbackStarted = ETrue;                                   
    aMessage.Complete( KErrNone );    
    }

// --------------------------------------------------------------------------
// CTactileFeedbackSession::ModifyPreviewFeedbackL
//
// --------------------------------------------------------------------------
//
void CTactileFeedbackSession::ModifyPreviewFeedback( const RMessage2& aMessage )
    {
    // Normal ModifyFeedback function is used also for preview.
    Server().ModifyFeedback( aMessage.Int0() );
    aMessage.Complete( KErrNone );    
    }

// --------------------------------------------------------------------------
// CTactileFeedbackSession::StopPreviewFeedbackL
//
// --------------------------------------------------------------------------
//
void CTactileFeedbackSession::StopPreviewFeedback( const RMessage2& aMessage )
    {
    Server().StopPreviewFeedback();
    iFeedbackStarted = EFalse;
    aMessage.Complete( KErrNone );    
    }
    
// End of file 
