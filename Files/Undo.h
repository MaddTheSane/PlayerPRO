#ifndef __UNDOH__
#define __UNDOH__

enum
{
	UPattern = 1,
	UHeader = 2,
	USample = 3,
	UAllPatterns = 4,
	UAllSamples
};

typedef struct
{
	short		UndoType;
	short		ID;
	Ptr			data;
	long			dataSize;
} UndoCmd;

void SaveUndo( short UndoType, short ID, Str255 textMenu);

#endif
