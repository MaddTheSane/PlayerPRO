//
//  PPInstrumentObject.m
//  PPMacho
//
//  Created by C.W. Betts on 1/10/13.
//
//

#import "PPSampleObject.h"

@implementation PPSampleObject

@synthesize stereo;
@synthesize name;
@synthesize loopType;
@synthesize c2spd;
@synthesize amplitude = amp;
@synthesize volume = vol;
@synthesize data;
@synthesize loopBegin;
@synthesize loopSize;
@synthesize relativeNote = relNote;
@synthesize dataSize = size;


- (id)initWithsData:(sData *)theData
{
	if (self = [super init]) {
		data = [[NSData alloc] initWithBytes:theData->data length:theData->size];
		size = theData->size;
		loopBeg = theData->loopBeg;
		loopSize = theData->loopSize;
		vol = theData->vol;
		c2spd = theData->c2spd;
		loopType = theData->loopType;
		amp = theData->amp;
		relNote = theData->relNote;
		name = [[NSString alloc] initWithCString:theData->name encoding:NSMacOSRomanStringEncoding];
		stereo = theData->stereo;
		
	}
	return self;
}

@end
