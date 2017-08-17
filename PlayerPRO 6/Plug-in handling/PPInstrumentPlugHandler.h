//
//  PPInstrumentPlugHandler.h
//  PPMacho
//
//  Created by C.W. Betts on 9/7/13.
//
//

#import <Foundation/Foundation.h>
#include <PlayerPROCore/PlayerPROCore.h>
#import <PlayerPROKit/PlayerPROKit.h>

@class PPInstrumentImporterObject;
@class PPDocument;

NS_ASSUME_NONNULL_BEGIN

@interface PPInstrumentPlugHandler : NSObject <NSFastEnumeration>
- (instancetype)init NS_DESIGNATED_INITIALIZER;

@property (readonly, copy) NSArray<PPInstrumentImporterObject*> *plugInArray;
@property (readonly) NSInteger plugInCount;
- (PPInstrumentImporterObject*)plugInAtIndex:(NSUInteger)idx;
- (PPInstrumentImporterObject *)objectAtIndexedSubscript:(NSInteger)index;

- (void)addPlugInFromPath:(NSString*)thePath;
- (void)addPlugInFromURL:(NSURL *)urlpath;
- (void)addPlugInFromBundle:(NSBundle *)theBund;

- (void)beginExportingInstrument:(PPInstrumentObject*)theIns ofType:(OSType)aType toURL:(NSURL*)aURL driver:(PPDriver*)driver parentDocument:(PPDocument*)document handler:(PPPlugErrorBlock)handler;
- (void)beginImportingInstrumentOfType:(OSType)aType fromURL:(NSURL*)aURL driver:(PPDriver*)driver parentDocument:(PPDocument*)document handler:(void (^)(NSError *__nullable errorCode, PPInstrumentObject *__nullable createdIns))handler;
- (MADErr)testInstrumentFile:(NSURL *)toTest type:(OSType)theType;
- (BOOL)isPlugAvailable:(OSType)kind;
- (BOOL)identifyInstrumentFile:(NSURL*)ref type:(OSType*)outType error:(NSError**)error NS_REFINED_FOR_SWIFT;

@end

NS_ASSUME_NONNULL_END
