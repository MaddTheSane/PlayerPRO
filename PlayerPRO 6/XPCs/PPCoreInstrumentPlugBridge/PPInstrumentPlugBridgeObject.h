//
//  PPInstrumentPlugBridgeObject.h
//  PPMacho
//
//  Created by C.W. Betts on 9/20/14.
//
//

#import <Foundation/Foundation.h>
#import <PlayerPROCore/PlayerPROCore.h>

@interface PPInstrumentPlugBridgeObject : NSObject
{
@private
	NSBundle *_bundleFile;
	PPInstrumentPlugin **xxxx;
}
@property (retain, readonly) NSBundle *bundleFile;
@property PPInstrumentPlugin **xxxx;

- (instancetype)init UNAVAILABLE_ATTRIBUTE;
- (instancetype)initWithBundle:(NSBundle *)tempBundle;

- (BOOL)canLoadFileAtURL:(NSURL*)theURL;
- (MADErr)importURL:(NSURL*)fileURL instrument:(InstrData*)insData sampleArray:(sData**)sdataref sampleIndex:(short*)insSamp;

@end
