/********************						***********************/
//
//	Player PRO 5.0 - DRIVER SOURCE CODE -
//
//	Library Version 5.0
//
//	To use with MAD Library for Mac: Symantec, CodeWarrior and MPW
//
//	Antoine ROSSET
//	16 Tranchees
//	1206 GENEVA
//	SWITZERLAND
//
//	COPYRIGHT ANTOINE ROSSET 1996, 1997, 1998
//
//	Thank you for your interest in PlayerPRO !
//
//	FAX:				(+41 22) 346 11 97
//	PHONE: 			(+41 79) 203 74 62
//	Internet: 	RossetAntoine@bluewin.ch
//
/********************						***********************/
//FIXME: There are some audio artifacts that aren't present in other tracker players.
//play GADGET.it from MikIT for more info
#ifdef __APPLE__
#include <PlayerPROCore/PlayerPROCore.h>
#else
#include "RDriver.h"
#include "FileUtils.h"
#endif
#include "IT.h"

#if defined(NOEXPORTFUNCS) && NOEXPORTFUNCS
#include "embeddedPlugs.h"
#endif

static MADByte		LastAEffect[MAXTRACK], LastJEffect[MAXTRACK];
static int		old_effect;

#define LOW(para) ((para) & 15)
#define HI(para) ((para) >> 4)

/* IT-Compressed status structure */
typedef struct ITPACK {
	unsigned short	bits;		/* current number of bits */
	unsigned short	bufbits;	/* bits in buffer */
	short			last;		/* last output */
	MADByte			buf;		/* bit buffer */
} ITPACK;


static inline MADByte _mm_read_UBYTE(char **samplePtr)
{
	MADByte val = *((MADByte*)*samplePtr);
	*samplePtr += 1;
	return val;
}

//TODO: byteswapping?
static inline unsigned short ReadUS(char **samplePtr)
{
	unsigned short result = _mm_read_UBYTE(samplePtr);
	result |= ((unsigned short)_mm_read_UBYTE(samplePtr)) << 8;
	return result;
}


/* unpack a 8bit IT packed sample */

static uintptr_t read_itcompr8(ITPACK* status, char **reader, MADByte *sl_buffer, unsigned short count, unsigned short* incnt)
{
	MADByte			*dest=sl_buffer,*end=sl_buffer+count;
	unsigned short	x,y,needbits,havebits,new_count=0;
	unsigned short	bits = status->bits;
	unsigned short	bufbits = status->bufbits;
	char			last = status->last;
	MADByte			buf;
	
	//FIXME: Static analyzer says that this assignment may be garbage under certain circumstances.
	buf = status->buf;
	
	while (dest < end) {
		needbits = new_count ? 3 : bits;
		x = havebits = 0;
		while (needbits) {
			// feed buffer
			if (!bufbits) {
				if((*incnt)--)
					buf = _mm_read_UBYTE(reader);
				else
					buf = 0;
				bufbits = 8;
			}
			// get as many bits as necessary
			y = needbits < bufbits ? needbits : bufbits;
			x |= (buf & ((1 << y)- 1)) << havebits;
			buf >>= y;
			bufbits -= y;
			needbits -= y;
			havebits += y;
		}
		if (new_count) {
			new_count = 0;
			if (++x >= bits)
				x++;
			bits = x;
			continue;
		}
		if (bits < 7) {
			if (x == (1 << (bits - 1))) {
				new_count = 1;
				continue;
			}
		} else if (bits < 9) {
			y = (0xff >> (9 - bits)) - 4;
			if ((x > y) && (x <= y + 8)) {
				if ((x-=y)>=bits)
					x++;
				bits = x;
				continue;
			}
		} else if (bits < 10) {
			if (x>=0x100) {
				bits=x-0x100+1;
				continue;
			}
		} else {
			// error in compressed data...
			//_mm_errno=MMERR_ITPACK_INVALID_DATA;
			return 0;
		}
		
		if (bits < 8) // extend sign
			x = ((char)(x <<(8-bits))) >> (8-bits);
		//	*(dest++)= (last+=x) << 8; // convert to 16 bit
		
		*(dest++) = (last += x);
	}
	status->bits = bits;
	status->bufbits = bufbits;
	status->last = last;
	status->buf = buf;
	return dest-sl_buffer;
}

// unpack a 16bit IT packed sample
static uintptr_t read_itcompr16(ITPACK *status,char* *reader,short *sl_buffer,unsigned short count,unsigned short* incnt)
{
	short 			*dest=sl_buffer,*end=sl_buffer+ count;// (short*) ((char*) sl_buffer+ count);
	int				x,y,needbits,havebits,new_count=0;
	unsigned short 	bits = status->bits;
	unsigned short 	bufbits = status->bufbits;
	short 			last = status->last;
	MADByte 		buf;
	
	//FIXME: Static analyzer says that this assignment may be garbage under certain circumstances.
	buf = status->buf;
	
	while (dest < end) {
		needbits = new_count ? 4 : bits;
		x = havebits = 0;
		while (needbits) {
			// feed buffer
			if (!bufbits) {
				if((*incnt)--)
					buf = _mm_read_UBYTE(reader);
				else
					buf = 0;
				bufbits = 8;
			}
			// get as many bits as necessary
			y = needbits < bufbits ? needbits : bufbits;
			x |= (buf &((1 << y) - 1)) << havebits;
			buf >>= y;
			bufbits -= y;
			needbits -= y;
			havebits += y;
		}
		if (new_count) {
			new_count = 0;
			if (++x >= bits)
				x++;
			bits = x;
			continue;
		}
		if (bits < 7) {
			if (x == (1 << (bits - 1))) {
				new_count = 1;
				continue;
			}
		} else if (bits < 17) {
			y = (0xffff >> (17 - bits)) - 8;
			if ((x > y) && (x <= y + 16)) {
				if ((x -= y) >= bits)
					x++;
				bits = x;
				continue;
			}
		} else if (bits < 18) {
			if (x >= 0x10000) {
				bits = x - 0x10000 + 1;
				continue;
			}
		} else {
			// error in compressed data...
			//_mm_errno=MMERR_ITPACK_INVALID_DATA;
			
			return 0;
		}
		
		if (bits < 16) // extend sign
			x = ((short)(x<<(16-bits)))>>(16-bits);
		*(dest++) = (last += x);
	}
	status->bits = bits;
	status->bufbits = bufbits;
	status->last = last;
	status->buf = buf;
	return dest-sl_buffer;
}

