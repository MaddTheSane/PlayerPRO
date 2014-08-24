//
//  PPComplexImportPlugObject.h
//  PPMacho
//
//  Created by C.W. Betts on 8/17/14.
//
//

#import <Foundation/Foundation.h>
#import <PlayerPROKit/PPMusicObject.h>

typedef void (^PPComplexImportHandler)(PPMusicObject* inMus, MADErr inErr);

@protocol PPComplexImportPlugInterfaceBase <NSObject>
- (BOOL)canImportURL:(NSURL*)theURL;
- (void)beginImportOfURL:(NSURL*)theURL withHandler:(PPComplexImportHandler)handler;

@optional
- (BOOL)canImportURL:(NSURL*)theURL error:(out NSError**)outErr;
- (NSDictionary*)getTrackerInformationFromURL:(NSURL*)theURL;
- (NSDictionary*)getTrackerInformationFromURL:(NSURL*)theURL error:(out NSError**)outErr;

@end

@protocol PPComplexImportPlugInterface <PPComplexImportPlugInterfaceBase>
- (instancetype)init;
@end
