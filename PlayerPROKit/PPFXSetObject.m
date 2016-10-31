//
//  PPFXSetObject.m
//  PPMacho
//
//  Created by C.W. Betts on 1/8/14.
//
//

#import "PPFXSetObject.h"
#if !(TARGET_OS_IPHONE || TARGET_OS_TV)
#import "PPPasteboardHandling.h"
#endif

#define kPPTrack @"PlayerPROKit FXSets Track"
#define kPPIdentifier @"PlayerPROKit FXSets Identifier"
#define kPPFXIdentifier @"PlayerPROKit FXSets FXIdentifier"
#define kPPArgumentNumbers @"PlayerPROKit FXSets Argument Numbers"
#define kPPValues @"PlayerPROKit FXSets Values"
#define kPPName @"PlayerPROKit FXSets Name"

@interface PPFXSetObject ()
@property (readwrite, nonatomic, strong) NSMutableArray *sets;
- (instancetype)initWithCoder:(NSCoder *)aDecoder NS_DESIGNATED_INITIALIZER;
@end

@implementation PPFXSetObject
@synthesize theSet;
@synthesize name = _name;

#if !(TARGET_OS_IPHONE || TARGET_OS_TV)
NSString * const kPPKFXSetPasteboardUTI = @"net.sourceforge.playerpro.FXSet";

static NSArray *UTIArray;
static dispatch_once_t initUTIOnceToken;
static const dispatch_block_t initUTIArray = ^{
	UTIArray = @[kPPKFXSetPasteboardUTI];
};

+ (NSArray *)readableTypesForPasteboard:(NSPasteboard *)pasteboard
{
	dispatch_once(&initUTIOnceToken, initUTIArray);
	return UTIArray;
}

- (NSArray *)writableTypesForPasteboard:(NSPasteboard *)pasteboard
{
	dispatch_once(&initUTIOnceToken, initUTIArray);
	return UTIArray;
}
- (id)pasteboardPropertyListForType:(NSString *)type
{
	if ([type isEqualToString:kPPKFXSetPasteboardUTI])
		return [NSKeyedArchiver archivedDataWithRootObject:self];
	else
		return nil;
}

+ (NSPasteboardReadingOptions)readingOptionsForType:(NSString *)type pasteboard:(NSPasteboard *)pasteboard
{
	if ([type isEqualToString:kPPKFXSetPasteboardUTI])
		return NSPasteboardReadingAsKeyedArchive;
	else
		return NSPasteboardReadingAsData;
}
#endif

- (NSString*)name
{
	if (!_name) {
		_name = CFBridgingRelease(CFStringCreateWithPascalString(kCFAllocatorDefault, theSet.name, kCFStringEncodingMacRoman));
		if (!_name) {
			_name = @"";
			memset(theSet.name, 0, sizeof(theSet.name));
		}
	}
	
	return _name;
}

- (void)setName:(NSString *)name
{
	if (![name isEqualToString:_name]) {
		if (CFStringGetPascalString((__bridge CFStringRef)name, theSet.name, 63, kCFStringEncodingMacRoman) == false) {
			memset(theSet.name, 0, sizeof(theSet.name));
		}
		_name = CFBridgingRelease(CFStringCreateWithPascalString(kCFAllocatorDefault, theSet.name, kCFStringEncodingMacRoman));
	}
}

- (instancetype)init
{
	FXSets tmpSets = {0};
	if (self = [self initWithFXSet:&tmpSets]) {
		
	}
	return self;
}

- (instancetype)initWithFXSet:(FXSets*)theSett
{
	if (self = [super init]) {
		if (!theSett) {
			return nil;
		}
		theSet = *theSett;
		_sets = [[NSMutableArray alloc] initWithCapacity:100];
		for (int i = 0; i < 100; i++) {
			[self.sets addObject:@(theSet.values[i])];
		}
	}
	return self;
}

- (short)track
{
	return theSet.track;
}

- (void)setTrack:(short)track
{
	theSet.track = track;
}

- (short)identifier
{
	return theSet.id;
}

- (void)setIdentifier:(short)identifier
{
	theSet.id = identifier;
}

- (int)effectIdentifier
{
	return theSet.FXID;
}

- (void)setEffectIdentifier:(int)fxIdentifier
{
	theSet.FXID = fxIdentifier;
}

- (short)countOfArguments
{
	return theSet.noArg;
}

- (void)setCountOfArguments:(short)argumentNumbers
{
	theSet.noArg = argumentNumbers;
}

- (NSArray*)effectValues
{
	return [NSArray arrayWithArray:_sets];
}

- (FXSets)theSet
{
	return theSet;
}

