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
	utiPlaylistType = -3,
	utiInstrumentType = -4,
	utiOtherType = -5
	};

typedef struct _trackerType {
	unsigned int tracker:1;
	unsigned int playlist:1;
	unsigned int instrument:1;
	unsigned int other:1;
	unsigned int reserved:12;
} trackerType;

static inline BOOL isTwoTrackerTypesEqual(trackerType rhl, trackerType lhl)
{
	if (rhl.playlist != lhl.playlist) {
		return NO;
	}else if (rhl.tracker != lhl.tracker) {
		return NO;
	} else if(rhl.instrument != lhl.instrument) {
		return NO;
	} else if(rhl.other != lhl.other) {
		return NO;
		//Ignoring reserved for now.
	} else {
		return YES;
	}
}

@interface OpenPanelViewItem : NSObject
{
	trackerType utiType;
	NSArray *utis;
	NSString *name;
}

@property (readonly) NSString *name;
@property (readonly) trackerType utiType;
@property (readonly) NSArray *utis;

- (id)initWithType:(int)typ utis:(NSArray*)ut name:(NSString*)nam;

@end

@implementation OpenPanelViewItem

- (id)init
{
	[self doesNotRecognizeSelector:_cmd];
	AUTORELEASEOBJNORETURN(self);
	return nil;
}

@synthesize name;
@synthesize utiType;
@synthesize utis;

