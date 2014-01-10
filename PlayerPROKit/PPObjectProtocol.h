//
//  PPObjectProtocol.h
//  PPMacho
//
//  Created by C.W. Betts on 1/9/14.
//
//

#import <Foundation/Foundation.h>

@protocol PPObject <NSCopying, NSCoding, NSObject>
- (void)writeBackToStruct;
@end
