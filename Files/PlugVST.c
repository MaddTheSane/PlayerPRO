#include "Shuddup.h"
#include "MAD.h"
#include <stdio.h>

#include "aeffectx.h"
#include "RDriver.h"

#include <Carbon/Carbon.h>

	extern	EventRecord				theEvent, gModalDialogEvent;
	extern	Cursor					HandCrsr;
	extern	DialogPtr				AHelpDlog;
	extern	WindowPtr				oldWindow;
	extern	MenuHandle				SampleMenu;
	extern	KeyMap					km;
	extern	RGBColor				theColor;
	static	DialogPtr				VSTDlog;
	static	short					CurrentcurSelec;
	
			short					tPlug;
			short					PlugsFolderOK;
	
	void SampleUpdateNow(void);
	VSTEffect* CreateVSTEffect(short effectID);
	void DisposeVSTEffect(VSTEffect *myEffect);
	long dispatcher(VSTEffect *effect, Boolean applyOnBoth, long opCode, long index, long value, void *ptr, float opt);
	Boolean VSTEditorClose(DialogPtr aDia, short itemHit);
	Boolean VSTEditorOpen(VSTEffect *ce, sData	*curData, long Start, long End, Boolean StereoMode, short channelID);
	void VSTEditorDoItemPress(short itemHit, DialogPtr aDia);
	void FillVSTEffects(void);
	OSErr GetApplicationPackageFSSpecFromBundle(FSSpecPtr theFSSpecPtr);
	
static	float 		*inputs[ 2], *outputs[ 2], *outputs2[ 2];
static	long		currentdatasize;

	audioMasterCallback audioMaster;

#define VSTBUFFERSIZE 4096
	
static	Boolean					ReadyToProcess = false;

typedef struct
{
	long						ID;
	FSSpec						file;
	
	Ptr							call;
	
}	VSTInfo;

static	VSTInfo 		*VSTPlug;

#define MAXVSTITEM 80
#define MAXVSTPREF 500
#define VSTPREFNAME "\pPlayerPRO VST Prefs"

typedef struct
{
	long						ID;
	Str63						name;
	long						flag;
	float						value[ MAXVSTITEM];
}	VSTPrefsStruct;

static	VSTPrefsStruct	*VSTPref[ MAXVSTPREF];

void PurgeVSTEffects(void)
{
	short i, x;
	
	for (i = 0; i < 10; i++)
	{
		if (MADDriver->masterVST[ i] != NULL)
		{
			if (MADDriver->masterVST[ i]->Active)
			{
				dispatcher(MADDriver->masterVST[ i], true, effMainsChanged, 0, false, 0, 0);
				dispatcher(MADDriver->masterVST[ i], true, effMainsChanged, 0, true, 0, 0);
			}
		}
	}
	
	for (i = 0; i < MAXTRACK; i++)
	{
		for (x = 0; x < 4; x++)
		{
			if (MADDriver->chanVST[ i][ x] != NULL)
			{
				if (MADDriver->chanVST[ i][ x]->Active)
				{
					dispatcher(MADDriver->chanVST[ i][ x], true, effMainsChanged, 0, false, 0, 0);
					dispatcher(MADDriver->chanVST[ i][ x], true, effMainsChanged, 0, true, 0, 0);
				}
			}
		}
	}
}

long dispatcher(VSTEffect *effect, Boolean applyOnBoth, long opCode, long index, long value, void *ptr, float opt)
{
	short	fileID;
	long	result;

	fileID = FSpOpenResFile(&VSTPlug[ effect->id].file, fsCurPerm);
	UseResFile(fileID);
	
	result = effect->ce[ 0]->dispatcher(effect->ce[ 0], opCode, index, value, ptr, opt);
	
	if (applyOnBoth)
	{
		if (effect->ce[ 1])
		{
			result = effect->ce[ 1]->dispatcher(effect->ce[ 1], opCode, index, value, ptr, opt);
		}
	}
	
	CloseResFile(fileID);
	
	return result;
}

void GetVSTPrefName(Str255 str, long *flag)
{
	short		itemHit,i;
	DialogPtr	TheDia;
	GrafPtr		myPort;
	
	GetPort(&myPort);

	TheDia = GetNewDialog(187,NULL, (WindowPtr) -1L);
	SetPortDialogPort(TheDia);
	AutoPosition(TheDia);
	
	if (str[ 0] == 0) SetDText(TheDia, 4, "\pMy new settings");
	else SetDText(TheDia, 4, str);
	
	SelectDialogItemText(TheDia, 4, 0, 32767);
	
	if (*flag) InverseRadio(6, TheDia);
	
	do
	{
		ModalDialog(MyDlgFilterDesc, &itemHit);
	
		switch(itemHit)
		{
			case 6:
				InverseRadio(6, TheDia);
				*flag = !*flag;
			break;
		}
	
	}while (itemHit != 1 && itemHit != 2);

	if (itemHit == 1)
	{
		GetDText(TheDia, 4, str);
	}
	else str[ 0] = 0;
	
	DisposeDialog(TheDia);
	
	SetPort(myPort);
}

short PressPresetButton(long id, Point myPt, short *curSelec)
{
	MenuHandle	presetMenu;
	long		i, mresult, maxitem, temp;
	short		returnVal = 0, table[ 200];
	
	presetMenu = NewMenu(244, "\pPreset Menu");
	
	
	for (i = 0, maxitem = 0; i < MAXVSTPREF;i++)
	{
		if (VSTPref[ i] != NULL)
		{
			if (VSTPref[ i]->ID == id)
			{
				AppendMenu(presetMenu, VSTPref[ i]->name);
				table[ maxitem] = i;
				maxitem++;
			}
		}
	}
	
	if (maxitem == 0)
	{
		AppendMenu(presetMenu, "\p(No settings available");
	}
	
	AppendMenu(presetMenu, "\p-");
	AppendMenu(presetMenu, "\pAdd current Settings");
	
	InsertMenu(presetMenu, hierMenu);
	
	LocalToGlobal(&myPt);
	
	if (*curSelec != -1)
	{
		SetItemMark(presetMenu, *curSelec+1, 0xa5);
	
		mresult = PopUpMenuSelect(	presetMenu,
									myPt.v,
									myPt.h,
									*curSelec+1);
	
		SetItemMark(presetMenu, *curSelec+1, 0);
	}
	else
	{
		mresult = PopUpMenuSelect(	presetMenu,
									myPt.v,
									myPt.h,
									maxitem + 2);
	}
	
	if (HiWord(mresult ) != 0 )
	{
		if (LoWord(mresult) - 1 >= maxitem)
		{
			*curSelec = LoWord(mresult) - 2;
			
			returnVal = -1;
		}
		else
		{
			temp = LoWord(mresult) - 1;
			returnVal = table[ temp];
			
			GetKeys(km);
			if (IsPressed(0x33))		// Delete this pref
			{
				if (InfoL(104))
				{
					DisposePtr((Ptr) VSTPref[ table[ temp]]);
					VSTPref[ table[ temp]] = NULL;
					
					*curSelec = -1;
				}
				returnVal = -2;
			}
			else *curSelec = temp;
		}
	}
	else returnVal = -2;
	
	DeleteMenu(GetMenuID(presetMenu));
	
	DisposeMenu(presetMenu);
	
	return returnVal;
}

void HandleVSTChoice(short item, VSTEffect** vst, short channelID)
{
	CurrentcurSelec = -1;	// for the popup !
	
	if (*vst)
	{
	}
	else
	{
		*vst = CreateVSTEffect(item);
	}
	
	VSTEditorOpen(*vst, NULL, 0, 0, false, channelID);
	
/*	if (VSTEditor(*vst, NULL, 0, 0, false) == false)
	{
		DisposeVSTEffect(*vst);
		*vst = NULL;
	}*/
}

