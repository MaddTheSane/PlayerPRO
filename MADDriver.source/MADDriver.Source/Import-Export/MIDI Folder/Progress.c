#include <Carbon/Carbon.h>

typedef struct CrsrRec
{
	short	total;
	short	current;
	short	no[ 30];
}CrsrRec;

static	CrsrRec	MyCrsrRec;
static	long		previousTime;
#ifdef QD_HEADERS_ARE_PRIVATE
//Workaround so it can build on 10.7 and later SDKs
typedef struct Cursor {
	short data[16];
	short mask[16];
	Point hotSpot;
} Cursor, *CursPtr, **CursHandle;
extern CursHandle GetCursor(short);
extern void SetCursor(const Cursor *);
#endif
static	Cursor	*myCursor = NULL;

void InitRollCrsr(void)
{
	Handle		tempHandle;
	short		i;
	CursHandle	tempH;
	
	myCursor = (Cursor*) NewPtr( 30 * sizeof( Cursor));
	
	previousTime = 0;

	tempHandle = GetResource( 'acur', 200);
	DetachResource( tempHandle);
	HLock( tempHandle);
	BlockMoveData( *tempHandle, &MyCrsrRec, GetHandleSize( tempHandle));
	HUnlock( tempHandle);
	DisposeHandle( tempHandle);
	
	for( i = 0; i < MyCrsrRec.total; i++)
	{
		tempH = GetCursor( MyCrsrRec.no[ i*2]);
		DetachResource( (Handle) tempH);
		HLock( (Handle) tempH);
			myCursor[ i] = **tempH;
		HUnlock( (Handle) tempH);
		DisposeHandle( (Handle) tempH);
	}
}

void RollCursor(void)
{
	if( previousTime + 10 >= TickCount()) return;

	previousTime = TickCount();
	MyCrsrRec.current ++;
	if( MyCrsrRec.current >= MyCrsrRec.total) MyCrsrRec.current = 0;
	
	SetCursor( &myCursor[ MyCrsrRec.current]);
}

void CloseRollCrsrc()
{
	DisposePtr( (Ptr) myCursor);
}