//
//  PPDocument.h
//  PPMacho
//
//  Created by C.W. Betts on 9/7/13.
//
//

#import <Cocoa/Cocoa.h>
#import <PlayerPROKit/PlayerPROKit.h>
#import "PPSoundSettingsViewController.h"

@interface PPDocument : NSDocument <PPSoundSettingsViewControllerDelegate>
{
	MADDriverSettings		exportSettings;
}

@property (weak) IBOutlet NSWindow	*exportWindow;
@property (weak) IBOutlet NSBox		*exportSettingsBox;
@property (strong) PPSoundSettingsViewController *exportController;


- (IBAction)exportMusic:(id)sender;

@end
