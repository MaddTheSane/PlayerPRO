//
//  PPInstrumentObject.h
//  PPMacho
//
//  Created by C.W. Betts on 1/10/13.
//
//

#import <Foundation/Foundation.h>
#include <PlayerPROCore/PlayerPROCore.h>
#import "PPObjectProtocol.h"

#ifndef NS_DESIGNATED_INITIALIZER
#define NS_DESIGNATED_INITIALIZER
#endif

@interface PPSampleObject : NSObject <PPObject>
@property (readwrite) NSInteger sampleIndex;
@property (readwrite) NSInteger instrumentIndex;
@property (readonly) sData theSample;

- (instancetype)init;
- (instancetype)initWithsData:(in sData *)theData;
- (instancetype)initWithSData:(in sData *)theData NS_DESIGNATED_INITIALIZER;
- (instancetype)initWithCoder:(NSCoder *)aDecoder NS_DESIGNATED_INITIALIZER;
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
@property (readonly) NSRange loop;


@property (readonly, getter=isBlankSample) BOOL blankSample;

@end
