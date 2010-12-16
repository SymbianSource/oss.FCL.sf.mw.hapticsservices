#include <e32base.h>
#include <e32cmn_private.h>
#include "tactilearearegistrythreadwatcher.h"
#include "tactileinternaldatatypes.h"
#include "tactilearearegistry.h"


CTactileAreaRegistryThreadWatcher::CTactileAreaRegistryThreadWatcher(
    CTactileAreaRegistry* aTactileAreaRegistry,
    RThread& aThread,
    TInt aWindowGroupId ) :
    CActive( EPriorityStandard ), // Standard priority
    iTactileAreaRegistry( aTactileAreaRegistry ),
    iThread( aThread ),
    iWindowGroupId( aWindowGroupId )
    {
    }

CTactileAreaRegistryThreadWatcher* CTactileAreaRegistryThreadWatcher::NewLC(
                                    CTactileAreaRegistry* aTactileAreaRegistry,
                                    RThread& aThread,
                                    TInt aWindowGroupId )
    {
    CTactileAreaRegistryThreadWatcher* self = 
            new (ELeave) CTactileAreaRegistryThreadWatcher( 
                    aTactileAreaRegistry,aThread,aWindowGroupId );
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

CTactileAreaRegistryThreadWatcher* CTactileAreaRegistryThreadWatcher::NewL(
                                    CTactileAreaRegistry* aTactileAreaRegistry,
                                    RThread& aThread,
                                    TInt aWindowGroupId )
    {
    CTactileAreaRegistryThreadWatcher* self = 
            CTactileAreaRegistryThreadWatcher::NewLC( 
                    aTactileAreaRegistry,aThread,aWindowGroupId );
    CleanupStack::Pop(); // self;
    return self;
    }

void CTactileAreaRegistryThreadWatcher::ConstructL()
    {
    CActiveScheduler::Add( this ); // Add to scheduler
    iThread.Logon( iStatus ); //do logon for client thread to notice if client dies unexpectedly
    SetActive(); // Tell scheduler a request is active
    }

CTactileAreaRegistryThreadWatcher::~CTactileAreaRegistryThreadWatcher()
    {
    Cancel(); // Cancel any request, if outstanding
    iThread.Close();
    }

void CTactileAreaRegistryThreadWatcher::DoCancel()
    {
    iThread.LogonCancel( iStatus );
    }

void CTactileAreaRegistryThreadWatcher::RunL()
    {
    //Thread has died
    TTactileFeedbackDisconnectData data;
    data.iWindowGroupId = iWindowGroupId; 
    iTactileAreaRegistry->HandleDisconnect( data );
    }

TInt CTactileAreaRegistryThreadWatcher::RunError(TInt /*aError*/)
    {
    //RunL cannot leave.
    return KErrNone;
    }
