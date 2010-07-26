//
//  ManagingViewController.m
//  Departments
//
//  Created by C.W. Betts on 7/24/10.
//  Copyright 2010 __MyCompanyName__. All rights reserved.
//

#import "ManagingViewController.h"


@implementation ManagingViewController
@synthesize managedObjectContext;

-(void)dealloc
{
	[managedObjectContext release];
	
	[super dealloc];
}

@end
