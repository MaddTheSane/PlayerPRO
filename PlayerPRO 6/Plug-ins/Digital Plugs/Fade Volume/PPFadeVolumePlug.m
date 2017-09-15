//
//  PPFadeVolumePlug.m
//  PPMacho
//
//  Created by C.W. Betts on 9/11/14.
//
//

#import "PPFadeVolumePlug.h"
#import "FadeVolumeController.h"

@implementation PPFadeVolumePlug 

- (BOOL)hasUIConfiguration
{
	return YES;
}

- (instancetype)initForPlugIn
{
	return self = [self init];
}

- (BOOL)runWithPcmd:(inout Pcmd *)aPcmd driver:(PPDriver *)driver error:(NSError * _Nullable __autoreleasing * _Nonnull)error
{
	if (error) {
		*error = [NSError errorWithDomain:PPMADErrorDomain code:MADOrderNotImplemented userInfo:nil];
	}
	
	return NO;
}

- (void)beginRunWithPcmd:(Pcmd *)aPcmd driver:(PPDriver *)driver parentWindow:(NSWindow*)document handler:(PPPlugErrorBlock)handle
{
	FadeVolumeController *controller = [[FadeVolumeController alloc] initWithWindowNibName:@"FadeVolumeController"];
	controller.thePcmd = aPcmd;
	controller.fadeFrom = 0.0;
	controller.fadeTo = 1.0;
	controller.currentBlock = handle;
	controller.parentWindow = document;

	[document beginSheet:controller.window completionHandler:^(NSModalResponse returnCode) {
		
	}];
}

@end
