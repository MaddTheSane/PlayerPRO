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
@property (readwrite) short sampleIndex;
@property (readwrite) short instrumentIndex;

- (id)init;
- (id)initWithsData:(sData *)theData;
- (sData *)createSData;

@property (readonly) SInt32 dataSize;
@property SInt32 loopBegin;
@property SInt32 loopSize;
@property unsigned char volume;
@property unsigned short c2spd;
@property unsigned char loopType;
@property unsigned char amplitude;
@property char relativeNote;
@property (copy) NSString *name;
@property (getter = isStereo) BOOL stereo;
@property (copy) NSData *data;

@end
