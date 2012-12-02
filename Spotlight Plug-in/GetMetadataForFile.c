#include <CoreFoundation/CoreFoundation.h>
#include <CoreServices/CoreServices.h>
#include <PlayerPROCore/PlayerPROCore.h>

/* -----------------------------------------------------------------------------
   Step 1
   Set the UTI types the importer supports
  
   Modify the CFBundleDocumentTypes entry in Info.plist to contain
   an array of Uniform Type Identifiers (UTI) for the LSItemContentTypes 
   that your importer can handle
  
   ----------------------------------------------------------------------------- */

/* -----------------------------------------------------------------------------
   Step 2 
   Implement the GetMetadataForFile function
  
   Implement the GetMetadataForFile function below to scrape the relevant
   metadata from your document and return it as a CFDictionary using standard keys
   (defined in MDItem.h) whenever possible.
   ----------------------------------------------------------------------------- */

/* -----------------------------------------------------------------------------
   Step 3 (optional) 
   If you have defined new attributes, update the schema.xml file
  
   Edit the schema.xml file to include the metadata keys that your importer returns.
   Add them to the <allattrs> and <displayattrs> elements.
  
   Add any custom types that your importer requires to the <attributes> element
  
   <attribute name="com_mycompany_metadatakey" type="CFString" multivalued="true"/>
  
   ----------------------------------------------------------------------------- */

static const CFStringRef kPPMDInstumentsList = CFSTR("net_sourceforge_playerpro_tracker_instumentlist");
static const CFStringRef kPPMDPatternList = CFSTR("net_sourcegorge_playerpro_tracker_patternlist");
static const CFStringRef kPPMDSignature = CFSTR("net_sourcegorge_playerpro_tracker_signature");
static const CFStringRef kPPMDTotalPatterns = CFSTR("net_sourcegorge_playerpro_tracker_totalpatterns");
static const CFStringRef kPPMDPartitionLength = CFSTR("net_sourcegorge_playerpro_tracker_partitionlength");
static const CFStringRef kPPMDTotalInstruments = CFSTR("net_sourceforge_playerpro_tracker_totalinstruments");
static const CFStringRef kPPMDTotalTracks = CFSTR("net_sourceforge_playerpro_tracker_totaltracks");
static const CFStringRef kPPMDFormatDescription = CFSTR("net_sourceforge_playerpro_tracker_formatdescription");



/* -----------------------------------------------------------------------------
    Get metadata attributes from file
   
   This function's job is to extract useful information your file format supports
   and return it as a dictionary
   ----------------------------------------------------------------------------- */

