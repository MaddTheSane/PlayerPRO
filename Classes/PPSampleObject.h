//
//  PPInstrumentObject.h
//  PPMacho
//
//  Created by C.W. Betts on 1/10/13.
//
//

#import <Foundation/Foundation.h>
#include <PlayerPROCore/PlayerPROCore.h>

@interface PPSampleObject : NSObject
{
	SInt32				loopBeg;			// LoopStart
	SInt32				loopSize;			// LoopLength
	unsigned char		vol;				// Base volume
	unsigned short		c2spd;				// c2spd
	unsigned char		loopType;
	unsigned char		amp;				// 8 or 16 bits
	char				relNote;
	NSString 			*name;				// Sample name
	BOOL				stereo;				// Stereo
	NSData				*data;				// Used only in memory, not in files

}

- (id)initWithsData:(sData *)theData;

@property (readonly) SInt32 dataSize;
@property (readwrite) SInt32 loopBegin;
@property (readwrite) SInt32 loopSize;
@property (readwrite) unsigned char volume;
@property (readwrite) unsigned short c2spd;
@property (readwrite) unsigned char loopType;
@property (readwrite) unsigned char amplitude;
@property (readwrite) char relativeNote;
@property (readwrite, retain) NSString *name;
@property (readonly) BOOL stereo;
@property (readwrite, retain) NSData *data;

@end