//SL_LoadInternal(void* buffer,UWORD infmt,UWORD outfmt,int scalefactor,ULONG length,MREADER* reader,BOOL dither)

#define SLBUFSIZE 2048

static MADErr DecompressSample(short bits, char* reader, size_t length, char *destPtr)		// sloader.c
{
	size_t			stodo;
	size_t			result, c_block=0;	/* compression bytes until next block */
	ITPACK			status;
	unsigned short	incnt;
	
	if (bits == 16)
		length /= 2;
	
	while(length) {
		stodo = (length < SLBUFSIZE) ? length : SLBUFSIZE;
		
		if (!c_block) {
			if (bits == 16)
				status.bits = 17;
			else
				status.bits = 9;
			
			status.last = status.bufbits = 0;
			
			incnt = ReadUS(&reader);
			
			if (bits == 16)
				c_block = 0x4000;
			else
				c_block = 0x8000;
		}
		
		if (bits == 16) {
			if(!(result=read_itcompr16(&status, &reader, (short*)destPtr, stodo, &incnt)))
				return 1;
		} else {
			if(!(result=read_itcompr8(&status, &reader, (MADByte*) destPtr, stodo, &incnt)))
				return 1;
		}
		
		if (result!=stodo) {
			//_mm_errno=MMERR_ITPACK_INVALID_DATA;
			
			return 1;
		}
		
		if (bits == 16) {
			result *= 2;
		}
		length -= stodo;
		c_block -= stodo;
		
		destPtr += result;
	}
	
	return MADNoErr;
}


static inline bool ITcompMem(const void *a, const void *b, size_t s)
{
	return memcmp(a, b, s) == 0;
}

static void ConvertITEffect(MADByte B0, MADByte B1, MADByte *Cmd, MADByte *Arg, short channel)
{
	MADByte LoB1 = LOW(B1);
	MADByte HiB1 = HI(B1);
	MADByte B0X40 = B0 + 0x40;
	
	switch(B0X40) {
			// Speed
		case 'A':
			*Cmd = speedE;
			*Arg = B1;
			break;
			// Tempo
		case 'T':
			*Cmd = speedE;
			*Arg = B1;
			break;
			
		case 'B':
			*Cmd = fastskipE;
			*Arg = B1;
			break;
			
		case 'C':
			*Cmd = skipE;
			*Arg = B1;
			break;
			
		case 'D':
			if (LoB1 == 0 || HiB1 == 0) {	// Slide volume
			
				*Cmd = slidevolE;
				*Arg = B1;
				
#if 0
				if (*Arg == 0)				// Use last command
				{
					*Arg = LastAEffect[channel];
				}
				else LastAEffect[channel] = *Arg;
#endif
			} else if (HiB1 == 0x0F) {		// Fine Slide volume DOWN
				*Cmd = extendedE;
				*Arg = 11 << 4;
				*Arg += LoB1;
			} else if (LoB1 == 0x0F) {		// Fine Slide volume UP
				*Cmd = extendedE;
				*Arg = 10 << 4;
				*Arg += HiB1;
			}
			break;
			
		case 'E':
			if (HiB1 == 0x0F) {				// FineSlide DOWN
				*Cmd = extendedE;
				*Arg = 2 << 4;				//not supported
				*Arg += LoB1;
			} else if (HiB1 == 0x0E) {		// ExtraFineSlide DOWN
				*Cmd = 0;
				*Arg = 0;					//not supported
			} else {						// Slide DOWN
				*Cmd = upslideE;
				*Arg = B1;
			}
			break;
			
		case 'F':
			if (HiB1 == 0x0F) {				// FineSlide UP
				*Cmd = extendedE;
				*Arg = 1 << 4;				//not supported
				*Arg += LoB1;
			} else if (HiB1 == 0x0E) {		// ExtraFineSlide UP
				*Cmd = 0;
				*Arg = 0;					//not supported
			} else {						// Slide UP
				*Cmd = downslideE;
				*Arg = B1;
			}
			break;
			
		case 'G':
			*Cmd = portamentoE;
			*Arg = B1;
			break;
			
		case 'H':
			*Cmd = vibratoE;
			*Arg = B1;
			break;
			
		case 'J':
			*Cmd = arpeggioE;
			*Arg = B1;
			
			if (*Arg == 0) 				// Use last command
				*Arg = LastJEffect[channel];
			else
				LastJEffect[channel] = *Arg;
			
			break;
			
		case 'K':
			*Cmd = vibratoslideE;
			*Arg = B1;
			break;
			
		case 'L':
			*Cmd = portaslideE;
			*Arg = B1;
			break;
			
		case 'O':
			*Cmd = offsetE;
			*Arg = B1;
			break;
			
		case 'S':		// Special Effects
			switch(HiB1) {
			case 2:		// FineTune
				*Cmd = extendedE;
				*Arg = 5 << 4;
				*Arg += LoB1;
				break;
				
			case 3:		// Set Vibrato WaveForm
				*Cmd = extendedE;
				*Arg = 4 << 4;
				*Arg += LoB1;
				break;
				
			case 4:		// Set Tremolo WaveForm
				*Cmd = extendedE;
				*Arg = 7 << 4;
				*Arg += LoB1;
				break;
				
			case 8:		// Set Panning
				*Cmd = extendedE;
				*Arg = 8 << 4;
				*Arg += LoB1;
				break;
				
			case 0xB:	// Loop pattern
				*Cmd = extendedE;
				*Arg = 6 << 4;
				*Arg += LoB1;
				break;
				
			case 0xC:	// Cut sample
				*Cmd = extendedE;
				*Arg = 12 << 4;
				*Arg += LoB1;
				break;
				
			case 0xD:	// Delay sample
				*Cmd = extendedE;
				*Arg = 13 << 4;
				*Arg += LoB1;
				break;
				
			case 0xE:	// Delay pattern
				*Cmd = extendedE;
				*Arg = 14 << 4;
				*Arg += LoB1;
				break;
				
			default:
				*Cmd = 0;
				*Arg = 0;
				break;
		}
			break;
			
		case 'X':
			if(old_effect & 1) {
				if (B1 <= 128) {
					*Cmd = panningE;
					if (B1 == 128)
						*Arg = 255;
					else
						*Arg = B1<<1;
				}
			} else {
				*Cmd = panningE;
				*Arg = B1;
			}
			break;
			
		default:
			*Cmd = 0;
			*Arg = 0;
			break;
	}
}

