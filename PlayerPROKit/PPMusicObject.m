//
//  PPMusicObject.m
//  PPMacho
//
//  Created by C.W. Betts on 12/1/12.
//
//

#import "PPMusicObject.h"
#import "PPLibrary.h"
#import "PPLibrary_PPKPrivate.h"
#import "PPDriver.h"
#import "PPDriver_Private.h"
#import "PPMusicObject_PPKPrivate.h"
#include <PlayerPROCore/RDriverInt.h>
#include "PPByteswap.h"

@interface PPMusicObject ()
{
	@package
	MADMusic *currentMusic;
	NSString *internalFileName;
	NSString *madInfo;
}
@property (readwrite, strong, nonatomic) NSString *internalFileName;
@property (readwrite, strong, nonatomic) NSString *madInfo;
@property (readwrite, strong) NSURL *filePath;
@end

@implementation PPMusicObject
@synthesize madInfo;
@synthesize attachedDriver;
@synthesize _currentMusic = currentMusic;
@synthesize internalFileName;

+ (OSErr)info:(PPInfoRec*)theInfo fromTrackerAtURL:(NSURL*)thURL usingLibrary:(PPLibrary*)theLib
{
	char filetype[5];
	CFURLRef tmpCFURL;
	OSErr theErr = noErr;
	if (!theInfo || !thURL || !theLib) {
		return MADParametersErr;
	}
	
	if ([[thURL pathExtension] caseInsensitiveCompare:@"madbundle"] == NSOrderedSame) {
		return [PPMusicObjectWrapper info:theInfo fromTrackerAtURL:thURL];
	}

	tmpCFURL = CFBridgingRetain(thURL);
	
	if ((theErr = MADMusicIdentifyCFURL(theLib._madLib, filetype, tmpCFURL)) != noErr)
		goto end;
	
	theErr = MADMusicInfoCFURL(theLib._madLib, filetype, tmpCFURL, theInfo);
	
end:
	CFRelease(tmpCFURL);
	return theErr;
}

- (NSString *)internalFileName
{
	if (!internalFileName) {
		self.internalFileName = [[NSString alloc] initWithCString:currentMusic->header->name encoding:NSMacOSRomanStringEncoding];
	}
	return internalFileName;
}

- (NSString*)madInfo
{
	if (!madInfo) {
		self.madInfo = [[NSString alloc] initWithCString:currentMusic->header->infos encoding:NSMacOSRomanStringEncoding];
	}
	return madInfo;
}

- (id)initWithURL:(NSURL *)url
{
	return [self initWithPath:[url path]];
}

- (id)initWithPath:(NSString *)url
{
	if (self = [super init]) {
		if (MADLoadMADFileCString(&currentMusic, [url fileSystemRepresentation]) != noErr)
			return nil;
		self.filePath = [NSURL fileURLWithPath:url];
	}
	return self;
}

- (id)initWithURL:(NSURL *)url library:(PPLibrary *)theLib
{
	if ([[url pathExtension] caseInsensitiveCompare:@"madbundle"]) {
		return self = [[PPMusicObjectWrapper alloc] initWithURL:url];
	}
	if (self = [super init]) {
		char type[5];
		CFURLRef tmpURL = CFBridgingRetain(url);
		if (MADMusicIdentifyCFURL(theLib._madLib, type, tmpURL) != noErr) {
			CFRelease(tmpURL);
			return nil;
		}
		if (strcmp(type, "MADK") == 0) {
			CFRelease(tmpURL);
			return self = [[PPMusicObject alloc] initWithURL:url];
		} else if (MADLoadMusicCFURLFile(theLib._madLib, &currentMusic, type, tmpURL) != noErr) {
			CFRelease(tmpURL);
			return nil;
		}
		CFRelease(tmpURL);
		self.filePath = url;
	}
	return self;
}

- (id)initWithPath:(NSString *)url library:(PPLibrary *)theLib
{
	return [self initWithURL:[NSURL fileURLWithPath:url] library:theLib];
}

