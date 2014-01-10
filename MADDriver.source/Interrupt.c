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
#ifdef _MAC_H
#include "PPPlug.h"
#endif
#include "PPPrivate.h"
#ifdef _MIDIHARDWARE_
extern short		gOutNodeRefNum, gOutputPortRefNum;
#endif

void SampleMIDI(Channel *curVoice, short channel, short curN, MADDriverRec *intDriver);
void ApplyFilters(MADDriverRec *intDriver);
void ApplySurround(MADDriverRec *intDriver);
void SendMIDIClock(MADDriverRec *intDriver, Byte MIDIByte);
void SendMIDITimingClock(MADDriverRec *intDriver);
void ConvertInstrument(Byte *tempPtr, size_t sSize);

Boolean IsVSTChanEffect(MADDriverRec *, short channel);
void ProcessVisualPlug(MADDriverRec*, short*, SInt32);
void ProcessVSTPlug(MADDriverRec*, SInt32*, SInt32, SInt32);

void ConvertInstrument(Byte *tempPtr, size_t sSize)
{
	Byte val = 0x80;

	while (sSize > 0) {
		sSize--;
		*(tempPtr++) += val;
	}
}

void ConvertInstrumentIn(Byte *tempPtr, size_t sSize)
{
	Byte val = 0x80;

	while (sSize > 0) {
		sSize--;
		*(tempPtr++) -= val;
	}
}

void ConvertInstrument16(short *tempPtr, size_t sSize)
{
	short val = 0x8000;
	
	sSize /= 2;
	
	while (sSize > 0) {
		sSize--;
		*(tempPtr++) += val;
	}
}

void ConvertInstrumentIn16(short *tempPtr, size_t sSize)
{
	short val = 0x8000;
	 
	sSize /= 2;
	 
	while (sSize > 0) {
		 sSize--;
		 *tempPtr++ -= val;
	}
}

void ConvertInstrumentOut16(short *tempPtr, size_t sSize)
{
	ConvertInstrument16(tempPtr, sSize);
}

#if 0
long DoVolPanning(short whichChannel, Channel *ch, MADDriverRec *intDriver)	// MAX = 64
{
	// Compute Volume !
	long pannValue;
	long temp = ( (long) ch->vol * (long) ch->volEnv * (long) ch->volFade) /( 64L*32767L );
	
	if (!intDriver->Active[ ch->ID]) return 0;
	
	if (intDriver->curMusic != NULL) temp = (temp * (long) intDriver->curMusic->header->chanVol[ ch->ID]) / MAX_VOLUME;
	else temp = 64;
	
	// Compute Panning
	
	if (whichChannel != 3)
	{
		pannValue = ch->pannEnv;
		
		if (whichChannel == 1) pannValue = MAX_PANNING - pannValue;
		
		temp = (temp * pannValue) / MAX_PANNING;
	}
	
	// Vol Global
	
	temp = (temp * intDriver->VolGlobal) / (MAX_VOLUME + 20);
	
	if (temp > 64) temp = 64;
	
	return temp;
}
#endif

SInt32 DoVolPanning256(short whichChannel, Channel *ch, MADDriverRec *intDriver, Boolean Interpol)	// MAX = 256
{
	// Compute Volume !
	SInt32	pannValue, volFade;
	SInt32	temp;
	SInt32	volEnv;
	SInt32 	tVSYNC;
	
	if (Interpol) {
		tVSYNC 		= intDriver->VSYNC;
		tVSYNC 		/= intDriver->finespeed;
		tVSYNC 		*= 8000;
		if (intDriver->VExt == 0) {
			tVSYNC = 0;
		} else
			tVSYNC /= intDriver->VExt;
		
		volEnv = Interpolate( ch->volEnvInter, 0, tVSYNC, ch->volEnv*256L, ch->nextvolEnv*256L);
	} else {
		if (!ch->volEnvActive)
			volEnv = ch->nextvolEnv * 256;
		else
			volEnv = 64L * 256;
	}
	
	// REMETTRE LE SOUND WINDOW CENTERING !!!!!!!!
	
	//if (volEnv != ch->volEnv) Debugger();
	
	//if (ch->volEnvInter > tVSYNC) Debugger();
	//if (ch->volEnvInter < 0) Debugger();
	
	if (Interpol) {
		volFade = Interpolate( ch->volEnvInter, 0, tVSYNC, ch->volFade, ch->nextvolFade);
		if (volFade < 0)
			volFade = 0;
	} else
		volFade = 32767;
	
	temp = ((float) ch->vol * (float)volEnv * (float)volFade) /((float)(16*32767));
	
	if (!intDriver->Active[ch->TrackID])
		return 0;
	
	if (intDriver->curMusic != NULL)
		temp = (temp * (SInt32) intDriver->curMusic->header->chanVol[ ch->TrackID]) / MAX_VOLUME;
	else
		temp = 256*256;
	
	// Compute Panning
	
	if (whichChannel != 3) {
		pannValue = ch->pannEnv;
		
		if (Interpol) {
			pannValue = Interpolate( ch->volEnvInter, 0, tVSYNC, ch->pannEnv*256L, ch->nextpannEnv*256L);
		} else {
			if (!ch->pannEnvActive) pannValue = ch->nextpannEnv*256L;
			else pannValue = 64L*256L;
		}
		pannValue += intDriver->globPan*256L - 64L*256L;	//	pannValue * intDriver->globPan) / 64;
		
		if (pannValue < 0 ) pannValue = 0;
		if (pannValue >= MAX_PANNING*256L) pannValue = MAX_PANNING*256L;
		
		if (whichChannel == 1) pannValue = (MAX_PANNING*256L) - pannValue;
		
		temp = (temp * pannValue) / (MAX_PANNING*256L);
	}
	
	// Vol Global
	
	temp = (temp * intDriver->VolGlobal) / (MAX_VOLUME + 40);	// ICI !!! + 30);
	
	return temp;
}

void MADCleanDriver(MADDriverRec *intDriver)
{
	SInt32		i, x/*, size*/;
	
#if 0
	switch( intDriver->DriverSettings.outPutBits)
	{
		case 8:
			for (i = 0; i < intDriver->ASCBUFFER*Tracks; i++)
				intDriver->IntDataPtr[ i] = 0x80;
			break;
			
		case 16:
			break;
	}
#endif
	
	for (i = 0; i < MAXTRACK; i++) {
		intDriver->chan[i].ID		= i;
		//intDriver->chan[i].TrackID	= i;
		
		intDriver->chan[i].begPtr 	= NULL;
		intDriver->chan[i].maxPtr 	= NULL;
		intDriver->chan[i].curPtr 	= NULL;
		intDriver->chan[i].sizePtr 	= 0;
		
		intDriver->chan[i].amp		= 8;
		
		intDriver->chan[i].loopBeg 	= 0;
		intDriver->chan[i].loopSize	= 0;
		
		intDriver->chan[i].ins 		= 0;
		intDriver->chan[i].insOld 	= 0;
		
		intDriver->chan[i].fineTune = NOFINETUNE;
		
		intDriver->chan[i].note		= 0xFF;
		intDriver->chan[i].noteOld	= 0xFF;
		intDriver->chan[i].relNoteOld = 0;
		
		intDriver->chan[i].period	= GetOldPeriod( 40, NOFINETUNE, intDriver);
		intDriver->chan[i].periodOld= GetOldPeriod( 40, NOFINETUNE, intDriver);
		
		intDriver->chan[i].vol		= 64;
		intDriver->chan[i].cmd		= 0;
		intDriver->chan[i].arg		= 0;
		
		for (x = 0; x < MAX_ARP; x++) intDriver->chan[i].arp[ x] = 0;
		intDriver->chan[i].arpindex = 0;
		intDriver->chan[i].arpUse = false;
		
		intDriver->chan[i].viboffset	= 0;
		intDriver->chan[i].vibdepth 	= 0;
		intDriver->chan[i].vibrate 		= 0;
		intDriver->chan[i].vibtype 		= 0;
		intDriver->chan[i].slide 		= 0;
		intDriver->chan[i].pitchgoal 	= 0;
		intDriver->chan[i].pitchrate 	= 0;
		intDriver->chan[i].volumerate 	= 0;
		intDriver->chan[i].samplePtr 	= NULL;
		intDriver->chan[i].volcmd		= 0;
		
		for (x = 0; x < 16; x++) intDriver->chan[ i].oldArg[ x] = 0;
		
		intDriver->chan[i].oldVibdepth = 0;
		intDriver->chan[i].oldVibrate = 0;
		
		intDriver->chan[i].KeyOn		= false;
		intDriver->chan[i].a			= 0;
		intDriver->chan[i].b			= 1;
		intDriver->chan[i].p			= 0;
		intDriver->chan[i].volEnv		= 64;
		intDriver->chan[i].volFade		= 32767;
		intDriver->chan[i].nextvolFade	= 32767;
		intDriver->chan[i].lAC			= 0;
		
		intDriver->chan[i].lastWordR		= 0;
		intDriver->chan[i].lastWordL		= 0;
		intDriver->chan[i].curLevelL		= 0;
		intDriver->chan[i].curLevelR		= 0;
		intDriver->chan[i].prevPtr		= 0;
		intDriver->chan[i].prevVol0		= DoVolPanning256( 0, &intDriver->chan[i], intDriver, false);
		intDriver->chan[i].prevVol1		= DoVolPanning256( 1, &intDriver->chan[i], intDriver, false);
		
		intDriver->chan[i].loopType	= eClassicLoop;
		intDriver->chan[i].pingpong	= false;
		intDriver->chan[i].PanningE8 = false;
		intDriver->chan[i].PatternLoopE6 = 1;
		intDriver->chan[i].PatternLoopE6Count = 0;
		intDriver->chan[i].PatternLoopE6ID = 0;
		intDriver->chan[i].trig			 = 0;
		intDriver->chan[i].preOff			= 0xFFFFFFFF;
		intDriver->chan[i].preVal2		= 0;
		intDriver->chan[i].preVal2R		= 0;
		
		intDriver->chan[i].spreVal2		= 0;
		intDriver->chan[i].spreVal2R	= 0;
		
		intDriver->chan[i].preVal			= 0;
		intDriver->chan[i].spreVal		= 0;
		
		intDriver->chan[i].LevelDirectionR = true;
		intDriver->chan[i].LevelDirectionL = true;
		
		intDriver->chan[i].RemoverWorking		= false;
		intDriver->chan[i].TICKREMOVESIZE		= 1;
		intDriver->chan[i].TimeCounter		= 0;
	}
	
	intDriver->PatDelay = 0;
	intDriver->BufCounter = 0;
	intDriver->BytesToGenerate = 0;
	
	for (i = 0; i < MAXTRACK; i++)
		intDriver->TrackLineReading[i] = true;
	for (i = 0; i < MAXTRACK; i++)
		intDriver->TrackReading[i] = true;
}

