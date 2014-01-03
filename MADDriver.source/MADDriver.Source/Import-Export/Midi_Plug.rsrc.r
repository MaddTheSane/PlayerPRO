#include <Carbon/Carbon.r>

resource 'vers' (1) {
	0x1,
	0x1,
	release,
	0x0,
	0,
	"1.01",
	"1.01 - ROSSET Antoine 95"
};

resource 'DLOG' (4094, "Midi import") {
	{154, 174, 227, 507},
	movableDBoxProc,
	invisible,
	noGoAway,
	0x0,
	4094,
	"Midi Import",
	centerMainScreen
};

resource 'DITL' (4094) {
	{	/* array DITLarray: 5 elements */
		/* [1] */
		{10, 270, 30, 328},
		Button {
			enabled,
			"OK"
		},
		/* [2] */
		{6, 8, 22, 156},
		StaticText {
			disabled,
			"Midi import settings:"
		},
		/* [3] */
		{31, 30, 47, 153},
		StaticText {
			disabled,
			"Number of tracks:"
		},
		/* [4] */
		{31, 158, 47, 177},
		EditText {
			enabled,
			"8"
		},
		/* [5] */
		{51, 30, 69, 286},
		CheckBox {
			enabled,
			"Use Quicktime Musical Instruments"
		}
	}
};
