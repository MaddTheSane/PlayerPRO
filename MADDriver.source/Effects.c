/********************						***********************/
//
//	Player PRO 5.9 - DRIVER SOURCE CODE -
//
//	Library Version 5.9
//
//	To use with MAD Library for Mac: Symantec, CodeWarrior and MPW
//
//	Antoine ROSSET
//	20 Micheli-Du-Crest
//	1205 GENEVA
//	SWITZERLAND
//
//	COPYRIGHT ANTOINE ROSSET 1996, 1997, 1998, 1999, 2000, 2001, 2002
//
//	Thank you for your interest in PlayerPRO !
//
//	FAX:			(+41 22) 346 11 97
//	PHONE: 			(+41 79) 203 74 62
//	Internet: 		RossetAntoine@bluewin.ch
//
/********************						***********************/

#include "RDriver.h"
#include "RDriverInt.h"
#include "MADPrivate.h"

#define LOW(para) ((para) & 15)
#define HI(para) ((para) >> 4)

void parse_slidevol(Channel *ch, MADByte Arg)
{
	if (HI(Arg))
		ch->volumerate = HI(Arg);
	else
		ch->volumerate = -LOW(Arg);
}

void CloseEffect(Channel *ch, short notUsed, MADDriverRec *intDriver)
{
	switch (ch->cmd) {
		case MADEffectArpeggio:
			if (ch->arpUse) {
				ch->period = ch->arp[0];
				ch->arpUse = false;
			}
			break;
			
		case MADEffectSkip:
			break;
			
		case MADEffectFastSkip:
			break;
			
		case MADEffectDownslide:
			break;
			
		case MADEffectUpslide:
			break;
			
		case MADEffectVibrato:
			ch->period = ch->periodOld;
			break;
			
		case MADEffectSlideVolume:
			break;
			
		case MADEffectPortamento:
			break;
			
		case MADEffectPortaSlide:
			ch->cmd = MADEffectPortamento;
			CloseEffect(ch, 0, intDriver);
			
			ch->cmd = MADEffectSlideVolume;
			CloseEffect(ch, 0, intDriver);
			
			ch->cmd = MADEffectPortaSlide;
			break;
			
		case MADEffectVibratoSlide:
			ch->cmd = MADEffectVibrato;
			CloseEffect(ch, 0, intDriver);
			
			ch->cmd = MADEffectSlideVolume;
			CloseEffect(ch, 0, intDriver);
			
			ch->cmd = MADEffectVibratoSlide;
			break;
			
		case MADEffectExtended:
			switch (HI(ch->arg)) {
				case 6:
					
					break;
			}
			break;
			
		default:
			break;
			
	}
	
	ch->arg = 0;
	ch->cmd = 0;
}

