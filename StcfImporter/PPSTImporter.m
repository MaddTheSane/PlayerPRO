//
//  PPSTImporter.m
//  PPMacho
//
//  Created by C.W. Betts on 4/20/14.
//
//

#import "PPSTImporter.h"
#import "PPTFMusicList.h"
#import "PPErrors.h"

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

- (void)dealloc
{
	
	
	[super dealloc];
}

- (void)loadStcfAtURL:(NSURL*)theURL withReply:(void (^)(PPMusicList* bookmarkData, NSError *error))reply
{
	PPMusicList *theList = [PPMusicList new];
	OSErr theErr = [theList loadOldMusicListAtURL:theURL];
	if (theErr) {
		NSError *tmpErr = CreateErrorFromMADErrorType(theErr);
		reply(nil, tmpErr);
		[tmpErr release];
		return;
	}
	
	reply(theList, nil);
	[theList release];
}

- (BOOL)listener:(NSXPCListener *)listener shouldAcceptNewConnection:(NSXPCConnection *)newConnection
{
    newConnection.exportedInterface = [NSXPCInterface interfaceWithProtocol:@protocol(PPSTImporterHelper)];
    newConnection.exportedObject = self;
    [newConnection resume];
    
    return YES;
}

@end
