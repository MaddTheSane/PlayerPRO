//
//  PPMIDIImportHelper.h
//  PPMacho
//
//  Created by C.W. Betts on 8/6/14.
//
//

#import <Foundation/Foundation.h>
#include <PlayerPROCore/PPDefs.h>

@protocol PPMIDIImportHelper <NSObject>

- (void)importMIDIFileAtURL:(NSURL*)theURL withReply:(void (^)(NSData *, MADErr error))reply;
- (void)getMIDIInfoFromFileAtURL:(NSURL*)theURL withReply:(void (^)(NSDictionary *, MADErr error))reply;
- (void)canImportMIDIFileAtURL:(NSURL*)theURL withReply:(void (^)(MADErr error))reply;

@end

@interface PPMIDIImporter : NSObject <NSXPCListenerDelegate, PPMIDIImportHelper>
+ (instancetype)sharedImporter;
@end
