/*	v 1.0			*/
/*	1999 by ANR		*/

#include <PlayerPROCore/PlayerPROCore.h>
#include <PlayerPROCore/PPPlug.h>
#include <Carbon/Carbon.h>
#include <QuickTime/QuickTime.h>
#include "WAV.h"


//Taken from OS X's Kerberos source code file FSpUtils.c, as found ot:
// http://www.opensource.apple.com/source/Kerberos/Kerberos-47/KerberosFramework/Common/Sources/FSpUtils.c
//Note that the URL might change.
static OSStatus CFURLToFSSpec (CFURLRef pathURL, FSSpec *outSpec)
{
	OSStatus err = noErr;
	FSRef ref;
	FSCatalogInfo info;
	CFStringRef pathString = NULL;
	CFURLRef parentURL = NULL;
	CFStringRef nameString = NULL;
	
	// First, try to create an FSRef for the full path
	if (err == noErr) {
		if (CFURLGetFSRef(pathURL, &ref)) {
			err = FSIsFSRefValid(&ref) ? noErr : fnfErr;
		} else {
			err = fnfErr;
		}
	}
	
	if (err == noErr) {
		// It's a directory or a file that exists; convert directly into an FSSpec:
		err = FSGetCatalogInfo (&ref, kFSCatInfoNone, NULL, NULL, outSpec, NULL);
	} else {
		Str255 fileName;
		// The suck case.  The file doesn't exist.
		err = noErr;
		
		// Get a CFString for the path
		if (err == noErr) {
			pathString = CFURLCopyFileSystemPath(pathURL, kCFURLPOSIXPathStyle);
			if (pathString == NULL) { err = memFullErr; }
		}
		
		// Get a CFURL for the parent
		if (err == noErr) {
			parentURL = CFURLCreateCopyDeletingLastPathComponent (kCFAllocatorDefault, pathURL);
			if (parentURL == NULL) { err = memFullErr; }
		}
		
		// Build an FSRef for the parent directory, which must be valid to make an FSSpec
		if (err == noErr) {
			Boolean converted = CFURLGetFSRef (parentURL, &ref);
			if (!converted) { err = fnfErr; }
		}
		
		// Get the node ID of the parent directory
		if (err == noErr) {
			err = FSGetCatalogInfo(&ref, kFSCatInfoNodeFlags|kFSCatInfoNodeID|kFSCatInfoVolume, &info, NULL, outSpec, NULL);
		}
		
		// Get a CFString for the file name
		if (err == noErr) {
			nameString = CFURLCopyLastPathComponent (pathURL);
			if (nameString == NULL) { err = memFullErr; }
		}
		
		// Copy the string into the FSSpec
		if (err == noErr) {
			Boolean converted = CFStringGetPascalString (pathString, fileName, sizeof(fileName),
														 kCFStringEncodingMacRoman);
			if (!converted) {
				converted = CFStringGetPascalString (pathString, fileName, sizeof(fileName),
													 CFStringGetSystemEncoding());
			}
			
			if (!converted) { err = fnfErr; }
		}
		
		// Set the node ID in the FSSpec
		if (err == noErr) {
			err = FSMakeFSSpec(info.volume, info.nodeID, fileName, outSpec);
		}
	}
	
	// Free allocated memory
	if (pathString != NULL) { CFRelease (pathString); }
	if (parentURL != NULL)  { CFRelease (parentURL);  }
	if (nameString != NULL) { CFRelease (nameString); }
	
	return err;
}

static OSErr mainQTInst(void					*unused,
						OSType					order,				// Order to execute
						InstrData				*InsHeader,			// Ptr on instrument header
						sData					**sample,			// Ptr on samples data
						short					*sampleID,			// If you need to replace/add only a sample, not replace the entire instrument (by example for 'AIFF' sound)
																	// If sampleID == -1 : add sample else replace selected sample.
						CFURLRef				AlienFileURLRef,	// IN/OUT file
						PPInfoPlug				*thePPInfoPlug)
{
	OSErr		myErr = noErr;
	FSIORefNum	iFileRefI;
	long		inOutBytes;
	FSSpec		tmpSpec;
	
	CFURLToFSSpec(AlienFileURLRef, &tmpSpec);
	
	switch( order)
	{
		case MADPlugPlay:
			break;
			
		case MADPlugImport:
		{
			Ptr				theSound;
			long			lS, lE;
			short			sS;
			unsigned long	rate;
			Boolean			stereo;
			FSSpec			newFile;
			
			myErr = ConvertDataToWAVE( tmpSpec, &newFile, thePPInfoPlug);
			if( myErr == noErr)
			{
				theSound = ConvertWAV( &newFile, &lS, &lE, &sS, &rate, &stereo);
				
				if( theSound)
				{
					long sndSize = GetPtrSize(theSound);
					Ptr newSound = malloc(sndSize);
					memcpy(newSound, theSound, sndSize);
					DisposePtr(theSound);
					inAddSoundToMAD( newSound, sndSize, lS, lE, sS, 60, rate, stereo, newFile.name, InsHeader, sample, sampleID);
				}
				else
				{
					myErr = MADNeedMemory;
				}
				
				FSpDelete( &newFile);
			}
		}
			break;
			
		case MADPlugTest:
		{
			FInfo fInfo;
			
			FSpGetFInfo( &tmpSpec, &fInfo);
			
			if( fInfo.fdType == thePPInfoPlug->fileType) myErr = noErr;
			else myErr = MADFileNotSupportedByThisPlug;
		}
			break;
			
		case MADPlugExport:
			if( *sampleID >= 0)
			{
				OSType				compType = 'NONE';
				unsigned long		rate;
				sData 				*curData = sample[ *sampleID];
				short				numChan;
				
				FSpDelete(&tmpSpec);
				myErr = FSpCreate( &tmpSpec, 'TVOD', 'AIFF', smCurrentScript);
				if(myErr == noErr) myErr = FSpOpenDF( &tmpSpec, fsCurPerm, &iFileRefI);
				
				if( myErr == noErr)
				{
					inOutBytes 	= curData->size;
					rate		= curData->c2spd;
					
					if( curData->stereo) numChan = 2;
					else numChan = 1;
					
					myErr = SetupAIFFHeader(iFileRefI,
											numChan,
											rate << 16L,
											curData->amp,
											compType,
											inOutBytes,
											0);
					
					if(myErr == noErr) myErr = FSWrite( iFileRefI, &inOutBytes, curData->data);
					FSCloseFork( iFileRefI);
				}
			}
			break;
			
		default:
			myErr = MADOrderNotImplemented;
			break;
	}
	
	return myErr;
}

// 26E9A321-0E15-48E3-8A94-062C46FDB875
#define PLUGUUID CFUUIDGetConstantUUIDWithBytes(kCFAllocatorSystemDefault, 0x26, 0xE9, 0xA3, 0x21, 0x0E, 0x15, 0x48, 0xE3, 0x8A, 0x94, 0x06, 0x2C, 0x46, 0xFD, 0xB8, 0x75)
#define PLUGINFACTORY QTInstFactory //The factory name as defined in the Info.plist file
#define PLUGMAIN mainQTInst //The old main function, renamed please

#include "CFPlugin-InstrBridge.c"
