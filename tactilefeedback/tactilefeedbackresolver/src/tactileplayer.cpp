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
* Description:  Interface class for tactile feedback players.
* Part of:      Tactile Feedback.
*
*/

#include "tactileplayer.h"

const TUid KTactilePlayerIFUid   = {0x2001CBA7};

_LIT8( KTactileHaptics, "Haptics" );
_LIT8( KTactileAudio,   "Audio"   );

// ======== LOCAL FUNCTIONS ========

// ---------------------------------------------------------------------------
// Constructor.
// ---------------------------------------------------------------------------
//
CTactilePlayer* CTactilePlayer::NewL( TUid aImplementationUid, 
                                      CRepository& aRepository )
    {
    return static_cast<CTactilePlayer*> ( REComSession::CreateImplementationL( 
            aImplementationUid, 
            _FOFF(CTactilePlayer, iDtor_ID_Key ), 
            &aRepository ) );
    }

// ---------------------------------------------------------------------------
// Lists the available plugin implementations for this interface.
// ---------------------------------------------------------------------------
// 
void CTactilePlayer::ListImplementationsL( TTactilePlayerType aType,
                                           RImplInfoPtrArray& aImplInfoArray )
    {
    if ( aType == ETactilePlayerAny )
        {
        REComSession::ListImplementationsL( KTactilePlayerIFUid, 
                                            aImplInfoArray );
        }
    else
        {
        RImplInfoPtrArray implInfoArray;
        CleanupClosePushL( implInfoArray );
        REComSession::ListImplementationsL( KTactilePlayerIFUid, 
                                            implInfoArray );
        for( TInt i(0) ; i < implInfoArray.Count() ; i++ )
            {
            switch( aType )
                {
                case ETactilePlayerHaptics:
                    if ( !implInfoArray[i]->DataType().Compare( KTactileHaptics ) )
                        {
                        aImplInfoArray.AppendL( implInfoArray[i] );
                        }
                    break;
                case ETactilePlayerAudio:
                    if ( !implInfoArray[i]->DataType().Compare( KTactileAudio ) )
                        {
                        aImplInfoArray.AppendL( implInfoArray[i] );
                        } 
                    break;
                default:
                    break;                       
                }
            }
        CleanupStack::PopAndDestroy( &implInfoArray );
        }
    }

// ---------------------------------------------------------------------------
// Destructor.
// ---------------------------------------------------------------------------
// 
EXPORT_C CTactilePlayer::~CTactilePlayer()
    {
    REComSession::DestroyedImplementation( iDtor_ID_Key );
    }

// ---------------------------------------------------------------------------
// Start feedback.
// ---------------------------------------------------------------------------
//   
EXPORT_C TInt CTactilePlayer::StartFeedback( TTouchContinuousFeedback /*aFeedback*/,
                                             TInt /*aIntensity*/ )
    {
    // Empty implementation for plugins, which don't support 
    // continuous feedback.
    return KErrNotSupported;
    }
        
// ---------------------------------------------------------------------------
// Modify feedback.
// ---------------------------------------------------------------------------
//                        
EXPORT_C TInt CTactilePlayer::ModifyFeedback( TInt /*aIntensity*/ )
    {
    // Empty implementation for plugins, which don't support 
    // continuous feedback.
    return KErrNotSupported;    
    }
    
// ---------------------------------------------------------------------------
// Stop feedback.
// ---------------------------------------------------------------------------
//    
EXPORT_C void CTactilePlayer::StopFeedback()
    {
    // Empty implementation for plugins, which don't support 
    // continuous feedback.
    }
    
// ---------------------------------------------------------------------------
// Maximum level.
// ---------------------------------------------------------------------------
// 
EXPORT_C TInt CTactilePlayer::MaxLevel()
    {
    // Default implementation returns 1 as maximum level, which means that
    // player has only one level.
    return 1; 
    }

// ---------------------------------------------------------------------------
// Play preview.
// ---------------------------------------------------------------------------
// 
EXPORT_C TInt CTactilePlayer::PlayPreviewFeedback( TInt /*aLevel*/,
                                         TTouchLogicalFeedback /*aFeedback*/ )
    {
    return KErrNotSupported;
    }

// ---------------------------------------------------------------------------
// Start preview.
// ---------------------------------------------------------------------------
// 
EXPORT_C TInt CTactilePlayer::StartPreviewFeedback( TInt /*aLevel*/,
                                       TTouchContinuousFeedback /*aFeedback*/,
                                       TInt /*aIntensity*/ )
    {
    return KErrNotSupported;
    }

// ---------------------------------------------------------------------------
// Stop preview.
// ---------------------------------------------------------------------------
// 
EXPORT_C void CTactilePlayer::StopPreviewFeedback()
    {    
    }
