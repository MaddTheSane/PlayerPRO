//
//  PPiMasterViewController.h
//  iOS PlayerPRO Player
//
//  Created by C.W. Betts on 9/19/13.
//
//

#import <UIKit/UIKit.h>

@class PPiDetailViewController;
@class PPiSettingsDetailController;

@interface PPiMasterViewController : UITableViewController

@property (strong, nonatomic) PPiDetailViewController *detailViewController;
//@property (strong, nonatomic) PPiSettingsDetailController *settingsViewController;

@end
