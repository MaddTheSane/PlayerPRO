//
//  Preferences.h
//  PPMacho
//
//  Created by C.W. Betts on 7/14/10.
//  Copyright 2010 __MyCompanyName__. All rights reserved.
//

#import <Cocoa/Cocoa.h>
@class ManagingViewController;

@interface PPPreferences : NSObject {
	IBOutlet NSWindow *preferenceWindow;
	IBOutlet NSBox *box;
	IBOutlet NSPopUpButton *popUp;
	NSMutableArray *viewControllers;
}
-(IBAction)changeViewController:(id)sender;
-(void)displayViewController:(ManagingViewController *)vc;

@end
