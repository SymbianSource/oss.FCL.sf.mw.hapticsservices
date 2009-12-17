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
* Description:  Area registry for one window.
* Part of:      Tactile Feedback.
*
*/


#ifndef C_TOUCHFEEDBACKREGISTRY_H
#define C_TOUCHFEEDBACKREGISTRY_H



#include <e32std.h>
#include <e32base.h>
#include <coemain.h>

#include "touchlogicalfeedback.h"


/**
 *  Area registry entry
 *
 *  There is one entry like this for each area that has been
 *  added to the registry.
 */
struct TFeedbackEntry
    {
    TUint  iId;
    TRect iRect;
    TTouchLogicalFeedback iFeedbackTypeDown;
    TTouchFeedbackType iFeedbackDown;
    TTouchLogicalFeedback iFeedbackTypeUp;
    TTouchFeedbackType iFeedbackUp;
    TTouchEventType iEventType;
    TBool iVibraEnabled;
    TBool iAudioEnabled;
    TBool iVisible;
    };

/**
 *  Registry entry for one window.
 *
 *  This class maintains area registry for one window. It offers
 *  services for adding, removing and mofidying areas in the
 *  registry, as well as resolving whether a pointer event
 *  hit any area on the registry or not.
 *
 *  @lib N/A (no exported functions)
 *  @since S60 v5.0
 */
NONSHARABLE_CLASS( CTouchFeedbackRegistry ): 
        public CBase
    {


public:

    /**
     * Instantiation mehtod
     *
     * @since S60 5.0
     * @param aWindowHandle - The Id of the window, where this registry is
     *                        associated.
     */
    static CTouchFeedbackRegistry* NewL( TUint aWindowHandle );


    virtual ~CTouchFeedbackRegistry();

    /**
     * Adds feedback area to registry.
     *
     * @param aRect         - The feedback area rectangle
     * @param aFeedbackType - The logical feedback type given
     * @param aEventType    - The pointer event type that triggers
     *                        the feedback
     * @param aId           - The unique identifier for this registry area
     * @param aVibraEnabled - If ETrue is given, then vibra feedback is
     *                        enabled for the added area.
     * @param aAudioEnabled - If ETrue is given, then audio feedback is
     *                        enabled for the added area.
     * @param aVisible      - If EFalse is given, then the whole area will
     *                        not be put to the registry. The information is
     *                        still stored on client-side so that area can
     *                        be added in case control becomes visible.
     */
    void AddFeedbackAreaL( 
        TRect aRect, 
        TTouchLogicalFeedback aFeedbackTypeDown, 
        TTouchFeedbackType aFeedbackDown,
        TTouchLogicalFeedback aFeedbackTypeUp, 
        TTouchFeedbackType aFeedbackUp,
        TTouchEventType aEventType,
        TUint aId,
        TBool aVibraEnabled,
        TBool aAudioEnabled,
        TBool aVisible );
    
    /**
     * Removes feedback area from the registry.
     *
     * @param aId - Identifier of the feedback area to be removed.
     */
    void RemoveFeedbackArea( TUint aId );

    /**
     * Changes feedback area in the registry.
     *
     * @param aId      - Identifier of the feedback area to be changed.
     * @param aNewRect - New feedback area rectangle.
     * @return ETrue the given area was found, and if it's feedback type
     *                   really changed.
     *               (in practice this means that changes need to be
     *                updated to server).
     *         EFalse otherwise.
     */
    TBool ChangeFeedbackArea( 
        TUint aId, 
        TRect aNewRect );
    
    /**
     * Changes feedback type for given area in the registry.
     *
     * @param aId              - Identifier of the feedback area to be changed.
     * @param aNewFeedbackTypeDown - New logical feedback type for down event.
     * @param aNewFeedbackTypeUp   - New logical feedback type for up event.
     * @return ETrue the given area was found, and if it really changed.
     *               (in practice this means that changes need to be
     *                updated to server).
     *         EFalse otherwise.
     */
    TBool ChangeFeedbackType( 
        TUint aId, 
        TTouchLogicalFeedback aNewFeedbackTypeDown,
        TTouchLogicalFeedback aNewFeedbackTypeUp );
      
    /**
     * Makes the given feedback area a first priority area in this window
     * @param aId      - The identifier of the area, which shall be prioritized.
     * @return KErrNotFound if an area with given ID is not found from the registry.
     */
    TInt MoveFeedbackAreaToFirstPriority( TUint aId ); 
    
    /**
     * Enables or disables given feedback area. This does not change area's
     * actual feedback type.
     * @param aId      - The identifier of the area, which feedback shall be 
     *                   enabled / disabled.
     * @param aVibraEnabled - Give ETrue to enable vibra feedback, and 
     *                   EFalse to disable vibra feedback for this area.
     * @param aAudioEnabled - Give ETrue to enable audio feedback, and 
     *                        EFalse to disable audio feedback for this area.
     * @param aVisible      - Visibility status of the control, who owns 
     *                        the area.
     * @return ETrue if registry actually changed due to this function call.
     */
    TBool SetFeedbackEnabled( TUint aId, 
                              TBool aVibraEnabled, 
                              TBool aAudioEnabled,
                              TBool aVisible ); 
        
    /**
     * Returns the window handle, where this registry is associated.
     *
     * @return Window handle of associated window.
     */
    TUint WindowHandle() const;
        
    /**
     * Returns count of areas in this registry
     */
    TInt AreaCount();  
    
    RArray<TFeedbackEntry>* WindowRegistry();  

    
private:


    CTouchFeedbackRegistry( TUint aWindowHandle );


private: // data

    /**
     * Handle number of the window, to which this registry is related.
     */
    TUint iWindowHandle;
    
    /**
     * Array of area registry items in this window
     */
    RArray<TFeedbackEntry> iAreaArray;
    };



#endif //  C_TOUCHFEEDBACKREGISTRY_H
