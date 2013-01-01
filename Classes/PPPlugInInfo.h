//
//  PPPlugInInfo.h
//  PPMacho
//
//  Created by C.W. Betts on 12/31/12.
//
//

#import <Foundation/Foundation.h>

@interface PPPlugInInfo : NSObject
{
	NSString *plugName;
	NSString *authorName;
	//OSType capabilities;
}

@property (readonly) NSString *plugName;
@property (readonly) NSString *authorName;
- (id)initWithPlugName:(NSString*)pn author:(NSString*)aut;

@end
