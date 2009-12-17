/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Tactile feedback player plugin interface.
* Part of:      Tactile Feedback.
*
*/

#ifndef C_TACTILEPLAYER_H
#define C_TACTILEPLAYER_H

#include <e32base.h>
#include <ecom/ecom.h>

#include <touchlogicalfeedback.h>

class CRepository;


/**
 * Types of tactile players. This enumeration is used for listing the 
 * implementations of players of certain type.
 * @since S60 5.2
 */
enum TTactilePlayerType
    {
    ETactilePlayerAny,
    ETactilePlayerHaptics,
    ETactilePlayerAudio
    };

/**
 *  Tactile feedback player ECom plugin interface.
 *
 *  There are two basic types of tactile players, haptics and audio 
 *  players. Each player must follow the player type specific settings 
 *  from Central Repository to play feedback in the specified level. If
 *  playing level for haptics or audio is set to zero, players of that type 
 *  must not produce any feedback.
 *
 *  UID for tactile feedback settings is: 
 *  KCRUidTactileFeedback = 0x2000B494;
 *
 *  Players are instantiated based on plugins' implementation UIDs, which
 *  are read from Central Repository by feedback resolver. Player types 
 *  are differentiated by using word "Haptics" or "Audio" in default_data  
 *  field in plugin's implementation info resource file.
 *
 *  Preview functions for playing immediate and starting continuous feedback
 *  are ment to use for example in settings view to get feedback played with 
 *  different level without actually changing the level in settings. It is
 *  feedback resolver's responsibility not to play regular feedbacks when
 *  preview functions are used.
 *
 *  @lib tactilefeedbackresolver.lib
 *  @since S60 5.2
 */
class CTactilePlayer : public CBase
    {
public:
    /**
     * Instantiates an object of this type.
     *
     * @since S60 5.2
     * @param aImplementationUid - Implementation uid of the plugin
     *                             to be created.
     * @param aRepository        - Reference to Central Repository, which is
     *                             used for reading Tactile Feedback 
     *                             settings. CR Keys for Tactile Feedback are 
     *                             defined in tactilefeedbackdomaincrkeys.h
     *
     * @return Instance of feedback player plugin.
     */
    static CTactilePlayer* NewL( TUid aImplementationUid, 
                                 CRepository& aRepository );

    /**
     * Destructor.
     */
    IMPORT_C virtual ~CTactilePlayer();

    /**
     * Returns a list of available implementations for this 
     * interface.
     *
     * @since S60 5.2
     * @param aType          - Type of players to be listed.
     * @param aImplInfoArray - Implementation information array of all
     *                         available implementations.
     */ 
    static void ListImplementationsL( TTactilePlayerType aType,
                                      RImplInfoPtrArray& aImplInfoArray );

public:
    /**
     * Produces immediate physical feedback according to given
     * logical feedback type. 
     * 
     * Haptics and audio players have onw level value, which is
     * specified in Tactile Feedback settings in Central Repository.
     * That value defines the strength of feedback produced by this function.
     * When settings are changed, plugin implementation must change the 
     * playing level according to that value. If feedback level for this 
     * type of player in settings is set to zero, this function must not 
     * produce any feedback.
     *
     * @since S60 5.2
     * @param aFeedback - The logical feedback type, which is used to 
     *                    select the physical response to feedback method.
     * @return TInt KErrNone if successful, otherwise one of the other
     *              system-wide error codes.     
     */
    virtual TInt PlayFeedback( TTouchLogicalFeedback aFeedback ) = 0;

    /**
     * Starts continuous feedback according to given logical 
     * continuous feedback type. Continuous feedback is started with given
     * intensity, which is a percentage value of maximum strength on current
     * level. Ongoing continuous feedback can be modified using 
     * ModifyFeedback() to change intensity of feedback.
     *
     * Continuous feedback is stopped using StopFeedback().
     *
     * @since S60 5.2
     * @param aFeedback  - The logical feedback type of continuous feedback, 
     *                     which is used to select the physical response to
     *                     continuous feedback method.     
     * @param aIntensity - The percentage value of continuous feedback's 
     *                     maximum strength.
     * @return TInt KErrNone if successful, otherwise one of the other
     *              system-wide error codes.   
     * @return TInt KErrNotSupported if player doesn't support continuous 
     *              feedback.            
     */
    IMPORT_C virtual TInt StartFeedback( TTouchContinuousFeedback aFeedback,
                                         TInt aIntensity );

    /**
     * Modifies the intensity of continuous feedback. 
     *
     * @since S60 5.2
     * @param aIntensity - The percentage value of continuous feedback's 
     *                     intensity.
     * @return TInt KErrNone if successful, otherwise one of the other
     *              system-wide error codes.          
     * @return TInt KErrNotSupported if player doesn't support continuous 
     *              feedback.
     */
    IMPORT_C virtual TInt ModifyFeedback( TInt aIntensity );
    
    /**
     * Stops continuous feedback.
     *
     * @since S60 5.2
     */    
    IMPORT_C virtual void StopFeedback();
    
    /**
     * Plugin's maximum feedback level, which defines the strength
     * of played feedback. The amount of feedback levels is dependent on
     * a plugin.
     *
     * Level values are used for playing preview of feedback. 
     * 
     * @since S60 5.2
     * @return TInt KErrNone if successful, otherwise one of the other
     *              system-wide error codes.          
     */    
    IMPORT_C virtual TInt MaxLevel();
        
    /**
     * Produces preview of feedback according to given locigal feedback 
     * type and given level value of feedback strength. 
     *
     * @since S60 5.2
     * @param aLevel    - A value, which defines the strength of preview
     *                    feedback. Preview level can be any positive 
     *                    value up to value of MaxLevel().
     * @param aFeedback - The logical feedback type, which is used to 
     *                    select the physical response to feedback method.
     * @return TInt KErrNone if successful, otherwise one of the other
     *              system-wide error codes.          
     * @return TInt KErrNotSupported if player doesn't support feedback
     *              preview.
     */
    IMPORT_C virtual TInt PlayPreviewFeedback( TInt aLevel,
                                      TTouchLogicalFeedback aFeedback );

    /**
     * Starts preview of continuous feedback according to given logical 
     * continuous feedback type and given level value of feedback strength.
     * Preview of continuous feedback is started with given intensity,
     * which is a percentage value of maximum strength in given level. 
     * Ongoing continuous feedback can be modified using ModifyFeedback() 
     * to change intensity of feedback.
     *
     * Preview of continuous feedback is stopped using StopFeedback().
     *
     * @since S60 5.2
     * @param aLevel     - A value, which defines the strength of preview
     *                     feedback. Preview level can be any positive 
     *                     value between zero and MaxLevel().
     * @param aFeedback  - The logical feedback type of continuous feedback, 
     *                     which is used to select the physical response to
     *                     continuous feedback method.
     * @param aIntensity - The percentage value of continuous feedback's 
     *                     maximum strength.
     * @return TInt KErrNone if successful, otherwise one of the other
     *              system-wide error codes.   
     * @return TInt KErrNotSupported if player doesn't support preview
     *              for continuous feedback.
     */
    IMPORT_C virtual TInt StartPreviewFeedback( TInt aLevel,
                                      TTouchContinuousFeedback aFeedback,
                                      TInt aIntensity );

    /**
     * Stops preview of continuous feedback. Player must set the original 
     * level value back in this function.
     *
     * @since S60 5.2
     */                                          
    IMPORT_C virtual void StopPreviewFeedback();

private:
    
    // Unique instance identifier key.  
    TUid iDtor_ID_Key;
        
    };
    
#endif // C_TACTILEPLAYER_H
