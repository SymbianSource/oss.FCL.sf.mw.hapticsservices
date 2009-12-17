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
* Description:  Tactile feedback server implementation.
* Part of:      Tactile Feedback.
*
*/

#include <e32debug.h>
#include <touchlogicalfeedback.h>

#include "tactilefeedbacksrv.h"
#include "tactilefeedbacksession.h"

// --------------------------------------------------------------------------
// CTactileFeedbackSrv::NewLC
// 2-phased constructor.
// --------------------------------------------------------------------------
//
CTactileFeedbackSrv* CTactileFeedbackSrv::NewL()
    {
    CTactileFeedbackSrv* self = new (ELeave) CTactileFeedbackSrv();
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }

// --------------------------------------------------------------------------
// CTactileFeedbackSrv::CTactileFeedbackSrv()
// C++ constructor.
// --------------------------------------------------------------------------
//
CTactileFeedbackSrv::CTactileFeedbackSrv() 
    : CPolicyServer( EPriorityNormal, 
                     KTactileFeedbackPolicy, 
                     ESharableSessions ),
      iSessionCount(0)
    {
    // Vibra and audio feedbacks are enabled by default.
    iDeviceFeedbackEnabled |= ETouchFeedbackAudio;
    iDeviceFeedbackEnabled |= ETouchFeedbackVibra;
    }

// --------------------------------------------------------------------------
// CTactileFeedbackSrv::ConstructL
// 2nd phase constructor.
// --------------------------------------------------------------------------
//
void CTactileFeedbackSrv::ConstructL()
    {
    StartL( KTactileFeedbackServer );
    iResolver = CTactileFeedbackResolver::NewL();
    }

// --------------------------------------------------------------------------
// CTactileFeedbackSrv::~CTactileFeedbackSrv()
// C++ destructor.
// --------------------------------------------------------------------------
//
CTactileFeedbackSrv::~CTactileFeedbackSrv()
    {
    delete iResolver;
    }

// --------------------------------------------------------------------------
// CTactileFeedbackSrv::NewSessionL
// from CServer2, creates a new session.
// --------------------------------------------------------------------------
//
CSession2* CTactileFeedbackSrv::NewSessionL( const TVersion& aVersion, 
                                             const RMessage2& /*aMsg*/ ) const
    {
    TBool supported = User::QueryVersionSupported( TVersion( 
                                                KTactileFeedbackServerMajor,
                                                KTactileFeedbackServerMinor,
                                                KTactileFeedbackServerBuild ),
                                                aVersion ); 
    if ( !supported )
        {
        User::Leave( KErrNotSupported );
        }
        
    return new (ELeave) CTactileFeedbackSession();  
    }   
    
// --------------------------------------------------------------------------
// CTactileFeedbackSrv::AddSession
// --------------------------------------------------------------------------
//  
void CTactileFeedbackSrv::AddSession()  
    {
    iSessionCount++;
    }
  
// --------------------------------------------------------------------------
// CTactileFeedbackSrv::RemoveSession
// --------------------------------------------------------------------------
//    
void CTactileFeedbackSrv::RemoveSession()
    {
    iSessionCount--;
    if ( !iSessionCount )
        {
        CActiveScheduler::Stop();
        }
    }    
    
// --------------------------------------------------------------------------
// CTactileFeedbackSrv::PlayFeedback
// --------------------------------------------------------------------------
//    
void CTactileFeedbackSrv::PlayFeedback( TTouchLogicalFeedback aFeedback,
                                        TBool aPlayHaptics,
                                        TBool aPlayAudio )
    {
    TBool playHaptics(EFalse);
    TBool playAudio(EFalse);    
    
    if ( (iDeviceFeedbackEnabled & ETouchFeedbackVibra) && aPlayHaptics )
        {
        playHaptics = ETrue;
        }    
    if ( (iDeviceFeedbackEnabled & ETouchFeedbackAudio) && aPlayAudio )
        {
        playAudio = ETrue;
        }

    if ( playHaptics || playAudio )
        {
        iResolver->PlayFeedback( aFeedback, playHaptics, playAudio );
        }        
    }        

// --------------------------------------------------------------------------
// CTactileFeedbackSrv::StartFeedback
// --------------------------------------------------------------------------
//    
void CTactileFeedbackSrv::StartFeedback( TTouchContinuousFeedback aType,
                                         TInt aIntensity )
    {
    iResolver->StartFeedback( aType, aIntensity );    
    }
        
// ---------------------------------------------------------------------------
// CTactileFeedbackSrv::ModifyFeedback.
// ---------------------------------------------------------------------------
//                        
void CTactileFeedbackSrv::ModifyFeedback( TInt aIntensity )
    {
    iResolver->ModifyFeedback( aIntensity );    
    }
    
// ---------------------------------------------------------------------------
// CTactileFeedbackSrv::StopFeedback.
// ---------------------------------------------------------------------------
//    
void CTactileFeedbackSrv::StopFeedback()
    {
    iResolver->StopFeedback();   
    }

// ---------------------------------------------------------------------------
// CTactileFeedbackSrv::EnableFeedbackForDevice
// ---------------------------------------------------------------------------
//    
void CTactileFeedbackSrv::EnableFeedbackForDevice( TTouchFeedbackType aFeedbackType )
    {
    iDeviceFeedbackEnabled = aFeedbackType;
    }

// ---------------------------------------------------------------------------
// CTactileFeedbackSrv::FeedbackEnabledForDeviceL
// ---------------------------------------------------------------------------
//
TTouchFeedbackType CTactileFeedbackSrv::FeedbackEnabledForDevice()
    {
    return static_cast<TTouchFeedbackType>( iDeviceFeedbackEnabled );
    }

// ---------------------------------------------------------------------------
// CTactileFeedbackSrv::PlayPreviewFeedbackL
// ---------------------------------------------------------------------------
//
void CTactileFeedbackSrv::PlayPreviewFeedback( TInt aLevel,
                                              TTouchLogicalFeedback aFeedback,
                                              TTouchFeedbackType aType )
    {
    iResolver->PlayPreviewFeedback( aLevel, aFeedback, aType );
    }

// ---------------------------------------------------------------------------
// CTactileFeedbackSrv::StartPreviewFeedbackL
// ---------------------------------------------------------------------------
//
void CTactileFeedbackSrv::StartPreviewFeedback( TInt aLevel,
                                          TTouchContinuousFeedback aFeedback,
                                          TInt aIntensity,
                                          TTouchFeedbackType aType )
    {
    iResolver->StartPreviewFeedback( aLevel, aFeedback, aIntensity, aType );
    }

// ---------------------------------------------------------------------------
// CTactileFeedbackSrv::StopPreviewFeedbackL
// ---------------------------------------------------------------------------
//
void CTactileFeedbackSrv::StopPreviewFeedback()
    {
    iResolver->StopPreviewFeedback();
    }

// End of file 