SInt32 Interpolate(SInt32 p, SInt32 p1, SInt32 p2, SInt32 v1, SInt32 v2)
{
	SInt32 dp, dv, di;

	if (p1 == p2) return v1;

	dv = v2 - v1;
	dp = p2 - p1;
	di = p - p1;

	return v1 + ((SInt32)(di*dv) / dp);
}

SInt32 InterpolateEnv(SInt32 p, EnvRec *a,EnvRec *b)
{
	if (p < a->pos)
		return a->val;
	return Interpolate(p,a->pos,b->pos,a->val,b->val);
}

void ProcessFadeOut(Channel *ch, MADDriverRec *intDriver)
{
	if (intDriver->curMusic != NULL) {
		if (!ch->KeyOn) {
			if ((intDriver->curMusic->fid[ ch->ins].volType & EFON) && (intDriver->curMusic->fid[ ch->ins].volType & EFSUSTAIN)) {
				if (intDriver->curMusic->fid[ ch->ins].volType & EFLOOP)
					goto FADEOUT;
			}
			else FADEOUT:
			{
				if (intDriver->curMusic->fid[ ch->ins].volFade == 0)
					ch->volFade = 0;
				else
					ch->volFade -= intDriver->curMusic->fid[ ch->ins].volFade;
				
				ch->nextvolFade = ch->volFade - intDriver->curMusic->fid[ ch->ins].volFade;
				
				if (ch->volFade < 0) {
					ch->volFade 	= 0;
					ch->loopBeg 	= 0;
					ch->loopSize 	= 0;
					ch->loopType	= eClassicLoop;
				}
			}
		}
	}
}

void ProcessEnvelope( Channel *ch, MADDriverRec *intDriver, Boolean Recurrent)
{
	SInt32		v;
	InstrData	*curIns;
	
	ch->volEnvActive = false;
	ch->volEnvInter = 0;
	ch->nextvolEnv = ch->volEnv = 64;
	if (ch->ins < 0) return;
	if (ch->samplePtr != NULL) return;
	if (intDriver->curMusic == NULL) return;
	curIns = &intDriver->curMusic->fid[ ch->ins];
	if (curIns->volSize <= 0) return;
	
	if (curIns->volType & EFON) {
		Byte 	a,b;
		float	p;
		
		//  active? -> copy variables
		
		ch->volEnvActive = true;
		
		a = ch->a;
		b = ch->b;
		p = ch->p;
		
		if (curIns->volSize == 1) {		// Just 1 point !
			v = curIns->volEnv[a].val;
			p = curIns->volEnv[a].pos;
		} else {
			float basePeriod = 0.0f;
			sData	*curData;
			
			if ((curIns->volType & EFNOTE)) {
				curData = intDriver->curMusic->sample[ curIns->firstSample + ch->samp];
				if (curData == NULL) //DebugStr( "\pNote Enveloppe curData = NULL");
					return;
				
				basePeriod = GetOldPeriod( 48 + curData->relNote, ch->fineTune, intDriver);
			}
			
			v = InterpolateEnv( p, &curIns->volEnv[a], &curIns->volEnv[b]);
			
			if((curIns->volType & EFSUSTAIN) && ch->KeyOn && a==curIns->volSus && p==curIns->volEnv[a].pos) {
				
			} else {
				if ((curIns->volType & EFNOTE))
					p += basePeriod / (float) ch->period;
				else
					p++;
				
				if(p >= curIns->volEnv[b].pos) {
					a=b; b++;
					
					if(curIns->volType & EFLOOP) {
						if(b > curIns->volEnd) {
							a=curIns->volBeg;
							b=a+1;
							p=curIns->volEnv[a].pos;
						}
					} else {
						if(b >= curIns->volSize) {
							b--;
							if ((curIns->volType & EFNOTE))
								p -= basePeriod / (float) ch->period;
							else
								p--;
						}
					}
				}
			}
		}
		
		ch->a = a;
		ch->b = b;
		ch->p = p;
		
		ch->volEnv = v;
		
		if (b >= curIns->volSize) {	// Check if we are out of the enveloppe with a 0 or 1 value...
			if (curIns->volType & EFLOOP) {
				
			} else {
				if (ch->volEnv < 2) {
					ch->curPtr = ch->maxPtr;
					ch->loopSize = 0;
					ch->loopType	= eClassicLoop;
				}
			}
		}
		
		// Compute next value for linear interpolation during sound processing
		if (Recurrent) {
			ProcessEnvelope(ch, intDriver, false);
			ch->nextvolEnv = ch->volEnv;
			
			
			// Re-Apply old values
			ch->a = a;
			ch->b = b;
			ch->p = p;
			
			ch->volEnv = v;
		}
	}
}

void ProcessPanning(Channel *ch, MADDriverRec *intDriver, Boolean Recurrent)
{
	SInt32		v;
	InstrData	*curIns;
	
	ch->pannEnvActive = false;
	ch->nextpannEnv = ch->pannEnv = ch->pann;
	if (ch->ins < 0)
		return;
	if (ch->samplePtr != NULL)
		return;
	if (intDriver->curMusic == NULL)
		return;
	curIns = &intDriver->curMusic->fid[ ch->ins];
	if (curIns->pannSize <= 0)
		return;
	
	if (curIns->pannType & EFON) {
		//  active? -> copy variables
		
		Byte 	aa,bb;
		float	pp;
		
		aa = ch->aa;
		bb = ch->bb;
		pp = ch->pp;
		
		if (curIns->pannSize == 1) {		// Just 1 point !
			v = curIns->pannEnv[aa].val;
			pp = curIns->pannEnv[aa].pos;
		} else {
			float basePeriod = 0.0f;
			
			ch->pannEnvActive = true;
			
			if ((curIns->volType & EFNOTE)) {
				sData *curData;
				
				curData = intDriver->curMusic->sample[curIns->firstSample + ch->samp];
				if (curData == NULL) //DebugStr( "\pNote Enveloppe curData = NULL");
					return;
				basePeriod = GetOldPeriod(48 + curData->relNote, ch->fineTune, intDriver);
			}
			
			v = InterpolateEnv( pp, &curIns->pannEnv[ aa], &curIns->pannEnv[ bb]);
			
			if ((curIns->pannType & EFNOTE))
				pp += basePeriod / (float) ch->period;
			else
				pp++;
			
			if(pp >= curIns->pannEnv[bb].pos) {
				aa=bb; bb++;
				
				if(curIns->pannType & EFLOOP) {
					if(bb > curIns->pannEnd) {
						aa=curIns->pannBeg;
						bb=aa+1;
						pp=curIns->pannEnv[aa].pos;
					}
				} else {
					if(bb >= curIns->pannSize) {
						bb--;
						if ((curIns->pannType & EFNOTE))
							pp -= basePeriod / (float) ch->period;
						else
							pp--;
					}
				}
			}
		}
		ch->aa=aa;
		ch->bb=bb;
		ch->pp=pp;
		
		ch->pannEnv = v;
		
		// Compute next value for linear interpolation during sound processing
		if (Recurrent) {
			ProcessPanning(ch, intDriver, false);
			ch->nextpannEnv = ch->pannEnv;
			
			
			// Re-Apply old values
			ch->aa = aa;
			ch->bb = bb;
			ch->pp = pp;
			
			ch->pannEnv = v;
		}
	}
}

void StartEnvelope(Channel *ch)
{
	if (!ch)
		return;
	ch->p = 0;
	ch->a = 0;
	ch->b = 1;
}

void StartPanning(Channel *ch)
{
	if (!ch)
		return;
	ch->pp=0;
	ch->aa=0;
	ch->bb=1;
}

