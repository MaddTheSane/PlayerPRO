//
//  PPDriver.h
//  PPMacho
//
//  Created by C.W. Betts on 11/28/12.
//
//

#import <Foundation/Foundation.h>
#include <PlayerPROCore/PlayerPROCore.h>

@class PPLibrary;

@interface PPDriver : NSObject
{
	MADDriverRec theRec;
	PPLibrary theLib;
}

- (id)initWithLibrary:(PPLibrary *)theLib;

- (id)initWithLibrary:(PPLibrary *)theLib settings:(MADDriverSettings *)theSettings;

@end
