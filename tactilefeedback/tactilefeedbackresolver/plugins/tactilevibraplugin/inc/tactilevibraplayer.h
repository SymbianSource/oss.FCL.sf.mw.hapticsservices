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
* Description:  Class for producing vibra feedback.
* Part of:      Tactile Feedback.
*
*/

#ifndef C_TACTILEVIBRAPLAYER_H
#define C_TACTILEVIBRAPLAYER_H


#include <e32base.h>
#include <cenrepnotifyhandler.h>
#include <touchlogicalfeedback.h>

#include "tactileplayer.h"

class CHWRMVibra;

/**
 *  Class for procuding vibra feedback based on logical
 *  feedback type.
 *
 *  @lib tactilevibraplugin.lib
 *  @since S60 5.2
  */

NONSHARABLE_CLASS( CTactileVibraPlayer ): public CTactilePlayer,
                                          public MCenRepNotifyHandlerCallback
    {
public:
    /**  
     * Parameters of one vibrator effect 
     */
    struct TTactileVibraParams
        {
        TInt iTime;
        TInt iIntensity;
        };

    /**
     * 2-phased constructor.
     * @param aRepository - Reference to Central Repository.
     * @return Instance of CTactileVibraPlayer.
     */
    static CTactileVibraPlayer* NewL( CRepository& aRepository );

    /**
     * Destructor.
     */
    virtual ~CTactileVibraPlayer();

 // from base class CTactilePlayer
 
    /**
     * @see CTactilePlayer.
     */ 
    TInt PlayFeedback( TTouchLogicalFeedback aFeedback );
    
    /**
     * @see CTactilePlayer.
     */     
    TInt StartFeedback( TTouchContinuousFeedback aFeedback,
                        TInt aIntensity );

    /**
     * @see CTactilePlayer.
     */ 
    TInt ModifyFeedback( TInt aIntensity );

    /**
     * @see CTactilePlayer.
     */ 
    void StopFeedback();    
    
    /**
     * @see CTactilePlayer.
     */    
    TInt PlayPreviewFeedback( TInt aLevel, TTouchLogicalFeedback aFeedback );
        

private: // From MCenRepNotifyHandlerCallback.
    /**
     * @see MCenRepNotifyHandlerCallback.
     */ 
    void HandleNotifyInt( TUint32 aId, TInt aNewValue );

private:
    /**
     * Plays vibra effect by logical feedback type.
     */
    void DoPlayFeedbackL( TInt aTime, TInt aIntensity );

    /**
     * Starts continuous vibra effect by logical feedback type.
     */
    void DoStartFeedbackL( TTouchContinuousFeedback aType,
                           TInt aIntensity );
    /**
     * Read settings values.
     */ 
    void ReadSettings();

private:
    /**
     * Constructor.
     */
    CTactileVibraPlayer( CRepository& aRepository );

    /**
     * 2nd phase constructor.
     */
    void ConstructL();
    

private: // data

    /**
     * Array for storing vibrator parameters for each
     * different physical feedback type.
     */
    RArray<TTactileVibraParams> iVibraParams;

    /**
     * Access to vibra functionality
     * Own.  
     */
    CHWRMVibra* iVibra;
    
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
    
    /**
     * Current vibra level.
     */
    TInt iVibraLevel;
    
    /**
     * Original vibra level is saved here when preview is played.
     */    
    TInt iOriginalLevel;
    
    };


#endif // C_TACTILEVIBRAPLAYER_H
