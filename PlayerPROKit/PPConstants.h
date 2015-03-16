//
//  PPConstants.h
//  PPMacho
//
//  Created by C.W. Betts on 2/24/15.
//
//

#ifndef PPMacho_PPConstants_h
#define PPMacho_PPConstants_h

#import <Foundation/Foundation.h>

__BEGIN_DECLS
#pragma mark PPLibrary info keys

/// The total number of patterns. Type is <code>NSNumber</code>.
extern NSString * const kPPTotalPatterns;
/// The length of a partition. Type is <code>NSNumber</code>.
extern NSString * const kPPPartitionLength;
/// The size of a file. Type is <code>NSNumber</code>.
///
/// \discussion You can ignore this and just use the system's file APIs to get a more accurate number
extern NSString * const kPPFileSize;
/// The tracker's signature. Type is <code>NSString</code>.
///
/// \discussion The signature might not match the plug-in type. For example, the MOD tracker has many different signatures.
extern NSString * const kPPSignature;
/// The total number of tracks. Type is <code>NSNumber</code>.
extern NSString * const kPPTotalTracks;
/// The total number of instruments. Type is <code>NSNumber</code>.
extern NSString * const kPPTotalInstruments;
/// The title of the tracker. Type is <code>NSString</code>.
extern NSString * const kPPInternalFileName;
/// The description of the tracker. Type is <code>NSString</code>.
extern NSString * const kPPFormatDescription;
__END_DECLS


#endif
