#include "MAD.h"
#include "RDriver.h"
#include "PPPrivate.h"

#ifdef __LINUX__
//TODO: Linux Sound Driver
#warning Linux sound driver currently not implemented

OSErr initALSA( MADDriverRec *inMADDriver)
{
	return MADUnknownErr;
}

OSErr closeALSA( MADDriverRec *inMADDriver)
{
	return MADUnknownErr;
}

void StopChannelALSA(MADDriverRec *inMADDriver)
{
	
}

void PlayChannelALSA(MADDriverRec *inMADDriver)
{
	
}

#endif
