//
//  PPComplexImportPlugObject.m
//  PPMacho
//
//  Created by C.W. Betts on 8/18/14.
//
//

#import "PPComplexImportPlugObject.h"

@interface PPComplexImportPlugObject ()
@property (readwrite, copy) NSArray *UTITypes;
@property (strong) id<PPComplexImportPlugInterface> plugInterface;
@end

@implementation PPComplexImportPlugObject

- (instancetype)initWithBundle:(NSBundle*)ourBundle
{
	if (self = [super initWithBundle:ourBundle]) {
		Class bundClass = [ourBundle principalClass];
		if (![bundClass conformsToProtocol:@protocol(PPComplexImportPlugInterface)]) {
			return nil;
		}
		self.plugInterface = [[bundClass alloc] initForPlugIn];
		NSMutableDictionary *tempDict = [[ourBundle infoDictionary] mutableCopy];
		[tempDict addEntriesFromDictionary:[ourBundle localizedInfoDictionary]];
		id DictionaryTemp = [tempDict valueForKey:(__bridge NSString*)kMadPlugUTITypesKey];
		if ([DictionaryTemp isKindOfClass:[NSArray class]]) {
			self.UTITypes = [DictionaryTemp copy];
		} else if ([DictionaryTemp isKindOfClass:[NSString class]]) {
			self.UTITypes = @[[NSString stringWithString:DictionaryTemp]];
		} else
			return nil;
		
		type = 'CImp';
	}
	return self;
}

- (void)beginImportOfURL:(NSURL *)theURL withHandler:(PPComplexImportHandler)handler
{
	[self.plugInterface beginImportOfURL:theURL withHandler:handler];
}

- (BOOL)canImportURL:(NSURL*)theURL error:(out NSError**)outErr
{
	return [_plugInterface canImportURL:theURL error:outErr];
}

@end
