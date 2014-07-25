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

@interface PPSampleObject : NSObject <PPObject>
@property (readwrite) NSInteger sampleIndex;
@property (readwrite) NSInteger instrumentIndex;
@property (readonly) sData theSample;

- (instancetype)init;
- (instancetype)initWithsData:(in sData *)theData;
- (instancetype)initWithSData:(in sData *)theData;
- (sData *)createSData;

- (int)dataSize DEPRECATED_ATTRIBUTE;
@property int loopBegin;
@property int loopSize;
@property MADByte volume;
@property unsigned short c2spd;
@property MADLoopType loopType;
@property MADByte amplitude;
@property char relativeNote;
@property (copy) NSString *name;
@property (getter = isStereo) BOOL stereo;
@property (copy) NSData *data;

@end
