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
@property (weak) IBOutlet NSTextField *nameField;
@property (weak) IBOutlet NSTextField *authorField;
@property (weak) IBOutlet NSTextField *typeField;
@property (weak) IBOutlet NSTextField *theCopyrightField;
@property (weak) IBOutlet NSImageView *pluginImage;

- (id)initWithPlugInInfo:(PPPlugInInfo *)plugInfo;

- (IBAction)closeWindow:(id)sender;
@end
