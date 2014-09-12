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

__BEGIN_DECLS
extern NSString * const kPPTotalPatterns;
extern NSString * const kPPPartitionLength;
extern NSString * const kPPFileSize;
extern NSString * const kPPSignature;
extern NSString * const kPPTotalTracks;
extern NSString * const kPPTotalInstruments;
extern NSString * const kPPInternalFileName;
extern NSString * const kPPFormatDescription;
__END_DECLS

@interface PPLibraryObject : NSObject
@property (readonly, copy) NSString *menuName;
@property (readonly, copy) NSString *authorString;
@property (readonly, strong) NSBundle *plugFile;
@property (readonly, strong) NSString *plugType;
@property (readonly, copy) NSArray* UTItypes;
@property (readonly) MADFourChar plugMode;
@property (readonly) BOOL canExport;
@property (readonly) BOOL canImport;
@property (readonly) UInt32 plugVersion;
@end

@interface PPLibrary : NSObject <NSFastEnumeration>
- (instancetype)init;
- (instancetype)initWithPlugInCPath:(const char*)cPath NS_DESIGNATED_INITIALIZER;
- (instancetype)initWithPlugInPath:(NSString *)path;
- (instancetype)initWithPlugInURL:(NSURL *)URL;
+ (void)deregisterDebugFunction;
+ (void)registerDebugFunction:(void (*)(short, const char*, const char*))newDebugFunc;
+ (void)registerDebugBlock:(void (^)(short, const char*, const char*))newDebugFunc;

- (PPLibraryObject *)pluginAtIndex:(NSUInteger)idx;
- (PPLibraryObject *)objectAtIndexedSubscript:(NSInteger)index;
@property (readonly) NSUInteger pluginCount;

- (MADErr)identifyFileAtPath:(NSString*)apath type:(out char*)atype;
- (MADErr)identifyFileAtPath:(NSString*)apath stringType:(out NSString* __autoreleasing *)atype;
- (MADErr)identifyFileAtURL:(NSURL*)apath type:(out char*)atype;
- (MADErr)identifyFileAtURL:(NSURL*)apath stringType:(out NSString* __autoreleasing *)atype;

- (MADErr)getInformationFromFileAtPath:(NSString*)apath type:(in const char*)atype info:(MADInfoRec*)infoRec;
- (MADErr)getInformationFromFileAtURL:(NSURL*)apath type:(in const char*)atype info:(MADInfoRec*)infoRec;

- (MADErr)getInformationFromFileAtURL:(NSURL*)apath type:(in const char*)atype infoDictionary:(out NSDictionary* __autoreleasing *)infoDict;
- (MADErr)getInformationFromFileAtURL:(NSURL*)apath stringType:(NSString*)atype infoDictionary:(out NSDictionary* __autoreleasing *)infoDict;
- (MADErr)getInformationFromFileAtPath:(NSString*)apath type:(in const char*)atype infoDictionary:(out NSDictionary* __autoreleasing *)infoDict;
- (MADErr)getInformationFromFileAtPath:(NSString*)apath stringType:(NSString*)atype infoDictionary:(out NSDictionary* __autoreleasing *)infoDict;

+ (NSDictionary*)infoRecToDictionary:(MADInfoRec)infoRec;

@end
