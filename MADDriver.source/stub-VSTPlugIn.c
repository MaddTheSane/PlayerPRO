//
//  stub-VSTPlugIn.c
//  PPMacho
//
//  Created by C.W. Betts on 2/6/14.
//
//

#include "VSTFunctions.h"

void DisposeVSTEffect(MADDriverRec *intDriver, VSTEffect *myEffect)
{
	
}

VSTEffect* CreateVSTEffect(MADDriverRec *intDriver, short effectID)
{
	return NULL;
}

short ConvertUniqueIDToIndex(MADDriverRec *intDriver, uint32_t idx)
{
	return 0;
}

void ApplyVSTSets(MADDriverRec *intDriver, VSTEffect* myEffect, FXSets* set)
{
	
}

bool IsVSTChanEffect(MADDriverRec *intDriver, short channel)
{
	return false;
}

void ProcessVSTPlug(MADDriverRec *intDriver, int *data, int datasize, short channel)
{
	
}

MADErr MADRegisterVSTPointer(MADDriverRecPtr intDriver, MADVSTFunctions * const funcs)
{
	// do nothing
	return MADNoErr;
}

MADErr MADDeregisterVSTPointer(MADDriverRecPtr intDriver)
{
	// do nothing
	return MADNoErr;
}
