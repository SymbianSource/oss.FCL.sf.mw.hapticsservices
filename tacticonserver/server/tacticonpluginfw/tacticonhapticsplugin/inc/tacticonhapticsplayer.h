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
* Description: Class for producing haptics tacticon.
* Part of:     Tacticon Server
*/

#ifndef C_TACTICONHAPTICSPLAYER_H
#define C_TACTICONHAPTICSPLAYER_H

#include <cenrepnotifyhandler.h>
#include <tacticon.hrh>

#include "tacticonprivatecrkeys.h"
#include "tacticonplayer.h"

class CHWRMHaptics;

/**
 *  Ecom plugin for playing haptics tacticons.
 *
 *  @lib tactilehapticsplugin.lib
 *  @since S60 5.2
  */

NONSHARABLE_CLASS( CTacticonHapticsPlayer ): public CTacticonPlayer,
                                             public MCenRepNotifyHandlerCallback
    {
public:
    /**
     * 2-phased constructor.
     * @return An instance of haptics tacticon player.
     */
    static CTacticonHapticsPlayer* NewL( CRepository& aRepository );

    /**
     * Destructor.
     */
    virtual ~CTacticonHapticsPlayer();

 // from base class CTacticonPlayer

    /**
    * Plays tacticon according to aTacticonType
    * @since S60 5.2 
    * @param aTacticonType, Tacticon type to be played
    */
    void PlayTacticon( TTacticonType aTacticonType ); 
    
    /**
    * Stops playing tacticon
    * @since S60 5.2 
    */    
    void StopTacticon(); 
        
private: // From MCenRepNotifyHandlerCallback.

    void HandleNotifyGeneric( TUint32 aId );

private:
    /**
     * Reads IVT data from file to buffer. 
     */    
    HBufC8* IVTBufAllocL( const TDesC& aFileName );
    
    /**
     * Change effect file in haptics instance.
     */        
    void ChangeIVTFileL( const TDesC& aFileName );
    
    /**
     * Handle Central Repository settings changes.
     */    
    void DoHandleNotifyGenericL( TUint32 aId );
private:

    /**
     * Constructor.
     */
    CTacticonHapticsPlayer( CRepository& aRepository );

    /**
     * 2nd phase constructor.
     */
    void ConstructL();
    

private: // data

    /**
     * Access to vibra functionality
     * Own.  
     */
    CHWRMHaptics* iHaptics;

    /**
     * Handle of effect file, which is loaded to haptics instance. 
     */
    TInt iIVTHandle;
    
    /**
     * Haptics strength.
     */
    TInt iStrength;
    
     /**
     * Access to settings.
     * Not own.
     */    
    CRepository& iRepository;
    
    /**
     * Utility for listening to changes in configurable parameters.
     * Own.  
     */
    CCenRepNotifyHandler* iCenRepNotifier;    
    
    };


#endif // C_TACTICONHAPTICSPLAYER_H
