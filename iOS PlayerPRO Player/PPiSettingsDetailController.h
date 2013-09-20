//
//  PPiSettingsDetailController.h
//  PPMacho
//
//  Created by C.W. Betts on 9/20/13.
//
//

#import <Foundation/Foundation.h>

@interface PPiSettingsDetailController : UIViewController <UISplitViewControllerDelegate>
	
	@property (strong, nonatomic) id detailItem;
	
	@property (weak, nonatomic) IBOutlet UILabel *detailDescriptionLabel;
@end
