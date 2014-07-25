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

#ifndef __PPC_FILEUTILS_H__
#define __PPC_FILEUTILS_H__

/*!
 *	@header		FileUtils.h
 *	@abstract	File operation API used by PlayerPRO.
 *	@discussion	This header was used as a bridge between Windows and Mac OS (pre-OS X) file APIs.
 *				PlayerPROCore 6's version is a thin wrapper over the POSIX file APIs.
 *				If you need more functionality, use another API instead.
 */

#ifndef __MADI__
#include "MAD.h"
#endif

#ifdef _MAC_H
#include <CoreFoundation/CFByteOrder.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*!
 * @typedef		UNFILE
 * @abstract	A pointer to a file structure.
 */
typedef FILE* UNFILE;

#ifdef __cplusplus
extern "C" {
#endif

/*!
 * @function	iFileOpen
 * @abstract	Deprecated: Calls <code>iFileOpenRead()</code>
 * @discussion	You should be using <code>iFileOpenRead()</code> instead
 * @deprecated	Use <code>iFileOpenRead()</code> instead
 */
PPEXPORT UNFILE	iFileOpen(const char *name) DEPRECATED_ATTRIBUTE;

/*!
 * @function	iFileOpenRead
 * @abstract	Opens a file for reading
 * @result		an UNFILE that can be read from
 * @param		name
 *					The location of the file to read
 * @discussion	The file location can be either relative to the current directory or absolute.
 */
PPEXPORT UNFILE	iFileOpenRead(const char *name);

/*!
 * @function	iFileOpenWrite
 * @abstract	Opens a file for writing
 * @result		an UNFILE that can be written to
 * @param		name
 *					The location of the file to write
 * @discussion	The file location can be either relative to the current directory or absolute.
 */
PPEXPORT UNFILE	iFileOpenWrite(const char *name);

/*!
 * @function	iFileCreate
 * @abstract	Create a file at path, optionally setting the file type
 * @param		path
 *					The path to create the new file
 * @param		type
 *					The File Type to set the file to
 * @discussion	<code>type</code> is ignored on platforms that aren't OS X. This includes iOS.
 *				<code>type</code> is mostly used on pre-OS X versions of Mac OS, and is rarely used nowadays.
 *				It has been replaced by file extensions denoting the file type.
 *				If <code>type</code> is zero, the file type is not set.
 */
PPEXPORT void	iFileCreate(const char *path, MADFourChar type);

/*!
 * @function	iGetEOF
 * @abstract	Get the length of the file
 * @result		the size of the file pointed to by <code>iFileRefI</code>
 * @param		iFileRefI
 *					The file pointer to get the size from
 * @discussion	As this returns a 'long' data type, files larger than 2 GiB on non-LP64 systems (<i>including</i> Win64) may result in an invalid value, or even crash your app.
 *				As most tracker types (and audio files in general) are not that large, this will most likely not be an issue.
 */
PPEXPORT long	iGetEOF(UNFILE iFileRefI);

/*!
 * @function	iRead
 * @abstract	Reads data in file referenced from iFileRefI
 * @result		An error value. 0, or <code>MADNoErr</code> if there was no error.
 * @param		size
 *					The size of the data to read
 * @param		dest
 *					A pointer to put the data
 * @param		iFileRefI
 *					The file reference to read from. Must have been opened with iFileOpenRead()
 * @discussion	The size cannot be larger than the data pointer, otherwise bad things will happen.
 */
PPEXPORT MADErr	iRead(long size, void *dest, UNFILE iFileRefI);

/*!
 * @function	iWrite
 * @abstract	Writes data to file reference from iFileRefI
 * @result		An error value. 0, or <code>MADNoErr</code> if there was no error.
 * @param		size
 *					The size of the data to write
 * @param		src
 *					A pointer to read data from
 * @param		iFileRefI
 *					The file reference to write to; must have been opened with iFileOpenWrite()
 * @discussion	The size cannot be larger than the data pointer, otherwise bad things will happen.
 *				Data is written to the file from the file's current position.
 */
PPEXPORT MADErr	iWrite(long size, const void *src, UNFILE iFileRefI);

/*!
 * @function	iSeekCur
 * @abstract	Change the file position of file pointed at by iFileRefI
 * @result		An error value. 0, or <code>MADNoErr</code> if there was no error.
 * @param		size
 *					The offset from the current position
 * @param		iFileRefI
 *					The file reference to change the file position on
 */
PPEXPORT MADErr	iSeekCur(long size, UNFILE iFileRefI);

/*!
 * @function	iClose
 * @abstract	Closes the file reference
 * @param		iFileRefI
 *					The file reference to close
 */
PPEXPORT void	iClose(UNFILE iFileRefI);

#ifdef __cplusplus
}
#endif

