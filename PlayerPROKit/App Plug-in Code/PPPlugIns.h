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

/**
 *
 *	@param		inMus
 *				The music object that was imported. Should be \c nil if
 *				\c inErr is <code>MADNoErr</code>.
 *	@param		inErr
 *				The \c MADErr returned by the plug-in, or \c MADNoErr on success.
 *	@abstract	This block is passed to complex plug-ins.
 */
typedef void (^PPComplexImportHandler)(PPMusicObject* __nullable inMus, MADErr inErr);
/**
 *
 *	@param		error
 *				The \c MADErr returned by the plug-in, or \c MADNoErr on success.
 *	@abstract	This block is passed to plug-ins that need a UI for user interaction.
 */
typedef void (^PPPlugErrorBlock)(MADErr error);

/**
 *	@protocol	PPPlugin
 *	@abstract	base protocol for PlayerPRO 6 plug-ins.
 */
@protocol PPPlugin <NSObject>

/// Initializer (constructor) for a plug-in.
- (nullable instancetype)initForPlugIn;

@end

/**
 *	@protocol	PPBasePlugin
 *	@abstract	Base protocol for PlayerPRO 6 plug-ins with an optional UI.
 */
@protocol PPBasePlugin <PPPlugin, NSObject>
/// Does the plug-in have a UI?
@property (nonatomic, readonly) BOOL hasUIConfiguration;

@end

/**
 *	@protocol	PPBaseImportPlugin
 *	@abstract	Base protocol for PlayerPRO 6 plug-ins with an optional UI used for importing.
 */
@protocol PPBaseImportPlugin <PPPlugin, NSObject>
/// Does the plug-in have a UI for importing?
@property (nonatomic, readonly) BOOL hasUIForImport;

@end

/**
 *	@protocol	PPBaseExportPlugin
 *	@abstract	base protocol for PlayerPRO 6 plug-ins with an optional UI used for exporting.
 */
@protocol PPBaseExportPlugin <PPPlugin, NSObject>
/// Does the plug-in have a UI for exporting?
@property (nonatomic, readonly) BOOL hasUIForExport;
@end

/**
 *	@protocol	PPDigitalPlugin
 *	@abstract	Plug-in that modifies the digital notes.
 */
@protocol PPDigitalPlugin <PPBasePlugin, NSObject>

/**
 *	@method		runWithPcmd:driver:
 *	@abstract	Run the plug-in that will modify the sent-in Pcmd.
 *	@param		aPcmd
 *				the \c Pcmd to modify.
 *	@param		driver
 *				The \c PPDriver to send in, just in case more info is needed.
 *	@return		A \c MADErr on failure, \c MADNoErr on succes, or 
 *				\c MADParametersErr if a UI is needed by the plug-in.
 *	@discussion	If this returns \c MADParametersErr and the plug-in doesn't implement
 *				<code>beginRunWithPcmd:driver:parentWindow:handler:</code>,
 *				PlayerPRO 6 will not attempt to call the UI function and will treat it
 *				as an error.
 *	@sa			beginRunWithPcmd:driver:parentWindow:handler:
 */
- (MADErr)runWithPcmd:(inout Pcmd*)aPcmd driver:(PPDriver *)driver;

@optional
/**
 *	@method		beginRunWithPcmd:driver:parentWindow:handler:
 *	@abstract	Put up a sheet that will help the user modify the <code>Pcmd</code>.
 *	@param		aPcmd
 *				the \c Pcmd to modify.
 *	@param		driver
 *				The \c PPDriver to send in, just in case more info is needed.
 *	@param		window
 *				The window to have the sheet come out of.
 *	@param		handler
 *				The block passed in. Should be called when the window is done.
 *	@discussion	This is useful if there needs to be more options for modifying the notes.
 *	@sa			runWithPcmd:driver:
 */
- (void)beginRunWithPcmd:(Pcmd*)aPcmd driver:(PPDriver*)driver parentWindow:(NSWindow*)window handler:(PPPlugErrorBlock)handler;

@end

/**
 *	@protocol	PPFilterPlugin
 *	@abstract	Plug-in that modifies a sample, usually by modifying the PCM waves.
 */
@protocol PPFilterPlugin <PPBasePlugin, NSObject>

