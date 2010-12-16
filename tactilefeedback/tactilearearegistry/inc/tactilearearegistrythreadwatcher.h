/*
 ============================================================================
 Name		: tactilearearegistrythreadwatcher.h
 Author	  : 
 Version	 : 
 Copyright   : 
 Description : CTactileAreaRegistryThreadWatcher declaration
 ============================================================================
 */

#ifndef TACTILEAREAREGISTRYTHREADWATCHER_H
#define TACTILEAREAREGISTRYTHREADWATCHER_H

#include <e32base.h>    // For CActive, link against: euser.lib

class CTactileAreaRegistry;
NONSHARABLE_CLASS( CTactileAreaRegistryThreadWatcher ): public CActive
    {
public:
    // Cancel and destroy
    ~CTactileAreaRegistryThreadWatcher();

    // Two-phased constructor.
    static CTactileAreaRegistryThreadWatcher* NewL( 
                            CTactileAreaRegistry* aTactileAreaRegistry,
                            RThread& aThread,
                            TInt aWindowGroupId );

    // Two-phased constructor.
    static CTactileAreaRegistryThreadWatcher* NewLC( 
                            CTactileAreaRegistry* aTactileAreaRegistry,
                            RThread& aThread,
                            TInt aWindowGroupId );

private:
    // C++ constructor
    CTactileAreaRegistryThreadWatcher( 
                                CTactileAreaRegistry* aTactileAreaRegistry,
                                RThread& aThread,
                                TInt aWindowGroupId );

    // Second-phase constructor
    void ConstructL();

public: 
    inline TInt WGID() { return iWindowGroupId; };
    
private:
    // From CActive
    // Handle completion
    void RunL();

    // How to cancel me
    void DoCancel();

    // Override to handle leaves from RunL(). Default implementation causes
    // the active scheduler to panic.
    TInt RunError(TInt aError);

private:
    CTactileAreaRegistry* iTactileAreaRegistry;
    RThread iThread;
    TInt iWindowGroupId;
    };

#endif // TACTILEAREAREGISTRYTHREADWATCHER_H
