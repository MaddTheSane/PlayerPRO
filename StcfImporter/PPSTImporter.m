//
//  PPSTImporter.m
//  PPMacho
//
//  Created by C.W. Betts on 4/20/14.
//
//

#import "PPSTImporter.h"
#import "PPMusicList.h"
#import <PlayerPROKit/PPErrors.h>

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

- (void)loadStcfAtURL:(NSURL*)theURL withReply:(void (^)(NSData* bookmarkData, NSError *error))reply
{
	PPMusicList *theList = [[PPMusicList alloc] init];
	OSErr theErr = [theList loadOldMusicListAtURL:theURL];
	if (theErr) {
		NSError *tmpErr = CreateErrorFromMADErrorType(theErr);
		reply(nil, tmpErr);
		return;
	}
	
	reply([NSKeyedArchiver archivedDataWithRootObject:theList], nil);
}

- (BOOL)listener:(NSXPCListener *)listener shouldAcceptNewConnection:(NSXPCConnection *)newConnection
{
    newConnection.exportedInterface = [NSXPCInterface interfaceWithProtocol:@protocol(PPSTImporterHelper)];
    newConnection.exportedObject = self;
    [newConnection resume];
    
    return YES;
}

@end