/**
 *	@method		runWithData:selectionRange:onlyCurrentChannel:driver:
 *	@abstract	Run the plug-in that will modify the sent-in Pcmd.
 *	@param		theData
 *				the \c PPSampleObject to modify.
 *	@param		selRange
 *				The range to apply the changes to.
 *	@param		StereoMode
 *				If the sample is stereo and if <code>YES</code>, modifies the right channel.
 *				Otherwise, modifies the right or mono channel.
 *	@param		driver
 *				The \c PPDriver to send in, just in case more info is needed.
 *	@return		A \c MADErr on failure, \c MADNoErr on succes, or
 *				\c MADParametersErr if a UI is needed by the plug-in.
 *	@discussion	If this returns \c MADParametersErr and the plug-in doesn't implement
 *				<code>beginRunWithData:driver:parentWindow:handler:</code>,
 *				PlayerPRO 6 will not attempt to call the UI function and will treat it
 *				as an error.
 *	@sa			beginRunWithData:driver:parentWindow:handler:
 */
- (MADErr)runWithData:(inout PPSampleObject*)theData selectionRange:(NSRange)selRange onlyCurrentChannel:(BOOL)StereoMode driver:(PPDriver*)driver;

@optional
/**
 *	@method		beginRunWithData:driver:parentWindow:handler:
 *	@abstract	Put up a sheet that will help the user modify the sample object.
 *	@param		theData
 *				the \c PPSampleObject to modify.
 *	@param		selRange
 *				The range to apply the changes to.
 *	@param		StereoMode
 *				If the sample is stereo and if <code>YES</code>, modifies the right channel.
 *				Otherwise, modifies the right or mono channel.
 *	@param		driver
 *				The \c PPDriver to send in, just in case more info is needed.
 *	@param		window
 *				The window to have the sheet come out of.
 *	@param		handler
 *				The block passed in. Should be called when the window is done.
 *	@discussion	This is useful if there needs to be more options for modifying the sample.
 *	@sa			runWithData:selectionRange:onlyCurrentChannel:driver:
 */
- (void)beginRunWithData:(PPSampleObject*)theData selectionRange:(NSRange)selRange onlyCurrentChannel:(BOOL)StereoMode driver:(PPDriver*)driver parentWindow:(NSWindow*)window handler:(PPPlugErrorBlock)handler;
@end


/**
 *	@protocol	PPSampleImportPlugin
 *	@abstract	Plug-in that imports samples for an instrument from a third-party format.
 */
@protocol PPSampleImportPlugin <PPBaseImportPlugin, NSObject>

/**
 *	@method		canImportSampleAtURL:
 *	@abstract	Check if the file URL passed in can be opened by the plug-in.
 *	@param		sampleURL
 *				The file URL pointing to the sample to check.
 *	@return		\c YES if the plug-in can import the file, \c NO on failure.
 */
- (BOOL)canImportSampleAtURL:(NSURL*)sampleURL;

/**
 *	@method		importSampleAtURL:sample:driver:
 *	@abstract	Tells the plug-in to import the sample at <code>sampleURL</code>.
 *	@param		sampleURL
 *				The file URL pointing to the sample to import.
 *	@param		sample
 *				A pointer to the sample object to be returned. The pointer must 
 *				be set to \c nil on failure.
 *	@param		driver
 *				The \c PPDriver to send in, just in case more info is needed.
 *	@return		A \c MADErr on failure, \c MADNoErr on succes, or
 *				\c MADParametersErr if a UI is needed by the plug-in.
 *	@discussion	If this returns \c MADParametersErr and the plug-in doesn't implement
 *				<code>beginImportSampleAtURL:driver:parentWindow:handler:</code>,
 *				PlayerPRO 6 will not attempt to call the UI function and will treat it 
 *				as an error.
 *	@sa			beginImportSampleAtURL:driver:parentWindow:handler:
 */
- (MADErr)importSampleAtURL:(NSURL*)sampleURL sample:(out PPSampleObject* __nonnull* __nullable)sample driver:(PPDriver*)driver;

@optional
/**
 *	@method		beginImportSampleAtURL:driver:parentWindow:handler:
 *	@abstract	Pull up a sheet with additional options for importing the
 *				sample from <code>sampleURL</code>.
 *	@param		sampleURL
 *				The file URL  pointing to the sample to import.
 *	@param		driver
 *				The \c PPDriver to send in, just in case more info is needed.
 *	@param		window
 *				The window to have the sheet come out of.
 *	@param		handler
 *				The handler passed in. On error, set the \c error parameter to the error, and 
 *				\c sample to <code>nil</code>. On success, set \c error to \c MADNoErr and the 
 *				\c sample to the created sample.
 *	@discussion	This is useful if there needs to be more options for importing a specific sample.
 *	@sa			importSampleAtURL:sample:driver:
 */
- (void)beginImportSampleAtURL:(NSURL*)sampleURL driver:(PPDriver*)driver parentWindow:(NSWindow*)window handler:(void (^)(MADErr error, PPSampleObject *__nullable sample))handler;

