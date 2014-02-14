//
//  Samples.h
//  PPMacho
//
//  Created by C.W. Betts on 2/13/14.
//
//

#ifndef PPMacho_Samples_h
#define PPMacho_Samples_h
#include "wds_general.h"

namespace PlayerPRO {
	class Samples : public wds_general {
		Samples();
		~Samples();
		void DoHelp(short **items, short *lsize);
	};
}

#endif
