//
//  PPLibrary.m
//  PPMacho
//
//  Created by C.W. Betts on 10/28/12.
//
//

#import <PlayerPROKit/PlayerPROKit-Swift.h>
#import "PPLibrary.h"
#import "PPLibrary_PPKPrivate.h"

@interface PPLibrary ()
- (instancetype)initWithPlugInCPath:(const char*)cPath NS_DESIGNATED_INITIALIZER;
@end

NSString * const kPPTotalPatterns = @"Total Patterns";
NSString * const kPPPartitionLength = @"Partition Length";
NSString * const kPPFileSize = @"File Size";
NSString * const kPPSignature = @"Signature";
NSString * const kPPTotalTracks = @"Total Tracks";
NSString * const kPPTotalInstruments = @"Total Instruments";
NSString * const kPPInternalFileName = @"Internal File Name";
NSString * const kPPFormatDescription = @"FormatDescription";

@interface PPLibraryObject (SwiftInternal)
- (instancetype)initWithPlugInfo:(const PlugInfo*)pi;
@end

@implementation PPLibrary
@synthesize _madLib = theLibrary;
@synthesize trackerLibs;

+ (void)deregisterDebugFunction
{
	MADRegisterDebugFunc(NULL);
}

+ (void)registerDebugFunction:(void (*)(short, const char*, const char*))newDebugFunc
{
	MADRegisterDebugFunc(newDebugFunc);
}

+ (void)registerDebugBlock:(void (^)(short, const char*, const char*))newDebugFunc
{
	MADRegisterDebugBlock(newDebugFunc);
}

- (PPLibraryObject *)objectAtIndexedSubscript:(NSInteger)index
{
	return trackerLibs[index];
}

- (instancetype)initWithPlugInCPath:(const char*)cPath
{
	if (self = [super init]) {
		if (MADInitLibrary(cPath, &theLibrary) != MADNoErr) {
			return nil;
		}
		NSMutableArray *tmpArray = [NSMutableArray arrayWithCapacity:theLibrary->TotalPlug];
		for (int i = 0; i < theLibrary->TotalPlug; i++) {
			PPLibraryObject *tmp = [[PPLibraryObject alloc] initWithPlugInfo:&theLibrary->ThePlug[i]];
			[tmpArray addObject:tmp];
		}
		trackerLibs = [[NSArray alloc] initWithArray:tmpArray];
	}
	return self;
}

- (instancetype)init
{
	return [self initWithPlugInCPath:NULL];
}

- (instancetype)initWithPlugInPath:(NSString *)path
{
	return [self initWithPlugInCPath:[path fileSystemRepresentation]];
}

- (instancetype)initWithPlugInURL:(NSURL *)URL
{
	return [self initWithPlugInCPath:[URL fileSystemRepresentation]];
}

- (NSInteger)pluginCount
{
	return [trackerLibs count];
}

- (void)dealloc
{
	if (theLibrary)
		MADDisposeLibrary(theLibrary);
}

- (MADErr)identifyFileAtPath:(NSString*)apath type:(char*)atype
{
	return [self identifyFileAtURL:[NSURL fileURLWithPath:apath] type:atype];
}

- (MADErr)identifyFileAtPath:(NSString*)apath stringType:(out NSString* __autoreleasing *)atype
{
	if (!atype) {
		return MADParametersErr;
	}
	char aChar[5] = {0};
	MADErr anErr = [self identifyFileAtPath:apath type:aChar];
	if (anErr != MADNoErr) {
		*atype = nil;
		return anErr;
	}
	*atype = [[NSString alloc] initWithBytes:aChar length:4 encoding:NSMacOSRomanStringEncoding];
	return anErr;
}

- (MADErr)identifyFileAtURL:(NSURL*)apath type:(char*)atype
{
	return MADMusicIdentifyCFURL(theLibrary, atype, (__bridge CFURLRef)apath);
}

