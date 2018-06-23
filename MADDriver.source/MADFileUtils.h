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
 *	@header		MADFileUtils.h
 *	@abstract	File operation API used by PlayerPRO.
 *	@discussion	This header was used as a bridge between Windows and Mac OS (pre-OS X) file APIs. <br>
 *				PlayerPROCore 6's version is a thin wrapper over the POSIX file APIs.
 *				If you need more functionality, you can use another API instead.
 */

#ifdef _MAC_H
#include <CoreFoundation/CFByteOrder.h>
#endif

#include "MADDefs.h"
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

/*!
 * @typedef		UNFILE
 * @abstract	A pointer to a \c FILE structure.
 */
typedef FILE* UNFILE;

#ifdef __cplusplus
extern "C" {
#endif

/*!
 * @function	iFileOpen
 * @abstract	Deprecated: Calls \c iFileOpenRead()
 * @discussion	You should be using \c iFileOpenRead() instead.
 * @deprecated	Use \c iFileOpenRead() instead.
 */
PPEXPORT UNFILE	iFileOpen(const char *name) DEPRECATED_ATTRIBUTE;

/*!
 * @function	iFileOpenRead
 * @abstract	Opens a file for reading
 * @result		An \c UNFILE that can be read from.
 * @param		name
 *					The location of the file to read.
 * @discussion	The file location can be either relative to the current directory or absolute.
 */
PPEXPORT UNFILE	iFileOpenRead(const char *name);

/*!
 * @function	iFileOpenWrite
 * @abstract	Opens a file for writing
 * @result		An \c UNFILE that can be written to.
 * @param		name
 *					The location of the file to write.
 * @discussion	The file location can be either relative to the current directory or absolute.
 */
PPEXPORT UNFILE	iFileOpenWrite(const char *name);

/*!
 * @function	iFileCreate
 * @abstract	Create a file at path, optionally setting the file type
 * @param		path
 *					The path to create the new file.
 * @param		type
 *					The File Type to set the file to.
 * @discussion	\c type is ignored on platforms that aren't OS X. This includes iOS.<br>
 *				\c type is mostly used on pre-OS X versions of Mac OS, and is rarely used nowadays.
 *				It has been replaced by file extensions denoting the file type.<br>
 *				If \c type is \c 0, the file type is not set.
 */
PPEXPORT void	iFileCreate(const char *path, MADFourChar type);

/*!
 * @function	iGetEOF
 * @abstract	Get the length of the file
 * @result		the size of the file pointed to by <code>iFileRefI</code>
 * @param		iFileRefI
 *					The file pointer to get the size from
 * @discussion	As this returns a 'long' data type, files larger than 2 GiB on non-LP64 systems (<i>including</i> Win64) may result in an invalid value, or even crash your app.<br>
 *				As most tracker types (and audio files in general) are not that large, this will most likely not be an issue.
 */
PPEXPORT long	iGetEOF(UNFILE iFileRefI);

/*!
 * @function	iRead
 * @abstract	Reads data in file referenced from \c iFileRefI
 * @result		An error value. 0, or \c MADNoErr if there was no error.
 * @param		size
 *					The size of the data to read.
 * @param		dest
 *					A pointer to put the data.
 * @param		iFileRefI
 *					The file reference to read from. Must have been opened with \c iFileOpenRead()
 * @discussion	The size cannot be larger than the data pointer, otherwise bad things will happen.
 */
PPEXPORT MADErr	iRead(long size, void *dest, UNFILE iFileRefI);

/*!
 * @function	iWrite
 * @abstract	Writes data to file reference from \c iFileRefI
 * @result		An error value. 0, or \c MADNoErr if there was no error.
 * @param		size
 *					The size of the data to write.
 * @param		src
 *					A pointer to read data from.
 * @param		iFileRefI
 *					The file reference to write to. Must have been opened with \c iFileOpenWrite()
 * @discussion	The size cannot be larger than the data pointer, otherwise bad things will happen.
 *				Data is written to the file from the file's current position.
 */
PPEXPORT MADErr	iWrite(long size, const void *src, UNFILE iFileRefI);

/*!
 * @function	iSeekCur
 * @abstract	Change the file position of file pointed at by \c iFileRefI
 * @result		An error value. 0, or \c MADNoErr if there was no error.
 * @param		size
 *					The offset from the current position.
 * @param		iFileRefI
 *					The file reference to change the file position on.
 */
PPEXPORT MADErr	iSeekCur(long size, UNFILE iFileRefI);

/*!
 * @function	iClose
 * @abstract	Closes the file reference
 * @param		iFileRefI
 *					The file reference to close.
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
 * @discussion  Refrain from using this function directly: use either \c MADLE32() or \c MADBE32()
 */
PPINLINE void MADByteSwap32(void *msg_buf)
{
	// Make sure it's properly aligned.
	if ((((uintptr_t)msg_buf) & 3) != 0) {
		// If not, do some hackery
		unsigned char *msg_buf_bytes = (unsigned char *)msg_buf;
		unsigned char byte1 = msg_buf_bytes[0];
		unsigned char byte2 = msg_buf_bytes[1];
		unsigned char byte3 = msg_buf_bytes[2];
		unsigned char byte4 = msg_buf_bytes[3];
		msg_buf_bytes[0] = byte4;
		msg_buf_bytes[1] = byte3;
		msg_buf_bytes[2] = byte2;
		msg_buf_bytes[3] = byte1;
		return;
	}
	uint32_t temp = *((uint32_t*)msg_buf);
#if defined(__llvm__)
	*((uint32_t*)msg_buf) = __builtin_bswap32(temp);
#elif defined(_MAC_H)
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
 * @discussion  Refrain from using this function directly: use either \c MADLE16() or \c MADBE16()
 */
PPINLINE void MADByteSwap16(void *msg_buf)
{
	// Make sure it's properly aligned.
	if ((((uintptr_t)msg_buf) & 1) != 0) {
		// If not, do some hackery
		unsigned char *msg_buf_bytes = (unsigned char *)msg_buf;
		unsigned char byte1 = msg_buf_bytes[0];
		unsigned char byte2 = msg_buf_bytes[1];
		msg_buf_bytes[0] = byte1;
		msg_buf_bytes[1] = byte2;
		return;
	}
	uint16_t buf = *((uint16_t*)msg_buf);
#if defined(__llvm__)
	*((uint16_t*)msg_buf) = __builtin_bswap16(buf);
#elif defined(_MAC_H)
	*((uint16_t*)msg_buf) = CFSwapInt16(buf);
#else
	*((uint16_t*)msg_buf) = ((((buf) << 8) & 0xFF00) | (((buf) >> 8) & 0x00FF));
#endif
}

/*!
 * @function    MADBE32
 * @abstract    Gets the native value of a 32-bit big-endian value
 * @param       msg_buf
 *					A pointer to a 32-bit value from, or to, a big endian source. 
 *					On return, the value is swapped on little-endian machines.
 *	@discussion	On big-endian machines, this is a no-op.
 */
PPINLINE void MADBE32(void *msg_buf)
{
#ifdef __LITTLE_ENDIAN__
	MADByteSwap32(msg_buf);
#endif
}

/*!
 * @function    MADBE16
 * @abstract    Gets the native value of a 16-bit big-endian value
 * @param       msg_buf
 *					A pointer to a 16-bit value from, or to, a big endian source.
 *					On return, the value is swapped on little-endian machines.
 *	@discussion	On big-endian machines, this is a no-op.
 */
PPINLINE void MADBE16(void *msg_buf)
{
#ifdef __LITTLE_ENDIAN__
	MADByteSwap16(msg_buf);
#endif
}

/*!
 * @function    MADLE32
 * @abstract    Gets the native value of a 32-bit little-endian value
 * @param       msg_buf
 *					A pointer to a 32-bit value from, or to, a little endian source.
 *					On return, the value is swapped on big-endian machines.
 *	@discussion	On little-endian machines, this is a no-op.
 */
PPINLINE void MADLE32(void *msg_buf)
{
#ifdef __BIG_ENDIAN__
	MADByteSwap32(msg_buf);
#endif
}

/*!
 * @function    MADLE16
 * @abstract    Gets the native value of a 16-bit little-endian value
 * @param       msg_buf
 *					A pointer to a 16-bit value from, or to, a little endian source. 
 *					On return, the value is swapped on big-endian machines.
 *	@discussion	On little-endian machines, this is a no-op.
 */
PPINLINE void MADLE16(void *msg_buf)
{
#ifdef __BIG_ENDIAN__
	MADByteSwap16(msg_buf);
#endif
}

/*!
 * @function    OSType2Ptr
 * @abstract    Converts an \c OSType to a c string.
 * @param       type
 *					The \c OSType to convert
 * @param		str
 *					The address of the char array to write to
 * @discussion  \c str must be at least five chars long: four for the size of the \c OSType, and one for the terminating null. <br>
 *				Note that <code>OSType</code>s use the Mac OS Roman string encoding.
 *				If needed, use the iconv library (or similar) to convert from the Mac OS Roman encoding.
 */
PPINLINE void OSType2Ptr(MADFourChar type, char *str)
{
	MADBE32(&type);
	memcpy(str, &type, 4);
	str[4] = 0;
}

/*!
 * @function    Ptr2OSType
 * @abstract    Converts a c-string to an \c OSType
 * @result      The converted \c OSType
 * @param       str
 *					The string to convert to an <code>OSType</code>, null-terminated.
 * @discussion  Note that <code>OSType</code>s use the Mac OS Roman string encoding. UTF-8 special characters may cause issues.
 *				If needed, use the iconv library (or similar) to convert to the Mac OS Roman encoding.
 */
PPINLINE MADFourChar Ptr2OSType(const char *str)
{
	short		i;
	MADFourChar	type = '    ';
	
	i = strlen(str);
	if (i > 4)
		i = 4;
	memcpy(&type, str, i);
	MADBE32(&type);
	
	return type;
}

#endif
