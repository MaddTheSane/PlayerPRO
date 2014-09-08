//
//  PPNoteTranslatePlug.m
//  PPMacho
//
//  Created by C.W. Betts on 9/7/14.
//
//

#import "PPNoteTranslatePlug.h"
#import "NoteTranslateController.h"

@implementation PPNoteTranslatePlug

- (BOOL)hasUIConfiguration
{
	return YES;
}

- (MADErr)runWithPcmd:(inout Pcmd *)aPcmd driver:(PPDriver *)driver
{
	return MADOrderNotImplemented;
}

- (void)beginRunWithPcmd:(Pcmd *)aPcmd driver:(PPDriver *)driver parentDocument:(NSDocument *)document handler:(PPPlugErrorBlock)handle
{
	NoteTranslateController *controller = [[NoteTranslateController alloc] initWithWindowNibName:@"NoteTranslateController"];
	controller.thePcmd = aPcmd;
	controller.transAmount = 0;
	controller.currentBlock = handle;
	
	[[controller window] beginSheet:[document windowForSheet] completionHandler:^(NSModalResponse returnCode) {
		;
	}];
}

@end
