//
//  PPSTImporter.h
//  PPMacho
//
//  Created by C.W. Betts on 4/20/14.
//
//

#import <Foundation/Foundation.h>

NS_ASSUME_NONNULL_BEGIN

@protocol PPSTImporterHelper <NSObject>

- (void)loadStcfAtURL:(NSURL*)theURL withReply:(void (^)(NSDictionary<NSString*, id>* __nullable bookmarkData, NSError * __nullable error))reply;

@end

@interface PPSTImporter : NSObject <NSXPCListenerDelegate, PPSTImporterHelper>

+ (instancetype)sharedImporter;

@end

NS_ASSUME_NONNULL_END