- (id)init
{
	if (self = [super init]) {
		currentMusic = CreateFreeMADK();
	}
	return self;
}

- (id)initWithURL:(NSURL *)url driver:(PPDriver *)theLib
{
	if (self = [self initWithURL:url library:theLib.theLibrary]) {
		self.attachedDriver = theLib;
		[theLib setCurrentMusic:self];
	}
	return nil;
}

- (id)initWithPath:(NSString *)url driver:(PPDriver *)theLib
{
	return [self initWithPath:url library:theLib.theLibrary];
}

- (void)attachToDriver:(PPDriver *)theDriv
{
	self.attachedDriver = theDriv;
	[theDriv setCurrentMusic:self];
}

- (void)dealloc
{
	if (currentMusic) {
		MADDisposeMusic(&currentMusic, NULL);
		currentMusic = NULL;
	}
}

- (OSErr)saveMusicToURL:(NSURL *)tosave
{
	return [self saveMusicToURL:tosave compress:NO];
}

- (OSErr)saveMusicToURL:(NSURL *)tosave compress:(BOOL)mad1Comp
{
	return MADMusicSaveCFURL(currentMusic, (__bridge CFURLRef)tosave, mad1Comp);
}

- (OSErr)exportMusicToURL:(NSURL *)tosave format:(NSString*)form library:(PPLibrary*)otherLib
{
	if ([form isEqualToString:@"MADK"]) {
		return [self saveMusicToURL:tosave];
	}
	return MADMusicExportCFURL(otherLib._madLib, self._currentMusic, (char*)[form cStringUsingEncoding:NSMacOSRomanStringEncoding], (__bridge CFURLRef)tosave);
}

@end

@interface PPMusicObjectWrapper ()
@property (readwrite) OSType madType;
@property (strong, readwrite) NSFileWrapper *musicWrapper;
- (void)syncMusicDataTypes;
@end

@implementation PPMusicObjectWrapper
@synthesize madType;

+ (OSErr)info:(PPInfoRec *)theInfo fromTrackerAtURL:(NSURL *)thURL
{
	OSErr theErr = noErr;
	if (!theInfo || !thURL) {
		return MADParametersErr;
	}
	PPMusicObjectWrapper *tmpVal = [[PPMusicObjectWrapper alloc] initWithURL:thURL];

	if (!tmpVal) {
		return MADReadingErr;
	}
	strcpy(theInfo->formatDescription, "MAD Bundle");

	@try {
		NSData *nameData = [tmpVal.internalFileName dataUsingEncoding:NSMacOSRomanStringEncoding allowLossyConversion:YES];
		if (!nameData || nameData.length == 0) {
			theInfo->internalFileName[0] = '\0';
		} else {
			char fileNameInt[60] = {0};
			[nameData getBytes:fileNameInt length:MIN(nameData.length, (sizeof(fileNameInt)))];
			strlcpy(theInfo->internalFileName, fileNameInt, sizeof(theInfo->internalFileName));
		}
		theInfo->signature = tmpVal.madType;
		theInfo->totalInstruments = tmpVal.totalInstruments;
		theInfo->partitionLength = tmpVal.partitionLength;
		theInfo->totalPatterns = tmpVal.totalPatterns;
		theInfo->totalTracks = tmpVal.totalTracks;
		theInfo->fileSize = [[tmpVal.musicWrapper fileAttributes][NSFileSize] longValue];
	}
	@catch (NSException *exception) {
		memset(theInfo, 0, sizeof(PPInfoRec));
		theErr = MADIncompatibleFile;
	}
	@finally {
		return theErr;
	}
}

- (id)copyWithZone:(NSZone *)zone
{
	[self syncMusicDataTypes];
	return [[[self class] alloc] initFromMusicObject:self];
}

- (NSString*)internalFileName
{
	return internalFileName;
}