void DoEffect(Channel *ch, short call, MADDriverRec *intDriver)
{
	int offset = 0;
	
	switch(ch->cmd) {
		case MADEffectArpeggio:						// OK
			if (ch->arg != 0 && ch->arpUse == true) {
				ch->arpindex++;
				if (ch->arpindex >= MAX_ARP)
					ch->arpindex = 0;
				
				ch->period = ch->arp[ch->arpindex];
			}
			break;
			
		case MADEffectSkip:							// OK
			if (call == intDriver->base.speed - 1) {
				intDriver->endPattern = true;
				
				if (intDriver->JumpToNextPattern) {
					if (intDriver->base.PartitionReader != 0) {
						intDriver->base.PL++;
						intDriver->base.Pat = intDriver->base.curMusic->header->oPointers[intDriver->base.PL];
					}
					
					intDriver->base.PartitionReader = HI(ch->arg) * 10 + LOW(ch->arg);
					
					if (intDriver->base.PL >= intDriver->base.curMusic->header->numPointers) {
						intDriver->base.PL = 0;
						intDriver->base.Pat = intDriver->base.curMusic->header->oPointers[intDriver->base.PL];
						
						MADCleanDriver(intDriver);
						if (!intDriver->base.DriverSettings.repeatMusic)
							intDriver->base.Reading = false;
						
						intDriver->base.musicEnd = true;
					}
				} else {
					intDriver->base.PartitionReader = 0;
				}
				ch->cmd = 0;
				ch->arg = 0;
			}
			break;
			
		case MADEffectFastSkip:						// OK
			if (call == intDriver->base.speed - 1) {
				intDriver->endPattern = true;
				
				if (intDriver->JumpToNextPattern) {
					if (intDriver->base.PL > ch->arg) {		// Evite les boucles
						intDriver->base.musicEnd = true;
						
						if (!intDriver->base.DriverSettings.repeatMusic)
							intDriver->base.Reading = false;
					}
					
					intDriver->base.PL = ch->arg;
					intDriver->base.Pat = intDriver->base.curMusic->header->oPointers[intDriver->base.PL];
					
					if (intDriver->base.PL >= intDriver->base.curMusic->header->numPointers) {
						intDriver->base.PL = 0;
						intDriver->base.Pat = intDriver->base.curMusic->header->oPointers[intDriver->base.PL];
						
						MADCleanDriver(intDriver);
						if (!intDriver->base.DriverSettings.repeatMusic)
							intDriver->base.Reading = false;
						
						intDriver->base.musicEnd = true;
					}
				}
				intDriver->base.PartitionReader = 0;
				ch->cmd = 0;
				ch->arg = 0;
			}
			break;
			
		case MADEffectDownslide:						// OK
			if (intDriver->MODMode) {
				ch->period -= ch->slide*4;
				if (ch->period < intDriver->MOD_MIN_PITCH)
					ch->period = intDriver->MOD_MIN_PITCH;
			} else {
				if (ch->period > intDriver->MIN_PITCH)
					ch->period -= ch->slide * 4;
			}
			break;
			
		case MADEffectUpslide:							// OK
			if (intDriver->MODMode) {
				ch->period += ch->slide*4;
				if (ch->period > intDriver->MOD_MAX_PITCH)
					ch->period = intDriver->MOD_MAX_PITCH;
			} else {
				if (ch->period < intDriver->MAX_PITCH)
					ch->period += ch->slide*4;
			}
			break;
			
		case MADEffectVibrato:
		{
			MADByte q = (ch->viboffset >> 2) & 0x1f;
			
			switch (ch->vibtype) {
				case 0:
					offset = intDriver->vibrato_table[q];
					break;
					
				case 1:
					q <<= 3;
					if (ch->viboffset < 0)
						q = 255 - q;
					offset = q;
					break;
					
				case 2:
					offset = 255;
					break;
			}
			
			offset *= ch->vibdepth;
			offset >>= 7;
			offset <<= 2;
			
			if (ch->viboffset >= 0)
				ch->period = ch->periodOld + offset;
			else
				ch->period = ch->periodOld - offset;
			
			ch->viboffset += ch->vibrate;
		}
			break;
			
		case MADEffectSlideVolume:						// OK
			ch->vol += ch->volumerate;
			
			if (ch->vol < MIN_VOLUME)
				ch->vol = MIN_VOLUME;
			else if (ch->vol > MAX_VOLUME)
				ch->vol = MAX_VOLUME;
			break;
			
		case MADEffectPortamento:
			if (ch->period != ch->pitchgoal) {
				if (ch->period < ch->pitchgoal) {
					ch->period += ch->pitchrate * 4;
					if (ch->period > ch->pitchgoal) {
						ch->cmd = 0;
						ch->arg = 0;
						ch->period = ch->pitchgoal;
					}
				} else if (ch->period > ch->pitchgoal) {
					ch->period -= ch->pitchrate*4;
					if (ch->period < ch->pitchgoal) {
						ch->cmd = 0;
						ch->arg = 0;
						ch->period = ch->pitchgoal;
					}
				}
			}
			break;
			
		case MADEffectPortaSlide:
			ch->cmd = MADEffectPortamento;
			DoEffect(ch, call, intDriver);
			
			ch->cmd = MADEffectSlideVolume;
			DoEffect(ch, call, intDriver);
			
			ch->cmd = MADEffectPortaSlide;
			break;
			
		case MADEffectVibratoSlide:
			ch->cmd = MADEffectVibrato;
			DoEffect(ch, call, intDriver);
			
			ch->cmd = MADEffectSlideVolume;
			DoEffect(ch, call, intDriver);
			
			ch->cmd = MADEffectVibratoSlide;
			break;
			
		case MADEffectExtended:
			switch (HI(ch->arg)) {
				case 12:
					if (call >= LOW(ch->arg))
						ch->vol = 0;
					break;
					
				case 9:
					if (LOW(ch->arg)) {
						if (ch->trig == 0) {
							ch->curPtr	= ch->begPtr;
							StartPanning(ch);
							StartEnvelope(ch);
							
							ch->trig = LOW(ch->arg);
						}
						ch->trig--;
					}
					break;
					
				case 6:
					if (call == intDriver->base.speed - 1) {
						if (LOW(ch->arg) == 0) { // Set Pattern loop
							ch->PatternLoopE6 = intDriver->base.PartitionReader;
							ch->PatternLoopE6ID = intDriver->base.PL;
						} else {
							if (ch->PatternLoopE6Count == 0)
								ch->PatternLoopE6Count = LOW(ch->arg);
							else
								ch->PatternLoopE6Count--;
							
							if (ch->PatternLoopE6Count > 0) {
								intDriver->base.PartitionReader = ch->PatternLoopE6-1;
								intDriver->base.PL = ch->PatternLoopE6ID;
								intDriver->base.Pat = intDriver->base.curMusic->header->oPointers[intDriver->base.PL];
							}
						}
					}
					break;
			}
			break;
			
		default:
			ch->cmd = 0;
			ch->arg = 0;
			return;
			break;
	}
#if 0
	if (call == intDriver->base.speed - 1)
	{
		ch->arg = 0;
		ch->cmd = 0;
	}
#endif
}

