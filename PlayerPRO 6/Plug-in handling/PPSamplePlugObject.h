//
//  PPSamplePlugObject.h
//  PPMacho
//
//  Created by C.W. Betts on 2/10/15.
//
//

#import "PPPlugInObject.h"

@class PPDocument;

NS_ASSUME_NONNULL_BEGIN

@interface PPSamplePlugObject : PPPlugInObject
@property (readonly, copy) NSArray<NSString*> *UTITypes;
@property (readonly) MADPlugModes mode;
@property (readonly) BOOL canImport;
@property (readonly) BOOL canExport;

- (nullable instancetype)init UNAVAILABLE_ATTRIBUTE;
- (nullable instancetype)initWithBundle:(NSBundle *)theBund;

- (BOOL)canImportFileAtURL:(NSURL *)fileURL;
- (MADErr)playSampleAtURL:(NSURL*)aSample driver:(PPDriver*)driver;
- (void)beginImportSampleAtURL:(NSURL*)sampleURL driver:(PPDriver*)driver parentDocument:(PPDocument*)document handler:(void (^)(MADErr theErr, PPSampleObject *aSample))handler;
- (void)beginExportSample:(PPSampleObject*)aSamp toURL:(NSURL*)sampleURL driver:(PPDriver *)driver parentDocument:(PPDocument *)document handler:(PPPlugErrorBlock)handler;

@end

NS_ASSUME_NONNULL_END
