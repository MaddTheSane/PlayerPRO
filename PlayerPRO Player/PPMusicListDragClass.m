//
//  PPMusicListDragClass.m
//  PPMacho
//
//  Created by C.W. Betts on 12/8/13.
//
//

#import "PPMusicListDragClass.h"

NSString * const PPMLDCUTI = @"net.sourceforge.playerpro.PlayerPRO-Player.playerlistdragtype";

@interface PPMusicListDragClass ()
@property (readwrite, strong) NSIndexSet *theIndexSet;
@end

@implementation PPMusicListDragClass

- (instancetype)initWithIndexSet:(NSIndexSet*)theSet
{
	if (self = [super init]) {
		self.theIndexSet = theSet;
	}
	return self;
}

- (id)initWithPasteboardPropertyList:(id)propertyList ofType:(NSString *)type
{
	if ([type isEqualToString:PPMLDCUTI]) {
		NSArray *theArray = propertyList;
		NSMutableIndexSet *newSet = [[NSMutableIndexSet alloc] init];
		for (NSNumber *theNum in theArray) {
			[newSet addIndex:[theNum unsignedIntegerValue]];
		}
		return self = [self initWithIndexSet:[newSet copy]];
	} else
		return nil;
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
	if ([type isEqualToString:PPMLDCUTI]) {
		NSMutableArray *theNums = [[NSMutableArray alloc] initWithCapacity:[_theIndexSet count]];
		NSUInteger currentIndex = [_theIndexSet firstIndex];
		while (currentIndex != NSNotFound) {
			[theNums addObject:@(currentIndex)];
			currentIndex = [_theIndexSet indexGreaterThanIndex:currentIndex];
		}
		return [theNums copy];
	} else
		return nil;
}
	
@end
