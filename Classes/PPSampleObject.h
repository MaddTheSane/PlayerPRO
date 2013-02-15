//
//  PPInstrumentObject.h
//  PPMacho
//
//  Created by C.W. Betts on 1/10/13.
//
//

#import <Foundation/Foundation.h>
#include <PlayerPROCore/PlayerPROCore.h>

@interface PPSampleObject : NSObject <NSCopying, NSCoding>
{
	sData		theSample;
	NSString	*name;
	NSData		*data;
	short		sampleIndex;
	short		instrumentIndex;
}

@property (readwrite) short sampleIndex;
@property (readwrite) short instrumentIndex;

- (id)init;
- (id)initWithsData:(sData *)theData;
- (sData *)createSData;

@property (readonly) SInt32 dataSize;
@property (readwrite) SInt32 loopBegin;
@property (readwrite) SInt32 loopSize;
@property (readwrite) unsigned char volume;
@property (readwrite) unsigned short c2spd;
@property (readwrite) unsigned char loopType;
@property (readwrite) unsigned char amplitude;
@property (readwrite) char relativeNote;
@property (readwrite, copy) NSString *name;
@property (readwrite, getter = isStereo) BOOL stereo;
@property (readwrite, copy) NSData *data;

@end