//TODO: use system-based functions, such as the ones used on OS X/iOS
/*!
 * @function    MADByteSwap32
 * @abstract    Byte-swaps a 32-bit value
 * @param       msg_buf
 *					A pointer to a 32-bit value. On return, the value will be byte-swapped.
 * @discussion  Refrain from using this function directly: use either PPLE32 or PPBE32
 */
static inline void MADByteSwap32(void *msg_buf)
{
	uint32_t temp = *((uint32_t*)msg_buf);
#ifdef _MAC_H
	*((uint32_t*)msg_buf) = CFSwapInt32(temp);
#else
	*((uint32_t*)msg_buf) = ((((temp & 0xff000000) >> 24) | \
	((temp & 0x00ff0000) >> 8) | ((temp & 0x0000ff00) << 8) | \
	(temp & 0x000000ff) << 24));
#endif
}

//TODO: use system-based functions, such as the ones used on OS X/iOS
/*!
 * @function    MADByteSwap16
 * @abstract    Byte-swaps a 16-bit value
 * @param       msg_buf
 *					a pointer to a 16-bit value. On return, the value will be byte-swapped.
 * @discussion  Refrain from using this function directly: use either PPLE16 or PPBE16
 */
static inline void MADByteSwap16(void *msg_buf)
{
	uint16_t buf = *((uint16_t*)msg_buf);
#ifdef _MAC_H
	*((uint16_t*)msg_buf) = CFSwapInt16(buf);
#else
	*((uint16_t*)msg_buf) = ((((buf) << 8) & 0xFF00) | (((buf) >> 8) & 0x00FF));
#endif
}

/*!
 * @function    PPBE32
 * @abstract    Gets the native value of a 32-bit big-endian value
 * @param       msg_buf
 *					A pointer to a 32-bit value from, or to, a big endian source. On return, the value is swapped on little-endian machines.
 */
static inline void PPBE32(void *msg_buf)
{
#ifdef __LITTLE_ENDIAN__
	MADByteSwap32(msg_buf);
#endif
}

/*!
 * @function    PPBE16
 * @abstract    Gets the native value of a 16-bit big-endian value
 * @param       msg_buf
 *					A pointer to a 16-bit value from, or to, a big endian source. On return, the value is swapped on little-endian machines.
 */
static inline void PPBE16(void *msg_buf)
{
#ifdef __LITTLE_ENDIAN__
	MADByteSwap16(msg_buf);
#endif
}

/*!
 * @function    PPLE32
 * @abstract    Gets the native value of a 32-bit little-endian value
 * @param       msg_buf
 *					A pointer to a 32-bit value from, or to, a little endian source. On return, the value is swapped on big-endian machines.
 */
static inline void PPLE32(void *msg_buf)
{
#ifdef __BIG_ENDIAN__
	MADByteSwap32(msg_buf);
#endif
}

/*!
 * @function    PPLE16
 * @abstract    Gets the native value of a 16-bit little-endian value
 * @param       msg_buf
 *					A pointer to a 16-bit value from, or to, a little endian source. On return, the value is swapped on big-endian machines.
 */
static inline void PPLE16(void *msg_buf)
{
#ifdef __BIG_ENDIAN__
	MADByteSwap16(msg_buf);
#endif
}

/*!
 * @function    OSType2Ptr
 * @abstract    Converts an OSType to a c-string.
 * @param       type
 *					The OSType to convert
 * @param		str
 *					The address of the char array to write to
 * @discussion  <code>str</code> must be at least five chars long: four for the size of the OSType, and one for the terminating null.
 *				Note that OSTypes use the Mac OS Roman encoding. If needed, use iconv to convert from the Mac OS Roman encoding.
 */
static inline void OSType2Ptr(MADFourChar type, char *str)
{
	PPBE32(&type);
	memcpy(str, &type, 4);
	str[4] = 0;
}

/*!
 * @function    Ptr2OSType
 * @abstract    Converts a c-string to an OSType
 * @result      The converted OSType.
 * @param       str
 *					The string to convert to an OSType, null-terminated.
 * @discussion  Note that OSTypes use the Mac OS Roman encoding. UTF-8 special characters may cause issues. If needed, use iconv to convert to the Mac OS Roman encoding.
 */
static inline MADFourChar Ptr2OSType(const char *str)
{
	short		i;
	MADFourChar	type = '    ';
	
	i = strlen(str);
	if (i > 4)
		i = 4;
	memcpy(&type, str, i);
	PPBE32(&type);
	
	return type;
}

#endif
