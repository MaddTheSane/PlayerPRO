//
//  PPPlugInObject.h
//  PPMacho
//
//  Created by C.W. Betts on 1/27/13.
//
//

#import <Foundation/Foundation.h>

@interface PPPlugInObject : NSObject
{
	@package
	OSType	type;
	UInt32	version;
}
@property (readonly, copy) NSString *menuName;
@property (readonly, copy) NSString *authorString;
@property (readonly, strong) NSBundle *file;
@property (readonly) OSType type;
@property (readonly) UInt32 version;

- (instancetype)init UNAVAILABLE_ATTRIBUTE;
- (instancetype)initWithBundle:(NSBundle *)aBund NS_DESIGNATED_INITIALIZER;
@end
