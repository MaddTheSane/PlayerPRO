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
@property (readwrite, copy) NSArray *UTITypes;
@property (strong) id<PPComplexImportPlugInterface> plugInterface;
@end

@implementation PPComplexImportPlugObject

- (BOOL)plugInRespondsToSelector:(SEL)aSelector
{
	return [_plugInterface respondsToSelector:aSelector];
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
		NSMutableDictionary *tempDict = [[ourBundle infoDictionary] mutableCopy];
		[tempDict addEntriesFromDictionary:[ourBundle localizedInfoDictionary]];
		id DictionaryTemp = [tempDict valueForKey:(__bridge NSString*)kMadPlugUTITypesKey];
		if ([DictionaryTemp isKindOfClass:[NSArray class]]) {
			self.UTITypes = [DictionaryTemp copy];
		} else if ([DictionaryTemp isKindOfClass:[NSString class]]) {
			self.UTITypes = @[[NSString stringWithString:DictionaryTemp]];
		} else
			return nil;

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
