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

#include <f32file.h>
#include <hwrmhaptics.h>
#include <centralrepository.h>
#include <ecom/implementationproxy.h>

#include "tacticonhapticsplayer.h"
#include "tacticonprivatecrkeys.h"
#include "tacticontrace.h"
#include "OstTraceDefinitions.h" 
#ifdef OST_TRACE_COMPILER_IN_USE 
#include "tacticonhapticsplayerTraces.h"
#endif


// Tacticons

_LIT8( KPositiveTacticon,     "PositiveTacticon" );
_LIT8( KNegativeTacticon,     "NegativeTacticon" );
_LIT8( KNeutralTacticon,      "NeutralTacticon" );
/*
    _LIT8( KAlertTacticon,        "AlertTacticon" );
    _LIT8( KInformationTacticon,  "InformationTacticon" );
    _LIT8( KNewMessageTacticon,   "NewMessageTacticon" );
    _LIT8( KNewEmailTacticon,     "NewEmailTacticon" );
    _LIT8( KCalendarTacticon,     "CalendarTacticon" );
    _LIT8( KClockTacticon,        "ClockTacticon" );
    _LIT8( KRingingToneTacticon,  "RingingToneTacticon" );
*/
//  the tacticon names following are temporary mapped to NeutralTacticon,
// they will be updated when the IVT files are ready.
    _LIT8( KAlertTacticon,        "NeutralTacticon" );
    _LIT8( KInformationTacticon,  "NeutralTacticon" );
    _LIT8( KNewMessageTacticon,   "NeutralTacticon" );
    _LIT8( KNewEmailTacticon,     "NeutralTacticon" );
    _LIT8( KCalendarTacticon,     "NeutralTacticon" );
    _LIT8( KClockTacticon,        "NeutralTacticon" );
    _LIT8( KRingingToneTacticon,  "NeutralTacticon" );
    
// max length of tacticon names defined above
const TInt KTacticonNameMaxLen = 128;

// ---------------------------------------------------------------------------
// Constructor.
// ---------------------------------------------------------------------------
//
CTacticonHapticsPlayer::CTacticonHapticsPlayer( CRepository& aRepository ) :
    iRepository(aRepository)
    {
    
    }

// ---------------------------------------------------------------------------
// 2nd phase constructor.
// ---------------------------------------------------------------------------
//
void CTacticonHapticsPlayer::ConstructL()
    {
    TRACE( "CTacticonHapticsPlayer::ConstructL - Begin" );
    iHaptics = CHWRMHaptics::NewL( NULL, NULL );

    TUint32 suppMask( 0 );
    User::LeaveIfError( iHaptics->SupportedActuators( suppMask ) );

    CRepository* playerRepository = CRepository::NewL( KCRUidTacticon ); 
    CleanupStack::PushL( playerRepository );
    TInt actuatorType( 0 );
    playerRepository->Get( KActuatorType, actuatorType );
    CleanupStack::PopAndDestroy( playerRepository );
    TRACE2( "CTacticonHapticsPlayer::ConstructL - ActuatorType: %d", actuatorType );
    if ( actuatorType & suppMask )
        {
        // supported actuator, use this
        iHaptics->OpenActuatorL( ( THWRMLogicalActuators )actuatorType );    
        }  
    else 
        {
        TRACE3( "CTacticonHapticsPlayer::ConstructL - ActuatorType is not supported: %d/%d", 
                actuatorType, 
                suppMask );
        // suggested actuator is not supported, use default one
        iHaptics->OpenActuatorL( EHWRMLogicalActuatorAny );    
        }  
        
    User::LeaveIfError( iHaptics->SetDeviceProperty( 
                                            CHWRMHaptics::EHWRMHapticsLicensekey,
                                            KNullDesC8() ) );
    TInt strength( 0 );
    User::LeaveIfError( iRepository.Get( KTacticonStrength, strength ) );
    iStrength = strength * 100; // strength is in percents
    
    iHaptics->SetDeviceProperty( CHWRMHaptics::EHWRMHapticsStrength, 
                                 iStrength );
    TRACE2( "CTacticonHapticsPlayer::ConstructL - Set strength: %d", iStrength );

    TFileName ivtFile;
    User::LeaveIfError( iRepository.Get( KTacticonIVTFile, ivtFile ) );
    TRACE2( "CTacticonHapticsPlayer::ConstructL - Loading IVT file: %S", &ivtFile );
    
    HBufC8* ivtBuf = IVTBufAllocL( ivtFile );
    CleanupStack::PushL( ivtBuf );
    User::LeaveIfError( iHaptics->LoadEffectData( *ivtBuf, iIVTHandle ) );
    CleanupStack::PopAndDestroy( ivtBuf );
    
    iCenRepNotifier = CCenRepNotifyHandler::NewL( *this, iRepository );
    iCenRepNotifier->StartListeningL();

    TRACE( "CTacticonHapticsPlayer::ConstructL - End" );
    }

