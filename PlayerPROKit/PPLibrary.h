//
//  PPLibrary.h
//  PPMacho
//
//  Created by C.W. Betts on 10/28/12.
//
//

#import <Foundation/Foundation.h>
#import <PlayerPROCore/PlayerPROCore.h>

@interface PPLibrary : NSObject
{
	MADLibrary *theLibrary;
}
- (id)initWithPlugInPath:(NSString *)path;
- (id)initWithPlugInURL:(NSURL *)URL;

@end
