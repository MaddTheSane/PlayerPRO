//
//  PPExportObject.m
//  PPMacho
//
//  Created by C.W. Betts on 9/22/13.
//
//

#import "PPExportObject.h"

@implementation PPExportObject

	- (id)initWithDestination:(NSURL*)dest exportBlock:(PPExportBlock)exportCode
	{
		return [self initWithExportBlock:exportCode destination:dest];
	}
	
- (id)initWithExportBlock:(PPExportBlock)exportCode destination:(NSURL*)dest
{
	if (self = [super init]) {
		self.exportBlock = exportCode;
		self.destination = dest;
	}
	return self;
}
	
@end
