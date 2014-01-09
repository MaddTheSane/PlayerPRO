//
//  PPInstrumentObject.h
//  PPMacho
//
//  Created by C.W. Betts on 1/10/13.
//
//

#import <Foundation/Foundation.h>
#include <PlayerPROCore/PlayerPROCore.h>
#import <PlayerPROKit/PPObjectProtocol.h>

@interface PPSampleObject : NSObject <NSCopying, NSCoding, PPObject>
@property (readwrite) NSInteger sampleIndex;
@property (readwrite) NSInteger instrumentIndex;
@property (readonly) sData theSample;

- (id)init;
- (id)initWithsData:(sData *)theData;
- (sData *)createSData;

@property (readonly) SInt32 dataSize DEPRECATED_ATTRIBUTE;
@property int loopBegin;
@property int loopSize;
@property Byte volume;
@property unsigned short c2spd;
@property Byte loopType;
@property Byte amplitude;
@property char relativeNote;
@property (copy) NSString *name;
@property (getter = isStereo) BOOL stereo;
@property (copy) NSData *data;

@end
