//
//  PatternHandler.h
//  PPMacho
//
//  Created by C.W. Betts on 2/3/13.
//
//

#import <Foundation/Foundation.h>
#include <PlayerPROCore/PlayerPROCore.h>

@interface PatternHandler : NSObject
{
	MADMusic **theMus;
	MADDriverRec **theRec;
	NSMutableArray *patternList;
}

- (id)initWithMusic:(MADMusic **)mus;

@property (readwrite) MADMusic **theMus;
@property (readwrite) MADDriverRec **theRec;

- (OSErr)testPcmdFileAtURL:(NSURL*)theURL;
- (OSErr)importPcmdFromURL:(NSURL*)theURL;

@end