long audioMasterFct(AEffect *effect, long opcode, long index, long value, void *ptr, float opt)
{
	long	returnVal = 0;

	switch(opcode)
	{
		case audioMasterAutomate:
			value = 0;	index = 0;	returnVal = 0;
			break;
		
		case audioMasterVersion:
			returnVal = 2;
			break;
		
		case audioMasterIdle:
			
			break;
		
		case audioMasterPinConnected:
		
			break;
		
		case audioMasterWillReplaceOrAccumulate:
		
			break;
		
		case audioMasterOpenWindow:
		//	Debugger();
			break;
		
		case audioMasterProcessEvents:
		//		Debugger();
			break;
		
		case audioMasterWantMidi:
		//		Debugger();
			break;
		
		case audioMasterSizeWindow:
		//	Debugger();
			break;
		
		default:
		//	Debugger();
			break;
	}
	
	return returnVal;
}

/*void VSTActivate(short no, Boolean state)
{
	VSTPlug[ no].Active = state;
}*/

static inline short ClipConvert(float x)
{
	//A FAIRE : LA GESTION DU CLIP ICI !!!!
	
	if (x > 1) return 32767;
	else if (x < -1) return -32767;
	else return x * 32767.;
}

static inline char ClipConvert8(float x)
{
	//A FAIRE : LA GESTION DU CLIP ICI !!!!
	
	if (x >= 1) return 127;
	else if (x <= -1) return -128;
	else return x * 128.;
}

void ApplyVSTFilter(VSTEffect *effect, sData *SDataSrc, long Start, long End)
{
	short			*data16;
	char			*data8;
	long			datasize, byteleft;
	long			i, x;
	float 			*in1	=	inputs[ 0];
    float 			*in2	=	inputs[ 1];
    
    float 			*out1	=	outputs[0];
    float 			*out2	=	outputs[1];
    
    float 			*out21	=	outputs2[0];
    float 			*out22	=	outputs2[1];
    
    AEffect			*ce[ 2];
	
	MADDriver->currentlyExporting = true;
	dispatcher(effect, true, effMainsChanged, 0, false, 0, 0);
	dispatcher(effect, true, effMainsChanged, 0, true, 0, 0);
	
	ce[ 0] = effect->ce[ 0];
	ce[ 1] = effect->ce[ 1];
	
	//byteleft = SDataSrc->size;
	
	byteleft = End - Start;
	
	data16 = (short*) (SDataSrc->data + Start);
	data8 = (char*) (SDataSrc->data + Start);
	
	while (byteleft > 0)
	{
		if (SDataSrc->stereo)
		{
			if (SDataSrc->amp == 8)
			{
				byteleft -= VSTBUFFERSIZE*2;
				
				if (byteleft < 0) datasize = (byteleft + VSTBUFFERSIZE*2)/2;
				else datasize = VSTBUFFERSIZE;
			}
			else
			{
				byteleft -= VSTBUFFERSIZE*4;
				
				if (byteleft < 0) datasize = (byteleft + VSTBUFFERSIZE*4)/4;
				else datasize = VSTBUFFERSIZE;
			}
		}
		else
		{
			if (SDataSrc->amp == 8)
			{
				byteleft -= VSTBUFFERSIZE;
				
				if (byteleft < 0) datasize = byteleft + VSTBUFFERSIZE;
				else datasize = VSTBUFFERSIZE;
			}
			else
			{
				byteleft -= VSTBUFFERSIZE*2;
				
				if (byteleft < 0) datasize = (byteleft + VSTBUFFERSIZE*2)/2;
				else datasize = VSTBUFFERSIZE;
			}
		}
		
		if (SDataSrc->stereo)
		{
			if (SDataSrc->amp == 8)
			{
				for (i = 0; i < datasize; i++)
				{
					in1[ i] = data8[ i*2] / 128.;
					in2[ i] = data8[ i*2 + 1] / 128.;
				}
			}
			else
			{
				for (i = 0; i < datasize; i++)
				{
					in1[ i] = data16[ i*2] / 32767.;
					in2[ i] = data16[ i*2 + 1] / 32767.;
				}
			}
		}
		else
		{
			if (SDataSrc->amp == 8)
			{
				for (i = 0; i < datasize; i++)
				{
					in1[ i] = data8[ i] / 128.;
					in2[ i] = data8[ i] / 128.;
				}
			}
			else
			{
				for (i = 0; i < datasize; i++)
				{
					in1[ i] = data16[ i] / 32767.;
					in2[ i] = data16[ i] / 32767.;
				}
			}
		}
		
		if (datasize != VSTBUFFERSIZE)
		{
			for (i = datasize; i < VSTBUFFERSIZE; i++)
			{
				in1[ i] = 0;
				in2[ i] = 0;
			}
		}
		
		// APPLY FILTER -- APPLY FILTER -- APPLY FILTER -- APPLY FILTER -- APPLY FILTER -- APPLY FILTER -- 
		
		if (ce[ 0]->numInputs == 2 && ce[ 0]->numOutputs == 2)
			ce[ 0]->processReplacing(ce[ 0], inputs, outputs, datasize);
		
		if (ce[ 0]->numInputs == 1 && ce[ 0]->numOutputs == 1)
		{
			ce[ 0]->processReplacing(ce[ 0], inputs, outputs, datasize);
			ce[ 0]->processReplacing(ce[ 1], inputs + 1, outputs + 1, datasize);
		}
		
		if (ce[ 0]->numInputs == 1 && ce[ 0]->numOutputs == 2)
		{
			for (i = 0; i < datasize; i++)
			{
				out1[ i] = 0;
				out2[ i] = 0;
				
				out21[ i] = 0;
				out22[ i] = 0;
			}
							
			ce[ 0]->processReplacing(ce[ 0], inputs, outputs, datasize);
			for (i = 0; i < datasize; i++) out1[ i] = (out1[ i] + out2[ i]) / 2.0;
			
			ce[ 1]->processReplacing(ce[ 1], &inputs[ 1], outputs2, datasize);
			for (i = 0; i < datasize; i++) out2[ i] = (out21[ i] + out22[ i]) / 2.0;
		}
		
		BlockMoveData(out1, in1, sizeof(float) * datasize);
		BlockMoveData(out2, in2, sizeof(float) * datasize);
		
		// APPLY FILTER -- APPLY FILTER -- APPLY FILTER -- APPLY FILTER -- APPLY FILTER -- APPLY FILTER -- 
		
		if (SDataSrc->stereo)
		{
			if (SDataSrc->amp == 8)
			{
				for (i = 0; i < datasize; i++)
				{
					data8[ i*2] 	= ClipConvert8(in1[ i]);
					data8[ i*2 + 1] = ClipConvert8(in2[ i]);
				}
			}
			else
			{
				for (i = 0; i < datasize; i++)
				{
					data16[ i*2] 		= ClipConvert(in1[ i]);
					data16[ i*2 + 1] 	= ClipConvert(in2[ i]);
				}
			}
		}
		else
		{
			if (SDataSrc->amp == 8)
			{
				for (i = 0; i < datasize; i++)
				{
					data8[ i] = (ClipConvert8(in1[ i]) + ClipConvert8(in2[ i])) / 2.;
				}
			}
			else
			{
				for (i = 0; i < datasize; i++)
				{
					data16[ i] = (ClipConvert(in1[ i]) + ClipConvert(in2[ i])) / 2.;
				}
			}
		}
		
		if (SDataSrc->stereo)
		{
			data16 += datasize*2;
			data8 += datasize*2;
		}
		else
		{
			data16 += datasize;
			data8 += datasize;
		}
	}
	
	dispatcher(effect, true, effMainsChanged, 0, false, 0, 0);
	dispatcher(effect, true, effMainsChanged, 0, true, 0, 0);
	MADDriver->currentlyExporting = false;
	
	SampleUpdateNow();
}

Boolean IsVSTChanEffect(MADDriverRec *intDriver, short channel)
{
	if (ReadyToProcess == false) return false;
	
	if (intDriver->curMusic != NULL)
	{
		if (intDriver->curMusic->header != NULL)
		{
			if (intDriver->curMusic->header->MultiChan)
			{
				Channel *curVoice = &intDriver->chan[ channel];
				
				if (curVoice->curPtr >= curVoice->maxPtr && curVoice->loopSize == 0)
				{
					if (curVoice->TimeCounter > 0)
					{
						channel = curVoice->TrackID;
					}
					else return false;
				}
				else channel = curVoice->TrackID;
			}
			
			if (intDriver->curMusic->header->chanBus[ channel].Active == false) return false;
			
			channel = intDriver->curMusic->header->chanBus[ channel].copyId;
			
		//	if (chanBus[ channel]->Active == false) return false;
			
			if (intDriver->chanVST[ channel][ 0]) if (intDriver->chanVST[ channel][ 0]) return true;
			if (intDriver->chanVST[ channel][ 1]) if (intDriver->chanVST[ channel][ 1]) return true;
			if (intDriver->chanVST[ channel][ 2]) if (intDriver->chanVST[ channel][ 2]) return true;
			if (intDriver->chanVST[ channel][ 3]) if (intDriver->chanVST[ channel][ 3]) return true;
			
			if (intDriver->curMusic->header->chanBus[ channel].ByPass) return true;
		}
	}	
	return false;
}

