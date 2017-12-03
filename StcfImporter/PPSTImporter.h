//
//  PPSTImporter.h
//  PPMacho
//
//  Created by C.W. Betts on 4/20/14.
//
//

#import <Foundation/Foundation.h>

NS_ASSUME_NONNULL_BEGIN

typedef NSString *PPSTKeys NS_STRING_ENUM;

extern PPSTKeys const PPSTKeyURLs NS_SWIFT_NAME(PPSTKeys.urls);
extern PPSTKeys const PPSTKeySelected NS_SWIFT_NAME(PPSTKeys.selected);
extern PPSTKeys const PPSTKeyLostCount NS_SWIFT_NAME(PPSTKeys.lostCount);

@protocol PPSTImporterHelper <NSObject>

- (void)loadStcfAtURL:(NSURL*)theURL withReply:(void (^)(NSDictionary<PPSTKeys, id>* __nullable bookmarkData, NSError * __nullable error))reply;

@end

@interface PPSTImporter : NSObject <NSXPCListenerDelegate, PPSTImporterHelper>

+ (PPSTImporter *)sharedImporter;

@end

NS_ASSUME_NONNULL_END
