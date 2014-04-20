//
//  PPSTImporter.h
//  PPMacho
//
//  Created by C.W. Betts on 4/20/14.
//
//

#import <Foundation/Foundation.h>

@protocol PPSTImporterHelper <NSObject>



@end

@interface PPSTImporter : NSObject <NSXPCListenerDelegate>

+ (instancetype)sharedImporter;

@end