void ProcessVSTPlug(MADDriverRec *intDriver, long *data, long datasize, short channel)
{
	Boolean			gogo = false, Apply = false;
	long			i, x;
	float 			*in1	=	inputs[ 0];
    float 			*in2	=	inputs[ 1];
    
    float 			*out1	=	outputs[0];
    float 			*out2	=	outputs[1];
    
    float 			*out21	=	outputs2[0];
    float 			*out22	=	outputs2[1];
	
	if (ReadyToProcess == false) return;
	
	if (intDriver->curMusic != NULL)
	{
		if (intDriver->curMusic->header != NULL)
		{
		
		}
		else return;
	}
	else return;
	
	if (channel == -1)		// GLOBAL EFFECTS
	{
		if (intDriver->curMusic->header->globalFXActive == false) return;
		
		for (x = 0; x < 10; x++)
		{
			if (intDriver->masterVST[ x])
			{
				gogo = true;
			}
		}
	}
	else
	{
		if (intDriver->curMusic != NULL)
		{
			if (intDriver->curMusic->header != NULL)
			{
				if (intDriver->curMusic->header->MultiChan)
				{
					Channel *curVoice = &intDriver->chan[ channel];
					
					if (curVoice->curPtr >= curVoice->maxPtr && curVoice->loopSize == 0)
					{
						if (curVoice->TimeCounter > 0)
						{
							curVoice->TimeCounter --;
							channel = curVoice->TrackID;
						}
						else return;
					}
					else channel = curVoice->TrackID;
				}
			}
		}
		
		gogo = true;
		
		channel = intDriver->curMusic->header->chanBus[ channel].copyId;
	}
	
	if (gogo == false) return;
	
	if (datasize <= currentdatasize)
	{
		short	toTest, ChanToApply;
		
		for (i = 0; i < datasize; i++)
		{
			in1[ i] = data[ i*2] / 32767.;
			in2[ i] = data[ i*2 + 1] / 32767.;
		}
		
		if (channel == -1) toTest = 10;
		else toTest = 4;
		
		ChanToApply = 0;
		for (x = 0; x < toTest; x++)
		{
			VSTEffect	*vsteffect;
			
			if (channel == -1) vsteffect = intDriver->masterVST[ x];
			else vsteffect = intDriver->chanVST[ channel][ x];
			
			if (vsteffect)
			{
				if (vsteffect->Active)
				{
					ChanToApply++;
				}
			}
		}
		
		for (x = 0; x < toTest; x++)
		{
			VSTEffect	*vsteffect;
			
			if (channel == -1) vsteffect = intDriver->masterVST[ x];
			else vsteffect = intDriver->chanVST[ channel][ x];
			
			if (vsteffect)
			{
				if (vsteffect->Active)
				{
					AEffect*	ce[ 2];
					
				/*	if (vsteffect->ProcessReplacingNotAvailable)
					{
						for (i = 0; i < datasize; i++)
						{
							out1[ i] = 0;
							out2[ i] = 0;
							
							out21[ i] = 0;
							out22[ i] = 0;
						}
						
						ce[ 0] = vsteffect->ce[ 0];
						ce[ 1] = vsteffect->ce[ 1];
						
						if (ce[ 0]->numInputs == 2 && ce[ 0]->numOutputs == 2)
							ce[ 0]->process(ce[ 0], inputs, outputs, datasize);
						
						if (ce[ 0]->numInputs == 1 && ce[ 0]->numOutputs == 1)
						{
							ce[ 0]->process(ce[ 0], inputs, outputs, datasize);
							ce[ 0]->process(ce[ 1], inputs + 1, outputs + 1, datasize);
						}
						
						if (ce[ 0]->numInputs == 1 && ce[ 0]->numOutputs == 2)
						{
							ce[ 0]->process(ce[ 0], inputs, outputs, datasize);
							for (i = 0; i < datasize; i++) out1[ i] = (out1[ i] + out2[ i]) / 2.0;
							
							ce[ 1]->process(ce[ 1], &inputs[ 1], outputs2, datasize);
							for (i = 0; i < datasize; i++) out2[ i] = (out21[ i] + out22[ i]) / 2.0;
						}
					}
					else*/
					{
						ce[ 0] = vsteffect->ce[ 0];
						ce[ 1] = vsteffect->ce[ 1];
						
						if (ce[ 0]->numInputs == 2 && ce[ 0]->numOutputs == 2)
							ce[ 0]->processReplacing(ce[ 0], inputs, outputs, datasize);
						
						if (ce[ 0]->numInputs == 1 && ce[ 0]->numOutputs == 1)
						{
							ce[ 0]->processReplacing(ce[ 0], inputs, outputs, datasize);
							ce[ 0]->processReplacing(ce[ 1], inputs + 1, outputs + 1, datasize);
						}
						
						if (ce[ 0]->numInputs == 1 && ce[ 0]->numOutputs == 2)
						{
							for (i = 0; i < datasize; i++)
							{
								out1[ i] = 0;
								out2[ i] = 0;
								
								out21[ i] = 0;
								out22[ i] = 0;
							}
							
							ce[ 0]->processReplacing(ce[ 0], inputs, outputs, datasize);
							for (i = 0; i < datasize; i++) out1[ i] = (out1[ i] + out2[ i]) / 2.0;
							
							ce[ 1]->processReplacing(ce[ 1], &inputs[ 1], outputs2, datasize);
							for (i = 0; i < datasize; i++) out2[ i] = (out21[ i] + out22[ i]) / 2.0;
						}
					}
					
					if (ChanToApply > 1) {
						BlockMoveData(out1, in1, sizeof(float) * datasize);
						BlockMoveData(out2, in2, sizeof(float) * datasize);
					}
					
					Apply = true;
				}
			}
		}
		
		if (Apply == true)
		{
			for (i = 0; i < datasize; i++)
			{
				data[ i*2]		= out1[ i] * 32767.;
				data[ i*2 + 1]	= out2[ i] * 32767.;
			}
		}
	}
}

void CloseVST(short no, VSTEffect *myEffect)
{
	OSErr	myErr;

	if (myEffect->connID == NULL) Debugger();
	myErr = CloseConnection(&myEffect->connID);
	myEffect->connID = NULL;
	
	if (myEffect->vstMain == NULL) Debugger();
	//DisposePtr((Ptr) myEffect->vstMain);
	myEffect->vstMain = NULL;
	
	if (myErr) Debugger();
}

