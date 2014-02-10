#include "Shuddup.h"
#include "RDriver.h"
#include "RDriverInt.h"
#include "Undo.h"
#include "Utils.h"

void UpdateStaffNote(short track);
void Update1Note(short tt, short track);

extern	MenuHandle EditMenu;
extern	DialogPtr	AdapDlog;

static	UndoCmd					myUndo;
static	Boolean					RedoMode;

static	Ptr						CopyAllInstruments[ MAXINSTRU];
static	InstrData				CopyFid;
static	PatData*				CopyPartition[ MAXPATTERN];
static	FXSets					*CopySets;

void UPDATE_Total(void)
{
	MADAttachDriverToMusic(MADDriver, curMusic, NULL);

	MADCleanCurrentMusic(curMusic, MADDriver);
	
	CreateInstruList();
	DrawInfoInstrument();
	UpdateSampleWindows();
	UpdateInstruMenu();
	
	UpdateMozartInfo();				// Classical Editor
	UpdateClassicInfo();			// Fast pattern view
	UpdateEditorInfo();				// Digital Editor
	UpdateCmdDlogInfo();			// Command Editor
	UpdatePatListInfo();			// Pattern List Update
	UpdatePartiInfo();
	UpdateWaveInfo();
	UpdateStaffInfo();
	ResetAdaptators();
}

void UPDATE_TrackActive(void)
{
	Rect	caRect;

	UpdateEditorInfo();
	UpdateWaveInfo();
	UpdateStaffInfo();
	UpdateMozartInfo();
	
	if (AdapDlog) {
		GetPortBounds(GetDialogPort(AdapDlog), &caRect);
		InvalWindowRect(GetDialogWindow(AdapDlog), &caRect);
		UpdateAdapWindow(AdapDlog);
	}
}

void UPDATE_Header(void)
{
	MADCleanCurrentMusic(curMusic, MADDriver);
	
	curMusic->hasChanged = true;
	
	// Action on instruments
	CreateInstruList();
	DrawInfoInstrument();
	UpdateSampleWindows();
	UpdateInstruMenu();
	
	//Partition
	UpdatePatListInfo();
	UpdatePartiInfo();
	ResetAdaptators();
}

void UPDATE_Note(short pos, short track)
{
	curMusic->hasChanged = true;
	
	SetPatternCell(pos, track);							// EDITOR
	
	Update1Note(pos, track);								// MOZART
	UpdateMozartInfoInternal();
	
	UpdateCmdWave(pos, track);								// WAVE
	
	UpdateCmdDlogInfo();									// CmdTools
	
	DrawCurrentNote(pos);									// ClassicalP
	
	UpdateStaffNote(track);
}

void UPDATE_NoteBOUCLE(short pos, short track)
{
	curMusic->hasChanged = true;
	
	SetPatternCell(pos, track);							// EDITOR
	
	Update1Note(pos, track);								// MOZART
	
	DrawCurrentNote(pos);									// ClassicalP

	UpdateStaffNote(track);
}

void UPDATE_NoteFINISH()
{
	curMusic->hasChanged = true;
	
	UpdateCmdDlogInfo();									// CmdTools
	
	UpdateMozartInfoInternal();
	
	UpdateWaveInfo();
}


void UPDATE_Pattern()
{
	MADCleanCurrentMusic(curMusic, MADDriver);
	
	UpdateEditorInfo();
	UpdateMozartInfo();
	UpdateWaveInfo();
	UpdateStaffInfo();
	UpdateCmdDlogInfo();
	UpdateClassicInfo();
	UpdatePatListInfo();
	
	if (MADDriver->PartitionReader >= curMusic->partition[ MADDriver->Pat]->header.size) {
		MADDriver->PartitionReader = curMusic->partition[ MADDriver->Pat]->header.size - 1;
	}
}

void InitUndo(void)
{
	short	i;
	
	for (i = 0; i < MAXINSTRU; i++) CopyAllInstruments[i] = NULL;
	for (i = 0; i < MAXPATTERN; i++) CopyPartition[i] = NULL;
	
	myUndo.UndoType = 0;
	myUndo.ID = 0;
	myUndo.data = NULL;
	RedoMode = false;
	
	DisableMenuItem(EditMenu, 1);
	
	CopySets = (FXSets*)NewPtrClear(MAXTRACK * sizeof(FXSets));
}

