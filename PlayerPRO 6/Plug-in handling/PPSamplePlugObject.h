//
//  PPSamplePlugObject.h
//  PPMacho
//
//  Created by C.W. Betts on 2/10/15.
//
//

#import "PPPlugInObject.h"

@class PPDocument;

@interface PPSamplePlugObject : PPPlugInObject
@property (readonly, copy) NSArray *UTITypes;
@property (readonly) MADPlugModes mode;
@property (readonly) BOOL canImport;
@property (readonly) BOOL canExport;

- (instancetype)init UNAVAILABLE_ATTRIBUTE;
- (instancetype)initWithBundle:(NSBundle *)theBund;

- (BOOL)canImportFileAtURL:(NSURL *)fileURL;
- (MADErr)playSampleAtURL:(NSURL*)aSample driver:(PPDriver*)driver;
- (void)beginImportSampleAtURL:(NSURL*)sampleURL driver:(PPDriver*)driver parentDocument:(PPDocument*)document handler:(void (^)(MADErr theErr, PPSampleObject *aSample))handler;
- (void)beginExportSample:(PPSampleObject*)aSamp toURL:(NSURL*)sampleURL driver:(PPDriver *)driver parentDocument:(PPDocument *)document handler:(PPPlugErrorBlock)handler;

@end
