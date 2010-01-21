#include <Carbon/Carbon.r>

resource 'STR#' (1000) {
	{	/* array StringArray: 4 elements */
		/* [1] */
		"STrk",
		/* [2] */
		"EXIM",
		/* [3] */
		"MOD",
		/* [4] */
		"Written by A.ROSSET 95"
	}
};

resource 'vers' (1) {
	0x1,
	0x4,
	release,
	0x0,
	0,
	"1.04",
	"1.04 - ROSSET Antoine 96"
};

resource 'BNDL' (128) {
	'SNPL',
	0,
	{	/* array TypeArray: 2 elements */
		/* [1] */
		'FREF',
		{	/* array IDArray: 1 elements */
			/* [1] */
			0, 128
		},
		/* [2] */
		'ICN#',
		{	/* array IDArray: 1 elements */
			/* [1] */
			0, 128
		}
	}
};

data 'SNPL' (0, "Owner resource") {
	$"00"                                                 /* . */
};

resource 'ICN#' (128) {
	{	/* array: 2 elements */
		/* [1] */
		$"1FFF FE00 1000 0300 1039 0280 103F 0240"
		$"103E 0220 1010 0210 1010 03F8 1008 0008"
		$"10E8 0008 113C 0008 117C 0008 11FC 0E08"
		$"10FC 1108 1078 2088 1000 4048 1000 4048"
		$"1200 8028 1200 8028 1101 0008 1101 0008"
		$"1082 0008 1044 0008 1038 0008 1000 0008"
		$"1044 CE08 106D 2908 1055 2908 1045 2908"
		$"1044 CE08 1000 0008 1000 0008 1FFF FFF8",
		/* [2] */
		$"1FFF FE00 1FFF FF00 1FFF FF80 1FFF FFC0"
		$"1FFF FFE0 1FFF FFF0 1FFF FFF8 1FFF FFF8"
		$"1FFF FFF8 1FFF FFF8 1FFF FFF8 1FFF FFF8"
		$"1FFF FFF8 1FFF FFF8 1FFF FFF8 1FFF FFF8"
		$"1FFF FFF8 1FFF FFF8 1FFF FFF8 1FFF FFF8"
		$"1FFF FFF8 1FFF FFF8 1FFF FFF8 1FFF FFF8"
		$"1FFF FFF8 1FFF FFF8 1FFF FFF8 1FFF FFF8"
		$"1FFF FFF8 1FFF FFF8 1FFF FFF8 1FFF FFF8"
	}
};

