//
//  PPExportObject.h
//  PPMacho
//
//  Created by C.W. Betts on 9/22/13.
//
//

#import <Foundation/Foundation.h>

typedef OSErr (^PPExportBlock)(NSURL* theURL, NSString **errStr);


@protocol PPExportObjectDelegate;

@interface PPExportObject : NSObject

- (id)initWithExportBlock:(PPExportBlock)exportCode destination:(NSURL*)dest;
- (id)initWithDestination:(NSURL*)dest exportBlock:(PPExportBlock)exportCode;

@property (weak) NSObject<PPExportObjectDelegate> *delegate;
@property (strong) NSURL *destination;
@property (copy) PPExportBlock exportBlock;
@end

@protocol PPExportObjectDelegate <NSObject>
- (void)PPExportObjectDidFinish:(PPExportObject*)theObj;
- (OSErr)PPExportObjectEncounteredError:(PPExportObject*)theObj;
@end
