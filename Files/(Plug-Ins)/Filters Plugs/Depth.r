#include <Carbon/Carbon.r>

resource 'DITL' (128) {
	{	/* array DITLarray: 5 elements */
		/* [1] */
		{41, 110, 58, 170},
		Button {
			enabled,
			"OK"
		},
		/* [2] */
		{41, 186, 58, 246},
		Button {
			enabled,
			"Cancel"
		},
		/* [3] */
		{11, 169, 27, 204},
		EditText {
			enabled,
			""
		},
		/* [4] */
		{11, 216, 27, 245},
		StaticText {
			disabled,
			"bits"
		},
		/* [5] */
		{10, 5, 29, 167},
		StaticText {
			disabled,
			"Simulate a bit depth of:"
		}
	}
};

data 'DLGX' (128) {
	$"0843 6861 7263 6F61 6C00 0000 0000 0000"            /* .Charcoal....... */
	$"0000 0000 0000 0000 0000 0000 0000 0000"            /* ................ */
	$"0000 0000 0000 0000 0000 0000 0000 0000"            /* ................ */
	$"0000 0000 0000 0000 0000 0000 0000 0000"            /* ................ */
	$"000C 0000 0000 0000 0004 0004 0000 0000"            /* ................ */
	$"0005 0000 0000 0000 0000 0000 0000 0001"            /* ................ */
	$"0000 0000 0000 0000 0000 0007 0000 0000"            /* ................ */
	$"0000 0000 0000 0006 0000 0000 0000 0000"            /* ................ */
	$"0000 0006 0000 0000 0000 0000 0000"                 /* .............. */
};

resource 'DLOG' (128) {
	{93, 104, 162, 355},
	movableDBoxProc,
	invisible,
	goAway,
	0x0,
	128,
	"Amplitude",
	centerMainScreen
};

resource 'STR#' (1000) {
	{	/* array StringArray: 1 elements */
		/* [1] */
		"Bit Depth"
	}
};

resource 'dctb' (128) {
	{	/* array ColorSpec: 5 elements */
		/* [1] */
		wContentColor, 56797, 56797, 56797,
		/* [2] */
		wFrameColor, 0, 0, 0,
		/* [3] */
		wTextColor, 0, 0, 0,
		/* [4] */
		wHiliteColor, 0, 0, 0,
		/* [5] */
		wTitleBarColor, 65535, 65535, 65535
	}
};

data 'ictb' (128) {
	$"0000 0000 0000 0000 0000 0000 0000 0000"            /* ................ */
	$"0000 0000"                                          /* .... */
};

