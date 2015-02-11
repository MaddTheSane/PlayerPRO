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
@class NSWindow;

typedef void (^PPComplexImportHandler)(PPMusicObject* inMus, MADErr inErr);
typedef void (^PPPlugErrorBlock)(MADErr error);

/**
 *	@protocol	PPPlugin
 *	@abstract	base protocol for PlayerPRO 6 plug-ins.
 */
@protocol PPPlugin <NSObject>
/// Does the plug-in have a UI?
@property (nonatomic, readonly) BOOL hasUIConfiguration;
/// Initializer (constructor) for a plug-in
- (instancetype)initForPlugIn;
@end

@protocol PPDigitalPlugin <PPPlugin, NSObject>

- (MADErr)runWithPcmd:(inout Pcmd*)aPcmd driver:(PPDriver *)driver;

@optional
- (void)beginRunWithPcmd:(Pcmd*)aPcmd driver:(PPDriver*)driver parentWindow:(NSWindow*)window handler:(PPPlugErrorBlock)handler;
@end

@protocol PPFilterPlugin <PPPlugin, NSObject>

- (MADErr)runWithData:(inout PPSampleObject*)theData selectionRange:(NSRange)selRange onlyCurrentChannel:(BOOL)StereoMode driver:(PPDriver*)driver;

@optional
- (void)beginRunWithData:(PPSampleObject*)theData selectionRange:(NSRange)selRange onlyCurrentChannel:(BOOL)StereoMode driver:(PPDriver*)driver parentWindow:(NSWindow*)window handler:(PPPlugErrorBlock)handler;
@end


@protocol PPSampleImportPlugin <NSObject>
- (instancetype)initForPlugIn;

@property (nonatomic, readonly) BOOL hasUIForImport;

- (BOOL)canImportSampleAtURL:(NSURL*)sampleURL;
- (MADErr)importSampleAtURL:(NSURL*)sampleURL sample:(out PPSampleObject**)sample driver:(PPDriver*)driver;

@optional
- (void)beginImportSampleAtURL:(NSURL*)sampleURL driver:(PPDriver*)driver parentWindow:(NSWindow*)window handler:(void (^)(MADErr error, PPSampleObject *sample))handler;
- (MADErr)playSampleAtURL:(NSURL*)aSample driver:(PPDriver*)driver;

@end

@protocol PPSampleExportPlugin <NSObject>
- (instancetype)initForPlugIn;

@property (nonatomic, readonly) BOOL hasUIForExport;

- (MADErr)exportSample:(PPSampleObject*)sample toURL:(NSURL*)sampleURL driver:(PPDriver*)driver;

@optional
- (void)beginExportSample:(PPSampleObject*)asample toURL:(NSURL*)sampleURL driver:(PPDriver*)driver parentWindow:(NSWindow*)window handler:(PPPlugErrorBlock)handler;


@end

@protocol PPInstrumentImportPlugin <NSObject>
- (instancetype)initForPlugIn;

@property (nonatomic, readonly) BOOL hasUIForImport;

- (BOOL)canImportInstrumentAtURL:(NSURL*)sampleURL;
- (MADErr)importInstrumentAtURL:(NSURL*)sampleURL instrument:(out PPInstrumentObject**)InsHeader driver:(PPDriver*)driver;

@optional
- (MADErr)playInstrumentAtURL:(NSURL*)aSample driver:(PPDriver*)driver;
- (void)beginImportInstrumentAtURL:(NSURL*)sampleURL driver:(PPDriver*)driver parentWindow:(NSWindow*)window handler:(void (^)(MADErr error, PPInstrumentObject *sample))handler;
@end

@protocol PPInstrumentExportPlugin <NSObject>
- (instancetype)initForPlugIn;

@property (nonatomic, readonly) BOOL hasUIForExport;

- (MADErr)exportInstrument:(PPInstrumentObject*)InsHeader toURL:(NSURL*)sampleURL driver:(PPDriver*)driver;

@optional
- (void)beginExportInstrument:(PPInstrumentObject*)instrument toURL:(NSURL*)sampleURL driver:(PPDriver*)driver parentWindow:(NSWindow*)window handler:(PPPlugErrorBlock)handler;
@end

//This doesn't need to conform to PPPlugin because it will always have a UI.
@protocol PPComplexImportPlugInterface <NSObject>
- (instancetype)initForPlugIn;

- (BOOL)canImportURL:(NSURL*)theURL error:(out NSError * __autoreleasing*)outErr;
- (void)beginImportOfURL:(NSURL*)theURL withHandler:(PPComplexImportHandler)handler;
@end
