//
//  ColorPreferenceController.h
//  PPMacho
//
//  Created by C.W. Betts on 7/26/10.
//  Copyright 2010 __MyCompanyName__. All rights reserved.
//

#import <Cocoa/Cocoa.h>
#import <MASPreferences/MASPreferencesViewController.h>

@class ColorPreferenceObject;

@interface ColorPreferenceController : NSViewController <MASPreferencesViewController>
@property (weak) IBOutlet NSCollectionView *colorWells;
@property (copy) NSArray<ColorPreferenceObject*> *colors;

+ (instancetype)newPreferenceView NS_RETURNS_RETAINED;

- (IBAction)changeColorWell:(id)sender;

- (void)replaceObjectInColorsAtIndex:(NSInteger)index withObject:(ColorPreferenceObject*)object;
@end
