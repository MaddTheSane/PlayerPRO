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
	NSString	*plugName;
	NSString	*authorName;
	NSString	*plugType;
	NSURL		*plugURL;
	//OSType capabilities;
}
@property (readonly) NSString *plugName;
@property (readonly) NSString *authorName;
@property (readonly) NSString *plugType;
@property (readonly) NSURL    *plugURL;

- (id)initWithPlugName:(NSString*)pn author:(NSString*)aut;
- (id)initWithPlugName:(NSString*)pn author:(NSString*)aut plugType:(NSString*)pt;
- (id)initWithPlugName:(NSString*)pn author:(NSString*)aut plugType:(NSString*)pt plugURL:(NSURL*)pu;

@end
