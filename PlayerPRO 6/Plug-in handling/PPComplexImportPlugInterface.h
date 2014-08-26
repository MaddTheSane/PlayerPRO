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
@required
- (BOOL)canImportURL:(NSURL*)theURL error:(out NSError**)outErr;
- (void)beginImportOfURL:(NSURL*)theURL withHandler:(PPComplexImportHandler)handler;
@end

@protocol PPComplexImportPlugInterface <PPComplexImportPlugInterfaceBase, NSObject>
@required
- (instancetype)init;
@end
