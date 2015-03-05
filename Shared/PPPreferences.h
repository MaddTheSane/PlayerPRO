//
//  Preferences.h
//  PPMacho
//
//  Created by C.W. Betts on 7/14/10.
//  Copyright 2010 __MyCompanyName__. All rights reserved.
//

#import <Cocoa/Cocoa.h>

#pragma pack(push)
typedef struct TagCoupling {
	int amount;
	NSInteger tag;
} TagCoupling;
#pragma pack(pop)

@protocol PPPreferenceObject <NSObject>
+ (nullable instancetype)newPreferenceView NS_RETURNS_RETAINED;
@property (nonatomic, readonly, nonnull) NSString *preferenceIdentifier;
@end
