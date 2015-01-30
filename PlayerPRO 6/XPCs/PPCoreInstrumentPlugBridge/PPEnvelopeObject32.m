//
//  PPEnvelopeObject32.m
//  PPMacho
//
//  Created by C.W. Betts on 9/21/14.
//
//

#import "PPEnvelopeObject32.h"

#pragma mark PlayerPROKit Envelope NSCoding keys
#define PPEnvPos @"PlayerPROKit EnvRec Position"
#define PPEnvVal @"PlayerPROKit EnvRec Value"

@implementation PPEnvelopeObject32
@synthesize envelopeRec;

#if __i386__
+ (void)initialize
{
	static dispatch_once_t onceToken;
	dispatch_once(&onceToken, ^{
		[NSKeyedArchiver setClassName:@"PPEnvelopeObject" forClass:[PPEnvelopeObject32 class]];
		[NSKeyedUnarchiver setClass:[PPEnvelopeObject32 class] forClassName:@"PPEnvelopeObject"];
	});
}
#endif

- (short)position
{
	return envelopeRec.pos;
}

- (void)setPosition:(short)position
{
	envelopeRec.pos = position;
}

- (short)value
{
	return envelopeRec.val;
}

- (void)setValue:(short)value
{
	envelopeRec.val = value;
}

- (instancetype)init
{
	if (self = [super init]) {
		
	}
	return self;
}

- (instancetype)initWithEnvRec:(EnvRec)theRec
{
	if (self = [super init]) {
		envelopeRec = theRec;
	}
	
	return self;
}

#pragma mark - NSCoding
+ (BOOL)supportsSecureCoding
{
	return YES;
}

- (instancetype)initWithCoder:(NSCoder *)aDecoder
{
	if (self = [super init]) {
		envelopeRec.pos = [aDecoder decodeIntForKey:PPEnvPos];
		envelopeRec.val = [aDecoder decodeIntForKey:PPEnvVal];
	}
	return self;
}

- (void)encodeWithCoder:(NSCoder *)aCoder
{
	[aCoder encodeInt:envelopeRec.pos forKey:PPEnvPos];
	[aCoder encodeInt:envelopeRec.val forKey:PPEnvVal];
}

@end
