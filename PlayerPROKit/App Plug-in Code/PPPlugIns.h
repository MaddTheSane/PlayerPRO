//
//  PPPlugIns.h
//  PPMacho
//
//  Created by C.W. Betts on 9/6/14.
//
//

#ifndef __PLAYERPROKIT_PPPLUGINS_H__
#define __PLAYERPROKIT_PPPLUGINS_H__

#include <PlayerPROCore/PlayerPROCore.h>
#include <PlayerPROCore/MADPlug.h>
#import <Foundation/Foundation.h>

@class PPSampleObject;
@class PPInstrumentObject;
@class PPMusicObject;
@class PPDriver;
@class NSWindow;

NS_ASSUME_NONNULL_BEGIN

typedef void (^PPComplexImportHandler)(PPMusicObject* __nullable inMus, MADErr inErr);
typedef void (^PPPlugErrorBlock)(MADErr error);

/**
 *	@protocol	PPPlugin
 *	@abstract	base protocol for PlayerPRO 6 plug-ins.
 */
@protocol PPPlugin <NSObject>
/// Does the plug-in have a UI?
@property (nonatomic, readonly) BOOL hasUIConfiguration;
/// Initializer (constructor) for a plug-in.
- (nullable instancetype)initForPlugIn;
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
- (nullable instancetype)initForPlugIn;

@property (nonatomic, readonly) BOOL hasUIForImport;

- (BOOL)canImportSampleAtURL:(NSURL*)sampleURL;
- (MADErr)importSampleAtURL:(NSURL*)sampleURL sample:(out PPSampleObject* __nonnull* __nullable)sample driver:(PPDriver*)driver;

@optional
- (void)beginImportSampleAtURL:(NSURL*)sampleURL driver:(PPDriver*)driver parentWindow:(NSWindow*)window handler:(void (^)(MADErr error, PPSampleObject *__nullable sample))handler;
- (MADErr)playSampleAtURL:(NSURL*)aSample driver:(PPDriver*)driver;

@end


@protocol PPSampleExportPlugin <NSObject>
- (nullable instancetype)initForPlugIn;

@property (nonatomic, readonly) BOOL hasUIForExport;

- (MADErr)exportSample:(PPSampleObject*)sample toURL:(NSURL*)sampleURL driver:(PPDriver*)driver;

@optional
- (void)beginExportSample:(PPSampleObject*)asample toURL:(NSURL*)sampleURL driver:(PPDriver*)driver parentWindow:(NSWindow*)window handler:(PPPlugErrorBlock)handler;

@end


@protocol PPInstrumentImportPlugin <NSObject>
- (nullable instancetype)initForPlugIn;

@property (nonatomic, readonly) BOOL hasUIForImport;

- (BOOL)canImportInstrumentAtURL:(NSURL*)sampleURL;
- (MADErr)importInstrumentAtURL:(NSURL*)sampleURL instrument:(out PPInstrumentObject* __nonnull* __nullable)InsHeader driver:(PPDriver*)driver;

@optional
- (MADErr)playInstrumentAtURL:(NSURL*)aSample driver:(PPDriver*)driver;
- (void)beginImportInstrumentAtURL:(NSURL*)sampleURL driver:(PPDriver*)driver parentWindow:(NSWindow*)window handler:(void (^)(MADErr error, PPInstrumentObject *sample))handler;
@end


@protocol PPInstrumentExportPlugin <NSObject>
- (nullable instancetype)initForPlugIn;

@property (nonatomic, readonly) BOOL hasUIForExport;

- (MADErr)exportInstrument:(PPInstrumentObject*)InsHeader toURL:(NSURL*)sampleURL driver:(PPDriver*)driver;

@optional
- (void)beginExportInstrument:(PPInstrumentObject*)instrument toURL:(NSURL*)sampleURL driver:(PPDriver*)driver parentWindow:(NSWindow*)window handler:(PPPlugErrorBlock)handler;
@end

//This doesn't need to conform to PPPlugin because it will always have a UI.
@protocol PPComplexImportPlugInterface <NSObject>
- (nullable instancetype)initForPlugIn;

- (BOOL)canImportURL:(NSURL*)theURL error:(out NSError * __nullable __autoreleasing* __nullable)outErr;
- (void)beginImportOfURL:(NSURL*)theURL withHandler:(PPComplexImportHandler)handler;
@end

NS_ASSUME_NONNULL_END

#endif
