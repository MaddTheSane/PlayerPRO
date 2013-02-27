//
//  OpenPanelViewController.h
//  PPMacho
//
//  Created by C.W. Betts on 12/21/12.
//
//

#import <Cocoa/Cocoa.h>

@interface OpenPanelViewController : NSViewController
{
	NSOpenPanel *openPanel;
	NSArray *utiObjects;
	//IBOutlet NSMenu *fileTypeSelectionMenu;
	IBOutlet NSPopUpButton *popUp;
}

- (IBAction)selectUTI:(id)sender;
- (id)initWithOpenPanel:(NSOpenPanel*)panel trackerDictionary:(NSDictionary *)td playlistDictionary:(NSDictionary*)pd;
- (id)initWithOpenPanel:(NSOpenPanel*)panel trackerDictionary:(NSDictionary *)td playlistDictionary:(NSDictionary*)pd instrumentDictionary:(NSDictionary*)insDict;
- (id)initWithOpenPanel:(NSOpenPanel*)panel trackerDictionary:(NSDictionary *)td playlistDictionary:(NSDictionary*)pd instrumentDictionary:(NSDictionary*)insDict additionalDictionary:(NSDictionary *)adddict;
- (id)initWithOpenPanel:(NSOpenPanel*)panel trackerDictionary:(NSDictionary *)td playlistDictionary:(NSDictionary*)pd additionalDictionary:(NSDictionary *)adddict;

- (void)setupDefaults;

@end
