//
//  NSColor+PPPreferences.h
//  PPMacho
//
//  Created by C.W. Betts on 7/16/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#import <AppKit/AppKit.h>


@interface NSColor (PPPreferences) 
-(NSData *)PPencodeColor;
+(NSColor *)PPDecodeColorWithData:(NSData *)theData;
@end
