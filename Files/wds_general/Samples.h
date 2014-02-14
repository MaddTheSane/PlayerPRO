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
		void CreateSampleWindow(short ins, short samp);
		void DrawEnvelope(short ins, DialogPtr theDia);
		void DrawPanningEnvelope(short ins, DialogPtr theDia);
		void DrawSample(short tSS, short tSE, short InstruNo, DialogPtr theDia);

	};
}

#endif
