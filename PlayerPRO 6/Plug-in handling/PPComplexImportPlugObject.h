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

NS_ASSUME_NONNULL_BEGIN

/**
 @class PPComplexImportPlugObject
 Not done in Swift because I don't know how to get it to behave.
 */
@interface PPComplexImportPlugObject : PPPlugInObject
@property (readonly, copy) NSArray<NSString*> *UTITypes;

- (nonnull instancetype)init UNAVAILABLE_ATTRIBUTE;
- (nullable instancetype)initWithBundle:(NSBundle*)ourBundle NS_DESIGNATED_INITIALIZER;

- (BOOL)canImportURL:(NSURL*)theURL error:(out NSError*__autoreleasing*)outErr;
- (void)beginImportOfURL:(NSURL*)theURL withHandler:(PPComplexImportHandler)handler;

@end

NS_ASSUME_NONNULL_END
