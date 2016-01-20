#include <Carbon/Carbon.r>
#include "RequestVideoPrivate.h"

data 'ALRT' (rConfirmSwtchAlrt, "Confirmation Alert") {
	$"0064 0087 00E0 01EE 0AAF 4444"                      /* .d.‡.à.îÂ¯DD */
};

resource 'DITL' (rConfirmSwtchAlrt, "Confirmation Alert") {
	{	/* array DITLarray: 4 elements */
		/* [1] */
		{91, 193, 111, 263},
		Button {
			enabled,
			"Cancel"
		},
		/* [2] */
		{91, 276, 111, 346},
		Button {
			enabled,
			"OK"
		},
		/* [3] */
		{13, 26, 45, 58},
		Icon {
			disabled,
			2
		},
		/* [4] */
		{13, 78, 79, 353},
		StaticText {
			disabled,
			"The setting for your monitor has been ch"
			"anged.  Click OK to confirm that the mon"
			"itor is displayed correctly, or click Ca"
			"ncel to revert to the previous setting."
		}
	}
};