static
UInt32 lineartable[ 800] = {
	535232,534749,534266,533784,533303,532822,532341,531861,
	531381,530902,530423,529944,529466,528988,528511,528034,
	527558,527082,526607,526131,525657,525183,524709,524236,
	523763,523290,522818,522346,521875,521404,520934,520464,
	519994,519525,519057,518588,518121,517653,517186,516720,
	516253,515788,515322,514858,514393,513929,513465,513002,
	512539,512077,511615,511154,510692,510232,509771,509312,
	508852,508393,507934,507476,507018,506561,506104,505647,
	505191,504735,504280,503825,503371,502917,502463,502010,
	501557,501104,500652,500201,499749,499298,498848,498398,
	497948,497499,497050,496602,496154,495706,495259,494812,
	494366,493920,493474,493029,492585,492140,491696,491253,
	490809,490367,489924,489482,489041,488600,488159,487718,
	487278,486839,486400,485961,485522,485084,484647,484210,
	483773,483336,482900,482465,482029,481595,481160,480726,
	480292,479859,479426,478994,478562,478130,477699,477268,
	476837,476407,475977,475548,475119,474690,474262,473834,
	473407,472979,472553,472126,471701,471275,470850,470425,
	470001,469577,469153,468730,468307,467884,467462,467041,
	466619,466198,465778,465358,464938,464518,464099,463681,
	463262,462844,462427,462010,461593,461177,460760,460345,
	459930,459515,459100,458686,458272,457859,457446,457033,
	456621,456209,455797,455386,454975,454565,454155,453745,
	453336,452927,452518,452110,451702,451294,450887,450481,
	450074,449668,449262,448857,448452,448048,447644,447240,
	446836,446433,446030,445628,445226,444824,444423,444022,
	443622,443221,442821,442422,442023,441624,441226,440828,
	440430,440033,439636,439239,438843,438447,438051,437656,
	437261,436867,436473,436079,435686,435293,434900,434508,
	434116,433724,433333,432942,432551,432161,431771,431382,
	430992,430604,430215,429827,429439,429052,428665,428278,
	427892,427506,427120,426735,426350,425965,425581,425197,
	424813,424430,424047,423665,423283,422901,422519,422138,
	421757,421377,420997,420617,420237,419858,419479,419101,
	418723,418345,417968,417591,417214,416838,416462,416086,
	415711,415336,414961,414586,414212,413839,413465,413092,
	412720,412347,411975,411604,411232,410862,410491,410121,
	409751,409381,409012,408643,408274,407906,407538,407170,
	406803,406436,406069,405703,405337,404971,404606,404241,
	403876,403512,403148,402784,402421,402058,401695,401333,
	400970,400609,400247,399886,399525,399165,398805,398445,
	398086,397727,397368,397009,396651,396293,395936,395579,
	395222,394865,394509,394153,393798,393442,393087,392733,
	392378,392024,391671,391317,390964,390612,390259,389907,
	389556,389204,388853,388502,388152,387802,387452,387102,
	386753,386404,386056,385707,385359,385012,384664,384317,
	383971,383624,383278,382932,382587,382242,381897,381552,
	381208,380864,380521,380177,379834,379492,379149,378807,
	378466,378124,377783,377442,377102,376762,376422,376082,
	375743,375404,375065,374727,374389,374051,373714,373377,
	373040,372703,372367,372031,371695,371360,371025,370690,
	370356,370022,369688,369355,369021,368688,368356,368023,
	367691,367360,367028,366697,366366,366036,365706,365376,
	365046,364717,364388,364059,363731,363403,363075,362747,
	362420,362093,361766,361440,361114,360788,360463,360137,
	359813,359488,359164,358840,358516,358193,357869,357547,
	357224,356902,356580,356258,355937,355616,355295,354974,
	354654,354334,354014,353695,353376,353057,352739,352420,
	352103,351785,351468,351150,350834,350517,350201,349885,
	349569,349254,348939,348624,348310,347995,347682,347368,
	347055,346741,346429,346116,345804,345492,345180,344869,
	344558,344247,343936,343626,343316,343006,342697,342388,
	342079,341770,341462,341154,340846,340539,340231,339924,
	339618,339311,339005,338700,338394,338089,337784,337479,
	337175,336870,336566,336263,335959,335656,335354,335051,
	334749,334447,334145,333844,333542,333242,332941,332641,
	332341,332041,331741,331442,331143,330844,330546,330247,
	329950,329652,329355,329057,328761,328464,328168,327872,
	327576,327280,326985,326690,326395,326101,325807,325513,
	325219,324926,324633,324340,324047,323755,323463,323171,
	322879,322588,322297,322006,321716,321426,321136,320846,
	320557,320267,319978,319690,319401,319113,318825,318538,
	318250,317963,317676,317390,317103,316817,316532,316246,
	315961,315676,315391,315106,314822,314538,314254,313971,
	313688,313405,313122,312839,312557,312275,311994,311712,
	311431,311150,310869,310589,310309,310029,309749,309470,
	309190,308911,308633,308354,308076,307798,307521,307243,
	306966,306689,306412,306136,305860,305584,305308,305033,
	304758,304483,304208,303934,303659,303385,303112,302838,
	302565,302292,302019,301747,301475,301203,300931,300660,
	300388,300117,299847,299576,299306,299036,298766,298497,
	298227,297958,297689,297421,297153,296884,296617,296349,
	296082,295815,295548,295281,295015,294749,294483,294217,
	293952,293686,293421,293157,292892,292628,292364,292100,
	291837,291574,291311,291048,290785,290523,290261,289999,
	289737,289476,289215,288954,288693,288433,288173,287913,
	287653,287393,287134,286875,286616,286358,286099,285841,
	285583,285326,285068,284811,284554,284298,284041,283785,
	283529,283273,283017,282762,282507,282252,281998,281743,
	281489,281235,280981,280728,280475,280222,279969,279716,
	279464,279212,278960,278708,278457,278206,277955,277704,
	277453,277203,276953,276703,276453,276204,275955,275706,
	275457,275209,274960,274712,274465,274217,273970,273722,
	273476,273229,272982,272736,272490,272244,271999,271753,
	271508,271263,271018,270774,270530,270286,270042,269798,
	269555,269312,269069,268826,268583,268341,268099,267857
};


UInt32 getfrequency(UInt32 period)
{
	return lineartable[period % 768] >> (period / 768);
}

SInt32 GetOld2Period(short note, SInt32 c2spd, MADDriverRec *intDriver)
{
	UInt32 	period, n, o;
	
	if (note == 0xFF || note == 0xFE)
		return 4242;
	
	//if(!c2spd) DebugStr("\pNo c2spd");
	if(!c2spd)
		return 4242;
	
	if (note < 0)
		note = 0;
	
	n = note%12;
	o = note/12;
	
	period = (UInt32) ((UInt32) ( 8363U * ((UInt32) intDriver->lib->mytab[n]) ) >> o ) / (UInt32) c2spd;
	
	if (period == 0)
		period = 7242;
	
	return period;
}

SInt32 getlinearperiod(short note, SInt32 c2spd, MADDriverRec *intDriver)
{
	if (note == 0xFF || note == 0xFE)
		return 4242;
		
	return ((10 * 12 - note) * 64);
}

#define LOGFAC 2*16

static const SInt32 logtab[]={
	LOGFAC*907,LOGFAC*900,LOGFAC*894,LOGFAC*887,LOGFAC*881,LOGFAC*875,LOGFAC*868,LOGFAC*862,
	LOGFAC*856,LOGFAC*850,LOGFAC*844,LOGFAC*838,LOGFAC*832,LOGFAC*826,LOGFAC*820,LOGFAC*814,
	LOGFAC*808,LOGFAC*802,LOGFAC*796,LOGFAC*791,LOGFAC*785,LOGFAC*779,LOGFAC*774,LOGFAC*768,
	LOGFAC*762,LOGFAC*757,LOGFAC*752,LOGFAC*746,LOGFAC*741,LOGFAC*736,LOGFAC*730,LOGFAC*725,
	LOGFAC*720,LOGFAC*715,LOGFAC*709,LOGFAC*704,LOGFAC*699,LOGFAC*694,LOGFAC*689,LOGFAC*684,
	LOGFAC*678,LOGFAC*675,LOGFAC*670,LOGFAC*665,LOGFAC*660,LOGFAC*655,LOGFAC*651,LOGFAC*646,
	LOGFAC*640,LOGFAC*636,LOGFAC*632,LOGFAC*628,LOGFAC*623,LOGFAC*619,LOGFAC*614,LOGFAC*610,
	LOGFAC*604,LOGFAC*601,LOGFAC*597,LOGFAC*592,LOGFAC*588,LOGFAC*584,LOGFAC*580,LOGFAC*575,
	LOGFAC*570,LOGFAC*567,LOGFAC*563,LOGFAC*559,LOGFAC*555,LOGFAC*551,LOGFAC*547,LOGFAC*543,
	LOGFAC*538,LOGFAC*535,LOGFAC*532,LOGFAC*528,LOGFAC*524,LOGFAC*520,LOGFAC*516,LOGFAC*513,
	LOGFAC*508,LOGFAC*505,LOGFAC*502,LOGFAC*498,LOGFAC*494,LOGFAC*491,LOGFAC*487,LOGFAC*484,
	LOGFAC*480,LOGFAC*477,LOGFAC*474,LOGFAC*470,LOGFAC*467,LOGFAC*463,LOGFAC*460,LOGFAC*457,
	LOGFAC*453,LOGFAC*450,LOGFAC*447,LOGFAC*443,LOGFAC*440,LOGFAC*437,LOGFAC*434,LOGFAC*431
};

SInt32 getlogperiod(short note, SInt32 fine, MADDriverRec *intDriver)
{
	Byte n,o;
	SInt32 i;
	
	n = note%12;
	o = note/12;
	i = (n<<3)+(fine>>4);  /* n*8 + fine/16 */
	
	return logtab[ i];
}

SInt32 GetOldPeriod(short note, SInt32 c2spd, MADDriverRec *intDriver)
{
	if (intDriver->XMLinear) {
		SInt32 tempLong;
		
		tempLong = getlinearperiod(note, c2spd, intDriver);
		
		return tempLong;
	}
	
	return GetOld2Period(note, c2spd, intDriver);
}

Boolean NewMADCommand(Cmd *theNoteCmd)
{
	Boolean		result = false;
	Cmd			intCmd = *theNoteCmd;
	
	if (intCmd.ins != 0 && (intCmd.note != 0xFF && intCmd.note != 0xFE)) {
		if (intCmd.cmd != portamentoE && intCmd.cmd != portaslideE) result = true;
	}
	return result;
}

void KillChannel( Channel *curVoice, MADDriverRec *intDriver)
{
	if (!curVoice || !intDriver)
		return;
	curVoice->curPtr	= curVoice->maxPtr;
	curVoice->loopSize	= 0;
	curVoice->loopType	= eClassicLoop;
}

void IntNoteOff(Channel *curVoice, MADDriverRec *intDriver)
{
	if (!curVoice || !intDriver)
		return;
	
	curVoice->KeyOn	= false;
	
	if (intDriver->DriverSettings.driverMode == MIDISoundDriver) {
		if (intDriver->NoteOld[curVoice->ID] != -1)
			NoteOff(intDriver->InstuNoOld[curVoice->ID], intDriver->NoteOld[curVoice->ID], intDriver->VelocityOld[curVoice->ID], intDriver);
		intDriver->NoteOld[curVoice->ID] = -1;
	}
}

