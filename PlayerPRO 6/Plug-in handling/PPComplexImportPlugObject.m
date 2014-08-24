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
@property (readwrite, copy) NSArray *utiArrays;
@property (strong) id<PPComplexImportPlugInterface> plugInterface;
@end

@implementation PPComplexImportPlugObject

- (BOOL)plugInRespondsToSelector:(SEL)aSelector
{
#define HandleSelector(aSel) \
if (aSelector == @selector( aSel )) { \
	if ([_plugInterface respondsToSelector:@selector( aSel )]) { \
		return YES; \
	} else { \
		return NO; \
	} \
}
	
	HandleSelector(canImportURL:error:);
	HandleSelector(getTrackerInformationFromURL:);
	HandleSelector(getTrackerInformationFromURL:error:);
	
	return [super respondsToSelector:aSelector];
#undef HandleSelector
}

- (instancetype)initWithBundle:(NSBundle*)ourBundle
{
	if (self = [super init]) {
		Class bundClass = [ourBundle principalClass];
		if (![bundClass conformsToProtocol:@protocol(PPComplexImportPlugInterface)]) {
			return nil;
		}
		self.ourBundle = ourBundle;
		self.plugInterface = [[bundClass alloc] init];
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

- (BOOL)canImportURL:(NSURL*)theURL error:(out NSError**)outErr
{
	return [_plugInterface canImportURL:theURL error:outErr];
}

- (NSDictionary*)getTrackerInformationFromURL:(NSURL*)theURL
{
	return [_plugInterface getTrackerInformationFromURL:theURL];
}

- (NSDictionary*)getTrackerInformationFromURL:(NSURL*)theURL error:(out NSError**)outErr
{
	return [_plugInterface getTrackerInformationFromURL:theURL error:outErr];
}

@end