- (id)initWithType:(int)typ utis:(NSArray*)ut name:(NSString*)nam;
{
	if (self = [super init]) {
		switch (typ) {
			case utiTrackerType:
				utiType.tracker = 1;
				break;
				
			case utiPlaylistType:
				utiType.playlist = 1;
				break;
				
			case utiInstrumentType:
				utiType.instrument = 1;
				break;
				
			case utiOtherType:
				utiType.other = 1;
				break;
				
			default:
				AUTORELEASEOBJNORETURN(self);
				return nil;
				break;
		}
		utis = [[NSArray alloc] initWithArray:ut];
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
	NSString *des = nil;
	if (utiType.playlist) {
		des = @"Playlist";
	} else if (utiType.instrument) {
		des = @"Instrument";
	} else if (utiType.tracker) {
		des = @"Tracker";
	} else if (utiType.other) {
		des = @"Other";
	}
	
	return [NSString stringWithFormat:@"%@: %@ - %@", name, des, [utis description]];
}

@end

@implementation OpenPanelViewController

- (id)init
{
	[self doesNotRecognizeSelector:_cmd];
	AUTORELEASEOBJNORETURN(self);
	return nil;
}

- (id)initWithOpenPanel:(NSOpenPanel*)panel trackerDictionary:(NSDictionary *)td playlistDictionary:(NSDictionary*)pd additionalDictionary:(NSDictionary *)adddict
{
	return [self initWithOpenPanel:panel trackerDictionary:td playlistDictionary:pd instrumentDictionary:nil additionalDictionary:adddict];
}

- (id)initWithOpenPanel:(NSOpenPanel*)panel trackerDictionary:(NSDictionary *)td playlistDictionary:(NSDictionary*)pd instrumentDictionary:(NSDictionary*)insDict
{
	return [self initWithOpenPanel:panel trackerDictionary:td playlistDictionary:pd instrumentDictionary:insDict additionalDictionary:nil];
}

- (id)initWithOpenPanel:(NSOpenPanel*)panel trackerDictionary:(NSDictionary *)td playlistDictionary:(NSDictionary*)pd
{
	return [self initWithOpenPanel:panel trackerDictionary:td playlistDictionary:pd instrumentDictionary:nil additionalDictionary:nil];
}

- (id)initWithOpenPanel:(NSOpenPanel*)panel trackerDictionary:(NSDictionary *)td playlistDictionary:(NSDictionary*)pd instrumentDictionary:(NSDictionary*)insDict additionalDictionary:(NSDictionary *)adddict
{
	if (self = [super initWithNibName:@"OpenPanelViewController" bundle:nil]) {
		if (!panel) {
			AUTORELEASEOBJNORETURN(self);
			return nil;
		}
		openPanel = RETAINOBJ(panel);
		
		NSMutableArray *mutArray = [NSMutableArray array];
		if (td) {
			for (NSString *key in td) {
				NSArray *utis = td[key];
				OpenPanelViewItem *obj = [[OpenPanelViewItem alloc] initWithType:utiTrackerType utis:utis name:key];
				[mutArray addObject:obj];
				RELEASEOBJ(obj);
			}
		}
		if (pd) {
			for (NSString *key in pd) {
				NSArray *utis = pd[key];
				OpenPanelViewItem *obj = [[OpenPanelViewItem alloc] initWithType:utiPlaylistType utis:utis name:key];
				[mutArray addObject:obj];
				RELEASEOBJ(obj);
			}
		}
		if (insDict) {
			for (NSString *key in insDict) {
				NSArray *utis = insDict[key];
				OpenPanelViewItem *obj = [[OpenPanelViewItem alloc] initWithType:utiInstrumentType utis:utis name:key];
				[mutArray addObject:obj];
				RELEASEOBJ(obj);
			}
		}
		if (adddict) {
			for (NSString *key in adddict) {
				NSArray *utis = adddict[key];
				OpenPanelViewItem *obj = [[OpenPanelViewItem alloc] initWithType:utiOtherType utis:utis name:key];
				[mutArray addObject:obj];
				RELEASEOBJ(obj);
			}
		}

		[mutArray sortUsingComparator:^NSComparisonResult(id obj1, id obj2) {
			trackerType obj1Type = [obj1 utiType], obj2Type = [obj2 utiType];
			if (obj1Type.tracker != obj2Type.tracker) {
				if (obj1Type.tracker) {
					return NSOrderedAscending;
				} else return NSOrderedDescending;
			} else if (obj1Type.playlist != obj2Type.playlist) {
				if (obj1Type.playlist) {
					return NSOrderedAscending;
				} else return NSOrderedDescending;
			} else if (obj1Type.instrument != obj2Type.instrument) {
				if (obj1Type.instrument) {
					return NSOrderedAscending;
				} else return NSOrderedDescending;
			} else if (obj1Type.other != obj2Type.other) {
				//Technically we shouldn't get here...
				if (obj1Type.other) {
					return NSOrderedAscending;
				} else return NSOrderedDescending;
			}
			
			NSComparisonResult result = [[obj1 name] localizedStandardCompare:[obj2 name]];
			return result;
		}];
		
		utiObjects = [[NSArray alloc] initWithArray:mutArray];
	}
	return self;
}

- (void)setupDefaults
{
	NSMutableArray *fileUTIs = [[NSMutableArray alloc] init];
	
	for (OpenPanelViewItem *obj in utiObjects) {
		[fileUTIs addObjectsFromArray:obj.utis];
	}
	
	[openPanel setAllowsMultipleSelection:NO];
	[openPanel setAllowedFileTypes:fileUTIs];
	[openPanel setAccessoryView:[self view]];

	RELEASEOBJ(fileUTIs);
}

/*- (id)initWithNibName:(NSString *)nibNameOrNil bundle:(NSBundle *)nibBundleOrNil
{
    self = [super initWithNibName:nibNameOrNil bundle:nibBundleOrNil];
    if (self) {
        // Initialization code here.
    }
    
    return self;
}*/

- (BOOL)hasMoreThanTwoTypes
{
	NSInteger i;
	NSUInteger utiCount = [utiObjects count];
	if (utiCount < 2) {
		return NO;
	}
	
	for (i = 1; i < utiCount; i++) {
		OpenPanelViewItem *obj1 = utiObjects[i - 1];
		OpenPanelViewItem *obj2 = utiObjects[i];
		if (!isTwoTrackerTypesEqual(obj1.utiType, obj2.utiType)) {
			return YES;
		}
	}
	return NO;
}

- (void)awakeFromNib
{
	NSMenu *fileTypeSelectionMenu = [popUp menu];
	int i = 0;
	BOOL moreThanTwoTypes = [self hasMoreThanTwoTypes];
	if (moreThanTwoTypes) {
		NSMenuItem *mi0 = [[NSMenuItem alloc] initWithTitle:@"All Openable Files" action:@selector(selectUTI:) keyEquivalent:@""];
		[mi0 setTag:utiAllType];
		[mi0 setTarget:self];
		[fileTypeSelectionMenu addItem:mi0];
		RELEASEOBJ(mi0);
		
		[fileTypeSelectionMenu addItem:[NSMenuItem separatorItem]];
	}
	
	for (OpenPanelViewItem *item in utiObjects) {
		if (item.utiType.tracker) {
			NSMenuItem *mi = [[NSMenuItem alloc] initWithTitle:@"All Trackers" action:@selector(selectUTI:) keyEquivalent:@""];
			[mi setTag:utiTrackerType];
			[mi setTarget:self];
			[fileTypeSelectionMenu addItem:mi];
			RELEASEOBJ(mi);
			break;
		}
	}
	
	for (OpenPanelViewItem *item in utiObjects) {
		if (item.utiType.playlist) {
			NSMenuItem *mi = [[NSMenuItem alloc] initWithTitle:@"All Playlists" action:@selector(selectUTI:) keyEquivalent:@""];
			[mi setTag:utiPlaylistType];
			[mi setTarget:self];
			[fileTypeSelectionMenu addItem:mi];
			RELEASEOBJ(mi);
			break;
		}
	}
	for (OpenPanelViewItem *item in utiObjects) {
		if (item.utiType.instrument) {
			NSMenuItem *mi = [[NSMenuItem alloc] initWithTitle:@"All Instruments" action:@selector(selectUTI:) keyEquivalent:@""];
			[mi setTag:utiInstrumentType];
			[mi setTarget:self];
			[fileTypeSelectionMenu addItem:mi];
			RELEASEOBJ(mi);
			break;
		}
	}
	
	for (OpenPanelViewItem *item in utiObjects) {
		if (item.utiType.other) {
			NSMenuItem *mi = [[NSMenuItem alloc] initWithTitle:@"All Other" action:@selector(selectUTI:) keyEquivalent:@""];
			[mi setTag:utiOtherType];
			[mi setTarget:self];
			[fileTypeSelectionMenu addItem:mi];
			RELEASEOBJ(mi);
			break;
		}
	}

	[fileTypeSelectionMenu addItem:[NSMenuItem separatorItem]];
	
	for (i = 0; i < [utiObjects count]; i++) {
		OpenPanelViewItem *curItem = utiObjects[i];
		if (moreThanTwoTypes) {
			if (i - 1 >= 0) {
				OpenPanelViewItem *prevItem = utiObjects[i - 1];
				if (!isTwoTrackerTypesEqual(curItem.utiType, prevItem.utiType)) {
					[fileTypeSelectionMenu addItem:[NSMenuItem separatorItem]];
				}
			}
		}
		NSMenuItem *mi = [[NSMenuItem alloc] initWithTitle:curItem.name action:@selector(selectUTI:) keyEquivalent:@""];
		[mi setTag:i];
		[mi setTarget:self];
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
				if (obj.utiType.tracker) {
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
				if (obj.utiType.playlist) {
					[trackerUTIs addObjectsFromArray:obj.utis];
				}
			}
			[openPanel setAllowedFileTypes:trackerUTIs];
		}
			break;
			
		case utiInstrumentType:
		{
			NSMutableArray *instrumentUTIs = [NSMutableArray array];
			for (OpenPanelViewItem *obj in utiObjects) {
				if (obj.utiType.instrument) {
					[instrumentUTIs addObjectsFromArray:obj.utis];
				}
			}
			[openPanel setAllowedFileTypes:instrumentUTIs];
		}
			break;
			
		case utiOtherType:
		{
			NSMutableArray *otherUTIs = [NSMutableArray array];
			for (OpenPanelViewItem *obj in utiObjects) {
				if (obj.utiType.other) {
					[otherUTIs addObjectsFromArray:obj.utis];
				}
			}
			[openPanel setAllowedFileTypes:otherUTIs];
		}
			break;
			
		default:
		{
			if (tag < [utiObjects count] && tag >= 0) {
				OpenPanelViewItem *selObj = utiObjects[tag];
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