void SetUpCmdEffect(Channel *ch, MADDriverRec *intDriver)
{
	short vol = ch->volcmd;
	short volLO = vol & 0xf;
	short cmdCopy = ch->cmd;
	short argCopy = ch->arg;
	
	switch (vol >> 4) {
		case 0x8:
			ch->cmd = MADEffectExtended;
			ch->arg = 0xb0 | volLO;
			
			SetUpEffect(ch, intDriver);
			break;
			
		case 0x9:
			ch->cmd = MADEffectExtended;
			ch->arg = 0xa0 | volLO;
			
			SetUpEffect(ch, intDriver);
			break;
			
		case 0xa: // set vibrato speed
			ch->cmd = MADEffectVibrato;
			ch->arg = vol << 4;
			
			SetUpEffect(ch, intDriver);
			break;
			
		case 0xb: // vibrato
			ch->cmd = MADEffectVibrato;
			ch->arg = volLO;
			
			SetUpEffect(ch, intDriver);
			break;
			
		case 0xc: // panning
			ch->cmd = MADEffectPanning;
			ch->arg = vol << 4;
			
			SetUpEffect(ch, intDriver);
			break;
			
		case 0xf: // tone porta
			ch->cmd = MADEffectPortamento;
			ch->arg = vol << 4;
			
			SetUpEffect(ch, intDriver);
			break;
	}
	
	ch->cmd = cmdCopy;
	ch->arg = argCopy;
}

