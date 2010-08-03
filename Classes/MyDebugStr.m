//
//  MyDebugStr.m
//  PPMacho
//
//  Created by C.W. Betts on 7/29/10.
//  Copyright 2010 __MyCompanyName__. All rights reserved.
//

#import <Cocoa/Cocoa.h>
#import <Carbon/Carbon.h>

extern void MyDebugStr( short line, Ptr file, Ptr text)
{
	NSLog(@"%s:%u error text:%s!", file, line, text);
	NSInteger alert = NSRunAlertPanel(NSLocalizedString(@"MyDebugStr_Error",@"Error"), [NSString stringWithFormat:NSLocalizedString(@"MyDebugStr_MainText", @"The program has encountered the following error:\n %s\nIt is reccomended to quit the program and send the debug report."), text], NSLocalizedString(@"MyDebugStr_Quit",@"Quit"), NSLocalizedString(@"MyDebugStr_Continue",@"Continue"), NSLocalizedString(@"MyDebugStr_Debug",@"Debug"));
	switch (alert) {
		case NSAlertAlternateReturn:
			break;
		case NSAlertOtherReturn:
			Debugger();
			break;
		case NSAlertDefaultReturn:
			NSLog(@"Choosing to fail!");
		default:
			exit(EXIT_FAILURE);
			break;

	}
}