void ResetUndo(void)
{
	short	i;
	
	myUndo.UndoType = 0;
	myUndo.ID = 0;
	
	if (myUndo.data != NULL)
		DisposePtr(myUndo.data);
	myUndo.data = NULL;
	myUndo.dataSize = 0;
	
	BlockZero(CopySets, MAXTRACK * sizeof(FXSets));
	
	for (i = 0; i < MAXINSTRU; i++) {
		if (CopyAllInstruments[i] != NULL)
			DisposePtr(CopyAllInstruments[i]);
		CopyAllInstruments[i] = NULL;
	}
	
	for (i = 0; i < MAXPATTERN; i++) {
		if (CopyPartition[i] != NULL)
			DisposePtr((Ptr)CopyPartition[i]);
		CopyPartition[i] = NULL;
	}
	
	RedoMode = false;
	
	DisableMenuItem(EditMenu, 1);
}

void SaveUndo(short UndoType, short ID, Str255 textMenu)
{
	short		i, x;
	long		tempL;
	
	MADCleanCurrentMusic(curMusic, MADDriver);
	
	RedoMode = false;
	
	myUndo.UndoType = UndoType;
	myUndo.ID = ID;
	
	if (myUndo.data != NULL)
		DisposePtr(myUndo.data);
	myUndo.data = NULL;
	
	for (i = 0; i < MAXINSTRU; i++) {
		if (CopyAllInstruments[i] != NULL)
			DisposePtr(CopyAllInstruments[i]);
		CopyAllInstruments[i] = NULL;
	}
	for (i = 0; i < MAXPATTERN; i++) {
		if (CopyPartition[i] != NULL)
			DisposePtr((Ptr)CopyPartition[i]);
		CopyPartition[i] = NULL;
	}
	
	switch (UndoType) {
		case UPattern:
			myUndo.dataSize = sizeof(PatHeader) + curMusic->header->numChn * curMusic->partition[ ID]->header.size * sizeof(Cmd);
			
			if (ID < 0 || ID >= curMusic->header->numPat)
				MyDebugStr(__LINE__, __FILE__, "Undo Pattern error.");
			else {
				myUndo.data = NewPtr(myUndo.dataSize);
				if (myUndo.data == NULL)
					ResetUndo();
				else
					BlockMoveData(curMusic->partition[ID], myUndo.data, myUndo.dataSize);
			}
			break;
			
		case USample:
			CopyFid = curMusic->fid[ ID];
			
			myUndo.dataSize = 0;
			for (i = 0; i < CopyFid.numSamples; i++) {
				sData	*curData = curMusic->sample[ curMusic->fid[ID].firstSample + i];
				myUndo.dataSize	+= sizeof(sData) + curData->size;
			}
			
			myUndo.data 		= NewPtr(myUndo.dataSize);
			if (myUndo.data == NULL)
				ResetUndo();
			else {
				myUndo.dataSize = 0;
				for (i = 0; i < CopyFid.numSamples; i++) {
					sData	*curData = curMusic->sample[ curMusic->fid[ ID].firstSample +  i];
					
					if (curData == NULL)
						MyDebugStr(__LINE__, __FILE__, "BIG UNDO Error 1");
					else if (curData->size > 0) {
						if (curData->data == NULL)
							MyDebugStr(__LINE__, __FILE__, "BIG UNDO Error 2A");
						if (GetPtrSize(curData->data) != curData->size)
							MyDebugStr(__LINE__, __FILE__, "BIG UNDO Error 3A");
					}
					
					BlockMoveData(curData, myUndo.data + myUndo.dataSize, sizeof(sData));
					myUndo.dataSize	+= sizeof(sData);
					
					if (curData->size > 0) {
						BlockMoveData(curData->data, myUndo.data + myUndo.dataSize, curData->size);
						myUndo.dataSize	+= curData->size;
					}
				}
			}
			break;
			
		case UHeader:
		case UAllSamples:
		case UAllPatterns:
			myUndo.dataSize = sizeof(struct MADSpec);
			
			myUndo.data = NewPtr(myUndo.dataSize);
			if (myUndo.data == NULL) {
				ResetUndo();
				break;
			} else
				BlockMoveData(curMusic->header, myUndo.data, myUndo.dataSize);
			
			BlockMoveData(curMusic->sets, CopySets, MAXTRACK * sizeof(FXSets));
			
			/*** UAllSamples ***/
			if (UndoType == UAllSamples) {
				for (i = 0; i < MAXINSTRU; i++) {
					long size;
					
					size = sizeof(InstrData);
					for (x = 0; x < curMusic->fid[ i].numSamples; x++) {
						sData	*curData = curMusic->sample[ curMusic->fid[ i].firstSample +  x];
						
						size	+= sizeof(sData) + curData->size;
					}
					
					CopyAllInstruments[i] = NewPtr(size);
					if (CopyAllInstruments[i] != NULL) {
						size = 0;
						BlockMoveData(&curMusic->fid[i], CopyAllInstruments[i] + size, sizeof(InstrData));
						size += sizeof(InstrData);
						
						for (x = 0; x < curMusic->fid[ i].numSamples; x++) {
							sData	*curData = curMusic->sample[ curMusic->fid[ i].firstSample +  x];
							
							if (curData == NULL)
								MyDebugStr(__LINE__, __FILE__, "BIG UNDO Error 1");
							if (curData->size > 0) {
								if (curData->data == NULL)
									MyDebugStr(__LINE__, __FILE__, "BIG UNDO Error 2");
								if (GetPtrSize(curData->data) != curData->size)
									MyDebugStr(__LINE__, __FILE__, "BIG UNDO Error 3");
							}
							
							BlockMoveData(curData, CopyAllInstruments[ i] + size, sizeof(sData));
							size	+= sizeof(sData);
							
							if (curData->size > 0) {
								BlockMoveData(curData->data, CopyAllInstruments[ i] + size, curData->size);
								size	+= curData->size;
							}
						}
					} else {
						ResetUndo();
						i = MAXINSTRU;
					}
				}
			}
			
			/*** UAllPatterns ***/
			if (UndoType == UAllPatterns) {
				for (i = 0; i < MAXPATTERN; i++) {
					CopyPartition[ i]  = NULL;
					
					if (i >= curMusic->header->numPat) if (curMusic->partition[ i] != NULL) MyDebugStr(__LINE__, __FILE__, "Check Partition-Partition UNDO.");
					
					if (curMusic->partition[i] != NULL) {
						tempL = sizeof(PatHeader) + curMusic->header->numChn * curMusic->partition[i]->header.size * sizeof(Cmd);
						
						if (GetPtrSize((Ptr)curMusic->partition[i]) != tempL)
							MyDebugStr(__LINE__, __FILE__, "Check Create Partition SIZE");
						
						CopyPartition[i] = (PatData*)NewPtr(tempL);
						if (CopyPartition[i] != NULL) {
							BlockMoveData(curMusic->partition[i], CopyPartition[i], tempL);
						}
					}
				}
			}
			break;
			
		default:
			MyDebugStr(__LINE__, __FILE__, "Unknown UNDO");
			break;
	}
	
	EnableMenuItem(EditMenu, 1);
	SetMenuItemText(EditMenu, 1, textMenu);
}

