//
//  PPSTImporter.h
//  PPMacho
//
//  Created by C.W. Betts on 4/20/14.
//
//

#import <Foundation/Foundation.h>

@protocol PPSTImporterHelper <NSObject>

- (void)loadStcfAtURL:(NSURL* __nonnull)theURL withReply:(void (^__nonnull)(__nullable NSDictionary* bookmarkData, __nullable NSError *error))reply;

@end

@interface PPSTImporter : NSObject <NSXPCListenerDelegate, PPSTImporterHelper>

+ (nonnull instancetype)sharedImporter;

@end
