//
//  PreferenceHandler.m
//  PPMacho
//
//  Created by C.W. Betts on 1/19/14.
//
//

#import <Cocoa/Cocoa.h>
#import "NSColor+PPPreferences.h"
#import "UserDefaultKeys.m"
#include "PreferenceHandler.h"
#include "Shuddup.h"
#include "Utils.h"

#define makeNSRGB(red1, green1, blue1) [NSColor colorWithCalibratedRed:red1 / (CGFloat)USHRT_MAX green:green1 / (CGFloat)USHRT_MAX blue:blue1 / (CGFloat)USHRT_MAX alpha:1]

static void WriteCFPreferencesWithQDColor(NSString*, RGBColor valVal);
static void WriteCFPreferencesArrayWithQDColor(NSInteger valNum, RGBColor valVal);
static BOOL ReadCFPreferencesWithQDColor(NSString*, RGBColor *valVal);
static BOOL ReadCFPreferencesArrayWithQDColor(NSInteger valNum, RGBColor *valVal);

void MyPPBeep()
{
	NSBeep();
}

Boolean CFPreferencesHaveBeenSet()
{
	BOOL isSet = NO;
	NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];
	isSet = [[NSUserDefaults standardUserDefaults] boolForKey:PPPreferencesSet];
	[pool drain];
	
	return isSet;
}

