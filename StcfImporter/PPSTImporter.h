//
//  PPSTImporter.h
//  PPMacho
//
//  Created by C.W. Betts on 4/20/14.
//
//

#import <Foundation/Foundation.h>

@protocol PPSTImporterHelper <NSObject>

- (void)loadStcfAtURL:(NSURL*)theURL withReply:(void (^)(NSDictionary* bookmarkData, NSError *error))reply;

@end

@interface PPSTImporter : NSObject <NSXPCListenerDelegate, PPSTImporterHelper>

+ (instancetype)sharedImporter;

@end
