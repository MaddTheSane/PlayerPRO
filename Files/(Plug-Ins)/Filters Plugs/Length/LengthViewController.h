//
//  LengthViewController.h
//  PPMacho
//
//  Created by C.W. Betts on 9/10/13.
//
//

#import <Cocoa/Cocoa.h>

@interface LengthViewController : NSViewController
@property (weak) IBOutlet NSTextField *samplesCount;
@property int dataSize;
@property int samplesNumCount;
@end