void ReadNote(Channel *curVoice, Cmd *theNoteCmd, MADDriverRec *intDriver)
{
	Cmd			intCmd = *theNoteCmd;
	Boolean		ChangedInstru = false;
	
	/********************************************/
	/*        EXTRA small positionning          */
	/********************************************/
	
	if (intCmd.cmd == 0x0E && (intCmd.arg >> 4) == 0x0D) {
		if (intDriver->smallcounter == 0 && !curVoice->GEffect) {
			curVoice->GEffect 	= true;
			curVoice->GPat 		= intDriver->Pat;
			curVoice->GReader 	= intDriver->PartitionReader;
		}
		
		if (intDriver->smallcounter >= (intCmd.arg & 0x0F)) {
			curVoice->GEffect = false;		// <- Continue - Play note NOW !
		} else
			return;							// <- Do it later
	}
	else
		curVoice->GEffect = false;			// <- Continue
	
	/********************************************/
	/* Play a sample sound in priority to music */
	/********************************************/
	if (curVoice->samplePtr != NULL)
		return;
	
	/********************************************/
	
	/********************************************/
	/*        Read command and compute it       */
	/********************************************/
	else if (intCmd.ins != 0 || (intCmd.note != 0xFF && intCmd.note != 0xFE)) {
		/********************************/
		/* Prépare les notes manquantes */
		/********************************/
		
		/********************************/
		
		if (intCmd.note == 0xFF) {
			if (intCmd.ins == curVoice->insOld) {		// RESET ONLY VOLUME
				if(intCmd.cmd != volumeE && intCmd.ins != 0 && curVoice->noteOld != 0xFF) {
					short ins	= intCmd.ins - 1;
					if (ins >= MAXINSTRU)
						ins = MAXINSTRU-1;
					
					if (curVoice->samp < intDriver->curMusic->fid[ins].numSamples) {
						sData	*curData;
						
						curData					= intDriver->curMusic->sample[ intDriver->curMusic->fid[ ins].firstSample + curVoice->samp];
						
						curVoice->vol 			= curData->vol;
						if (curVoice->vol > MAX_VOLUME)
							curVoice->vol = MAX_VOLUME;
						curVoice->volFade		= 32767;
						curVoice->nextvolFade	= 32767;
					}
				} else {
					curVoice->volFade		= 32767;
					curVoice->nextvolFade	= 32767;
				}
			} else {
				intCmd.note = curVoice->noteOld - curVoice->relNoteOld;
			}
		} else {
			curVoice->noteOld 		= intCmd.note;
			
			if (intCmd.ins != 0) {
				short ins	= intCmd.ins - 1;
				if (ins >= MAXINSTRU)
					ins = MAXINSTRU-1;
				
				if (curVoice->samp < intDriver->curMusic->fid[ ins].numSamples) {
					sData	*curData;
					
					curData					= intDriver->curMusic->sample[intDriver->curMusic->fid[ins].firstSample + curVoice->samp];
					curVoice->noteOld		= intCmd.note + curData->relNote;
					curVoice->relNoteOld	= curData->relNote;
				}
			}
		}
		/********************************/
		
		if (intCmd.ins == 0)
			intCmd.ins = curVoice->insOld;
		else {
			if (intCmd.ins != curVoice->insOld)
				ChangedInstru = true;
			curVoice->insOld = intCmd.ins;
		}
		
		/********************************/
		
		if (intCmd.ins != 0 && (intCmd.note != 0xFF && intCmd.note != 0xFE)) {	//intCmd.note -- theNoteCmd
			sData	*curData;
			short	ins, samp;
			
			/**** INSTRUMENT ****/
			
			ins						= intCmd.ins - 1;
			if (ins >= MAXINSTRU)
				ins = MAXINSTRU-1;
			samp					= intDriver->curMusic->fid[ ins].what[ intCmd.note];
			
			if (intDriver->DriverSettings.driverMode == MIDISoundDriver)
				curVoice->ins			= ins;
			
			if (samp < intDriver->curMusic->fid[ ins].numSamples) {
				
				curData					= intDriver->curMusic->sample[ intDriver->curMusic->fid[ ins].firstSample + samp];
				
				if (theNoteCmd->note != 0xFF) {
					curVoice->ins			= ins;
					curVoice->amp			= curData->amp;
					curVoice->stereo		= curData->stereo;
					curVoice->samp			= samp;
					curVoice->loopType		= curData->loopType;
					
					/**** RESET NOTE ****/
					
					if (ChangedInstru == true || (intCmd.cmd != portamentoE && intCmd.cmd != portaslideE)) {
						curVoice->prevPtr		= NULL;
						curVoice->maxPtr 		= curVoice->curPtr = curVoice->begPtr = curData->data;
						curVoice->maxPtr 		+= curData->size;
						curVoice->sizePtr		= curData->size;
						curVoice->lAC			= 0;
						curVoice->pingpong	= false;
						//intDriver->chan[i].PanningE8
						curVoice->preOff		= 0xFFFFFFFF;
						curVoice->preVal		= 0;
						curVoice->spreVal		= 0;
						curVoice->preVal2		= *curVoice->curPtr;
						if (curVoice->amp == 8)
							curVoice->preVal2R	= *(curVoice->curPtr+1);
						else
							curVoice->preVal2R	= *(curVoice->curPtr+2);
						curVoice->spreVal2	= *(short*) curVoice->curPtr;
						curVoice->spreVal2R	= *(short*) (curVoice->curPtr+2);
						
						if (curData->loopSize > 2) {
							curVoice->loopBeg 	= curData->loopBeg;
							curVoice->loopSize	= curData->loopSize;
							curVoice->maxPtr 	= (char*) ((size_t) curData->data + curData->loopBeg + curData->loopSize);
						} else {
							curVoice->loopBeg 	= 0;
							curVoice->loopSize	= 0;
							curVoice->loopType	= eClassicLoop;
						}
						if (theNoteCmd->note != 0xFF && theNoteCmd->note != 0xFE)
							curVoice->viboffset = 0;
						
						if(intCmd.cmd != panningE) {
							if (curVoice->PanningE8 == false) {
								curVoice->pann = intDriver->curMusic->header->chanPan[curVoice->TrackID];
								if (curVoice->pann > MAX_PANNING)
									curVoice->pann = MAX_PANNING;
							}
						}
					}
					
					
					StartPanning( curVoice);
					
					StartEnvelope( curVoice);
				}
				
				//StartEnvelope( curVoice);
				
				if(intCmd.cmd != volumeE && theNoteCmd->ins != 0) {
					curVoice->vol 			= curData->vol;
					if (curVoice->vol > MAX_VOLUME)
						curVoice->vol = MAX_VOLUME;
					curVoice->volFade		= 32767;
					curVoice->nextvolFade	= 32767;
				} else {
					curVoice->volFade		= 32767;
					curVoice->nextvolFade	= 32767;
				}
				intDriver->InstruTube[ins] 		= 64;
				intDriver->InstruActif[ins] 	= curVoice->TrackID;
			}
		}
		
		if (intCmd.note != 0xFF && intCmd.note != 0xFE) {	//intCmd.note
			/**** NOTE & PERIOD ****/
			
			sData	*curData;
			short	samp;
			
			samp					= intDriver->curMusic->fid[ curVoice->ins].what[ intCmd.note];
			if (samp < intDriver->curMusic->fid[ curVoice->ins].numSamples) {
				curData					= intDriver->curMusic->sample[ intDriver->curMusic->fid[ curVoice->ins].firstSample + samp];
				
				curVoice->note			= intCmd.note + curData->relNote;
				curVoice->fineTune		= curData->c2spd;
				curVoice->KeyOn			= true;
				
				if (intCmd.cmd != portamentoE && intCmd.cmd != portaslideE) {
					curVoice->period	= GetOldPeriod(curVoice->note, curVoice->fineTune, intDriver);
					curVoice->periodOld	= curVoice->period;
				}
			}
			
			/***********************/
			/* Pour le MIDI Driver */
			/***********************/
			if (intDriver->DriverSettings.driverMode == MIDISoundDriver) {
				if (intDriver->NoteOld[curVoice->ID] != -1) {
					NoteOff(intDriver->InstuNoOld[curVoice->ID], intDriver->NoteOld[curVoice->ID], intDriver->VelocityOld[curVoice->ID], intDriver);
					intDriver->NoteOld[ curVoice->ID] = -1;
				}
				
				SampleMIDI( curVoice, curVoice->ins, intCmd.note, intDriver);
				
				intDriver->InstuNoOld[ curVoice->ID]	= curVoice->ins;
				intDriver->NoteOld[ curVoice->ID]		= intCmd.note;
				intDriver->VelocityOld[ curVoice->ID]	= curVoice->vol;
			}
			/***********************/
		}
	} else {
		//curVoice->note = 0xFF;
	}
	
	/**************/
	/*   VOLUME   */
	/**************/
	if (intCmd.vol != 0xFF) {
		if(intCmd.vol >= 0x10 && intCmd.vol <= 0x50) {
			curVoice->vol = intCmd.vol - 0x10;
			if (curVoice->vol < MIN_VOLUME)
				curVoice->vol = MIN_VOLUME;
			else if (curVoice->vol > MAX_VOLUME)
				curVoice->vol = MAX_VOLUME;
			
			curVoice->volcmd = 0;
		} else
			curVoice->volcmd = intCmd.vol;
	} else
		curVoice->volcmd = 0;
	
	/*curVoice->cmd		= intCmd.cmd;
	 curVoice->arg 		= intCmd.arg;
	 
	 SetUpCmdEffect( curVoice, intDriver);
	 SetUpEffect( curVoice, intDriver);*/
	
	if (intCmd.ins != 0 && intCmd.note != 0xFF)
		intDriver->Tube[curVoice->TrackID] = curVoice->vol;
	
	
	/**************/
	/*   KEY OFF  */
	/**************/
	if (intCmd.note == 0xFE)
		IntNoteOff(curVoice, intDriver);
}

