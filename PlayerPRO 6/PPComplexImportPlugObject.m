//
//  PPComplexImportPlugObject.m
//  PPMacho
//
//  Created by C.W. Betts on 8/18/14.
//
//

#import "PPComplexImportPlugObject.h"

@interface PPComplexImportPlugObject ()
@property (readwrite, strong) NSBundle *ourBundle;
@property (strong) id<PPComplexImportPlugInterface> plugInterface;
@end

@implementation PPComplexImportPlugObject

- (instancetype)initWithBundle:(NSBundle*)ourBundle
{
	if (self = [super init]) {
		Class bundClass = [ourBundle principalClass];
		if (![bundClass conformsToProtocol:@protocol(PPComplexImportPlugInterface)]) {
			return nil;
		}
		self.ourBundle = ourBundle;
	}
	return self;
}

- (BOOL)canImportURL:(NSURL *)theURL
{
	return [self.plugInterface canImportURL:theURL];
}

- (void)beginImportOfURL:(NSURL *)theURL withHandler:(PPComplexImportHandler)handler
{
	[self.plugInterface beginImportOfURL:theURL withHandler:handler];
}

@end
