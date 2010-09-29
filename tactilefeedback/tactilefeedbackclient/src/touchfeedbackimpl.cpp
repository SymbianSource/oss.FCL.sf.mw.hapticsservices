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
* Description:  This is the actual Client API implementation.
* Part of:      Tactile Feedback.
*
*/

#include <eikenv.h>
#include <coecntrl.h>
#include <featmgr.h>
#include <AknUtils.h> 
#include <tactilefeedbacktrace.h>
#include <touchfeedbackspec.h>

#include "touchfeedbackimpl.h"
#include "touchfeedbackclient.h"
#include "touchfeedbackregistry.h"
#include "touchfeedbackclientpanic.h"

/**
 * This is the maximum number that can be used as feedback area identifier.
 * after this the values starting from 1 are taken into use again when
 * new areas are added. This also defines the maximum number of feedback
 * areas that one application can have.
 */
const TInt KTouchMaxFeedbackAreaIdentifier = 5000;


// ======== LOCAL FUNCTIONS ========

// ---------------------------------------------------------------------------
// Cleanup items for removing latest item from the given array. These are used
// for cleanup in case something leaves when we are adding new area
// to the registry.
// ---------------------------------------------------------------------------
//
void CleanupCacheArray( TAny* aPtr )
    {
    RArray<TControlCacheEntry>* array = 
        static_cast<RArray<TControlCacheEntry>*> ( aPtr );
        
    TInt index = array->Count() - 1;
    
    // If we are removing control from cache, then we have to clean the
    // associated area array also, or otherwise there will be a memory leak.
    (*array)[index].iAreaArray.Reset();
    (*array)[index].iAreaArray.Close();
    
    array->Remove( array->Count()-1 );        
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CleanupAreaArray( TAny* aPtr )
    {
    RArray<TAreaIndexEntry>* array = 
        static_cast<RArray<TAreaIndexEntry>*> ( aPtr );
        
    array->Remove( array->Count()-1 );    
    }


// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// Constructor.
// ---------------------------------------------------------------------------
//
CTouchFeedbackImpl::CTouchFeedbackImpl() :
    iAudioEnabledForThisApp( ETrue ), 
    iVibraEnabledForThisApp( ETrue )
    {
    }

// ---------------------------------------------------------------------------
// We check first if Tactile Feedback feature is supported in this device
// and only construct the iClient -member in case needed. We thus use 
// the existence of iClient as feature check in other API functions.
//
// This also means that iClient member needs to be checked in all the API
// functions, so that we won't cause a KERN-EXEC 3 by trying to access
// a NULL pointer in case the feature is disabled.
// ---------------------------------------------------------------------------
//
void CTouchFeedbackImpl::ConstructL()
    { 
    TRACE("CTouchFeedbackImpl::ConstructL - Begin");

    // Check if the whole Tactile Feedback feature is supported or not
    
    // Initialize feature manager, just in case we happen to be the first
    // one using it in this thread.
    FeatureManager::InitializeLibL();
    
    TBool featureSupported =
        FeatureManager::FeatureSupported( KFeatureIdTactileFeedback );

    // We don't need feature manager anymore, as it is not possible
    // to change this feature on/off at run-time
    FeatureManager::UnInitializeLib();
    
    // Only create client in case feature is supported
    if ( featureSupported )
        {
        iClient = CTouchFeedbackClient::NewL( *this );
        }
    else
        {
        TRACE("Tactile Feedback feature is not supported");
        }
    
    // Check if pen usage is enabled. We use this information later for
    // disabling area registry -based feedback in case pen usage is not
    // enabled.    
    iPenEnabled = AknLayoutUtils::PenEnabled();
    
    TRACE3("CTouchFeedbackImpl::ConstructL - End, feature supported = %d, pen Enabled: %d", featureSupported, iPenEnabled);
    }

// ---------------------------------------------------------------------------
// We cannot afford to leave here (at least for the moment), because
// in that case the whole system won't boot up. Constructing of the
// client (in ConstructL) will fail if click plugin is not configured in
// WSINI.INI file, and that can happen very easily at least in the beginning
// when this is a new feature. Thus we TRAP any leaves here.
//
// This also means that iClient member needs to be checked in all the API
// functions. We will also use this for recognizing the situation when
// touch feedback is not supported (in that case we won't even try to
// instantiate the client).
// ---------------------------------------------------------------------------
//
CTouchFeedbackImpl* CTouchFeedbackImpl::New()
    {
    CTouchFeedbackImpl* self = new CTouchFeedbackImpl;
    if ( self )
        {
        TRAP_IGNORE( self->ConstructL() );    
        }    
    return self;
    }

// ---------------------------------------------------------------------------
// Destructor.
// ---------------------------------------------------------------------------
//
CTouchFeedbackImpl::~CTouchFeedbackImpl()
    {
    TRACE("CTouchFeedbackImpl::~CTouchFeedbackImpl");

    delete iClient;
    
    TInt i = 0;
    
    for ( i=0; i < iRegistryArray.Count(); i++ )
        {
        delete iRegistryArray[i];
        }
        
    for ( i=iControlCache.Count()-1; i >= 0; i-- )
        {
        RemoveControlFromCache( i );
        }
        
    iRegistryArray.Close();
    iControlCache.Close();
    }

// ---------------------------------------------------------------------------
// We return ETrue if iClient exist, because instantiation of the client is
// not done in case the feature is not enabled. And on the other hand if 
// instantiation of client fails, then using of feedback is not possible
// anyway.
// ---------------------------------------------------------------------------
//
TBool CTouchFeedbackImpl::TouchFeedbackSupported()
    {
    if ( iClient )
        {
        return ETrue;        
        }
    else
        {
        return EFalse;
        }
    }
    
// ---------------------------------------------------------------------------
// Store the current status of feedback for this application, and
// notify server of the change.
// ---------------------------------------------------------------------------
//
void CTouchFeedbackImpl::SetFeedbackEnabledForThisApp( TBool aEnabled )
    {
    TRACE2( "CTouchFeedbackImpl::SetFeedbackEnabledForThisApp %d", aEnabled );
    
    // Only notify server when enabled value really changes.
    if ( iClient && 
       (( aEnabled != iAudioEnabledForThisApp ) || 
         (aEnabled != iVibraEnabledForThisApp ) ))
        {
        iClient->RegistryChanged();        
        }
        
    iAudioEnabledForThisApp = aEnabled;      
    iVibraEnabledForThisApp = aEnabled;      
    }

// ---------------------------------------------------------------------------
// Here we just return the current status.
// ---------------------------------------------------------------------------
//
TBool CTouchFeedbackImpl::FeedbackEnabledForThisApp() 
    {
    return iAudioEnabledForThisApp || iVibraEnabledForThisApp;
    }
        
// ---------------------------------------------------------------------------
// This functions main purpose is to do all possible parameter checks to
// the arguments before actually setting the area.
// 
// #1 Check that feedback is enabled
// #2 Check that we weren't given a NULL pointer as control.
// #3 Check that feedback type and pointer event are supported
// #4 Check that we can generate a client handle from the control
//    (if not, then there is something wrong with given control)
// #5 Call overridden method SetFeedbackArea for actually setting the 
//    feedback area.
// ---------------------------------------------------------------------------
//
TInt CTouchFeedbackImpl::SetFeedbackArea( 
    const CCoeControl* aControl, 
    TUint32 aIndex, 
    TRect aRect, 
    TTouchLogicalFeedback aFeedbackType, 
    TTouchEventType aEventType )
    {
    TRACE("CTouchFeedbackImpl::SetFeedbackArea - Begin");
    
    // #1
    if ( !iClient )
        {
        return KErrNone;
        }
    
    // #2
    if ( !aControl )
        {
        TRACE("CTouchFeedbackImpl::SetFeedbackArea - Err: NULL Control");
        return KErrArgument;
        }
        
    // #3    
    if ( aFeedbackType > ETouchFeedbackSensitive || 
        ( aEventType!= ETouchEventStylusDown ) )
        {
        TRACE("CTouchFeedbackImpl::SetFeedbackArea - Err: Unsupported params");
        return KErrNotSupported;
        }
      
    // #4        
    TUint32 clientHandle = ClientHandle( aControl );
    
    if ( !clientHandle )
        {
        TRACE("CTouchFeedbackImpl::SetFeedbackArea - Err: Invalid Control");        
        return KErrArgument;
        }
    
    // #5    
    TInt err(KErrGeneral);
    CFeedbackSpec* spec = CFeedbackSpec::New();
    if (spec)
        {
        spec->AddFeedback(aEventType, aFeedbackType);
        err = SetFeedbackArea(aControl, aIndex, aRect, spec);
        delete spec;
        spec = NULL;
        }
        
    TRACE2("CTouchFeedbackImpl::SetFeedbackArea - End, err = %d", err );
    return err;    
    }
      
// ---------------------------------------------------------------------------
// Here we remove the given area from registry. 
//
// We still keep the control in the registry even if would have no areas,
// because otherwise we'll lose feedback disabled/enabled information.
//
// #1 First check that given area is actually found
// #2 Find corresponsing registry entry
// #3 Remove feedback area entry from control cache
// #4 Finally remove the actual area from registry, and notify server
// ---------------------------------------------------------------------------
//
void CTouchFeedbackImpl::RemoveFeedbackArea( 
    const CCoeControl* aControl, 
    TUint32 aIndex )
    {
    TRACE3( "CTouchFeedbackImpl::RemoveFeedbackArea: 0x%x, %d", aControl, aIndex );

    if ( iClient )
        {
        TInt cacheIndex = KErrNotFound;
        TInt areaIndex = KErrNotFound;
        
        FindAreaFromCache( aControl, aIndex, cacheIndex, areaIndex );
        
        // #1 Only do something in case the area was found
        if ( cacheIndex >=0 && areaIndex >= 0 )
            {
            // #2
            TInt registryIndex = FindWindowFromRegistry ( 
                iControlCache[cacheIndex].iClientHandle );

            TInt areaId = 
                iControlCache[cacheIndex].iAreaArray[areaIndex].iAreaId;
            
            // #3 Remove this feedback area entry
            iControlCache[cacheIndex].iAreaArray.Remove( areaIndex );
            
            iNbOfFeedbackAreas--;
            
            // #4 Remove the area from registry
            if ( registryIndex != KErrNotFound )
                {
                iRegistryArray[registryIndex]->RemoveFeedbackArea( areaId );
                
                iClient->RegistryChanged();
                }        
            }
        }
    }

// ---------------------------------------------------------------------------
// Here we return all areas related to given control.
//
// #1 Only do something in case the control is found from cache
// #2 Find corresponding registry index.
// #3 Remove all feedback areas from the registry.
// #4 Remove the control from cache
// #5 Notify server, if some areas were really removed.
// ---------------------------------------------------------------------------
//
void CTouchFeedbackImpl::RemoveFeedbackForControl( 
    const CCoeControl* aControl )
    {
    TRACE2("CTouchFeedbackImpl::RemoveFeedbackForControl: 0x%x", aControl);
    
    if ( iClient )
        {
        TInt controlIndex = FindControlFromCache( aControl );
        
        // #1
        if ( controlIndex >= 0 ) // If control was found
            {
            TBool changesMade = EFalse;

            TControlCacheEntry& entry( iControlCache[controlIndex] );
            
            // #2
            TInt registryIndex = 
                FindWindowFromRegistry( entry.iClientHandle );
            
            // #3 If registry entry was found, then remove all feedback
            // areas.
            if ( registryIndex >= 0) 
                {
                for ( TInt i=0; i < entry.iAreaArray.Count(); i++ )
                    {
                    iRegistryArray[registryIndex]->
                            RemoveFeedbackArea( 
                            entry.iAreaArray[i].iAreaId );
                            
                    changesMade = ETrue;   
                    
                    iNbOfFeedbackAreas--;     
                    }
                }
            
            // #4 Anyway remove controls feedback areas from cache
            RemoveControlFromCache( controlIndex );
           
            // #5 Notify server of changes
            if ( changesMade )
                {
                iClient->RegistryChanged();
                }
            }        
        }
    }
    
// ---------------------------------------------------------------------------
// Here we change the rectangle of existing feedback area.
//
// #1 Only do something in case the area is really found.
// #2 Find corresponding registry entry
// #3 If the entry was found, then change the feedback area
// #4 Only notify the server when the area really changed
// ---------------------------------------------------------------------------
//
void CTouchFeedbackImpl::ChangeFeedbackArea( const CCoeControl* aControl,
                                             TUint32 aIndex, 
                                             TRect aNewRect )
    {
    TRACE( "CTouchFeedbackImpl::ChangeFeedbackArea: 0x%x" );

    if ( iClient )
        {
        TInt cacheIndex = KErrNotFound;
        TInt areaIndex  = KErrNotFound;

        FindAreaFromCache( aControl, aIndex, cacheIndex, areaIndex );
        
        // #1
        if ( cacheIndex >= 0 && areaIndex >= 0 )
            {
            TInt id = 
                iControlCache[cacheIndex].iAreaArray[areaIndex].iAreaId;
            
            // #2 
            TInt registryIndex = 
                FindWindowFromRegistry ( 
                    iControlCache[cacheIndex].iClientHandle );

            // #3
            if ( registryIndex != KErrNotFound )
                {
                TBool changed = 
                    iRegistryArray[registryIndex]->ChangeFeedbackArea( 
                        id, 
                        aNewRect );

                // #4 Only send upates to server when something 
                //    really changed.
                if ( changed )
                    {
                    iClient->RegistryChanged();                
                    }
                }           
            }        
        }
    }

// ---------------------------------------------------------------------------
// Here we change the feedback type of existing feedback area.
//
// #1 Only do something in case the area is really found.
// #2 Find corresponding registry entry
// #3 If the entry was found, then change the feedback area
// #4 Only notify the server when the feedback type really changed
// ---------------------------------------------------------------------------
//
void CTouchFeedbackImpl::ChangeFeedbackType( const CCoeControl* aControl,
                                             TUint32 aIndex, 
                                             TTouchLogicalFeedback aNewType )
    {
    TRACE4("CTouchFeedbackImpl::ChangeFeedbackType: 0x%x, %d, %d", aControl, aIndex, aNewType);

    if ( iClient )
        {
        TInt cacheIndex = KErrNotFound;
        TInt areaIndex  = KErrNotFound;

        FindAreaFromCache( aControl, aIndex, cacheIndex, areaIndex );
        
        // #1
        if ( cacheIndex >= 0 && areaIndex >= 0 )
            {
            TInt id = iControlCache[cacheIndex].iAreaArray[areaIndex].iAreaId;
            
            // #2
            TInt registryIndex = 
                FindWindowFromRegistry ( 
                    iControlCache[cacheIndex].iClientHandle );

            // #3
            if ( registryIndex != KErrNotFound )
                {
                TBool changed = 
                    iRegistryArray[registryIndex]->ChangeFeedbackType( 
                        id, 
                        aNewType,
                        // Feedback type change for up event is not supported 
                        // by this function.
                        static_cast<TTouchLogicalFeedback>( 0xFFFFFFFF ) );

                // #4 Only send upates to server when something 
                //    really changed.
                if ( changed )
                    {
                    iClient->RegistryChanged();                
                    }
                }           
            }        
        }
    }    
   
// ---------------------------------------------------------------------------
// Here we change move the given feedback area to first priority, so that
// it will be the topmost in its window.
//
// #1 Only do something in case the area is really found.
// #2 Find corresponding registry entry
// #3 If the entry was found, then change the feedback area
// #4 Only notify the server when the move was successfull
// ---------------------------------------------------------------------------
//
void CTouchFeedbackImpl::MoveFeedbackAreaToFirstPriority( 
    const CCoeControl* aControl, 
    TUint32 aIndex )
    {
    if ( iClient )
        {
        TInt cacheIndex = KErrNotFound;
        TInt areaIndex  = KErrNotFound;

        FindAreaFromCache( aControl, aIndex, cacheIndex, areaIndex );
        
        // #1
        if ( cacheIndex >= 0 && areaIndex >= 0 )
            {
            TInt id = iControlCache[cacheIndex].iAreaArray[areaIndex].iAreaId;
            
            // #2
            TInt registryIndex = 
                FindWindowFromRegistry ( 
                    iControlCache[cacheIndex].iClientHandle );

            // #3
            if ( registryIndex != KErrNotFound )
                {
                TInt err = 
                    iRegistryArray[registryIndex]->
                        MoveFeedbackAreaToFirstPriority( id );

                // #4
                if ( err == KErrNone )
                    {
                    TRACE3("CTouchFeedbackImpl::MoveFeedbackAreaToFirstPriority 0x%x, %d", aControl, aIndex);
                    iClient->RegistryChanged();
                    }
                }  
            }
        }
    }   

// ---------------------------------------------------------------------------
// Pass request to server in case feedback is enabled for this 
// application.
// ---------------------------------------------------------------------------
//
void CTouchFeedbackImpl::InstantFeedback( TTouchLogicalFeedback aType )
    {
    if ( iClient && ( iAudioEnabledForThisApp || iVibraEnabledForThisApp ) )
        {
        TRACE2("CTouchFeedbackImpl::InstantFeedback %d", aType);
        iClient->ImmediateFeedback( 
            aType, 
            iVibraEnabledForThisApp, 
            iAudioEnabledForThisApp );        
        }
    }
    
// ---------------------------------------------------------------------------
// In this overload we only play direct feedback if the feedback has
// not been disabled for this control.
// ---------------------------------------------------------------------------
//
void CTouchFeedbackImpl::InstantFeedback( 
    const CCoeControl* aControl,
    TTouchLogicalFeedback aType )
    {
    if ( iClient && ( iAudioEnabledForThisApp || iVibraEnabledForThisApp ) )
        {
        
        // First take application level vibra and audio enablers
        // as default
        TBool vibraEnabled = iVibraEnabledForThisApp;
        TBool audioEnabled = iAudioEnabledForThisApp;

        TInt cacheIndex = FindControlFromCache( aControl );
        
        
        // Check control level disablers for vibra and audio.
        // (These can only prevent vibra or audio effect playback,
        //  but not to force it on)
        if ( cacheIndex >=0 && vibraEnabled ) 
            {
            vibraEnabled = !iControlCache[cacheIndex].iVibraDisabled;
            }
            
        if ( cacheIndex >=0 && audioEnabled ) 
            {
            audioEnabled = !iControlCache[cacheIndex].iAudioDisabled;
            }
            
        TRACE3("CTouchFeedbackImpl::InstantFeedback 0x%x, %d", aControl, aType);
        iClient->ImmediateFeedback( aType, vibraEnabled, audioEnabled );   
        }
    }    

// ---------------------------------------------------------------------------
// It is not enough to check that the control exists in the cache, we
// also need to check that it actually has some areas registered.
// ---------------------------------------------------------------------------
//
TBool CTouchFeedbackImpl::ControlHasFeedback( const CCoeControl* aControl )
    {
    TBool hasFeedback = EFalse;
    
    if ( iClient )
        {
        for ( TInt i=0; i < iControlCache.Count(); i++ )
            {
            if ( iControlCache[i].iControl == aControl )
                {
                if ( iControlCache[i].iAreaArray.Count() > 0 )
                    {
                    hasFeedback =  ETrue;                    
                    }
                    
                // Break out from loop anyway, since we found the
                // control
                break;
                }
            }        
        }
        
    return hasFeedback;
    }

// ---------------------------------------------------------------------------
// Control has feedback if it has a feedback identifier with given index
// number.
// ---------------------------------------------------------------------------
//
TBool CTouchFeedbackImpl::ControlHasFeedback( 
    const CCoeControl* aControl, 
    TUint32 aIndex )
    {
    TBool hasArea = EFalse;
    
    if ( iClient )
        {
        TInt cacheIndex = KErrNotFound;
        TInt areaIndex  = KErrNotFound;
        
        FindAreaFromCache( aControl, aIndex, cacheIndex, areaIndex );
        
        if ( cacheIndex >= 0 && areaIndex >= 0 )
            {
            hasArea = ETrue;
            }
        }

    return hasArea;
    }  
  
// ---------------------------------------------------------------------------
// This overload is only wrapper to the real function
// ---------------------------------------------------------------------------
//
void CTouchFeedbackImpl::EnableFeedbackForControl( 
    const CCoeControl* aControl, 
    TBool aEnable )
    {
    EnableFeedbackForControl( aControl, aEnable, aEnable );
    }    

// ---------------------------------------------------------------------------
// #1 Find control from the cache
// #2 In case control was found, disable/enable it's registry entries, and
//    also take into accound it's dimming status
// #3 If control was not found, then add it to cache
// ---------------------------------------------------------------------------
//
void CTouchFeedbackImpl::EnableFeedbackForControl( 
    const CCoeControl* aControl,
    TBool aEnableVibra,
    TBool aEnableAudio )
    {
    TRACE4( "CTouchFeedbackImpl::EnableFeedbackForControl - 0x%x Vibra:%d Audio:=%d", 
           aControl, aEnableVibra, aEnableAudio );

    if ( aControl )
        {
        // #1
        TInt cacheIndex = FindControlFromCache( aControl );

        if ( cacheIndex >= 0 )
            {
            // #2
            TControlCacheEntry& entry ( iControlCache[cacheIndex] );
            
            entry.iVibraDisabled = !aEnableVibra;
            entry.iAudioDisabled = !aEnableAudio;
            
            // Let's update visibility and dimming information also now
            // that we have the pointer to control (due to broken object
            // provider chain we might not have the correct information on 
            // the moment).
            entry.iDimmed  = aControl->IsDimmed();
            entry.iVisible = aControl->IsVisible();
            
            if ( ControlsAreasActive( cacheIndex ) )
                {
                // If control is active (undimmed and visible), then we 
                // set the areas according to parameters.
                DoEnableControlsAreasInRegistry( cacheIndex, 
                                                 aEnableVibra, 
                                                 aEnableAudio,
                                                 aControl->IsVisible() );
                }
            else
                {
                // If control is not active, then we disable all feedback.
                DoEnableControlsAreasInRegistry( cacheIndex, 
                                                 EFalse, 
                                                 EFalse, 
                                                 aControl->IsVisible() );
                }
            }
        else if ( !aEnableVibra || !aEnableAudio  )
            {
            // #3 We have to add control to the cache in case
            // it is not there yet (and disabling is wanted), because
            // otherwise it will be impossible to block feedback
            TUint32 clientHandle = ClientHandle( aControl );
            
            // We need to rely elsewhere that client handle exist if control
            // is found in the cache --> Only make new entry if client
            // handle can be aquired.
            if ( clientHandle > 0 )
                {
                TControlCacheEntry newCacheEntry;
                
                newCacheEntry.iControl        = aControl;
                newCacheEntry.iClientHandle   = clientHandle;
                newCacheEntry.iVibraDisabled  = !aEnableVibra;
                newCacheEntry.iAudioDisabled  = !aEnableAudio;
                newCacheEntry.iVisible        = aControl->IsVisible();
                newCacheEntry.iDimmed         = aControl->IsDimmed();
                // if append fail just make its action like before
                TRAP_IGNORE( iControlCache.AppendL( newCacheEntry ) );
                }
            }        
        }
    }

// ---------------------------------------------------------------------------
// Store the current status of audio and vibra feedbacks for this 
// application, and notify server of the change.
// ---------------------------------------------------------------------------
//
void CTouchFeedbackImpl::SetFeedbackEnabledForThisApp( 
    TBool aVibraEnabled, 
    TBool aAudioEnabled )
    {
    TRACE3( "CTouchFeedbackImpl::SetFeedbackEnabledForThisApp %d %d", aVibraEnabled, aAudioEnabled );
    
    // Only notify server when enabled value really changes.
    if ( iClient && 
       (( aAudioEnabled != iAudioEnabledForThisApp ) || 
         (aVibraEnabled != iVibraEnabledForThisApp ) ))
        {
        iClient->RegistryChanged();        
        }
        
    iAudioEnabledForThisApp = aAudioEnabled;      
    iVibraEnabledForThisApp = aVibraEnabled;      
    }

// ---------------------------------------------------------------------------
// Simple utility for CTouchFeedbackClient
// ---------------------------------------------------------------------------
//
RPointerArray<CTouchFeedbackRegistry>* CTouchFeedbackImpl::RegistryArray()
    {
    return &iRegistryArray;
    }
        
// ---------------------------------------------------------------------------
// Simple utility for CTouchFeedbackClient
//
// We return zero in case pen is not supported in current layout. This way
// area registry is disabled in non-touch powered layouts, but feedback areas
// are still kept in memory so that feedback can be enabled again in case 
// layout changes.
// ---------------------------------------------------------------------------
//
void CTouchFeedbackImpl::GetAreaCount( TInt& aAreaCount, TInt& aWindowCount )
    {
    aWindowCount = 0;
    aAreaCount = 0;
    
    if ( iPenEnabled )
        {
        aWindowCount = iRegistryArray.Count();
        aAreaCount =  iNbOfFeedbackAreas;            
        }
    }
    
// ---------------------------------------------------------------------------
// This function is called by the application framework when any control's
// visibility or dimming status has changed.
//
// We need to determine here whether the given control has any feedback,
// and whether its current state requires a change in feedback
// areas (for e.g. feedback areas will be disabled for any dimmed control).
//
// #1 Find control from cache (we don't need to do anything in case it
//    is not found.
// #2 Check if control's feedback areas are now active (based on dimming,
//    visibility and feedback disabling). Also check control's current 
//    visibility status as itself, as this determines whether the area will
//    be put to registry at all.
// #3 Save control's current dimming and visibility status
// #4 Check again if control's areas should  be active
// #5 Disable/Enable areas in case needed
// ---------------------------------------------------------------------------
//
void CTouchFeedbackImpl::ControlVisibilityChanged( 
    const CCoeControl* aControl )
    {
    if ( iClient && aControl )
        {
        // #1
        TInt cacheIndex = FindControlFromCache( aControl );
        
        if ( cacheIndex >= 0 )
            {
            // #2
            TBool oldStatus = ControlsAreasActive( cacheIndex );
            TBool oldVisible = iControlCache[cacheIndex].iVisible;
            
            // #3
            iControlCache[cacheIndex].iVisible = aControl->IsVisible();
            iControlCache[cacheIndex].iDimmed  = aControl->IsDimmed();
                
            
            // #4
            TBool newStatus = ControlsAreasActive( cacheIndex );
            
            // #5
            if ( ( oldStatus != newStatus ) || 
                 ( oldVisible != aControl->IsVisible() ) )
                {
                if ( newStatus ) // If control is now active
                    {
                    DoEnableControlsAreasInRegistry( 
                        cacheIndex, 
                        !iControlCache[cacheIndex].iVibraDisabled,
                        !iControlCache[cacheIndex].iAudioDisabled,
                        aControl->IsVisible() );
                    }
                else
                    {
                    DoEnableControlsAreasInRegistry( cacheIndex, 
                                                     EFalse, 
                                                     EFalse,
                                                     aControl->IsVisible() );
                    }
                }
            }
        }
    }

// ---------------------------------------------------------------------------
// Here we check whether the pen support status has changed, and update
// registry in case needed.
//
// Notice that this has not effect on direct feedback: It is completely on
// UI control's responsibility to check if pen is supported with direct
// feedback (direct feedback is not automatically disabled because that
// would cause problems in compatibility mode).
//
// Also notice that we don't actually destroy any feedback areas when pen
// usage is not supported: We just report zero areas to server side, but
// still keep everything in memory (see GetAreaCount -function)
// ---------------------------------------------------------------------------
//
void CTouchFeedbackImpl::LayoutChanged()
    {    
    if ( iClient )
        {
        TBool penEnabledNow = AknLayoutUtils::PenEnabled();

        TRACE2("CTouchFeedbackImpl::LayoutChanged, PenEnabled = %d", penEnabledNow);
        
        if ( penEnabledNow != iPenEnabled )
            {
            iPenEnabled = penEnabledNow;
            
            iClient->RegistryChanged();                    
            }        
        }
    }

// new functions from MTouchFeedback that came in 5.2
// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TBool CTouchFeedbackImpl::FeedbackEnabledForThisApp( TTouchFeedbackType aFeedbackType )
    {
    TBool feedbackEnabled( ETrue );
    
    if ( (aFeedbackType & ETouchFeedbackAudio) && !iAudioEnabledForThisApp )
        {
        feedbackEnabled = EFalse;
        }

    if ( (aFeedbackType & ETouchFeedbackVibra) && !iVibraEnabledForThisApp )
        {
        feedbackEnabled = EFalse;
        }        
        
    if ( !aFeedbackType )        
        {
        feedbackEnabled = EFalse;
        }
   
    return feedbackEnabled;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CTouchFeedbackImpl::StartFeedback( const CCoeControl* aControl,
                                        TTouchContinuousFeedback aType,
                                        const TPointerEvent* /*aPointerEvent*/,
                                        TInt aIntensity,
                                        TTimeIntervalMicroSeconds32 aTimeout )
    {
    if ( iClient && ( iAudioEnabledForThisApp || iVibraEnabledForThisApp ) )
        {
        // First take application level vibra and audio enablers
        // as default
        TBool vibraEnabled = iVibraEnabledForThisApp;
        TBool audioEnabled = iAudioEnabledForThisApp;

        TInt cacheIndex = FindControlFromCache( aControl );
        
        
        // Check control level disablers for vibra and audio.
        // (These can only prevent vibra or audio effect playback,
        //  but not to force it on)
        if ( cacheIndex >=0 && vibraEnabled ) 
            {
            vibraEnabled = !iControlCache[cacheIndex].iVibraDisabled;
            }
            
        if ( cacheIndex >=0 && audioEnabled ) 
            {
            audioEnabled = !iControlCache[cacheIndex].iAudioDisabled;
            }
            
        if ( vibraEnabled || audioEnabled )
            {
            TRACE4("CTouchFeedbackImpl::StartFeedback, type:=%d intensity:%d, timeout:%d",
                    aType, aIntensity, aTimeout.Int());
            TUint32 clientHandle = ClientHandle( aControl );
            iClient->StartFeedback( clientHandle, 
                                    aType, 
                                    aIntensity, 
                                    aTimeout );    
            }
        }    
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CTouchFeedbackImpl::ModifyFeedback( const CCoeControl* aControl,
                                         TInt aIntensity )
    {
    if ( iClient )
        {
        TRACE2("CTouchFeedbackImpl::ModifyFeedbac, intensity:%d, timeout:%d", aIntensity);
        TUint32 clientHandle = ClientHandle( aControl );
        iClient->ModifyFeedback( clientHandle, aIntensity );
        }    
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//                             
void CTouchFeedbackImpl::StopFeedback( const CCoeControl* aControl )
    {
    if ( iClient )
        {
        TUint32 clientHandle = ClientHandle( aControl );
        iClient->StopFeedback( clientHandle );
        }        
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TInt CTouchFeedbackImpl::SetFeedbackEnabledForDevice( TTouchFeedbackType aFeedbackType )
    {
    TInt ret( KErrGeneral );
    if ( iClient )
        {
        ret = iClient->SetFeedbackEnabledForDevice( aFeedbackType );
        }
    return ret;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TTouchFeedbackType CTouchFeedbackImpl::FeedbackEnabledForDevice()
    {
    TTouchFeedbackType enabled( static_cast<TTouchFeedbackType>(0) );
    if ( iClient )
        {
        enabled = iClient->FeedbackEnabledForDevice();
        }
    return enabled;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CTouchFeedbackImpl::InstantFeedback( const CCoeControl* aControl,
                                          TTouchLogicalFeedback aType,
                                          const TPointerEvent& /*aPointerEvent*/ )
    {
    InstantFeedback(aControl, aType);
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TInt CTouchFeedbackImpl::SetFeedbackArea( const CCoeControl* aControl, 
                                          TUint32 aIndex,
                                          TRect aRect, 
                                          CFeedbackSpec* aFeedbackSpec )
    {
    // #1
    if ( !iClient )
        {
        return KErrNone;
        }
    
    // #2
    if ( !aControl )
        {
        TRACE("CTouchFeedbackImpl::SetFeedbackArea(spec) - Err: NULL Control");
        return KErrArgument;
        }
        
    // #4        
    TUint32 clientHandle = ClientHandle( aControl );
    
    if ( !clientHandle )
        {
        TRACE("CTouchFeedbackImpl::SetFeedbackArea(spec) - Err: Invalid Control");        
        return KErrArgument;
        }
    
    // #5    
    TRAPD( err, 
        DoSetFeedbackAreaL( 
            aControl, 
            aIndex, 
            aRect, 
            aFeedbackSpec, 
            clientHandle ) );

    if (err)
        {
        TRACE2("CTouchFeedbackImpl::SetFeedbackArea(spec) err = %d", err );
        }
    
    return err;    
    }
    
// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//    
void CTouchFeedbackImpl::InstantFeedback( const CCoeControl* aControl,
                                          TTouchLogicalFeedback aType,
                                          TTouchFeedbackType aFeedbackType,
                                          const TPointerEvent& /*aPointerEvent*/ )
    {
    if ( iClient && ( iAudioEnabledForThisApp || iVibraEnabledForThisApp ) )
        {
        // Initialize vibra and audio enablers as given in param.
        TBool vibraEnabled = aFeedbackType & ETouchFeedbackVibra;
        TBool audioEnabled = aFeedbackType & ETouchFeedbackAudio;

        // Check application level vibra and audio enablers
        if ( !iVibraEnabledForThisApp )
            {
            vibraEnabled = EFalse;
            }
        if ( !iAudioEnabledForThisApp )
            {
            audioEnabled = EFalse;
            }            

        TInt cacheIndex = FindControlFromCache( aControl );
        
        
        // Check control level disablers for vibra and audio.
        // (These can only prevent vibra or audio effect playback,
        //  but not to force it on)
        if ( cacheIndex >=0 && vibraEnabled ) 
            {
            vibraEnabled = !iControlCache[cacheIndex].iVibraDisabled;
            }
            
        if ( cacheIndex >=0 && audioEnabled ) 
            {
            audioEnabled = !iControlCache[cacheIndex].iAudioDisabled;
            }
            
        TRACE5("CTouchFeedbackImpl::InstantFeedback, 0x%x, type:=%d Vibra:%d Audio:%d", 
               aControl, aType, vibraEnabled, audioEnabled);
        iClient->ImmediateFeedback( aType, vibraEnabled, audioEnabled );   
        }
    }
        
// ---------------------------------------------------------------------------
// CTouchFeedbackImpl::InstantFeedback
// ---------------------------------------------------------------------------
//    
void CTouchFeedbackImpl::InstantFeedback( TTouchLogicalFeedback aType,
                                          TTouchFeedbackType aFeedbackType )
    {
    if ( iClient && ( iAudioEnabledForThisApp || iVibraEnabledForThisApp ) )
        {
        // Initialize vibra and audio enablers as given in param.
        TBool vibraEnabled = aFeedbackType & ETouchFeedbackVibra;
        TBool audioEnabled = aFeedbackType & ETouchFeedbackAudio;

        // Check application level vibra and audio enablers
        if ( !iVibraEnabledForThisApp )
            {
            vibraEnabled = EFalse;
            }
        if ( !iAudioEnabledForThisApp )
            {
            audioEnabled = EFalse;
            }

        TRACE4( "CTouchFeedbackImpl::InstantFeedback, type:=%d Vibra:%d Audio:%d",
                aType, vibraEnabled, audioEnabled );
        iClient->ImmediateFeedback( aType, vibraEnabled, audioEnabled );
        }
    }

// ---------------------------------------------------------------------------
// Here we do the actual work for adding new area to the registry
// (or updating an existing one).
//
// #1 Check that control exist in the cache
//    #2a If it does not exist, then add it
//    #2b If control exists, but area not, then add new area
//    #2c If control and area both exist, then use existing id
// #3 Find corresponding registry entry
// #4 Create new entry if this was the first area for this window
// #5 Determine if new area should actually be active, and then
//    add it to registry
// #6 Inform server if we added an active area
//
// We use specific cleanup items for removing the added area and/or control
// from the control cache in caes adding of the area to the actual 
// registry fails. This way actual registry and control cache will remain
// in sync no matter what happens.
// ---------------------------------------------------------------------------
//
void CTouchFeedbackImpl::DoSetFeedbackAreaL( 
                                const CCoeControl* aControl, 
                                TUint32 aIndex,
                                TRect aRect, 
                                CFeedbackSpec* aFeedbackSpec, 
                                TUint32 aClientHandle )
    {
    TInt id = KErrNotFound;
    TInt nbOfCleanupItems = 0;
    TBool newArea = EFalse;
    
    // #1
    TInt cacheIndex = KErrNotFound;
    TInt areaIndex  = KErrNotFound;
    FindAreaFromCache( aControl, aIndex, cacheIndex, areaIndex );

    // #2a
    if ( cacheIndex < 0 )
        {
        id = GenerateUniqueIdL();
        
        TControlCacheEntry newCacheEntry;
        
        newCacheEntry.iControl            = aControl;
        newCacheEntry.iClientHandle       = aClientHandle;
        newCacheEntry.iVibraDisabled      = EFalse;
        newCacheEntry.iAudioDisabled      = EFalse;
        newCacheEntry.iVisible         = aControl->IsVisible();
        newCacheEntry.iDimmed          = aControl->IsDimmed();
        
        // New entry will be added at the end of array
        // This assignment will also make it possible to use
        // "cacheIndex" -variable in the rest of the function
        // in all cases.
        cacheIndex = iControlCache.Count();
        
        iControlCache.AppendL( newCacheEntry );  
        
        // Use cleanup item to remove the control in case something
        // fails later in this function.
        CleanupStack::PushL(
            TCleanupItem( CleanupCacheArray, &iControlCache ) );  
        nbOfCleanupItems++;    
        
        TAreaIndexEntry newAreaEntry;
        
        newAreaEntry.iIndex  = aIndex;
        newAreaEntry.iAreaId = id;
        
        iControlCache[cacheIndex].iAreaArray.AppendL( newAreaEntry );
        
        // Use cleanup item to remove the area id in case something
        // fails later in this function.
        CleanupStack::PushL( 
            TCleanupItem( CleanupAreaArray, 
                          &iControlCache[cacheIndex].iAreaArray) );  
        nbOfCleanupItems++;    
                 
        newArea = ETrue;   
        
        // Enable control state change reports on the new control
        CCoeControl* tmp =  const_cast<CCoeControl*>( aControl );
        tmp->EnableReportControlStateChange( ETrue );
        }
    // #2b   
    else if ( areaIndex < 0 )
        {
        id = GenerateUniqueIdL();
        
        TAreaIndexEntry newAreaEntry;
    
        newAreaEntry.iIndex  = aIndex;
        newAreaEntry.iAreaId = id;

        iControlCache[cacheIndex].iAreaArray.AppendL( newAreaEntry );
        
        CleanupStack::PushL( 
            TCleanupItem( CleanupAreaArray, 
                          &iControlCache[cacheIndex].iAreaArray) );  
        nbOfCleanupItems++;    

        newArea = ETrue;   
        }
    // #2c
    else
        {
        id = iControlCache[cacheIndex].iAreaArray[areaIndex].iAreaId;
        }
               
    // #3              
    TInt registryIndex = FindWindowFromRegistry ( aClientHandle );
    
    // #4   
    if (  registryIndex == KErrNotFound )
        {
        // There isn't yet a registry entry for this window
        // --> Create it
        CTouchFeedbackRegistry* newRegistry = 
            CTouchFeedbackRegistry::NewL( aClientHandle );
            
        CleanupStack::PushL( newRegistry );
        
        registryIndex = iRegistryArray.Count();
                
        iRegistryArray.AppendL( newRegistry );
        
        CleanupStack::Pop( newRegistry );
        }

    // #5 Determine what is the vibra and audio status for the new area
    //    (if control is invisible or dimmed then both will be disabled)    
    TBool vibraEnabled = EFalse;
    TBool audioEnabled = EFalse;
    
    TBool areaActive = ControlsAreasActive( cacheIndex );
    
    if ( areaActive )
        {
        vibraEnabled = !iControlCache[cacheIndex].iVibraDisabled;
        audioEnabled = !iControlCache[cacheIndex].iAudioDisabled;
        }
        
    
    RArray<TTactileFbItem> feedbackEntries;
    CleanupClosePushL( feedbackEntries );
    TTouchEventType eventType(ETouchEventStylusDown);
    TTouchLogicalFeedback feedbackTypeDown = ETouchFeedbackNone;
    TTouchLogicalFeedback feedbackTypeUp   = ETouchFeedbackNone;
    TTouchFeedbackType feedbackDown = ETouchFeedbackVibra;
    TTouchFeedbackType feedbackUp   = ETouchFeedbackVibra;
    aFeedbackSpec->GetFeedbackSpec(feedbackEntries);
    TInt feedbackItems = feedbackEntries.Count();
    for (TInt i=0;i<feedbackItems;i++)
        {
        TTactileFbItem item = feedbackEntries[i];
        eventType = item.iEventType;
        
        if ( item.iEventType == ETouchEventStylusDown )
            {
            feedbackTypeDown = item.iFeedback;
            feedbackDown = item.iFeedbackType;
            }
        else if ( item.iEventType == ETouchEventStylusUp )
            {
            feedbackTypeUp = item.iFeedback;
            feedbackUp = item.iFeedbackType;
            }
        }
    if ( feedbackItems > 0 )
        {
        iRegistryArray[registryIndex]->AddFeedbackAreaL( 
            aRect, feedbackTypeDown, feedbackDown, feedbackTypeUp, feedbackUp, 
            eventType, id, vibraEnabled, audioEnabled, aControl->IsVisible() );
        }
    CleanupStack::PopAndDestroy( &feedbackEntries );
        
    // We can now remove the cleanup items, because nothing can fail 
    // anymore.    
    CleanupStack::Pop( nbOfCleanupItems );    
    
    // We can also update the counter now, when we know that the area
    // was actually added
    if ( newArea )
        {
        iNbOfFeedbackAreas++;
        }

    // #6 We only need to inform server in case we added an active area
    // to the registry.
    if ( areaActive ) 
        {
        iClient->RegistryChanged();        
        }
    }
        
// ---------------------------------------------------------------------------
// A simple for -loop should be enough here, as we are not likely to have
// that many windows in one application process.
// ---------------------------------------------------------------------------
//
TInt CTouchFeedbackImpl::FindWindowFromRegistry( TUint aWindowHandle )
    {
    TInt index = KErrNotFound;
    
    for ( TInt i=0; i < iRegistryArray.Count(); i++ )
        {
        if ( iRegistryArray[i]->WindowHandle() == aWindowHandle )
            {
            index = i;
            break;
            }
        }
        
    return index;
    }
    
// ---------------------------------------------------------------------------
// Just ask client to do updates immediately
// ---------------------------------------------------------------------------
//
void CTouchFeedbackImpl::FlushRegistryUpdates( )
    {
    if ( iClient )
        {
        TRACE("CTouchFeedbackImpl::FlushRegistryUpdates" ); 
        iClient->FlushUpdates();
        }
    }    
    
// ---------------------------------------------------------------------------
// We use integer numers starting from 1 as area identifiers. 
//
// There is a bookkeeping of last used identifier, and we'll start from one
// again after reaching the maximum value. 
//
// There is also a separate bookkeeping to know if our id values have been
// rotated at least once or not. From that we know if we have to check that
// the id is not in use already (if we are on the first round, then there 
// cannot be any area with the new id).
//
// Notice that it should be checked already _before_ calling this function,
// whether it is actually possible to find an unused identifier anymore.
// ---------------------------------------------------------------------------
//
TUint CTouchFeedbackImpl::GenerateUniqueIdL()
    {
    // First check that there actually is an identifier that we can find
    // (i.e. check that maximum number of areas has not been reached)
    if ( iNbOfFeedbackAreas >= KTouchMaxFeedbackAreaIdentifier )
        {
        User::Leave( KErrNoMemory );
        }
    
    iCurrentId++;
    
    if ( iCurrentId > KTouchMaxFeedbackAreaIdentifier )
        {
        iCurrentId = 1;
        
        iCurrentIdCounterRotated = ETrue;
        }
        
    if ( iCurrentIdCounterRotated )
        {
        while ( FeedbackAreaExists( iCurrentId ) )
            {
            iCurrentId++;
            
            if ( iCurrentId > KTouchMaxFeedbackAreaIdentifier )
                {
                iCurrentId = 1;
                }
            }
        
        }
    
    return iCurrentId;
    }
    
// ---------------------------------------------------------------------------
// Scan through local registry in order to know whether the area with
// given identifier exists.
//
// This function is only used when creating a new unique identifier.
// ---------------------------------------------------------------------------
//
TBool CTouchFeedbackImpl::FeedbackAreaExists( TUint aAreaId )
    {
    TBool areaExists = EFalse;
    
    for ( TInt i=0; i < iRegistryArray.Count() && !areaExists; i++ )
        {
        RArray<TFeedbackEntry>* entryArray = 
            iRegistryArray[i]->WindowRegistry(); 
        
        for ( TInt j=0; j < entryArray->Count() && !areaExists; j++ )
            {
            if ( (*entryArray)[j].iId == aAreaId )
                {
                areaExists = ETrue;
                }
            }
        }   
    
    return areaExists;
    }
        
// ---------------------------------------------------------------------------
// Because we use the so called "client handle" for identifying windows on
// client- and server sides, we must be able to generate a client handle
// from CCoeControl -pointer.
//
// The algorithm:
//
// #1 Loop as long as we haven't reached the top-level control, and we still
//    haven't found the first window-owning control.
// 
// #2 If this is a window-owning control, then it's address is the
//    client-side handle
//
// #3 Otherwise move up to next parent
//
// #4 If this control doesn't have paren't pointer, then try to get
//    a pointer to this control's window.
//
// #5 If we got the window, then use the ClientHandle -function for 
//    retrieving the pointer from server side
//
// #6 If we don't have parent- or window pointers, then give up.
// ---------------------------------------------------------------------------
//
TUint32 CTouchFeedbackImpl::ClientHandle( const CCoeControl* aControl )
    {
    TUint32 clientHandle = 0;
    
    const CCoeControl* parent = aControl;
    
    // #1
    while ( clientHandle == 0 && parent )
        {
        if ( parent->OwnsWindow() )
            {
            // #2
            clientHandle = reinterpret_cast<TUint32>( parent );            
            }
        else
            {
            // #3
            parent = parent->Parent();
            
            if ( !parent )
                {      
                // #4          
                RDrawableWindow* window = aControl->DrawableWindow();
                
                if ( window )
                    {
                    // #5
                    TRACE("CTouchFeedbackImpl::ClientHandle - Ask handle from wserv - begin" );
                    clientHandle = window->ClientHandle();
                    TRACE("CTouchFeedbackImpl::ClientHandle - Ask handle from wserv - end" );
                    }
                else
                    {
                    // #6
                    TRACE("CTouchFeedbackImpl::ClientHandle - Error: No window defined, not possible to get handle!!");
                    }
                
                }
            }        
        }
        
    return clientHandle;
    }
    
// ---------------------------------------------------------------------------
// We could optimize this search in case we get very many controls to the
// array, but so far it hasn't been necessary.
// ---------------------------------------------------------------------------
//
TInt CTouchFeedbackImpl::FindControlFromCache( 
    const CCoeControl* aControl )
    {
    TInt position = KErrNotFound;
    
    for ( TInt i=0; i < iControlCache.Count(); i++ )
        {
        if ( iControlCache[i].iControl == aControl )
            {
            position = i;
            break;
            }
        }
        
    return position;    
    }
        
// ---------------------------------------------------------------------------
// If this search is starting to take too long time, then the best 
// optimization would be to optimize the FindControlFromCache -function,
// as there can potentially be very many controls, but not likely very many
// ares per each control.
// ---------------------------------------------------------------------------
//
void CTouchFeedbackImpl::FindAreaFromCache( 
    const CCoeControl* aControl, 
    TUint32 aIndex, 
    TInt& aCacheIndex, 
    TInt& aAreaIndex )
    {
    aAreaIndex  = KErrNotFound;

    aCacheIndex = FindControlFromCache( aControl );
        
    if ( aCacheIndex >= 0 )
        {
        TControlCacheEntry& entry (iControlCache[aCacheIndex]);
        
        for ( TInt i=0; i < entry.iAreaArray.Count(); i++ )
            {
            if ( entry.iAreaArray[i].iIndex == aIndex )
                {
                aAreaIndex = i;
                break;
                }
            } 
        }
    }   
   
// ---------------------------------------------------------------------------
// Here we remove the control from the cache. It is assumed that
// corresponding feedback areas have been removed from registry
// already.
// ---------------------------------------------------------------------------
//
void CTouchFeedbackImpl::RemoveControlFromCache( TInt aControlIndex )
    {
    __ASSERT_ALWAYS(  
        aControlIndex >= 0 && aControlIndex < iControlCache.Count(),
        Panic( ETouchClientPanicArrayAccess ) );
        
    iControlCache[aControlIndex].iAreaArray.Reset();
    iControlCache[aControlIndex].iAreaArray.Close();
    
    iControlCache.Remove( aControlIndex );
    }
         
// ---------------------------------------------------------------------------
// We have a separate function for knowing if control's feedback areas
// should be active or not, because determining this is not so simple, and
// because this part can still change.
// 
// Control's areas are NOT active in case
//
//  #1 Its feedback has been explicitely disabled for audio and vibra
//
// OR
//
//  #2 It is not visible 
//
// OR 
//
//  #3 It is dimmed.
//
// ---------------------------------------------------------------------------
//
TBool CTouchFeedbackImpl::ControlsAreasActive( TInt aControlIndex )
    {
    __ASSERT_ALWAYS(  
        aControlIndex >= 0 && aControlIndex < iControlCache.Count(),
        Panic( ETouchClientPanicArrayAccess ) );

    TBool ret = ETrue;
    
    if ( ( iControlCache[aControlIndex].iVibraDisabled &&     // #1
           iControlCache[aControlIndex].iAudioDisabled ) ||   
          !iControlCache[aControlIndex].iVisible || // #2
          iControlCache[aControlIndex].iDimmed )    // #3
        {
        ret = EFalse;
        }
        
    return ret;
    }   
   
// ---------------------------------------------------------------------------
// This function can be used for disabling or enabling all control's 
// feedback areas in the registry.
//
// #1 Find out the registry index.
// #2 In a loop, disable / enable each feedback area for this control
// #3 Maintain bookkeeping, so that we will know in the end if anything 
//    changed or not (so that we won't make unnecessary updates to server)
// #4 Make a pending update request if registry really changed.
// ---------------------------------------------------------------------------
//
void CTouchFeedbackImpl::DoEnableControlsAreasInRegistry( 
    TInt aControlIndex,
    TBool aEnableVibra,
    TBool aEnableAudio,
    TBool aVisible )
    {
    __ASSERT_ALWAYS(  
        aControlIndex >= 0 && aControlIndex < iControlCache.Count(),
        Panic( ETouchClientPanicArrayAccess ) );

    TBool registryChanged = EFalse;
    
    TControlCacheEntry& entry ( iControlCache[aControlIndex] );
    
    // #1
    TInt registryIndex = FindWindowFromRegistry ( entry.iClientHandle );

    if ( registryIndex != KErrNotFound )
        {
        for ( TInt i=0; i < entry.iAreaArray.Count(); i++ )
            {
            TInt id = entry.iAreaArray[i].iAreaId;
            
            // #2
            TBool changedNow = 
                iRegistryArray[registryIndex]->SetFeedbackEnabled( 
                    id, aEnableVibra, aEnableAudio, aVisible );
                    
            // #3       
            registryChanged = registryChanged || changedNow;
            }
        }
    
    // #4
    if ( registryChanged )
        {
        iClient->RegistryChanged();
        }
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//    
EXPORT_C CFeedbackSpec* CFeedbackSpec::New()
    {
    return new CFeedbackSpec;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
CFeedbackSpec::~CFeedbackSpec()
    {
    iFbArray.Close();
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
EXPORT_C TInt CFeedbackSpec::AddFeedback( TTouchEventType aEventType, 
                                          TTouchLogicalFeedback aFeedback )
    {
    // Vibra feedback is enabled by default for every event type.
    TInt fbType( ETouchFeedbackVibra | ETouchFeedbackAudio );
    
    return AddFeedback( aEventType, 
                        aFeedback, 
                        static_cast<TTouchFeedbackType>( fbType ) );
    }
    
// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
EXPORT_C TInt CFeedbackSpec::AddFeedback( TTouchEventType aEventType, 
                                          TTouchLogicalFeedback aFeedback,
                                          TTouchFeedbackType aFeedbackType )
    {
    TTactileFbItem item;
    if ( !(aEventType >= ETouchEventStylusDown && aEventType <= ETouchEventStylusPressUp ) )
        {
        return KErrArgument;
        }
    item.iEventType = aEventType;        
    
	// range check. update when logical feedback types are changed.
    if ( !(aFeedback >= ETouchFeedbackNone && aFeedback <= ETouchFeedbackSensitive) 
            && !(aFeedback >= ETouchFeedbackBasicButton && aFeedback <= ETouchFeedbackLongPress) )
        {
        return KErrArgument;
        }
    item.iFeedback = aFeedback;        
    item.iFeedbackType = aFeedbackType;
        
    TInt err = iFbArray.Append(item);
    return err;
    }    

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CFeedbackSpec::GetFeedbackSpec( RArray<TTactileFbItem>& aArray )
    {
    // copy values from one array to another
    TInt count = iFbArray.Count();
    aArray.Reset(); // remove records from array if any
    
    for ( TInt i=0 ; i < count ; i++ )
        {
        TTactileFbItem item;
        item.iFeedback  = iFbArray[i].iFeedback;
        item.iEventType = iFbArray[i].iEventType;
        item.iFeedbackType = iFbArray[i].iFeedbackType;
        // if append fail just make its action like before
        TRAP_IGNORE( aArray.AppendL( item ) );
        }
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
CFeedbackSpec::CFeedbackSpec()
    {
    }

// End of File