void CloseVSTPlug(void)
{
	long 	i, x, inOutBytes;
	OSErr	iErr;
	FSSpec	spec;
	short	fRefNum;
	
	iErr = FindFolder(kOnSystemDisk, kPreferencesFolderType, kCreateFolder, &spec.vRefNum, &spec.parID);
	pStrcpy(spec.name, VSTPREFNAME);
	
	iErr = FSpDelete(&spec);
	
	iErr = FSpCreate(&spec, 'SNPL', 'PREF',smSystemScript);
	
	iErr = FSpOpenDF(&spec, fsCurPerm, &fRefNum);
	if (iErr == noErr) {
		for (i = 0; i < MAXVSTPREF; i++) {
			if (VSTPref[i] != NULL) {
				inOutBytes = sizeof(VSTPrefsStruct);
			
				iErr = FSWriteFork(fRefNum, fsAtMark, 0, inOutBytes, VSTPref[i], NULL);
			}
		}
		
		FSCloseFork(fRefNum);
	}
	
	// Save current state ***  ***  ***  ***  ***  ***  ***  *** 
/*	
	pStrcpy(spec.name, VSTPREFSTATE);
	
	iErr = FSpDelete(&spec);
	
	iErr = FSpCreate(&spec, 'SNPL', 'PREF',smSystemScript);
	
	iErr = FSpOpenDF(&spec, fsCurPerm, &fRefNum);
	if (iErr == noErr)
	{
		VSTPrefsStruct	tempPref;
		
		for (i = 0; i < 10; i++)
		{
			if (MADDriver->masterVST[ i] != 0)
			{
				VSTEffect	*ce = MADDriver->masterVST[ i];	
				
				inOutBytes = sizeof(VSTPrefsStruct);
				
				tempPref.ID = ce->ce[ 0]->uniqueID;
				
				if (ce->ce[ 0]->numParams >= MAXVSTITEM) MyDebugStr(__LINE__, __FILE__, "Too many VSTITEM, press continue.");
				
				for (x = 0; x < ce->ce[ 0]->numParams && x < MAXVSTITEM; x++)
				{
					tempPref.value[ x] = ce->ce[ 0]->getParameter(ce->ce[ 0], x);
				}
				
				iErr = FSWrite(fRefNum, &inOutBytes, &tempPref);
			}
			else
			{
				inOutBytes = sizeof(VSTPrefsStruct);
				
				tempPref.ID = 0;	// EMPTY EFFECT !!!!
				
				iErr = FSWrite(fRefNum, &inOutBytes, &tempPref);
			}
		}
		
		FSClose(fRefNum);
	}*/
	
	// ***  ***  ***  ***  ***  ***  ***  ***  ***  ***  *** 
	
//	for (i = 0; i < tPlug; i++) CloseVST(i);
	
	DisposePtr((Ptr) inputs[ 0]);
	DisposePtr((Ptr) inputs[ 1]);
	
	DisposePtr((Ptr) outputs[ 0]);
	DisposePtr((Ptr) outputs[ 1]);
	
	DisposePtr((Ptr) outputs2[ 0]);
	DisposePtr((Ptr) outputs2[ 1]);
}

void InitVST(short no, VSTEffect* myEffect)
{
	OSErr				myErr;
	Str255				errName;
	GrafPtr				savedPort;
	short				fileID;

	audioMaster = audioMasterFct;
	
	fileID = FSpOpenResFile(&VSTPlug[ no].file, fsCurPerm);
	UseResFile(fileID);
	myErr = GetMemFragment(	VSTPlug[ no].call,
								GetPtrSize(VSTPlug[ no].call),
								VSTPlug[ no].file.name,
                                kLoadCFrag,
                                &myEffect->connID,
                                (Ptr *) &myEffect->vstMain,
                                errName);
	
	if (myErr) DebugStr("\pErr in InitVST");
	
	CloseResFile(fileID);
}

Boolean LoadVSTPLUG(short No, StringPtr theName)
{
	Handle		theRes;
	short		fileID, i, temp;
	Str255		tStr;
	char		aStr[ 256];

	/***********************/
	
	HGetVol(NULL, &VSTPlug[ No].file.vRefNum, &VSTPlug[ No].file.parID);
	pStrcpy(VSTPlug[ No].file.name, theName);
	
	{
		Boolean		targetIsFolder, wasAliased;
		
		ResolveAliasFile(&VSTPlug[ No].file, true, &targetIsFolder, &wasAliased);
	}
	
	fileID = FSpOpenResFile(&VSTPlug[ No].file, fsCurPerm);
	
	theRes = Get1Resource('aEff', 0);
	if (theRes == NULL) {
		theRes = GetIndResource('aEff', 1);
		if (theRes == NULL) return false;
	}
	DetachResource(theRes);
	HNoPurge(theRes);
	HLock(theRes);
	
	VSTPlug[ No].call = NewPtr(GetHandleSize(theRes));
	BlockMoveData(*theRes, VSTPlug[ No].call, GetHandleSize(theRes));
	HUnlock(theRes);
	DisposeHandle(theRes);
	CloseResFile(fileID);
	
	/*************************/
	
//	InitVST(No);
	
	return true;
}

void ScanDirVSTPlug(long dirID, short VRefNum)
{
	CInfoPBRec		info;
	Str255			tempStr, volName;
	long			dirIDCopy;
	short			i, vRefNum;
	OSErr			iErr;

	info.hFileInfo.ioNamePtr = tempStr;
	info.hFileInfo.ioVRefNum = VRefNum;
	
	for (i = 1; true; i ++)
	{
		info.hFileInfo.ioDirID = dirID;
		info.hFileInfo.ioFDirIndex = i;
		
		if (PBGetCatInfoSync(&info) != noErr) break;
		
		if (info.hFileInfo.ioFlFndrInfo.fdType == 'aPcs')
		{	
			HGetVol(NULL, &vRefNum, &dirIDCopy);
			
			iErr = HSetVol(NULL, info.hFileInfo.ioVRefNum, dirID);
			
			if (tPlug > MAXVST) MyDebugStr(__LINE__, __FILE__, "Too many plugs");
			
			LoadVSTPLUG(tPlug, info.hFileInfo.ioNamePtr);
			
			tPlug++;
			
			iErr = HSetVol(NULL, vRefNum, dirIDCopy);
			if (iErr != noErr) MyDebugStr(__LINE__, __FILE__, "HSetVol error...");
		}
		else if((info.hFileInfo.ioFlAttrib & 16)) {
			if (EqualString(info.hFileInfo.ioNamePtr, "\pPlugs", false, false) || PlugsFolderOK > 0) {
				if ((EqualString(info.hFileInfo.ioNamePtr, "\pMacOS X", false, false) != true)) {
					PlugsFolderOK++;
					ScanDirVSTPlug(info.dirInfo.ioDrDirID, VRefNum);
					PlugsFolderOK--;
				}
			}
		}
	}
}


