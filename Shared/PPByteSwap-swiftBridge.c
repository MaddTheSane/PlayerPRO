//
//  PPByteSwap-swiftBridge.c
//  PPMacho
//
//  Created by C.W. Betts on 7/18/14.
//
//

#include <CoreFoundation/CFByteOrder.h>

extern inline void PPBE16(void *msg_buf)
{
	uint32_t temp = *((uint32_t*)msg_buf);
	*((uint16_t*)msg_buf) = CFSwapInt16BigToHost(temp);
}

extern inline void PPBE32(void *msg_buf)
{
	uint32_t temp = *((uint32_t*)msg_buf);
	*((uint32_t*)msg_buf) = CFSwapInt32BigToHost(temp);
}


extern inline void PPLE16(void *msg_buf)
{
	uint32_t temp = *((uint32_t*)msg_buf);
	*((uint16_t*)msg_buf) = CFSwapInt16LittleToHost(temp);
}

extern inline void PPLE32(void *msg_buf)
{
	uint32_t temp = *((uint32_t*)msg_buf);
	*((uint32_t*)msg_buf) = CFSwapInt32LittleToHost(temp);
}


