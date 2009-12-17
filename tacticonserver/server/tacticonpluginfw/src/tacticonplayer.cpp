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
* Description: Interface class for tacticon players.
* Part of    : Tacticon Server
*/

#include "tacticonplayer.h"
#include "tacticonpluginfwuids.hrh"

const TUid KTacticonPlayerIFUid   = { ETacticonPlayerIFUID };

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
EXPORT_C CTacticonPlayer* CTacticonPlayer::NewL( TUid aImplementationUid, 
                                        CRepository& aRepository )
    {
    return ( CTacticonPlayer* ) 
                REComSession::CreateImplementationL( aImplementationUid, 
                _FOFF(CTacticonPlayer, iDtor_ID_Key ), &aRepository );
    }

// ---------------------------------------------------------------------------
// Lists the available plugin implementations for this interface.
// ---------------------------------------------------------------------------
// 
EXPORT_C void CTacticonPlayer::ListImplementationsL( RImplInfoPtrArray& aImplInfoArray )
    {
    REComSession::ListImplementationsL( KTacticonPlayerIFUid, 
                                        aImplInfoArray );
    }

// ---------------------------------------------------------------------------
// Destructor.
// ---------------------------------------------------------------------------
// 
EXPORT_C CTacticonPlayer::~CTacticonPlayer()
    {
    REComSession::DestroyedImplementation( iDtor_ID_Key );
    }

// End of file.