void StartEffect(Channel *curVoice, Cmd *theNoteCmd, MADDriverRec *intDriver)
{
	if (!curVoice || !theNoteCmd || !intDriver)
		return;
	curVoice->cmd = theNoteCmd->cmd;
	curVoice->arg = theNoteCmd->arg;
	
	SetUpCmdEffect(curVoice, intDriver);
	SetUpEffect(curVoice, intDriver);
}

void ComputeReverb8(Byte *orgPtr, Byte *destPtr, SInt32 xx, SInt32 strength)
{
	SInt32	temp1;
	
	while (xx-- > 0) {
		temp1 = (*destPtr) + ((strength * (*orgPtr++ - 0x80)) / 100);		// - 0x8NULL
		
		if (temp1 > 0xFF)
			temp1 = 0xFF;	// overflow ?
		else if (temp1 < 0)
			temp1 = 0;
		
		*(destPtr)++ = temp1;
	}
}

void ComputeReverb16(short *orgPtr, short *destPtr, SInt32 xx, SInt32 strength)
{
	SInt32	temp1;
	SInt32	valP = 0x7FFF, valN = -0x7FFF;
	
	while (xx-- > 0) {
		temp1 = *destPtr + ((strength * (SInt32) *orgPtr++) / 100);
		
		if (temp1 > valP)
			temp1 = valP;	// overflow ?
		else if (temp1 < valN )
			temp1 = valN;
		
		*destPtr++ = temp1;
	}
}

short FindAFreeChannel(MADDriverRec *intDriver)
{
	short		i;
	Channel		*curVoice;
	short		chanID = -1;
	SInt32		oldTick = 1000000; //FIXME: largest SInt32 value here?
	
	for (i = 0; i < intDriver->MultiChanNo; i++) {
		curVoice = &intDriver->chan[ i];
		
		if (curVoice->curPtr >= curVoice->maxPtr && curVoice->loopSize == 0) {
			chanID = i;
			i = intDriver->MultiChanNo;
		}
	}
	
	if (chanID == -1) {	// Find the oldest channel
		chanID = 0;
		
		for (i = 0; i < intDriver->MultiChanNo; i++) {
			curVoice = &intDriver->chan[ i];
			
			if (curVoice->eventTime < oldTick) {
				chanID = i;
				oldTick = curVoice->eventTime;
			}
		}
	}
	
	return chanID;
}

void ApplyVSTEffects( MADDriverRec *intDriver, Boolean ByPass)
{
	SInt32 i;
	if (!intDriver) return;

	if (intDriver->curMusic != NULL) {
		if (intDriver->curMusic->header != NULL) {
			
		}
		else
			return;
	}
	else
		return;
	
	for ( i = 0; i < MAXCHANEFFECT; i++) {
		if (intDriver->EffectBufferID[ i] != -1) {
			if (intDriver->curMusic->header->chanBus[ intDriver->EffectBufferID[ i]].ByPass == ByPass) {
				SInt32		*eee, *ASCBuffer, ii;
				
				if (intDriver->DriverSettings.oversampling > 1) {
					SInt32	*intempLong, mul = intDriver->DriverSettings.oversampling, x, tempL, tempR;
					
					eee = intDriver->DASCEffectBuffer[ i];
					
					for (ii = 0; ii < intDriver->ASCBUFFER / mul; ii++) {
						// LEFT & RIGHT
						
						tempL = 0;
						tempR = 0;
						intempLong = &eee[ ii*mul*2];
						
						x = mul;
						while (x-- > 0) {
							tempL += *(intempLong++);
							tempR += *(intempLong++);
						}
						eee[ii * 2] = tempL / mul;
						eee[ii * 2 + 1] = tempR / mul;
					}
				}
				
				// APPLY VST - EFFECTS
				
#if defined( MAINPLAYERPRO)
				//TODO: Apply VST effects
				if (intDriver->currentlyExporting)
				{
					if (intDriver->thisExport) ProcessVSTPlug( intDriver, (SInt32*) intDriver->DASCEffectBuffer[ i], intDriver->ASCBUFFERReal, intDriver->EffectBufferRealID[ i]);
				}
				else ProcessVSTPlug( intDriver, (SInt32*) intDriver->DASCEffectBuffer[ i], intDriver->ASCBUFFERReal, intDriver->EffectBufferRealID[ i]);
#endif
				
				
				// *** *** *** *** ***
				// ADD IT TO THE OUTPUT BUFFER
				
				eee = intDriver->DASCEffectBuffer[ i];
				ASCBuffer = intDriver->DASCBuffer;
				
				ii = intDriver->ASCBUFFERReal*2;
				while (ii-- > 0) {
					*ASCBuffer++ += *eee++;
				}
			}
		}
	}
}