/**
 *	@method		playSampleAtURL:driver:
 *	@abstract	Plays the sample pointed to by the \c aSample file URL.
 *	@param		aSample
 *				The file URL where the sample is that the user wants to play.
 *	@param		driver
 *				The driver passed in to play the sample.
 *	@return		A \c MADErr on failure or \c MADNoErr on succes.
 */
- (MADErr)playSampleAtURL:(NSURL*)aSample driver:(PPDriver*)driver;

@end


/**
 *	@protocol	PPSampleExportPlugin
 *	@abstract	Plug-in that exports samples in an instrument to a third-party format.
 */
@protocol PPSampleExportPlugin <PPBaseExportPlugin, NSObject>

/**
 *	@method		exportSample:toURL:driver:
 *	@abstract	Export the selected sample to the specified file URL.
 *	@param		sample
 *				The sample to export.
 *	@param		sampleURL
 *				The file URL to export the sample to.
 *	@param		driver
 *				The \c PPDriver to send in, just in case more info is needed.
 *	@return		A \c MADErr on failure, \c MADNoErr on succes, or
 *				\c MADParametersErr if a UI is needed by the plug-in.
 *	@discussion	If this returns \c MADParametersErr and the plug-in doesn't implement
 *				<code>beginRunWithPcmd:driver:parentWindow:handler:</code>,
 *				PlayerPRO 6 will not attempt to call the UI function and will treat it
 *				as an error.
 *	@sa			beginExportSample:toURL:driver:parentWindow:handler:
 */
- (MADErr)exportSample:(PPSampleObject*)sample toURL:(NSURL*)sampleURL driver:(PPDriver*)driver;

@optional
/**
 *	@method		beginExportSample:toURL:driver:parentWindow:handler:
 *	@abstract	Pull up a sheet with additional options for exporting the
 *				selected sample to <code>sampleURL</code>.
 *	@param		asample
 *				The sample to begin exporting.
 *	@param		sampleURL
 *				The file URL to save the sample to.
 *	@param		driver
 *				The \c PPDriver to send in, just in case more info is needed.
 *	@param		window
 *				The window to have the sheet come out of.
 *	@param		handler
 *				The block passed in. Should be called when the window is done.
 *	@discussion	This is useful if there needs to be more options for exporting a specific sample.
 *	@sa			exportSample:toURL:driver:
 */
- (void)beginExportSample:(PPSampleObject*)asample toURL:(NSURL*)sampleURL driver:(PPDriver*)driver parentWindow:(NSWindow*)window handler:(PPPlugErrorBlock)handler;

@end


/**
 *	@protocol	PPInstrumentImportPlugin
 *	@abstract	Plug-in that imports a third-party instrument format.
 */
@protocol PPInstrumentImportPlugin <PPBaseImportPlugin, NSObject>

/**
 *	@method		canImportInstrumentAtURL:
 *	@abstract	Check if the file URL passed in can be opened by the plug-in.
 *	@param		sampleURL
 *				The file URL pointing to the instrument to test.
 *	@return		\c YES if the plug-in can import the file, \c NO on failure.
 */
- (BOOL)canImportInstrumentAtURL:(NSURL*)sampleURL;

/**
 *	@method		importInstrumentAtURL:instrument:driver:
 *	@param		sampleURL
 *				The file URL pointing to the file to import.
 *	@param		InsHeader
 *				The newly created instrument object, or \c nil on failure.
 *	@param		driver
 *				The \c PPDriver to send in, just in case more info is needed.
 *	@return		A \c MADErr on failure, \c MADNoErr on succes, or
 *				\c MADParametersErr if a UI is needed by the plug-in.
 *	@discussion	If this returns \c MADParametersErr and the plug-in doesn't implement
 *				<code>beginRunWithPcmd:driver:parentWindow:handler:</code>,
 *				PlayerPRO 6 will not attempt to call the UI function and will treat it
 *				as an error.
 *	@sa			beginImportInstrumentAtURL:driver:parentWindow:handler:
 */
- (MADErr)importInstrumentAtURL:(NSURL*)sampleURL instrument:(out PPInstrumentObject* __nonnull* __nullable)InsHeader driver:(PPDriver*)driver;

@optional

/**
 *	@method		playInstrumentAtURL:driver:
 *	@abstract	Plays the instrument pointed to by the \c aSample file URL.
 *	@param		aSample
 *				The file URL where the instrument is that the user wants to play.
 *	@param		driver
 *				The driver passed in to play the instrument.
 *	@return		A \c MADErr on failure or \c MADNoErr on succes.
 */