#if 0
void ConvertMADEffect(MADByte Cmd, MADByte Arg, MADByte *B0, MADByte *B1)
{
	*B0 = 0;
	*B1 = 0;
	
	switch(Cmd)
	{
		case speedE:
			*B0 = 'A' - 0x40;
			*B1 = Arg;
			break;
			
		case fastskipE:
			*B0 = 'B' - 0x40;
			*B1 = Arg;
			break;
			
		case skipE:
			*B0 = 'C' - 0x40;
			*B1 = Arg;
			break;
			
		case slidevolE:
			*B0 = 'D' - 0x40;
			*B1 = Arg;
			break;
			
		case upslideE:
			*B0 = 'E' - 0x40;
			*B1 = Arg;
			break;
			
		case downslideE:
			*B0 = 'F' - 0x40;
			*B1 = Arg;
			break;
			
		case portamentoE:
			*B0 = 'G' - 0x40;
			*B1 = Arg;
			break;
			
		case vibratoE:
			*B0 = 'H' - 0x40;
			*B1 = Arg;
			break;
			
		case arpeggioE:
			*B0 = 'J' - 0x40;
			*B1 = Arg;
			break;
			
		case vibratoslideE:
			*B0 = 'K' - 0x40;
			*B1 = Arg;
			break;
			
		case portaslideE:
			*B0 = 'L' - 0x40;
			*B1 = Arg;
			break;
			
		case offsetE:
			*B0 = 'O' - 0x40;
			*B1 = Arg;
			break;
			
		case extendedE:
			switch((Arg & 0xF0) >> 4)
		{
			case 1:
				*B0 = 'F' - 0x40;
				*B1 = 0xf0 + (Arg & 0x0F);
				break;
				
			case 2:
				*B0 = 'E' - 0x40;
				*B1 = 0xf0 + (Arg & 0x0F);
				break;
				
			case 5:
				*B0 = 'S' - 0x40;
				*B1 = 0x20 + (Arg & 0x0F);
				break;
				
			case 4:
				*B0 = 'S' - 0x40;
				*B1 = 0x30 + (Arg & 0x0F);
				break;
				
			case 7:
				*B0 = 'S' - 0x40;
				*B1 = 0x40 + (Arg & 0x0F);
				break;
				
			case 6:
				*B0 = 'S' - 0x40;
				*B1 = 0xb0 + (Arg & 0x0F);
				break;
				
			case 12:
				*B0 = 'S' - 0x40;
				*B1 = 0xc0 + (Arg & 0x0F);
				break;
				
			case 13:
				*B0 = 'S' - 0x40;
				*B1 = 0xd0 + (Arg & 0x0F);
				break;
				
			case 14:
				*B0 = 'S' - 0x40;
				*B1 = 0xe0 + (Arg & 0x0F);
				break;
				
			case 11:
				*B0 = 'D' - 0x40;
				*B1 = 0xf0 + (Arg & 0x0F);
				break;
				
			case 10:
				*B0 = 'D' - 0x40;
				*B1 = 0x0f + ((Arg & 0x0F)<<4);
				break;
		}
			break;
	}
}
#endif

