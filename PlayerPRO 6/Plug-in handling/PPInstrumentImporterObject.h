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

- (instancetype)init UNAVAILABLE_ATTRIBUTE;
- (instancetype)initWithBundle:(NSBundle *)theBund;

- (BOOL)canImportFileAtURL:(NSURL *)fileURL;
- (MADErr)playInstrumentAtURL:(NSURL*)aSample driver:(PPDriver*)driver;

- (void)beginImportInstrumentAtURL:(NSURL*)sampleURL driver:(PPDriver*)driver parentDocument:(PPDocument*)document handler:(void (^)(MADErr err, PPInstrumentObject* instrument))handler;
- (void)beginExportInstrument:(PPInstrumentObject*)anIns toURL:(NSURL*)sampURL driver:(PPDriver*)driver parentDocument:(PPDocument*)document handler:(PPPlugErrorBlock)handler;

@end
