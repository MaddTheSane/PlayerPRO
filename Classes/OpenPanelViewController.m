//
//  OpenPanelViewController.m
//  PPMacho
//
//  Created by C.W. Betts on 12/21/12.
//
//

#import "OpenPanelViewController.h"

@interface OpenPanelViewController ()
@property (strong) NSOpenPanel *openPanel;
@property (strong) NSArray *utiObjects;
@end

typedef NS_ENUM(int, utiType) {
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
	} else if (rhl.tracker != lhl.tracker) {
		return NO;
	} else if (rhl.instrument != lhl.instrument) {
		return NO;
	} else if (rhl.other != lhl.other) {
		return NO;
		//Ignoring reserved for now.
	} else {
		return YES;
	}
}

@interface OpenPanelViewItem : NSObject
{
	trackerType theUtiType;
	NSArray *utis;
	NSString *name;
}

@property (readonly) NSString *name;
@property (readonly) trackerType theUtiType;
@property (readonly) NSArray *utis;

- (id)initWithType:(utiType)typ utis:(NSArray*)ut name:(NSString*)nam;

@end

@implementation OpenPanelViewItem

- (id)init
{
	[self doesNotRecognizeSelector:_cmd];
	return nil;
}

@synthesize name;
@synthesize theUtiType;
@synthesize utis;

- (id)initWithType:(utiType)typ utis:(NSArray*)ut name:(NSString*)nam;
{
	if (self = [super init]) {
		switch (typ) {
			case utiTrackerType:
				theUtiType.tracker = 1;
				break;
				
			case utiPlaylistType:
				theUtiType.playlist = 1;
				break;
				
			case utiInstrumentType:
				theUtiType.instrument = 1;
				break;
				
			case utiOtherType:
				theUtiType.other = 1;
				break;
				
			default:
				return nil;
				break;
		}
		utis = [[NSArray alloc] initWithArray:ut];
		name = [nam copy];
	}
	return self;
}

- (NSString* )description
{
	NSString *des;
	if (theUtiType.playlist) {
		des = @"Playlist";
	} else if (theUtiType.instrument) {
		des = @"Instrument";
	} else if (theUtiType.tracker) {
		des = @"Tracker";
	} else if (theUtiType.other) {
		des = @"Other";
	} else {
		des = @"Unknown";
	}
	
	return [NSString stringWithFormat:@"%@: %@ - %@", name, des, [utis description]];
}

@end

@implementation OpenPanelViewController
@synthesize allowsMultipleSelectionOfTrackers;
- (BOOL)allowsMultipleSelectionOfTrackers
{
	return allowsMultipleSelectionOfTrackers;
}

- (void)setAllowsMultipleSelectionOfTrackers:(BOOL)theVal
{
	allowsMultipleSelectionOfTrackers = theVal;
	[openPanel setAllowsMultipleSelection:allowsMultipleSelectionOfTrackers];
}

@synthesize openPanel;
@synthesize popUp;
@synthesize utiObjects;

