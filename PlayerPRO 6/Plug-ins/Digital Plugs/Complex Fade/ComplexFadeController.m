//
//  ComplexFadeController.m
//  PPMacho
//
//  Created by C.W. Betts on 9/10/13.
//
//

#import "ComplexFadeController.h"
//#import "CompFadeNoteFormatter.h"

#define kNoteCompareOptions (NSCaseInsensitiveSearch | NSWidthInsensitiveSearch | NSDiacriticInsensitiveSearch)

@interface ComplexFadeController ()

@end

static int NSStringToHex(NSString *str)
{
	NSScanner *tmpScanner = [[NSScanner alloc] initWithString:str];
	unsigned int tmpVal = 0;
	if (![tmpScanner scanHexInt:&tmpVal])
		return -1;
	else if (tmpVal == UINT32_MAX || tmpVal > 0xFF) {
		return -1;
	}
	
	return tmpVal;
}

@implementation ComplexFadeController
@synthesize fadeType;
@synthesize thePcmd = myPcmd;

- (BOOL)validateSettings;
{
	int from, to, step;
	NSNumberFormatter *numFormat = [NSNumberFormatter new];
	NSNumber *ourNumber;

	switch(fadeType) {
		case PPFadeTypeInstrument:
			if ([numFormat getObjectValue:&ourNumber forString:self.fromCell.stringValue errorDescription:nil]) {
				from = ourNumber.intValue;
			} else {
				return NO;
			}
			
			if ([numFormat getObjectValue:&ourNumber forString:self.toCell.stringValue errorDescription:nil]) {
				to = ourNumber.intValue;
			} else {
				return NO;
			}

			if (from < 1 || from > 64) {
				return NO;
			}
			if (to < 1 || to > 64) {
				return NO;
			}
			break;
			
		case PPFadeTypeNote:
			from = [PPSampleObject noteFromString:self.fromCell.stringValue];
			to = [PPSampleObject noteFromString:self.toCell.stringValue];
			
			if (from < 0 || from >= 96) {
				return NO;
			}
			if (to < 0 || to >= 96) {
				return NO;
			}
			break;
			
		case PPFadeTypeArgument:
			from = NSStringToHex(self.fromCell.stringValue);
			to = NSStringToHex(self.toCell.stringValue);
			
			if (from < 0 || from > 0xFF) {
				return NO;
			}
			if (to < 0 || to > 0xFF) {
				return NO;
			}
			break;
			
		case PPFadeTypeVolume:
			from = NSStringToHex(self.fromCell.stringValue);
			to = NSStringToHex(self.toCell.stringValue);
			
			if (from < 0 || from > 0xFF) {
				return NO;
			}
			if (to < 0 || to > 0xFF) {
				return NO;
			}
			break;
	}
	
	if ([numFormat getObjectValue:&ourNumber forString:self.stepCell.stringValue errorDescription:nil]) {
		step = ourNumber.intValue;
	} else {
		return NO;
	}

	
	if (step < 1 || step > 64) {
		return NO;
	}
	
	return YES;
}

#if 0
- (instancetype)initWithWindow:(NSWindow *)window
{
	if (self = [super initWithWindow:window]) {
	}
	
	return self;
}

- (void)windowDidLoad
{
	[super windowDidLoad];
	// Implement this method to handle any initialization after your window controller's window has been loaded from its nib file.
}
#endif

- (IBAction)changeFadeType:(id)sender
{
	self.fadeType = [sender tag];
	switch (self.fadeType) {
		case PPFadeTypeInstrument:
			self.valueInfo.stringValue = [[NSBundle bundleForClass:[self class]] localizedStringForKey:@"From 1 to 64" value:nil table:nil];
			self.fromCell.placeholderString = @"1";
			self.toCell.placeholderString = @"64";
			break;
			
		case PPFadeTypeArgument:
			self.valueInfo.stringValue = [[NSBundle bundleForClass:[self class]] localizedStringForKey:@"From C0 to B7" value:nil table:nil];
			self.fromCell.placeholderString = @"C0";
			self.toCell.placeholderString = @"B7";
			break;
			
		case PPFadeTypeNote:
			self.valueInfo.stringValue = [[NSBundle bundleForClass:[self class]] localizedStringForKey:@"From 00 to FF" value:nil table:nil];
			self.fromCell.placeholderString = @"00";
			self.toCell.placeholderString = @"FF";
			break;
			
		case PPFadeTypeVolume:
			self.valueInfo.stringValue = [[NSBundle bundleForClass:[self class]] localizedStringForKey:@"From 00 to FF" value:nil table:nil];
			self.fromCell.placeholderString = @"00";
			self.toCell.placeholderString = @"FF";
			break;
	}
}

- (IBAction)okay:(id)sender
{
	if (![self validateSettings]) {
		NSBeep();
		NSAlert *badSettings = [[NSAlert alloc] init];
		badSettings.messageText = @"Invalid Value";
		badSettings.informativeText = @"There is one or more invalid value.";
		[badSettings addButtonWithTitle:@"OK"];
		
		[badSettings beginSheetModalForWindow:[self window] completionHandler:^(NSModalResponse returnCode) {
			;//Do nothing right now
		}];
	} else {
		int from = 0, to = 0, step = 1;
		NSNumberFormatter *numFormat = [NSNumberFormatter new];
		NSNumber *ourNumber;
		
		switch(fadeType) {
			case PPFadeTypeInstrument:
				ourNumber = [numFormat numberFromString:self.fromCell.stringValue];
				from = ourNumber.intValue;
				
				ourNumber = [numFormat numberFromString:self.toCell.stringValue];
				to = ourNumber.intValue;
				break;
				
			case PPFadeTypeNote:
				from = [PPSampleObject noteFromString:self.fromCell.stringValue];
				to = [PPSampleObject noteFromString:self.toCell.stringValue];
				break;
				
			case PPFadeTypeArgument:
				from = NSStringToHex(self.fromCell.stringValue);
				to = NSStringToHex(self.toCell.stringValue);
				break;
				
			case PPFadeTypeVolume:
				from = NSStringToHex(self.fromCell.stringValue);
				to = NSStringToHex(self.toCell.stringValue);
				break;
		}
		
		if ([numFormat getObjectValue:&ourNumber forString:self.stepCell.stringValue errorDescription:nil]) {
			step = ourNumber.intValue;
		}
		
		for (short track = 0; track < myPcmd->tracks; track ++) {
			for (short row = 0; row < myPcmd->length; row += step) {
				Cmd *myCmd = MADGetCmd(row, track, myPcmd);
				
				if (myPcmd->length > 1) {			// no zero div !!
					switch(fadeType) {
						case PPFadeTypeInstrument:
							myCmd->ins	= from + ((to-from) * row) / (myPcmd->length-1);
							break;
							
						case PPFadeTypeNote:
							myCmd->note = from + ((to-from) * row) / (myPcmd->length-1);
							break;
							
						case PPFadeTypeArgument:
							myCmd->arg	= from + ((to-from) * row) / (myPcmd->length-1);
							break;
							
						case PPFadeTypeVolume:
							myCmd->vol	= from + ((to-from) * row) / (myPcmd->length-1);
							break;
					}
				}
			}
		}

		[(NSApplication*)NSApp endSheet:self.window];
		_currentBlock(MADNoErr);
	}
}

- (IBAction)cancel:(id)sender
{
	[(NSApplication*)NSApp endSheet:self.window];
	_currentBlock(MADUserCanceledErr);
}

@end
