#include "RDriver.h"
#include "Be-DriverClass.h"

#ifndef __MAINMADH__
#define __MAINMADH__

class MyApp : public BApplication
{
	public:
							MyApp(ulong signature);
							~MyApp();
							
	virtual void RefsReceived(BMessage *msg);
	
			MADDriverClass	*myMADDriverClass;
			
};

#endif