//
//  Preferences.h
//  PPMacho
//
//  Created by C.W. Betts on 7/14/10.
//  Copyright 2010 __MyCompanyName__. All rights reserved.
//

#import <Cocoa/Cocoa.h>

enum {
	PPStopPlaying = 0,
	PPLoopMusic,
	PPLoadNext,
	PPLoadRandom
};

typedef struct _tagCoupling {
	int amount;
	NSInteger tag;
} TagCoupling;

@interface PPPreferences : NSWindowController {
	IBOutlet NSBox *box;
	IBOutlet NSPopUpButton *popUp;
	NSArray *viewControllers;
}

-(IBAction)changeViewController:(id)sender;
-(void)displayViewController:(NSViewController *)vc;

@end
