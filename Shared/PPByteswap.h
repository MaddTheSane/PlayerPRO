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

#define BYTESWAP_STRIDE 8

static inline void ByteSwapMADSpec(MADSpec *toSwap)
{
	MADBE32(&toSwap->MAD);
	MADBE16(&toSwap->speed);
	MADBE16(&toSwap->tempo);
	MADBE32(&toSwap->EPitch);
	MADBE32(&toSwap->ESpeed);
	
	for (int i = 0; i < 10; i++) {
		MADBE32(&toSwap->globalEffect[i]);
	}
	
	for (int i = 0; i < MAXTRACK * 4; i++) {
		MADBE32(&toSwap->chanEffect[i / 4][i % 4]);
	}
	
	for (int i = 0; i < MAXTRACK; i++) {
		MADBE16(&toSwap->chanBus[i].copyId);
	}
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
	for (int x = 0; x < 12; x++) {
		MADBE16(&toSwap->volEnv[x].pos);
		MADBE16(&toSwap->volEnv[x].val);
		
		MADBE16(&toSwap->pannEnv[x].pos);
		MADBE16(&toSwap->pannEnv[x].val);
		
		MADBE16(&toSwap->pitchEnv[x].pos);
		MADBE16(&toSwap->pitchEnv[x].val);
	}
}

static inline void ByteSwapFXSets(FXSets *toSwap)
{
	MADBE16(&toSwap->track);
	MADBE16(&toSwap->id);
	MADBE32(&toSwap->FXID);
	MADBE16(&toSwap->noArg);
#if __LITTLE_ENDIAN__
	dispatch_apply(100 / BYTESWAP_STRIDE, dispatch_get_global_queue(0, 0), ^(size_t y) {
		size_t j = y * BYTESWAP_STRIDE;
		
		MADBE32(&toSwap->values[j+0]);
		MADBE32(&toSwap->values[j+1]);
		MADBE32(&toSwap->values[j+2]);
		MADBE32(&toSwap->values[j+3]);
		MADBE32(&toSwap->values[j+4]);
		MADBE32(&toSwap->values[j+5]);
		MADBE32(&toSwap->values[j+6]);
		MADBE32(&toSwap->values[j+7]);
	});
	for (int i = 100 - (100 % BYTESWAP_STRIDE); i < 100; i++) {
		MADBE32(&toSwap->values[i]);
	}
#endif
}

static inline void ByteSwapsData(sData *toSwap)
{
	MADBE32(&toSwap->size);
	MADBE32(&toSwap->loopBeg);
	MADBE32(&toSwap->loopSize);
	MADBE16(&toSwap->c2spd);
}

#undef BYTESWAP_STRIDE

#endif