void RegisterCFDefaults()
{
	NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];
	NSArray *pianoArray = [[NSArray alloc] initWithObjects:
						   [NSNumber numberWithShort:-4400],
						   [NSNumber numberWithShort:-1],
						   [NSNumber numberWithShort:-1],
						   [NSNumber numberWithShort:27400],
						   [NSNumber numberWithShort:8824],
						   [NSNumber numberWithShort:-1],
						   [NSNumber numberWithShort:9024],
						   [NSNumber numberWithShort:-6],
						   [NSNumber numberWithShort:12328],
						   [NSNumber numberWithShort:-1],
						   [NSNumber numberWithShort:2232],
						   [NSNumber numberWithShort:-1],
						   [NSNumber numberWithShort:886],
						   [NSNumber numberWithShort:2048],
						   [NSNumber numberWithShort:-1],
						   [NSNumber numberWithShort:26118],
						   [NSNumber numberWithShort:2296],
						   [NSNumber numberWithShort:-1],
						   [NSNumber numberWithShort:886],
						   [NSNumber numberWithShort:28672],
						   [NSNumber numberWithShort:24576],
						   [NSNumber numberWithShort:568],
						   [NSNumber numberWithShort:24832],
						   [NSNumber numberWithShort:-4446],
						   [NSNumber numberWithShort:-1],
						   [NSNumber numberWithShort:-1],
						   [NSNumber numberWithShort:28178],
						   [NSNumber numberWithShort:26374],
						   [NSNumber numberWithShort:24832],
						   [NSNumber numberWithShort:-1],
						   [NSNumber numberWithShort:-1],
						   [NSNumber numberWithShort:-1],
						   [NSNumber numberWithShort:-490],
						   [NSNumber numberWithShort:-1],
						   [NSNumber numberWithShort:-1],
						   [NSNumber numberWithShort:9631],
						   [NSNumber numberWithShort:-1],
						   [NSNumber numberWithShort:-1],
						   [NSNumber numberWithShort:-1],
						   [NSNumber numberWithShort:-1],
						   [NSNumber numberWithShort:-1],
						   [NSNumber numberWithShort:-1],
						   [NSNumber numberWithShort:-1],
						   [NSNumber numberWithShort:21314],
						   [NSNumber numberWithShort:9281],
						   [NSNumber numberWithShort:-1],
						   [NSNumber numberWithShort:8722],
						   [NSNumber numberWithShort:24818],
						   [NSNumber numberWithShort:33],
						   [NSNumber numberWithShort:24],
						   [NSNumber numberWithShort:25],
						   [NSNumber numberWithShort:26],
						   [NSNumber numberWithShort:27],
						   [NSNumber numberWithShort:28],
						   [NSNumber numberWithShort:29],
						   [NSNumber numberWithShort:30],
						   [NSNumber numberWithShort:31],
						   [NSNumber numberWithShort:32],
						   [NSNumber numberWithShort:-1],
						   [NSNumber numberWithShort:-1],
						   [NSNumber numberWithShort:-1],
						   [NSNumber numberWithShort:-1],
						   [NSNumber numberWithShort:-1],
						   [NSNumber numberWithShort:-1],
						   [NSNumber numberWithShort:-1],
						   [NSNumber numberWithShort:70],
						   [NSNumber numberWithShort:83],
						   [NSNumber numberWithShort:81],
						   [NSNumber numberWithShort:72],
						   [NSNumber numberWithShort:62],
						   [NSNumber numberWithShort:73],
						   [NSNumber numberWithShort:74],
						   [NSNumber numberWithShort:75],
						   [NSNumber numberWithShort:67],
						   [NSNumber numberWithShort:76],
						   [NSNumber numberWithShort:77],
						   [NSNumber numberWithShort:78],
						   [NSNumber numberWithShort:85],
						   [NSNumber numberWithShort:84],
						   [NSNumber numberWithShort:68],
						   [NSNumber numberWithShort:69],
						   [NSNumber numberWithShort:60],
						   [NSNumber numberWithShort:63],
						   [NSNumber numberWithShort:71],
						   [NSNumber numberWithShort:64],
						   [NSNumber numberWithShort:66],
						   [NSNumber numberWithShort:82],
						   [NSNumber numberWithShort:61],
						   [NSNumber numberWithShort:80],
						   [NSNumber numberWithShort:79],
						   [NSNumber numberWithShort:65],
						   [NSNumber numberWithShort:26196],
						   [NSNumber numberWithShort:12840],
						   [NSNumber numberWithShort:-1],
						   [NSNumber numberWithShort:28238],
						   [NSNumber numberWithShort:27908],
						   [NSNumber numberWithShort:29186],
						   [NSNumber numberWithShort:44],
						   [NSNumber numberWithShort:57],
						   [NSNumber numberWithShort:55],
						   [NSNumber numberWithShort:46],
						   [NSNumber numberWithShort:36],
						   [NSNumber numberWithShort:47],
						   [NSNumber numberWithShort:48],
						   [NSNumber numberWithShort:49],
						   [NSNumber numberWithShort:41],
						   [NSNumber numberWithShort:50],
						   [NSNumber numberWithShort:51],
						   [NSNumber numberWithShort:52],
						   [NSNumber numberWithShort:59],
						   [NSNumber numberWithShort:58],
						   [NSNumber numberWithShort:42],
						   [NSNumber numberWithShort:43],
						   [NSNumber numberWithShort:34],
						   [NSNumber numberWithShort:37],
						   [NSNumber numberWithShort:45],
						   [NSNumber numberWithShort:38],
						   [NSNumber numberWithShort:40],
						   [NSNumber numberWithShort:56],
						   [NSNumber numberWithShort:35],
						   [NSNumber numberWithShort:54],
						   [NSNumber numberWithShort:53],
						   [NSNumber numberWithShort:39],
						   [NSNumber numberWithShort:10724],
						   [NSNumber numberWithShort:26126],
						   [NSNumber numberWithShort:12329],
						   [NSNumber numberWithShort:-1],
						   [NSNumber numberWithShort:19678],
						   [NSNumber numberWithShort:-1],
						   [NSNumber numberWithShort:12608],
						   [NSNumber numberWithShort:-1],
						   [NSNumber numberWithShort:24580],
						   [NSNumber numberWithShort:19678],
						   [NSNumber numberWithShort:768],
						   [NSNumber numberWithShort:19011],
						   [NSNumber numberWithShort:26118],
						   [NSNumber numberWithShort:19048],
						   [NSNumber numberWithShort:-1],
						   [NSNumber numberWithShort:27910],
						   [NSNumber numberWithShort:12650],
						   [NSNumber numberWithShort:-1],
						   [NSNumber numberWithShort:-1],
						   [NSNumber numberWithShort:24832],
						   [NSNumber numberWithShort:406],
						   [NSNumber numberWithShort:24832],
						   [NSNumber numberWithShort:5632],
						   [NSNumber numberWithShort:26128],
						   [NSNumber numberWithShort:12330],
						   [NSNumber numberWithShort:-1],
						   [NSNumber numberWithShort:-28312],
						   [NSNumber numberWithShort:-1],
						   [NSNumber numberWithShort:12330],
						   [NSNumber numberWithShort:162],
						   [NSNumber numberWithShort:-28312],
						   [NSNumber numberWithShort:-1],
						   [NSNumber numberWithShort:2088],
						   [NSNumber numberWithShort:-1],
						   [NSNumber numberWithShort:-1],
						   [NSNumber numberWithShort:26138],
						   [NSNumber numberWithShort:12348],
						   [NSNumber numberWithShort:31744],
						   [NSNumber numberWithShort:-20376],
						   [NSNumber numberWithShort:-1],
						   [NSNumber numberWithShort:25102],
						   [NSNumber numberWithShort:12608],
						   [NSNumber numberWithShort:-1],
						   [NSNumber numberWithShort:-20376],
						   [NSNumber numberWithShort:-1],
						   [NSNumber numberWithShort:25092],
						   [NSNumber numberWithShort:12608],
						   [NSNumber numberWithShort:-1],
						   [NSNumber numberWithShort:24678],
						   [NSNumber numberWithShort:12650],
						   [NSNumber numberWithShort:-1],
						   [NSNumber numberWithShort:-1],
						   [NSNumber numberWithShort:12650],
						   [NSNumber numberWithShort:-1],
						   [NSNumber numberWithShort:-1],
						   [NSNumber numberWithShort:12650],
						   [NSNumber numberWithShort:-1],
						   [NSNumber numberWithShort:-1],
						   [NSNumber numberWithShort:12650],
						   [NSNumber numberWithShort:-1],
						   [NSNumber numberWithShort:-1],
						   [NSNumber numberWithShort:12650],
						   [NSNumber numberWithShort:-1],
						   [NSNumber numberWithShort:-1],
						   [NSNumber numberWithShort:17064],
						   [NSNumber numberWithShort:-1],
						   [NSNumber numberWithShort:12650],
						   [NSNumber numberWithShort:-1],
						   [NSNumber numberWithShort:-1],
						   [NSNumber numberWithShort:17064],
						   [NSNumber numberWithShort:-1],
						   [NSNumber numberWithShort:12668],
						   [NSNumber numberWithShort:-1],
						   [NSNumber numberWithShort:-1],
						   [NSNumber numberWithShort:24832],
						   [NSNumber numberWithShort:5526],
						   [NSNumber numberWithShort:26174],
						   [NSNumber numberWithShort:8554],
						   [NSNumber numberWithShort:-1],
						   [NSNumber numberWithShort:-1],
						   [NSNumber numberWithShort:12650],
						   [NSNumber numberWithShort:98],
						   [NSNumber numberWithShort:-1],
						   [NSNumber numberWithShort:8554],
						   [NSNumber numberWithShort:100],
						   [NSNumber numberWithShort:-1],
						   [NSNumber numberWithShort:8554],
						   [NSNumber numberWithShort:114],
						   [NSNumber numberWithShort:-1],
						   [NSNumber numberWithShort:8554],
						   [NSNumber numberWithShort:118],
						   [NSNumber numberWithShort:-1],
						   [NSNumber numberWithShort:12040],
						   [NSNumber numberWithShort:17384],
						   [NSNumber numberWithShort:-1],
						   [NSNumber numberWithShort:16874],
						   [NSNumber numberWithShort:122],
						   [NSNumber numberWithShort:28704],
						   [NSNumber numberWithShort:-24530],
						   [NSNumber numberWithShort:8287],
						   [NSNumber numberWithShort:8650],
						   [NSNumber numberWithShort:1006],
						   [NSNumber numberWithShort:24832],
						   [NSNumber numberWithShort:5470],
						   [NSNumber numberWithShort:26118],
						   [NSNumber numberWithShort:24832],
						   [NSNumber numberWithShort:-920],
						   [NSNumber numberWithShort:26114],
						   [NSNumber numberWithShort:28672],
						   [NSNumber numberWithShort:24576],
						   [NSNumber numberWithShort:-4686],
						   [NSNumber numberWithShort:24832],
						   [NSNumber numberWithShort:-4870],
						   [NSNumber numberWithShort:24832],
						   [NSNumber numberWithShort:-900],
						   [NSNumber numberWithShort:26214],
						   [NSNumber numberWithShort:24832],
						   [NSNumber numberWithShort:156],
						   [NSNumber numberWithShort:26208],
						   [NSNumber numberWithShort:24832],
						   [NSNumber numberWithShort:4394],
						   [NSNumber numberWithShort:26202],
						   [NSNumber numberWithShort:9576],
						   [NSNumber numberWithShort:-1],
						   [NSNumber numberWithShort:-1],
						   [NSNumber numberWithShort:9576],
						   [NSNumber numberWithShort:-1],
						   [NSNumber numberWithShort:-1],
						   [NSNumber numberWithShort:12328],
						   [NSNumber numberWithShort:-1],
						   [NSNumber numberWithShort:576],
						   [NSNumber numberWithShort:-32768],
						   [NSNumber numberWithShort:12842],
						   [NSNumber numberWithShort:-1],
						   [NSNumber numberWithShort:577],
						   [NSNumber numberWithShort:32767],
						   [NSNumber numberWithShort:-32192],
						   [NSNumber numberWithShort:13633],
						   [NSNumber numberWithShort:-1],
						   [NSNumber numberWithShort:24832],
						   [NSNumber numberWithShort:5396],
						   [NSNumber numberWithShort:26146],
						   [NSNumber numberWithShort:9576],
						   [NSNumber numberWithShort:-1],
						   [NSNumber numberWithShort:-1],
						   [NSNumber numberWithShort:9576],
						   [NSNumber numberWithShort:-1],
						   [NSNumber numberWithShort:-1],
						   [NSNumber numberWithShort:13672],
						   [NSNumber numberWithShort:-1],
						   [NSNumber numberWithShort:98],
						   [NSNumber numberWithShort:12040],
						   [NSNumber numberWithShort:16872],
						   [NSNumber numberWithShort:-1],
						   [NSNumber numberWithShort:17386],
						   [NSNumber numberWithShort:122],
						   [NSNumber numberWithShort:28704],
						   [NSNumber numberWithShort:-24530],
						   [NSNumber numberWithShort:8287],
						   [NSNumber numberWithShort:8232],
						   [NSNumber numberWithShort:-1],
						   [NSNumber numberWithShort:26372],
						   [NSNumber numberWithShort:257],
						   [NSNumber numberWithShort:257],
						   [NSNumber numberWithShort:-1],
						   [NSNumber numberWithShort:-1],
						   [NSNumber numberWithShort:259],
						   [NSNumber numberWithShort:-1],
						   [NSNumber numberWithShort:-1],
						   [NSNumber numberWithShort:-1],
						   [NSNumber numberWithShort:219],
						   [NSNumber numberWithShort:196],
						   [NSNumber numberWithShort:-1],
						   [NSNumber numberWithShort:-1],
						   [NSNumber numberWithShort:148],
						   [NSNumber numberWithShort:-1],
						   [NSNumber numberWithShort:102],
						   nil];
	
	NSMutableArray *filters = [NSMutableArray arrayWithCapacity:EQPACKET * 2];
	for (int i = 0; i < EQPACKET*2; i++) {
		[filters addObject:[NSNumber numberWithDouble:1.0]];
	}
	
	static NSArray *theColors = nil;
	if (!theColors) {
		theColors = [NSArray arrayWithObjects:
					 [makeNSRGB(0xEEEE, 0x0000, 0x0000) PPencodeColor],
					 [makeNSRGB(0x8C8C, 0xBCBC, 0x1C1C) PPencodeColor],
					 [makeNSRGB(0x3333, 0xFFFF, 0xFFFF) PPencodeColor],
					 [[NSColor yellowColor] PPencodeColor],
					 [makeNSRGB(0x6060, 0xCACA, 0x8E8E) PPencodeColor],
					 [makeNSRGB(0x3333, 0xFFFF, 0xFFFF) PPencodeColor],
					 [makeNSRGB(0x6666, 0xCCCC, 0xFFFF) PPencodeColor],
					 [makeNSRGB(0xFFFF, 0x6354, 0xFFFF) PPencodeColor],
					 [makeNSRGB(0xCCCC, 0x9999, 0x6666) PPencodeColor],
					 [makeNSRGB(0x3333, 0x6666, 0x3333) PPencodeColor],
					 [makeNSRGB(0xFFFF, 0x6666, 0xFFFF) PPencodeColor],
					 [makeNSRGB(0xFFFF, 0x9999, 0x0000) PPencodeColor],
					 [makeNSRGB(0xCCCC, 0xFFFF, 0xCCCC) PPencodeColor],
					 [makeNSRGB(0x9999, 0x0000, 0x6666) PPencodeColor],
					 [makeNSRGB(0x6666, 0xCCCC, 0x0000) PPencodeColor],
					 [makeNSRGB(0xFFFF, 0x9999, 0xFFFF) PPencodeColor],
					 [makeNSRGB(0xCCCC, 0xFFFF, 0x6666) PPencodeColor],
					 [makeNSRGB(0x6666, 0x0000, 0x9999) PPencodeColor],
					 [makeNSRGB(0xCCCC, 0x0000, 0x9999) PPencodeColor],
					 [makeNSRGB(0xCCCC, 0x9999, 0xFFFF) PPencodeColor],
					 [makeNSRGB(0x6666, 0xCCCC, 0x6666) PPencodeColor],
					 [makeNSRGB(0xCCCC, 0xCCCC, 0x3333) PPencodeColor],
					 [makeNSRGB(0x9999, 0xCCCC, 0xCCCC) PPencodeColor],
					 [makeNSRGB(0x6666, 0x3333, 0x6666) PPencodeColor],
					 [makeNSRGB(0xCCCC, 0x9999, 0xCCCC) PPencodeColor],
					 [makeNSRGB(0x3333, 0xFFFF, 0xFFFF) PPencodeColor],
					 [makeNSRGB(0x3333, 0xFFFF, 0x6666) PPencodeColor],
					 [makeNSRGB(0x6666, 0xFFFF, 0x3333) PPencodeColor],
					 [makeNSRGB(0xCCCC, 0x9999, 0xCCCC) PPencodeColor],
					 [makeNSRGB(0xCCCC, 0x9999, 0x3333) PPencodeColor],
					 [makeNSRGB(0xCCCC, 0xCCCC, 0x3333) PPencodeColor],
					 [makeNSRGB(0xFFFF, 0xCCCC, 0x0000) PPencodeColor],
					 [makeNSRGB(0xEEEE, 0x0000, 0x0000) PPencodeColor],
					 [makeNSRGB(0x0000, 0xFFFF, 0x6666) PPencodeColor],
					 [[NSColor cyanColor] PPencodeColor], 
					 [[NSColor yellowColor] PPencodeColor],
					 [[NSColor greenColor] PPencodeColor],
					 [makeNSRGB(0x3333, 0xCCCC, 0xFFFF) PPencodeColor],
					 [makeNSRGB(0x9999, 0xCCCC, 0x9999) PPencodeColor],
					 [makeNSRGB(0xFFFF, 0x9999, 0x6666) PPencodeColor],
					 [makeNSRGB(0x9999, 0xFFFF, 0xFFFF) PPencodeColor],
					 [makeNSRGB(0x6666, 0xFFFF, 0xFFFF) PPencodeColor],
					 [makeNSRGB(0xFFFF, 0x9999, 0xFFFF) PPencodeColor],
					 [makeNSRGB(0x0000, 0x5555, 0x0000) PPencodeColor],
					 [makeNSRGB(0xCCCC, 0xFFFF, 0xCCCC) PPencodeColor],
					 [makeNSRGB(0x6666, 0x0000, 0x0000) PPencodeColor],
					 [makeNSRGB(0x3333, 0xFFFF, 0x6666) PPencodeColor],
					 [makeNSRGB(0xFFFF, 0xFFFF, 0x6666) PPencodeColor],
					 [makeNSRGB(0xCCCC, 0x0000, 0x0000) PPencodeColor],
					 [makeNSRGB(0x9999, 0x0000, 0x3333) PPencodeColor],
					 [makeNSRGB(0x9999, 0xFFFF, 0x9999) PPencodeColor],
					 [makeNSRGB(0x9999, 0xCCCC, 0xCCCC) PPencodeColor],
					 [makeNSRGB(0xCCCC, 0xCCCC, 0x0000) PPencodeColor],
					 [makeNSRGB(0xCCCC, 0xCCCC, 0x3333) PPencodeColor],
					 [makeNSRGB(0xFFFF, 0x9999, 0xFFFF) PPencodeColor],
					 [[NSColor redColor] PPencodeColor],
					 [makeNSRGB(0xFFFF, 0x0000, 0x6666) PPencodeColor],
					 [makeNSRGB(0x6666, 0xFFFF, 0xFFFF) PPencodeColor],
					 [makeNSRGB(0x6666, 0xFFFF, 0xCCCC) PPencodeColor],
					 [makeNSRGB(0x9999, 0xCCCC, 0x6666) PPencodeColor],
					 [makeNSRGB(0x9999, 0xCCCC, 0x9999) PPencodeColor],
					 [makeNSRGB(0x9999, 0xCCCC, 0x6666) PPencodeColor],
					 [makeNSRGB(0xCCCC, 0xCCCC, 0x6666) PPencodeColor],
					 [makeNSRGB(0xCCCC, 0xCCCC, 0x9999) PPencodeColor],
					 [makeNSRGB(0xEEEE, 0x0000, 0x0000) PPencodeColor],
					 [makeNSRGB(0x0000, 0xFFFF, 0x6666) PPencodeColor],
					 [makeNSRGB(0x0000, 0xFFFF, 0xFFFF) PPencodeColor],
					 [makeNSRGB(0xFFFF, 0xFFFF, 0x0000) PPencodeColor],
					 [[NSColor greenColor] PPencodeColor],
					 [makeNSRGB(0x3333, 0xCCCC, 0xFFFF) PPencodeColor],
					 [makeNSRGB(0x9999, 0xCCCC, 0x9999) PPencodeColor],
					 [makeNSRGB(0xFFFF, 0x9999, 0x6666) PPencodeColor],
					 [makeNSRGB(0x9999, 0xFFFF, 0xFFFF) PPencodeColor],
					 [makeNSRGB(0x6666, 0xFFFF, 0xFFFF) PPencodeColor],
					 [makeNSRGB(0xFFFF, 0x9999, 0xFFFF) PPencodeColor],
					 [makeNSRGB(0x0000, 0x5555, 0x0000) PPencodeColor],
					 [makeNSRGB(0xCCCC, 0xFFFF, 0xCCCC) PPencodeColor],
					 [makeNSRGB(0x6666, 0x0000, 0x0000) PPencodeColor],
					 [makeNSRGB(0x3333, 0xFFFF, 0x6666) PPencodeColor],
					 [makeNSRGB(0xFFFF, 0xFFFF, 0x6666) PPencodeColor], 
					 [makeNSRGB(0xCCCC, 0x0000, 0x0000) PPencodeColor],
					 [makeNSRGB(0x9999, 0x0000, 0x3333) PPencodeColor],
					 [makeNSRGB(0x9999, 0xFFFF, 0x9999) PPencodeColor],
					 [makeNSRGB(0x9999, 0xCCCC, 0xCCCC) PPencodeColor],
					 [makeNSRGB(0xCCCC, 0xCCCC, 0x0000) PPencodeColor],
					 [makeNSRGB(0xCCCC, 0xCCCC, 0x3333) PPencodeColor], 
					 [makeNSRGB(0xFFFF, 0x9999, 0xFFFF) PPencodeColor], 
					 [[NSColor redColor] PPencodeColor], 
					 [makeNSRGB(0x9999, 0x0000, 0x6666) PPencodeColor],
					 [makeNSRGB(0x6666, 0xFFFF, 0xFFFF) PPencodeColor], 
					 [makeNSRGB(0x6666, 0xFFFF, 0xCCCC) PPencodeColor], 
					 [makeNSRGB(0x9999, 0xCCCC, 0x6666) PPencodeColor], 
					 [makeNSRGB(0x9999, 0xCCCC, 0x9999) PPencodeColor], 
					 [makeNSRGB(0x9999, 0xCCCC, 0x6666) PPencodeColor], 
					 [makeNSRGB(0xCCCC, 0xCCCC, 0x6666) PPencodeColor], 
					 [makeNSRGB(0xCCCC, 0xCCCC, 0x9999) PPencodeColor], nil];
		[theColors retain];
	}
	
	[[NSUserDefaults standardUserDefaults]
	 registerDefaults:
	 [NSDictionary dictionaryWithObjectsAndKeys:
	  [NSNumber numberWithBool:YES], PPPreferencesSet,
	  [NSNumber numberWithBool:YES], PPMAddExtension,
	  [NSNumber numberWithBool:YES], PPMMadCompression,
	  [NSNumber numberWithBool:YES], PPAutoCreator,
	  @"State 1", PPWindowName1,
	  @"State 2", PPWindowName2,
	  @"State 3", PPWindowName3,
	  [NSNumber numberWithInt:0], PPStartNumber,
	  pianoArray, PPPianoKeys,
	  [NSNumber numberWithShort:1], PPLoopType,
	  [NSNumber numberWithShort:27], PPVolumeLevel,
	  [NSNumber numberWithFloat:FixedToFloat((unsigned)rate44khz)], PPSoundOutRate,
	  [NSNumber numberWithShort:SoundManagerDriver], PPSoundDriver,
	  [NSNumber numberWithShort:16], PPSoundOutBits,
	  [NSNumber numberWithBool:NO], PPSurroundToggle,
	  [NSNumber numberWithBool:NO], PPReverbToggle,
	  [NSNumber numberWithLong:25], PPReverbAmount,
	  [NSNumber numberWithLong:60], PPReverbStrength,
	  [NSNumber numberWithLong:1], PPOversamplingAmount,
	  [NSNumber numberWithInt:0], PPStereoDelayAmount,
	  [NSNumber numberWithBool:NO], PPStereoDelayToggle,
	  [NSNumber numberWithBool:NO], PPOversamplingToggle,
	  [NSNumber numberWithInt:1], PPOversamplingAmount,
	  [NSNumber numberWithBool:NO], PPCEShowNotesLen,
	  [NSNumber numberWithBool:NO], PPCEShowMarkers,
	  [NSNumber numberWithShort:130], PPCETrackHeight,
	  [NSNumber numberWithShort:4], PPCETempoNum,
	  [NSNumber numberWithShort:4], PPCETempoUnit,
	  
	  [makeNSRGB(0xFFFF, 0xFFFF, 0x9999) PPencodeColor], PPDEMarkerColorPref,
	  [NSNumber numberWithBool:YES], PPDEShowArgument,
	  [NSNumber numberWithBool:YES], PPDEShowEffect,
	  [NSNumber numberWithBool:YES], PPDEShowInstrument,
	  [NSNumber numberWithBool:YES], PPDEShowMarkers,
	  [NSNumber numberWithBool:YES], PPDEShowNote,
	  [NSNumber numberWithBool:YES], PPDEShowVolume,
	  [NSNumber numberWithBool:NO], PPDEPatternWrappingPartition,
	  
	  theColors, PPCColorArray,
	  
	  [NSNumber numberWithBool:NO], PPMIDISendClock,
	  
	  [NSNumber numberWithBool:NO], PPUseEQ,
	  [NSNumber numberWithBool:NO], PPDontUseFileMixer,
	  filters, PPFilterArray,
	  [NSNumber numberWithBool:NO], PPAutomaticOpen,
	  [NSNumber numberWithInt:1], PPRecordAllTrack,
	  nil]];
	
	[pianoArray release];
	[pool drain];
}