void NoteAnalyse(MADDriverRec *intDriver)
{
	SInt32		InterruptBufferSize, i, ASCBUFFERCopy, ASCBUFFERRealCopy;
	void		*DataPtrCopy;
	SInt32		tVSYNC;
	Boolean		NoteReading;
	SInt32		*DASCopy, *DASECopy[MAXCHANEFFECT];
	short		*DASCopy8;
	
	if (intDriver->curMusic != NULL){
		if (intDriver->curMusic->musicUnderModification) {	// SILENCE
			SInt32 Tracks;
			
			switch( intDriver->DriverSettings.outPutMode)
			{
				case PolyPhonic:
					Tracks = intDriver->DriverSettings.numChn;
					break;
					
				default:
					Tracks = 2;
					break;
			}
			
			Tracks = 1;
			
			switch(intDriver->DriverSettings.outPutBits)
			{
				case 8:
					for (i = 0; i < intDriver->ASCBUFFER*Tracks; i++)
						intDriver->IntDataPtr[i] = 0x80;
					break;
					
				case 16:
					DASCopy8 = (short*) intDriver->IntDataPtr;
					for (i = 0; i < intDriver->ASCBUFFER*Tracks*2L; i++)
						DASCopy8[i] = 0;
					break;
			}
			return;
		}
	}
	
	intDriver->BytesToRemoveAtEnd = 0;
	
	DataPtrCopy					= intDriver->IntDataPtr;
	DASCopy						= intDriver->DASCBuffer;
	for (i = 0; i < MAXCHANEFFECT; i++)
		DASECopy[i] = intDriver->DASCEffectBuffer[i];
	DASCopy8					= intDriver->DASCBuffer8;
	ASCBUFFERCopy				= intDriver->ASCBUFFER;
	ASCBUFFERRealCopy			= intDriver->ASCBUFFERReal;
	InterruptBufferSize			= intDriver->ASCBUFFER;
	
	while (InterruptBufferSize > 0) {
		/********************/
		/* Sound Generating */
		/********************/
		
		intDriver->ASCBUFFER = intDriver->BytesToGenerate - intDriver->BufCounter;
		
		if (intDriver->ASCBUFFER < 0)
			intDriver->ASCBUFFER = 0;
		if (intDriver->ASCBUFFER > InterruptBufferSize) {
			intDriver->ASCBUFFER = InterruptBufferSize;
			NoteReading = false;
		} else
			NoteReading = true;
		
		if (intDriver->ASCBUFFER > 0) {
			GenerateSound( intDriver);
			intDriver->BufCounter	+= intDriver->ASCBUFFER;
			InterruptBufferSize		-= intDriver->ASCBUFFER;
		}
		
		intDriver->ASCBUFFERReal = intDriver->ASCBUFFER / intDriver->DriverSettings.oversampling;
		
		/**************************/
		/* Note & Effect Analyser */
		/**************************/
		
		if (!NoteReading)
			InterruptBufferSize = -1;
		else {
			NoteReading = false;
			
			/*********/
			// GEffect : extrasmallpositionning
			
			if (intDriver->curMusic != NULL && intDriver->Reading) {
				for (i = 0; i < intDriver->curMusic->header->numChn; i++) {
					if (intDriver->chan[ i].GEffect) {
						if (intDriver->Reading && intDriver->OneMoreBeforeEnd == false && intDriver->TrackLineReading[i]) {
							intDriver->chan[i].TrackID = i;
							if (intDriver->TrackReading[i]) {
								ReadNote(&intDriver->chan[i], GetMADCommand(intDriver->chan[i].GReader, i, intDriver->curMusic->partition[intDriver->chan[i].GPat]), intDriver);
							}
							
							StartEffect(&intDriver->chan[i], GetMADCommand(intDriver->chan[i].GReader, i, intDriver->curMusic->partition[intDriver->chan[i].GPat]), intDriver);
						}
					}
				}
			}
			
			/*********/
			
			//intDriver->extrasmallcounter--;
			//if (intDriver->extrasmallcounter <= 0)
			{
				//intDriver->extrasmallcounter = EXTRASMALLCOUNTER;
				
				intDriver->smallcounter++;
				if (intDriver->smallcounter >= intDriver->speed) {		// NEW LINE
					intDriver->smallcounter = 0;
					
					if (intDriver->PatDelay)
						intDriver->PatDelay--;
					
					if (intDriver->curMusic != NULL && intDriver->PatDelay == 0) {
						for (i = 0; i < intDriver->MultiChanNo; i++)		//intDriver->curMusic->header->numChn
							CloseEffect(&intDriver->chan[i], intDriver->smallcounter, intDriver);
						
						if (intDriver->OneMoreBeforeEnd == true) {
							intDriver->PL = 0;
							intDriver->Pat = intDriver->curMusic->header->oPointers[ intDriver->PL];
							
							intDriver->musicEnd = true;
							intDriver->OneMoreBeforeEnd = false;
							
							MADCleanDriver( intDriver);
							if (!intDriver->DriverSettings.repeatMusic) intDriver->Reading = false;
							
							tVSYNC =	intDriver->VSYNC;
							tVSYNC /=	intDriver->finespeed;
							tVSYNC *=	8000;
							tVSYNC /=	intDriver->VExt;
							
							intDriver->BytesToRemoveAtEnd = (InterruptBufferSize) / intDriver->DriverSettings.oversampling;
							if (intDriver->BytesToRemoveAtEnd < 0)
								intDriver->BytesToRemoveAtEnd = 0;
						}
						
						// *** *** *** *** *** *** *** *** *** *** *** *** *** *** *** *** *** *** *** *** *** *** *** *** *** ***
						// MULTI-CHANNEL MODE *** *** *** *** *** *** *** *** *** *** *** *** *** *** *** *** *** *** *** *** *** ***
						// *** *** *** *** *** *** *** *** *** *** *** *** *** *** *** *** *** *** *** *** *** *** *** *** *** ***
						
						if (intDriver->curMusic->header->MultiChan && intDriver->DriverSettings.outPutMode != PolyPhonic) {
							// Is a new Command? Else assign it to the last Channel used for this track
							
							for (i = 0; i < intDriver->curMusic->header->numChn; i++) {
								if (intDriver->Reading && intDriver->OneMoreBeforeEnd == false && intDriver->TrackLineReading[i]) {
									Cmd *tempcmd = GetMADCommand(intDriver->PartitionReader, i, intDriver->curMusic->partition[intDriver->Pat]);
									
									if (tempcmd->cmd == noteoffE)	// G-Effet, Note-Off previous channel note
									{
										switch(tempcmd->arg)
										{
											case 0:
												KillChannel(&intDriver->chan[intDriver->lastChannelUsed[i]], intDriver);
												break;
												
											case 1:
												IntNoteOff(&intDriver->chan[intDriver->lastChannelUsed[i]], intDriver);
												break;
										}
									}
									
									if (NewMADCommand(tempcmd))
									{
										short chanID;		// Find a free channel
										
										chanID = FindAFreeChannel(intDriver);
										
										intDriver->lastChannelUsed[i] = chanID;
										
										intDriver->chan[chanID].TrackID = i;
										intDriver->chan[chanID].TimeCounter = 100;
										
										if (intDriver->TrackReading[i])
										{
											ReadNote(&intDriver->chan[chanID], tempcmd, intDriver);
										}
										StartEffect(&intDriver->chan[chanID], tempcmd, intDriver);
									}
									else
									{
										//intDriver->chan[ intDriver->lastChannelUsed[ i]].TrackID = i;
										if (intDriver->TrackReading[i])
										{
											ReadNote(&intDriver->chan[intDriver->lastChannelUsed[i]], tempcmd, intDriver);
										}
										StartEffect(&intDriver->chan[intDriver->lastChannelUsed[i]], tempcmd, intDriver);
									}
								}
							}
						}
						else
						{
							for (i = 0; i < intDriver->curMusic->header->numChn; i++)
							{
								if (intDriver->Reading && intDriver->OneMoreBeforeEnd == false && intDriver->TrackLineReading[ i])
								{
									intDriver->chan[i].TrackID = i;
									if (intDriver->TrackReading[i])
									{
										ReadNote(&intDriver->chan[i], GetMADCommand(intDriver->PartitionReader, i, intDriver->curMusic->partition[intDriver->Pat]), intDriver);
									}
									StartEffect(&intDriver->chan[i], GetMADCommand(intDriver->PartitionReader, i, intDriver->curMusic->partition[intDriver->Pat]), intDriver);
								}
							}
						}
						
						for (i = 0; i < intDriver->MultiChanNo; i++)	//intDriver->curMusic->header->numChn
						{
							ProcessEnvelope( &intDriver->chan[i], intDriver, true);
							ProcessPanning( &intDriver->chan[i], intDriver, true);
							ProcessFadeOut( &intDriver->chan[i], intDriver);
						}
						
						if (intDriver->Reading)
						{
							if (intDriver->wasReading == false)
							{
#ifdef _MIDIHARDWARE_
								//if (intDriver->SendMIDIClockData) SendMIDIClock( intDriver, 0xFB);
								if (intDriver->PartitionReader == 0 && intDriver->PL == 0) SendMIDIClock( intDriver, 0xFA);
								else SendMIDIClock( intDriver, 0xFB);
#endif
								intDriver->wasReading = true;
							}
							
#ifdef _MIDIHARDWARE_
							if (intDriver->SendMIDIClockData)
							{
								SendMIDIClock( intDriver, 0xF8);
								SendMIDITimingClock( intDriver);
							}
#endif
							
							for (i = 0; i < MAXTRACK; i++) intDriver->TrackLineReading[ i] = true;
							
							if (intDriver->PartitionReader == 0)
							{
								for (i = 0; i < intDriver->MultiChanNo; i++)	//intDriver->curMusic->header->numChn
								{
									intDriver->chan[ i].PatternLoopE6 = 1;
									intDriver->chan[ i].PatternLoopE6ID = intDriver->PL;
								}
							}
							
							intDriver->PartitionReader++;
							if (intDriver->PartitionReader >= intDriver->curMusic->partition[ intDriver->Pat]->header.size)
							{
								intDriver->PartitionReader = 0;
								intDriver->endPattern = true;
								
								if (intDriver->JumpToNextPattern)
								{
									intDriver->PL++;
									intDriver->Pat = intDriver->curMusic->header->oPointers[ intDriver->PL];
									
									if (intDriver->speed == 1 && intDriver->PL >= intDriver->curMusic->header->numPointers)
									{
										intDriver->PL = 0;
										intDriver->Pat = intDriver->curMusic->header->oPointers[ intDriver->PL];
										
#if 0
										MADCleanDriver( intDriver);
										if (!intDriver->DriverSettings.repeatMusic) intDriver->Reading = false;
										
										tVSYNC 		= intDriver->VSYNC;
										tVSYNC 		/= intDriver->finespeed;
										tVSYNC 		*= 800NULL;
										tVSYNC 		/= intDriver->VExt;
										
										intDriver->BytesToRemoveAtEnd = (InterruptBufferSize - tVSYNC) / intDriver->DriverSettings.oversampling;	//
										if (intDriver->BytesToRemoveAtEnd < 0) intDriver->BytesToRemoveAtEnd = 0;
										intDriver->BytesToRemoveAtEnd = 0;
#endif
										
										intDriver->OneMoreBeforeEnd = true;
									}
								}
							}
						}
						else
						{
							
							if (intDriver->wasReading == true)
							{
#ifdef _MIDIHARDWARE_
								if (intDriver->SendMIDIClockData) SendMIDIClock( intDriver, 0xFC);
#endif
								intDriver->wasReading = false;
							}
						}
					}
				}
				else		// SMALLCOUNTER
				{
					if (intDriver->curMusic != NULL)
					{
						for (i = 0 ; i < intDriver->MultiChanNo; i++)		//intDriver->DriverSettings.numChn
						{
							//if (intDriver->Reading)
							{
								DoVolCmd( &intDriver->chan[ i], intDriver->smallcounter, intDriver);
								DoEffect( &intDriver->chan[ i], intDriver->smallcounter, intDriver);
							}
							ProcessEnvelope( &intDriver->chan[ i], intDriver, true);
							ProcessPanning( &intDriver->chan[ i], intDriver, true);
							ProcessFadeOut( &intDriver->chan[ i], intDriver);
						}
						
						if (intDriver->Reading)
						{
#if 0
							if (intDriver->wasReading == false)
							{
#ifdef _MIDIHARDWARE_
								if (intDriver->SendMIDIClockData)
								{
									if (intDriver->PartitionReader == 0 && intDriver->PL == 0) SendMIDIClock( intDriver, 0xFA);
									else SendMIDIClock( intDriver, 0xFB);
								}
#endif
								intDriver->wasReading = true;
							}
#endif
							
#ifdef _MIDIHARDWARE_
							if (intDriver->SendMIDIClockData) SendMIDIClock( intDriver, 0xF8);
#endif
							
							if (intDriver->smallcounter == intDriver->speed - 1)
							{
								if (intDriver->PL >= intDriver->curMusic->header->numPointers)
								{
									intDriver->OneMoreBeforeEnd = true;
									//	intDriver->musicEnd = true;
								}
							}
						}
						else
						{
							if (intDriver->wasReading == true)
							{
#ifdef _MIDIHARDWARE_
								if (intDriver->SendMIDIClockData) SendMIDIClock( intDriver, 0xFC);
#endif
								intDriver->wasReading = false;
							}
						}
					}
				}
			}
			tVSYNC 		= intDriver->VSYNC;
			tVSYNC 		/= intDriver->finespeed;
			
			tVSYNC 		*= 8000;
			if (intDriver->VExt == 0) {
				
			}
			else tVSYNC 		/= intDriver->VExt;
			
			intDriver->BytesToGenerate += tVSYNC;
		}
	}
	
	intDriver->ASCBUFFER		= ASCBUFFERCopy;
	intDriver->ASCBUFFERReal	= ASCBUFFERRealCopy;
	intDriver->IntDataPtr		= DataPtrCopy;
	intDriver->DASCBuffer		= DASCopy;
	for (i = 0; i < MAXCHANEFFECT; i++) intDriver->DASCEffectBuffer[ i]	= DASECopy[ i];
	intDriver->DASCBuffer8		= DASCopy8;
	
	//  ***** ***** ***** ***** ***** ***** ***** ***** ***** ***** ***** ***** *****
	//  ***** ***** ***** ***** ***** ***** ***** ***** ***** ***** ***** ***** *****
	//  ***** ***** ***** ***** ***** ***** ***** ***** ***** ***** ***** ***** *****
	//  ***** ***** ***** ***** ***** ***** ***** ***** ***** ***** ***** ***** *****
	
	// APPLY OVERSAMPLING
	
	if (intDriver->DriverSettings.oversampling > 1)
	{
		switch( intDriver->DriverSettings.outPutBits)
		{
			case 8:
			{
				short		*tempShortIn = (short*) intDriver->DASCBuffer8, *intempShort;
				SInt32		mul = intDriver->DriverSettings.oversampling;
				SInt32		x, tempL, tempR;
				
				for (i = 0; i < intDriver->ASCBUFFER / mul; i++) {
					// LEFT & RIGHT
					tempL = 0;
					tempR = 0;
					intempShort = &tempShortIn[i * mul * 2];
					x = mul;
					while (x-- > 0)	{
						tempL += *(intempShort++);
						tempR += *(intempShort++);
					}
					tempShortIn[i * 2] = tempL / mul;
					tempShortIn[i * 2 + 1] = tempR / mul;
				}
			}
				break;
				
			case 16:
			{
				SInt32		*tempLongIn = (SInt32*) intDriver->DASCBuffer, *intempLong;
				SInt32		mul = intDriver->DriverSettings.oversampling;
				SInt32		x, tempL, tempR;
				
				for (i = 0; i < intDriver->ASCBUFFER / mul; i++) {
					// LEFT & RIGHT
					
					tempL = 0;	tempR = 0;	intempLong = &tempLongIn[ i*mul*2];
					x = mul;
					while (x-- > 0) {
						tempL += *(intempLong++);
						tempR += *(intempLong++);
					}
					tempLongIn[i * 2] = tempL / mul;
					tempLongIn[i * 2 + 1] = tempR / mul;
				}
			}
				break;
		}
	}
	
	// En cas de channel effects:
	
	// Applique l'effect, puis l'additionne au DASCBuffer
	
	if (intDriver->DriverSettings.outPutBits == 16) {
		ApplyVSTEffects(intDriver, false);
	}
	
	// *** *** *** *** *** *** *** *** *** *** *** *** *** *** *** *** *** *** *** ***
	
#if defined( MAINPLAYERPRO)
	//TODO: Process VST Plug-in
	if (intDriver->DriverSettings.outPutBits == 16)
	{
		if (intDriver->currentlyExporting)
		{
			if (intDriver->thisExport)
			{
				ProcessVSTPlug( intDriver, (SInt32*) intDriver->DASCBuffer, intDriver->ASCBUFFERReal, -1);
			}
		}
		else ProcessVSTPlug( intDriver, (SInt32*) intDriver->DASCBuffer, intDriver->ASCBUFFERReal, -1);
	}
#endif
	
	// *** *** *** *** *** *** *** *** *** *** *** *** *** *** *** *** *** *** *** ***
	
	if (intDriver->DriverSettings.outPutBits == 16) {
		ApplyVSTEffects(intDriver, true);
	}
	
	// Sound Window Centering ! AND compute LEVELS ***** ***** ***** ***** ***** ***** *****
	if (intDriver->DriverSettings.outPutBits == 16) {
		SInt32		maxL, maxR, minL, minR, tempL;
		SInt32		*tempLong = intDriver->DASCBuffer, centerL, centerR;
		
		switch (intDriver->DriverSettings.outPutBits)
		{
			case 16:
				maxL = maxR = -300000;
				minL = minR = 300000;
				
				for (i = 0; i < intDriver->ASCBUFFERReal; i++) {
					tempL = tempLong[i * 2];
					if (tempL < minL)
						minL = tempL;
					else if (tempL > maxL)
						maxL = tempL;
					
					tempL = tempLong[ i*2 + 1];
					if (tempL < minR) minR = tempL;
					else if (tempL > maxR) maxR = tempL;
				}
				
				intDriver->levelL = maxL - minL;
				intDriver->levelR = maxR - minR;
				if (intDriver->levelL < 0) intDriver->levelL = 0;
				if (intDriver->levelL > 0xFFFF) intDriver->levelL = 0xFFFF;
				if (intDriver->levelR < 0) intDriver->levelR = 0;
				if (intDriver->levelR > 0xFFFF) intDriver->levelR = 0xFFFF;
				
				
				centerL = minL + (maxL-minL)/2;
				centerR = minR + (maxR-minR)/2;
				
				centerL -= intDriver->curCenterL;
				centerR -= intDriver->curCenterR;
				
			{
				SInt32	curL, curR, add, addR;
				double temp;
				
				curR = intDriver->curCenterR;
				curL = intDriver->curCenterL;
				
				tempLong = intDriver->DASCBuffer;
				tempL = intDriver->ASCBUFFERReal;
				
				
				temp = (double) centerL / (double) tempL;
				
				add = temp * (1 << BYTEDIV);
				addR = curL << BYTEDIV;
				
				for (i = 0; i < tempL; i++) {
					//	*tempLong -= (i * centerL)/tempL + curL;
					addR += add;
					*tempLong -= addR >> BYTEDIV;
					tempLong += 2;
				}
				
				
				tempLong = intDriver->DASCBuffer + 1;
				tempL = intDriver->ASCBUFFERReal;
				
				temp = (double) centerR / (double) tempL;
				
				add = temp * (1 << BYTEDIV);
				addR = curR << BYTEDIV;
				
				for (i = 0; i < tempL; i++) {
					//*tempLong		-= (i * centerR)/tempL + curR;
					addR += add;
					*tempLong -= addR >> BYTEDIV;
					tempLong += 2;
				}
			}
				
				intDriver->curCenterL = minL + (maxL-minL)/2;
				intDriver->curCenterR = minR + (maxR-minR)/2;
				break;
		}
	}
	
	// Convert 32 bytes data to 16 bits !***** ***** ***** ***** ***** ***** *****
	if (intDriver->DriverSettings.outPutBits == 8) {
		if (intDriver->DriverSettings.outPutMode != PolyPhonic) {
			short			*ttt;
			char			*ASCBuffer;
			
			ttt = intDriver->DASCBuffer8;
			ASCBuffer = intDriver->IntDataPtr;
			
			i = intDriver->ASCBUFFER*2;
			while (i-- > 0) {
				*ASCBuffer++ = *(intDriver->OverShoot + *ttt);
				*ttt++ = 0;
			}
		}
	} else {
		short			*ASCBuffer, valP = 0x7FFFL, valN = -0x7FFFL;
		SInt32			*ttt;
		SInt32			tempL;
		
		intDriver->clipL = false;
		intDriver->clipR = false;
		
		ttt = intDriver->DASCBuffer;
		ASCBuffer = (short*) intDriver->IntDataPtr;
		
		i = intDriver->ASCBUFFERReal;
		// Left
		while (i-- > 0) {
			// Compute Clipping
			tempL = *ttt;
			//	*ttt = 0;
			ttt += 2;
			
			if (tempL > valP) {
				*ASCBuffer = valP;
				intDriver->clipL = true;
			} else if (tempL < valN) {
				*ASCBuffer = valN;
				intDriver->clipL = true;
			} else {
				*ASCBuffer = tempL;
			}
			ASCBuffer += 2;
		}
		
		ttt = intDriver->DASCBuffer;
		ttt++;
		ASCBuffer = (short*) intDriver->IntDataPtr;
		ASCBuffer++;
		
		i = intDriver->ASCBUFFERReal;
		// Right
		while (i-- > 0) {
			// Compute Clipping
			tempL = *ttt;
			//	*ttt = 0;
			ttt += 2;
			
			if (tempL > valP) {
				*ASCBuffer = valP;
				intDriver->clipR = true;
			} else if (tempL < valN) {
				*ASCBuffer = valN;
				intDriver->clipR = true;
			} else {
				*ASCBuffer = tempL;
			}
			
			ASCBuffer += 2;
		}
	}
	
	// APPLY MICRO DELAY *** *** *** *** *** *** *** *** *** *** *** *** *** ***
	
	if (intDriver->DriverSettings.MicroDelaySize) {
		SInt32 realMDelay = intDriver->MDelay;
		
		switch (intDriver->DriverSettings.outPutBits)
		{
				//TODO: 24-bit, maybe even 20-bit
			case 16:
				memmove(intDriver->DASCBuffer, intDriver->DASCBuffer + intDriver->ASCBUFFER*2, realMDelay*8L);
				memset(intDriver->DASCBuffer + realMDelay*2L, 0, intDriver->ASCBUFFER*8L);
				
				for (i = 0; i < MAXCHANEFFECT; i++) {
					if (intDriver->EffectBufferID[i] != -1) {
						memmove(intDriver->DASCEffectBuffer[i] + intDriver->ASCBUFFER*2, intDriver->DASCEffectBuffer[i], realMDelay*8L);
						memset((intDriver->DASCEffectBuffer[i] + realMDelay*2L), 0, intDriver->ASCBUFFER*8L);
					}
				}
				break;
				
			case 8:
				if (realMDelay % 2 != 0) {
					memmove(intDriver->DASCBuffer8, intDriver->DASCBuffer8 + intDriver->ASCBUFFER*2, 1 + realMDelay*4L);
					memset((intDriver->DASCBuffer8 + realMDelay*2L), 0, (1 + intDriver->ASCBUFFER/2)*8L);
				} else {
					memmove(intDriver->DASCBuffer8, intDriver->DASCBuffer8 + intDriver->ASCBUFFER*2,  realMDelay*4L);
					
					memset((intDriver->DASCBuffer8 + realMDelay*2L), 0, (intDriver->ASCBUFFER/2)*8L);
				}
				break;
		}
	} else {
		if (intDriver->DriverSettings.outPutMode != PolyPhonic) {
			switch (intDriver->DriverSettings.outPutBits)
			{
				case 16:
					memset((intDriver->DASCBuffer), 0, intDriver->ASCBUFFER*8L);
					for (i = 0; i < MAXCHANEFFECT; i++) {
						if (intDriver->EffectBufferID[i] != -1)
							memset(intDriver->DASCEffectBuffer[i], 0, intDriver->ASCBUFFER*8L);
						
					}
					break;
					
				case 8:
					memset((intDriver->DASCBuffer8), 0, (intDriver->ASCBUFFER/2)*8L);
					break;
			}
		}
	}
	
	
	
	// ***** ***** ***** ***** ***** ***** ***** ***** ***** ***** ***** ***** *****
	
	
	
	if (intDriver->DriverSettings.surround)
		ApplySurround(intDriver);
	
	if (intDriver->DriverSettings.Reverb && intDriver->ASCBUFFERReal < intDriver->RDelay) {
		if (intDriver->DriverSettings.outPutMode == DeluxeStereoOutPut) {
			switch (intDriver->DriverSettings.outPutBits)
			{
				case 8:
					ComputeReverb8((Byte*)intDriver->ReverbPtr, (Byte*)intDriver->IntDataPtr, intDriver->ASCBUFFERReal*2L, intDriver->DriverSettings.ReverbStrength);
					memmove(intDriver->ReverbPtr, (void*)((size_t)intDriver->ReverbPtr + intDriver->ASCBUFFERReal*2L), intDriver->RDelay*2L - intDriver->ASCBUFFERReal*2L);
					memmove((void*)((size_t)intDriver->ReverbPtr + intDriver->RDelay*2L - intDriver->ASCBUFFERReal*2L), intDriver->IntDataPtr, intDriver->ASCBUFFERReal*2L);
					break;
					
				case 16:
					ComputeReverb16((short*) intDriver->ReverbPtr, (short*) intDriver->IntDataPtr, intDriver->ASCBUFFERReal*2L, intDriver->DriverSettings.ReverbStrength);
					memmove(intDriver->ReverbPtr, (void*)((size_t)intDriver->ReverbPtr + intDriver->ASCBUFFERReal*4), (intDriver->RDelay - intDriver->ASCBUFFERReal)*4);
					memmove((void*)((size_t)intDriver->ReverbPtr + intDriver->RDelay*4 - intDriver->ASCBUFFERReal*4), intDriver->IntDataPtr, intDriver->ASCBUFFERReal*4);
					break;
			}
		}
	}
	
	if (intDriver->Equalizer) {	// Apply Fourier Transform
		switch (intDriver->DriverSettings.outPutBits)
		{
			case 8:
				ConvertInstrument((Byte*)intDriver->IntDataPtr, (intDriver->ASCBUFFERReal*2));
				FFT8S(intDriver->IntDataPtr, (intDriver->ASCBUFFERReal*2), intDriver->Filter, intDriver, 2, false);
				ConvertInstrumentIn((Byte*)intDriver->IntDataPtr, (intDriver->ASCBUFFERReal*2));
				break;
				
			case 16:
				FFT16S((short*)intDriver->IntDataPtr, (intDriver->ASCBUFFERReal*4), intDriver->Filter, intDriver, 2, false);
				break;
		}
	}
	
#if defined( MAINPLAYERPRO)
	//TODO: Process visual Plug-in
	if (intDriver->DriverSettings.outPutBits == 16)
	{
		if (intDriver->currentlyExporting)
		{
			if (intDriver->thisExport) ProcessVisualPlug( intDriver, (short*) intDriver->IntDataPtr, intDriver->ASCBUFFERReal);
		}
		else ProcessVisualPlug( intDriver, (short*) intDriver->IntDataPtr, intDriver->ASCBUFFERReal);
	}
#endif
}