void InitVSTPlug(void)
{
	short				FXCounter, vRefNum, fRefNum;
	long				i, x, dirID, inOutBytes;
	OSErr				iErr;
	FSSpec				spec;
	VSTPrefsStruct		tempStruct;
	VSTEffect			*myEffect;
	
	VSTDlog = NULL;
	
	currentdatasize = VSTBUFFERSIZE;
	
	inputs[ 0] 	= (float*) NewPtrClear(sizeof(float) * currentdatasize);
	inputs[ 1] 	= (float*) NewPtrClear(sizeof(float) * currentdatasize);
	
	outputs[ 0]	= (float*) NewPtrClear(sizeof(float) * currentdatasize);
	outputs[ 1]	= (float*) NewPtrClear(sizeof(float) * currentdatasize);
	
	outputs2[ 0]	= (float*) NewPtrClear(sizeof(float) * currentdatasize);
	outputs2[ 1]	= (float*) NewPtrClear(sizeof(float) * currentdatasize);
	
	VSTPlug = (VSTInfo*) MyNewPtr(MAXVST * sizeof(VSTInfo));
	
	HGetVol(NULL, &vRefNum, &dirID);
	
	for (i = 0; i < 10; i++) MADDriver->masterVST[ i] = NULL;
	for (i = 0; i < MAXTRACK; i++)
	{
		for (x = 0; x < 4; x++) MADDriver->chanVST[ i][ x] = NULL;
	}
	for (i = 0; i < MAXVSTPREF; i++) VSTPref[ i] = NULL;
	
	tPlug		= 0;
	PlugsFolderOK = 0;
	
	GetApplicationPackageFSSpecFromBundle(&spec);
	ScanDirVSTPlug(spec.parID, spec.vRefNum);
	
//	if (MacOSXSystem) tPlug = 0;
	
	// Load the Unique ID
	for (i = 0; i < tPlug; i++)
	{
		myEffect = CreateVSTEffect(i);
		
		VSTPlug[ i].ID = myEffect->ce[ 0]->uniqueID;
		
		DisposeVSTEffect(myEffect);
	}
	
	// TEST TEST TEST TEST TEST
	
/*	for (i = 0; i < tPlug; i++)
	{
		Rect	*tRect;
		
		myEffect = CreateVSTEffect(i);
		
	//	dispatcher(myEffect, true, effOpen, 0, 0, 0, 0);					// <- Présent???
		dispatcher(myEffect, true, effIdentify, 0, 0, 0, 0);
	//	dispatcher(myEffect, true, effSetSampleRate, 0, 0, 0, 44100);
	//	dispatcher(myEffect, true, effSetBlockSize, 0, 4096, 0, 0);
		dispatcher(myEffect, true, effMainsChanged, 0, 1, 0, 0);

		dispatcher(myEffect, true, effEditGetRect, 0, 0, &tRect, 0);
		if (tRect)
		{
			dispatcher(myEffect, true, effEditOpen, 0, 0, VSTDlog, 0);
			dispatcher(myEffect, true, effEditDraw, 0, 0, &tRect, 0);
			dispatcher(myEffect, true, effEditIdle, 0, 0, 0, 0);
			dispatcher(myEffect, true, effEditClose, 0, 0, 0, 0);
		}
	//	dispatcher(myEffect, true, effClose, 0, 0, 0, 0);
		
		DisposeVSTEffect(myEffect);
	}

	Debugger();*/

	
	// Load VST Pref File *** *** *** *** *** *** *** *** *** *** *** *** *** *** *** ***
	
	iErr = FindFolder(kOnSystemDisk, kPreferencesFolderType, kCreateFolder, &spec.vRefNum, &spec.parID);
	pStrcpy(spec.name, VSTPREFNAME);
	
	iErr = FSpOpenDF(&spec, fsCurPerm, &fRefNum);
	if (iErr == noErr)
	{
		i = 0;
		do
		{
			inOutBytes = sizeof(VSTPrefsStruct);
			
			iErr = FSRead(fRefNum, &inOutBytes, &tempStruct);
			if (iErr == noErr)
			{
				VSTPref[ i] = (VSTPrefsStruct*) NewPtrClear(inOutBytes);
				
				*VSTPref[ i] = tempStruct;
				
				i++;
			}
		}
		while (iErr == noErr);
		
		FSCloseFork(fRefNum);
	}
	
	// *** *** *** *** *** *** *** *** *** *** *** *** *** *** *** *** *** *** *** *** *** ***
	
	HSetVol(NULL, vRefNum, dirID);
	
	if (tPlug > 0)
	{
		for (i = 0; i < tPlug; i++)	AppendMenu(VSTMenu, VSTPlug[ i].file.name);
		
		AppendMenu(SampleMenu, "\p-");
		
		for (i = 0; i < tPlug; i++)	AppendMenu(SampleMenu, VSTPlug[ i].file.name);
	}
	else
	{
		AppendMenu(VSTMenu, "\p(No VST Plugs available");
	}
	
	
	ReadyToProcess = true;
	
	// Load VST State File *** *** *** *** *** *** *** *** *** *** *** *** *** *** *** ***
	
/*	pStrcpy(spec.name, VSTPREFSTATE);
	
	FXCounter = 0;
	
	iErr = FSpOpenDF(&spec, fsCurPerm, &fRefNum);
	if (iErr == noErr)
	{
		do
		{
			inOutBytes = sizeof(VSTPrefsStruct);
			
			iErr = FSRead(fRefNum, &inOutBytes, &tempStruct);
			if (iErr == noErr)
			{
				if (FXCounter >= 10) Debugger();
				
				for (i = 0; i < tPlug; i++)
				{
					if (tempStruct.ID == VSTPlug[ i].ID)
					{
						MADDriver->masterVST[ FXCounter] = CreateVSTEffect(i);
						
						for (x = 0; x < MADDriver->masterVST[ FXCounter]->ce[ 0]->numParams; x++)
						{
							MADDriver->masterVST[ FXCounter]->ce[ 0]->setParameter(MADDriver->masterVST[ FXCounter]->ce[ 0], x, tempStruct.value[ x]);
							if (MADDriver->masterVST[ FXCounter]->ce[ 1]) MADDriver->masterVST[ FXCounter]->ce[ 1]->setParameter(MADDriver->masterVST[ FXCounter]->ce[ 1], x, tempStruct.value[ x]);
						}
					}
				}
			}
			FXCounter++;
		}
		while (iErr == noErr);
		
		FSClose(fRefNum);
	}*/
}

short ConvertUniqueIDToIndex(long uniqueID)
{
	long i;
	
	for (i = 0; i < tPlug; i++)
	{
		if (uniqueID == VSTPlug[ i].ID)
		{
			return i;
		}
	}
	
	return -1;
}

static	ControlHandle	VSTCntl[ 100], DescCntl[ 100];
static	short			CurrentDialogItem;
static	VSTEffect		*CurrentDialogCE;
static	sData			*CurrentcurData, CurrentSData;
static	long 			CurrentStart, CurrentEnd, CurrentStereoMode;
static	Rect			*CurrentVSTRect;
static	Handle			theDITL;

void VSTFilter(DialogPtr theDialog, EventRecord *theEventI, short *itemHit)
{
/*		WindowPtr		whichWindow;
		short			thePart,i, theChar;
		GrafPtr			oldPort;
		Point			aPoint, theCell;
 		Str255			str1, str2;
 		Boolean			DrawAll;
		VstEvents		VstEvt;
		VstMidiEvent	VstEv;

		GetPort(&oldPort);
		SetPortDialogPort(theDialog);
		
		if (theEventI->what == keyDown || theEventI->what == keyDown)
		{
			theChar = theEventI->message & charCodeMask;
			i = ConvertCharToNote(theChar);
			if (i != -1)
			{
				VstEvt.numEvents = 1;
				VstEvt.reserved = 0;
				VstEvt.events[ 0] = (VstEvent*) &VstEv;
				
				VstEv.type = kVstMidiType;
				VstEv.byteSize = 0;
				VstEv.deltaFrames = 0;
				VstEv.flags = 0;
				VstEv.noteLength = 0;
				VstEv.noteOffset = 0;
				VstEv.midiData[ 0] = 0x90;
				VstEv.midiData[ 1] = i + 12;
				VstEv.midiData[ 2] = 127;
				VstEv.detune = 0;
				VstEv.noteOffVelocity = 0;
				VstEv.reserved1 = 0;
				VstEv.reserved2 = 0;
				
				dispatcher(CurrentDialogCE, true, effProcessEvents, 0, 0, &VstEvt, 0);
			}
		}
		SetPort(oldPort);
		
		return;*/
}

void VSTSampleEditor(short item, sData	*curData, long Start, long End, Boolean StereoMode)
{
	VSTEffect	*tempEffect;
	short		itemHit;
	
	tempEffect = CreateVSTEffect(item);
	
	if (tempEffect)
	{
		//VSTEditor(tempEffect, curData, Start, End, StereoMode);
		
		VSTEditorOpen(tempEffect, curData, Start, End, StereoMode, -2);
		
		do
		{
			ModalDialog(MyDlgFilterDesc, &itemHit);
			
			VSTEditorDoItemPress(itemHit, VSTDlog);
		
		}while (itemHit != 11 && itemHit != 14);
		
		VSTEditorClose(VSTDlog, itemHit);
	}
		
	DisposeVSTEffect(tempEffect);
}

void CheckVSTEditor(VSTEffect *ce)
{
	if (ce == NULL) VSTEditorClose(VSTDlog, 12);
	else
	{
		if (ce == CurrentDialogCE) VSTEditorClose(VSTDlog, 0);
	}
}

