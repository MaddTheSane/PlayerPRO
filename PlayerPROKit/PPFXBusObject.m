//
//  PPFXBusObject.m
//  PPMacho
//
//  Created by C.W. Betts on 1/8/14.
//
//

#import "PPFXBusObject.h"

#define kPPBypass @"PlayerPROKit FXBus ByPass"
#define kPPCopyID @"PlayerPROKit FXBus CopyId"
#define kPPIsActive @"PlayerPROKit FXBus Active"

@implementation PPFXBusObject
@synthesize theBus;

- (id)init
{
	return self = [self initWithFXBus:NULL];
}

- (id)initWithFXBus:(FXBus *)set
{
	if (self = [super init]) {
		if (set) {
			theBus = *set;
		}
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

#pragma mark NSCopying protocol

- (id)copyWithZone:(NSZone *)zone
{
	return [[[self class] allocWithZone:zone] initWithFXBus:&theBus];
}

#pragma mark NSCoding protocol

- (id)initWithCoder:(NSCoder *)aDecoder
{
	if (self = [super init]) {
		theBus.ByPass = [aDecoder decodeBoolForKey:kPPBypass];
		theBus.Active = [aDecoder decodeBoolForKey:kPPIsActive];
		theBus.copyId = [(NSNumber*)[aDecoder decodeObjectForKey:kPPCopyID] shortValue];
	}
	return self;
}

- (void)encodeWithCoder:(NSCoder *)aCoder
{
	[aCoder encodeBool:theBus.ByPass forKey:kPPBypass];
	[aCoder encodeBool:theBus.Active forKey:kPPIsActive];
	[aCoder encodeObject:@(theBus.copyId) forKey:kPPCopyID];
}

@end
