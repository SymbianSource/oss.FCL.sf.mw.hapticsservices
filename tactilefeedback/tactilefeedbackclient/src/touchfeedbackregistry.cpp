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
* Description:  Local area registry management in client's process.
* Part of:      Tactile Feedback.
*
*/

#include <eikenv.h>

#include "touchfeedbackregistry.h"

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// Constructor.
// ---------------------------------------------------------------------------
//
CTouchFeedbackRegistry::CTouchFeedbackRegistry( TUint aWindowHandle ): 
    iWindowHandle( aWindowHandle )
    {
    }

// ---------------------------------------------------------------------------
// 2-phased constructor.
// ---------------------------------------------------------------------------
//
CTouchFeedbackRegistry* CTouchFeedbackRegistry::NewL(
    TUint aWindowHandle )
    {
    CTouchFeedbackRegistry* self = 
        new( ELeave ) CTouchFeedbackRegistry ( aWindowHandle );

    // ConstructL not needed on the moment
    // --> Just return created instance right away
    return self;
    }

// ---------------------------------------------------------------------------
// Destructor.
// ---------------------------------------------------------------------------
//
CTouchFeedbackRegistry::~CTouchFeedbackRegistry()
    {
    iAreaArray.Close();
    }

// ---------------------------------------------------------------------------
// 
// #1 If area exists already, then update it
// #2 Otherwise add it to the beginning of registry
//
// ---------------------------------------------------------------------------
//
void CTouchFeedbackRegistry::AddFeedbackAreaL( 
    TRect aRect,
    TTouchLogicalFeedback aFeedbackTypeDown, 
    TTouchFeedbackType aFeedbackDown,
    TTouchLogicalFeedback aFeedbackTypeUp, 
    TTouchFeedbackType aFeedbackUp,
    TTouchEventType aEventType,
    TUint aId,
    TBool aVibraEnabled,
    TBool aAudioEnabled,
    TBool aVisible )
    {
    TBool update = EFalse;
    
    // #1
    for ( TInt i = 0; i < iAreaArray.Count() && !update; i++ )
        {
        if ( iAreaArray[i].iId == aId )
            {
            iAreaArray[i].iRect         = aRect;
            iAreaArray[i].iFeedbackTypeDown = aFeedbackTypeDown;
            iAreaArray[i].iFeedbackDown     = aFeedbackDown;
            iAreaArray[i].iFeedbackTypeUp   = aFeedbackTypeUp;
            iAreaArray[i].iFeedbackUp       = aFeedbackUp;
            iAreaArray[i].iEventType    = aEventType;
            iAreaArray[i].iVibraEnabled = aVibraEnabled;
            iAreaArray[i].iAudioEnabled = aAudioEnabled;
            iAreaArray[i].iVisible      = aVisible;
            
            update = ETrue;
            }
        }

    // #2
    if ( !update )
        {
        TFeedbackEntry newEntry;
        
        newEntry.iRect         = aRect;
        newEntry.iFeedbackTypeDown = aFeedbackTypeDown;
        newEntry.iFeedbackDown     = aFeedbackDown;
        newEntry.iFeedbackTypeUp   = aFeedbackTypeUp;
        newEntry.iFeedbackUp       = aFeedbackUp;
        newEntry.iEventType    = aEventType;
        newEntry.iId           = aId;
        newEntry.iVibraEnabled = aVibraEnabled;
        newEntry.iAudioEnabled = aAudioEnabled;
        newEntry.iVisible      = aVisible;
        
        iAreaArray.AppendL( newEntry );        
        }
    }
    
// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CTouchFeedbackRegistry::RemoveFeedbackArea( TUint aId )
    {
    for ( TInt i = 0; i < iAreaArray.Count(); i++ )
        {
        if ( iAreaArray[i].iId == aId )
            {
            iAreaArray.Remove( i );
            break;
            }
        }
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
TBool CTouchFeedbackRegistry::ChangeFeedbackArea(
    TUint aId, 
    TRect aNewRect )
    {
    TBool changed = EFalse;
    
    for ( TInt i = 0; i < iAreaArray.Count(); i++ )
        {
        if ( iAreaArray[i].iId == aId )
            {
            // Check if we really have to change something
            if ( iAreaArray[i].iRect != aNewRect )
                {
                iAreaArray[i].iRect = aNewRect;
                
                changed = ETrue;                
                }
            
            // Anyway break out from the loop, as we found the area
            // already
            break;
            }
        }
    
    return changed;
    }
    
// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
TBool CTouchFeedbackRegistry::ChangeFeedbackType(
    TUint aId, 
    TTouchLogicalFeedback aNewFeedbackTypeDown,
    TTouchLogicalFeedback aNewFeedbackTypeUp )
    {
    TBool changed = EFalse;
    
    for ( TInt i = 0; i < iAreaArray.Count(); i++ )
        {
        if ( iAreaArray[i].iId == aId )
            {
            // Check if we really have to change something
            if ( iAreaArray[i].iFeedbackTypeDown != aNewFeedbackTypeDown )
                {
                iAreaArray[i].iFeedbackTypeDown = aNewFeedbackTypeDown;
                changed = ETrue;               
                }
            if ( aNewFeedbackTypeUp != 0xFFFFFFFF && 
                 iAreaArray[i].iFeedbackTypeUp != aNewFeedbackTypeUp )
                {
                iAreaArray[i].iFeedbackTypeUp = aNewFeedbackTypeUp;
                changed = ETrue;               
                }
            
            // Anyway break out from the loop, as we found the area
            // already
            break;
            }
        }
    
    return changed;
    }
    
// ---------------------------------------------------------------------------
// Areas are moved to server side starting from last position
// --> We need to make the desired area the last one in the array.
// ---------------------------------------------------------------------------
//
TInt CTouchFeedbackRegistry::MoveFeedbackAreaToFirstPriority( TUint aId )
    {
    TInt ret = KErrNotFound;
    
    for ( TInt i = 0; i < iAreaArray.Count(); i++ )
        {
        if ( iAreaArray[i].iId == aId )
            {
            TFeedbackEntry tmpEntry = iAreaArray[i];
            
            TInt lastPosition = iAreaArray.Count()-1;
            
            iAreaArray[i]            = iAreaArray[lastPosition];
            iAreaArray[lastPosition] = tmpEntry;
            
            ret = KErrNone;
            
            break;
            }
        }
    
    return ret;    
    }
    
// ---------------------------------------------------------------------------
// Here we set the new enabled / disabled state for this area.
//
// We also return ETrue if the status really changed. This way caller of this
// function can know if any updates to server side have to be done or not.
// ---------------------------------------------------------------------------
//
TBool CTouchFeedbackRegistry::SetFeedbackEnabled( TUint aId, 
                                                  TBool aVibraEnabled, 
                                                  TBool aAudioEnabled,
                                                  TBool aVisible )
    {
    TInt changed = EFalse;
    
    for ( TInt i = 0; i < iAreaArray.Count(); i++ )
        {
        if ( iAreaArray[i].iId == aId )
            {
            // Check if the status really changes
            if ( ( iAreaArray[i].iVibraEnabled != aVibraEnabled ) || 
                  (iAreaArray[i].iAudioEnabled != aAudioEnabled ) ||
                  (iAreaArray[i].iVisible      != aVisible ))
                {
                changed = ETrue;
                
                iAreaArray[i].iVibraEnabled = aVibraEnabled;
                iAreaArray[i].iAudioEnabled = aAudioEnabled;
                iAreaArray[i].iVisible      = aVisible;
                }
             
            break;
            }
        }
        
    return changed;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
TUint CTouchFeedbackRegistry::WindowHandle() const
    {
    return iWindowHandle;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
TInt CTouchFeedbackRegistry::CTouchFeedbackRegistry::AreaCount()
    {
    return iAreaArray.Count();
    }
    
    
// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
RArray<TFeedbackEntry>* CTouchFeedbackRegistry::WindowRegistry()
    {
    return &iAreaArray;
    }
