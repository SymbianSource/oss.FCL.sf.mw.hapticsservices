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
* Description: Tacticon player plugin interface.
* Part of    : Tacticon Server
*/

#ifndef C_TACTICONPLAYER_H
#define C_TACTICONPLAYER_H

#include <e32base.h>
#include <ecom/ecom.h>

#include <tacticon.hrh>

class CRepository;

/**
 *  Tacticon player ECom plugin interface.
 *
 *  @since S60 5.2
 */
class CTacticonPlayer : public CBase
    {
public:
    /**
     * Instantiates an object of this type.
     *
     * @since S60 5.2
     * @param aImplementationUid - Implementation uid of the plugin
     *                             to be created.
     * @param aRepository        - Reference to Central Repository, which is
     *                             used for reading Tacticon settings. 
     *                             CR Keys for Tacticons are defined in 
     *                             tacticondomaincrkeys.h
     *
     * @return Instance of tacticon player plugin.
     */
    IMPORT_C static CTacticonPlayer* NewL( TUid aImplementationUid, 
                                           CRepository& aRepository );

    /**
     * Destructor.
     */
    IMPORT_C virtual ~CTacticonPlayer();

    /**
     * Returns a list of available implementations for this 
     * interface.
     *
     * @since S60 5.2
     * @param aImplInfoArray - Implementation information array of all
     *                         available implementations.
     */ 
    IMPORT_C static void ListImplementationsL( RImplInfoPtrArray& aImplInfoArray );

public:
    /**
    * Plays tacticon according to aTacticonType
    * @since S60 5.2 
    * @param aTacticonType, Tacticon type to be played
    */
    virtual void PlayTacticon( TTacticonType aTacticonType ) = 0; 
    
    /**
    * Stops playing tacticon
    * @since S60 5.2 
    */    
    virtual void StopTacticon() = 0;

private:
    
    // Unique instance identifier key.  
    TUid iDtor_ID_Key;
        
    };
    
#endif // C_TACTICONPLAYER_H
