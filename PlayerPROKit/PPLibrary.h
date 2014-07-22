//
//  PPLibrary.h
//  PPMacho
//
//  Created by C.W. Betts on 10/28/12.
//
//

#import <Foundation/Foundation.h>
#import <PlayerPROCore/PlayerPROCore.h>

@interface PPLibraryObject : NSObject
@property (readonly, copy) NSString *menuName;
@property (readonly, copy) NSString *authorString;
#if !TARGET_OS_IPHONE
@property (readonly, strong) NSBundle *plugFile;
#endif
@property (readonly, strong) NSString *plugType;
@property (readonly, copy) NSArray* UTItypes;
@property (readonly) OSType plugMode;
@property (readonly) BOOL canExport;
@property (readonly) BOOL canImport;
@property (readonly) UInt32 plugVersion;
@end

@interface PPLibrary : NSObject <NSFastEnumeration>
- (instancetype)init;
- (instancetype)initWithPlugInPath:(NSString *)path;
- (instancetype)initWithPlugInURL:(NSURL *)URL;

- (PPLibraryObject *)pluginAtIndex:(NSUInteger)idx;
@property (readonly) NSUInteger pluginCount;

- (OSErr)identifyFileAtPath:(NSString*)apath type:(char*)atype;
- (OSErr)identifyFileAtURL:(NSURL*)apath type:(char*)atype;

- (OSErr)getInformationFromFileAtPath:(NSString*)apath type:(char*)atype info:(PPInfoRec*)infoRec;
- (OSErr)getInformationFromFileAtURL:(NSURL*)apath type:(char*)atype info:(PPInfoRec*)infoRec;

@end
