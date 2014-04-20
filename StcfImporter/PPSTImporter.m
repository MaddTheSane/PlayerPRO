//
//  PPSTImporter.m
//  PPMacho
//
//  Created by C.W. Betts on 4/20/14.
//
//

#import "PPSTImporter.h"

@implementation PPSTImporter

+ (instancetype)sharedImporter
{
	static dispatch_once_t onceToken;
    static PPSTImporter *shared;
    dispatch_once(&onceToken, ^{
        shared = [PPSTImporter new];
    });
    return shared;
}

- (void)dealloc
{
	
	
	[super dealloc];
}

@end
