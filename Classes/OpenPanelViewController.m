//
//  OpenPanelViewController.m
//  PPMacho
//
//  Created by C.W. Betts on 12/21/12.
//
//

#import "OpenPanelViewController.h"
#import "ARCBridge.h"

@interface OpenPanelViewController ()

@end

enum utiType {
	utiAllType = -1,
	utiTrackerType = -2,
	utiPlaylistType = -3
	};

typedef struct _trackerType {
	unsigned int tracker:1;
	unsigned int playlist:1;
	unsigned int reserved:14;
} trackerType;

static BOOL isTwoTrackerTypesEqual(trackerType rhl, trackerType lhl)
{
	if (rhl.playlist != lhl.playlist) {
		return NO;
	}else if (rhl.tracker != lhl.tracker) {
		return NO;
		//Ignoring reserved for now.
	} else {
		return YES;
	}
}

@interface OpenPanelViewItem : NSObject
{
	trackerType type;
	NSArray *utis;
	NSString *name;
}

@property (readonly) NSString *name;
@property (readonly) trackerType type;
@property (readonly) NSArray *utis;

- (id)initWithType:(int)typ utis:(NSArray*)ut name:(NSString*)nam;

@end

@implementation OpenPanelViewItem

@synthesize name;
@synthesize type;
@synthesize utis;

- (id)initWithType:(int)typ utis:(NSArray*)ut name:(NSString*)nam;
{
	if (self = [super init]) {
		switch (typ) {
			case utiTrackerType:
				type.tracker = 1;
				break;
				
			case utiPlaylistType:
				type.playlist = 1;
				break;
				
			default:
				AUTORELEASEOBJNORETURN(self);
				return nil;
				break;
		}
		utis = RETAINOBJ(ut);
		name = [nam copy];
	}
	return self;
}

#if !__has_feature(objc_arc)
- (void)dealloc
{
	[utis release];
	[name release];
	
	[super dealloc];
}
#endif

- (NSString* )description
{
	return [NSString stringWithFormat:@"%@ %@", [utis description], name];
}

@end

@implementation OpenPanelViewController

- (id)initWithOpenPanel:(NSOpenPanel*)panel trackerDictionary:(NSDictionary *)td playlistDictionary:(NSDictionary*)pd
{
	if (self = [super initWithNibName:@"OpenPanelViewController" bundle:nil]) {
		openPanel = RETAINOBJ(panel);
		
		NSMutableArray *mutArray = [NSMutableArray array];
		if (td) {
			for (NSString *key in td) {
				NSArray *utis = [td objectForKey:key];
				OpenPanelViewItem *obj = [[OpenPanelViewItem alloc] initWithType:utiTrackerType utis:utis name:key];
				[mutArray addObject:obj];
				RELEASEOBJ(obj);
			}
		}
		if (pd) {
			for (NSString *key in pd) {
				NSArray *utis = [pd objectForKey:key];
				OpenPanelViewItem *obj = [[OpenPanelViewItem alloc] initWithType:utiPlaylistType utis:utis name:key];
				[mutArray addObject:obj];
				RELEASEOBJ(obj);
			}
		}
		utiObjects = [[NSArray alloc] initWithArray:mutArray];
	}
	return self;
}

/*- (id)initWithNibName:(NSString *)nibNameOrNil bundle:(NSBundle *)nibBundleOrNil
{
    self = [super initWithNibName:nibNameOrNil bundle:nibBundleOrNil];
    if (self) {
        // Initialization code here.
    }
    
    return self;
}*/

- (void)awakeFromNib
{
	NSMenu *fileTypeSelectionMenu = [popUp menu];
	int i = 0;
	{
		NSMenuItem *mi0 = [[NSMenuItem alloc] initWithTitle:@"All Openable Files" action:@selector(selectUTI:) keyEquivalent:@""];
		[mi0 setTag:utiAllType];
		[fileTypeSelectionMenu addItem:mi0];
		RELEASEOBJ(mi0);
	}
	[fileTypeSelectionMenu addItem:[NSMenuItem separatorItem]];
	
	for (OpenPanelViewItem *item in utiObjects) {
		if (item.type.tracker) {
			NSMenuItem *mi = [[NSMenuItem alloc] initWithTitle:@"All Trackers" action:@selector(selectUTI:) keyEquivalent:@""];
			[mi setTag:utiTrackerType];
			[fileTypeSelectionMenu addItem:mi];
			RELEASEOBJ(mi);
			break;
		}
	}
	
	for (OpenPanelViewItem *item in utiObjects) {
		if (item.type.playlist) {
			NSMenuItem *mi = [[NSMenuItem alloc] initWithTitle:@"All Playlists" action:@selector(selectUTI:) keyEquivalent:@""];
			[mi setTag:utiPlaylistType];
			[fileTypeSelectionMenu addItem:mi];
			RELEASEOBJ(mi);
			break;
		}
	}
	[fileTypeSelectionMenu addItem:[NSMenuItem separatorItem]];
	
	for (i = 0; i < [utiObjects count]; i++) {
		OpenPanelViewItem *curItem = [utiObjects objectAtIndex:i];
		if (i - 1 >= 0) {
			OpenPanelViewItem *prevItem = [utiObjects objectAtIndex:i - 1];
			if (!isTwoTrackerTypesEqual(curItem.type, prevItem.type)) {
				[fileTypeSelectionMenu addItem:[NSMenuItem separatorItem]];
			}
		}
		NSMenuItem *mi = [[NSMenuItem alloc] initWithTitle:curItem.name action:@selector(selectUTI:) keyEquivalent:@""];
		[mi setTag:i];
		[fileTypeSelectionMenu addItem:mi];
		RELEASEOBJ(mi);
	}
	
	[popUp selectItemAtIndex:0];
}

-(IBAction)selectUTI:(id)sender
{
	NSInteger tag = [sender tag];
	switch (tag) {
		case utiAllType:
		{
			NSMutableArray *allUtis = [NSMutableArray array];
			for (OpenPanelViewItem *obj in utiObjects) {
				[allUtis addObjectsFromArray:obj.utis];
			}
			[openPanel setAllowedFileTypes:allUtis];
		}
			break;
			
		case utiTrackerType:
		{
			NSMutableArray *trackerUTIs = [NSMutableArray array];
			for (OpenPanelViewItem *obj in utiObjects) {
				if (obj.type.tracker) {
					[trackerUTIs addObjectsFromArray:obj.utis];
				}
			}
			[openPanel setAllowedFileTypes:trackerUTIs];
		}
			break;
			
		case utiPlaylistType:
		{
			NSMutableArray *trackerUTIs = [NSMutableArray array];
			for (OpenPanelViewItem *obj in utiObjects) {
				if (obj.type.playlist) {
					[trackerUTIs addObjectsFromArray:obj.utis];
				}
			}
			[openPanel setAllowedFileTypes:trackerUTIs];
		}
			break;
			
		default:
		{
			if (tag < [utiObjects count]) {
				OpenPanelViewItem *selObj = [utiObjects objectAtIndex:tag];
				[openPanel setAllowedFileTypes:selObj.utis];
			}
		}
			break;
	}
}

#if !__has_feature(objc_arc)
- (void)dealloc
{
	[openPanel release];
	[utiObjects release];
	
	[super dealloc];
}
#endif

@end
