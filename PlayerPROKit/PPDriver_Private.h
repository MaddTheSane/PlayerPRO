//
//  PPDriver_Private.h
//  PPMacho
//
//  Created by C.W. Betts on 12/1/12.
//
//

#import "PPDriver.h"

@interface PPDriver ()

@property (readonly) MADDriverRec *rec;
@property (readwrite, strong) PPLibrary *theLibrary;

@end
