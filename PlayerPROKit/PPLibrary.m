//
//  PPLibrary.m
//  PPMacho
//
//  Created by C.W. Betts on 10/28/12.
//
//

#import "PPLibrary.h"
#import "PPLibrary_PPKPrivate.h"

@interface PPLibraryObject ()
@property (readwrite, copy) NSString *menuName;
@property (readwrite, copy) NSString *authorString;
#if !TARGET_OS_IPHONE
@property (readwrite, strong) NSBundle *plugFile;
#endif
@property (readwrite, strong) NSString *plugType;
@property (readwrite, copy) NSArray* UTItypes;
@property (readwrite) BOOL canExport;
@property (readwrite) BOOL canImport;
@property (readwrite) UInt32 plugVersion;
@end

@implementation PPLibraryObject

- (OSType)plugMode
{
	if (self.canExport && self.canImport) {
		return MADPlugImportExport;
	} else if (self.canExport) {
		return MADPlugExport;
	} else {
		return MADPlugImport;
	}
}

@end

@implementation PPLibrary
@synthesize _madLib = theLibrary;
@synthesize trackerLibs;

- (id)initWithPlugInCPath:(const char*)cPath
{
	if (self = [super init]) {
		if(MADInitLibrary((char*)cPath, &theLibrary) != noErr)
		{
			return nil;
		}
		NSMutableArray *tmpArray = [NSMutableArray arrayWithCapacity:theLibrary->TotalPlug];
		for (int i = 0; i < theLibrary->TotalPlug; i++) {
			PPLibraryObject *tmp = [[PPLibraryObject alloc] init];
			tmp.menuName = (__bridge NSString *)theLibrary->ThePlug[i].MenuName;
			tmp.authorString = (__bridge NSString *)theLibrary->ThePlug[i].AuthorString;
#if !TARGET_OS_IPHONE
			tmp.plugFile = [NSBundle bundleWithURL:CFBridgingRelease(CFBundleCopyBundleURL(theLibrary->ThePlug[i].file))];
#endif
			tmp.plugType = [NSString stringWithCString:theLibrary->ThePlug[i].type encoding:NSMacOSRomanStringEncoding];
			tmp.UTItypes = (__bridge NSArray *)theLibrary->ThePlug[i].UTItypes;
			tmp.plugVersion = theLibrary->ThePlug[i].version;
			switch (theLibrary->ThePlug[i].mode) {
				case MADPlugImportExport:
					tmp.canExport = YES;
					tmp.canImport = YES;
					break;
					
				case MADPlugExport:
					tmp.canExport = YES;
					tmp.canImport = NO;
					break;
					
				case MADPlugImport:
				default:
					tmp.canExport = NO;
					tmp.canImport = YES;
					break;
			}
			[tmpArray addObject:tmp];
		}
		trackerLibs = [[NSArray alloc] initWithArray:tmpArray];
	}
	return self;
}

- (id)init
{
	return [self initWithPlugInCPath:NULL];
}

- (id)initWithPlugInPath:(NSString *)path
{
	return [self initWithPlugInCPath:[path fileSystemRepresentation]];
}

- (id)initWithPlugInURL:(NSURL *)URL
{
	return [self initWithPlugInCPath:[[URL path] fileSystemRepresentation]];
}

- (PPLibraryObject *)pluginAtIndex:(NSUInteger)idx
{
	return trackerLibs[idx];
}

- (NSUInteger)pluginCount
{
	return [trackerLibs count];
}

- (void)dealloc
{
	if(theLibrary)
		MADDisposeLibrary(theLibrary);
}

- (NSUInteger)countByEnumeratingWithState:(NSFastEnumerationState *)state objects:(id __unsafe_unretained [])buffer count:(NSUInteger)len
{
	return [trackerLibs countByEnumeratingWithState:state objects:buffer count:len];
}

- (OSErr)identifyFileAtPath:(NSString*)apath type:(char*)atype
{
	return [self identifyFileAtURL:[NSURL fileURLWithPath:apath] type:atype];
}

- (OSErr)identifyFileAtURL:(NSURL*)apath type:(char*)atype
{
	return MADMusicIdentifyCFURL(theLibrary, atype, (__bridge CFURLRef)apath);
}

- (OSErr)getInformationFromFileAtPath:(NSString*)apath type:(char*)atype info:(PPInfoRec*)infoRec
{
	return [self getInformationFromFileAtURL:[NSURL fileURLWithPath:apath] type:atype info:infoRec];
}

- (OSErr)getInformationFromFileAtURL:(NSURL*)apath type:(char*)atype info:(PPInfoRec*)infoRec
{
	return MADMusicInfoCFURL(theLibrary, atype, (__bridge CFURLRef)apath, infoRec);
}


@end