- (void)replaceEffectValueAtIndex:(NSInteger)theLoc withFloat:(float)theNum
{
	[self replaceEffectValueAtIndex:theLoc withNumber:@(theNum)];
}

- (void)replaceEffectValueAtIndex:(NSInteger)theLoc withDouble:(double)theNum
{
	[self replaceEffectValueAtIndex:theLoc withNumber:@(theNum)];
}

- (void)replaceEffectValueAtIndex:(NSInteger)theLoc withInt:(int)theNum
{
	[self replaceEffectValueAtIndex:theLoc withNumber:@(theNum)];
}

- (void)replaceEffectValueAtIndex:(NSInteger)theLoc withInteger:(NSInteger)theNum
{
	[self replaceEffectValueAtIndex:theLoc withNumber:@(theNum)];
}

- (void)replaceEffectValueAtIndex:(NSInteger)theLoc withNumber:(NSNumber*)theNum
{
	NSParameterAssert(theLoc < 100 && theLoc > -1);
	NSIndexSet *tmpIdx = [NSIndexSet indexSetWithIndex:theLoc];
	[self willChange:NSKeyValueChangeReplacement valuesAtIndexes:tmpIdx forKey:@"sets"];
	self.sets[theLoc] = theNum;
	theSet.values[theLoc] = [theNum floatValue];
	[self didChange:NSKeyValueChangeReplacement valuesAtIndexes:tmpIdx forKey:@"sets"];
}

- (void)setSets:(NSMutableArray<NSNumber*> *)sets
{
	_sets = sets;
	dispatch_apply(100, dispatch_get_global_queue(0, 0), ^(size_t i) {
		theSet.values[i] = [_sets[i] floatValue];
	});
}

- (float)effectValueAtIndex:(NSInteger)idx
{
	return [_sets[idx] floatValue];
}

- (NSInteger)countOfValues
{
	return 100;
}

#pragma mark Equatable function

- (BOOL)isEqual:(id)anObj
{
	if (anObj == NULL) {
		return NO;
	}
	if ([anObj isKindOfClass:[PPFXSetObject class]]) {
		PPFXSetObject *unwrapped = anObj;
		if (unwrapped.track != self.track) {
			return NO;
		}
		if (unwrapped.identifier != self.identifier) {
			return NO;
		}
		if (unwrapped.effectIdentifier != self.effectIdentifier) {
			return NO;
		}
		if (unwrapped.countOfArguments != self.countOfArguments) {
			return NO;
		}
		if ([unwrapped.name isEqualToString:self.name]) {
			return NO;
		}
		for (int i = 0; i < self.countOfArguments; i++) {
			if ([self effectValueAtIndex:i] != [unwrapped effectValueAtIndex:i]) {
				return NO;
			}
		}
		
		
		return YES;
	}
	return NO;
}

#pragma mark KVO/KVC helpers

+ (NSSet*)keyPathsForValuesAffectingTheSet
{
	return [NSSet setWithObjects:@"track", @"identifier", @"fxIdentifier", @"argumentNumbers", @"effectValues", @"name", @"sets", nil];
}

+ (NSSet *)keyPathsforValuesAffectingEffectValues
{
	return [NSSet setWithObject:@"sets"];
}

#pragma mark NSCopying protocol

- (id)copyWithZone:(NSZone *)zone
{
	PPFXSetObject *new = [[[self class] alloc] initWithFXSet:&theSet];
	if (new) {
		new.sets = [_sets mutableCopy];
		new.name = _name;
	}
	
	return new;
}

#pragma mark NSCoding protocol

+ (BOOL)supportsSecureCoding
{
	return YES;
}

- (instancetype)initWithCoder:(NSCoder *)aDecoder
{
	if (self = [super init]) {
		self.name = [aDecoder decodeObjectForKey:kPPName];
		self.effectIdentifier = [aDecoder decodeIntForKey:kPPFXIdentifier];
		self.sets = [(NSArray*)[aDecoder decodeObjectForKey:kPPValues] mutableCopy];
		self.track = [aDecoder decodeIntForKey:kPPTrack];
		self.identifier = [aDecoder decodeIntForKey:kPPIdentifier];
		self.countOfArguments = [aDecoder decodeIntForKey:kPPArgumentNumbers];
	}
	return self;
}

- (void)encodeWithCoder:(NSCoder *)aCoder
{
	[aCoder encodeObject:self.name forKey:kPPName];
	[aCoder encodeObject:_sets forKey:kPPValues];
	[aCoder encodeInt:self.effectIdentifier forKey:kPPFXIdentifier];
	[aCoder encodeInt:self.track forKey:kPPTrack];
	[aCoder encodeInt:self.countOfArguments forKey:kPPArgumentNumbers];
	[aCoder encodeInt:self.identifier forKey:kPPIdentifier];
}

@end
