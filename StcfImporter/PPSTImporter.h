//
//  PPSTImporter.h
//  PPMacho
//
//  Created by C.W. Betts on 4/20/14.
//
//

#import <Foundation/Foundation.h>

@protocol PPSTImporterHelper <NSObject>

- (void)loadStcfAtURL:(NSURL* __nonnull)theURL withReply:(void (^__nonnull)(NSDictionary* __nullable  bookmarkData, NSError * __nullable error))reply;

@end

@interface PPSTImporter : NSObject <NSXPCListenerDelegate, PPSTImporterHelper>

+ (nonnull instancetype)sharedImporter;

@end
