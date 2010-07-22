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
* Description:  This class implements Tactile Feedback Client API
* Part of:      Tactile Feedback.
*
*/



#ifndef C_TOUCHFEEDBACKIMPL_H
#define C_TOUCHFEEDBACKIMPL_H

#include <e32std.h>
#include <e32base.h>
#include <coemain.h>

#include "touchlogicalfeedback.h"
#include "touchfeedback.h"


class CTouchFeedbackRegistry;
class CTouchFeedbackClient;


/**
 * This structure stores one index / area id pair.
 * This kind of entries are stored for each control, which have
 * feedback areas defined.
 */
struct TAreaIndexEntry
    {
    TUint32 iIndex;
    TInt iAreaId;
    };

/**
 * This structure stores one control's information on client-side.
 * This information includes control's visibility, dimming and feedback
 * disabled information, as well as array of the feedback area
 * identifiers.
 *
 * Notice that the client handle is stored for two reasons: Firstly
 * it is an optimization to store it as this way we can avoid unnecessary
 * client-server transaction (window server client-side API does not 
 * cache the handle). Secondly some times control's window may have been
 * deleted before control removes it's feedback areas. In these situations
 * we even wouldn't be able to use the control for getting the handle 
 * anymore.
 */
struct TControlCacheEntry
    {
    const CCoeControl* iControl;
    TUint32 iClientHandle;
    TBool iVibraDisabled;
    TBool iAudioDisabled;
    TBool iVisible;
    TBool iDimmed;
    RArray<TAreaIndexEntry> iAreaArray;
    };

/**
 *  This class implements the Client API that is provided for UI Controls
 *  and applications.
 *
 *  The implemented interface is defined by MTouchFeedback -class.
 *
 *  This class' main task is to have a bookkeeping of all the controls
 *  that have feedback areas defined. Services such as automatic disabling
 *  of the feedback areas of dimmed or invisible controls are implemented
 *  here.
 *
 *  @lib touchfeedback.lib
 *  @since S60 v5.0
 */
