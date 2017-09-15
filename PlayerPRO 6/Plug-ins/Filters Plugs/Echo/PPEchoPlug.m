//
//  PPEchoPlug.m
//  PPMacho
//
//  Created by C.W. Betts on 9/11/14.
//
//

#import "PPEchoPlug.h"
#import "EchoWindowController.h"
#import <Cocoa/Cocoa.h>
#import <PlayerPROKit/PlayerPROKit.h>

@implementation PPEchoPlug

- (BOOL)hasUIConfiguration
{
	return YES;
}

- (instancetype)initForPlugIn
{
	return self = [self init];
}

- (BOOL)runWithData:(inout PPSampleObject *)theData selectionRange:(NSRange)selRange onlyCurrentChannel:(BOOL)StereoMode driver:(PPDriver *)driver error:(NSError * _Nullable __autoreleasing * _Nonnull)error
{
	if (error) {
		*error = [NSError errorWithDomain:PPMADErrorDomain code:MADOrderNotImplemented userInfo:nil];
	}
	
	return NO;
}

- (void)beginRunWithData:(PPSampleObject *)theData selectionRange:(NSRange)selRange onlyCurrentChannel:(BOOL)StereoMode driver:(PPDriver *)driver parentWindow:(NSWindow*)document handler:(PPPlugErrorBlock)handle
{
	EchoWindowController *controller = [[EchoWindowController alloc] initWithWindowNibName:@"EchoWindowController"];
	controller.echoStrength = 0.50;
	controller.echoDelay = 250;
	controller.theData = theData;
	controller.selectionRange = selRange;
	controller.currentBlock = handle;
	controller.parentWindow = document;

	[document beginSheet:controller.window completionHandler:^(NSModalResponse returnCode) {
		
	}];
}

@end
