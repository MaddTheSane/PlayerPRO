#include "MAD.h"
#include "RDriver.h"
#include "PPPrivate.h"

#ifdef _OSSSOUND
//TODO: OSS Sound Driver
#warning The OSS Sound driver is currently not implemented

OSErr initOSS( MADDriverRec *inMADDriver)
{
	return MADUnknownErr;
}

OSErr closeOSS( MADDriverRec *inMADDriver)
{
	return MADUnknownErr;
}

void StopChannelOSS(MADDriverRec *inMADDriver)
{
	
}

void PlayChannelOSS(MADDriverRec *inMADDriver)
{
	
}

#endif