void ResetCFPreferences()
{
	NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];
	NSString *appDomain = [[NSBundle mainBundle] bundleIdentifier];
	[[NSUserDefaults standardUserDefaults] removePersistentDomainForName:appDomain];
	[pool drain];
}

void ReadCFPreferences()
{
	NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];
	NSUserDefaults *defaults = [NSUserDefaults standardUserDefaults];
	NSString *tempStr = nil;
	NSArray *tempArray = nil;
	NSUInteger i;
	memset(&thePrefs, 0, sizeof(Prefs));
	thePrefs.addExtension = [defaults boolForKey:PPMAddExtension];
	thePrefs.MADCompression = [defaults boolForKey:PPMMadCompression];
	thePrefs.OscilloLine = [defaults boolForKey:PPMOscilloscopeDrawLines];
#define PPCOLOR(val) ReadCFPreferencesArrayWithQDColor(val, &thePrefs.tracksColor[val - 1])
	PPCOLORPOPULATE();
#undef PPCOLOR
#if defined(powerc) || defined (__powerc) || defined(__ppc__)
	thePrefs.PPCMachine = true;
#else
	thePrefs.PPCMachine = false;
#endif
	tempStr = [defaults stringForKey:PPWindowName1];
	CFStringGetPascalString((CFStringRef)tempStr, thePrefs.WinNames[0], sizeof(thePrefs.WinNames[0]), kCFStringEncodingMacRoman);
	tempStr = [defaults stringForKey:PPWindowName2];
	CFStringGetPascalString((CFStringRef)tempStr, thePrefs.WinNames[1], sizeof(thePrefs.WinNames[0]), kCFStringEncodingMacRoman);
	tempStr = [defaults stringForKey:PPWindowName3];
	CFStringGetPascalString((CFStringRef)tempStr, thePrefs.WinNames[2], sizeof(thePrefs.WinNames[0]), kCFStringEncodingMacRoman);
	tempStr = nil;
	thePrefs.NoStart = [defaults integerForKey:PPStartNumber];
	tempArray = [defaults arrayForKey:PPPianoKeys];
	for (i = 0; i < 300; i++) {
		thePrefs.PianoKey[i] = [[tempArray objectAtIndex:i] shortValue];
	}
	thePrefs.LoopType = [defaults integerForKey:PPLoopType];
	thePrefs.volumeLevel = [defaults integerForKey:PPVolumeLevel];
	thePrefs.Compressor = 'NONE';
	thePrefs.outPutMode = thePrefs.DirectDriverType.outPutMode = DeluxeStereoOutPut;	// force DeluxeStereoOutPut
	thePrefs.channelNumber = 2;
	thePrefs.DirectDriverType.numChn = 4;
	thePrefs.useEQ = [defaults boolForKey:PPUseEQ];
	thePrefs.DirectDriverType.TickRemover = thePrefs.TickRemover = true;
	
	thePrefs.Reverb = thePrefs.DirectDriverType.Reverb = [defaults boolForKey:PPReverbToggle];
	thePrefs.ReverbSize = thePrefs.DirectDriverType.ReverbSize = [defaults integerForKey:PPReverbAmount];
	thePrefs.ReverbStrength = thePrefs.DirectDriverType.ReverbStrength = [defaults integerForKey:PPReverbStrength];
	thePrefs.outPutRate = thePrefs.FrequenceSpeed = thePrefs.DirectDriverType.outPutRate = FloatToFixed([defaults floatForKey:PPSoundOutRate]);
	thePrefs.outPutBits = thePrefs.amplitude = thePrefs.DirectDriverType.outPutBits = [defaults integerForKey:PPSoundOutBits];
	thePrefs.driverMode = thePrefs.DirectDriverType.driverMode = [defaults integerForKey:PPSoundDriver];
	thePrefs.surround = thePrefs.DirectDriverType.surround = [defaults boolForKey:PPSurroundToggle];
	if ([defaults boolForKey:PPStereoDelayToggle]) {
		thePrefs.MicroDelaySize = thePrefs.DirectDriverType.MicroDelaySize = [defaults integerForKey:PPStereoDelayAmount];
	} else {
		thePrefs.MicroDelaySize = thePrefs.DirectDriverType.MicroDelaySize = 0;
	}
	
	if ([defaults boolForKey:PPOversamplingToggle]) {
		thePrefs.oversampling = thePrefs.DirectDriverType.oversampling = [defaults integerForKey:PPOversamplingAmount];
	} else {
		thePrefs.oversampling = thePrefs.DirectDriverType.oversampling = 1;
	}
	
	ReadCFPreferencesWithQDColor(PPDEMarkerColorPref, &thePrefs.yellC);
	thePrefs.StaffShowAllNotes = [defaults boolForKey:PPCEShowMarkers];
	thePrefs.StaffShowLength = [defaults boolForKey:PPCEShowNotesLen];
	thePrefs.TempsNum = [defaults integerForKey:PPCETempoNum];
	thePrefs.TempsUnit = [defaults integerForKey:PPCETempoUnit];
	thePrefs.TrackHeight = [defaults integerForKey:PPCETrackHeight];
	
	thePrefs.DigitalArgu = [defaults boolForKey:PPDEShowArgument];
	thePrefs.DigitalEffect = [defaults boolForKey:PPDEShowEffect];
	thePrefs.DigitalInstru = [defaults boolForKey:PPDEShowInstrument];
	thePrefs.DigitalNote = [defaults boolForKey:PPDEShowNote];
	thePrefs.DigitalVol = [defaults boolForKey:PPDEShowVolume];
	thePrefs.patternWrapping = [defaults boolForKey:PPDEPatternWrappingPartition];
	
	thePrefs.SendMIDIClockData = [defaults boolForKey:PPMIDISendClock];
	
	thePrefs.DontUseFilesMix = [defaults boolForKey:PPDontUseFileMixer];
	thePrefs.AutomaticOpen = [defaults boolForKey:PPAutomaticOpen];
	thePrefs.RecordAllTrack = [defaults integerForKey:PPRecordAllTrack];
	
	NSArray *filterArray = [defaults objectForKey:PPFilterArray];
	NSUInteger count = [filterArray count];
	for (i = 0; i < count; i++) {
		NSNumber * obj = [filterArray objectAtIndex:i];
		thePrefs.Filter[i] = obj.doubleValue;
	}
	[pool drain];
}