void ApplySurround( MADDriverRec *intDriver)
{
	switch (intDriver->DriverSettings.outPutBits)
	{
		case 8:
			switch (intDriver->DriverSettings.outPutMode)
		{
			case DeluxeStereoOutPut:
			{
				SInt32	i = intDriver->ASCBUFFER;
				char	*data = (char*)intDriver->IntDataPtr;
				
				while (i-- > 0) {
					*data = -1 - *data;
					data += 2;
				}
			}
				break;
		}
			break;
			
		case 16:
			switch(intDriver->DriverSettings.outPutMode)
		{
			case DeluxeStereoOutPut:
			{
				SInt32	i = intDriver->ASCBUFFER;
				short	*data = (short*)intDriver->IntDataPtr;
				
				while (i-- > 0) {
					*data = -1 - *data;
					data += 2;
				}
			}
				break;
		}
			break;
	}
}

void GenerateSound(MADDriverRec *intDriver)
{
	SInt32 i;
	
	if (intDriver->DriverSettings.driverMode == MIDISoundDriver)
		return;
	
	switch (intDriver->DriverSettings.outPutBits)
	{
		case 8:
			switch (intDriver->DriverSettings.outPutMode)
		{
#if 0
			case MonoOutPut:
				Play8Mono(intDriver);
				intDriver->IntDataPtr	+= intDriver->ASCBUFFER;
				break;
				
			case StereoOutPut:
				Play8Stereo(intDriver);
				intDriver->IntDataPtr	+= intDriver->ASCBUFFER*2L;
				break;
#endif
			case DeluxeStereoOutPut:
				Play8StereoDelay(intDriver);
				intDriver->IntDataPtr	+= intDriver->ASCBUFFER*2L;
				intDriver->DASCBuffer8	+= intDriver->ASCBUFFER*2L;
				break;
				
			case PolyPhonic:
				//case MultiFiles:
				Play8PolyPhonic(intDriver);
				intDriver->IntDataPtr	+= intDriver->ASCBUFFER * intDriver->DriverSettings.numChn;
				break;
		}
			break;
			
		case 16:
			switch (intDriver->DriverSettings.outPutMode)
		{
#if 0
			case MonoOutPut:
				Play16Mono(intDriver);
				intDriver->IntDataPtr	+= intDriver->ASCBUFFER*2L;
				break;
				
			case StereoOutPut:
				Play16Stereo(intDriver);
				intDriver->IntDataPtr	+= intDriver->ASCBUFFER*4L;
				break;
				
#endif
			case DeluxeStereoOutPut:
				Play16StereoDelay(intDriver);
				intDriver->IntDataPtr	+= (intDriver->ASCBUFFER*4L) ;
				intDriver->DASCBuffer	+= (intDriver->ASCBUFFER*2L) ;
				
				for (i = 0; i < MAXCHANEFFECT; i++) intDriver->DASCEffectBuffer[ i] += (intDriver->ASCBUFFER*2L) ;
				break;
				
			case PolyPhonic:
				//case MultiFiles:
				//Play16PolyPhonic(intDriver);
				intDriver->IntDataPtr	+= intDriver->ASCBUFFER * 2L * intDriver->DriverSettings.numChn;
				break;
		}
			break;
	}
}

