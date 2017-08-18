//
//  PPSTImporter.m
//  PPMacho
//
//  Created by C.W. Betts on 4/20/14.
//
//

#import "PPSTImporter.h"
#include <Carbon/Carbon.h>

static StringPtr GetStringFromHandle(Handle aResource, ResourceIndex aId)
{
	const Size handSize = GetHandleSize(aResource);
	long curSize = 2;
	
	if (!aResource)
		return NULL;
	UInt8 *data = *(UInt8**)aResource;
	UInt16 count = *(UInt16*)data;
	count = CFSwapInt16BigToHost(count);
	
	// First 2 bytes are the count of strings that this resource has.
	if (count < aId)
		return NULL;
	// skip count
	data += 2;
	
	// looking for data.  data is in order
	while (--aId >= 0)
	{
		short toAdd = (*(UInt8*)data) + 1;
		curSize += toAdd;
		if (curSize >= handSize) {
			return NULL;
		}
		data = data + toAdd;
	}
	
	return data;
}

@implementation PPSTImporter

+ (instancetype)sharedImporter
{
	static dispatch_once_t onceToken;
	static PPSTImporter *shared;
	dispatch_once(&onceToken, ^{
		shared = [PPSTImporter new];
	});
	return shared;
}

static NSString *pascalStringToNSString(StringPtr aStr, CFStringEncoding encoding)
{
	// First try the passed-in encoding
	NSString *CFaStr = CFBridgingRelease(CFStringCreateWithPascalString(kCFAllocatorDefault, aStr, encoding));
	if (CFaStr == nil) {
		// Perhaps the string is in another encoding. Try just using MacRoman.
		CFaStr = CFBridgingRelease(CFStringCreateWithPascalString(kCFAllocatorDefault, aStr, kCFStringEncodingMacRoman));
	}
	
	// Final check to make sure we do have a string value.
	// If we don't have a valid object pointer, the XPC service will crash.
	return CFaStr ?: @"";
}

- (OSErr)loadOldMusicListAtURL:(NSURL *)toOpen toDictionary:(out NSDictionary **)outDict
{
	NSUInteger		lostMusicCount = 0;
	NSInteger		selectedMusic = -1;
	ResFileRefNum	refNum;
	Handle	aHandle, locHand;
	FSRef	theRef;
	UInt16	theNo;
	if (CFURLGetFSRef((__bridge CFURLRef)toOpen, &theRef) == NO) {
		return fnfErr;
	}
	if (FSIsFSRefValid(&theRef) == false) {
		return fnfErr;
	}
	refNum = FSOpenResFile(&theRef, fsRdPerm);
	OSErr resErr = ResError();
	if (resErr) {
		return resErr;
	}
	UseResFile(refNum);
	aHandle = Get1Resource('STR#', 128);
	if (aHandle == NULL) {
		CloseResFile(refNum);
		return ResError();
	}
	locHand = Get1Resource('selc', 128);
	if (locHand == NULL) {
		ReleaseResource(aHandle);
		CloseResFile(refNum);
		return ResError();
	}
	DetachResource(aHandle);
	DetachResource(locHand);
	CloseResFile(refNum);
	
	HLock(aHandle);
	theNo = *((UInt16*)(*aHandle));
	theNo = CFSwapInt16BigToHost(theNo);
	
	theNo /= 2;
	
	HLock(locHand);
	short location = **((short**)locHand);
	location = CFSwapInt16BigToHost(location);
	HUnlock(locHand);
	DisposeHandle(locHand);
	locHand = NULL;
	
	NSMutableArray<NSString*> *newArray = [[NSMutableArray alloc] initWithCapacity:theNo];
	CFStringEncoding MacCompatible = CFStringGetMostCompatibleMacStringEncoding(CFStringGetSystemEncoding());
	
	for (int i = 0; i < theNo * 2; i += 2) {
		StringPtr aStr, aStr2;
		aStr = GetStringFromHandle(aHandle, i);
		aStr2 = GetStringFromHandle(aHandle, i + 1);
		if (!aStr || !aStr2) {
			break;
		}
		
		NSString *CFaStr = pascalStringToNSString(aStr, MacCompatible);
		NSString *CFaStr2 = pascalStringToNSString(aStr2, MacCompatible);
		NSURL *fullPath;
		
		NSURL *tmpPath = CFBridgingRelease(CFURLCreateWithFileSystemPath(kCFAllocatorDefault, (__bridge CFStringRef)CFaStr, kCFURLHFSPathStyle, true));
		if (![tmpPath checkResourceIsReachableAndReturnError:NULL]) {
			CFaStr = pascalStringToNSString(aStr, kCFStringEncodingMacRoman);
			CFaStr2 = pascalStringToNSString(aStr2, kCFStringEncodingMacRoman);
			tmpPath = CFBridgingRelease(CFURLCreateWithFileSystemPath(kCFAllocatorDefault, (__bridge CFStringRef)CFaStr, kCFURLHFSPathStyle, true));
		}
		fullPath = [tmpPath URLByAppendingPathComponent:CFaStr2];
		if (![fullPath checkResourceIsReachableAndReturnError:NULL]) {
			fullPath = nil;
		}
		
		if (fullPath == nil) {
			NSString *together = [@[CFaStr, CFaStr2] componentsJoinedByString:@":"];
			
			fullPath = CFBridgingRelease(CFURLCreateWithFileSystemPath(kCFAllocatorDefault, (__bridge CFStringRef)together, kCFURLHFSPathStyle, false));
		}
		if ([fullPath checkResourceIsReachableAndReturnError:NULL]) {
			[newArray addObject:[fullPath path]];
		} else {
			if (location != -1 && location == (i / 2)) {
				location = -1;
			} else if (location != -1 && location > (i / 2)) {
				location--;
			}
			lostMusicCount++;
		}
	}
	HUnlock(aHandle);
	DisposeHandle(aHandle);
	
	selectedMusic = (location <= [newArray count]) ? location : -1;
	
	*outDict = @{@"MusicPaths":		[newArray copy],
				 @"SelectedMusic":	@(selectedMusic),
				 @"lostMusicCount":	@(lostMusicCount)};
	
	return noErr;
}

- (void)loadStcfAtURL:(NSURL*)theURL withReply:(void (^)(NSDictionary<NSString*, id>* bookmarkData, NSError * error))reply
{
	NSError *error;
	if (![theURL checkResourceIsReachableAndReturnError:&error]) {
		reply(nil, error);
		return;
	}
	NSDictionary *myDict;
	OSErr myErr = [self loadOldMusicListAtURL:theURL toDictionary:&myDict];
	if (myErr != noErr) {
		NSError *nsErr = [NSError errorWithDomain:NSOSStatusErrorDomain code:myErr userInfo:nil];
		reply(nil, nsErr);
	} else {
		reply(myDict, nil);
	}
}

- (BOOL)listener:(NSXPCListener *)listener shouldAcceptNewConnection:(NSXPCConnection *)newConnection
{
	newConnection.exportedInterface = [NSXPCInterface interfaceWithProtocol:@protocol(PPSTImporterHelper)];
	newConnection.exportedObject = self;
	[newConnection resume];
	
	return YES;
}

@end
