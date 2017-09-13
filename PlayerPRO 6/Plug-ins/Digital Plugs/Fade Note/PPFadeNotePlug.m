//
//  PPFadeNotePlug.m
//  PPMacho
//
//  Created by C.W. Betts on 9/11/14.
//
//

#import "PPFadeNotePlug.h"
#import "FadeNoteController.h"

@implementation PPFadeNotePlug

- (BOOL)hasUIConfiguration
{
	return YES;
}

- (instancetype)initForPlugIn
{
	return self = [self init];
}

- (BOOL)runWithPcmd:(inout Pcmd *)aPcmd driver:(PPDriver *)driver error:(NSError * _Nullable __autoreleasing * _Nullable)error
{
	if (error) {
		*error = [NSError errorWithDomain:PPMADErrorDomain code:MADOrderNotImplemented userInfo:nil];
	}
	
	return NO;
}

- (void)beginRunWithPcmd:(Pcmd *)aPcmd driver:(PPDriver *)driver parentWindow:(NSWindow*)document handler:(PPPlugErrorBlock)handle
{
	FadeNoteController *controller = [[FadeNoteController alloc] initWithWindowNibName:@"FadeNoteController"];
	controller.thePcmd = aPcmd;
	controller.initialFrom = @"C 3";
	controller.initialTo = @"C 7";
	controller.currentBlock = handle;
	controller.parentWindow = document;
	
	[document beginSheet:controller.window completionHandler:^(NSModalResponse returnCode) {
		
	}];
}

@end
