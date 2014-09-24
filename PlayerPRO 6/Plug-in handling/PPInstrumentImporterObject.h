//
//  PPInstrumentImporterObject.h
//  PPMacho
//
//  Created by C.W. Betts on 12/27/12.
//
//

#import <Foundation/Foundation.h>
#include <PlayerPROCore/PlayerPROCore.h>
#import "PPPlugInObject.h"

@class PPDocument;

@interface PPInstrumentImporterObject : PPPlugInObject
@property (readonly, copy) NSArray *UTITypes;
@property (readonly) MADPlugModes mode;
@property (readonly) BOOL canImport;
@property (readonly) BOOL canExport;
@property (readonly, getter=isSample, nonatomic) BOOL sample;

- (instancetype)init UNAVAILABLE_ATTRIBUTE;
- (instancetype)initWithBundle:(NSBundle *)theBund;

- (BOOL)canImportFileAtURL:(NSURL *)fileURL;
- (MADErr)playSampleAtURL:(NSURL*)aSample driver:(PPDriver*)driver;
- (void)beginImportSampleAtURL:(NSURL*)sampleURL instrument:(inout PPInstrumentObject*)InsHeader sample:(inout PPSampleObject*)sample sampleID:(inout short*)sampleID driver:(PPDriver*)driver parentDocument:(PPDocument*)document handler:(PPPlugErrorBlock)handle;
- (void)beginExportSampleToURL:(NSURL*)sampleURL instrument:(PPInstrumentObject*)InsHeader sample:(PPSampleObject*)sample sampleID:(short)sampleID driver:(PPDriver*)driver parentDocument:(PPDocument*)document handler:(PPPlugErrorBlock)handle;

@end
