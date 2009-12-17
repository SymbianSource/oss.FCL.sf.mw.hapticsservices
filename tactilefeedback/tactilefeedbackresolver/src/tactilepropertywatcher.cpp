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
* Description:  Class for monitoring P&S property changes.
* Part of:      Tactile Feedback.
*
*/

#include <e32std.h>
#include <e32cmn.h> 

#include "tactilepropertywatcher.h"

// ---------------------------------------------------------------------------
// Constructor.
// ---------------------------------------------------------------------------
//
CTactilePropertyWatcher::CTactilePropertyWatcher( 
        MTactilePropertyObserver& aObserver, 
        const TUid aCategory, 
        const TInt aKey,
        CActive::TPriority aPriority ) : 
            CActive( aPriority ), 
            iObserver( aObserver ), 
            iCategory( aCategory ), 
            iKey( aKey )
      
    {
    CActiveScheduler::Add( this );
    }

// ---------------------------------------------------------------------------
// 2nd phase constructor.
// ---------------------------------------------------------------------------
//
void CTactilePropertyWatcher::ConstructL()
    {
    User::LeaveIfError( iProperty.Attach( iCategory, iKey ) );

    Subscribe();
    }

// ---------------------------------------------------------------------------
// 2-phased constructor.
// ---------------------------------------------------------------------------
//
CTactilePropertyWatcher* CTactilePropertyWatcher::NewL( 
        MTactilePropertyObserver& aObserver, 
        const TUid aCategory, 
        const TInt aKey,
        CActive::TPriority aPriority )
    {
    CTactilePropertyWatcher* self = new (ELeave) 
        CTactilePropertyWatcher( aObserver, aCategory, aKey, aPriority );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// Destructor.
// ---------------------------------------------------------------------------
//
CTactilePropertyWatcher::~CTactilePropertyWatcher()
    {
    Cancel(); 
    iProperty.Close();
    }

// ---------------------------------------------------------------------------
// Retrieves an integer value associated with this property.
// ---------------------------------------------------------------------------
//       
void CTactilePropertyWatcher::GetL( TInt& aValue )
    {
    User::LeaveIfError( iProperty.Get( aValue ) );
    }

    
// ---------------------------------------------------------------------------
// Retrieves a buffer associated with this property.
// ---------------------------------------------------------------------------
//       
void CTactilePropertyWatcher::GetL( TDes8& aValue )
    {
    User::LeaveIfError( iProperty.Get( aValue ) );
    }

// ---------------------------------------------------------------------------
// Retrieves an integer value associated with this property.
// ---------------------------------------------------------------------------
//       
TInt CTactilePropertyWatcher::Get( TInt& aValue )
    {
    return iProperty.Get( aValue );
    }
    
// ---------------------------------------------------------------------------
// Retrieves a buffer associated with this property.
// ---------------------------------------------------------------------------
//       
TInt CTactilePropertyWatcher::Get( TDes8& aValue )
    {
    return iProperty.Get( aValue );
    }

// ---------------------------------------------------------------------------
// Sets a P&S property to an integer value. The attached version of
// RProperty::Set is used which is faster than using RProperty::Set
// directly without attach.
// ---------------------------------------------------------------------------
//       
void CTactilePropertyWatcher::SetL( const TInt aValue )
    {
    User::LeaveIfError( iProperty.Set( aValue ) );
    }

// ---------------------------------------------------------------------------
// Sets a P&S property to a (large)bytearray value. The attached version of
// RProperty::Set is used which is faster than using RProperty::Set
// directly without attach.
// ---------------------------------------------------------------------------
//       
void CTactilePropertyWatcher::SetL( const TDesC8& aValue )
    {
    User::LeaveIfError( iProperty.Set( aValue ) );        
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CTactilePropertyWatcher::Subscribe()
    {
    if ( !IsActive() )
        {
        iProperty.Subscribe( iStatus );

        SetActive();
        }
    }

// ---------------------------------------------------------------------------
// From class CActive
// ---------------------------------------------------------------------------
//
void CTactilePropertyWatcher::DoCancel()
    {
    iProperty.Cancel();
    }

// ---------------------------------------------------------------------------
// From class CActive
// ---------------------------------------------------------------------------
//
void CTactilePropertyWatcher::RunL()
    {
    if ( iStatus.Int() == KErrNone )
        {
        // Subscribe first, then notify the observer that a property
        //  has changed. 
        Subscribe();
        iObserver.PropertyChangedL( iCategory, iKey );
        }
    else if ( iStatus.Int() == KErrNotFound )
        {
        // We get here if a property gets deleted. We still need to subscribe
        // to it because it might get redefined later.
        Subscribe();        
        }
    }

// End of File