- (void)setInternalFileName:(NSString *)_internalFileName
{
	[self willChangeValueForKey:@"internalFileName"];
	internalFileName = [_internalFileName copy];
	[self didChangeValueForKey:@"internalFileName"];
}

- (NSString*)madInfo
{
	return madInfo;
}

- (void)setMadInfo:(NSString *)_madInfo
{
	[self willChangeValueForKey:@"madInfo"];
	madInfo = [_madInfo copy];
	[self didChangeValueForKey:@"madInfo"];
}

#define kMADMusicName @"Mad Name"
#define kMADMusicInfo @"Mad Info"
#define kMADMusicType @"Mad Type"
#define kMADMusicAuthor @"Mad Author"

- (void)setUpObjCStructures
{
	self.internalFileName = [NSString stringWithCString:currentMusic->header->name encoding:NSMacOSRomanStringEncoding];
	self.madInfo = [NSString stringWithCString:currentMusic->header->infos encoding:NSMacOSRomanStringEncoding];
	self.madAuthor = @"";
	self.madType = currentMusic->header->MAD;
}

- (id)init
{
	if (self = [super init]) {
		[self setUpObjCStructures];
	}
	return self;
}

static MADMusic *DeepCopyMusic(MADMusic* oldMus)
{
	MADMusic *toreturn = calloc(sizeof(MADMusic), 1);
	memcpy(toreturn, oldMus, sizeof(MADMusic));
	
	return toreturn;
}

- (id)initFromMusicObject:(PPMusicObject*)oldFormat
{
	if (self = [super init]) {
		MADDisposeMusic(&currentMusic, NULL);
		currentMusic = DeepCopyMusic(oldFormat._currentMusic);
		[self setUpObjCStructures];
	}
	return self;
}

- (void)syncMusicDataTypes
{
	
}

- (id)initWithPath:(NSString *)url
{
	return self = [self initWithURL:[NSURL fileURLWithPath:url]];
}

- (id)initWithURL:(NSURL *)url
{
	if (self = [self initWithFileWrapper:[[NSFileWrapper alloc] initWithURL:url options:NSFileWrapperReadingImmediate error:NULL]]) {
		self.filePath = url;
	}
	return self;
}

- (id)initWithFileWrapper:(NSFileWrapper*)wrapper
{
	if (self = [super init]) {
		self.musicWrapper = wrapper;
		NSDictionary *stuff = [wrapper fileWrappers];
		stuff = nil;
		return nil;
		
	}
	
	return self;
}

- (id)initWithURL:(NSURL *)url library:(PPLibrary *)theLib
{
	if ([[url pathExtension] caseInsensitiveCompare:@"madbundle"]) {
		return self = [self initWithURL:url];
	}
	PPMusicObject *tmpObj = [[PPMusicObject alloc] initWithURL:url library:theLib];
	return self = [self initFromMusicObject:tmpObj];
}

- (OSErr)exportMusicToURL:(NSURL *)tosave format:(NSString*)form library:(PPLibrary*)otherLib
{
	if ([form isEqualToString:@"MADK"]) {
		return [self exportMusicToURL:tosave];
	}
	[self syncMusicDataTypes];
	return [super exportMusicToURL:tosave format:form library:otherLib];
}

- (OSErr)exportMusicToURL:(NSURL *)tosave
{
	[self syncMusicDataTypes];
	
	return [super saveMusicToURL:tosave];
}

- (OSErr)saveMusicToURL:(NSURL *)tosave compress:(BOOL)mad1Comp
{
	return [self saveMusicToURL:tosave];
}

- (OSErr)saveMusicToURL:(NSURL *)tosave
{
	if (![self.musicWrapper writeToURL:tosave options:NSFileWrapperWritingWithNameUpdating originalContentsURL:self.filePath error:NULL]) {
		return MADWritingErr;
	} else {
		return noErr;
	}
}

- (MADMusic *)newMadMusicStruct
{
	[self syncMusicDataTypes];
	return DeepCopyMusic(self._currentMusic);
}

@end