Boolean VSTEditorOpen(VSTEffect *ce, sData	*curData, long Start, long End, Boolean StereoMode, short channelID)
{
	short					id, i, itemHit, itemType, x;
	Str255					theString, theString2;
	GrafPtr					savedPort;
	OSErr					err;
	Rect					itemRect, itemRect2, itemRect3, caRect;
	Handle					itemHandle;
	//short					orgX = 0, orgY = 0;
	short					theChar;
	ControlHandle			ctl;
	ControlFontStyleRec		style;
	Point					mouse;

	
	if (VSTDlog != NULL)
	{
		VSTEditorClose(VSTDlog, 11);
	}
	
	GetPort(&savedPort);
	
	CurrentDialogItem = ce->id;
	CurrentDialogCE = ce;
	CurrentcurData = curData;
	CurrentStart = Start;
	CurrentEnd = End;
	CurrentStereoMode = StereoMode;
	
	if (channelID == -2) VSTDlog = GetNewDialog(189, NULL, (WindowPtr) -1);
	else VSTDlog = GetNewDialog(186, NULL, GetDialogWindow(ToolsDlog));
	
	SetPortDialogPort(VSTDlog);
	
	pStrcpy(theString, VSTPlug[ ce->id].file.name);
	pStrcat(theString, "\p / ");
	
	if (channelID == -2) pStrcat(theString, "\pSample Effect");
	else if (channelID == -1) pStrcat(theString, "\pGlobal Effect");
	else
	{
		pStrcat(theString, "\pTrack #");
		NumToString(channelID + 1, theString2);
		pStrcat(theString, theString2);
	}
	
	SetWTitle(GetDialogWindow(VSTDlog), theString);
	
	SetCursor(GetQDGlobalsArrow(&qdarrow));
	
	CurrentVSTRect = NULL;
	dispatcher(ce, false, effEditGetRect, 0, 0, &CurrentVSTRect, 0);
	if (CurrentVSTRect == NULL)
	{
		GetDialogItem(VSTDlog, 5, &itemType, &itemHandle, &itemRect);
		GetDialogItem(VSTDlog, 6, &itemType, &itemHandle, &itemRect2);
		GetDialogItem(VSTDlog, 7, &itemType, &itemHandle, &itemRect3);
		
		for (i = 0; i < ce->ce[ 0]->numParams; i++)
		{
			VSTCntl[ i] = NewControl(GetDialogWindow(VSTDlog), &itemRect, "\p", true, ce->ce[ 0]->getParameter(ce->ce[ 0], i) * 100, 0, 100, 80, i);
			
			dispatcher(ce, false, effGetParamName, i, 0, (Ptr) theString, 0);
			MyC2PStr((Ptr) theString);
			pStrcat(theString, "\p : ");
			
			ctl = NewControl(GetDialogWindow(VSTDlog), &itemRect2, theString, true, 0, 0, 0, kControlStaticTextProc, i);
			SetControlData(ctl, kControlNoPart, kControlStaticTextTextTag, *theString,(Ptr) theString + 1);
			style.flags = kControlUseJustMask;
			style.just = teFlushRight;
			SetControlData(ctl, kControlNoPart, kControlStaticTextStyleTag, sizeof(style),(Ptr) &style);
			
			dispatcher(ce, false, effGetParamDisplay, i, 0, (Ptr) theString, 0);
			MyC2PStr((Ptr) theString);
			pStrcat(theString, "\p ");
			
			dispatcher(ce, false, effGetParamLabel, i, 0, (Ptr) theString2, 0);
			MyC2PStr((Ptr) theString2);
			pStrcat(theString, theString2);
			
			DescCntl[ i] = NewControl(GetDialogWindow(VSTDlog), &itemRect3, theString, true, 0, 0, 0, kControlStaticTextProc, i);
			SetControlData(DescCntl[ i], kControlNoPart, kControlStaticTextTextTag, *theString,(Ptr) theString + 1);
			
			itemRect.top += 20;
			itemRect.bottom += 20;
			
			itemRect2.top += 20;
			itemRect2.bottom += 20;
			
			itemRect3.top += 20;
			itemRect3.bottom += 20;
		}
		
		GetPortBounds(GetDialogPort(VSTDlog), &caRect);
		
		itemRect.bottom -= 20;
		
		SizeWindow(GetDialogWindow(VSTDlog), caRect.right, itemRect.bottom, true);
	}
	else
	{
		dispatcher(ce, false, effEditOpen, 0, 0, GetDialogWindow(VSTDlog), 0);
		
		GetDialogItem(VSTDlog, 4, &itemType, &itemHandle, &itemRect);
		dispatcher(ce, false, effEditGetRect, 0, 0, &CurrentVSTRect, 0);
		SizeWindow(GetDialogWindow(VSTDlog), CurrentVSTRect->right - CurrentVSTRect->left, CurrentVSTRect->bottom - CurrentVSTRect->top + itemRect.top, true);
		
		dispatcher(ce, false, effEditDraw, 0, 0, CurrentVSTRect, 0);
	}
	
	GetPortBounds(GetDialogPort(VSTDlog), &caRect);
	
	GetDialogItem(VSTDlog, 4, &itemType, &itemHandle, &itemRect);
	itemRect.bottom = caRect.bottom;
	SetDialogItem(VSTDlog, 4, itemType, itemHandle, &itemRect);
	
	if (channelID == -2) theDITL = GetResource('DITL', 188);
	else theDITL = GetResource('DITL', 190);
	
	AppendDITL(VSTDlog, theDITL, appendDITLBottom);
	
	SetDialogDefaultItem(VSTDlog, 11);
	
	AutoPosition(VSTDlog);
	
	if (channelID != -2) SelectWindow2(GetDialogWindow(VSTDlog));
	
	if (curData == NULL)
	{
		RGBBackColor(&theColor);
		HideDialogItem(VSTDlog, 12);
	}
	
	if (curData)
	{
		CurrentSData = *curData;
		CurrentSData.data = NewPtr(curData->size);
		if (CurrentSData.data == NULL) return false;
	
		BlockMoveData(curData->data, CurrentSData.data, curData->size);
		ApplyVSTFilter(ce, curData, Start, End);
	}
	
	if (CurrentVSTRect != NULL)
	{
		dispatcher(ce, false, effEditDraw, 0, 0, CurrentVSTRect, 0);
	}
	
	return true;
}

void VSTEditorUpdate(DialogPtr aDia)
{
	GrafPtr	SavePort;
	
	GetPort(&SavePort);
 	SetPortDialogPort(aDia);
	
	BeginUpdate(GetDialogWindow(aDia));
	
	DrawDialog(aDia);
	
	if (CurrentVSTRect != NULL)
	{
		dispatcher(CurrentDialogCE, false, effEditDraw, 0, 0, CurrentVSTRect, 0);
	}
	
	EndUpdate(GetDialogWindow(aDia));
	
	SetPort(SavePort);
}

void VSTEditorDoNull(void)
{
	GrafPtr	SavePort;
	
	if (VSTDlog == NULL) return;
	
	GetPort(&SavePort);
 	SetPortDialogPort(VSTDlog);
	
	dispatcher(CurrentDialogCE, false, effEditIdle, 0, 0, 0, 0);
	
	SetPort(SavePort);
}

