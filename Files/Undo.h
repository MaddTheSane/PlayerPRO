#ifndef __UNDOH__
#define __UNDOH__
#include <Carbon/Carbon.h>

enum
{
	UPattern = 1,
	UHeader,
	USample,
	UAllPatterns,
	UAllSamples
};

typedef struct
{
	short	UndoType;
	short	ID;
	Ptr		data;
	long	dataSize;
} UndoCmd;

void SaveUndo(short UndoType, short ID, Str255 textMenu);

#endif
