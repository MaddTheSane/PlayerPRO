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
	SInt32 				size;				// Sample length
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
@property (readonly) SInt32 loopBegin;
@property (readonly) SInt32 loopSize;
@property (readonly) unsigned char volume;
@property (readonly) unsigned short c2spd;
@property (readonly) unsigned char loopType;
@property (readonly) unsigned char amplitude;
@property (readonly) char relativeNote;
@property (readonly) NSString *name;
@property (readonly) BOOL stereo;
@property (readonly) NSData *data;

@end
