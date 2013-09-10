/*
	
	dlsmac.h
	
	Description:
	
	Common interface definitions that do not normally exist in 
	Macintosh development, but are needed for dls.h and dls2.h

	Include this file before the dls.h and dls2.h includes
	Example:
		#include "dlsmac.h"
		#include "dls.h"
		#include "dls2.h"

	Written by Robert Rampley 2000.  Released for public use.
*/
 
#ifndef _INC_DLSMAC
#define _INC_DLSMAC

/*
	AIFF.h 
	a standard Mac include that defines Mac IFF types 
	and includes the required <MacTypes.h>
*/

//#include <AIFF.h>
#include <Carbon/Carbon.h>

/*
	Data Type defs for compatibility with Windows headers
*/

/* FAR is a macro used for pointer defs on Windows */
#ifndef FAR
#define	FAR
#endif

#if 0
#ifndef DWORD
typedef UInt32		DWORD;
#endif

#ifndef ULONG
typedef UInt32		ULONG;
#endif

#ifndef LONG
typedef SInt32		LONG;
#endif

#ifndef USHORT
typedef UInt16		USHORT;
#endif

#ifndef SHORT
typedef SInt16		SHORT;
#endif

#ifndef BYTE
typedef UInt8		BYTE;
#endif

#else
#include "PPDefs.h"
#endif


/*
	mmioFOURCC and MAKEFOURCC
	Similar but slightly different than Mac's FOUR_CHAR_CODE macro.
	read and interpreted the same way, but contructed differently.
	This contruction is reversed from Windows here because we
	are on a Mac(Big Endian).
*/

#ifndef MAKEFOURCC 
#define MAKEFOURCC(ch0, ch1, ch2, ch3)							\
				(UInt32)(										\
				(((UInt32)(UInt8)(ch0) << 24)	& 0xFF000000) |	\
				(((UInt32)(UInt8)(ch1) << 16)	& 0x00FF0000) |	\
				(((UInt32)(UInt8)(ch2) << 8) 	& 0x0000FF00) |	\
				((UInt32)(UInt8)(ch3)			& 0x000000FF))	
#endif

#ifndef mmioFOURCC 
#define mmioFOURCC(ch0, ch1, ch2, ch3)		MAKEFOURCC(ch0, ch1, ch2, ch3)
#endif


/*
	Standard RIFF elements used in DLS files
*/

/* Standard RIFF Container ID */
#ifndef FOURCC_RIFF
#define FOURCC_RIFF		FOUR_CHAR_CODE('RIFF')
#endif

/* Standard LIST Container ID */
#ifndef FOURCC_LIST
#define FOURCC_LIST		FOUR_CHAR_CODE('LIST')
#endif

/* Standard INFO Container ID */
#ifndef FOURCC_INFO
#define FOURCC_INFO		FOUR_CHAR_CODE('INFO')
#endif

/* INAM Chunk ID - title name found in INFO Containers */
#ifndef FOURCC_INAM
#define FOURCC_INAM		FOUR_CHAR_CODE('INAM')
#endif


/*
	DLSID and DEFINE_DLSID
	needed for dls2.h
*/

/* DLSID - a GUID struct */
#ifndef _DLSID
typedef struct _DLSID
{
	ULONG		ulData1;
	USHORT		usData2;
	USHORT		usData3;
	BYTE		abData4[8];
} DLSID;
#endif

/* DEFINE_DLSID    DLSID queries for <cdl-ck> */
#ifndef DEFINE_DLSID
#define DEFINE_DLSID(defName, ul1, us2, us3, ab40, ab41, ab42, ab43, ab44, ab45, ab46, ab47)	\
		static const DLSID defName = { ul1, us2, us3, { ab40, ab41, ab42, ab43, ab44, ab45, ab46, ab47 } }
#endif



#endif /* _INC_DLSMAC */

