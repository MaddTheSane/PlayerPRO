//
//  CppIncludeTest.m
//  PPMacho
//
//  Created by C.W. Betts on 1/12/15.
//
//

#import <Foundation/Foundation.h>
#import <XCTest/XCTest.h>
#import <PlayerPROCore/PlayerPROCore.h>
#import <PlayerPROKit/PlayerPROKit.h>
#import <PlayerPROKit/PlayerPROKit-Swift.h>

@interface CppIncludeTest : XCTestCase

@end

@implementation CppIncludeTest

- (void)setUp {
    [super setUp];
    // Put setup code here. This method is called before the invocation of each test method in the class.
}

- (void)tearDown {
    // Put teardown code here. This method is called after the invocation of each test method in the class.
    [super tearDown];
}

- (void)testError
{
	NSError *err1 = PPCreateErrorFromMADErrorType(MADReadingErr);
	XCTAssert([err1.domain isEqual:PPMADErrorDomain]);
	NSError *err2 = PPCreateErrorFromMADErrorTypeConvertingToCocoa(MADReadingErr, NO);
	XCTAssert([err2.domain isEqual:PPMADErrorDomain]);

	NSError *err3 = PPCreateErrorFromMADErrorTypeConvertingToCocoa(MADReadingErr, YES);
	XCTAssert(![err3.domain isEqual:PPMADErrorDomain]);
}

@end
