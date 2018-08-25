//
//  PPSamplePlugObject-private.h
//  PPMacho
//
//  Created by C.W. Betts on 2/10/15.
//
//

#import "PPSamplePlugObject.h"
#import "PPInstrumentImporterObject-private.h"


NS_ASSUME_NONNULL_BEGIN

@interface PPSamplePlugObject ()
@property (readwrite, copy) NSArray<NSString*> *UTITypes;
@property (readwrite) BOOL canImport;
@property (readwrite) BOOL canExport;

- (nullable instancetype)initWithBundleNoInit:(NSBundle *)tempBundle NS_DESIGNATED_INITIALIZER;

@end

NS_ASSUME_NONNULL_END
