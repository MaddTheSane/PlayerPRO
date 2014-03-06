//
//  PPExportObject.m
//  PPMacho
//
//  Created by C.W. Betts on 9/22/13.
//
//

#import "PPExportObject.h"

@implementation PPExportObject
@synthesize delegate;

- (id)initWithDestination:(NSURL*)dest exportBlock:(PPExportBlock)exportCode
{
	if (self = [super init]) {
		self.exportBlock = exportCode;
		self.destination = dest;
	}
	return self;
}

- (void)run
{
	dispatch_async(dispatch_get_global_queue(0, 0), ^{
		OSErr retErr = noErr;
		__block NSString *errStr = nil;
		
		retErr = self.exportBlock(self.destination, &errStr);

		if (delegate) {
			dispatch_async(dispatch_get_main_queue(), ^{
				if (retErr == noErr) {
					[delegate PPExportObjectDidFinish:self];
				} else {
					if (errStr == nil) {
						NSError *tmpErr = CreateErrorFromMADErrorType(retErr);
						errStr = [tmpErr description];
					}
					[delegate PPExportObjectEncounteredError:self errorCode:retErr errorString:errStr];
				}
			});
		}
	});
}

@end