static MADErr ConvertIT2Mad(char* theIT, size_t MODSize, MADMusic *theMAD, MADDriverSettings *init)
{
	int		i, x, z, channel = 0, Row;
	char	*MaxPtr;
	char	*theInstrument[256];
	MADByte	tempChar, *theITCopy;
	short	maxTrack;
	//short	ITperiod[12] = {1712,1616,1524,1440,1356,1280,1208,1140,1076,1016, 960, 907};
	bool	useLinear;
	
	/**** Variables pour le MAD ****/
	Cmd *aCmd = NULL;
	
	/**** Variables pour le IT ****/
	
	ITForm ITinfo = {0};
	/********************************/
		
	memset(theInstrument, 0, sizeof(theInstrument));
		
	/**** Header principal *****/
	theITCopy = (MADByte*) theIT;
	
	memcpy(&ITinfo, theITCopy, sizeof(ITinfo));
	theITCopy += 192;
	
	PPLE16(&ITinfo.orderNum);
	PPLE16(&ITinfo.insNum);
	PPLE16(&ITinfo.smpNum);
	PPLE16(&ITinfo.patNum);
	PPLE16(&ITinfo.flags);
	PPLE16(&ITinfo.cwtv);
	PPLE16(&ITinfo.cmwt);
	
	if (ITinfo.cmwt < 0x100)
		return MADFileNotSupportedByThisPlug;
	
	old_effect = 0;
	useLinear = false;
	
	if(ITinfo.flags&8) {
		useLinear = true;	//(UF_XMPERIODS | UF_LINEAR);
		old_effect |= 2;
	}
	if((ITinfo.cwtv >= 0x106) && (ITinfo.flags & 16))
		old_effect |= 1;
	
	/**** Order Num *****/
	ITinfo.orders = (unsigned char *)malloc(ITinfo.orderNum);
	if (ITinfo.orders == NULL)
		return MADNeedMemory;
	memcpy(ITinfo.orders, theITCopy, ITinfo.orderNum);
	theITCopy += ITinfo.orderNum;
	
	/**** Ins Num *****/
	ITinfo.parapins = (int *)malloc(ITinfo.insNum * 4);
	if (ITinfo.parapins == NULL)
		return MADNeedMemory;
	memcpy(ITinfo.parapins, theITCopy, ITinfo.insNum * 4);
	theITCopy += ITinfo.insNum * 4;
	for (i = 0; i < ITinfo.insNum; i++)
		PPLE32(&ITinfo.parapins[i]);
	//if (ITinfo.insNum > 0) DebugStr("\pInsNum");
	
	/**** Samp Num *****/
	ITinfo.parapsamp = (int *)malloc(ITinfo.smpNum * 4);
	if (ITinfo.parapsamp == NULL)
		return MADNeedMemory;
	memcpy(ITinfo.parapsamp, theITCopy, ITinfo.smpNum * 4);
	theITCopy += ITinfo.smpNum * 4;
	for (i = 0; i < ITinfo.smpNum; i++)
		PPLE32(&ITinfo.parapsamp[i]);
	
	/**** Pat Num *****/
	ITinfo.parappat = (int *)malloc(ITinfo.patNum * 4);
	if (ITinfo.parappat == NULL)
		return MADNeedMemory;
	memcpy(ITinfo.parappat, theITCopy, ITinfo.patNum * 4);
	theITCopy += ITinfo.patNum * 4;
	for (i = 0; i < ITinfo.patNum; i++)
		PPLE32(&ITinfo.parappat[i]);
	
	if (ITinfo.flags & 4) {
		/**** Ins Data ****/
		//if (ITinfo.insNum > 64) ITinfo.insNum = 64;
		ITinfo.insdata = (ITInsForm *)malloc(sizeof(ITInsForm) * ITinfo.insNum);
		if (ITinfo.insdata == NULL)
			return MADNeedMemory;
		for (i = 0; i < ITinfo.insNum; i++) {
			theITCopy = (MADByte*) theIT;
			theITCopy += ITinfo.parapins[i];
			
			memcpy(&ITinfo.insdata[i], theITCopy, sizeof(ITInsForm));
			
			if (!ITcompMem((char*)&ITinfo.insdata[i].ID, "IMPI", 4)) {
				free(ITinfo.orders);
				free(ITinfo.parapins);
				free(ITinfo.parappat);
				free(ITinfo.insdata);
				free(ITinfo.parapsamp);
				
				return MADIncompatibleFile;
			}
		}
	} else
		ITinfo.insdata = (ITInsForm *)malloc(4); //FIXME!!
	
	/**** Samp Data ****/
	//if (ITinfo.insNum > 64) ITinfo.insNum = 64;
	ITinfo.sampdata = (ITSampForm *) malloc(sizeof(ITSampForm) * ITinfo.smpNum);
	if (ITinfo.sampdata == NULL)
		return MADNeedMemory;
	for (i = 0; i < ITinfo.smpNum; i++) {
		theITCopy = (MADByte*)theIT;
		theITCopy += ITinfo.parapsamp[i];
		
		memcpy(&ITinfo.sampdata[i], theITCopy, sizeof(ITSampForm));
		
		if (!ITcompMem((char*) &ITinfo.sampdata[i].ID, "IMPS", 4)) {
			free(ITinfo.orders);
			free(ITinfo.parapins);
			free(ITinfo.parappat);
			free(ITinfo.insdata);
			free(ITinfo.parapsamp);
			free(ITinfo.sampdata);
			
			return MADIncompatibleFile;
		}
		
		PPLE32(&ITinfo.sampdata[i].length);
		PPLE32(&ITinfo.sampdata[i].loopBegin);
		PPLE32(&ITinfo.sampdata[i].loopEnd);
		PPLE32(&ITinfo.sampdata[i].C5Speed);
		PPLE32(&ITinfo.sampdata[i].SusLoopBegin);
		PPLE32(&ITinfo.sampdata[i].SusLoopEnd);
		PPLE32(&ITinfo.sampdata[i].samplePtr);
		PPLE16(&ITinfo.sampdata[i].Convert);
		
		if (ITinfo.sampdata[i].length > 0) {
			theInstrument[i] = (char*)theIT;
			theInstrument[i] += ITinfo.sampdata[i].samplePtr;
		} else
			theInstrument[i] = 0;
	}
	
	
	// ******** Le IT a été lu et analysé ***********
	// ******** Copie des informations dans le MAD ***
	
	theMAD->header = (MADSpec*)calloc(sizeof(MADSpec), 1);
	if (theMAD->header == NULL)
		return MADNeedMemory;
	
	theMAD->header->MAD = 'MADK';
	memset(theMAD->header->name, 0, sizeof(theMAD->header->name));
	strncpy(theMAD->header->name, ITinfo.name, sizeof(ITinfo.name));
	
	strncpy(theMAD->header->infos, "Converted by PlayerPRO IT Plug (\251Antoine ROSSET <rossetantoine@bluewin.ch>)", sizeof(theMAD->header->infos));
	
	theMAD->header->numPat			= ITinfo.patNum;
	theMAD->header->numPointers		= ITinfo.orderNum;
	theMAD->header->speed			= ITinfo.iSpeed;
	theMAD->header->tempo			= ITinfo.iTempo;
	theMAD->header->XMLinear		= useLinear;
	
	//for(i=0; i<256; i++) theMAD->header->oPointers[i] = 0;
	for(i = 0; i < ITinfo.orderNum; i++) {
		theMAD->header->oPointers[i] = ITinfo.orders[i];
		
		if (theMAD->header->oPointers[i] < 0 || theMAD->header->oPointers[i] >= ITinfo.patNum)
			theMAD->header->oPointers[i] = 0;
	}
	
	for (i = 0; i < MAXTRACK; i++) {
		if (i < 64) {
			theMAD->header->chanPan[i] = ITinfo.chanPan[i];
			theMAD->header->chanVol[i] = ITinfo.chanVol[i];
			
			if (theMAD->header->chanPan[i] < 0 || theMAD->header->chanPan[i] > 64)
				theMAD->header->chanPan[i] = 32;
			if (theMAD->header->chanVol[i] < 0 || theMAD->header->chanVol[i] > 64)
				theMAD->header->chanVol[i] = 64;
		} else {
			if (i % 2 == 0)
				theMAD->header->chanPan[i] = MAX_PANNING / 4;
			else
				theMAD->header->chanPan[i] = MAX_PANNING - MAX_PANNING / 4;
			
			theMAD->header->chanVol[i] = MAX_VOLUME;
		}
	}
	theMAD->header->generalVol		= 64;
	theMAD->header->generalSpeed	= 80;
	theMAD->header->generalPitch	= 80;
	
	theMAD->sets = (FXSets*) calloc(MAXTRACK, sizeof(FXSets));
	for (i = 0; i < MAXTRACK; i++)
		theMAD->header->chanBus[i].copyId = i;
	
	// ********************
	// ***** INSTRUMENTS *****
	// ********************
	
	theMAD->fid = (InstrData*)calloc(sizeof(InstrData), MAXINSTRU);
	if (!theMAD->fid)
		return MADNeedMemory;
	
	theMAD->sample = (sData**)calloc(sizeof(sData*), MAXINSTRU * MAXSAMPLE);
	if (!theMAD->sample)
		return MADNeedMemory;
	
	for (i = 0; i < MAXINSTRU; i++)
		theMAD->fid[i].firstSample = i * MAXSAMPLE;
	
	for(i  = 0 ; i < MAXINSTRU; i++) {
		for (x = 0; x < MAXSAMPLE; x++)
			theMAD->sample[i * MAXSAMPLE + x] = NULL;
		
		theMAD->fid[i].numSamples	= 0;
	}
	
	if (ITinfo.flags & 4) {		// USE INSTRUMENTS
		short minSamp;
		
		for (i = 0; i < ITinfo.insNum; i++) {
			InstrData *curIns	= &theMAD->fid[i];
			
			curIns->type		= 0;
			
			{
				sData	*curData;
				short	prevSamp, zz, bb;
				
				// Instrument conversion
				
				curIns->numSamples	= 0;	//ITinfo.insdata[i].NoS;
				PPLE16(&ITinfo.insdata[i].FadeOut);
				
				for (x = 0; x < 26; x++)
					curIns->name[x] = ITinfo.insdata[i].INSName[x];
				
				memset(curIns->what, 0, sizeof(curIns->what));
				
				minSamp = 200;
				for (x = 0; x < 120; x++) {
					if (ITinfo.insdata[i].keyMap[x].samp) {
						if (ITinfo.insdata[i].keyMap[x].samp - 1 < minSamp)
							minSamp = ITinfo.insdata[i].keyMap[x].samp - 1;
					}
				}
				
				for (x = 12; x < 96 + 12; x++) {
					if (ITinfo.insdata[i].keyMap[x].samp) {
						if (x >= 12 && x < 96+12) {
							curIns->what[x-12] = ITinfo.insdata[i].keyMap[x].samp - 1 - minSamp;
#if 0
							if (ITinfo.insdata[i].keyMap[x].note != x) {
								Str32	str;
								
								NumToString(ITinfo.insdata[i].keyMap[x].note, str);
								DebugStr(str);
							}
#endif
						}
					}
				}
				
				// Samples conversion
				
				zz = 0;
				
				for (zz = 0; zz < 120; zz++) {
					//CHECK ALL PREVIOUS SAMPLES !!!!
					prevSamp = false;
					for (bb = 0; bb < zz; bb++) {
						if (ITinfo.insdata[i].keyMap[zz].samp - 1 == ITinfo.insdata[i].keyMap[bb].samp - 1)
							prevSamp = true;
					}
					
					if (prevSamp == false && ITinfo.insdata[i].keyMap[zz].samp != 0) {
						
						prevSamp = ITinfo.insdata[i].keyMap[zz].samp-1;
						if (prevSamp >= 0 && prevSamp < ITinfo.smpNum) {
							curData = theMAD->sample[i * MAXSAMPLE + curIns->numSamples] = (sData*)calloc(sizeof(sData), 1);
							if (curData == NULL)
								return MADNeedMemory;
							
							curData->size			= ITinfo.sampdata[prevSamp].length;
							
							if (ITinfo.sampdata[prevSamp].Flag & 16) {
								curData->loopBeg 	= ITinfo.sampdata[prevSamp].loopBegin;
								curData->loopSize	= ITinfo.sampdata[prevSamp].loopEnd - ITinfo.sampdata[prevSamp].loopBegin;
							} else {
								curData->loopBeg = 0;
								curData->loopSize = 0;
							}
							
							if (ITinfo.sampdata[prevSamp].Flag & 64)
								curData->loopType = ePingPongLoop;
							else
								curData->loopType = eClassicLoop;
							
							curData->vol			= ITinfo.sampdata[prevSamp].GvL;
							curData->c2spd			= ITinfo.sampdata[prevSamp].C5Speed / 2;
							curData->loopType		= 0;
							curData->amp			= 8;
							
							if (ITinfo.sampdata[prevSamp].Flag & 2) {
								curData->amp		= 16;
								
								curData->size		*= 2;
								curData->loopBeg	*= 2;
								curData->loopSize	*= 2;
							}
							
							
							curData->relNote	= ((MADByte) ITinfo.insdata[i].keyMap[zz].note) -zz;// - 12;
							
							/*
							 if (((MADByte) ITinfo.insdata[i].keyMap[zz].note))
							 {
							 curData->relNote	= ((MADByte) ITinfo.insdata[i].keyMap[zz].note) - zz;
							 }
							 else curData->relNote = -12;*/
							
							for (z = 0; z < 26; z++)
								curData->name[z] = ITinfo.sampdata[prevSamp].SampName[z];
							
							curData->data 		= (char*)malloc(curData->size);
							if (curData->data == NULL) {
								curData->loopBeg	= 0;
								curData->loopSize	= 0;
								curData->size		= 0;
								curData->data 		= (char*)malloc(curData->size);
							} else {
								if (ITinfo.sampdata[prevSamp].Flag&8) {	// Compressed Sample
									MADErr	err;
									
									err = DecompressSample(curData->amp, theInstrument[prevSamp], curData->size, curData->data);
									
									if (err) {
										size_t temp;
										
										for (temp = 0; temp < curData->size; temp++)
											*(curData->data + temp) = 0;
									}
								} else
									memcpy(curData->data, theInstrument[prevSamp], curData->size);
								
								if (!(ITinfo.sampdata[prevSamp].Convert & 1) && curData->amp == 8) {
									size_t temp;
									
									for (temp = 0; temp < curData->size; temp++)
										*(curData->data + temp) -= 0x80;
								}
								
								if (curData->amp == 16 && !(ITinfo.sampdata[prevSamp].Flag & 8)) {
									unsigned short *tempShort = (unsigned short*)curData->data;
									size_t temp;
									
									for (temp = 0; temp < curData->size / 2; temp++) {
										PPLE16(&((tempShort[temp])));
										
										if (!(ITinfo.sampdata[prevSamp].Convert & 1))
											*(tempShort + temp) -= 0x8000;
									}
								}
								
								if ((ITinfo.sampdata[prevSamp].Convert & 4)) {		// Delta values -> Real
									if (curData->amp == 16) {
										short *tt;
										
										tt = (short*) curData->data;
										
										{
											/* Delta to Real */
											int	oldV, newV;
											int	xL;
											
											oldV = 0;
											for (xL = 0; xL < curData->size / 2; xL++) {
												newV = tt[xL] + oldV;
												oldV = newV;
												if (xL  % 16383 == 0)
													oldV = 0;
												tt[xL] = newV;
												
											}
										}
									} else {
										/* Delta to Real */
										int	oldV, newV;
										int	xL;
										
										oldV = 0;
										for (xL = 0; xL < curData->size; xL++) {
											newV = curData->data[xL] + oldV;
											oldV = newV;
											if (xL  % (32768) == 0)
												oldV = 0;
											curData->data[xL] = newV;
										}
									}
								}
							}
							curIns->numSamples++;
						}
					}
				}
			}
		}
	} else {										// USE SAMPLES AS INSTRUMENTS
		for(i=0; i<ITinfo.smpNum; i++) {
			InstrData *curIns = &theMAD->fid[i];
			curIns->type	= 0;
			
			if (theInstrument[i] != NULL) {
				sData	*curData;
				
				curIns->numSamples	= 1;
				curIns->volFade		= DEFAULT_VOLFADE;
				
				curData = theMAD->sample[i * MAXSAMPLE + 0] = (sData*)calloc(sizeof(sData), 1);
				if (curData == NULL)
					return MADNeedMemory;
				
				curData->size = ITinfo.sampdata[i].length;
				
				
				if (ITinfo.sampdata[i].Flag&16) {
					curData->loopBeg 	= ITinfo.sampdata[i].loopBegin;
					curData->loopSize	= ITinfo.sampdata[i].loopEnd - ITinfo.sampdata[i].loopBegin;
				} else {
					curData->loopBeg = 0;
					curData->loopSize = 0;
				}
				
				if (ITinfo.sampdata[i].Flag & 64)
					curData->loopType = ePingPongLoop;
				else
					curData->loopType = eClassicLoop;
				
				curData->vol		= ITinfo.sampdata[i].GvL;
				curData->c2spd		= ITinfo.sampdata[i].C5Speed / 2;
				curData->amp		= 8;
				
				if (ITinfo.sampdata[i].Flag & 2) {
					curData->amp		= 16;
					
					curData->size		*= 2;
					curData->loopBeg	*= 2;
					curData->loopSize 	*= 2;
				}
				
				curData->relNote	= 0;
				for (x = 0; x < 26; x++)
					curIns->name[x] = ITinfo.sampdata[i].SampName[x];
				
				curData->data 		= (char*)malloc(curData->size);
				if (curData->data == NULL)
					return MADNeedMemory;
				
				if (curData->data != NULL) {
					if (ITinfo.sampdata[i].Flag & 8) {	// Compressed Sample
						MADErr	err;
						
						err = DecompressSample(curData->amp, theInstrument[i], curData->size, curData->data);
						
						if (err) {
							size_t temp;
							
							for (temp = 0; temp < curData->size; temp++)
								*(curData->data + temp) = 0;
						}
					} else
						memcpy(curData->data, theInstrument[i], curData->size);
					
#if 0
					if (!(ITinfo.sampdata[i].Convert & 1) && curData->amp == 8)
					{
						long temp;
						
						for (temp = 0; temp < curData->size; temp++) *(curData->data + temp) -= 0x80;
					}
					
					if (curData->amp == 16)
					{
						unsigned short 	*tempShort = (unsigned short*) curData->data;
						long 						temp;
						
						for (temp = 0; temp < curData->size/2; temp++)
						{
							PPLE16(&((tempShort[temp])));
							
							if (!(ITinfo.sampdata[i].Convert & 1)) *(tempShort + temp) -= 0x8000;
						}
					}
#endif
					
					if (!(ITinfo.sampdata[i].Convert & 1) && curData->amp == 8) {
						int temp;
						
						for (temp = 0; temp < curData->size; temp++)
							*(curData->data + temp) -= 0x80;
					}
					
					if (curData->amp == 16 && !(ITinfo.sampdata[i].Flag & 8)) {
						unsigned short 	*tempShort = (unsigned short*)curData->data;
						int				temp;
						
						for (temp = 0; temp < curData->size/2; temp++) {
							PPLE16(&((tempShort[temp])));
							
							if (!(ITinfo.sampdata[i].Convert & 1))
								*(tempShort + temp) -= 0x8000;
						}
					}
				}
			}
			else curIns->numSamples = 0;
		}
	}
	//	*********************
	//	*   Check MaxTrack  *
	//	*********************
	
	maxTrack = 0;
	
	for (i = 0; i < theMAD->header->numPat ; i++) {
		ITPatForm		*curITPat;
		
		if (ITinfo.parappat[i]) {
			curITPat = (ITPatForm*)(theIT + ITinfo.parappat[i]);
			
			PPLE16(&curITPat->length);
			PPLE16(&curITPat->row);
			
			if (ITinfo.parappat[i] > 0) {
				char	*curDataPat = curITPat->data;
				char	maskvariable = 0, prevmaskvariable[MAXTRACK];
				bool	NeedChannelToRead = true;
				short	xx, newmaxTrack;
				
				for (xx = 0; xx < MAXTRACK; xx++)
					prevmaskvariable[xx] = 0;
				
				Row = 0;
				while(Row < curITPat->row) {
					tempChar = *curDataPat;
					curDataPat++;
					
					newmaxTrack = maxTrack;
					
					if (tempChar == 0)
						Row++;
					else {
						if (NeedChannelToRead) {
							// Channel
							channel = (tempChar-1) & 63;
							
							if (channel > maxTrack)
								newmaxTrack = channel;
						}
						
						if(tempChar & 128) {
							prevmaskvariable[channel] = maskvariable = *curDataPat;
							curDataPat++;
						} else
							maskvariable = prevmaskvariable[channel];
						
						// NOTE
						if (maskvariable & 1) {
							curDataPat++;
							maxTrack = newmaxTrack;
						}
						
						if (maskvariable & 2) {
							curDataPat++;
							maxTrack = newmaxTrack;
						}
						
						if (maskvariable & 4) {
							curDataPat++;
							maxTrack = newmaxTrack;
						}
						
						if (maskvariable & 8) {
							curDataPat += 2;
							maxTrack = newmaxTrack;
						}
						
						if (maskvariable & 16)
							maxTrack = newmaxTrack;
						
						
						if (maskvariable & 32)
							maxTrack = newmaxTrack;
						
						if (maskvariable & 64)
							maxTrack = newmaxTrack;
						
						if (maskvariable & 128)
							maxTrack = newmaxTrack;
					}
				}
				
				if (curDataPat - curITPat->data !=  curITPat->length)
					return MADUnknownErr;
			}
		}
	}
	
	maxTrack += 2;
	maxTrack /= 2;
	maxTrack *= 2;
	
	// ********************
	// ***** TEMPORAIRE ******
	// ********************
	
	memset(LastAEffect, 0, sizeof(LastAEffect));
	memset(LastJEffect, 0, sizeof(LastJEffect));
		
	theMAD->header->numChn = maxTrack;
	
	memset(theMAD->partition, 0, sizeof(theMAD->partition));
	
	for (i = 0; i < theMAD->header->numPat ; i++) {
		ITPatForm		*curITPat = NULL;
		
		if (ITinfo.parappat[i]) {
			curITPat = (ITPatForm*) (theIT + ITinfo.parappat[i]);
#if 0
			curITPat->length	= PPLE16(&curITPat->length);
			curITPat->row		= PPLE16(&curITPat->row);
			
			//Deja fait dans la recherche du maxtrack
#endif
			
			theMAD->partition[i] = (PatData*) calloc(sizeof(PatHeader) + theMAD->header->numChn * curITPat->row * sizeof(Cmd), 1);
			if (theMAD->partition[i] == NULL)
				return MADNeedMemory;
			
			theMAD->partition[i]->header.size 		= curITPat->row;
			theMAD->partition[i]->header.compMode 	= 'NONE';
			
			memset(theMAD->partition[i]->header.name, 0, 20);
			
			MaxPtr = (char*)theMAD->partition[i];
			MaxPtr += sizeof(PatHeader) + theMAD->header->numChn * curITPat->row * sizeof(Cmd);
			
			for (Row = 0; Row < curITPat->row; Row++) {
				for(z = 0; z < theMAD->header->numChn; z++) {
					aCmd = GetMADCommand(Row, z, theMAD->partition[i]);
					
					aCmd->note	= 0xFF;
					aCmd->ins	= 0;
					aCmd->cmd	= 0;
					aCmd->arg	= 0;
					aCmd->vol	= 0xFF;
				}
			}
		} else {	// No Data for this pattern - Clear Pattern
#define DEFSIZE 10
			
			theMAD->partition[i] = (PatData*) calloc(sizeof(PatHeader) + theMAD->header->numChn * DEFSIZE * sizeof(Cmd), 1);
			if (theMAD->partition[i] == NULL)
				return MADNeedMemory;
			
			theMAD->partition[i]->header.size 		= DEFSIZE;
			theMAD->partition[i]->header.compMode 	= 'NONE';
			
			strncpy(theMAD->partition[i]->header.name, "Not used pattern", sizeof(theMAD->partition[i]->header.name));
			
			for (Row = 0; Row < DEFSIZE; Row++) {
				for(z = 0; z < theMAD->header->numChn; z++) {
					aCmd = GetMADCommand(Row, z, theMAD->partition[i]);
					
					aCmd->note	= 0xFF;
					aCmd->ins	= 0;
					aCmd->cmd	= 0;
					aCmd->arg	= 0;
					aCmd->vol	= 0xFF;
				}
			}
		}
		
		if (ITinfo.parappat[i]) {
			char *curDataPat = curITPat->data;
			char maskvariable = 0, prevmaskvariable[MAXTRACK];
			bool NeedChannelToRead = true;
			char lastnote[MAXTRACK], lastins[MAXTRACK], lastvol[MAXTRACK], lastcmd[MAXTRACK], lasteff[MAXTRACK];
			
			memset(prevmaskvariable, 0, sizeof(prevmaskvariable));
			
			Row = 0;
			while(Row < curITPat->row) {
				tempChar = *curDataPat;
				curDataPat++;
				
				if (tempChar == 0)
					Row++;
				else {
					if (NeedChannelToRead) {
						// Channel
						channel = (tempChar-1) & 63;
						if (channel >= 0 && channel < theMAD->header->numChn)
							aCmd = GetMADCommand(Row, channel, theMAD->partition[i]);
						else
							return MADUnknownErr;
					}
					
					if(tempChar & 128) {
						prevmaskvariable[channel] = maskvariable = *curDataPat;
						curDataPat++;
					} else
						maskvariable = prevmaskvariable[channel];
					
					// NOTE
					if (maskvariable & 1 || maskvariable & 16)
					{
						char note;
						
						if (maskvariable & 1)
							note = *curDataPat++;
						else
							note = lastnote[channel];
						lastnote[channel] = note;
						
						if (aCmd != NULL) {
							aCmd->note = note;
							if (aCmd->note == 255)
								aCmd->note = 0xFE;
							else if (aCmd->note < 0 || aCmd->note >= NUMBER_NOTES)
								aCmd->note = 0xFF;
						}
					}
					
					// INSTRUMENT
					if (maskvariable & 2 || maskvariable & 32) {
						char ins;
						
						if (maskvariable & 2)
							ins = *curDataPat++;
						else
							ins = lastins[channel];
						lastins[channel] = ins;
						
						if (aCmd != NULL)
							aCmd->ins = ins;
					}
					
					// VOLUME
					if (maskvariable & 4 || maskvariable & 64) {
						char vol;
						
						if (maskvariable & 4)
							vol = *curDataPat++;
						else
							vol = lastvol[channel];
						lastvol[channel] = vol;
						
						if (aCmd != NULL) {
							aCmd->vol = vol;
							if (aCmd->vol > 64)
								aCmd->vol = 64;
							aCmd->vol += 0x10;
						}
					} else
						aCmd->vol = 255;
					
					// PARAMETER
					if (maskvariable & 8 || maskvariable & 128) {
						char eff, cmd;
						
						if (maskvariable & 8) {
							eff = *curDataPat++;
							cmd = *curDataPat++;
						} else {
							eff = lasteff[channel];
							cmd = lastcmd[channel];
						}
						lasteff[channel] = eff;
						lastcmd[channel] = cmd;
						
						if (aCmd != NULL)
							ConvertITEffect(eff, cmd, &aCmd->cmd, &aCmd->arg, channel);
					}
				}
			}
			
			if (curDataPat - curITPat->data !=  curITPat->length)
				return MADUnknownErr;
		}
	}
	
	
	
	if (ITinfo.orders)
		free(ITinfo.orders);
	if (ITinfo.parapins)
		free(ITinfo.parapins);
	if (ITinfo.parappat)
		free(ITinfo.parappat);
	if (ITinfo.insdata)
		free(ITinfo.insdata);
	if (ITinfo.parapsamp)
		free(ITinfo.parapsamp);
	if (ITinfo.sampdata)
		free(ITinfo.sampdata);
	
	return MADNoErr;
}

