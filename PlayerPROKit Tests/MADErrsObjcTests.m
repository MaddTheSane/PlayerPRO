//
//  MADErrsObjcTests.m
//  PPMacho
//
//  Created by C.W. Betts on 4/8/18.
//

#import <XCTest/XCTest.h>
#import <PlayerPROKit/PlayerPROKit.h>

#ifndef __MACERRORS__
#define paramErr (-50)
#define userCanceledErr (-128)
#endif

@interface MADErrsObjcTests : XCTestCase

@end

@implementation MADErrsObjcTests

- (void)setUp {
    [super setUp];
    // Put setup code here. This method is called before the invocation of each test method in the class.
}

- (void)tearDown {
    // Put teardown code here. This method is called after the invocation of each test method in the class.
    [super tearDown];
}

- (void)testNonMADErrError {
	NSError *err = PPCreateErrorFromMADErrorType(userCanceledErr);
	XCTAssertNotEqualObjects(err.domain, PPMADErrorDomain);
	XCTAssertEqualObjects(err.domain, NSOSStatusErrorDomain);
}

@end
