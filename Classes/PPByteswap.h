//
//  PPByteswap.h
//  PPMacho
//
//  Created by C.W. Betts on 3/2/13.
//
//

#ifndef PPMacho_PPByteswap_h
#define PPMacho_PPByteswap_h

#include <dispatch/dispatch.h>
#include <PlayerPROCore/PlayerPROCore.h>

static inline void ByteSwapMADSpec(MADSpec *toSwap)
{
	PPBE32( &toSwap->MAD);
	PPBE16( &toSwap->speed);
	PPBE16( &toSwap->tempo);
	PPBE32( &toSwap->EPitch);
	PPBE32( &toSwap->ESpeed);
}

static inline void ByteSwapPatHeader(PatHeader *toSwap)
{
	PPBE32( &toSwap->size);
	PPBE32( &toSwap->compMode);
	PPBE32( &toSwap->patBytes);
	PPBE32( &toSwap->unused2);
}

static inline void ByteSwapInstrData(InstrData *toSwap)
{
	PPBE16( &toSwap->numSamples);
	PPBE16( &toSwap->firstSample);
	PPBE16( &toSwap->volFade);
	
	PPBE16( &toSwap->MIDI);
	PPBE16( &toSwap->MIDIType);
	__block InstrData *blockHappy = toSwap;
	dispatch_apply(12, dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_DEFAULT , 0), ^(size_t x) {
		PPBE16( &blockHappy->volEnv[ x].pos);
		PPBE16( &blockHappy->volEnv[ x].val);
		
		PPBE16( &blockHappy->pannEnv[ x].pos);
		PPBE16( &blockHappy->pannEnv[ x].val);
		
		PPBE16( &blockHappy->pitchEnv[ x].pos);
		PPBE16( &blockHappy->pitchEnv[ x].val);
	});
}

static inline void ByteSwapsData(sData *toSwap)
{
	PPBE32( &toSwap->size);
	PPBE32( &toSwap->loopBeg);
	PPBE32( &toSwap->loopSize);
	PPBE16( &toSwap->c2spd);
}

#endif