Boolean GetMetadataForFile(void* thisInterface, 
			   CFMutableDictionaryRef attributes, 
			   CFStringRef contentTypeUTI,
			   CFStringRef pathToFile)
{
    /* Pull any available metadata from the file at the specified path */
    /* Return the attribute keys and attribute values in the dict */
    /* Return TRUE if successful, FALSE if there was no data provided */
    MADDriverRec			*MADDriver;
	MADMusic				*MADMusic1;
	MADLibrary				*MADLib;
	MADDriverSettings		init;
	
	MADGetBestDriver(&init);
	init.driverMode = NoHardwareDriver;
	{
		char builtinPluginPath[PATH_MAX];
		CFBundleRef metaDataBundle = CFBundleGetBundleWithIdentifier(CFSTR("net.sourceforge.playerpro.PlayerPROImporter"));
	
		CFURLRef pluginURL = CFBundleCopyBuiltInPlugInsURL(metaDataBundle);
		CFURLGetFileSystemRepresentation(pluginURL, true, (UInt8*)builtinPluginPath, PATH_MAX);
		CFRelease(pluginURL);
		if(MADInitLibrary(builtinPluginPath, &MADLib) != noErr) return FALSE;
	}
	if( MADCreateDriver( &init, MADLib, &MADDriver) != noErr)
	{
		MADDisposeLibrary(MADLib);
		return FALSE;
	}
	
	{
		char		type[ 5];
#ifdef DEBUG
		char		utiType[5] = {0};
		{
			OSType info;
			CFStringRef ostypes;
			//Try to get the OSType of the UTI.
			ostypes = UTTypeCopyPreferredTagWithClass(contentTypeUTI, kUTTagClassOSType);
			
			info = UTGetOSTypeFromString(ostypes);
			CFRelease(ostypes);
			if (info) {
				OSType2Ptr(info, utiType);
			}
		}
		
#endif
		CFURLRef tempRef = CFURLCreateWithFileSystemPath(kCFAllocatorDefault, pathToFile, kCFURLPOSIXPathStyle, FALSE);

		if(MADMusicIdentifyCFURL(MADLib, type, tempRef) != noErr)
		{
			CFRelease(tempRef);
			goto fail1;
		}

#ifdef DEBUG
		if (strcmp(utiType, type) != 0) {
			printf("File types differ, UTI says %s, PlayerPRO says %s\n", utiType, type);
		}
#endif
		
		if( MADPlugAvailable( MADLib, type))		// Is available a plug to open this file?
		{
			OSErr err = noErr;
			err = MADLoadMusicCFURLFile(MADLib, &MADMusic1, type, tempRef);
			if(err != noErr)
			{
				CFRelease(tempRef);
				goto fail1;
			}
		} else {
			CFRelease(tempRef);
			goto fail1;
		}
		
		{
			//Set the title metadata
			//CFStringRef title = CFStringCreateWithCString(kCFAllocatorDefault, MADMusic1->header->name, kCFStringEncodingMacRoman); //TODO: Check for other encodings?
			//CFDictionarySetValue(attributes, kMDItemTitle, title);
			//CFRelease(title);
		}
		
		{
			PPInfoRec rec;
			{
				char sig[5];
				MADMusicInfoCFURL(MADLib, type, tempRef, &rec);
				OSType2Ptr(rec.signature, sig);
				CFStringRef CFSig = CFStringCreateWithCString(kCFAllocatorDefault, sig, kCFStringEncodingMacRoman);
				CFDictionarySetValue(attributes, kPPMDSignature, CFSig);
				CFRelease(CFSig);
			}
			{
				//Set the title metadata
				CFStringRef title = CFStringCreateWithCString(kCFAllocatorDefault, rec.internalFileName, kCFStringEncodingMacRoman); //TODO: Check for other encodings?
				CFDictionarySetValue(attributes, kMDItemTitle, title);
				CFRelease(title);
			}
			CFNumberRef tempNum = CFNumberCreate(kCFAllocatorDefault, kCFNumberSInt32Type, &rec.totalPatterns);
			CFDictionarySetValue(attributes, kPPMDTotalPatterns, tempNum);
			CFRelease(tempNum); tempNum = NULL;
			tempNum = CFNumberCreate(kCFAllocatorDefault, kCFNumberSInt32Type, &rec.partitionLength);
			CFDictionarySetValue(attributes, kPPMDPartitionLength, tempNum);
			CFRelease(tempNum); tempNum = NULL;
			tempNum = CFNumberCreate(kCFAllocatorDefault, kCFNumberShortType, &rec.totalInstruments);
			CFDictionarySetValue(attributes, kPPMDTotalInstruments, tempNum);
			CFRelease(tempNum); tempNum = NULL;
			tempNum = CFNumberCreate(kCFAllocatorDefault, kCFNumberShortType, &rec.totalTracks);
			CFDictionarySetValue(attributes, kPPMDTotalTracks, tempNum);
			CFRelease(tempNum);
			{
				CFStringRef FormatDes = CFStringCreateWithCString(kCFAllocatorDefault, rec.formatDescription, kCFStringEncodingMacRoman);
				CFDictionarySetValue(attributes, kPPMDFormatDescription, FormatDes);
				CFRelease(FormatDes);
			}
			
		}
		CFRelease(tempRef);

	}
	
	{
		//Set duration metadata
		MADAttachDriverToMusic( MADDriver, MADMusic1, NULL);
		long fT, cT;
		MADGetMusicStatus( MADDriver, &fT, &cT);	// Some infos about current music
		double fTd = fT / 60.0;
		
		CFNumberRef duration = CFNumberCreate(kCFAllocatorDefault, kCFNumberDoubleType, &fTd);
		CFDictionarySetValue(attributes, kMDItemDurationSeconds, duration);
		CFRelease(duration);
	}
	
	{
		CFMutableArrayRef InstruArray = CFArrayCreateMutable(kCFAllocatorDefault, MAXINSTRU, &kCFTypeArrayCallBacks);
		int	i;

		for( i = 0; i < MAXINSTRU ; i++)
		{
			CFStringRef temp = CFStringCreateWithCString(kCFAllocatorDefault, MADMusic1->fid[i].name, kCFStringEncodingMacRoman);//TODO: check for other encodings?
			if (!(CFEqual(CFSTR(""), temp))) {
				CFArrayAppendValue(InstruArray, temp);
			}
			CFRelease(temp);
		}
		
		CFDictionarySetValue(attributes, kPPMDInstumentsList, InstruArray);
		
		CFRelease(InstruArray);
	}
	
	{
		CFMutableArrayRef PatArray = CFArrayCreateMutable(kCFAllocatorDefault, MAXPATTERN, &kCFTypeArrayCallBacks);
		int i;
		for( i = 0; i < MAXPATTERN; i++)
		{
			if (MADMusic1->partition != NULL && MADMusic1->partition[i] != NULL)
			{
				CFStringRef temp = CFStringCreateWithCString(kCFAllocatorDefault, MADMusic1->partition[i]->header.name, kCFStringEncodingMacRoman);//TODO: check for other encodings?
				if (!(CFEqual(CFSTR(""), temp))) {
					CFArrayAppendValue(PatArray, temp);
				}
				CFRelease(temp);
			}
		}
		CFDictionarySetValue(attributes, kPPMDPatternList, PatArray);

		CFRelease(PatArray);
	}
	
	MADCleanDriver( MADDriver);
	MADDisposeMusic( &MADMusic1, MADDriver);	// Dispose the current music
	MADStopDriver(MADDriver);					// Stop driver interrupt function
	MADDisposeDriver(MADDriver);				// Dispose music driver
	MADDisposeLibrary(MADLib);					// Close music library
	return TRUE;
	
fail1:
	MADStopDriver(MADDriver);				// Stop driver interrupt function
	MADDisposeDriver(MADDriver);			// Dispose music driver
	MADDisposeLibrary(MADLib);				// Close music library
	
    return FALSE;
}
