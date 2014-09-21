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
@property (strong, readonly) NSBundle *bundleFile;
@property PPInstrumentPlugin **xxxx;

- (instancetype)init UNAVAILABLE_ATTRIBUTE;
- (instancetype)initWithBundle:(NSBundle *)tempBundle;

@end