NONSHARABLE_CLASS( CTouchFeedbackImpl ): 
    public CBase,
    public MTouchFeedback
    {
public:
    /**
     * Instantiation method.
     *
     * @since S60 5.0
     * @return New CTouchFeedbackImpl instance.
     */
    static CTouchFeedbackImpl* New();

    /**
     * Destructor.
     * @since S60 5.0
     */
    virtual ~CTouchFeedbackImpl();

    /**
     * @see MTouchFeedback.
     */
    TBool TouchFeedbackSupported();

    /**
     * @see MTouchFeedback.
     */
    void SetFeedbackEnabledForThisApp( TBool aEnabled );
    
    /**
     * @see MTouchFeedback.
     */
    TBool FeedbackEnabledForThisApp(); 

    /**
     * @see MTouchFeedback.
     */
    TInt SetFeedbackArea( const CCoeControl* aControl, 
                          TUint32 aIndex,
                          TRect aRect, 
                          TTouchLogicalFeedback aFeedbackType, 
                          TTouchEventType aEventType );

    
    
    /**
     * @see MTouchFeedback.
     */
    void RemoveFeedbackArea( const CCoeControl* aControl, TUint32 aIndex );
    
    /**
     * @see MTouchFeedback.
     */
    void RemoveFeedbackForControl( const CCoeControl* aControl );
    
    /**
     * @see MTouchFeedback.
     */
    void ChangeFeedbackArea( const CCoeControl* aControl, 
                             TUint32 aIndex, 
                             TRect aNewRect );
     
    /**
     * @see MTouchFeedback.
     */
    void ChangeFeedbackType( const CCoeControl* aControl, 
                             TUint32 aIndex, 
                             TTouchLogicalFeedback aNewType );

    /**
     * @see MTouchFeedback.
     */
    void MoveFeedbackAreaToFirstPriority( const CCoeControl* aControl, 
                                          TUint32 aIndex );
         
    /**
     * @see MTouchFeedback.
     */
    void FlushRegistryUpdates( );
    
    /**
     * @see MTouchFeedback.
     */
    void InstantFeedback( TTouchLogicalFeedback aType );
        
    /**
     * @see MTouchFeedback.
     */
    void InstantFeedback( const CCoeControl* aControl,
                          TTouchLogicalFeedback aType ); 

    /**
     * @see MTouchFeedback.
     */
    TBool ControlHasFeedback( const CCoeControl* aControl );
    
    /**
     * @see MTouchFeedback.
     */
    TBool ControlHasFeedback( const CCoeControl* aControl, TUint32 aIndex );
    
    /**
     * @see MTouchFeedback.
     */
    void EnableFeedbackForControl( 
        const CCoeControl* aControl, TBool aEnable );       

    /**
     * @see MTouchFeedback.
     */
    void EnableFeedbackForControl( const CCoeControl* aControl,
                                   TBool aEnableVibra,
                                   TBool aEnableAudio );       

    /**
     * @see MTouchFeedback.
     */
    virtual void SetFeedbackEnabledForThisApp( TBool aVibraEnabled,
                                               TBool aAudioEnabled );
       
public:    
    /**
     * This function provides a way for getting the registry content.
     *
     * Used by CTouchFeedbackClient for listing all the registry
     * areas, so that they can be put to shared memory.
     *
     * @since S60 5.0
     */
    RPointerArray<CTouchFeedbackRegistry>* RegistryArray();
    
    /**
     * This function can be used for getting the number of areas in the
     * registry.
     *
     * @since S60 5.0
     * @param aAreaCount - The total number of areas in the registry.
     * @param aWindowCount - Number of different windows is returned here.
     */
    void GetAreaCount( TInt& aAreaCount, TInt& aWindowCount );
    
    /**
     * This should be called  for informing
     * that given control's state (i.e. visiblity or dimming status)
     * has been changed. This way Tactile FW knows to disable or enable
     * the feedback for that control again.
     *
     * @since S60 5.0
     * @param aControl - Pointer to the control, which visiblity 
     *                   or dimming status has changed.
     */
    void ControlVisibilityChanged( const CCoeControl* aControl );
  
    /**
     * This should be called when the layout is changed.
     *
     * @since S60 5.0     
     * The motivation is to disable this application's feedback areas
     * for this application in case pen usage is not supported in current 
     * layout.
     */
    void LayoutChanged();

public: // new API functions since 5.2, defined in MTouchFeedback
    /**
     * @see MTouchFeedback.
     */
    TBool FeedbackEnabledForThisApp( TTouchFeedbackType aFeedbackType );
    
    /**
     * @see MTouchFeedback.
     */
    void StartFeedback( const CCoeControl* aControl,
                        TTouchContinuousFeedback aType,
                        const TPointerEvent* aPointerEvent,
                        TInt aIntensity,
                        TTimeIntervalMicroSeconds32 aTimeout );

    /**
     * @see MTouchFeedback.
     */
    void ModifyFeedback( const CCoeControl* aControl,
                         TInt aIntensity );

    /**
     * @see MTouchFeedback.
     */
    void StopFeedback( const CCoeControl* aControl );

    /**
     * @see MTouchFeedback.
     */
    TInt SetFeedbackEnabledForDevice( TTouchFeedbackType aFeedbackType );

    /**
     * @see MTouchFeedback.
     */
    TTouchFeedbackType FeedbackEnabledForDevice();

    /**
     * @see MTouchFeedback.
     */
    void InstantFeedback( const CCoeControl* aControl,
                          TTouchLogicalFeedback aType,
                          const TPointerEvent& aPointerEvent );

    /**
     * @see MTouchFeedback.
     */
    TInt SetFeedbackArea( const CCoeControl* aControl, 
                          TUint32 aIndex,
                          TRect aRect, 
                          CFeedbackSpec* aFeedbackSpec );

    /**
     * @see MTouchFeedback.
     */                          
    void InstantFeedback( const CCoeControl* aControl,
                          TTouchLogicalFeedback aType,
                          TTouchFeedbackType aFeedbackType,
                          const TPointerEvent& aPointerEvent );
      
    /**
     * @see MTouchFeedback.
     */
    void InstantFeedback( TTouchLogicalFeedback aType,
                          TTouchFeedbackType aFeedbackType );

private:
    /**
     * Constructor.
     */
    CTouchFeedbackImpl();

    /**
     * 2nd phase constructor.
     */
    void ConstructL();
    
    /**
     * Actual functionality of setting a feedback area.
     *
     * The API function SetFeedbackArea does the basic parameter checks and
     * runs this function under trap harness so that we can leave here.
     *
     * @see MTouchFeedback.
     */
    void DoSetFeedbackAreaL( const CCoeControl* aControl, 
                             TUint32 aIndex,
                             TRect aRect, 
                             CFeedbackSpec* aSpec, 
                             TUint32 aClientHandle );
    
    
    /**
     * Finds window from the registry if it exists.
     *
     * Returns an index to iResolverArray, or KErrNotFound if
     * there is no entry for the window yet.
     *
     * @since S60 5.0
     * @param aWindowHandle - The window id to find from the array
     * @return KErrNotFound if an area with given ID is not found from the registry.
     */
    TInt FindWindowFromRegistry( TUint aWindowHandle );
    
    /**
     * Generates a unique identifier inside this application process.
     *
     * This is the id that is used to distinguish the area registry
     * entries from each other.
     *
     * @since S60 5.0
     * @return Unique identifier.
     */
    TUint GenerateUniqueIdL();
    
    /**
     * Can be used to check whether feedback area with given id exists or not.
     *
     * This is as helper function when generating unique identifiers for
     * new feedback areas.
     *
     * @since S60 5.0
     * @return ETrue if feedback area with given identifier exists already.
     *         EFalse otherwise.
     */
    TBool FeedbackAreaExists( TUint aAreaId );
    
    /**
     * Returns the client handle of the window, where given control
     * is located. Client handle is either directly casted from first
     * window-owning control's address, or then asked from window 
     * server.
     *
     * @since S60 5.0
     * @param aControl - Pointer to the control.
     * @return Client handle of the window where given control is.
     */
    TUint32 ClientHandle( const CCoeControl* aControl );
        
    /**
     * Finds given control from the cache.
     *
     * @since S60 5.0
     * @param aControl - Pointer to the control, which is searched
     * @return The position in cache array, where searhed entry is found,
     *         or KErrNotFound if it is not found.
     */
    TInt FindControlFromCache( const CCoeControl* aControl );
    
        
    /**
     * Finds specific feedback area from control cache.
     *
     * @since S60 5.0
     * @param aControl - Pointer to the control.
     * @param aIndex   - Area index (given by client)
     * @param aCacheIndex  - If control is found, then index to control cache is
     *                       returned here.
     * @param aAreaIndex   - If area is found, then index to area array in
     *                       corresponding control cache entry is returned
     *                       here.
     */
    void FindAreaFromCache( const CCoeControl* aControl, 
                            TUint32 aIndex, 
                            TInt& aCacheIndex, 
                            TInt& aAreaIndex );    
          
                            
    /**
     * Removes control with given index number from the control cache.
     *
     * @since S60 5.0
     * @param aControlIndex - Index number of the control in the control
     *                        array.
     */
    void RemoveControlFromCache( TInt aControlIndex );       
    
    /**
     * Can be used for querying, whether control's areas should be active
     * according to it's recorced dimming-, visibility-, and feedback
     * disabling status.
     *
     * @since S60 5.0
     * @param aControlIndex - Index number of the control in the control
     *                        array.
     * @return ETrue if control's areas should be active, EFalse otherwise.
     */
    TBool ControlsAreasActive( TInt aControlIndex );  
    
    /**
     * Can be used for disabling or enabling control's feedback areas
     * in the registry.
     *
     * This function only modifies the registry, and not the control cache.
     *
     * @since S60 5.0
     * @param aControlIndex - Index number of the control in the control
     *                        array.
     * @param aEnableVibra   - New disable / enable status for vibra
     * @param aEnableAudio   - New disable / enable status for audio
     * @param aVisible       - Visibility status of the control. If
     *                         control is invisible, it's feedback areas
     *                         will be temporarily removed from the 
     *                         registry.
     */
    void DoEnableControlsAreasInRegistry( 
        TInt aControlIndex,
        TBool aEnableVibra, 
        TBool aEnableAudio,
        TBool aVisible );               
    
private: // data

    /**
     * Array of area registries. There is one entry for each window.
     * Own.
     */
    RPointerArray<CTouchFeedbackRegistry> iRegistryArray;

    /**
     * The id what was given to the area that was last added to the array.
     */
    TUint iCurrentId;
    
    /**
     * We maintain a bookkeeping to know in case the currently used 
     * feedback area id has already reached its maximum value at 
     * least once. This way we save some time for e.g. during boot-up
     * when plenty of areas are added to the registry.
     */
    TBool iCurrentIdCounterRotated;
    
    /**
     * Pointer to client, used to inform it about registry changes.
     * Own.
     */
    CTouchFeedbackClient* iClient;
    
    /**
     * Array of controls and their feedback area ids. This way users of the
     * API don't have to store the ids themselves.
     */    
    RArray<TControlCacheEntry> iControlCache;

    /**
     * Used to store the status of audio feedback for this application.
     */
    TBool iAudioEnabledForThisApp;
    
    /**
     * Used to store the status of vibra feedback for this application.
     */
    TBool iVibraEnabledForThisApp;
    
    /**
     * For bookkeeping about feedback areas. We need this information for
     * recognizing the situation when we cannot add another area to the
     * registry anymore (for e.g. due to Id generation problem).
     */    
    TInt iNbOfFeedbackAreas;
    
    /**
     * For bookkeeping wheater pen is enabled in current layout or not. This
     * way we can optimize layout change handling so that we only react
     * to changes from non-pen enabled to pen enabled layout and vice versa,
     * but do nothing when layout changes without effect to pen support.
     */    
    TBool iPenEnabled;
    };



#endif //  C_TOUCHFEEDBACKIMPL_H
