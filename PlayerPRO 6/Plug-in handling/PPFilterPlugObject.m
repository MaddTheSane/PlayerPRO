//
//  PPFilterPlugObject.m
//  PPMacho
//
//  Created by C.W. Betts on 1/22/13.
//
//

#import "PPFilterPlugObject.h"
#import "PPPlugInCommon.h"

#define PPFilterLoadPlug(theBundle) (PPFiltersPlugin**)GetCOMPlugInterface(theBundle, kPlayerPROFiltersPlugTypeID, kPlayerPROFiltersPlugInterfaceID)

@interface PPFilterPlugObject ()
@property (strong) id<PPFilterPlugin> plugCode;
@property BOOL hasUI;
@end

@implementation PPFilterPlugObject
@synthesize plugCode;

- (BOOL)isEqual:(id)object
{
	if (!object) {
		return NO;
	}
	
	if (self == object) {
		return YES;
	}
	
	if ([object isKindOfClass:[PPFilterPlugObject class]]) {
		if (![self.menuName isEqualToString:[object menuName]]) {
			return NO;
		} else if (![self.file isEqual:[object file]]) {
			return NO;
			//Ignore version, authorstring and type
		} else return YES;
	}
	return NO;
}

- (NSUInteger)hash
{
	return [self.menuName hash];
}

- (instancetype)initWithBundle:(NSBundle *)aBund
{
	if (self = [super initWithBundle:aBund]) {
		{
			NSArray *archs = aBund.executableArchitectures;
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
		
		Class bundClass = [aBund principalClass];
		if ([bundClass conformsToProtocol:@protocol(PPFilterPlugin)]) {
			self.plugCode = [[bundClass alloc] initForPlugIn];
		}
		
		if (!plugCode) {
			return nil;
		}
		
		self.hasUI = [self.plugCode hasUIConfiguration];
		
		type = 'PLug';
	}
	return self;
}

- (void)beginRunWithData:(PPSampleObject*)theData selectionRange:(NSRange)selRange onlyCurrentChannel:(BOOL)StereoMode driver:(PPDriver*)driver parentDocument:(NSDocument*)document handler:(PPPlugErrorBlock)handler
{
	MADErr iErr = [plugCode runWithData:theData selectionRange:selRange onlyCurrentChannel:StereoMode driver:driver];
	if (iErr == MADOrderNotImplemented && [plugCode respondsToSelector:@selector(beginRunWithData:selectionRange:onlyCurrentChannel:driver:parentWindow:handler:)]) {
		[plugCode beginRunWithData:theData selectionRange:selRange onlyCurrentChannel:StereoMode driver:driver parentWindow:[document windowForSheet] handler:handler];
	} else {
		handler(iErr);
	}
}

@end
