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
* Description:  Class for monitoring changes in P&S keys.
* Part of:      Tactile Feedback.
*
*/


#ifndef TACTILEPROPERTYWATCHER_H
#define TACTILEPROPERTYWATCHER_H


#include <e32base.h> 
#include <e32std.h>
#include <e32property.h>

#include "tactilepropertyobserver.h"



/** 
 *  Observer class for accessing to P&S property values, and listening
 *  to changes.
 *
 *  @lib tactilefeedbackresolver.lib
 *  @since S60 v5.0
 */
NONSHARABLE_CLASS( CTactilePropertyWatcher ): public CActive
    {
public:

    /**
     * Intantiation method. 
     * 
     * This will create a property wather active
     * object with given active object priority. 
     *
     * @param aObserver - Reference to observer interface.
     * @param aCategory - P&S category, where the observed key is.
     * @param aKey      - Observed P&S key.
     * @param aPriority - Active object priority, which is given to
     *                    this wather instance.
     * @return New property watcher instance.
     */
    static CTactilePropertyWatcher* NewL( 
        MTactilePropertyObserver& aObserver, 
        const TUid aCategory, 
        const TInt aKey,
        CActive::TPriority aPriority );

    ~CTactilePropertyWatcher();

    
    /**
     * Retrieves an integer value associated with this property.
     *
     * Leaves with Standard Symbian OS error codes in case 
     * retreving of the value fails. 
     *
     * @param aValue - The property value is returned here
     */
    void GetL( TInt& aValue );

    /**
     * Fills aValue with the buffer associated with this property.
     *
     * Leaves with Standard Symbian OS error codes in case 
     * retreving of the value fails. 
     *
     * @param aValue - The property value is returned here
     */
    void GetL( TDes8& aValue );

    /**
     * Retrieves an integer value associated with this property.
     *
     * Does not leave in case reading of the property fails, but
     * instead returns Standard Symbian OS error code.
     *
     * @param aValue the property value is returned here
     * @return standard Symbian OS error code
     */
    TInt Get( TInt& aValue );

    /**
     * Fills aValue with the buffer associated with this property.
     *
     * Does not leave in case reading of the property fails, but
     * instead returns Standard Symbian OS error code.
     *
     * @param aValue the property value is returned here
     * @return standard Symbian OS error code
     */
    TInt Get( TDes8& aValue );
            
    /**
     * Sets a P&S property to an integer value. The property must be
     * already defined.
     *
     * @param aValue - The integer value to set to the property
     */
    void SetL( const TInt aValue );

            
    /**
     * Sets a P&S property to a (large) bytearray value. The property
     * must be already defined.
     *
     * @param aValue - The integer value to set to the property
     */
    void SetL( const TDesC8& aValue );
                
private:
        
    CTactilePropertyWatcher( 
        MTactilePropertyObserver& aObserver, 
        const TUid aCategory, 
        const TInt aKey,
        CActive::TPriority aPriority );

    void ConstructL();

    void Subscribe();


 // from base class CActive

    /**
     * Standard DoCancel of an active object.
     */
    void DoCancel();

    /**
     * Standard RunL of an active object.
     */
    void RunL();

private: // data

    /**
     * Access to observer interface
     * Not own.
     */
    MTactilePropertyObserver& iObserver;

    /**
     * The UID of this property
     */
    TUid iCategory;

    /**
     * The key of this property
     */
    TUint iKey;

    /**
     * Handle to the attached property
     * Own.
     */
    RProperty iProperty;
    };


#endif // TACTILEPROPERTYWATCHER_H

// End of File

