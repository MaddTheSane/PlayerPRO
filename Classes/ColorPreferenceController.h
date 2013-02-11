//
//  ColorPreferenceController.h
//  PPMacho
//
//  Created by C.W. Betts on 7/26/10.
//  Copyright 2010 __MyCompanyName__. All rights reserved.
//

#import <Cocoa/Cocoa.h>

@interface ColorPreferenceController : NSViewController {

	IBOutlet __weak NSColorWell *colorWell1;
	IBOutlet __weak NSColorWell *colorWell2;
	IBOutlet __weak NSColorWell *colorWell3;
	IBOutlet __weak NSColorWell *colorWell4;
	IBOutlet __weak NSColorWell *colorWell5;
	IBOutlet __weak NSColorWell *colorWell6;
	IBOutlet __weak NSColorWell *colorWell7;
	IBOutlet __weak NSColorWell *colorWell8;
}
- (IBAction)changeColorWell:(id)sender;

@end