static MADErr ExtractITInfo(PPInfoRec *info, void *AlienFile)
{
	//short			i;
	ITForm			ITinfo;
	/********************************/
	
	/**** Header principal *****/
	memcpy(&ITinfo, AlienFile, 96);
	
	/*** Signature ***/
	
	info->signature = 'IT  ';
	
	/*** Internal name ***/
	
	strncpy(info->internalFileName, ITinfo.name, sizeof(ITinfo.name));
	
	/*** Total Patterns ***/
	
	PPLE16(&ITinfo.patNum);
	info->totalPatterns = ITinfo.patNum;
	
	/*** Partition Length ***/
	
	PPLE16(&ITinfo.orderNum);
	info->partitionLength = ITinfo.orderNum;
	
	/*** Total Instruments ***/
	
	PPLE16(&ITinfo.insNum);
	info->totalInstruments = ITinfo.insNum;
	
	/*** Tracks ***/
	
	//info->totalTracks	 = PPLE16( &ITinfo.insNum);
	
	strncpy(info->formatDescription, "IT Plug", sizeof(info->formatDescription));
	
	return MADNoErr;
}

static MADErr TestITFile(const void *AlienFile)
{
	ITForm *myIT = (ITForm*)AlienFile;
	
	if (ITcompMem(&myIT->ID, "IMPM", 4))
		return MADNoErr;
	else
		return MADFileNotSupportedByThisPlug;
}

