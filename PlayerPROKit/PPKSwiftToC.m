//
//  PPKSwiftToC.m
//  PPMacho
//
//  Created by C.W. Betts on 1/12/15.
//
//

#import <Foundation/Foundation.h>
#import <PlayerPROKit/PlayerPROKit-Swift.h>
#import "PPSampleObject.h"

short PPNoteFromString(NSString *aNote)
{
	return [PPSampleObject noteFromString:aNote];
}

NSString *PPStringFromNote(short note)
{
	return [PPSampleObject octaveNameFromNote:note];
}

