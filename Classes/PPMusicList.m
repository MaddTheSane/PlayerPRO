//
//  PPMusicList.m
//  PPMacho
//
//  Created by C.W. Betts on 8/8/10.
//  Copyright 2010 __MyCompanyName__. All rights reserved.
//

#import "PPMusicList.h"
#include "RDriver.h"

#define kMUSICLISTKEY @"Music List Key1"

@implementation PPMusicList

- (id)init
{
	self = [super init];
	if (self) {
		musicList = [[NSMutableArray alloc] init];
	}
	return self;
}

- (void)dealloc
{
	[musicList release];
	
	[super dealloc];
}

- (void)addMusicURL:(NSURL *)musicToLoad
{
	[musicList addObject:musicToLoad];
}

- (void)removeObjectAtIndex:(NSUInteger)object
{
	[musicList removeObjectAtIndex:object];
}

- (NSURL*)URLAtIndex:(NSUInteger)index
{
	return [musicList objectAtIndex:index];
}

- (NSInteger)numberOfRowsInTableView:(NSTableView *)tableView
{
	return [musicList count];
}

- (id)tableView:(NSTableView *)tableView objectValueForTableColumn:(NSTableColumn *)tableColumn row:(NSInteger)row
{
	NSTableCellView *view = [[NSTableCellView alloc] init];
	NSTextField *field = view.textField;
	[field setStringValue:[[musicList objectAtIndex:row] lastPathComponent]];
	//[field setAllowsEditingTextAttributes:NO];
	[field setEditable:NO];
	return [view autorelease];
}

#pragma mark Archiving

- (id)initWithCoder:(NSCoder *)decoder {
	
	if ((self = [super init])) 
	{
		NSMutableArray *BookmarkArray = [decoder decodeObjectForKey:kMUSICLISTKEY];
		NSInteger i = 0;
		musicList = [[NSMutableArray alloc] initWithCapacity:[BookmarkArray count]];
		for (i = 0; i < [BookmarkArray count]; i++) {
			[musicList insertObject:[NSURL URLByResolvingBookmarkData:[BookmarkArray objectAtIndex:i] options:NSURLBookmarkResolutionWithoutUI relativeToURL:nil bookmarkDataIsStale:NULL error:nil]  atIndex:i];
		}
	}
	return self;
}

- (void)encodeWithCoder:(NSCoder *)encoder {
	NSMutableArray *BookmarkArray = [NSMutableArray arrayWithCapacity:[musicList count]];
	NSInteger i = 0;
	for (i = 0; i < [musicList count]; i++)
	{
		[BookmarkArray insertObject:[[musicList objectAtIndex:i] bookmarkDataWithOptions:NSURLBookmarkCreationPreferFileIDResolution includingResourceValuesForKeys:nil relativeToURL:nil error:nil] atIndex:i];
	}
	[encoder encodeObject:BookmarkArray forKey:kMUSICLISTKEY];
}

@end
