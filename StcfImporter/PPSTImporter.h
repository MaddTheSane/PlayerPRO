//
//  PPSTImporter.h
//  PPMacho
//
//  Created by C.W. Betts on 4/20/14.
//
//

#import <Foundation/Foundation.h>

@protocol PPSTImporterHelper <NSObject>

- (void)loadStcfAtURL:(nonnull NSURL*)theURL withReply:(nonnull void (^)(NSDictionary* __nullable  bookmarkData, NSError * __nullable error))reply;

@end

@interface PPSTImporter : NSObject <NSXPCListenerDelegate, PPSTImporterHelper>

+ (nonnull instancetype)sharedImporter;

@end
