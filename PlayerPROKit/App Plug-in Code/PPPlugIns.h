//
//  PPPlugIns.h
//  PPMacho
//
//  Created by C.W. Betts on 9/6/14.
//
//

#import <Foundation/Foundation.h>
#include <PlayerPROCore/PlayerPROCore.h>
@class PPSampleObject;
@class PPInstrumentObject;
@class PPMusicObject;

typedef void (^PPComplexImportHandler)(PPMusicObject* inMus, MADErr inErr);

@protocol PPPlugin <NSObject>
@required
+ (BOOL)hasUIConfiguration;
@end

@protocol PPDigitalPlugin <PPPlugin, NSObject>

@required
- (MADErr)runWithPcmd:(inout Pcmd*)aPcmd pluginInfo:(in PPInfoPlug *)info;

@optional
- (void)beginRunWithPcmd:(Pcmd*)aPcmd pluginInfo:(PPInfoPlug *)info handler:(void (^)(MADErr error))handle;

@end

@protocol PPFilterPlugin <PPPlugin, NSObject>

@required
- (MADErr)runWithData:(inout PPSampleObject*)theData selectionRange:(NSRange)selRange onlyCurrentChannel:(BOOL)StereoMode pluginInfo:(in PPInfoPlug*)info;

@optional
- (void)beginRunWithData:(PPSampleObject*)theData selectionRange:(NSRange)selRange onlyCurrentChannel:(BOOL)StereoMode pluginInfo:(in PPInfoPlug*)info handler:(void (^)(MADErr error))handle;

@end

@protocol PPInstrumentImportPlugin <PPPlugin, NSObject>

@required

- (BOOL)canImportSampleAtURL:(NSURL*)sampleURL;
+ (BOOL)isInstrument;

- (MADErr)importSampleAtURL:(NSURL*)sampleURL instrument:(inout PPInstrumentObject*)InsHeader sample:(inout PPSampleObject*)sample sampleID:(inout short*)sampleID pluginInfo:(in PPInfoPlug *)info;

@optional
- (MADErr)playSampleAtURL:(NSURL*)aSample;
- (void)beginImportSampleAtURL:(NSURL*)sampleURL instrument:(PPInstrumentObject*)InsHeader sample:(PPSampleObject*)sample sampleID:(short*)sampleID pluginInfo:(PPInfoPlug *)info handler:(void (^)(MADErr error))handle;

@end

@protocol PPInstrumentExportPlugin <PPPlugin, NSObject>

+ (BOOL)isInstrument;
- (MADErr)exportSampleToURL:(NSURL*)sampleURL instrument:(PPInstrumentObject*)InsHeader sample:(PPSampleObject*)sample sampleID:(short)sampleID pluginInfo:(PPInfoPlug *)info;

@optional
- (void)beginExportSampleToURL:(NSURL*)sampleURL instrument:(PPInstrumentObject*)InsHeader sample:(PPSampleObject*)sample sampleID:(short)sampleID pluginInfo:(PPInfoPlug *)info handler:(void (^)(MADErr error))handle;

@end

//This doesn't need to conform to PPPlugin because it will always have a UI.
@protocol PPComplexImportPlugInterfaceBase <NSObject>
- (BOOL)canImportURL:(NSURL*)theURL error:(out NSError**)outErr;
- (void)beginImportOfURL:(NSURL*)theURL withHandler:(PPComplexImportHandler)handler;
@end

@protocol PPComplexImportPlugInterface <PPComplexImportPlugInterfaceBase>
- (instancetype)init;
@end