// ---------------------------------------------------------------------------
// 2-phased constructor.
// ---------------------------------------------------------------------------
//
CTacticonHapticsPlayer* CTacticonHapticsPlayer::NewL( CRepository& aRepository )
    {
    CTacticonHapticsPlayer* self = 
                        new ( ELeave ) CTacticonHapticsPlayer( aRepository );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// Destructor.
// ---------------------------------------------------------------------------
//
CTacticonHapticsPlayer::~CTacticonHapticsPlayer()
    {
    delete iCenRepNotifier;
    delete iHaptics;
    }

// ---------------------------------------------------------------------------
// Load IVT file.
// ---------------------------------------------------------------------------
//
HBufC8* CTacticonHapticsPlayer::IVTBufAllocL( const TDesC& aFileName )
    {
    RFs fs;
    User::LeaveIfError( fs.Connect() );  
    CleanupClosePushL( fs );
    
    RFile file;
    User::LeaveIfError( file.Open( fs, aFileName, EFileRead ) );
    CleanupClosePushL( file );
    
    TInt fileSize( 0 );
    file.Size( fileSize );
    
    HBufC8* ivtFileBuf = HBufC8::NewLC( fileSize );
    TPtr8 dataBufPtr = ivtFileBuf->Des();
    
    User::LeaveIfError( file.Read( dataBufPtr ) );

    CleanupStack::Pop( ivtFileBuf );    
    CleanupStack::PopAndDestroy( &file );
    CleanupStack::PopAndDestroy( &fs );
    return ivtFileBuf;
    }

// ---------------------------------------------------------------------------
// Check if there's a change in tacticon settings and set new values if needed.
// ---------------------------------------------------------------------------
//  
void CTacticonHapticsPlayer::DoHandleNotifyGenericL( TUint32 aId )
    {
    switch ( aId )
        {
        case KTacticonStrength:
            {
            // Strength value in settings is multiplied by 100 to scale value 
            // suitable for haptics.
            TInt strength(0);
            iRepository.Get( KTacticonStrength, strength );
            
            iStrength = strength * 100;
            iHaptics->SetDeviceProperty( CHWRMHaptics::EHWRMHapticsStrength, 
                                         iStrength );        
            }
            break;
        case KTacticonIVTFile:
            {
            TFileName ivtFile;
            iRepository.Get( KTacticonIVTFile, ivtFile );
            ChangeIVTFileL( ivtFile );
            }
            break;            
        default:
            break;
        } 
    }

// ---------------------------------------------------------------------------
// From MCenRepNotifyHandlerCallback.
// ---------------------------------------------------------------------------
//    
void CTacticonHapticsPlayer::HandleNotifyGeneric( TUint32 aId )
    {
    TRAP_IGNORE( DoHandleNotifyGenericL( aId ) );
    }

// ---------------------------------------------------------------------------
// Load new effect file.
// ---------------------------------------------------------------------------
//
void CTacticonHapticsPlayer::ChangeIVTFileL( const TDesC& aFileName )
    {
    iHaptics->DeleteEffectData( iIVTHandle );
    
    HBufC8* ivtBuf = IVTBufAllocL( aFileName );
   
    iHaptics->LoadEffectData( *ivtBuf, iIVTHandle );    

    delete ivtBuf;
    } 

// ---------------------------------------------------------------------------
// Play tacticon.
// ---------------------------------------------------------------------------
//
void CTacticonHapticsPlayer::PlayTacticon( TTacticonType aTacticon )
    {
    TRACE2( "CTacticonHapticsPlayer::PlayTacticon(%d) - Begin", aTacticon );

    TBuf8<KTacticonNameMaxLen> name;
    switch( aTacticon )
        {
        case EPositiveTacticon:
            name = KPositiveTacticon;
            break;
        case ENegativeTacticon:
            name = KNegativeTacticon;
            break;
        case ENeutralTacticon:
            name = KNeutralTacticon;
            break;
        case EAlertTacticon:
            name = KAlertTacticon;
            break;
        case EInformationTacticon:
            name = KInformationTacticon;
            break;
        case ENewMessageTacticon:
            name = KNewMessageTacticon;
            break;
        case ENewEmailTacticon:
            name = KNewEmailTacticon;
            break;
        case ECalendarTacticon:
            name =KCalendarTacticon;
            break;
        case EClockTacticon:
            name = KClockTacticon;
            break;
        case ERingingToneTacticon:
            name = KRingingToneTacticon;
            break;
        default:
            return;
        }
    TInt effectIndex(0);
    iHaptics->GetEffectIndexFromName( iIVTHandle, name, effectIndex );

    OstTrace1( TACTICON_PERFORMANCE, TACTICON_HAPTICS_PLAYER_PLAY_TACTICON_1,
               "e_TACTICON_HAPTICS_PLAYER_PLAY_TACTICON 1 0x%x", aTacticon );

    TInt effectHandle(0);
    iHaptics->PlayEffect( iIVTHandle, effectIndex, effectHandle );

    OstTrace1( TACTICON_PERFORMANCE, TACTICON_HAPTICS_PLAYER_PLAY_TACTICON_0,
               "e_TACTICON_HAPTICS_PLAYER_PLAY_TACTICON 0 0x%x", aTacticon );

    TRACE( "CTacticonHapticsPlayer::PlayTacticon(%d) - End" );
    }     
    
// ---------------------------------------------------------------------------
// Stop playing tacticon.
// ---------------------------------------------------------------------------
//
void CTacticonHapticsPlayer::StopTacticon()
    {
    OstTrace0( TACTICON_PERFORMANCE, TACTICON_HAPTICS_PLAYER_STOP_TACTICON_1,
               "e_TACTICON_HAPTICS_PLAYER_STOP_TACTICON 1" );
               
    iHaptics->StopAllPlayingEffects();

    OstTrace0( TACTICON_PERFORMANCE, TACTICON_HAPTICS_PLAYER_STOP_TACTICON_0,
               "e_TACTICON_HAPTICS_PLAYER_STOP_TACTICON 0" );
    } 

//---------------------------------------------------------------------------
// ImplementationTable[]
//
//---------------------------------------------------------------------------
//
const TImplementationProxy ImplementationTable[] = 
    {
    IMPLEMENTATION_PROXY_ENTRY( 0x2001FE51, CTacticonHapticsPlayer::NewL )
    };

//---------------------------------------------------------------------------
// TImplementationProxy* ImplementationGroupProxy()
//
//---------------------------------------------------------------------------
//
EXPORT_C const TImplementationProxy* ImplementationGroupProxy( TInt& aTableCount )
    {
    aTableCount = sizeof( ImplementationTable ) / sizeof( TImplementationProxy );
    return ImplementationTable;
    }
   
// End of file
