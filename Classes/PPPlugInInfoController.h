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
	IBOutlet __weak NSTextField *nameField;
	IBOutlet __weak NSTextField *authorField;
	IBOutlet __weak NSTextField *typeField;
}
- (id)initWithPlugInInfo:(PPPlugInInfo *)plugInfo;
- (IBAction)closeWindow:(id)sender;

@end
