//
//  OSX-VSTPlugIn.c
//  PPMacho
//
//  Created by C.W. Betts on 2/6/14.
//
//

#include "VSTFunctions.h"
#include "MADPrivate.h"

void DisposeVSTEffect(MADDriverRec *intDriver, VSTEffect *myEffect)
{
	if (intDriver->vstFunctions) {
		intDriver->vstFunctions->DisposeVSTEffect(myEffect);
	}
}

VSTEffect* CreateVSTEffect(MADDriverRec *intDriver, short effectID)
{
	if (intDriver->vstFunctions) {
		intDriver->vstFunctions->CreateVSTEffect(effectID);
	}

	return NULL;
}

short ConvertUniqueIDToIndex(MADDriverRec *intDriver, uint32_t idx)
{
	if (intDriver->vstFunctions) {
		intDriver->vstFunctions->ConvertUniqueIDToIndex(idx);
	}
	
	return 0;
}

void ApplyVSTSets(MADDriverRec *intDriver, VSTEffect* myEffect, FXSets* set)
{
	if (intDriver->vstFunctions) {
		intDriver->vstFunctions->ApplyVSTSets(myEffect, set);
	}
}

bool IsVSTChanEffect(MADDriverRec *intDriver, short channel)
{
	if (intDriver->vstFunctions && intDriver->vstFunctions->IsVSTChanEffect) {
		return intDriver->vstFunctions->IsVSTChanEffect(intDriver, channel);
	}

	if( intDriver->base.curMusic != NULL) {
		if (intDriver->base.curMusic->header != NULL) {
			if (intDriver->base.curMusic->header->MultiChan) {
				MADChannel *curVoice = &intDriver->base.chan[channel];
				
				if (curVoice->curPtr >= curVoice->maxPtr && curVoice->loopSize == 0) {
					if (curVoice->TimeCounter > 0) {
						channel = curVoice->TrackID;
					} else {
						return false;
					}
				} else {
					channel = curVoice->TrackID;
				}
			}
			
			if (intDriver->base.curMusic->header->chanBus[channel].Active == false) {
				return false;
			}
			
			channel = intDriver->base.curMusic->header->chanBus[ channel].copyId;
			
		//	if( chanBus[ channel]->Active == false) return false;
			
			if (intDriver->chanVST[channel][0]) {
				if(intDriver->chanVST[channel][0]) {
					return true;
				}
			}
			if (intDriver->chanVST[channel][1]) {
				if(intDriver->chanVST[channel][1]) {
					return true;
				}
			}
			if (intDriver->chanVST[channel][2]) {
				if (intDriver->chanVST[channel][2]) {
					return true;
				}
			}
			if (intDriver->chanVST[channel][3]) {
				if (intDriver->chanVST[channel][3]) {
					return true;
				}
			}
			
			if (intDriver->base.curMusic->header->chanBus[channel].ByPass) {
				return true;
			}
		}
	}
	return false;
}

void ProcessVSTPlug(MADDriverRec *intDriver, int *data, int datasize, short channel)
{
	if (intDriver->vstFunctions) {
		intDriver->vstFunctions->ProcessVSTPlug(intDriver, data, datasize, channel);
	}
}

MADErr MADRegisterVSTPointer(MADDriverRecPtr intDriver, MADVSTFunctions * const funcs)
{
	if (intDriver->base.curMusic != NULL) {
		return MADDriverHasMusic;
	}
	intDriver->vstFunctions = funcs;
	return MADNoErr;
}

MADErr MADDeregisterVSTPointer(MADDriverRecPtr intDriver)
{
	if (intDriver->base.curMusic != NULL) {
		return MADDriverHasMusic;
	}
	intDriver->vstFunctions = NULL;
	return MADNoErr;
}
