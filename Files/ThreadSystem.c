//#include <threads.h>
#include <stdio.h>
#include <Gestalt.h>

#if defined(powerc) || defined (__powerc)
#include <CodeFragments.h>
#endif

pascal void *MyThread_A( void *refCon);
/*
void	InitThreadSystem( void)
{
	OSErr		errWhatErr;
	int			i;
	ThreadID	threadID_A, threadID_B, threadID_C;
	long		threadGestaltInfo;


	if( Gestalt( gestaltThreadMgrAttr, &threadGestaltInfo) != noErr ||
#if defined(powerc) || defined (__powerc)
		threadGestaltInfo & (1<<gestaltThreadsLibraryPresent) == 0 ||
		(Ptr) NewThread == kUnresolvedSymbolAddress ||
#endif
		threadGestaltInfo & (1<<gestaltThreadMgrPresent) == 0)
	{
		printf( "The Threads Mgr isn't present... Can't run the test.\n");
		return;
	}
	
	errWhatErr = NewThread( kPreemptiveThread, MyThread_A, (void *)0, 0,
							kCreateIfNeeded,
							(void**)nil, &threadID_A);
}

pascal void *MyThread_A( void *)
{
	int			i;
	
	DoGlobalNull();
	
	return nil;
}*/
