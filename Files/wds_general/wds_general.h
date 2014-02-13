/*
 *  wds_general.h
 *  PPMacho
 *
 *  Created by C.W. Betts on 2/13/14.
 *  Copyright 2014 __MyCompanyName__. All rights reserved.
 *
 */

#include "Shuddup.h"

namespace PlayerPRO {
	class wds_general {
	public:
		wds_general(DialogPtr aDiag);
		virtual ~wds_general();
		
	protected:
		DialogPtr theDialog;
	};
}

