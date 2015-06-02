//
//  PPInstrumentPlugBridgeObject.h
//  PPMacho
//
//  Created by C.W. Betts on 9/20/14.
//
//

#import <Foundation/Foundation.h>
#import <PlayerPROCore/PlayerPROCore.h>
#import <PlayerPROCore/MADPlug.h>

@interface PPInstrumentPlugBridgeObject : NSObject
{
@private
	NSBundle *_bundleFile;
	PPInstrumentPlugin **xxxx;
	BOOL _isSample;
}
@property (retain, readonly) NSBundle *bundleFile;
@property PPInstrumentPlugin **xxxx;
@property (readonly, getter=isSample) BOOL sample;

- (instancetype)init UNAVAILABLE_ATTRIBUTE;
- (instancetype)initWithBundle:(NSBundle *)tempBundle;

- (BOOL)canLoadFileAtURL:(NSURL*)theURL;
- (MADErr)importURL:(NSURL*)fileURL instrument:(InstrData*)insData sampleArray:(sData**)sdataref sampleIndex:(short*)insSamp;

@end
