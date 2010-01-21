#include <Carbon/Carbon.r>

resource 'ALRT' (5011, purgeable) {
	{80, 116, 235, 422},
	5011,
	{	/* array: 4 elements */
		/* [1] */
		OK, visible, sound1,
		/* [2] */
		OK, visible, sound1,
		/* [3] */
		OK, visible, sound1,
		/* [4] */
		OK, visible, sound1
	},
	alertPositionMainScreen
};

resource 'DITL' (5010, purgeable) {
	{	/* array DITLarray: 9 elements */
		/* [1] */
		{68, 340, 85, 400},
		Button {
			enabled,
			"OK"
		},
		/* [2] */
		{68, 259, 85, 319},
		Button {
			enabled,
			"Cancel"
		},
		/* [3] */
		{8, 96, 24, 267},
		StaticText {
			disabled,
			"Untitled"
		},
		/* [4] */
		{36, 96, 52, 267},
		StaticText {
			disabled,
			"Untitled"
		},
		/* [5] */
		{8, 296, 24, 396},
		UserItem {
			enabled
		},
		/* [6] */
		{36, 296, 52, 396},
		UserItem {
			enabled
		},
		/* [7] */
		{8, 8, 24, 93},
		StaticText {
			disabled,
			"Instrument:"
		},
		/* [8] */
		{36, 8, 52, 93},
		StaticText {
			disabled,
			"Clipboard:"
		},
		/* [9] */
		{4, 4, 56, 400},
		UserItem {
			disabled
		}
	}
};

resource 'DITL' (5011, purgeable) {
	{	/* array DITLarray: 2 elements */
		/* [1] */
		{123, 229, 143, 289},
		Button {
			enabled,
			"OK"
		},
		/* [2] */
		{13, 60, 118, 294},
		StaticText {
			disabled,
			"^0"
		}
	}
};

data 'DLGX' (5010) {
	$"0843 6861 7263 6F61 6C00 0000 0000 0000"            /* .Charcoal....... */
	$"0000 0000 0000 0000 0000 0000 0000 0000"            /* ................ */
	$"0000 0000 0000 0000 0000 0000 0000 0000"            /* ................ */
	$"0000 0000 0000 0000 0000 0000 0000 0000"            /* ................ */
	$"000C 0000 0000 0000 0004 0004 0000 0000"            /* ................ */
	$"0009 0000 0000 0000 0000 0000 0000 0001"            /* .Æ.............. */
	$"0000 0000 0000 0000 0000 0006 0000 0000"            /* ................ */
	$"0000 0000 0000 0006 0000 0000 0000 0000"            /* ................ */
	$"0000 000A 0000 0000 0000 0000 0000 000A"            /* ...Â...........Â */
	$"0000 0000 0000 0000 0000 0006 0000 0000"            /* ................ */
	$"0000 0000 0000 0006 0000 0000 0000 0000"            /* ................ */
	$"0000 000A 0000 0000 0000 0000 0000"                 /* ...Â.......... */
};

resource 'DLOG' (5010, purgeable) {
	{50, 43, 149, 449},
	movableDBoxProc,
	invisible,
	goAway,
	0x0,
	5010,
	"Mix",
	alertPositionMainScreen
};

resource 'STR#' (1000) {
	{	/* array StringArray: 1 elements */
		/* [1] */
		"MixÉ"
	}
};

resource 'STR#' (5010) {
	{	/* array StringArray: 3 elements */
		/* [1] */
		"Sorry, but Mix requires a sound to be in"
		" the clipboard.",
		/* [2] */
		"Sorry, but I don't know about this sound"
		" format.",
		/* [3] */
		"Sorry, but the sound in the clipboard ai"
		"n't a sampled sound"
	}
};

resource 'dctb' (5010) {
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

data 'ictb' (5010) {
	$"0000 0000 0000 0000 0000 0000 0000 0000"            /* ................ */
	$"0000 0000 0000 0000 0000 0000 0000 0000"            /* ................ */
	$"0000 0000"                                          /* .... */
};

