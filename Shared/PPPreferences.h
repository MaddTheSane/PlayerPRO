//
//  Preferences.h
//  PPMacho
//
//  Created by C.W. Betts on 7/14/10.
//  Copyright 2010 __MyCompanyName__. All rights reserved.
//

#import <Cocoa/Cocoa.h>

typedef NS_ENUM(NSInteger, PPPlaylistMode) {
	PPStopPlaying = 0,
	PPLoopMusic,
	PPLoadNext,
	PPLoadRandom
};

typedef struct _tagCoupling {
	int amount;
	NSInteger tag;
} TagCoupling;

@protocol PPPreferenceObject <NSObject>

+ (instancetype)newPreferenceView;

@end

@interface PPPreferences : NSWindowController
@property (weak) IBOutlet NSBox *box;
@property (weak) IBOutlet NSPopUpButton *popUp;

-(IBAction)changeViewController:(id)sender;
-(void)displayViewController:(NSViewController *)vc;

@end