- (MADErr)playInstrumentAtURL:(NSURL*)aSample driver:(PPDriver*)driver;

/**
 *	@method		beginImportInstrumentAtURL:driver:parentWindow:handler:
 *	@abstract	Pull up a sheet with additional options for importing the
 *				instrument from <code>sampleURL</code>.
 *	@param		sampleURL
 *				The file URL pointing to the file to import.
 *	@param		driver
 *				The \c PPDriver to send in, just in case more info is needed.
 *	@param		window
 *				The window to have the sheet come out of.
 *	@param		handler
 *				The block passed in. Should be called when the window is done.<br>
 *				On failure, pass the \c MADErr closest to the error that occured to 
 *				<code>error</code>, and \c nil for <code>sample</code>.
 *				Otherwise, pass \c MADNoErr for \c error and the created instrument to
 *				<code>sample</code>.
 *	@discussion	This is useful if there needs to be more options for
 *				importing a specific instrument.
 *	@sa			importInstrumentAtURL:instrument:driver:
 */
- (void)beginImportInstrumentAtURL:(NSURL*)sampleURL driver:(PPDriver*)driver parentWindow:(NSWindow*)window handler:(void (^)(MADErr error, PPInstrumentObject *__nullable sample))handler;
@end


/**
 *	@protocol	PPInstrumentExportPlugin
 *	@abstract	Plug-in that exports to a third-party instrument format.
 */
@protocol PPInstrumentExportPlugin <PPBaseExportPlugin, NSObject>
/**
 *	@method		exportInstrument:toURL:driver:
 *	@abstract	Exports the selected instrument to the specified file URL.
 *	@property	InsHeader
 *				The instrument to export
 *	@property	sampleURL
 *				The file URL to write to.
 *	@param		driver
 *				The \c PPDriver to send in, just in case more info is needed.
 *	@return		A \c MADErr on failure, \c MADNoErr on succes, or
 *				\c MADParametersErr if a UI is needed by the plug-in.
 *	@discussion	If this returns \c MADParametersErr and the plug-in doesn't implement
 *				<code>beginExportInstrument:toURL:driver:parentWindow:handler:</code>,
 *				PlayerPRO 6 will not attempt to call the UI function and will treat it
 *				as an error.
 *	@sa			beginExportInstrument:toURL:driver:parentWindow:handler:
 */
- (MADErr)exportInstrument:(PPInstrumentObject*)InsHeader toURL:(NSURL*)sampleURL driver:(PPDriver*)driver;

@optional
/**
 *	@method		beginExportInstrument:toURL:driver:parentWindow:handler:
 *	@abstract	Pull up a sheet with additional options for exporting the 
 *				selected instrument to <code>sampleURL</code>.
 *	@property	instrument
 *				The instrument to export.
 *	@property	sampleURL
 *				The file URL to write the instrument to.
 *	@param		driver
 *				The \c PPDriver to send in, just in case more info is needed.
 *	@param		window
 *				The window to have the sheet come out of.
 *	@param		handler
 *				The block passed in. Should be called when the window is done.
 *	@discussion	This is useful if there needs to be more options for
 *				exporting a specific instrument.
 *	@sa			exportInstrument:toURL:driver:
 */
- (void)beginExportInstrument:(PPInstrumentObject*)instrument toURL:(NSURL*)sampleURL driver:(PPDriver*)driver parentWindow:(NSWindow*)window handler:(PPPlugErrorBlock)handler;
@end

/**
 *	@protocol	PPComplexImportPlugInterface
 *	@discussion	This doesn't need to conform to \c PPCorePlugin because
 *				it will <b>always</b> have a UI.
 */
@protocol PPComplexImportPlugInterface <PPPlugin, NSObject>

/**
 *	@method		canImportURL:error:
 *	@abstract	Check to see if the plug-in can import the file at the specified URL.
 *	@param		theURL
 *				The file URL to check.
 *	@param		outErr
 *				If returned <code>NO</code>, should be filled with an error object
 *				describing why it failed.
 *	@return		\c YES if the file can be imported, or \c NO otherwise.
 */
- (BOOL)canImportURL:(NSURL*)theURL error:(out NSError * __nullable __autoreleasing* __nullable)outErr;

/**
 *	@method		beginImportOfURL:withHandler:
 *	@abstract	Bring up a window for importing the file at the specified URL.
 *	@param		theURL
 *				The file URL to import.
 *	@param		handler
 *				The block passed in. Should be called when the window is closed.
 */
- (void)beginImportOfURL:(NSURL*)theURL withHandler:(PPComplexImportHandler)handler;
@end

NS_ASSUME_NONNULL_END

#endif
