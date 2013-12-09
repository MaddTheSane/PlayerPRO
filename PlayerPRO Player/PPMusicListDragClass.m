//
//  PPMusicListDragClass.m
//  PPMacho
//
//  Created by C.W. Betts on 12/8/13.
//
//

#import "PPMusicListDragClass.h"

NSString * const PPMLDCUTI = @"net.sourceforge.playerpro.playerpro_player.playerlistdragtype";

@interface PPMusicListDragClass ()
@property (readwrite, strong) NSIndexSet *theIndexSet;
@end

@implementation PPMusicListDragClass

- (id)initWithIndexSet:(NSIndexSet*)theSet
{
	if (self = [super init]) {
		self.theIndexSet = theSet;
	}
	return self;
}

- (id)initWithPasteboardPropertyList:(id)propertyList ofType:(NSString *)type
{
	if ([type isEqualToString:PPMLDCUTI]) {
		
	} else
		return nil;
}

+ (NSArray *)readableTypesForPasteboard:(NSPasteboard *)pasteboard
{
	
}

- (NSArray *)writableTypesForPasteboard:(NSPasteboard *)pasteboard
{
	
}

- (id)pasteboardPropertyListForType:(NSString *)type
{
	if ([type isEqualToString:PPMLDCUTI]) {
		NSMutableArray *theNums = [[NSMutableArray alloc] init];
		
		
		
		return [theNums copy];
	} else
		return nil;
}

	
@end
