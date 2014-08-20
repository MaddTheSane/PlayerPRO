//
//  PPComplexImportPlugObject.h
//  PPMacho
//
//  Created by C.W. Betts on 8/17/14.
//
//

#import <Foundation/Foundation.h>

typedef void (^PPComplexImportHandler)(void);

@protocol PPComplexImportPlugInterfaceBase <NSObject>
- (BOOL)canImportURL:(NSURL*)theURL;
- (void)beginImportOfURL:(NSURL*)theURL withHandler:(PPComplexImportHandler)handler;

@optional
- (BOOL)canImportURL:(NSURL*)theURL error:(out NSError**)outErr;
- (NSDictionary*)getTrackerInformationFromURL:(NSURL*)theURL;
- (NSDictionary*)getTrackerInformationFromURL:(NSURL*)theURL error:(out NSError**)outErr;

@end

@protocol PPComplexImportPlugInterface <PPComplexImportPlugInterfaceBase, NSObject>
- (instancetype)init;
@end
