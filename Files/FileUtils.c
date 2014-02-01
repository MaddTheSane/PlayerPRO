#include <Carbon/Carbon.h>

extern	KeyMap		km;
		void DrawNumberMusic();

/*
 * Pascal string utilities
 */
/*
 * pstrcat - add string 'src' to end of string 'dst'
 */
void pstrcat(StringPtr dst, StringPtr src)
{
	/* copy string in */
	BlockMoveData(src + 1, dst + *dst + 1, *src);
	/* adjust length byte */
	*dst += *src;
}

/*
 * pstrinsert - insert string 'src' at beginning of string 'dst'
 */
void pstrinsert(StringPtr dst, StringPtr src)
{
	/* make room for new string */
	BlockMoveData(dst + 1, dst + *src + 1, *dst);
	/* copy new string in */
	BlockMoveData(src + 1, dst + 1, *src);
	/* adjust length byte */
	*dst += *src;
}
void ScanDir(long dirID, short VRefNum, Boolean);

extern	DialogPtr MODListDlog;
		Boolean remonte;

void DoScanDir(long	DirID, short VRefNum)
{
	GrafPtr	savePort;
	Rect	caRect;
	
	GetPort(&savePort);
	SetPortDialogPort(MODListDlog);
	
	remonte = false;
	
	ScanDir(DirID, VRefNum, true);
	
	GetPortBounds(GetDialogPort(MODListDlog), &caRect);
	
	InvalWindowRect(GetDialogWindow(MODListDlog), &caRect);
	
	SetPort(savePort);
}

static Str255 asc_WorkStr;

void ScanDir(long dirID, short VRefNum, Boolean recurse)
{
	short			i;
	CInfoPBRec		info;
	FSSpec			spec;
	
	if (remonte) return;
	
	info.hFileInfo.ioNamePtr = asc_WorkStr;
	info.hFileInfo.ioVRefNum = VRefNum;
	for (i = 1; 1; i ++) {
		info.hFileInfo.ioDirID = dirID;
		info.hFileInfo.ioFDirIndex = i;
		if (PBGetCatInfoSync(&info) != noErr)
			break;
		
		pStrcpy(spec.name, info.hFileInfo.ioNamePtr);
		spec.vRefNum = info.hFileInfo.ioVRefNum;
		spec.parID = dirID;
		
		RollCursor();
		
		if (OpenableFile(info.hFileInfo.ioFlFndrInfo.fdType, &spec)) {
			if (info.hFileInfo.ioFlFndrInfo.fdType != 'sTAT' && info.hFileInfo.ioFlFndrInfo.fdType != 'STCf')
				AddMODList(false,
						   info.hFileInfo.ioNamePtr,
						   info.hFileInfo.ioVRefNum,
						   dirID);
		} else if (info.hFileInfo.ioFlAttrib & 16) {
			ScanDir(info.dirInfo.ioDrDirID, VRefNum, true);
		}
		
		GetKeys(km);
		if (IsPressed(0x37) && IsPressed(0x2F)) {
			remonte = true;
			break;
		}
	}
}

void PathNameFromDirID(long dirID, short vRefNum, StringPtr fullPathName)
{
	CInfoPBRec	block;
	Str255		directoryName;
	OSErr		err;
	
	pStrcpy(directoryName, "\p");
	pStrcpy(fullPathName, "\p");
	
	block.dirInfo.ioDrParID = dirID;
	block.dirInfo.ioNamePtr = directoryName;
	do {
		block.dirInfo.ioVRefNum = vRefNum;
		block.dirInfo.ioFDirIndex = -1;
		block.dirInfo.ioDrDirID = block.dirInfo.ioDrParID;
		err = PBGetCatInfoSync(&block);
		if (fullPathName[ 0] < 200)
		{
			pstrcat(directoryName, (StringPtr)"\p: ");
			pstrinsert(fullPathName, directoryName);
		}
	} while (block.dirInfo.ioDrDirID != 2 && err == noErr);
}
