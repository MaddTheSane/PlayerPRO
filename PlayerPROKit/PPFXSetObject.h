//
//  PPFXSetObject.h
//  PPMacho
//
//  Created by C.W. Betts on 1/8/14.
//
//

#ifndef __PLAYERPROKIT_PPFXSETOBJECT_H__
#define __PLAYERPROKIT_PPFXSETOBJECT_H__

#import <Foundation/Foundation.h>
#include <PlayerPROCore/MAD.h>
#import <PlayerPROKit/PPObjectProtocol.h>

#ifndef NS_DESIGNATED_INITIALIZER
#define NS_DESIGNATED_INITIALIZER
#endif

@interface PPFXSetObject : NSObject <PPObject>
@property (readonly) FXSets theSet;
@property short track;
@property short identifier;
@property int effectIdentifier;
@property short countOfArguments;
@property (readonly, copy, nonnull) NSArray<NSNumber*> *effectValues;
@property (copy, null_resettable) NSString *name;

- (nonnull instancetype)initWithFXSet:(nullable FXSets*)theSet NS_DESIGNATED_INITIALIZER;

- (float)effectValueAtIndex:(NSInteger)idx;

- (void)replaceEffectValueAtIndex:(NSInteger)theLoc withFloat:(float)theNum;

/// The native value of the effect values is float.
/// As such, passing a double will result in a loss of precision.
- (void)replaceEffectValueAtIndex:(NSInteger)theLoc withDouble:(double)theNum DEPRECATED_ATTRIBUTE;
- (void)replaceEffectValueAtIndex:(NSInteger)theLoc withInt:(int)theNum;
- (void)replaceEffectValueAtIndex:(NSInteger)theLoc withInteger:(NSInteger)theNum;
- (void)replaceEffectValueAtIndex:(NSInteger)theLoc withNumber:(nonnull NSNumber*)theNum;

@end

#endif
