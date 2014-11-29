//
//  PPAPPLHelpers.h
//  PPMacho
//
//  Created by C.W. Betts on 11/29/14.
//
//

#import <Foundation/Foundation.h>

void PPGetResInfo(Handle resHandle, ResID *resIDShort, ResType *resTypeCode, long *size, NSString **name);
NSData *PPAPPLDataFromResource(ResID resourceID, ResType resourceType);


Handle PPGet1IndResource(ResType type, ResourceIndex index);
