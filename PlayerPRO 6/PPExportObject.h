//
//  PPExportObject.h
//  PPMacho
//
//  Created by C.W. Betts on 9/22/13.
//
//

#import <Foundation/Foundation.h>
#import <PlayerPROCore/PPDefs.h>

typedef MADErr (^PPExportBlock)(NSURL* theURL, NSString **errStr);

@protocol PPExportObjectDelegate;

@interface PPExportObject : NSObject
@property (weak) NSObject<PPExportObjectDelegate> *delegate;
@property (strong) NSURL *destination;
@property (copy) PPExportBlock exportBlock;

- (id)initWithDestination:(NSURL*)dest exportBlock:(PPExportBlock)exportCode;
- (void)run;
@end

@protocol PPExportObjectDelegate <NSObject>
- (void)PPExportObjectDidFinish:(PPExportObject*)theObj;
- (void)PPExportObjectEncounteredError:(PPExportObject*)theObj errorCode:(OSErr)errCode errorString:(NSString*)errStr;
@end
