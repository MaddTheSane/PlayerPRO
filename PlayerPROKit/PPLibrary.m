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
@property (readwrite, strong) NSBundle *plugFile;
@property (readwrite, strong) NSString *plugType;
@property (readwrite, copy) NSArray* UTItypes;
@property (readwrite) BOOL canExport;
@property (readwrite) BOOL canImport;
@property (readwrite) UInt32 plugVersion;
@end

@implementation PPLibraryObject

- (instancetype)initWithPlugInfo:(PlugInfo*)pi
{
	if (self = [super init]) {
		self.menuName = (__bridge NSString *)pi->MenuName;
		self.authorString = (__bridge NSString *)pi->AuthorString;
		self.plugFile = [NSBundle bundleWithURL:CFBridgingRelease(CFBundleCopyBundleURL(pi->file))];
		self.plugType = [NSString stringWithCString:pi->type encoding:NSMacOSRomanStringEncoding];
		self.UTItypes = (__bridge NSArray *)pi->UTItypes;
		self.plugVersion = pi->version;
		
		switch (pi->mode) {
			case MADPlugImportExport:
				self.canExport = YES;
				self.canImport = YES;
				break;
				
			case MADPlugExport:
				self.canExport = YES;
				self.canImport = NO;
				break;
				
			case MADPlugImport:
			default:
				self.canExport = NO;
				self.canImport = YES;
				break;
		}
	}
	return self;
}

- (OSType)plugMode
{
	if (self.canExport && self.canImport)
		return MADPlugImportExport;
	else if (self.canExport)
		return MADPlugExport;
	else
		return MADPlugImport;
}

@end

@implementation PPLibrary
@synthesize _madLib = theLibrary;
@synthesize trackerLibs;

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
	if (theLibrary)
		MADDisposeLibrary(theLibrary);
}

- (MADErr)identifyFileAtPath:(NSString*)apath type:(char*)atype
{
	return [self identifyFileAtURL:[NSURL fileURLWithPath:apath] type:atype];
}

- (MADErr)identifyFileAtURL:(NSURL*)apath type:(char*)atype
{
	return MADMusicIdentifyCFURL(theLibrary, atype, (__bridge CFURLRef)apath);
}

- (MADErr)getInformationFromFileAtPath:(NSString*)apath type:(char*)atype info:(PPInfoRec*)infoRec
{
	return [self getInformationFromFileAtURL:[NSURL fileURLWithPath:apath] type:atype info:infoRec];
}

- (MADErr)getInformationFromFileAtURL:(NSURL*)apath type:(char*)atype info:(PPInfoRec*)infoRec
{
	return MADMusicInfoCFURL(theLibrary, atype, (__bridge CFURLRef)apath, infoRec);
}

#pragma mark NSFastEnumeration protocol

- (NSUInteger)countByEnumeratingWithState:(NSFastEnumerationState *)state objects:(id __unsafe_unretained [])buffer count:(NSUInteger)len
{
	return [trackerLibs countByEnumeratingWithState:state objects:buffer count:len];
}

@end
