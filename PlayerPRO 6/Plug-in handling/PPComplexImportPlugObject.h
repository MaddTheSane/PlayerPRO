//
//  PPComplexImportPlugObject.h
//  PPMacho
//
//  Created by C.W. Betts on 8/18/14.
//
//

#import <Foundation/Foundation.h>
#import "PPComplexImportPlugInterface.h"

/**
 @class PPComplexImportPlugObject
 Not done in Swift because I don't know how to get it to behave.
 */

@interface PPComplexImportPlugObject : NSObject <PPComplexImportPlugInterfaceBase>
@property (readonly, strong) NSBundle *ourBundle;
@property (readonly, copy) NSArray *UTITypes;

- (instancetype)init UNAVAILABLE_ATTRIBUTE;
- (instancetype)initWithBundle:(NSBundle*)ourBundle NS_DESIGNATED_INITIALIZER;

- (BOOL)plugInRespondsToSelector:(SEL)aSelector;

#pragma mark plug-in functions

- (void)beginImportOfURL:(NSURL*)theURL withHandler:(PPComplexImportHandler)handler;

- (BOOL)canImportURL:(NSURL*)theURL;
- (BOOL)canImportURL:(NSURL*)theURL error:(out NSError**)outErr;

- (NSDictionary*)getTrackerInformationFromURL:(NSURL*)theURL;
- (NSDictionary*)getTrackerInformationFromURL:(NSURL*)theURL error:(out NSError**)outErr;

@end