#ifndef _MAC_H

EXP MADErr FillPlug(PlugInfo *p);
EXP MADErr PPImpExpMain(MADFourChar order, char* AlienFileName, MADMusic *MadFile, PPInfoRec *info, MADDriverSettings *init);

EXP MADErr FillPlug(PlugInfo *p)		// Function USED IN DLL - For PC & BeOS
{
	strncpy(p->type, 		"IT  ", sizeof(p->type));
	strncpy(p->MenuName, 	"IT Files", sizeof(p->MenuName));
	p->mode		= MADPlugImport;
	p->version	= 2 << 16 | 0 << 8 | 0;
	
	return MADNoErr;
}
#endif

#if defined(NOEXPORTFUNCS) && NOEXPORTFUNCS
MADErr mainIT(MADFourChar order, char *AlienFileName, MADMusic *MadFile, PPInfoRec *info, MADDriverSettings *init)
#else
extern MADErr PPImpExpMain(MADFourChar order, char *AlienFileName, MADMusic *MadFile, PPInfoRec *info, MADDriverSettings *init)
#endif
{
	MADErr	myErr = MADNoErr;
	void*	AlienFile;
	long	sndSize;
	UNFILE	iFileRefI;
	
	switch (order) {
		case MADPlugImport:
			iFileRefI = iFileOpenRead(AlienFileName);
			if (iFileRefI) {
				sndSize = iGetEOF(iFileRefI);
				
				// ** MEMORY Test Start
				AlienFile = malloc(sndSize * 2);
				if (AlienFile == NULL)
					myErr = MADNeedMemory;
				// ** MEMORY Test End
				
				else {
					free(AlienFile);
					
					AlienFile = malloc(sndSize);
					if (AlienFile == NULL)
						myErr = MADNeedMemory;
					else {
						myErr = iRead(sndSize, AlienFile, iFileRefI);
						
						if (myErr == MADNoErr) {
							myErr = TestITFile(AlienFile);
							if (myErr == MADNoErr)
								myErr = ConvertIT2Mad(AlienFile, sndSize, MadFile, init);
						}
					}
					free(AlienFile);
					AlienFile = NULL;
				}
				iClose(iFileRefI);
			}
			break;
			
		case MADPlugTest:
			iFileRefI = iFileOpenRead(AlienFileName);
			if (iFileRefI) {
				sndSize = 1024L;
				
				AlienFile = malloc(sndSize);
				if (AlienFile == NULL)
					myErr = MADNeedMemory;
				else {
					myErr = iRead(sndSize, AlienFile, iFileRefI);
					if(myErr == MADNoErr)
						myErr = TestITFile(AlienFile);
					
					free(AlienFile);
					AlienFile = NULL;
				}
				iClose(iFileRefI);
			}
			break;
			
		case MADPlugInfo:
			iFileRefI = iFileOpenRead(AlienFileName);
			if (iFileRefI) {
				info->fileSize = iGetEOF(iFileRefI);
				
				sndSize = 5000L;	// Read only 5000 first bytes for optimisation
				
				AlienFile = malloc(sndSize);
				if (AlienFile == NULL)
					myErr = MADNeedMemory;
				else {
					myErr = iRead(sndSize, AlienFile, iFileRefI);
					if (myErr == MADNoErr) {
						myErr = TestITFile(AlienFile);
						if (!myErr)
							myErr = ExtractITInfo(info, AlienFile);
					}
					free(AlienFile);
					AlienFile = NULL;
				}
				iClose(iFileRefI);
			}
			break;
			
		default:
			myErr = MADOrderNotImplemented;
			break;
	}
	
	return myErr;
}
