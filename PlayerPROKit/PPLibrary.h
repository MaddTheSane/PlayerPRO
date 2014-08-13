//
//  PPLibrary.h
//  PPMacho
//
//  Created by C.W. Betts on 10/28/12.
//
//

#import <Foundation/Foundation.h>
#import <PlayerPROCore/PlayerPROCore.h>

#ifndef NS_DESIGNATED_INITIALIZER
#define NS_DESIGNATED_INITIALIZER
#endif

@interface PPLibraryObject : NSObject
@property (readonly, copy) NSString *menuName;
@property (readonly, copy) NSString *authorString;
@property (readonly, strong) NSBundle *plugFile;
@property (readonly, strong) NSString *plugType;
@property (readonly, copy) NSArray* UTItypes;
@property (readonly) OSType plugMode;
@property (readonly) BOOL canExport;
@property (readonly) BOOL canImport;
@property (readonly) UInt32 plugVersion;
@end

@interface PPLibrary : NSObject <NSFastEnumeration>
- (instancetype)init;
- (instancetype)initWithPlugInCPath:(const char*)cPath NS_DESIGNATED_INITIALIZER;
- (instancetype)initWithPlugInPath:(NSString *)path;
- (instancetype)initWithPlugInURL:(NSURL *)URL;

- (PPLibraryObject *)pluginAtIndex:(NSUInteger)idx;
@property (readonly) NSUInteger pluginCount;

- (MADErr)identifyFileAtPath:(NSString*)apath type:(out char*)atype;
- (MADErr)identifyFileAtURL:(NSURL*)apath type:(out char*)atype;

- (MADErr)getInformationFromFileAtPath:(NSString*)apath type:(char*)atype info:(PPInfoRec*)infoRec;
- (MADErr)getInformationFromFileAtURL:(NSURL*)apath type:(char*)atype info:(PPInfoRec*)infoRec;

@end
