//
//  CppIncludeTest.m
//  PPMacho
//
//  Created by C.W. Betts on 1/12/15.
//
//

#import <Foundation/Foundation.h>
#import <XCTest/XCTest.h>
#if TARGET_OS_IPHONE
#import <UIKit/UIKit.h>
#endif
#import <PlayerPROCore/PlayerPROCore.h>
#import <PlayerPROKit/PlayerPROKit.h>
#import <PlayerPROKit/PlayerPROKit-Swift.h>
#import "tmpHeader.h"

void testDebugFunc(short line, const char* file, const char* info)
{
	NSString *formattedStr = [NSString stringWithFormat:@"%s:%d %s", file, line, info];
	NSLog(@"%@", formattedStr);
}

void cXTCFail(short line, const char* file, const char* info)
{
	_XCTFailureHandler(currentTestClass, NO, file, line, @"MADDebugStr was called!", @"%s", info);
}

void (*cDebugFunc)(short, const char*, const char*) = testDebugFunc;
void (*cXTCFailFunc)(short, const char*, const char*) = cXTCFail;

XCTestCase *currentTestClass = nil;

@interface CppIncludeTest : XCTestCase

@end

@implementation CppIncludeTest

- (void)setUp {
    [super setUp];
    // Put setup code here. This method is called before the invocation of each test method in the class.
	currentTestClass = self;
	MADRegisterDebugFunc(cXTCFail);
}

- (void)tearDown {
    // Put teardown code here. This method is called after the invocation of each test method in the class.
    [super tearDown];
}

- (void)testError
{
	NSError *err1 = PPCreateErrorFromMADErrorType(MADReadingErr);
	XCTAssertEqualObjects(err1.domain, PPMADErrorDomain);
	NSError *err2 = PPCreateErrorFromMADErrorTypeConvertingToCocoa(MADReadingErr, NO);
	XCTAssertEqualObjects(err2.domain, PPMADErrorDomain);

	NSError *err3 = PPCreateErrorFromMADErrorTypeConvertingToCocoa(MADReadingErr, YES);
	XCTAssertNotEqualObjects(err3.domain, PPMADErrorDomain);
}

@end
