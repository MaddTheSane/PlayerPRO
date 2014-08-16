//
//  PPMIDIImportHelper.h
//  PPMacho
//
//  Created by C.W. Betts on 8/6/14.
//
//

#import <Foundation/Foundation.h>
#include <PlayerPROCore/PPDefs.h>

#define kPPTotalPatterns		"Total Patterns"
#define kPPPartitionLength		"Partition Length"
#define kPPFileSize				"File Size"
#define kPPSignature			"Signature"
#define kPPTotalTracks			"Total Tracks"
#define kPPTotalInstruments		"Total Instruments"
#define kPPInternalFileName		"Internal File Name"
#define kPPFormatDescription	"FormatDescription"
#define kPPMIDIErr				"Error Value"
#define kPPMIDICall				"PlugInCall"
#define kPPMIDIInfo				"Info"

typedef NS_ENUM(NSInteger, PPMIDIImportValue) {
	PPImportMIDI,
	PPInfoMIDI,
	PPTestMIDI
};

//extern MADErr ImportMIDIFile(NSURL* theURL, NSData **);

@interface PPMIDIImporter : NSObject
- (void)importMIDIFileAtURL:(NSURL*)theURL withReply:(void (^)(NSData *, MADErr error))reply;
- (void)getMIDIInfoFromFileAtURL:(NSURL*)theURL withReply:(void (^)(NSDictionary *, MADErr error))reply;
- (void)canImportMIDIFileAtURL:(NSURL*)theURL withReply:(void (^)(MADErr error))reply;

@end
