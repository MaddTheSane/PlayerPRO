#include "Gestalt.h"

Boolean TestProcessorChip()
{
	OSErr		iErr;
	Boolean		result;
	short		myBit;
	long		gestaltAnswer;
	
	iErr = Gestalt( gestaltPowerPCProcessorFeatures, &gestaltAnswer);
	if( iErr) return false;
	
	myBit = gestaltPowerPCHasVectorInstructions;
	result = BitTst( &gestaltAnswer, 31-myBit);
	if( result == false) return result;
	
	myBit = gestaltPowerPCHasDataStreams;
	result = BitTst( &gestaltAnswer, 31-myBit);
	if( result == false) return result;
	
	return true;
}
