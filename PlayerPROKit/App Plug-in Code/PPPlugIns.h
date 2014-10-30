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

/*!
 *	\protocol	PPPlugin
 *	\abstract	base protocol for PlayerPRO 6 plug-ins.
 *	\property	hasUIConfiguration
 *				Does the plug-in have a UI?
 *	\c			initForPlugIn
 *				Initializer (constructor) for a plug-in
 */
@protocol PPPlugin <NSObject>
@property (nonatomic, readonly) BOOL hasUIConfiguration;
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

@protocol PPInstrumentImportPlugin <PPPlugin, NSObject>

@property (nonatomic, readonly) BOOL isInstrument;

- (BOOL)canImportSampleAtURL:(NSURL*)sampleURL;
- (MADErr)importSampleAtURL:(NSURL*)sampleURL instrument:(inout PPInstrumentObject*)InsHeader sample:(inout PPSampleObject*)sample sampleID:(inout short*)sampleID driver:(PPDriver*)driver;

@optional
- (MADErr)playSampleAtURL:(NSURL*)aSample driver:(PPDriver*)driver;
- (void)beginImportSampleAtURL:(NSURL*)sampleURL instrument:(PPInstrumentObject*)InsHeader sample:(PPSampleObject*)sample sampleID:(short*)sampleID driver:(PPDriver*)driver parentWindow:(NSWindow*)window handler:(PPPlugErrorBlock)handler;
@end

@protocol PPInstrumentExportPlugin <PPPlugin, NSObject>

@property (nonatomic, readonly) BOOL isInstrument;

- (MADErr)exportSampleToURL:(NSURL*)sampleURL instrument:(PPInstrumentObject*)InsHeader sample:(PPSampleObject*)sample sampleID:(short)sampleID driver:(PPDriver*)driver;

@optional
- (void)beginExportSampleToURL:(NSURL*)sampleURL instrument:(PPInstrumentObject*)InsHeader sample:(PPSampleObject*)sample sampleID:(short)sampleID driver:(PPDriver*)driver parentWindow:(NSWindow*)window handler:(PPPlugErrorBlock)handler;
@end

//This doesn't need to conform to PPPlugin because it will always have a UI.
@protocol PPComplexImportPlugInterface <NSObject>
- (instancetype)initForPlugIn;

- (BOOL)canImportURL:(NSURL*)theURL error:(out NSError**)outErr;
- (void)beginImportOfURL:(NSURL*)theURL withHandler:(PPComplexImportHandler)handler;
@end