- (id)init
{
	[self doesNotRecognizeSelector:_cmd];
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
			return nil;
		}
		openPanel = panel;
		
		NSMutableArray *mutArray = [NSMutableArray array];
		if (td) {
			for (NSString *key in td) {
				NSArray *utis = td[key];
				OpenPanelViewItem *obj = [[OpenPanelViewItem alloc] initWithType:utiTrackerType utis:utis name:key];
				[mutArray addObject:obj];
			}
		}
		if (pd) {
			for (NSString *key in pd) {
				NSArray *utis = pd[key];
				OpenPanelViewItem *obj = [[OpenPanelViewItem alloc] initWithType:utiPlaylistType utis:utis name:key];
				[mutArray addObject:obj];
			}
		}
		if (insDict) {
			for (NSString *key in insDict) {
				NSArray *utis = insDict[key];
				OpenPanelViewItem *obj = [[OpenPanelViewItem alloc] initWithType:utiInstrumentType utis:utis name:key];
				[mutArray addObject:obj];
			}
		}
		if (adddict) {
			for (NSString *key in adddict) {
				NSArray *utis = adddict[key];
				OpenPanelViewItem *obj = [[OpenPanelViewItem alloc] initWithType:utiOtherType utis:utis name:key];
				[mutArray addObject:obj];
			}
		}

		[mutArray sortUsingComparator:^NSComparisonResult(id obj1, id obj2) {
			trackerType obj1Type = [obj1 theUtiType], obj2Type = [obj2 theUtiType];
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
	
	if (!allowsMultipleSelectionOfTrackers) {
		[openPanel setAllowsMultipleSelection:NO];
	}
	
	[openPanel setAllowedFileTypes:fileUTIs];
	[openPanel setAccessoryView:[self view]];
}

#if 0
- (id)initWithNibName:(NSString *)nibNameOrNil bundle:(NSBundle *)nibBundleOrNil
{
	self = [super initWithNibName:nibNameOrNil bundle:nibBundleOrNil];
	if (self) {
		// Initialization code here.
	}
	
	return self;
}
#endif

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
		if (!isTwoTrackerTypesEqual(obj1.theUtiType, obj2.theUtiType)) {
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
		
		[fileTypeSelectionMenu addItem:[NSMenuItem separatorItem]];
	}
	
	for (OpenPanelViewItem *item in utiObjects) {
		if (item.theUtiType.tracker) {
			NSMenuItem *mi = [[NSMenuItem alloc] initWithTitle:@"All Trackers" action:@selector(selectUTI:) keyEquivalent:@""];
			[mi setTag:utiTrackerType];
			[mi setTarget:self];
			[fileTypeSelectionMenu addItem:mi];
			break;
		}
	}
	
	for (OpenPanelViewItem *item in utiObjects) {
		if (item.theUtiType.playlist) {
			NSMenuItem *mi = [[NSMenuItem alloc] initWithTitle:@"All Playlists" action:@selector(selectUTI:) keyEquivalent:@""];
			[mi setTag:utiPlaylistType];
			[mi setTarget:self];
			[fileTypeSelectionMenu addItem:mi];
			break;
		}
	}
	for (OpenPanelViewItem *item in utiObjects) {
		if (item.theUtiType.instrument) {
			NSMenuItem *mi = [[NSMenuItem alloc] initWithTitle:@"All Instruments" action:@selector(selectUTI:) keyEquivalent:@""];
			[mi setTag:utiInstrumentType];
			[mi setTarget:self];
			[fileTypeSelectionMenu addItem:mi];
			break;
		}
	}
	
	for (OpenPanelViewItem *item in utiObjects) {
		if (item.theUtiType.other) {
			NSMenuItem *mi = [[NSMenuItem alloc] initWithTitle:@"All Other" action:@selector(selectUTI:) keyEquivalent:@""];
			[mi setTag:utiOtherType];
			[mi setTarget:self];
			[fileTypeSelectionMenu addItem:mi];
			break;
		}
	}

	[fileTypeSelectionMenu addItem:[NSMenuItem separatorItem]];
	
	for (i = 0; i < [utiObjects count]; i++) {
		OpenPanelViewItem *curItem = utiObjects[i];
		if (moreThanTwoTypes) {
			if (i - 1 >= 0) {
				OpenPanelViewItem *prevItem = utiObjects[i - 1];
				if (!isTwoTrackerTypesEqual(curItem.theUtiType, prevItem.theUtiType)) {
					[fileTypeSelectionMenu addItem:[NSMenuItem separatorItem]];
				}
			}
		}
		NSMenuItem *mi = [[NSMenuItem alloc] initWithTitle:curItem.name action:@selector(selectUTI:) keyEquivalent:@""];
		[mi setTag:i];
		[mi setTarget:self];
		[fileTypeSelectionMenu addItem:mi];
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
			if (allowsMultipleSelectionOfTrackers) {
				[openPanel setAllowsMultipleSelection:YES];
			}
		}
			break;
			
		case utiTrackerType:
		{
			NSMutableArray *trackerUTIs = [NSMutableArray array];
			for (OpenPanelViewItem *obj in utiObjects) {
				if (obj.theUtiType.tracker) {
					[trackerUTIs addObjectsFromArray:obj.utis];
				}
			}
			[openPanel setAllowedFileTypes:trackerUTIs];
			if (allowsMultipleSelectionOfTrackers) {
				[openPanel setAllowsMultipleSelection:YES];
			}
		}
			break;
			
		case utiPlaylistType:
		{
			NSMutableArray *trackerUTIs = [NSMutableArray array];
			for (OpenPanelViewItem *obj in utiObjects) {
				if (obj.theUtiType.playlist) {
					[trackerUTIs addObjectsFromArray:obj.utis];
				}
			}
			[openPanel setAllowedFileTypes:trackerUTIs];
			if (allowsMultipleSelectionOfTrackers) {
				[openPanel setAllowsMultipleSelection:NO];
			}
		}
			break;
			
		case utiInstrumentType:
		{
			NSMutableArray *instrumentUTIs = [NSMutableArray array];
			for (OpenPanelViewItem *obj in utiObjects) {
				if (obj.theUtiType.instrument) {
					[instrumentUTIs addObjectsFromArray:obj.utis];
				}
			}
			[openPanel setAllowedFileTypes:instrumentUTIs];
			if (allowsMultipleSelectionOfTrackers) {
				[openPanel setAllowsMultipleSelection:NO];
			}
		}
			break;
			
		case utiOtherType:
		{
			NSMutableArray *otherUTIs = [NSMutableArray array];
			for (OpenPanelViewItem *obj in utiObjects) {
				if (obj.theUtiType.other) {
					[otherUTIs addObjectsFromArray:obj.utis];
				}
			}
			[openPanel setAllowedFileTypes:otherUTIs];
			if (allowsMultipleSelectionOfTrackers) {
				[openPanel setAllowsMultipleSelection:YES];
			}
		}
			break;
			
		default:
			if (tag < [utiObjects count] && tag >= 0) {
				OpenPanelViewItem *selObj = utiObjects[tag];
				[openPanel setAllowedFileTypes:selObj.utis];
				if (allowsMultipleSelectionOfTrackers) {
					if (selObj.theUtiType.tracker) {
						[openPanel setAllowsMultipleSelection:YES];
					} else {
						[openPanel setAllowsMultipleSelection:NO];
					}
				}

			}
			break;
	}
}

@end