void WriteCFPreferences()
{
	NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];
	NSUserDefaults *defaults = [NSUserDefaults standardUserDefaults];
	NSMutableArray *tmpMutable = nil;
	int i;
	NSAutoreleasePool *innerPool = [[NSAutoreleasePool alloc] init];
	[defaults setBool:thePrefs.addExtension forKey:PPMAddExtension];
	[defaults setBool:thePrefs.MADCompression forKey:PPMMadCompression];
#define PPCOLOR(val) WriteCFPreferencesArrayWithQDColor(val, thePrefs.tracksColor[val - 1])
	PPCOLORPOPULATE();
#undef PPCOLOR
	[defaults setObject:[(NSString*)CFStringCreateWithPascalString(kCFAllocatorDefault, thePrefs.WinNames[0], kCFStringEncodingMacRoman) autorelease] forKey:PPWindowName1];
	[defaults setObject:[(NSString*)CFStringCreateWithPascalString(kCFAllocatorDefault, thePrefs.WinNames[1], kCFStringEncodingMacRoman) autorelease] forKey:PPWindowName2];
	[defaults setObject:[(NSString*)CFStringCreateWithPascalString(kCFAllocatorDefault, thePrefs.WinNames[2], kCFStringEncodingMacRoman) autorelease] forKey:PPWindowName3];
	[defaults setInteger:thePrefs.NoStart forKey:PPStartNumber];
	tmpMutable = [[NSMutableArray alloc] initWithCapacity:300];
	for (i = 0; i < 300; i++) {
		[tmpMutable addObject:[NSNumber numberWithShort:thePrefs.PianoKey[i]]];
	}
	[defaults setObject:tmpMutable forKey:PPPianoKeys];
	[tmpMutable release]; tmpMutable = nil;
	[innerPool drain]; innerPool = nil;
	
	[defaults setInteger:thePrefs.LoopType forKey:PPLoopType];
	[defaults setInteger:thePrefs.volumeLevel forKey:PPVolumeLevel];
	[defaults setInteger:thePrefs.ReverbStrength forKey:PPReverbStrength];
	[defaults setInteger:thePrefs.ReverbSize forKey:PPReverbAmount];
	[defaults setBool:thePrefs.Reverb forKey:PPReverbToggle];
	[defaults setFloat: FixedToFloat(thePrefs.outPutRate) forKey:PPSoundOutRate];
	[defaults setInteger:thePrefs.outPutBits forKey:PPSoundOutBits];
	[defaults setInteger:thePrefs.driverMode forKey:PPSoundDriver];
	[defaults setBool:thePrefs.surround forKey:PPSurroundToggle];
	if (thePrefs.MicroDelaySize > 0) {
		[defaults setBool:YES forKey:PPStereoDelayToggle];
	} else {
		[defaults setBool:NO forKey:PPStereoDelayToggle];
	}
	[defaults setInteger:thePrefs.MicroDelaySize forKey:PPStereoDelayAmount];

	if (thePrefs.oversampling > 1) {
		[defaults setBool:YES forKey:PPOversamplingToggle];
	} else {
		[defaults setBool:NO forKey:PPOversamplingToggle];
	}
	[defaults setInteger:thePrefs.oversampling forKey:PPOversamplingAmount];
	
	WriteCFPreferencesWithQDColor(PPDEMarkerColorPref, thePrefs.yellC);
	[defaults setBool:thePrefs.StaffShowAllNotes forKey:PPCEShowMarkers];
	[defaults setBool:thePrefs.StaffShowLength forKey:PPCEShowNotesLen];
	[defaults setInteger:thePrefs.TempsNum forKey:PPCETempoNum];
	[defaults setInteger:thePrefs.TempsUnit forKey:PPCETempoUnit];
	[defaults setInteger:thePrefs.TrackHeight forKey:PPCETrackHeight];
	
	[defaults setBool:thePrefs.DigitalArgu forKey:PPDEShowArgument];
	[defaults setBool:thePrefs.DigitalEffect forKey:PPDEShowEffect];
	[defaults setBool:thePrefs.DigitalInstru forKey:PPDEShowInstrument];
	[defaults setBool:thePrefs.DigitalNote forKey:PPDEShowNote];
	[defaults setBool:thePrefs.DigitalVol forKey:PPDEShowVolume];
	[defaults setBool:thePrefs.patternWrapping forKey:PPDEPatternWrappingPartition];
	
	[defaults setBool:thePrefs.SendMIDIClockData forKey:PPMIDISendClock];

	tmpMutable = [[NSMutableArray alloc] initWithCapacity:EQPACKET * 2];
	for (int i = 0; i < EQPACKET * 2; i++) {
		[tmpMutable addObject:[NSNumber numberWithDouble:thePrefs.Filter[i]]];
	}
	[defaults setObject:tmpMutable forKey:PPFilterArray];
	[tmpMutable release]; tmpMutable = nil;

	[defaults setBool:thePrefs.useEQ forKey:PPUseEQ];
	[defaults setBool:thePrefs.DontUseFilesMix forKey:PPDontUseFileMixer];
	[defaults setBool:thePrefs.AutomaticOpen forKey:PPAutomaticOpen];
	[defaults setInteger:thePrefs.RecordAllTrack forKey:PPRecordAllTrack];
	
	[defaults synchronize];
	
	[pool drain];
}