- (MADErr)identifyFileAtURL:(NSURL*)apath stringType:(out NSString* __autoreleasing *)atype
{
	if (!atype) {
		return MADParametersErr;
	}
	char aChar[5] = {0};
	MADErr anErr = MADMusicIdentifyCFURL(theLibrary, aChar, (__bridge CFURLRef)apath);
	if (anErr != MADNoErr) {
		*atype = nil;
		return anErr;
	}
	*atype = [[NSString alloc] initWithBytes:aChar length:4 encoding:NSMacOSRomanStringEncoding];
	return anErr;
}

- (MADErr)getInformationFromFileAtPath:(NSString*)apath type:(in const char*)atype info:(MADInfoRec*)infoRec
{
	return [self getInformationFromFileAtURL:[NSURL fileURLWithPath:apath] type:atype info:infoRec];
}

- (MADErr)getInformationFromFileAtURL:(NSURL*)apath type:(in const char*)atype info:(MADInfoRec*)infoRec
{
	return MADMusicInfoCFURL(theLibrary, (char*)atype, (__bridge CFURLRef)apath, infoRec);
}

- (MADErr)getInformationFromFileAtPath:(NSString*)apath type:(in const char*)atype infoDictionary:(out NSDictionary* __autoreleasing *)infoDict
{
	return  [self getInformationFromFileAtURL:[NSURL fileURLWithPath:apath] type:atype infoDictionary:infoDict];
}

- (MADErr)getInformationFromFileAtURL:(NSURL*)apath type:(in const char*)atype infoDictionary:(out NSDictionary* __autoreleasing*)infoDict
{
	if (!infoDict) {
		return MADParametersErr;
	}
	MADInfoRec infoRec = {0};
	MADErr theErr = [self getInformationFromFileAtURL:apath type:atype info:&infoRec];
	if (theErr != MADNoErr) {
		return theErr;
	}
	
	*infoDict = PPInfoRecToDictionary(infoRec);
	return theErr;
}

- (MADErr)getInformationFromFileAtURL:(NSURL*)apath stringType:(NSString*)atype infoDictionary:(out NSDictionary* __autoreleasing *)infoDict
{
	const char *tmpType = [atype cStringUsingEncoding:NSMacOSRomanStringEncoding];
	return [self getInformationFromFileAtURL:apath type:tmpType infoDictionary:infoDict];
}

- (MADErr)getInformationFromFileAtPath:(NSString*)apath stringType:(NSString*)atype infoDictionary:(out NSDictionary* __autoreleasing *)infoDict
{
	return [self getInformationFromFileAtURL:[NSURL fileURLWithPath:apath] stringType:atype infoDictionary:infoDict];
}

static inline NSString* OSTypeToNSString(OSType theOSType)
{
#if !TARGET_OS_IPHONE
	NSString *checkForValid = CFBridgingRelease(UTCreateStringForOSType(theOSType));
#else
	char ourOSType[5] = {0};
	
	OSType2Ptr(theOSType, ourOSType);
	NSString *checkForValid = [[NSString alloc] initWithBytes:ourOSType length:4 encoding:NSMacOSRomanStringEncoding];
#endif
	
	if (!checkForValid) {
		checkForValid = [[NSString alloc] initWithFormat:@"0x%X", (unsigned int)theOSType];
	}
	
	return checkForValid;
}

NSDictionary *PPInfoRecToDictionary(MADInfoRec infoRec)
{
	return @{kPPTotalPatterns:		@(infoRec.totalPatterns),
			 kPPPartitionLength:	@(infoRec.partitionLength),
			 kPPFileSize:			@(infoRec.fileSize),
			 kPPSignature:			OSTypeToNSString(infoRec.signature),
			 kPPTotalTracks:		@(infoRec.totalTracks),
			 kPPTotalInstruments:	@(infoRec.totalInstruments),
			 kPPInternalFileName:	[NSString stringWithCString:infoRec.internalFileName encoding:NSMacOSRomanStringEncoding],
			 kPPFormatDescription:	[NSString stringWithCString:infoRec.formatDescription encoding:NSMacOSRomanStringEncoding]};
}

#pragma mark NSFastEnumeration protocol

- (NSUInteger)countByEnumeratingWithState:(NSFastEnumerationState *)state objects:(id __unsafe_unretained [])buffer count:(NSUInteger)len
{
	return [trackerLibs countByEnumeratingWithState:state objects:buffer count:len];
}

@end
