//
//  PPQuickView.h
//  PPMacho
//
//  Created by C.W. Betts on 2/14/14.
//
//

#ifndef PPMacho_PPQuickView_h
#define PPMacho_PPQuickView_h

#include <CoreFoundation/CoreFoundation.h>
#include <CoreServices/CoreServices.h>
#include <QuickLook/QuickLook.h>

#define __private_extern__ __attribute__((visibility("hidden")))

// The thumbnail generation function to be implemented in GenerateThumbnailForURL.c
__private_extern__ OSStatus GenerateThumbnailForURL(void *thisInterface, QLThumbnailRequestRef thumbnail,
													CFURLRef url, CFStringRef contentTypeUTI, CFDictionaryRef options,
													CGSize maxSize);
__private_extern__ void CancelThumbnailGeneration(void* thisInterface, QLThumbnailRequestRef thumbnail);

// The preview generation function to be implemented in GeneratePreviewForURL.c
__private_extern__ OSStatus GeneratePreviewForURL(void *thisInterface, QLPreviewRequestRef preview,
												  CFURLRef url, CFStringRef contentTypeUTI, CFDictionaryRef options);
__private_extern__ void CancelPreviewGeneration(void *thisInterface, QLPreviewRequestRef preview);


#endif
