//
//  PPSTImporter.h
//  PPMacho
//
//  Created by C.W. Betts on 4/20/14.
//
//

#import <Foundation/Foundation.h>

@class PPMusicList;

@protocol PPSTImporterHelper <NSObject>

- (void)loadStcfAtURL:(NSURL*)theURL withReply:(void (^)(NSData* bookmarkData, NSError *error))reply;

@end

@interface PPSTImporter : NSObject <NSXPCListenerDelegate, PPSTImporterHelper>

+ (instancetype)sharedImporter;

@end