void VSTEditorDoItemPress(short itemHit, DialogPtr aDia)
{
	GrafPtr				savePort;
	Point				mouse, myPt;
	short				ctlPart, itemType;
	ControlHandle		theControl;
	Rect				itemRect, itemRect2;
	Handle				itemHandle;
	short				id;
	Str255				theString, theString2;
	long				i, x;
	
	GetPort(&savePort);
	SetPortDialogPort(aDia);
	
	GetDialogItem(aDia, 5, &itemType, &itemHandle, &itemRect);
	
	if (theEvent.what == mouseDown) /* See if a mouse click */
	{
		myPt = theEvent.where;
		GlobalToLocal(&myPt);
		
		ctlPart = FindControl(myPt, GetDialogWindow(aDia), &theControl);
	}

	switch(itemHit)
	{
		/*	case 3:
				CurrentDialogCE->Active = !CurrentDialogCE->Active;
				
				dispatcher(CurrentDialogCE, true, effMainsChanged, 0, CurrentDialogCE->Active, 0, 0);
				
				TurnRadio(3, aDia, CurrentDialogCE->Active);
			break;*/
			
		case 12:
			if (CurrentcurData)
			{
				BlockMoveData(CurrentSData.data, CurrentcurData->data, CurrentcurData->size);
					
				ApplyVSTFilter(CurrentDialogCE, CurrentcurData, CurrentStart, CurrentEnd);
					
				MADPlaySoundDataSYNC(MADDriver, CurrentcurData->data, CurrentSData.size, 0, 48 + CurrentSData.relNote, CurrentSData.amp, CurrentSData.loopBeg, CurrentSData.loopSize, ((long)CurrentSData.c2spd) << 16L, CurrentSData.stereo);
			}
			break;
			
		case 13:	// Preset button!
			GetDialogItem(aDia, itemHit, &itemType, &itemHandle, &itemRect2);
			mouse.h = itemRect2.left;
			mouse.v = itemRect2.top;

			id = PressPresetButton(CurrentDialogCE->ce[ 0]->uniqueID, mouse, &CurrentcurSelec);
				
			if (id == -1)
			{
				long flag;
				
				theString[ 0] = 0;
				flag = 0;
				GetVSTPrefName(theString, &flag);
				if (theString[ 0] > 0)
				{
					if (flag)	// Clear all settings flags
					{
						for (i = 0; i < MAXVSTPREF; i++)
						{
							if (VSTPref[ i] != NULL)
							{
								if (VSTPref[ i]->ID == CurrentDialogCE->ce[ 0]->uniqueID)
								{
									VSTPref[ i]->flag = 0;
								}
							}
						}
					}
						
					for (i = 0; i < MAXVSTPREF; i++)
					{
						if (VSTPref[ i] == NULL)
						{
							VSTPref[ i] = (VSTPrefsStruct*) NewPtrClear(sizeof(VSTPrefsStruct));
							
							VSTPref[ i]->ID = CurrentDialogCE->ce[ 0]->uniqueID;
							VSTPref[ i]->flag = flag;
							
							pStrcpy(VSTPref[ i]->name, theString);
							
							if (CurrentDialogCE->ce[ 0]->numParams >= MAXVSTITEM) MyDebugStr(__LINE__, __FILE__, "MAXVST TOO SMALL, Press continue.");
								
							for (x = 0; x < CurrentDialogCE->ce[ 0]->numParams && x < MAXVSTITEM; x++)
							{
								VSTPref[ i]->value[ x] = CurrentDialogCE->ce[ 0]->getParameter(CurrentDialogCE->ce[ 0], x);
							}
							
							i = MAXVSTPREF;	// break
						}
					}
				}
			}
			
			if (id >= 0)
			{
				GetKeys(km);
				if (IsPressed(0x0037))		// Replace this pref
				{
					long flag;
					
					pStrcpy(theString, VSTPref[ id]->name);
					flag = VSTPref[ id]->flag;
					GetVSTPrefName(theString, &flag);
					
					if (theString[ 0] > 0)
					{
						if (flag)	// Clear all settings flags
						{
							for (i = 0; i < MAXVSTPREF; i++)
							{
								if (VSTPref[ i] != NULL)
								{
									if (VSTPref[ i]->ID == CurrentDialogCE->ce[ 0]->uniqueID)
									{
										VSTPref[ i]->flag = 0;
									}
								}
							}
						}
						
						pStrcpy(VSTPref[ id]->name, theString);
						VSTPref[ id]->flag = flag;
						
						for (x = 0; x < CurrentDialogCE->ce[ 0]->numParams; x++)
						{
							VSTPref[ id]->value[ x] = CurrentDialogCE->ce[ 0]->getParameter(CurrentDialogCE->ce[ 0], x);
						}
					}
				}
				else
				{
					for (x = 0; x < CurrentDialogCE->ce[ 0]->numParams; x++)
					{
						CurrentDialogCE->ce[ 0]->setParameter(CurrentDialogCE->ce[ 0], x, VSTPref[ id]->value[ x]);
						if (CurrentDialogCE->ce[ 1]) CurrentDialogCE->ce[ 1]->setParameter(CurrentDialogCE->ce[ 1], x, VSTPref[ id]->value[ x]);
						
						if (CurrentVSTRect == NULL)
						{
							SetControlValue(VSTCntl[ x], CurrentDialogCE->ce[ 0]->getParameter(CurrentDialogCE->ce[ 0], GetControlReference(VSTCntl[ x])) * 100);
									
							dispatcher(CurrentDialogCE, false, effGetParamDisplay, x, 0, (Ptr) theString, 0);
							MyC2PStr((Ptr) theString);
							pStrcat(theString, "\p ");
						
							dispatcher(CurrentDialogCE, false, effGetParamLabel, x, 0, (Ptr) theString2, 0);
							MyC2PStr((Ptr) theString2);
							pStrcat(theString, theString2);
						
							SetControlData(DescCntl[ x], kControlNoPart, kControlStaticTextTextTag, *theString,(Ptr) theString + 1);
							Draw1Control(DescCntl[ x]);
						}
						
						if (CurrentcurData)
						{
							BlockMoveData(CurrentSData.data, CurrentcurData->data, CurrentcurData->size);
							ApplyVSTFilter(CurrentDialogCE, CurrentcurData, CurrentStart, CurrentEnd);
						}
					}
				}
			}
		break;
	}
	
	if (CurrentVSTRect == NULL)		// NO EDITOR !!!!
	{
		switch(itemHit)
		{
			case 4:
			{
				Point				myPt;
				ControlHandle		theControl;
				short				ctlPart, bogus, ctlID;
				long				oldH = 0;
				ControlActionUPP	MyControlUPP;
				
				GetMouse(&myPt);
				
				theControl = NULL;
		
				for (i = 0; i < CurrentDialogCE->ce[ 0]->numParams; i++)
				{
					Rect contrlRect;
					
					GetControlBounds(VSTCntl[ i], &contrlRect);
					
					if (PtInRect(myPt, &contrlRect))
					{
						theControl = VSTCntl[ i];
						ctlID = i;
					}
				}
				
				if (theControl)
				{
					while (Button())
					{
						DoGlobalNull();
						
						GetMouse(&myPt);
						
						if (oldH != myPt.h)
						{
							oldH = myPt.h;
							
							if (myPt.h < itemRect.left) 		myPt.h = itemRect.left;
							else if (myPt.h > itemRect.right)	myPt.h = itemRect.right;
							
							CurrentDialogCE->ce[ 0]->setParameter(CurrentDialogCE->ce[ 0], GetControlReference(theControl), (float) (myPt.h - itemRect.left) / (float) (itemRect.right - itemRect.left));
							if (CurrentDialogCE->ce[ 1]) CurrentDialogCE->ce[ 1]->setParameter(CurrentDialogCE->ce[ 1], GetControlReference(theControl), (float) (myPt.h - itemRect.left) / (float) (itemRect.right - itemRect.left));
							
							SetControlValue(theControl, CurrentDialogCE->ce[ 0]->getParameter(CurrentDialogCE->ce[ 0], GetControlReference(theControl)) * 100);
							
							dispatcher(CurrentDialogCE, false, effGetParamDisplay, ctlID, 0, (Ptr) theString, 0);
							MyC2PStr((Ptr) theString);
							pStrcat(theString, "\p ");
							
							dispatcher(CurrentDialogCE, false, effGetParamLabel, ctlID, 0, (Ptr) theString2, 0);
							MyC2PStr((Ptr) theString2);
							pStrcat(theString, theString2);
							
							SetControlData(DescCntl[ ctlID], kControlNoPart, kControlStaticTextTextTag, *theString,(Ptr) theString + 1);
							Draw1Control(DescCntl[ ctlID]);
							
							CurrentcurSelec = -1;
							
							if (CurrentcurData)
							{
								BlockMoveData(CurrentSData.data, CurrentcurData->data, CurrentcurData->size);
								ApplyVSTFilter(CurrentDialogCE, CurrentcurData, CurrentStart, CurrentEnd);
							}
						}
					}
				}
			}
			break;
		}
	}
	else					// OWN EDITOR
	{			
		switch(itemHit)
		{
			case -5:
				dispatcher(CurrentDialogCE, false, effEditIdle, 0, 0, CurrentVSTRect, 0);
			break;
			
			case -updateEvt:
				dispatcher(CurrentDialogCE, false, effEditDraw, 0, 0, CurrentVSTRect, 0);
			break;
			
			case 4:
			{
				Point	myPt;
				
			//	SetOrigin(orgX, orgY);
				GetMouse(&myPt);
				dispatcher(CurrentDialogCE, false, effEditMouse, myPt.h, myPt.v, NULL, 0);
				
				if (CurrentDialogCE->ce[ 1])
				{
					for (i = 0; i < CurrentDialogCE->ce[ 0]->numParams; i++)
					{
						float myVal;
						
						myVal = CurrentDialogCE->ce[ 0]->getParameter(CurrentDialogCE->ce[ 0], i);
						CurrentDialogCE->ce[ 1]->setParameter(CurrentDialogCE->ce[ 1], i, myVal);
					}
				}
			//	SetOrigin(0, 0);
				
				CurrentcurSelec = -1;
				
				if (CurrentcurData)
				{
					BlockMoveData(CurrentSData.data, CurrentcurData->data, CurrentcurData->size);
					ApplyVSTFilter(CurrentDialogCE, CurrentcurData, CurrentStart, CurrentEnd);
				}
			}
			break;
		}
	}
}

