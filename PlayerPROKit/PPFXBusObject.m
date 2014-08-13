//
//  PPFXBusObject.m
//  PPMacho
//
//  Created by C.W. Betts on 1/8/14.
//
//

#import "PPFXBusObject.h"
#if !TARGET_OS_IPHONE
#import "PPPasteboardHandling.h"
#endif

#define kPPBypass @"PlayerPROKit FXBus ByPass"
#define kPPCopyID @"PlayerPROKit FXBus CopyId"
#define kPPIsActive @"PlayerPROKit FXBus Active"

@implementation PPFXBusObject
@synthesize theBus;

- (instancetype)init
{
	return self = [self initWithFXBus:NULL];
}

- (instancetype)initWithFXBus:(FXBus *)set
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
	theBus.ByPass = bypass;
}

- (short)copyID
{
	return theBus.copyId;
}

- (void)setCopyID:(short)copyId
{
	theBus.copyId = copyId;
}

- (BOOL)isActive
{
	return theBus.Active;
}

- (void)setActive:(BOOL)active
{
	theBus.Active = active;
}

#pragma mark NSCopying protocol

- (id)copyWithZone:(NSZone *)zone
{
	return [[[self class] allocWithZone:zone] initWithFXBus:&theBus];
}

#pragma mark NSCoding protocol
+ (BOOL)supportsSecureCoding
{
	return YES;
}

- (instancetype)initWithCoder:(NSCoder *)aDecoder
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
