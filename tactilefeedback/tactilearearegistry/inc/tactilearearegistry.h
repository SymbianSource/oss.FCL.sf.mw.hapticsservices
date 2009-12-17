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
* Description:  The class to be used from server side for accessing registry.
*                Hit testing is done using this API.
* Part of:      Tactile Feedback.
*
*/




#ifndef C_TACTILEAREAREGISTRY_H
#define C_TACTILEAREAREGISTRY_H


#include <e32base.h>
#include <w32std.h>

#include <touchlogicalfeedback.h>
#include "tactileinternaldatatypes.h"


/**
 *  Access to area registry from server side.
 *
 *  This class defines the interface that is used from server side for
 *  managing area registry (e.g. handling connect and disconnect requests)
 *  
 *  The main purpose of this class is to do hit testing of pointer events,
 *  for knowing if feedback should be triggered based on the pointer event.
 *
 *  This class also implements bookkeeping of created window groups and
 *  corresponding window server connection handles. This information is
 *  used for finding correct application's registry entries when a pointer
 *  event arrives (application usually has only one window group, but some
 *  pop-up controls create additional ones).
 *
 *  @lib tactilearearegistry.lib
 *  @since S60 v5.0
  */
class CTactileAreaRegistry: public CBase
    {
public:

    IMPORT_C static CTactileAreaRegistry* NewL();

    virtual ~CTactileAreaRegistry();

    /**
     * Handles connection requests from client application. This has to be 
     * called for every application that connects to the click plugin.
     *
     * Area registry immediately tries to connect to the chunk created
     * by client application. This function leaves with Standard 
     * Symbian OS error codes in case connection fails.
     *
     * @param aData - The data sent by client application.
     */
    IMPORT_C void HandleConnectL( const TTactileFeedbackConnectData& aData );


    /**
     * Handles disconnect request from client application. This has to be 
     * called for every application that disconnects from the click plugin.
     *
     * @param aData - The data sent by client application.
     */
    IMPORT_C void HandleDisconnect( 
        const TTactileFeedbackDisconnectData& aData );
        
        
    /**
     * Records information about a newly created window group.
     *
     * This function has to be called when click plugin receives a 
     * notification about created window group.
     *
     * @param aIdentifier       - Identifier of the created window group.
     * @param aConnectionHandle - Connection handle of the window server
     *                            client, who created the window group.
     */
    IMPORT_C void HandleWindowGroupCreated( 
        TInt aIdentifier, TUint aConnectionHandle );
        
        
    /**
     * Clears stored information about a window group that is closed.
     *
     * This function has to be called when click plugin receives a
     * notification about closed window group. 
     *
     * @param aIdentifier - Window group identifier of the window group
     *                      that was closed.
     */
    IMPORT_C void HandleWindowGroupClosed( TInt aIdentifier );    

    /**
     * Does hit testing of given pointer event against area registry content.
     *
     * Hit testing is done in three phases:
     *
     * 1. First this function uses window group identifier for finding the 
     *    application, which shall receive the pointer event (registry
     *    contents has to be grouped by applicaitons because window 
     *    identifiers are only unique inside one application)
     *
     * 2. Window handle is used for finding those registry entries, which 
     *    are registered to the same window where pointer event hit now 
     *    (coordinates are window -relative).
     *
     * 3. Window's registry entries are hit tested one by one against the
     *    given pointer event, and in case a match is found, then 
     *    corresponding registry entries feedback type is returned.
     *
     * @param aPointerEvent - The pointer event.
     * @param aWgIdentifier - Identifier of the window group, where pointer
     *                        event hit.
     * @param aWindowHandle - Handle number of the window, where pointer
     *                        event hit.
     * @return The logical feedback type that was triggered by the pointer
     *         event, or ETouchFeedbackNone in case pointer event did not
     *         hit any feedback area.
     */
    IMPORT_C TTouchLogicalFeedback HitTestPointerEvent( 
        const TPointerEvent& aPointerEvent,
        TInt aWgIdentifier,
        TUint32 aWindowHandle );


private:

    CTactileAreaRegistry();
    
    /**
     * This function is used for hit testing one chunk.
     *
     * It is assumed that the given chunk is the chunk of that application,
     * where pointer event shall be delivered.
     *
     * @param aPointerEvent - The pointer event.
     * @param aWgIdentifier - Identifier of the window group, where pointer
     *                        event hit.
     * @param aWindowHandle - Handle number of the window, where pointer
     *                        event hit.
     * @return The logical feedback type that was triggered by the pointer
     *         event, or ETouchFeedbackNone in case pointer event did not
     *         hit any feedback area.
     */
    TTouchLogicalFeedback HitTestChunk( 
        RChunk&              aChunk, 
        const TPointerEvent& aPointerEvent,
        TInt                 aWgIdentifier, 
        TUint32              aWindowHandle );

    /**
     * This function checks whether given pointer event and area registry 
     * entry match, and sets feedback type accordingly in case they match.
     *
     * It is assumed that pointer event shall hit that window, where given
     * area is registered (as coordinates are window-relative).
     *
     * Notice that feedback is actually returned in reference parameter
     * aFeedback, and the main return value of the function tells whether
     * hit testing should be continued on next possible area or not.
     * This is because feedback can be "None" even if pointer event
     * hit the feedback area (because "None" is a valid type for feedback
     * areas)
     *
     * @param aPointerEvent - The pointer event.
     * @param aEntry        - Area registry entry, representing feedback area
     *                        in same window where pointer event hit.
     * @param aWgIdentifier - Identifier of the window group, where pointer
     *                        event hit.
     * @param aWindowHandle - Handle number of the window, where pointer
     *                        event hit.
     * @return ETrue if the given pointer event matched given area registry
     *         entry, and hit testing should not be continued anymore. 
     *         EFalse indicates that hit testing should be continued with
     *         next possibly matching feedback area.
     */
    TBool HitTestRegistryEntry( 
        const TPointerEvent&           aPointerEvent, 
        const TFeedbackChunkAreaEntry& aEntry,
        TInt                           aWgIdentifier, 
        TUint32                        aWindowHandle,
        TTouchLogicalFeedback&         aFeedback );

    /**
     * Finds the correct shared memory chunk based on window group id.
     *
     * This is used for performing first phase of hit testing.
     *
     * @param aWgIdentifier - Identifier of the window group, which chunk
     *                        is searched.
     * @return Valid index to member variable iChunkArray, or KErrNotFound
     *         in case given window group was not found.
     */
    TInt ChunkIndexByWindowGroupId( TInt aWgIdentifier ) const;
    
    /**
     * Finds window server client's connection handle based on given
     * window group identifier.
     *
     * This is used for finding correct shared memory chunk based on window
     * group identifier.
     *
     * @param aWgIdentifier - The window group identifier.
     * @return Connection handle of the client, who has created the given
     *         window group. Zero is returned if no client is found.
     */
    TUint ConnectionHandleByWgId( TInt aWgIdentifier ) const;
    

private: // data types

    /**
     * Items of this type are stored into member variable iChunkArray.
     * There is one item in the array for each connecting application.
     *
     * Stored window group id is the identifier of the main window group
     * of connecting application. Stored connection handle is the
     * window server connection handle of the application.
     */
    struct TTactileAreaChunk
        {
        TInt    iWindowGroupId;
        TUint   iConnectionHandle;
        RChunk  iChunk;
        };
        
    /**
     * This type of items are used in member variable iWgArray, which
     * is used for mapping window groups and client applications together.
     */
    struct TTactileWgroupItem
        {
        TInt iWindowGroupId;
        TUint iConnectionHandle;
        };    
        
    
    /**
     * This auxiliary class is used for storing information about pen down
     * event, so that we can know if pen up event happened in same feedback
     * area as corresponding pen down event. This information is necessary
     * in case we want to play feedback both on down- and up events, but only
     * when those happen within same feedback area 
     * (to distinguish cancel -case)
     */
    class TTactilePenDownEvent
        {
    public:
        TTactilePenDownEvent();
        TTactilePenDownEvent( TInt aWindowGroupId, 
                              TUint32 aWindowHandle, 
                              TRect aFeedbackArea );
        void Reset();    
    public:
        TInt iWindowGroupId;
        TUint32 iWindowHandle;
        TRect iFeedbackArea;
        };


private: // data

    /**
     * Array of chunk handles and identifying window group identifiers.
     * Each entry in the array represents one application and its 
     * registered feedback areas.
     */
    RArray<TTactileAreaChunk> iChunkArray;
    
    /** 
     * Here we store the information about created window groups.
     * For each group we also store the connection handle, so that we can
     * know into which application newly created window groups belong.  
     */
    RArray<TTactileWgroupItem> iWgArray;

    /**
     * Handle to the global semaphore, which is used for mutual exclusion
     * when accessing shared memory chunks.
     */
    RSemaphore iTactileSemaphore;
    
    
    /**
     * Information about last received pen down event. This can be used
     * for determining whether pen up event should produde feedback or not.
     */
    TTactilePenDownEvent iLastPenDown;

    };


#endif // C_TACTILEAREAREGISTRY_H
