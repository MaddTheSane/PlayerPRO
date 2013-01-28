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
	@protected
	NSString		*menuName;
	NSString		*authorString;
	NSBundle		*file;
	OSType			type;
	UInt32			version;
}

@property (readonly) NSString *menuName;
@property (readonly) NSString *authorString;
@property (readonly) NSBundle *file;
@property (readonly) OSType type;
@property (readonly) UInt32 version;

- (id)initWithBundle:(NSBundle *)aBund;

@end
