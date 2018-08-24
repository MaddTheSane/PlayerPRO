//
//  PPMix.m
//  Mix
//
//  Created by C.W. Betts on 8/24/18.
//

#import "PPMix.h"
#import <PlayerPROKit/PPSampleObject.h>
#import <PlayerPROKit/PPErrors.h>

@implementation PPMix

- (BOOL)hasUIConfiguration
{
	return YES;
}

- (nullable instancetype)initForPlugIn {
	return self = [super init];
}

- (BOOL)runWithData:(inout PPSampleObject *)theData selectionRange:(NSRange)selRange onlyCurrentChannel:(BOOL)StereoMode driver:(PPDriver *)driver error:(NSError * _Nullable __autoreleasing * _Nonnull)error
{
	if (error) {
		*error = [NSError errorWithDomain:PPMADErrorDomain code:MADOrderNotImplemented userInfo:nil];
	}
	
	return NO;
}

- (void)beginRunWithData:(PPSampleObject*)theData selectionRange:(NSRange)selRange onlyCurrentChannel:(BOOL)StereoMode driver:(PPDriver*)driver parentWindow:(NSWindow*)window handler:(PPPlugErrorBlock)handler
{
	handler([NSError errorWithDomain:NSOSStatusErrorDomain code:unimpErr userInfo:nil]);
}

@end
