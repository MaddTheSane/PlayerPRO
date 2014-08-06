//
//  PPMIDIImportHelper.h
//  PPMacho
//
//  Created by C.W. Betts on 8/6/14.
//
//

#import <Foundation/Foundation.h>

@protocol PPMIDIImportHelper <NSObject>



@end

@interface PPMIDIImporter : NSObject <NSXPCListenerDelegate, PPMIDIImportHelper>
+ (instancetype)sharedImporter;

@end