Boolean DirectSaveAlways(char *myPtr, MADDriverSettings *driverType, MADDriverRec *intDriver)
{
	char				*ptrCopy;
	MADDriverSettings	driverCopy;
	
	if (intDriver == NULL)
		return false;
	
	/*** Copy values ***/
	ptrCopy 		= intDriver->IntDataPtr;
	if (driverType != NULL)
		driverCopy	= intDriver->DriverSettings;
	
	/*** Install New Values ***/
	
	intDriver->IntDataPtr 		= myPtr;
	if (driverType != NULL)
		intDriver->DriverSettings	= *driverType;
	
	/***/			/***/
	/***/			/***/
	/***/			/***/
	
	NoteAnalyse( intDriver);
	
	/***/			/***/
	/***/			/***/
	/***/			/***/
	
	/*** Restore values ***/
	
	intDriver->IntDataPtr 		= ptrCopy;
	if (driverType != NULL)
		intDriver->DriverSettings	= driverCopy;
	
	return true;
}

Boolean DirectSave( char *myPtr, MADDriverSettings *driverType, MADDriverRec *intDriver)
{
	char				*ptrCopy;
	MADDriverSettings	driverCopy;
	
	if (intDriver == NULL)
		return false;
	if (!intDriver->Reading)
		return false;
	
	/*** Copy values ***/
	ptrCopy 		= intDriver->IntDataPtr;
	if (driverType != NULL) driverCopy	= intDriver->DriverSettings;
	
	/*** Install New Values ***/
	
	intDriver->IntDataPtr 		= myPtr;
	if (driverType != NULL)
		intDriver->DriverSettings	= *driverType;
	
	/***/			/***/
	/***/			/***/
	/***/			/***/
	
	NoteAnalyse(intDriver);
	
	/***/			/***/
	/***/			/***/
	/***/			/***/
	
	/*** Restore values ***/
	
	intDriver->IntDataPtr 		= ptrCopy;
	if (driverType != NULL)
		intDriver->DriverSettings	= driverCopy;
	
	if (intDriver->musicEnd == true)
		return false;
	
#if 0
	if (intDriver->curMusic != NULL)
	{
		if (intDriver->PL >= intDriver->curMusic->header->numPointers) return false;
	}
#endif
	
	return true;
}

#ifdef _MIDIHARDWARE_
void NoteOff(short oldIns, short oldN, short oldV, MADDriverRec *intDriver)
{
	OMSMIDIPacket pack;
	
	if(	gOutNodeRefNum == -1) return;
	
	/*** Note - OFF ***/
	
	pack.flags	= 0;	//midiMsgType + midiTimeStampCurrent + midiNoCont;
	pack.len	= 3;
	
	pack.data[ 0] = 0x80 + oldIns;
	pack.data[ 1] = oldN + 12;
	
	if (oldV < 64) pack.data[ 2] = 63 + oldV;
	else pack.data[ 2] = 127;
}

void AllNoteOff( MADDriverRec *intDriver)
{
	short 		i;
	
	if (gOutNodeRefNum == -1) return;
	
	for (i = 0; i < MAXTRACK; i++)
	{
		if (intDriver->NoteOld[ i] != -1)
		{
			NoteOff( intDriver->InstuNoOld[ i], intDriver->NoteOld[ i], intDriver->VelocityOld[ i], intDriver);
			intDriver->NoteOld[ i] = -1;
		}
	}
}

void SampleMIDI( Channel *curVoice, short channel, short curN, MADDriverRec *intDriver)
{
	OMSMIDIPacket pack;
	
	if (gOutNodeRefNum == -1) return;
	
	/*** Note - ON ***/
	
	pack.flags	= 0;	//midiMsgType + midiTimeStampCurrent + midiNoCont;
	pack.len	= 3;
	
	pack.data[ 0] = 0x90 + channel;
	
	pack.data[ 1] = curN + 12;
	if (curVoice->vol < 64) pack.data[ 2] = 63 + curVoice->vol;
	else pack.data[ 2] = 127;
}
#else
void SampleMIDI( Channel *curVoice, short channel, short curN, MADDriverRec *intDriver){}
void AllNoteOff( MADDriverRec *intDriver){}
void NoteOff(short oldIns, short oldN, short oldV, MADDriverRec *intDriver){}
#endif