resource 'icl8' (128) {
	$"0000 00FF FFFF FFFF FFFF FFFF FFFF FFFF"
	$"FFFF FFFF FFFF FF00 0000 0000 0000 0000"
	$"0000 00FF F5F5 F5F5 F5F5 F5F5 F5F5 F5F5"
	$"F5F5 F5F5 F5F5 FFFF 0000 0000 0000 0000"
	$"0000 00FF F5F6 F6F6 F6F6 FFFF FFF6 F6FF"
	$"F6F6 F6F6 F6F8 FFF5 FF00 0000 0000 0000"
	$"0000 00FF F5F6 F6F6 F6F6 FFFF FFFF FFFF"
	$"F6F6 F6F6 F6F8 FFF5 F8FF 0000 0000 0000"
	$"0000 00FF F5F6 F6F6 F6F6 FFFF FFFF FFF6"
	$"F6F6 F6F6 F6F8 FFF5 F6F8 FF00 0000 0000"
	$"0000 00FF F5F6 F6F6 F6F6 F6FF F6F6 F6F6"
	$"F6F6 F6F6 F6F8 FFF5 F8F8 F8FF 0000 0000"
	$"0000 00FF F5F6 F6F6 F6F6 F6FF F6F6 F6F6"
	$"F6F6 F6F6 F6F8 FFFF FFFF FFFF FF00 0000"
	$"0000 00FF F5F6 F6F6 F6F6 F6F6 FFF6 F6F6"
	$"F6F6 F6F6 F6F8 F5F5 F5F5 F5F5 FF00 0000"
	$"0000 00FF F5F6 F6F6 FFFF FFF6 FFF6 F6F6"
	$"F6F6 F6F6 F6F6 F6F6 F6F6 F6F8 FF00 0000"
	$"0000 00FF F5F6 F6FF 0000 FFFF FFFF F6F6"
	$"F6F6 F6F6 F6F6 F6F6 F6F6 F6F8 FF00 0000"
	$"0000 00FF F5F6 F6FF 00FF FFFF FFFF F6F6"
	$"F6F6 F6F6 F6F6 F6F6 F6F6 F6F8 FF00 0000"
	$"0000 00FF F5F6 F6FF FFFF FFFF FFFF F6F6"
	$"F6F6 F6F8 FFFF FFF8 F6F6 F6F8 FF00 0000"
	$"0000 00FF F5F6 F6F6 FFFF FFFF FFFF F6F6"
	$"F6F6 F8FF F8F6 F8FF F8F6 F6F8 FF00 0000"
	$"0000 00FF F5F6 F6F6 F6FF FFFF FFF6 F6F6"
	$"F6F8 FFF8 F6F6 F6F8 FFF8 F6F8 FF00 0000"
	$"0000 00FF F5F6 F6F6 F6F6 F6F6 F6F6 F6F6"
	$"F6FF F8F6 F6F6 F6F6 F8FF F6F8 FF00 0000"
	$"0000 00FF F5F6 F6F6 F6F6 F6F6 F6F6 F6F6"
	$"F8FF F6F6 F6F6 F6F6 F6FF F8F8 FF00 0000"
	$"0000 00FF F5F6 FFF6 F6F6 F6F6 F6F6 F6F6"
	$"FFF8 F6F6 F6F6 F6F6 F6F8 FFF8 FF00 0000"
	$"0000 00FF F5F6 FFF8 F6F6 F6F6 F6F6 F6F8"
	$"FFF6 F6F6 F6F6 F6F6 F6F6 FFF8 FF00 0000"
	$"0000 00FF F5F6 F8FF F6F6 F6F6 F6F6 F6FF"
	$"F8F6 F6F6 F6F6 F6F6 F6F6 F6F8 FF00 0000"
	$"0000 00FF F5F6 F6FF F8F6 F6F6 F6F6 F8FF"
	$"F6F6 F6F6 F6F6 F6F6 F6F6 F6F8 FF00 0000"
	$"0000 00FF F5F6 F6F8 FFF8 F6F6 F6F8 FFF8"
	$"F6F6 F6F6 F6F6 F6F6 F6F6 F6F8 FF00 0000"
	$"0000 00FF F5F6 F6F6 F8FF F8F6 F8FF F8F6"
	$"F6F6 F6F6 F6F6 F6F6 F6F6 F6F8 FF00 0000"
	$"0000 00FF F5F6 F6F6 F6F8 FFFF FFF8 F6F6"
	$"F6F6 F6F6 F6F6 F6F6 F6F6 F6F8 FF00 0000"
	$"0000 00FF F5F6 F6F6 F6F6 F6F6 F6F6 F6F6"
	$"F6F6 F6F6 F6F6 F6F6 F6F6 F6F8 FF00 0000"
	$"0000 00FF F5F6 F6F6 F6EC F6F6 F6EC F6F6"
	$"ECEC F6F6 ECEC ECF6 F6F6 F6F8 FF00 0000"
	$"0000 00FF F5F6 F6F6 F6EC ECF6 ECEC F6EC"
	$"F6F6 ECF6 ECF6 F6EC F6F6 F6F8 FF00 0000"
	$"0000 00FF F5F6 F6F6 F6EC F6EC F6EC F6EC"
	$"F6F6 ECF6 ECF6 F6EC F6F6 F6F8 FF00 0000"
	$"0000 00FF F5F6 F6F6 F6EC F6F6 F6EC F6EC"
	$"F6F6 ECF6 ECF6 F6EC F6F6 F6F8 FF00 0000"
	$"0000 00FF F5F6 F6F6 F6EC F6F6 F6EC F6F6"
	$"ECEC F6F6 ECEC ECF6 F6F6 F6F8 FF00 0000"
	$"0000 00FF F5F6 F6F6 F6F6 F6F6 F6F6 F6F6"
	$"F6F6 F6F6 F6F6 F6F6 F6F6 F6F8 FF00 0000"
	$"0000 00FF F5F8 F8F8 F8F8 F8F8 F8F8 F8F8"
	$"F8F8 F8F8 F8F8 F8F8 F8F8 F8F8 FF00 0000"
	$"0000 00FF FFFF FFFF FFFF FFFF FFFF FFFF"
	$"FFFF FFFF FFFF FFFF FFFF FFFF FF"
};

resource 'ics#' (128) {
	{	/* array: 2 elements */
		/* [1] */
		$"7FF0 4018 4014 401E 4002 4062 4092 408A"
		$"510A 5102 4A02 4402 4002 4FF2 4002 7FFE",
		/* [2] */
		$"7FF0 7FF8 7FFC 7FFE 7FFE 7FFE 7FFE 7FFE"
		$"7FFE 7FFE 7FFE 7FFE 7FFE 7FFE 7FFE 7FFE"
	}
};

resource 'ics8' (128) {
	$"00FF FFFF FFFF FFFF FFFF FFFF 0000 0000"
	$"00FF F5F5 F5F5 F5F5 F5F5 F5FF FF00 0000"
	$"00FF F5F6 F6F6 F6F6 F6F6 F8FF F6FF 0000"
	$"00FF F5F6 F6F6 F6F6 F6F6 F8FF FFFF FF00"
	$"00FF F5F6 F6F6 F6F6 F6F6 F8F5 F5F5 FF00"
	$"00FF F5F6 F6F6 F6F6 F8FF FFF8 F6F8 FF00"
	$"00FF F5F6 F6F6 F6F6 FFF8 F8FF F8F8 FF00"
	$"00FF F5F6 F6F6 F6F8 FFF6 F6F8 FFF8 FF00"
	$"00FF F5FF F6F6 F6FF F8F6 F6F6 FFF8 FF00"
	$"00FF F5FF F8F6 F8FF F6F6 F6F6 F6F8 FF00"
	$"00FF F5F8 FFF8 FFF8 F6F6 F6F6 F6F8 FF00"
	$"00FF F5F6 F8FF F8F6 F6F6 F6F6 F6F8 FF00"
	$"00FF F5F6 F6F6 F6F6 F6F6 F6F6 F6F8 FF00"
	$"00FF F5F6 ECEC ECEC ECEC ECEC F6F8 FF00"
	$"00FF F5F8 F8F8 F8F8 F8F8 F8F8 F8F8 FF00"
	$"00FF FFFF FFFF FFFF FFFF FFFF FFFF FF"
};

resource 'FREF' (128) {
	'STrk',
	0,
	""
};

