//
//  PPAPPLObject.h
//  PPMacho
//
//  Created by C.W. Betts on 9/15/14.
//
//

#import <Foundation/Foundation.h>

@interface PPAPPLObject : NSObject
@property (readonly) ResID resourceID;
@property (readonly) ResType resourceType;
@property (readonly) long size;
@property (readonly, copy) NSString *name;

- (instancetype)initWithHandle:(Handle)resHandle;
@end
