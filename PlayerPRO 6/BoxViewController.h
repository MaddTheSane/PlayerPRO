//
//  BoxViewController.h
//  PPMacho
//
//  Created by C.W. Betts on 9/30/13.
//
//

#import <Cocoa/Cocoa.h>

@class PPDocument;

@interface BoxViewController : NSViewController

@property (weak) IBOutlet PPDocument *currentDocument;

@end
