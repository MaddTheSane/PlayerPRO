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
#include <PlayerPROCore/MAD.h>

static inline void ByteSwapMADSpec(MADSpec *toSwap)
{
	MADBE32(&toSwap->MAD);
	MADBE16(&toSwap->speed);
	MADBE16(&toSwap->tempo);
	MADBE32(&toSwap->EPitch);
	MADBE32(&toSwap->ESpeed);
	
	dispatch_apply(10, dispatch_get_global_queue(0, 0), ^(size_t i) {
		MADBE32(&toSwap->globalEffect[i]);
	});
	
	dispatch_apply(MAXTRACK * 4, dispatch_get_global_queue(0, 0), ^(size_t i) {
		MADBE32(&toSwap->chanEffect[i / 4][i % 4]);
	});
	
	dispatch_apply(MAXTRACK, dispatch_get_global_queue(0, 0), ^(size_t i) {
		MADBE16(&toSwap->chanBus[i].copyId);
	});
}

static inline void ByteSwapPatHeader(PatHeader *toSwap)
{
	MADBE32(&toSwap->size);
	MADBE32(&toSwap->compMode);
	MADBE32(&toSwap->patBytes);
	MADBE32(&toSwap->unused2);
}

static inline void ByteSwapInstrData(InstrData *toSwap)
{
	MADBE16(&toSwap->numSamples);
	MADBE16(&toSwap->firstSample);
	MADBE16(&toSwap->volFade);
	
	MADBE16(&toSwap->MIDI);
	MADBE16(&toSwap->MIDIType);
	dispatch_apply(12, dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_DEFAULT, 0), ^(size_t x) {
		MADBE16(&toSwap->volEnv[x].pos);
		MADBE16(&toSwap->volEnv[x].val);
		
		MADBE16(&toSwap->pannEnv[x].pos);
		MADBE16(&toSwap->pannEnv[x].val);
		
		MADBE16(&toSwap->pitchEnv[x].pos);
		MADBE16(&toSwap->pitchEnv[x].val);
	});
}

static inline void ByteSwapFXSets(FXSets *toSwap)
{
	MADBE16(&toSwap->track);
	MADBE16(&toSwap->id);
	MADBE32(&toSwap->FXID);
	MADBE16(&toSwap->noArg);
	dispatch_apply(100, dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_DEFAULT, 0), ^(size_t i) {
		MADBE32(&toSwap->values[i]);
	});
}

static inline void ByteSwapsData(sData *toSwap)
{
	MADBE32(&toSwap->size);
	MADBE32(&toSwap->loopBeg);
	MADBE32(&toSwap->loopSize);
	MADBE16(&toSwap->c2spd);
}

#endif
