//
//  PPComplexImportPlugObject.h
//  PPMacho
//
//  Created by C.W. Betts on 8/18/14.
//
//

#import <Foundation/Foundation.h>
#import "PPComplexImportPlugInterface.h"

/**
 @class PPComplexImportPlugObject
 A tiny shim to make Swift behave.
 
 */

@interface PPComplexImportPlugObject : NSObject <PPComplexImportPlugInterfaceBase>
@property (readonly, strong) NSBundle *ourBundle;

- (instancetype)initWithBundle:(NSBundle*)ourBundle;

@end
