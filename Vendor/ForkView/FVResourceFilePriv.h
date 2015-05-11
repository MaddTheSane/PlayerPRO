//
//  FVResourceFilePriv.h
//  ForkView
//
//  Created by Kevin Wojniak on 5/25/11.
//  Copyright 2011 Kevin Wojniak. All rights reserved.
//

#import "FVResourceFile.h"

@class FVResource;

@interface FVResourceFile ()

- (nullable NSData *)dataForResource:(nonnull FVResource *)resource;

@end

