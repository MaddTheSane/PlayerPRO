//
//  Preferences.h
//  PPMacho
//
//  Created by C.W. Betts on 7/14/10.
//  Copyright 2010 __MyCompanyName__. All rights reserved.
//

#import <Cocoa/Cocoa.h>

typedef NS_ENUM(NSInteger, PPPlaylistMode) {
	PPStopPlaying = 0,
	PPLoopMusic,
	PPLoadNext,
	PPLoadRandom
};

#pragma pack(push)
typedef struct TagCoupling {
	int amount;
	NSInteger tag;
} TagCoupling;
#pragma pack(pop)

@protocol PPPreferenceObject <NSObject>
+ (instancetype)newPreferenceView NS_RETURNS_RETAINED;
@property (nonatomic, readonly) NSString *preferenceIdentifier;
@end