void SetUpEffect(Channel *ch, MADDriverRec *intDriver)
{
	short 	temp, note;
	long	aL;
	
	if (ch->arg == 0) {
		switch (ch->cmd) {
			case MADEffectArpeggio:
			case MADEffectNothing:
			case MADEffectFastSkip:
			case MADEffectVolume:
			case MADEffectPanning:
			case MADEffectSkip:
			case MADEffectExtended:
			case MADEffectSpeed:
				//case MADEffectSlideVolume:
				break;
				
			default:
				ch->arg = ch->oldArg[ch->cmd];
				break;
		}
	} else
		ch->oldArg[ch->cmd] = ch->arg;
	
	switch (ch->cmd) {
		case MADEffectUpslide: // OK
			if (ch->arg)
				ch->slide = ch->arg;
			break;
			
		case MADEffectDownslide: // OK
			if (ch->arg)
				ch->slide = ch->arg;
			break;
			
		case MADEffectVibrato: // OK
			if (HI(ch->arg))
				ch->vibrate = (ch->arg & 0xf0) >> 2;
			else
				ch->vibrate = ch->oldVibrate;
			
			if (LOW(ch->arg))
				ch->vibdepth = LOW(ch->arg);
			else
				ch->vibdepth = ch->oldVibdepth;
			
			ch->oldVibdepth = ch->vibdepth;
			ch->oldVibrate = ch->vibrate;
			
			//	ch->viboffset = 0;
			ch->periodOld = ch->period;
			break;
			
		case MADEffectArpeggio: // OK
			if (ch->arg == 0)
				ch->arpUse = false;
			else {
				int inNote = ch->note;
				
				if (inNote == 0xFF)
					inNote = ch->noteOld;
				
				if (inNote != 0xFF) {
					note = inNote + HI(ch->arg);
					if (note < NUMBER_NOTES)
						ch->arp[1] = GetOldPeriod(note, ch->fineTune, intDriver);
					
					note = inNote + LOW(ch->arg);
					if (note < NUMBER_NOTES)
						ch->arp[2] = GetOldPeriod(note, ch->fineTune, intDriver);
					
					ch->arpindex = 0;
					ch->arp[0] = ch->period;
					
					ch->arpUse = true;
				} else
					ch->arpUse = false;
			}
			break;
			
		case MADEffectSlideVolume: // OK
			parse_slidevol(ch, ch->arg);
			break;
			
		case MADEffectExtended:
			switch (HI(ch->arg)) {
				case 0: // Turn On/Off filter
					break;
					
				case 1: // Fineslide up
					temp = LOW(ch->arg);
					ch->period -= temp * 4;
					break;
					
				case 2: // Fineslide down
					temp = LOW(ch->arg);
					ch->period += temp * 4;
					break;
					
				case 3: // Set glissando on/off
					break;
					
				case 4:		// Set vibrato waveform
					switch (LOW(ch->arg)) {
						case 0:
						case 4:
							ch->vibtype = 0;
							break;
							
						case 1:
						case 5:
							ch->vibtype = 1;
							break;
							
						case 2:
						case 6:
							ch->vibtype = 2;
							break;
							
						case 3:
						case 7:
							ch->vibtype = 0;
							break;
					}
					break;
					
				case 5: // Set finetune value
					//ch->fineTune	= finetune[LOW(ch->arg)];
					//ch->period	= GetOldPeriod(ch->Amiga, ch->fineTune);
					break;
					
				case 6: // Loop pattern
					break;
					
				case 7: // Set tremolo waveform
					break;
					
				case 8: // Set Panning
					ch->pann = LOW(ch->arg);
					
					if(ch->pann <= 8)
						ch->pann <<= 4;
					else
						ch->pann *= 17;
					
					ch->pann = ((int)ch->pann *  MAX_PANNING) / (int)0xFF;
					
					if (ch->pann < 0)
						ch->pann = 0;
					else if (ch->pann > MAX_PANNING)
						ch->pann = MAX_PANNING;
					
					ch->PanningE8 = true;
					break;
					
				case 9:
					if (LOW(ch->arg)) {
						if (ch->trig == 0) {
							ch->trig	= LOW(ch->arg);
						}
						ch->trig--;
					}
					break;
					
				case 10: // Fine volume slide up
					ch->vol += LOW(ch->arg);
					
					if (ch->vol < MIN_VOLUME)
						ch->vol = MIN_VOLUME;
					else if (ch->vol > MAX_VOLUME)
						ch->vol = MAX_VOLUME;
					break;
					
				case 11: // Fine volume slide down
					ch->vol -= LOW(ch->arg);
					
					if (ch->vol < MIN_VOLUME)
						ch->vol = MIN_VOLUME;
					else if (ch->vol > MAX_VOLUME)
						ch->vol = MAX_VOLUME;
					break;
					
				case 12: // Cut sample
					break;
					
				case 13: // Delay sample
					break;
					
				case 14: // Delay pattern
					intDriver->PatDelay = LOW(ch->arg) + 1;
					break;
					
				case 15: // Invert loop
					break;
			}
			
			break;
			
		case MADEffectPortamento:				// OK
		{
			int inNote = ch->note;
			
			ch->pitchrate = ch->arg;
			
			if (inNote == 0xFF)
				inNote = ch->noteOld;
			
			if (inNote != 0xFF) {
				ch->pitchgoal = GetOldPeriod(inNote, ch->fineTune, intDriver);
			} else if (ch->pitchgoal == 0)
				ch->pitchgoal = ch->period;
		}
			break;
			
		case MADEffectPortaSlide:				// OK
		{
			int inNote = ch->note;
			
			if (inNote == 0xFF)
				inNote = ch->noteOld;
			
			if (inNote != 0xFF) {
				ch->pitchgoal = GetOldPeriod(inNote, ch->fineTune, intDriver);
			} else if (ch->pitchgoal == 0)
				ch->pitchgoal = ch->period;
			
			parse_slidevol(ch, ch->arg);
		}
			break;
			
		case MADEffectVibratoSlide:
			ch->periodOld = ch->period;
			
			parse_slidevol(ch, ch->arg);
			break;
			
		case MADEffectSpeed:
			if (ch->arg < 32) {
				/** Setting de la speed + reset de la finespeed **/
				if (ch->arg != 0) intDriver->base.speed = ch->arg;
			} else {
				/** Setting de finespeed **/
				intDriver->base.finespeed = ch->arg;
			}
			break;
			
		case MADEffectSkip:
			break;
			
		case MADEffectFastSkip:
			break;
			
		case MADEffectLoop:
			if (ch->loopSize > 2) {
				short direction = (char)ch->arg;
				
				if (ch->amp == 16)
					direction *= 2;
				if (ch->stereo == true)
					direction *= 2;
				
				ch->loopBeg += direction;
				
				if (ch->loopBeg >= ch->sizePtr)
					ch->loopBeg = ch->sizePtr;
				
				ch->maxPtr 	= (char*) ((uintptr_t)ch->begPtr + ch->loopBeg + ch->loopSize);
				
				if (ch->maxPtr < ch->begPtr)
					ch->maxPtr = ch->begPtr;
				if (ch->maxPtr > ch->begPtr + ch->sizePtr)
					ch->maxPtr = ch->begPtr + ch->sizePtr;
			}
			break;
			
		case MADEffectNOffset:
			ch->curPtr = ch->begPtr;
			
			aL = (ch->arg * (ch->sizePtr)) / 255U;
			
			if (ch->amp == 16)
				aL /= 2;
			if (ch->stereo == true)
				aL /= 2;
			
			if (ch->amp == 16)
				aL *= 2;
			if (ch->stereo == true)
				aL *= 2;
			
			ch->curPtr += aL;
			
			if (ch->curPtr > ch->maxPtr)
				ch->maxPtr = ch->begPtr + ch->sizePtr;
			else {
				if (ch->loopSize > 2) {
					ch->maxPtr 	= (char*)((uintptr_t)ch->begPtr + ch->loopBeg + ch->loopSize);
				}
			}
			break;
			
		case MADEffectOffset:
			ch->curPtr = ch->begPtr;
			
			aL = ch->arg;
			aL *= 256L;
			
			if (ch->amp == 16)
				aL *= 2;
			if (ch->stereo == true)
				aL *= 2;
			
			ch->curPtr += aL;
			break;
			
		case MADEffectPanning:
			ch->pann = ch->arg;
			
			ch->pann = ((int)ch->pann *  MAX_PANNING) / 0xFF;
			
			if (ch->pann < 0)
				ch->pann = 0;
			else if (ch->pann > MAX_PANNING)
				ch->pann = MAX_PANNING;
			break;
			
		case MADEffectVolume:
			ch->vol = ch->arg;
			
			if (ch->vol < MIN_VOLUME)
				ch->vol = MIN_VOLUME;
			else if (ch->vol > MAX_VOLUME)
				ch->vol = MAX_VOLUME;
			break;
			
		default:
			break;
	}
}

