//
//  WaveViewController.h
//  PPMacho
//
//  Created by C.W. Betts on 7/26/14.
//
//

#import <Cocoa/Cocoa.h>

@class PPDocument;

@interface WaveViewController : NSViewController

@property (weak) IBOutlet PPDocument *currentDocument;

@end
