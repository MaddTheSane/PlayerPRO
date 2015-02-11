//
//  PPSamplePlugObject-private.h
//  PPMacho
//
//  Created by C.W. Betts on 2/10/15.
//
//

#import "PPSamplePlugObject.h"
#import "PPInstrumentImporterObject-private.h"


@interface PPSamplePlugObject ()
@property (readwrite, copy) NSArray *UTITypes;
@property (readwrite) BOOL canImport;
@property (readwrite) BOOL canExport;

- (instancetype)initWithBundleNoInit:(NSBundle *)tempBundle NS_DESIGNATED_INITIALIZER;

@end
