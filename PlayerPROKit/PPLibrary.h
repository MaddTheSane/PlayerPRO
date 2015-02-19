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
extern __nonnull NSString * const kPPTotalPatterns;
extern __nonnull NSString * const kPPPartitionLength;
extern __nonnull NSString * const kPPFileSize;
extern __nonnull NSString * const kPPSignature;
extern __nonnull NSString * const kPPTotalTracks;
extern __nonnull NSString * const kPPTotalInstruments;
extern __nonnull NSString * const kPPInternalFileName;
extern __nonnull NSString * const kPPFormatDescription;
__END_DECLS

@class PPLibraryObject;

@interface PPLibrary : NSObject <NSFastEnumeration>
- (instancetype)init;
- (instancetype)initWithPlugInPath:(__nonnull NSString *)path;
- (instancetype)initWithPlugInURL:(__nonnull NSURL *)URL;
+ (void)deregisterDebugFunction;
+ (void)registerDebugFunction:(void (* __nonnull)(short, const char* __null_unspecified, const char* __null_unspecified))newDebugFunc;
+ (void)registerDebugBlock:(void (^ __nonnull)(short, const char* __null_unspecified, const char* __null_unspecified))newDebugFunc;

- (__nonnull PPLibraryObject *)objectAtIndexedSubscript:(NSInteger)index;
@property (readonly) NSInteger pluginCount;

- (MADErr)identifyFileAtPath:(__nonnull NSString*)apath type:(__nonnull char*)atype;
- (MADErr)identifyFileAtPath:(__nonnull NSString*)apath stringType:(out __nonnull NSString* __autoreleasing  *__nullable)atype;
- (MADErr)identifyFileAtURL:(__nonnull NSURL*)apath type:(__nonnull char*)atype;
- (MADErr)identifyFileAtURL:(__nonnull NSURL*)apath stringType:(out __nonnull NSString* __autoreleasing *__nullable)atype;

- (MADErr)getInformationFromFileAtPath:(__nonnull NSString*)apath type:(in __nonnull const char*)atype info:(__nonnull MADInfoRec*)infoRec;
- (MADErr)getInformationFromFileAtURL:(__nonnull NSURL*)apath type:(in __nonnull const char*)atype info:(__nonnull MADInfoRec*)infoRec;

- (MADErr)getInformationFromFileAtURL:(__nonnull NSURL*)apath type:(in __nonnull const char*)atype infoDictionary:(out __nonnull NSDictionary* __autoreleasing *__nullable)infoDict;
- (MADErr)getInformationFromFileAtURL:(__nonnull NSURL*)apath stringType:(__nonnull NSString*)atype infoDictionary:(out __nonnull NSDictionary* __autoreleasing * __nullable)infoDict;
- (MADErr)getInformationFromFileAtPath:(__nonnull NSString*)apath type:(in __nonnull const char*)atype infoDictionary:(out __nonnull NSDictionary* __autoreleasing * __nullable)infoDict;
- (MADErr)getInformationFromFileAtPath:(__nonnull NSString*)apath stringType:(__nonnull NSString*)atype infoDictionary:(out __nonnull NSDictionary* __autoreleasing * __nullable)infoDict;

/**
 *	@abstract Gets a plug-in type from a UTI
 *	@param aUTI The UTI to find a plug-in type for.
 *	@return A plug-in type, four characters long, or \c nil if there's
 *	no plug-in that opens the UTI
 */
- (NSString* __nullable)typeFromUTI:(__nonnull NSString*)aUTI;

/**
 *	@abstract Gets the first UTI from a plug-in type.
 *	@param aType the four-character plug-in type to get a UTI for.
 *	@return a UTI, or \c nil if the type isn't listed
 */
- (NSString* __nullable)typeToUTI:(__nonnull NSString*)aType;
@end

__BEGIN_DECLS
extern __nonnull NSDictionary *PPInfoRecToDictionary(MADInfoRec infoRec);
__END_DECLS

