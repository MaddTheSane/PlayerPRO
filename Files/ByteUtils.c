/*
 *  ByteUtils.c
 *  PPMacho
 *
 *  Created by C.W. Betts on 12/2/09.
 *  Copyright 2009 __MyCompanyName__. All rights reserved.
 *
 */

#include "Shuddup.h"
#include "ByteUtils.h"

void SwapPrefs(Prefs *swapPrefs)
{
#ifdef __LITTLE_ENDIAN__
//TODO: Floats and double byteswap?
	int i, x;
	swapPrefs->Version = EndianS16_BtoN(swapPrefs->Version);
	swapPrefs->Frequence = EndianS32_BtoN(swapPrefs->Frequence);
	swapPrefs->oldShort = EndianS16_BtoN(swapPrefs->oldShort);
	swapPrefs->NoStart = EndianS16_BtoN(swapPrefs->NoStart);
	for (i=0; i < 30; i++) {
		swapPrefs->a1[i].v = EndianS16_BtoN(swapPrefs->a1[i].v);
		swapPrefs->a1[i].h = EndianS16_BtoN(swapPrefs->a1[i].h);
		swapPrefs->a2[i] = EndianS16_BtoN(swapPrefs->a2[i]);
		swapPrefs->a3[i] = EndianS16_BtoN(swapPrefs->a3[i]);
		swapPrefs->a4[i] = EndianS16_BtoN(swapPrefs->a4[i]);
		swapPrefs->a5[i] = EndianS32_BtoN(swapPrefs->a5[i]);
		for (x = 0; x<3; x++) {
			swapPrefs->a6[x][i].v = EndianS16_BtoN(swapPrefs->a6[x][i].v);
			swapPrefs->a6[x][i].h = EndianS16_BtoN(swapPrefs->a6[x][i].h);
			swapPrefs->a7[x][i] = EndianS16_BtoN(swapPrefs->a7[x][i]);
			swapPrefs->a8[x][i] = EndianS16_BtoN(swapPrefs->a8[x][i]);
			swapPrefs->a9[x][i] = EndianS16_BtoN(swapPrefs->a9[x][i]);
			swapPrefs->a10[x][i] = EndianS32_BtoN(swapPrefs->a10[x][i]);
		}
	}
	for (i=0; i < 300; i++) {
		swapPrefs->PianoKey[i] = EndianS16_BtoN(swapPrefs->PianoKey[i]);
	}
	
	swapPrefs->LoopType = EndianS16_BtoN(swapPrefs->LoopType);
	swapPrefs->SNumber = EndianS32_BtoN(swapPrefs->SNumber);
	swapPrefs->checkSum = EndianS32_BtoN(swapPrefs->checkSum);
	swapPrefs->Mz = EndianS32_BtoN(swapPrefs->Mz);
	swapPrefs->firstStart = EndianU32_BtoN(swapPrefs->firstStart);
	for (i=0; i < MAXTRACK; i++) {
		//FIXME: Is this the right way to do this?
		swapPrefs->tracksColor[i].red = EndianU16_BtoN(swapPrefs->tracksColor[i].red);
		swapPrefs->tracksColor[i].green = EndianU16_BtoN(swapPrefs->tracksColor[i].green);
		swapPrefs->tracksColor[i].blue = EndianU16_BtoN(swapPrefs->tracksColor[i].blue);
//		swapPrefs->unused[i] = EndianS16_BtoL(swapPrefs->unused[i]);
		swapPrefs->Previous_chanBus[i].copyId = EndianS16_BtoL(swapPrefs->Previous_chanBus[i].copyId);
		swapPrefs->Previous_Sets[i].track = EndianS16_BtoL(swapPrefs->Previous_Sets[i].track);
		swapPrefs->Previous_Sets[i].id = EndianS16_BtoL(swapPrefs->Previous_Sets[i].id);
		swapPrefs->Previous_Sets[i].FXID = EndianS32_BtoL(swapPrefs->Previous_Sets[i].FXID);
		swapPrefs->Previous_Sets[i].noArg = EndianS16_BtoL(swapPrefs->Previous_Sets[i].noArg);
		for (x=0; x < 4; x++) {
			swapPrefs->Previous_chanEffect[i][x] = EndianS32_BtoN(swapPrefs->Previous_chanEffect[i][x]);
		}
	}
	
	swapPrefs->PianoPos = EndianS16_BtoN(swapPrefs->PianoPos);
	swapPrefs->volumeLevel = EndianS16_BtoN(swapPrefs->volumeLevel);
	swapPrefs->TextS = EndianS16_BtoN(swapPrefs->TextS);
	swapPrefs->OscilloSize = EndianS16_BtoN(swapPrefs->OscilloSize);
	swapPrefs->OscilloType = EndianS16_BtoN(swapPrefs->OscilloType);
	swapPrefs->fileTypeExportSND = EndianS16_BtoN(swapPrefs->fileTypeExportSND);
	swapPrefs->CompressionExportSND = EndianU32_BtoN(swapPrefs->CompressionExportSND);
	swapPrefs->SpectrumSize = EndianS16_BtoN(swapPrefs->SpectrumSize);
	swapPrefs->SpectrumType = EndianS16_BtoN(swapPrefs->SpectrumType);
	swapPrefs->numChn = EndianS16_BtoN(swapPrefs->numChn);
	swapPrefs->outPutBits = EndianS16_BtoN(swapPrefs->outPutBits);
	swapPrefs->outPutRate = EndianU32_BtoN(swapPrefs->outPutRate);
	swapPrefs->outPutMode = EndianS16_BtoN(swapPrefs->outPutMode);
	swapPrefs->driverMode = EndianS16_BtoN(swapPrefs->driverMode);
	swapPrefs->MarkersSize = EndianS16_BtoN(swapPrefs->MarkersSize);
	swapPrefs->MarkersOffSet = EndianS16_BtoN(swapPrefs->MarkersOffSet);
	swapPrefs->MozartX = EndianS16_BtoN(swapPrefs->MozartX);
	swapPrefs->FastDigitalEdition = EndianS32_BtoN(swapPrefs->FastDigitalEdition);
	swapPrefs->KeyUpMode = EndianS16_BtoN(swapPrefs->KeyUpMode);
	swapPrefs->MicroDelaySize = EndianS32_BtoN(swapPrefs->MicroDelaySize);
	swapPrefs->MozartC1h = EndianS16_BtoN(swapPrefs->MozartC1h);
	swapPrefs->MozartC2h = EndianS16_BtoN(swapPrefs->MozartC2h);
	swapPrefs->SoundTypeSamp = EndianU32_BtoN(swapPrefs->SoundTypeSamp);
	swapPrefs->SoundTypeIns = EndianU32_BtoN(swapPrefs->SoundTypeIns);
	swapPrefs->LinesHeight = EndianS16_BtoN(swapPrefs->LinesHeight);
	swapPrefs->softVolumeLevel = EndianS16_BtoN(swapPrefs->softVolumeLevel);
	swapPrefs->ReverbSize = EndianS32_BtoN(swapPrefs->ReverbSize);
	swapPrefs->ReverbStrength = EndianS32_BtoN(swapPrefs->ReverbStrength);
	swapPrefs->ChannelType = EndianS16_BtoN(swapPrefs->ChannelType);
	swapPrefs->amplitude = EndianS16_BtoN(swapPrefs->amplitude);
	swapPrefs->Compressor = EndianU32_BtoN(swapPrefs->Compressor);
	swapPrefs->FrequenceSpeed = EndianS32_BtoN(swapPrefs->FrequenceSpeed);
	swapPrefs->TempsNum = EndianS16_BtoN(swapPrefs->TempsNum);
	swapPrefs->TempsUnit = EndianS16_BtoN(swapPrefs->TempsUnit);
	swapPrefs->TrackHeight = EndianS16_BtoN(swapPrefs->TrackHeight);
	
	for (i=0; i < 20; i++) {
		swapPrefs->FKeyActive[i] = EndianS16_BtoN(swapPrefs->FKeyActive[i]);
		swapPrefs->FKeyItem[i] = EndianS16_BtoN(swapPrefs->FKeyItem[i]);
		swapPrefs->FKeyWind[i] = EndianS16_BtoN(swapPrefs->FKeyWind[i]);
	}
	
	swapPrefs->yellC.red = EndianU16_BtoN(swapPrefs->yellC.red);
	swapPrefs->yellC.green = EndianU16_BtoN(swapPrefs->yellC.green);
	swapPrefs->yellC.blue = EndianU16_BtoN(swapPrefs->yellC.blue);

	swapPrefs->whichEditorPatterns = EndianS16_BtoN(swapPrefs->whichEditorPatterns);
	swapPrefs->oversampling = EndianS32_BtoN(swapPrefs->oversampling);
	swapPrefs->RAWBits = EndianS16_BtoN(swapPrefs->RAWBits);
	swapPrefs->RAWRate = EndianS32_BtoN(swapPrefs->RAWRate);
	swapPrefs->RAWLength = EndianS32_BtoN(swapPrefs->RAWLength);
	swapPrefs->RAWHeader = EndianS32_BtoN(swapPrefs->RAWHeader);
	swapPrefs->pianoOffset = EndianS16_BtoN(swapPrefs->pianoOffset);
	swapPrefs->previousSpec.MAD = EndianU32_BtoN(swapPrefs->previousSpec.MAD);
	swapPrefs->previousSpec.EPitch = EndianS32_BtoN(swapPrefs->previousSpec.EPitch);
	swapPrefs->previousSpec.ESpeed = EndianS32_BtoN(swapPrefs->previousSpec.ESpeed);
	swapPrefs->previousSpec.speed = EndianS16_BtoN(swapPrefs->previousSpec.speed);
	swapPrefs->previousSpec.tempo = EndianS16_BtoN(swapPrefs->previousSpec.tempo);
	
	for (x = 0; x < EQPACKET*2; x++)
		{
			union {
				UInt64 inte;
				double doub;
			} v;
			v.doub = swapPrefs->Filter[x];
			v.inte = EndianU64_BtoN(v.inte);
			swapPrefs->Filter[x] = v.doub;
		}
	for (i=0; i < MAXWINDOWS; i++) {
		swapPrefs->WinPos[i].v = EndianS16_BtoN(swapPrefs->WinPos[i].v);
		swapPrefs->WinPos[i].h = EndianS16_BtoN(swapPrefs->WinPos[i].h);
		swapPrefs->WinEtat[i] = EndianS16_BtoN(swapPrefs->WinEtat[i]);
		swapPrefs->WinHi[i] = EndianS16_BtoN(swapPrefs->WinHi[i]);
		swapPrefs->WinLarg[i] = EndianS16_BtoN(swapPrefs->WinLarg[i]);
		swapPrefs->WinID[i] = EndianS32_BtoN(swapPrefs->WinID[i]);
		
		for (x = 0; x < 3; x++) {
			swapPrefs->WinPosO[x][i].v = EndianS16_BtoN(swapPrefs->WinPosO[x][i].v);
			swapPrefs->WinPosO[x][i].h = EndianS16_BtoN(swapPrefs->WinPosO[x][i].h);
			swapPrefs->WinEtatO[x][i] = EndianS16_BtoN(swapPrefs->WinEtatO[x][i]);
			swapPrefs->WinLargO[x][i] = EndianS16_BtoN(swapPrefs->WinLargO[x][i]);
			swapPrefs->WinIDO[x][i] = EndianS32_BtoN(swapPrefs->WinIDO[x][i]);
			swapPrefs->WinHiO[x][i] = EndianS16_BtoN(swapPrefs->WinHiO[x][i]);
		}
	}
	
	swapPrefs->lastVisualPlugin = EndianS16_BtoN(swapPrefs->lastVisualPlugin);
	swapPrefs->channelNumber = EndianS16_BtoN(swapPrefs->channelNumber);

	swapPrefs->DirectDriverType.numChn = EndianS16_BtoN(swapPrefs->DirectDriverType.numChn);
	swapPrefs->DirectDriverType.outPutBits = EndianS16_BtoN(swapPrefs->DirectDriverType.outPutBits);
	swapPrefs->DirectDriverType.outPutRate = EndianU32_BtoN(swapPrefs->DirectDriverType.outPutRate);
	swapPrefs->DirectDriverType.outPutMode = EndianS16_BtoN(swapPrefs->DirectDriverType.outPutMode);
	swapPrefs->DirectDriverType.driverMode = EndianS16_BtoN(swapPrefs->DirectDriverType.driverMode);
	swapPrefs->DirectDriverType.MicroDelaySize = EndianS32_BtoN(swapPrefs->DirectDriverType.MicroDelaySize);
	swapPrefs->DirectDriverType.ReverbSize = EndianS32_BtoN(swapPrefs->DirectDriverType.ReverbSize);
	swapPrefs->DirectDriverType.ReverbStrength = EndianS32_BtoN(swapPrefs->DirectDriverType.ReverbStrength);
	swapPrefs->DirectDriverType.oversampling = EndianS32_BtoN(swapPrefs->DirectDriverType.oversampling);
	for (i=0; i < 10; i++) {
		swapPrefs->Previous_globalEffect[i] = EndianS16_BtoN(swapPrefs->Previous_globalEffect[i]);
	}
	
#endif
}
