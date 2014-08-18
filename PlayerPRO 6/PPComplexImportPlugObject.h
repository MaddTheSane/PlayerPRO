//
//  PPComplexImportPlugObject.h
//  PPMacho
//
//  Created by C.W. Betts on 8/17/14.
//
//

#import <Foundation/Foundation.h>

@protocol PPComplexImportPlugObject <NSObject>
- (BOOL)canImportURL:(NSURL*)theURL;
- (instancetype)initWithBundle:(NSBundle*)ourBundle;


@optional
- (BOOL)canImportURL:(NSURL*)theURL error:(out NSError**)outErr;
- (NSDictionary*)getTrackerInformationFromURL:(NSURL*)theURL;
- (NSDictionary*)getTrackerInformationFromURL:(NSURL*)theURL error:(out NSError**)outErr;
@end
