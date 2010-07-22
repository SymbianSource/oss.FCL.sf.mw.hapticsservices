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


#ifndef C_TACTILEHAPTICSPLAYER_H
#define C_TACTILEHAPTICSPLAYER_H


#include <e32base.h>
#include <cenrepnotifyhandler.h>
#include <touchlogicalfeedback.h>
#include <MProfileChangeObserver.h>

#include "tactileplayer.h"

class CHWRMHaptics;

class MProfileEngine;
class CProfileChangeNotifyHandler;
/**
 *  Class for procuding haptics feedback based on logical
 *  feedback type.
 *
 *  @lib tactilehapticsplugin.lib
 *  @since S60 5.2
  */

NONSHARABLE_CLASS( CTactileHapticsPlayer ): public CTactilePlayer,
                                            public MCenRepNotifyHandlerCallback,
                                            public MProfileChangeObserver
    {
public:
    /**
     * 2-phased constructor.
     * @param aRepository - Reference to Central Repository.
     * @return Instance of CTactileHapticsPlayers.
     */
    static CTactileHapticsPlayer* NewL( CRepository& aRepository );

    /**
     * Destructor.
     */
    virtual ~CTactileHapticsPlayer();

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

    /**
     * @see CTactilePlayer.
     */
    TInt StartPreviewFeedback( TInt aLevel,
                               TTouchContinuousFeedback aFeedback,
                               TInt aIntensity );

    /**
     * @see CTactilePlayer.
     */
    void StopPreviewFeedback();                               

private: // From MCenRepNotifyHandlerCallback.
    /**
     * @see MCenRepNotifyHandlerCallback.
     */
    void HandleNotifyGeneric( TUint32 aId );

private: // From MProfileChangeObserver
    /**
     * @see MProfileChangeObserver.
     */
    void HandleActiveProfileEventL( TProfileEvent aProfileEvent, 
                                    TInt aProfileId );

private:
    /**
     * Plays haptics effect by logical feedback type.
     * 
     */
    void DoPlayFeedbackL( TTouchLogicalFeedback aFeedback );
    
    /**
     * Get IVT file Id via vibra level.
     */
    TUint32 IVTFileId();
    
    /**
     * Reads IVT data from file to buffer. 
     *
     */    
    HBufC8* IVTBufAllocL( const TDesC& aFileName );
    
    /**
     * Get new settings value from Central Repository and 
     * change haptics property.
     */
     void DoHandleNotifyGenericL( TUint32 aId );

    /**
     * Profiles engine -related initializations.
     * (User changeable settings are stored in profiles)
     */
    void InitializeProfilesEngineL();
    
private:
    /**
     * Constructor.
     */    
    CTactileHapticsPlayer( CRepository& aRepository );

    /**
     * 2nd phase constructor.
     */
    void ConstructL();    

private: // data

    /**
     * Access to haptics functionality
     * Own.  
     */
    CHWRMHaptics* iHaptics;

    /**
     * Current effect handle.
     */
    TInt    iEffectHandle;
    
    /**
     * Current effect index.
     */    
    TInt    iEffectIndex;

    /**
     * Handle to effect data file.
     */
    TInt iIVTHandle;

    /**
     * Multiplier, which is used for scaling the intenstiy of
     * continuous feedback.
     */
    TInt iMultiplier;
    
    /**
     * Haptics effect strength.
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
    
    
    /**
     * Access to user changeable settings (stored into profiles)
     * Own.
     */
     MProfileEngine* iProfileEng;

    /**
     * For listening to changes in profiles.
     * Own.  
     */
    CProfileChangeNotifyHandler* iProfileHandler;
    
    /**
     * Current vibra level.
     */
    TInt iVibraLevel;
    };


#endif // C_TACTILEHAPTICSPLAYER_H
