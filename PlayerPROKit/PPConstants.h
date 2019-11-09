//
//  PPConstants.h
//  PPMacho
//
//  Created by C.W. Betts on 2/24/15.
//
//

#ifndef __PLAYERPROKIT_PPConstants_h__
#define __PLAYERPROKIT_PPConstants_h__

#import <Foundation/Foundation.h>

__BEGIN_DECLS

NS_ASSUME_NONNULL_BEGIN

#pragma mark PPLibrary info keys
//! PPLibrary dictionary info keys.
typedef NSString *PPLibraryInfoKeys NS_TYPED_ENUM;

/// The total number of patterns. Type is <code>NSNumber</code>.
extern PPLibraryInfoKeys const kPPTotalPatterns;
/// The length of a partition. Type is <code>NSNumber</code>.
extern PPLibraryInfoKeys const kPPPartitionLength;
/// The size of a file. Type is <code>NSNumber</code>.
///
/// \discussion You can ignore this and just use the system's file APIs to get a more accurate number.
extern PPLibraryInfoKeys const kPPFileSize;
/// The tracker's signature. Type is <code>NSString</code>.
///
/// \discussion The signature might not match the plug-in type. For example, the MOD tracker has many different signatures.
extern PPLibraryInfoKeys const kPPSignature;
/// The total number of tracks. Type is <code>NSNumber</code>.
extern PPLibraryInfoKeys const kPPTotalTracks;
/// The total number of instruments. Type is <code>NSNumber</code>.
extern PPLibraryInfoKeys const kPPTotalInstruments;
/// The title of the tracker. Type is <code>NSString</code>.
extern PPLibraryInfoKeys const kPPInternalFileName;
/// The description of the tracker. Type is <code>NSString</code>.
extern PPLibraryInfoKeys const kPPFormatDescription;

//! The options for converting a numerical note to a string.
typedef NS_OPTIONS(NSUInteger, PPSampleNoteOptions) {
	//! Use solfège of the <em>fixed do</em> scale instead of single letters.
	PPSampleNoteOptionSolfege = 1 << 0,
	//! Use the unicode sharp symbol instead of the pound sign.
	PPSampleNoteOptionUseSharpSymbol = 1 << 1,
};

NS_ASSUME_NONNULL_END

__END_DECLS

#endif