void DoVolCmd(Channel *ch, short call, MADDriverRec *intDriver)
{
	short	vol = ch->volcmd;
	short	volLO = vol & 0xf;
	
	switch (vol >> 4) {
		case 0x6: // volslide down
			ch->vol -= volLO;
			
			if (ch->vol < MIN_VOLUME)
				ch->vol = MIN_VOLUME;
			else if (ch->vol > MAX_VOLUME)
				ch->vol = MAX_VOLUME;
			break;
			
		case 0x7: // volslide up
			ch->vol += volLO;
			
			if (ch->vol < MIN_VOLUME)
				ch->vol = MIN_VOLUME;
			else if (ch->vol > MAX_VOLUME)
				ch->vol = MAX_VOLUME;
			break;
			
			// volume-row fine volume slide is compatible with protracker
			//   EBx and EAx effects i.e. a zero nibble means DO NOT SLIDE, as
			//  opposed to 'take the last sliding value'.
			//
			
		case 0x8: // finevol down
			break;
			
		case 0x9: // finevol up
			break;
			
		case 0xa: // set vibrato speed
			break;
			
		case 0xb: // vibrato
			break;
			
		case 0xc: // set panning
			break;
			
		case 0xd: // pann slide left
			if(vol & 0xf) {
				ch->pann -= volLO/4;
				
				if (ch->pann < 0)
					ch->pann = 0;
				else if (ch->pann > MAX_PANNING)
					ch->pann = MAX_PANNING;
			}
			break;
			
		case 0xe:						// pann slide right
			if(vol & 0xf) {
				ch->pann += volLO / 4;
				
				if (ch->pann < 0)
					ch->pann = 0;
				else if (ch->pann > MAX_PANNING)
					ch->pann = MAX_PANNING;
			}
			break;
			
		case 0xf:						// tone porta
			break;
	}
}
