//
//  PPDigitalPlugInObject.m
//  PPMacho
//
//  Created by C.W. Betts on 1/1/13.
//
//

#import "PPDigitalPlugInObject.h"
#import "PPPlugInCommon.h"
#import <PlayerPROKit/PPPlugIns.h>
#import "PlayerPRO_6-Swift.h"

@interface PPDigitalPlugInObject ()
@property (strong) id<PPDigitalPlugin> plugCode;
@property BOOL hasUI;
@end

@implementation PPDigitalPlugInObject
@synthesize plugCode;

- (BOOL)isEqual:(id)object
{
	if (!object)
		return NO;
	
	if (self == object)
		return YES;
	
	if ([object isKindOfClass:[PPDigitalPlugInObject class]]) {
		if (![self.menuName isEqualToString:[object menuName]]) {
			return NO;
		} else if (![self.file isEqual:[object file]]) {
			return NO;
			//Ignore version, authorstring and type
		} else
			return YES;
	} else
		return NO;
}

- (NSUInteger)hash
{
	return [self.menuName hash];
}

- (instancetype)initWithBundle:(NSBundle*)toInit
{
	if (self = [super initWithBundle:toInit]) {
		{
			NSArray *archs = toInit.executableArchitectures;
			BOOL hasArch = NO;
			for (NSNumber *arch in archs) {
				if ([arch isEqual:@(NSBundleExecutableArchitectureX86_64)]) {
					hasArch = YES;
					break;
				}
			}
			
			if (!hasArch) {
				return nil;
			}
		}
		
		Class bundClass = [toInit principalClass];
		if (![bundClass conformsToProtocol:@protocol(PPDigitalPlugin)]) {
			return nil;
		}
		
		self.plugCode = [[bundClass alloc] init];
		self.hasUI = [self.plugCode hasUIConfiguration];
		type = 'PPDG';
	}
	return self;
}

- (void)beginCallWithPcmd:(inout Pcmd*)myPcmd driver:(PPDriver*)driver parentDocument:(PPDocument*)theDoc handler:(PPPlugErrorBlock)handle
{
	if (self.hasUI) {
		[plugCode beginRunWithPcmd:myPcmd driver:driver parentDocument:theDoc handler:handle];
	} else {
		MADErr ourErr = [plugCode runWithPcmd:myPcmd driver:driver];
		handle(ourErr);
	}
}

@end
