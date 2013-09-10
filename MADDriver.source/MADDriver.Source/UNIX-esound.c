#include "MAD.h"
#include "RDriver.h"
#include "PPPrivate.h"

#ifdef _ESOUND
//TODO: Esound Sound Driver
#warning The Esound Sound driver is currently not implemented

OSErr initESD( MADDriverRec *inMADDriver)
{
	return MADUnknownErr;
}

OSErr closeESD( MADDriverRec *inMADDriver)
{
	return MADUnknownErr;
}

void StopChannelESD(MADDriverRec *inMADDriver)
{
	
}

void PlayChannelESD(MADDriverRec *inMADDriver)
{
	
}

#endif
