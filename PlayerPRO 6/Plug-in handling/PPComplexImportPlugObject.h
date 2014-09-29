//
//  PPComplexImportPlugObject.h
//  PPMacho
//
//  Created by C.W. Betts on 8/18/14.
//
//

#import <Foundation/Foundation.h>
#import <PlayerPROKit/PPPlugIns.h>
#import "PPPlugInObject.h"

/**
 @class PPComplexImportPlugObject
 Not done in Swift because I don't know how to get it to behave.
 */

@interface PPComplexImportPlugObject : PPPlugInObject
@property (readonly, copy) NSArray *UTITypes;

- (instancetype)init UNAVAILABLE_ATTRIBUTE;
- (instancetype)initWithBundle:(NSBundle*)ourBundle NS_DESIGNATED_INITIALIZER;

- (BOOL)canImportURL:(NSURL*)theURL error:(out NSError**)outErr;
- (void)beginImportOfURL:(NSURL*)theURL withHandler:(PPComplexImportHandler)handler;

@end