void WriteCFPreferencesArrayWithQDColor(NSInteger valueNum, RGBColor valVal)
{
	NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];
	NSUserDefaults *defaults = [NSUserDefaults standardUserDefaults];
	NSMutableArray *colorsArray = [[defaults valueForKey:PPCColorArray] mutableCopy];
	NSData *colorData = [[NSColor PPColorFromQDColor:valVal] PPencodeColor];
	[colorsArray replaceObjectAtIndex:valueNum - 1 withObject:colorData];
	//[[NSUserDefaults standardUserDefaults] setObject:[[NSColor PPColorFromQDColor:valVal] PPencodeColor] forKey:valName];
	[defaults setObject:colorsArray forKey:PPCColorArray];
	
	[colorsArray release];
	[pool drain];
}

void WriteCFPreferencesWithQDColor(NSString *valName, RGBColor valVal)
{
	NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];
	[[NSUserDefaults standardUserDefaults] setObject:[[NSColor PPColorFromQDColor:valVal] PPencodeColor] forKey:valName];
	[pool drain];
}

BOOL ReadCFPreferencesWithQDColor(NSString *valName, RGBColor *valVal)
{
	NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];
	NSData *colorDat = [[NSUserDefaults standardUserDefaults] valueForKey:valName];
	if (colorDat) {
		NSColor *colorClass = [NSColor PPDecodeColorWithData:colorDat];
		if (colorClass) {
			*valVal = [colorClass PPQDColor];
			[pool drain];
			return YES;
		}
	}
	[pool drain];
	return NO;
}

BOOL ReadCFPreferencesArrayWithQDColor(NSInteger valueNum, RGBColor *valVal)
{
	NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];
	NSArray *colorArray = [[NSUserDefaults standardUserDefaults] valueForKey:PPCColorArray];
	if (colorArray) {
		NSData *colorDat = [colorArray objectAtIndex:valueNum - 1];
		if (colorDat) {
			NSColor *colorClass = [NSColor PPDecodeColorWithData:colorDat];
			if (colorClass) {
				*valVal = [colorClass PPQDColor];
				[pool drain];
				return YES;
			}
		}
	}
	[pool drain];
	return NO;
}
