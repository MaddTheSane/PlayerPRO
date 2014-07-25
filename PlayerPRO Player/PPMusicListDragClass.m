//
//  PPMusicListDragClass.m
//  PPMacho
//
//  Created by C.W. Betts on 12/8/13.
//
//

#import "PPMusicListDragClass.h"
#import "UserDefaultKeys.h"

@interface PPMusicListDragClass ()
@property (readwrite, copy) NSIndexSet *theIndexSet;
@end

@implementation PPMusicListDragClass

- (instancetype)initWithIndexSet:(NSIndexSet*)theSet
{
	if (self = [super init]) {
		self.theIndexSet = theSet;
	}
	return self;
}

+ (instancetype)dragWithIndexSet:(NSIndexSet*)theSet
{
	return [[[self class] alloc] initWithIndexSet:theSet];
}

+ (NSPasteboardReadingOptions)readingOptionsForType:(NSString *)type pasteboard:(NSPasteboard *)pasteboard
{
	if ([type isEqualToString:PPMLDCUTI])
		return NSPasteboardReadingAsKeyedArchive;
	else
		return NSPasteboardReadingAsData;
}

static NSArray *UTIArray;
static dispatch_once_t dragOnceToken;
static const dispatch_block_t initUTIArray = ^{
	UTIArray = @[PPMLDCUTI];
};

+ (NSArray *)readableTypesForPasteboard:(NSPasteboard *)pasteboard
{
	dispatch_once(&dragOnceToken, initUTIArray);
	return UTIArray;
}

- (NSArray *)writableTypesForPasteboard:(NSPasteboard *)pasteboard
{
	dispatch_once(&dragOnceToken, initUTIArray);
	return UTIArray;
}

- (id)pasteboardPropertyListForType:(NSString *)type
{
	if ([type isEqualToString:PPMLDCUTI])
		return [NSKeyedArchiver archivedDataWithRootObject:self];
	else
		return nil;
}

- (void)encodeWithCoder:(NSCoder *)aCoder
{
	[aCoder encodeObject:_theIndexSet forKey:PPMLDCUTI];
}

- (id)initWithCoder:(NSCoder *)aDecoder
{
	return self = [self initWithIndexSet:[aDecoder decodeObjectForKey:PPMLDCUTI]];
}

@end
