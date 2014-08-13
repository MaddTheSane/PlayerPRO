//
//  PPFXSetObject.h
//  PPMacho
//
//  Created by C.W. Betts on 1/8/14.
//
//

#import <Foundation/Foundation.h>
#include <PlayerPROCore/PlayerPROCore.h>
#import <PlayerPROKit/PPObjectProtocol.h>

#ifndef NS_DESIGNATED_INITIALIZER
#define NS_DESIGNATED_INITIALIZER
#endif

@interface PPFXSetObject : NSObject <PPObject>
@property (readonly) FXSets theSet;
@property short track;
@property short identifier;
@property int fxIdentifier;
@property short argumentNumbers;
@property (readonly, unsafe_unretained) NSArray *values;
@property (copy) NSString *name;

- (instancetype)initWithFXSet:(FXSets*)theSet;

- (void)replaceSetAtIndex:(NSInteger)theLoc withFloat:(float)theNum;

//The native value of the sets is float.
- (void)replaceSetAtIndex:(NSInteger)theLoc withDouble:(double)theNum DEPRECATED_ATTRIBUTE;
- (void)replaceSetAtIndex:(NSInteger)theLoc withInt:(int)theNum;
- (void)replaceSetAtIndex:(NSInteger)theLoc withInteger:(NSInteger)theNum;
- (void)replaceSetAtIndex:(NSInteger)theLoc withNumber:(NSNumber*)theNum;

@end
