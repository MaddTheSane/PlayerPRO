//
//  PPPlugInInfoController.h
//  PPMacho
//
//  Created by C.W. Betts on 12/31/12.
//
//

#import <Cocoa/Cocoa.h>
@class PPPlugInInfo;

@interface PPPlugInInfoController : NSWindowController
{
	PPPlugInInfo *info;
	IBOutlet NSTextField *nameField;
	IBOutlet NSTextField *authorField;
	IBOutlet NSTextField *typeField;
}
- (id)initWithPlugInInfo:(PPPlugInInfo *)plugInfo;
- (IBAction)closeWindow:(id)sender;

@end