void DoUndo(void)
{
	Ptr				myTempUndo, myTempInstru, myTempSets;
	long			myTempSize, size;
	InstrData		myTempFid;
	Str255			myStr;
	short			i, x;
	Boolean			IsReading;

	if (myUndo.UndoType == 0) {
		MyPPBeep();
		return;
	}
	
	MADCleanCurrentMusic(curMusic, MADDriver);
	
	IsReading = MADDriver->Reading;
	MADDriver->Reading = false;
	MADPurgeTrack(MADDriver);
	
	switch (myUndo.UndoType) {
		case UPattern:
			/** REDO **/
			myTempUndo = NewPtr(myUndo.dataSize);
			if (myTempUndo)
			{
				BlockMoveData(curMusic->partition[ myUndo.ID], myTempUndo, myUndo.dataSize);
				/********/
			
				/** UNDO **/
				BlockMoveData(myUndo.data, curMusic->partition[ myUndo.ID], myUndo.dataSize);
				/********/
				
				/** REDO **/
				DisposePtr(myUndo.data);
				myUndo.data		= myTempUndo;
				/********/
				
				UPDATE_Pattern();
			}
			break;
		
		case USample:
			// ** REDO **
			myTempFid 	= curMusic->fid[ myUndo.ID];
			myTempSize 	= 0;
			for (i = 0; i < myTempFid.numSamples; i++) {
				sData	*curData = curMusic->sample[curMusic->fid[myUndo.ID].firstSample +  i];
				myTempSize	+= sizeof(sData) + curData->size;
			}
			
			myTempUndo 		= NewPtr(myTempSize);
			if (myTempUndo == NULL) {
				ResetUndo();
				break;
			} else {
				myTempSize = 0;
				for (i = 0; i < myTempFid.numSamples; i++) {
					sData	*curData = curMusic->sample[ curMusic->fid[ myUndo.ID].firstSample +  i];
					
					BlockMoveData(curData, myTempUndo + myTempSize, sizeof(sData));
					myTempSize	+= sizeof(sData);
					
					if (curData->size > 0) {
						BlockMoveData(curData->data, myTempUndo + myTempSize, curData->size);
						myTempSize	+= curData->size;
					}
				}
			}
			// ********
			
			
			// ** UNDO **
			
			// After save for Redo, delete it !!
			MADKillInstrument(curMusic, myUndo.ID);
			
			x = curMusic->fid[myUndo.ID].firstSample;
			curMusic->fid[myUndo.ID]	= CopyFid;
			curMusic->fid[myUndo.ID].firstSample = x;
			curMusic->fid[myUndo.ID].numSamples = 0;
			
			size = 0;
			for (i = 0; i < CopyFid.numSamples; i++) {
				sData	*curData;
				
				/* Sample header */
				curData = MADCreateSample(curMusic, myUndo.ID, i);
				if (curData) {
					BlockMoveData(myUndo.data + size, curData, sizeof(sData));
					size += sizeof(sData);
					
					/* Sample data */
					curData->data = NewPtr(curData->size);
					if (curData->data) {
						BlockMoveData(myUndo.data + size, curData->data, curData->size);
						size += curData->size;
					}
				}
			}
			if (size != myUndo.dataSize)
				MyDebugStr(__LINE__, __FILE__, "USample DoUndo() Prob");
			
			// ********
			
			// ** REDO **
			DisposePtr(myUndo.data);
			myUndo.data		= myTempUndo;
			myUndo.dataSize	= myTempSize;
			CopyFid			= myTempFid;
			// ********
			
			CreateInstruList();
			DrawInfoInstrument();
			UpdateSampleWindows();
			UpdateInstruMenu();
			UpdateSampleWindows();
			break;
		
		case UAllSamples:
		case UAllPatterns:
		case UHeader:
		
			// *********************
			// HEADER
			// *********************
			
			/** REDO **/
			myTempUndo = NewPtr(myUndo.dataSize);
			if (myTempUndo == NULL)
				MyDebugStr(__LINE__, __FILE__, "UndoError - MEMORY");
			BlockMoveData(curMusic->header, myTempUndo, myUndo.dataSize);
			
			myTempSets = NewPtrClear(MAXTRACK * sizeof(FXSets));
			if (myTempUndo == NULL)
				MyDebugStr(__LINE__, __FILE__, "UndoError - MEMORY");
			BlockMoveData(curMusic->sets, myTempSets, MAXTRACK * sizeof(FXSets));
			/********/
			
			/** UNDO **/
			BlockMoveData(myUndo.data, curMusic->header, myUndo.dataSize);
			BlockMoveData(CopySets, curMusic->sets, MAXTRACK * sizeof(FXSets));
			/********/
			
			/** REDO **/
			DisposePtr(myUndo.data);
			myUndo.data = myTempUndo;
			BlockMoveData(myTempSets, CopySets, MAXTRACK * sizeof(FXSets));
			DisposePtr(myTempSets);
			/********/
			
			// *********************
			// *** UAllSamples
			// *********************
			
			if (myUndo.UndoType == UAllSamples) {
				for (i = 0; i < MAXINSTRU; i++) {
					long size;
					
					// ** REDO **
					myTempSize 	= sizeof(InstrData);
					for (x = 0; x < curMusic->fid[i].numSamples; x++) {
						sData	*curData = curMusic->sample[curMusic->fid[i].firstSample +  x];
						myTempSize	+= sizeof(sData) + curData->size;
					}
					
					myTempInstru 	= NewPtr(myTempSize);
					if (myTempInstru != NULL) {
						myTempSize = 0;
						BlockMoveData(&curMusic->fid[i], myTempInstru + myTempSize, sizeof(InstrData));
						myTempSize	+= sizeof(InstrData);
						
						for (x = 0; x < curMusic->fid[i].numSamples; x++) {
							sData	*curData = curMusic->sample[curMusic->fid[i].firstSample + x];
							
							BlockMoveData(curData, myTempInstru + myTempSize, sizeof(sData));
							myTempSize	+= sizeof(sData);
							
							if (curData->size > 0) {
								BlockMoveData(curData->data, myTempInstru + myTempSize, curData->size);
								myTempSize	+= curData->size;
							}
						}
						
						// After save for Redo, delete it !!
						MADKillInstrument(curMusic, i);
					}
					
					
					// *** UNDO *****
					
					size = 0;
					if (CopyAllInstruments[i] != NULL) {
						short numSamples, firstSample;
						
						firstSample = curMusic->fid[ i].firstSample;
						
						size = 0;
						BlockMoveData(CopyAllInstruments[ i] + size, &curMusic->fid[ i], sizeof(InstrData));
						size += sizeof(InstrData);
						
						curMusic->fid[ i].firstSample = firstSample;
						
						numSamples = curMusic->fid[ i].numSamples;
						curMusic->fid[ i].numSamples = 0;
						
						for (x = 0; x < numSamples; x++) {
							sData	*curData;
							
							// Sample header
							curData = MADCreateSample(curMusic, i, x);
							if (curData) {
								BlockMoveData(CopyAllInstruments[i] + size, curData, sizeof(sData));
								size += sizeof(sData);
								
								// Sample data
								curData->data = NewPtr(curData->size);
								if (curData->data) {
									BlockMoveData(CopyAllInstruments[ i] + size, curData->data, curData->size);
									size += curData->size;
								}
							}
						}
						
						DisposePtr(CopyAllInstruments[i]);
						CopyAllInstruments[i] = NULL;
					}
					
					// ** REDO **
					CopyAllInstruments[i] = myTempInstru;
					// ********
				}
			}
			
			/*** UAllPatterns ***/

			if (myUndo.UndoType == UAllPatterns) {
				for (i = 0; i < MAXPATTERN; i++) {
					/** REDO **/
					if (curMusic->partition[i] != NULL) {
						myTempInstru = NewPtr(GetPtrSize((Ptr)curMusic->partition[i]));
						if (myTempInstru != NULL) {
							BlockMoveData(curMusic->partition[i], myTempInstru, GetPtrSize((Ptr)curMusic->partition[i]));
						}
						// After save for Redo, delete it !!
						
						DisposePtr((Ptr)curMusic->partition[i]);
						curMusic->partition[i] = NULL;
					} else
						myTempInstru = NULL;
					/********/

					if (CopyPartition[i] != NULL) {
						if (i >= curMusic->header->numPat)
							MyDebugStr(__LINE__, __FILE__, "UNDO Partition ERROR");
					
						curMusic->partition[i] = (PatData*)NewPtr(GetPtrSize((Ptr)CopyPartition[i]));
						if (curMusic->partition[i] != NULL) {
							BlockMoveData(CopyPartition[i], curMusic->partition[i], GetPtrSize((Ptr) CopyPartition[i]));
						}
						
						DisposePtr((Ptr)CopyPartition[i]);
						CopyPartition[i] = NULL;
					}
					
					/** REDO **/
					CopyPartition[i] = (PatData*)myTempInstru;
					/********/
				}
			}
			
			UPDATE_Total();
			break;
		
		default:
			MyDebugStr(__LINE__, __FILE__, "Unknown UNDO");
			break;
	}
	
	MADDriver->Reading = IsReading;
	
	RedoMode = !RedoMode;
	
	GetMenuItemText(EditMenu, 1, myStr);
	if (RedoMode) {
		myStr[1] = 'R';
		myStr[2] = 'e';
		myStr[3] = 'd';
		myStr[4] = 'o';
	} else {
		myStr[1] = 'U';
		myStr[2] = 'n';
		myStr[3] = 'd';
		myStr[4] = 'o';
	}
	SetMenuItemText(EditMenu, 1, myStr);
}
