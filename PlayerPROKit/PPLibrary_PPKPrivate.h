//
//  PPLibrary_PPKPrivate.h
//  PPMacho
//
//  Created by C.W. Betts on 11/28/12.
//
//

#import <PlayerPROKit/PlayerPROKit-Swift.h>

@interface PPLibrary (internalSwift)
@property (readonly) MADLibrary *theLibrary NS_RETURNS_INNER_POINTER;
//@property (readonly) NSArray *trackerLibs;
@end
