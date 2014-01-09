//
//  PPFXBusObject.m
//  PPMacho
//
//  Created by C.W. Betts on 1/8/14.
//
//

#import "PPFXBusObject.h"

@implementation PPFXBusObject
@synthesize theBus;

- (id)initWithFXBus:(FXBus *)set
{
	if (self = [super init]) {
		if (!set) {
			return nil;
		}
		theBus = *set;
	}
	return self;
}

- (BOOL)bypass
{
	return theBus.ByPass;
}

- (void)setBypass:(BOOL)bypass
{
	[self willChangeValueForKey:@"bypass"];
	theBus.ByPass = bypass;
	[self didChangeValueForKey:@"bypass"];
}

- (short)copyId
{
	return theBus.copyId;
}

- (void)setCopyId:(short)copyId
{
	[self willChangeValueForKey:@"copyId"];
	theBus.copyId = copyId;
	[self didChangeValueForKey:@"copyId"];
}

- (BOOL)isActive
{
	return theBus.Active;
}

- (void)setActive:(BOOL)active
{
	[self willChangeValueForKey:@"active"];
	theBus.Active = active;
	[self didChangeValueForKey:@"active"];
}

#pragma mark NSCoding protocol

- (id)copyWithZone:(NSZone *)zone
{
	return [[[self class] allocWithZone:zone] initWithFXBus:&theBus];
}

@end
