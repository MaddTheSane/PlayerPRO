//
//  PPiDetailViewController.h
//  iOS PlayerPRO Player
//
//  Created by C.W. Betts on 9/19/13.
//
//

#import <UIKit/UIKit.h>

@interface PPiDetailViewController : UIViewController <UISplitViewControllerDelegate>

@property (strong, nonatomic) id detailItem;

@property (weak, nonatomic) IBOutlet UILabel *detailDescriptionLabel;
@end