Boolean VSTEditorClose(DialogPtr aDia, short itemHit)
{
	if (VSTDlog == NULL) return false;
	
	if (CurrentVSTRect != NULL)
	{
		dispatcher(CurrentDialogCE, false, effEditClose, 0, 0, NULL, 0);
	}
	
	if (CurrentcurData)
	{
		BlockMoveData(CurrentSData.data, CurrentcurData->data, CurrentcurData->size);
		if (itemHit == 11) ApplyVSTFilter(CurrentDialogCE, CurrentcurData, CurrentStart, CurrentEnd);
		
		DisposePtr(CurrentSData.data);
		CurrentSData.data = NULL;
	}
	
	if (itemHit == 11) FillVSTEffects();
	
	DisposeDialog(aDia);
	//HideWindow(aDia);

	UpdateALLWindow();
	
	ReleaseResource(theDITL);
	
	VSTDlog = NULL;
	
	CurrentDialogCE = NULL;
	
	if (itemHit == 11) return true;
	else return false;
}

void DisposeVSTEffect(VSTEffect	*myEffect)
{
//	dispatcher(myEffect, true, effMainsChanged, 0, false, 0, 0);
	
	myEffect->Active = false;
	
	dispatcher(myEffect, true, effClose, 0, 0, 0, 0);
	
	CloseVST(myEffect->id, myEffect);
	
	DisposePtr((Ptr) myEffect);
}

/*long GetVSTEffectUniqueID(short effectID)
{
	short		fileID;
	long		i, x, returnID;
	Rect		*tRect;
	VSTEffect	*myEffect;
	
	myEffect = (VSTEffect*) NewPtrClear(sizeof(VSTEffect));
	
	myEffect->id = effectID;
	
	InitVST(effectID, myEffect);
	
	fileID = FSpOpenResFile(&VSTPlug[ effectID].file, fsCurPerm);
	UseResFile(fileID);
	
	myEffect->ce[ 0] = myEffect->vstMain (audioMaster);
	if (myEffect->ce[ 0] && myEffect->ce[ 0]->magic == kEffectMagic)
	{
	}
	else Debugger();
	
	CloseResFile(fileID);
	
//	dispatcher(myEffect, true, effOpen, 0, 0, 0, 0);
	
	returnID = myEffect->ce[ 0]->uniqueID;
	
	dispatcher(myEffect, true, effClose, 0, 0, 0, 0);
	
	CloseVST(myEffect->id, myEffect);
	
	DisposePtr((Ptr) myEffect);
	myEffect = NULL;
	
	return returnID;
}*/

VSTEffect* CreateVSTEffect(short effectID)
{
	VSTEffect	*myEffect;
	short		fileID, counter;
	long		i, x;
	Rect		*tRect;
		
	myEffect = (VSTEffect*) NewPtrClear(sizeof(VSTEffect));
	
	pStrcpy(myEffect->name, VSTPlug[ effectID].file.name);
	
	InitVST(effectID, myEffect);
	
	fileID = FSpOpenResFile(&VSTPlug[ effectID].file, fsCurPerm);
	UseResFile(fileID);
	
	myEffect->id = effectID;
	
	myEffect->ce[ 0] = myEffect->vstMain (audioMaster);
	if (myEffect->ce[ 0] && myEffect->ce[ 0]->magic == kEffectMagic)
	{
	}
	else Debugger();
	
	if (myEffect->ce[ 0]->numInputs == 2 && myEffect->ce[ 0]->numOutputs == 2)
	{
		myEffect->ce[ 1] = NULL;
	}
	else
	{
		myEffect->ce[ 1] = myEffect->vstMain (audioMaster);
		
		if (myEffect->ce[ 1] && myEffect->ce[ 1]->magic == kEffectMagic)
		{
		}
		else Debugger();
	}
	CloseResFile(fileID);
	
	dispatcher(myEffect, true, effOpen, 0, 0, 0, 0);
	
//	dispatcher(myEffect, true, effSetSampleRate, 0, 0, 0, 44100.);
	
	counter = 0;
	
	if (myEffect->ce[ 0]->flags & effFlagsCanReplacing) myEffect->ProcessReplacingNotAvailable = false;
	else myEffect->ProcessReplacingNotAvailable = true;
	
	// Check to see a default settings !
	for (i = 0; i < MAXVSTPREF; i++)
	{
		if (VSTPref[ i] != NULL)
		{
			if (VSTPref[ i]->ID == myEffect->ce[ 0]->uniqueID)
			{
				if (VSTPref[ i]->flag)
				{
					for (x = 0; x < myEffect->ce[ 0]->numParams; x++)
					{
						myEffect->ce[ 0]->setParameter(myEffect->ce[ 0], x, VSTPref[ i]->value[ x]);
						if (myEffect->ce[ 1]) myEffect->ce[ 1]->setParameter(myEffect->ce[ 1], x, VSTPref[ i]->value[ x]);
					}
					
					CurrentcurSelec = counter;
				}
				
				counter++;
			}
		}
	}
	
	myEffect->Active = true;
	
	return myEffect;
}

void FillVSTEffects(void)
{
	short		x, xx, i, alpha;
	FXSets*		set;
	VSTEffect*	myEffect;
	
//	return;
	
	for (i = 0; i < 10; i++)
	{
		if (MADDriver->masterVST[ i]) curMusic->header->globalEffect[ i] = MADDriver->masterVST[ i]->ce[ 0]->uniqueID;
		else curMusic->header->globalEffect[ i] = 0;
	}
	
	for (i = 0; i < MAXTRACK; i++)
	{
		for (x = 0; x < 4; x++)
		{
			if (MADDriver->chanVST[ i][ x]) curMusic->header->chanEffect[ i][ x] = MADDriver->chanVST[ i][ x]->ce[ 0]->uniqueID;
			else curMusic->header->chanEffect[ i][ x] = 0;
		}
	}
	
	alpha = 0;
	for (i = 0; i < 10 ; i++)	// Global Effects
	{
		if (MADDriver->masterVST[ i])
		{
			myEffect = MADDriver->masterVST[ i];
			
			set = &curMusic->sets[ alpha];
			set->id = i;
			set->track = -1;
			
			set->FXID = myEffect->ce[ 0]->uniqueID;
			set->noArg = myEffect->ce[ 0]->numParams;
			
			pStrcpy(set->name, myEffect->name);
			
			for (xx = 0; xx < set->noArg; xx++)
			{
				set->values[ xx] = myEffect->ce[ 0]->getParameter(myEffect->ce[ 0], xx);
			}
			
			alpha++;
		}
	}
	
	for (i = 0; i < curMusic->header->numChn ; i++)	// Channel Effects
	{
		for (x = 0; x < 4; x++)
		{
			if (MADDriver->chanVST[ i][ x])
			{
				myEffect = MADDriver->chanVST[ i][ x];
				
				set = &curMusic->sets[ alpha];
				set->id = i;
				set->track = -1;
				
				set->FXID = myEffect->ce[ 0]->uniqueID;
				set->noArg = myEffect->ce[ 0]->numParams;
				
				for (xx = 0; xx < set->noArg; xx++)
				{
					set->values[ xx] = myEffect->ce[ 0]->getParameter(myEffect->ce[ 0], xx);
				}
				
				alpha++;
			}
		}
	}
}

void ApplyVSTSets(VSTEffect* myEffect, FXSets* set)
{
	short	x;
	
	if (set->noArg != myEffect->ce[ 0]->numParams) DebugStr("\pApplyVSTSets");
	
	for (x = 0; x < myEffect->ce[ 0]->numParams; x++)
	{
		myEffect->ce[ 0]->setParameter(myEffect->ce[ 0], x, set->values[ x]);
		if (myEffect->ce[ 1]) myEffect->ce[ 1]->setParameter(myEffect->ce[ 1], x, set->values[ x]);
	}
}
