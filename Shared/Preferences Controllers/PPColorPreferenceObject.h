//
//  PPColorPreferenceObject.h
//  PPMacho
//
//  Created by C.W. Betts on 8/21/14.
//
//

#import <Foundation/Foundation.h>

@interface PPColorPreferenceObject : NSObject
@property (strong) NSColor *color;
@property (readonly) NSInteger colorIndex;

- (instancetype)initWithColor:(NSColor*)aColor index:(NSInteger)idx;

@end
