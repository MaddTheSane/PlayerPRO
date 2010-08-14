//
//  Alias.h
//  Slide Master
//
//  Created by C.W. Betts on 11/17/08.
//  Copyright 2008 __MyCompanyName__. All rights reserved.
//

#import <Foundation/Foundation.h>
#import <CoreServices/CoreServices.h>

@interface PPAlias : NSObject <NSCoding>
{
	AliasHandle mAliasHandle;
}
-(id)initWithPath:(NSString *)path;
-(id)initWithURL:(NSURL *)path;

-(NSString *)path;
-(NSURL *)url;

@end
