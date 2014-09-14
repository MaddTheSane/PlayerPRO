//
//  PPPlugIns.h
//  PPMacho
//
//  Created by C.W. Betts on 9/6/14.
//
//

#include <PlayerPROCore/PlayerPROCore.h>
#import <Foundation/Foundation.h>

@class PPSampleObject;
@class PPInstrumentObject;
@class PPMusicObject;
@class PPDriver;
@class NSDocument;

typedef void (^PPComplexImportHandler)(PPMusicObject* inMus, MADErr inErr);
typedef void (^PPPlugErrorBlock)(MADErr error);

@protocol PPPlugin <NSObject>
@property (nonatomic, readonly) BOOL hasUIConfiguration;
@end

@protocol PPDigitalPlugin <PPPlugin, NSObject>

- (MADErr)runWithPcmd:(inout Pcmd*)aPcmd driver:(PPDriver *)driver;

@optional
- (void)beginRunWithPcmd:(Pcmd*)aPcmd driver:(PPDriver*)driver parentDocument:(NSDocument*)document handler:(PPPlugErrorBlock)handle;

@end

static inline NSInteger PPSelectionStart(NSRange selRange)
{
	return selRange.location;
}

static inline NSInteger PPSelectionEnd(NSRange selRange)
{
	return selRange.location + selRange.length;
}

@protocol PPFilterPlugin <PPPlugin, NSObject>

- (MADErr)runWithData:(inout PPSampleObject*)theData selectionRange:(NSRange)selRange onlyCurrentChannel:(BOOL)StereoMode driver:(PPDriver*)driver;

@optional
- (void)beginRunWithData:(PPSampleObject*)theData selectionRange:(NSRange)selRange onlyCurrentChannel:(BOOL)StereoMode driver:(PPDriver*)driver parentDocument:(NSDocument*)document handler:(PPPlugErrorBlock)handle;

@end

@protocol PPInstrumentImportPlugin <PPPlugin, NSObject>

- (BOOL)canImportSampleAtURL:(NSURL*)sampleURL;
@property (nonatomic, readonly) BOOL isInstrument;

- (MADErr)importSampleAtURL:(NSURL*)sampleURL instrument:(inout PPInstrumentObject*)InsHeader sample:(inout PPSampleObject*)sample sampleID:(inout short*)sampleID driver:(PPDriver*)driver;

@optional
- (MADErr)playSampleAtURL:(NSURL*)aSample driver:(PPDriver*)driver;
- (void)beginImportSampleAtURL:(NSURL*)sampleURL instrument:(PPInstrumentObject*)InsHeader sample:(PPSampleObject*)sample sampleID:(short*)sampleID driver:(PPDriver*)driver parentDocument:(NSDocument*)document handler:(PPPlugErrorBlock)handle;

@end

@protocol PPInstrumentExportPlugin <PPPlugin, NSObject>

@property (nonatomic, readonly) BOOL isInstrument;
- (MADErr)exportSampleToURL:(NSURL*)sampleURL instrument:(PPInstrumentObject*)InsHeader sample:(PPSampleObject*)sample sampleID:(short)sampleID driver:(PPDriver*)driver;

@optional
- (void)beginExportSampleToURL:(NSURL*)sampleURL instrument:(PPInstrumentObject*)InsHeader sample:(PPSampleObject*)sample sampleID:(short)sampleID driver:(PPDriver*)driver parentDocument:(NSDocument*)document handler:(PPPlugErrorBlock)handle;

@end

//This doesn't need to conform to PPPlugin because it will always have a UI.
@protocol PPComplexImportPlugInterface <NSObject>
- (BOOL)canImportURL:(NSURL*)theURL error:(out NSError**)outErr;
- (void)beginImportOfURL:(NSURL*)theURL withHandler:(PPComplexImportHandler)handler;
@end
