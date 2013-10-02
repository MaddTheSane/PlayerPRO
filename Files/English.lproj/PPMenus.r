#include <Carbon/Carbon.r>
#include "PPResDefs.h"

resource 'MBAR' (MAINMENUBAR) {
	{	/* array MenuArray: 8 elements */
		/* [1] */
		mAppleMenu,
		/* [2] */
		mFileMenu,
		/* [3] */
		mEditMenu,
		/* [4] */
		mViewsMenu,
		/* [5] */
		mEditorMenu,
		/* [6] */
		mInstrumentsMenu,
		/* [7] */
		mPatternsMenu,
		/* [8] */
		mInternetMenu,
		//TODO: Help and Window menus
//		mWindowMenu,
		mHelpMenu
		
	}
};

//data 'MDEF' (200, $FC) {
data 'MDEF' (200) {
	$"600A 0000 4D44 4546 07B4 0000 4EFA 0042"            /* `�..MDEF.�..N�.B */
	$"4E75 206F 0004 226F 0008 4A2F 000E 6608"            /* Nu o.."o..J/..f. */
	$"3169 0002 0002 600A 7010 D069 0002 3140"            /* 1i....`�p.�i..1@ */
	$"0002 700F D068 0002 3140 0006 700E C1EF"            /* ..p.�h..1@..p.�� */
	$"000C D051 3080 700E D050 3140 0004 4E75"            /* ..�Q0�p.�P1@..Nu */
	$"48E7 1F32 4FEF FDE2 266F 024E 246F 024A"            /* H�.2O���&o.N$o.J */
	$"2C6F 0242 4267 A887 4267 A88A 302F 0252"            /* ,o.BBg��Bg��0/.R */
	$"6716 5340 6700 012C 5340 6700 04E8 5340"            /* g.S@g..,S@g..�S@ */
	$"6700 052E 6000 060E 3F7C DDDD 0008 3F7C"            /* g...`...?|��..?| */
	$"DDDD 000A 3F7C DDDD 000C 4878 0021 A862"            /* ��.�?|��..Hx.!�b */
	$"486F 0008 AA15 7A00 0C45 0002 6C00 008A"            /* Ho..�.z..E..l..� */
	$"41EF 011E 5288 2C08 7600 0C43 0010 6C70"            /* A�..R�,.v..C..lp */
	$"2C46 2F0B 3003 5240 3F00 486F 0124 A946"            /* ,F/.0.R@?.Ho.$�F */
	$"1F05 3F03 2F0A 486F 011E 61FF FFFF FF36"            /* ..?./�Ho..a����6 */
	$"0C45 0001 4FEF 000C 660C 2053 3810 5344"            /* .E..O�..f. S8.SD */
	$"0244 000F 600C 2053 3810 5344 0244 00F0"            /* .D..`. S8.SD.D.� */
	$"E844 B843 6608 4878 00CD A862 6006 4878"            /* �D�Cf.Hx.ͨb`.Hx */
	$"0021 A862 2F0E 7000 102F 0122 2F00 486F"            /* .!�b/.p../."/.Ho */
	$"011E 3F3C 0001 A9CE 5243 0C43 0010 6D92"            /* ..?<..��RC.C..m� */
	$"5245 0C45 0002 6D80 4878 0021 A862 3E2A"            /* RE.E..m�Hx.!�b>* */
	$"0002 302A 0006 9047 3200 E049 EE49 D240"            /* ..0*..�G2.�I�I�@ */
	$"E241 D247 3F01 3F12 A893 3F6A 0002 0006"            /* �A�G?.?.��?j.... */
	$"302A 0006 906F 0006 3200 E049 EE49 D240"            /* 0*..�o..2.�I�I�@ */
	$"E241 D26F 0006 3F01 70F2 D06A 0004 3F00"            /* �A�o..?.p��j..?. */
	$"A891 3F2A 0002 70D6 D06A 0004 3F00 A893"            /* ��?*..p��j..?.�� */
	$"3F2A 0006 70D6 D06A 0004 3F00 A891 6000"            /* ?*..p��j..?.��`. */
	$"04F4 2F52 0116 2F6A 0004 011A 046F 002A"            /* .�/R../j.....o.* */
	$"011A 554F 2F2F 0248 486F 011C A8AD 101F"            /* ..UO//.HHo..��.. */
	$"6610 7600 554F A974 101F 6600 018E 6000"            /* f.v.UO�t..f..�`. */
	$"04C4 2E13 2047 0C50 07D0 6700 013E 3C90"            /* .�.. G.P.�g..><� */
	$"2053 30BC 07D0 4A56 6F44 1F3C 0001 3016"            /*  S0�.�JVoD.<..0. */
	$"5340 0240 000F 3F00 2F0A 486F 011E 61FF"            /* S@.@..?./�Ho..a� */
	$"FFFF FE12 486F 0122 A8A4 4227 3016 5340"            /* ���.Ho."��B'0.S@ */
	$"0240 00F0 E840 3F00 2F0A 486F 012A 61FF"            /* .@.��@?./�Ho.*a� */
	$"FFFF FDF2 486F 012E A8A4 4FEF 0018 2F0B"            /* ����Ho..��O�../. */
	$"3016 5340 0240 000F 5240 3F00 486F 0014"            /* 0.S@.@..R@?.Ho.. */
	$"A946 2F0B 3016 5340 0240 00F0 E840 5240"            /* �F/.0.S@.@.��@R@ */
	$"3F00 486F 0124 A946 522F 011E 1F6F 000F"            /* ?.Ho.$�FR/...o.. */
	$"0120 362A 0004 486F 0116 3F2A 0002 70D7"            /* . 6*..Ho..?*..p� */
	$"D043 3F00 3F2A 0006 3F03 A8A7 41EF 011E"            /* �C?.?*..?.��A�.. */
	$"5288 2608 4850 7000 102F 0122 2F00 486F"            /* R�&.HPp../."/.Ho */
	$"011E 3F3C 0001 A9CE 3E16 5347 3047 2F08"            /* ..?<..��>.SG0G/. */
	$"486F 0122 61FF 0000 0402 382A 0004 486F"            /* Ho."a�....8*..Ho */
	$"0116 3F2A 0002 70E5 D044 3F00 3F2A 0006"            /* ..?*..p��D?.?*.. */
	$"3F04 A8A7 2F03 7000 102F 0122 2F00 486F"            /* ?.��/.p../."/.Ho */
	$"011E 3F3C 0001 A9CE 3816 5344 4884 3044"            /* ..?<..��8.SDH�0D */
	$"2F08 486F 0122 61FF 0000 03C0 3A2A 0004"            /* /.Ho."a�...�:*.. */
	$"486F 0116 3F2A 0002 70F3 D045 3F00 3F2A"            /* Ho..?*..p��E?.?* */
	$"0006 3F05 A8A7 2F03 7000 102F 0122 2F00"            /* ..?.��/.p../."/. */
	$"486F 011E 3F3C 0001 A9CE 362F 0246 9652"            /* Ho..?<..��6/.F�R */
	$"48C3 87FC 000E 302F 0248 906A 0002 48C0"            /* HÇ�..0/.H�j..H� */
	$"81FC 000F 4A00 6612 E94B 4A56 671A 3016"            /* ��..J.f.�KJVg.0. */
	$"5340 0240 000F D640 600E 4A56 670A 3016"            /* S@.@..�@`.JVg�0. */
	$"5340 0240 00F0 D640 5243 4A43 666E 4A56"            /* S@.@.��@RCJCfnJV */
	$"6F44 1F3C 0001 3016 5340 0240 000F 3F00"            /* oD.<..0.S@.@..?. */
	$"2F0A 486F 011E 61FF FFFF FC9A 486F 0122"            /* /�Ho..a�����Ho." */
	$"A8A4 4227 3016 5340 0240 00F0 E840 3F00"            /* ��B'0.S@.@.��@?. */
	$"2F0A 486F 012A 61FF FFFF FC7A 486F 012E"            /* /�Ho.*a����zHo.. */
	$"A8A4 4FEF 0018 4256 3C2A 0004 486F 0116"            /* ��O�..BV<*..Ho.. */
	$"3F2A 0002 70D7 D046 3F00 3F2A 0006 3F06"            /* ?*..p��F?.?*..?. */
	$"A8A7 486F 0116 A8A3 6000 02CA 3816 B644"            /* ��Ho..��`..�8.�D */
	$"6700 02C2 4A56 6F44 1F3C 0001 3004 5340"            /* g..�JVoD.<..0.S@ */
	$"0240 000F 3F00 2F0A 486F 011E 61FF FFFF"            /* .@..?./�Ho..a��� */
	$"FC24 486F 0122 A8A4 4227 3016 5340 0240"            /* �$Ho."��B'0.S@.@ */
	$"00F0 E840 3F00 2F0A 486F 012A 61FF FFFF"            /* .��@?./�Ho.*a��� */
	$"FC04 486F 012E A8A4 4FEF 0018 1F3C 0001"            /* �.Ho..��O�...<.. */
	$"3003 5340 3800 0240 000F 3F00 2F0A 486F"            /* 0.S@8..@..?./�Ho */
	$"011E 61FF FFFF FBDE 486F 0122 A8A4 4227"            /* ..a�����Ho."��B' */
	$"3004 0240 00F0 E840 3F00 2F0A 486F 012A"            /* 0..@.��@?./�Ho.* */
	$"61FF FFFF FBC0 486F 012E A8A4 3C83 2F0B"            /* a�����Ho..��<�/. */
	$"3016 5340 0240 000F 5240 3F00 486F 002C"            /* 0.S@.@..R@?.Ho., */
	$"A946 2F0B 3016 5340 0240 00F0 E840 5240"            /* �F/.0.S@.@.��@R@ */
	$"3F00 486F 013C A946 522F 0136 1F6F 0027"            /* ?.Ho.<�FR/.6.o.' */
	$"0138 3F6A 0004 001C 486F 012E 3F2A 0002"            /* .8?j....Ho..?*.. */
	$"70D7 D06F 0022 3F00 3F2A 0006 3F2F 0026"            /* p��o."?.?*..?/.& */
	$"A8A7 41EF 0136 5288 2648 4850 7000 102F"            /* ��A�.6R�&HHPp../ */
	$"013A 2F00 486F 0136 3F3C 0001 A9CE 3616"            /* .:/.Ho.6?<..��6. */
	$"5343 3043 2F08 486F 013A 61FF 0000 01CC"            /* SC0C/.Ho.:a�...� */
	$"3F6A 0004 001A 486F 012E 3F2A 0002 70E5"            /* ?j....Ho..?*..p� */
	$"D06F 0020 3F00 3F2A 0006 3F2F 0024 A8A7"            /* �o. ?.?*..?/.$�� */
	$"2F0B 7000 102F 013A 2F00 486F 0136 3F3C"            /* /.p../.:/.Ho.6?< */
	$"0001 A9CE 3616 5343 4883 3043 2F08 486F"            /* ..��6.SCH�0C/.Ho */
	$"013A 61FF 0000 0184 3F6A 0004 0018 486F"            /* .:a�...�?j....Ho */
	$"012E 3F2A 0002 70F3 D06F 001E 3F00 3F2A"            /* ..?*..p��o..?.?* */
	$"0006 3F2F 0022 A8A7 2F0B 7000 102F 013A"            /* ..?/."��/.p../.: */
	$"2F00 486F 0136 3F3C 0001 A9CE 4FEF 0018"            /* /.Ho.6?<..��O�.. */
	$"6000 0132 7800 7600 0C43 0010 6C2A 2F0B"            /* `..2x.v..C..l*.. */
	$"3F03 486F 0124 A946 554F 486F 0120 A88C"            /* ?.Ho.$�FUOHo. �� */
	$"301F B044 6F0A 554F 486F 0120 A88C 381F"            /* 0.�Do�UOHo. ��8. */
	$"5243 0C43 0010 6DD6 3004 D040 5440 2053"            /* RC.C..m�0.�@T@ S */
	$"3140 0002 2053 317C 010A 0004 6000 00E6"            /* 1@.. S1|.�..`..� */
	$"2053 3096 486F 0246 A972 3016 5340 0240"            /*  S0�Ho.F�r0.S@.@ */
	$"00F0 E840 C1FC 000E 5E40 322F 0246 9240"            /* .��@��..^@2/.F�@ */
	$"3481 302F 0248 5F40 3540 0002 701F D06A"            /* 4�0/.H_@5@..p.�j */
	$"0002 3540 0006 3012 0640 010A 3540 0004"            /* ..5@..0..@.�5@.. */
	$"594F AA29 265F 200B 6700 008C 554F 2F2F"            /* YO�)&_ .g..�UO// */
	$"0248 2053 2068 0016 2050 4868 0006 A8AD"            /* .H S h.. PHh..�� */
	$"101F 6772 2053 2068 0016 2050 2F68 0006"            /* ..gr S h.. P/h.. */
	$"010E 2F68 000A 0112 382F 0112 362A 0004"            /* ../h.�..8/..6*.. */
	$"B644 6F0C 2F0A 4267 3004 9043 3F00 A8A8"            /* �Do./�Bg0.�C?.�� */
	$"3612 B66F 010E 6C0E 2F0A 4267 302F 0114"            /* 6.�o..l./�Bg0/.. */
	$"9043 3F00 A8A8 382F 0114 362A 0006 B644"            /* �C?.��8/..6*..�D */
	$"6F0C 2F0A 3004 9043 3F00 4267 A8A8 382F"            /* o./�0.�C?.Bg��8/ */
	$"0110 362A 0002 B644 6C0C 2F0A 3004 9043"            /* ..6*..�Dl./�0.�C */
	$"3F00 4267 A8A8 594F 2F0B AA2B 265F 200B"            /* ?.Bg��YO/.�+&_ . */
	$"6600 FF64 4267 A887 4267 A88A 4FEF 021E"            /* f.�dBg��Bg��O�.. */
	$"4CDF 4CF8 4E74 0012 206F 0004 202F 0008"            /* L�L�Nt.. o.. /.. */
	$"4267 A9EE 205F 504F 4ED0"                           /* Bg�� _PON� */
};

data 'MDEF' (1972, sysheap, purgeable, locked, protected, preload) {
	$"600A 0000 4D44 4546 07B4 0000 4EFA 0066"            /* `�..MDEF.�..N�.f */
	$"4E75 0000 48E7 0410 266F 000C 226F 0010"            /* Nu..H�..&o.."o.. */
	$"322F 0014 5341 3041 2008 81FC 000C 3A00"            /* 2/..SA0A .��..:. */
	$"C1FC 000C 3401 9440 3205 701B C1C2 D069"            /* ��..4.�@2.p.���i */
	$"0002 3740 0002 3002 5240 C1FC 001B D069"            /* ..7@..0.R@��..�i */
	$"0002 3740 0006 700E C1C1 D051 3680 3001"            /* ..7@..p.���Q6�0. */
	$"5240 C1FC 000E D051 3740 0004 4CDF 0820"            /* R@��..�Q7@..L�.  */
	$"4E75 0000 48E7 1E32 4FEF FEEA 3A2F 0146"            /* Nu..H�.2O���:/.F */
	$"266F 0142 246F 013E 2C6F 0136 554F 2F0B"            /* &o.B$o.>,o.6UO/. */
	$"A950 301F 3800 4267 A887 4267 A88A 3005"            /* �P0.8.Bg��Bg��0. */
	$"6716 5340 6700 0096 5340 6700 0128 5340"            /* g.S@g..�S@g..(S@ */
	$"6700 016A 6000 026E 3EBC DDDD 3F7C DDDD"            /* g..j`..n>���?|�� */
	$"0002 3F7C DDDD 0004 4857 AA14 2F0A A8A2"            /* ..?|��..HW�./¨� */
	$"4878 0021 A862 4857 AA15 7601 4DEF 000E"            /* Hx.!�bHW�.v.M�.. */
	$"528E 604A 2F0B 3F03 486F 0014 A946 536F"            /* R�`J/.?.Ho..�FSo */
	$"000A 3F03 2F0A 486F 000C 61FF FFFF FF18"            /* .�?./�Ho..a����. */
	$"2053 B650 4FEF 000A 6608 4878 00CD A862"            /*  S�PO�.�f.Hx.ͨb */
	$"6006 4878 0021 A862 2F0E 7000 102F 0012"            /* `.Hx.!�b/.p../.. */
	$"2F00 486F 000E 3F3C 0001 A9CE 5243 B644"            /* /.Ho..?<..��RC�D */
	$"6FB2 4878 0021 A862 6000 01EA 554F 2F2F"            /* o�Hx.!�b`..�UO// */
	$"013C 2F0A A8AD 101F 6604 7600 602A 362F"            /* .</¨�..f.v.`*6/ */
	$"013A 9652 48C3 87FC 000E 48C3 C7FC 000C"            /* .:�RHÇ�..H���.. */
	$"302F 013C 906A 0002 48C0 81FC 001B D640"            /* 0/.<�j..H���..�@ */
	$"5243 B644 6D02 3604 4A43 661E 3F16 2F0A"            /* RC�Dm.6.JCf.?./� */
	$"486F 000C 61FF FFFF FE8E 486F 0010 A8A4"            /* Ho..a�����Ho..�� */
	$"4256 4FEF 000A 6000 018C 3816 B644 6700"            /* BVO�.�`..�8.�Dg. */
	$"0184 3F04 2F0A 486F 000C 61FF FFFF FE68"            /* .�?./�Ho..a����h */
	$"486F 0010 A8A4 3F03 2F0A 486F 0016 61FF"            /* Ho..��?./�Ho..a� */
	$"FFFF FE54 486F 001A A8A4 3C83 4FEF 0014"            /* ���THo..��<�O�.. */
	$"6000 0152 7A00 7601 6026 2F0B 3F03 486F"            /* `..Rz.v.`&/.?.Ho */
	$"0014 A946 554F 486F 0010 A88C 301F B045"            /* ..�FUOHo..��0.�E */
	$"6F0C 554F 486F 0010 A88C 301F 3A00 5243"            /* o.UOHo..��0.:.RC */
	$"B644 6FD6 3005 5440 2053 3140 0002 700E"            /* �Do�0.T@ S1@..p. */
	$"C1C4 2053 3140 0004 6000 010A 2053 3096"            /* �� S1@..`..� S0� */
	$"3016 5340 3600 3043 2008 81FC 000C 3C00"            /* 0.S@6.0C .��..<. */
	$"C1FC 000C 3A03 9A40 3606 700E C1C3 322F"            /* ��..:.�@6.p.��2/ */
	$"013C 9240 3481 701B C1C5 322F 013A 9240"            /* .<�@4�p.��2/.:�@ */
	$"3541 0002 302A 0002 0640 0144 3540 0006"            /* 5A..0*...@.D5@.. */
	$"3004 5340 48C0 81FC 000C 5240 C1FC 000E"            /* 0.S@H���..R@��.. */
	$"D052 3540 0004 486F 013A A972 594F AA29"            /* �R5@..Ho.:�rYO�) */
	$"205F 2648 200B 6700 008C 554F 2F2F 013C"            /*  _&H .g..�UO//.< */
	$"2053 2068 0016 2050 4868 0006 A8AD 101F"            /*  S h.. PHh..��.. */
	$"6772 2053 2068 0016 2050 2F68 0006 010E"            /* gr S h.. P/h.... */
	$"2F68 000A 0112 382F 0112 362A 0004 B644"            /* /h.�..8/..6*..�D */
	$"6F0C 2F0A 4267 3004 9043 3F00 A8A8 3612"            /* o./�Bg0.�C?.��6. */
	$"B66F 010E 6C0E 2F0A 4267 302F 0114 9043"            /* �o..l./�Bg0/..�C */
	$"3F00 A8A8 382F 0114 362A 0006 B644 6F0C"            /* ?.��8/..6*..�Do. */
	$"2F0A 3004 9043 3F00 4267 A8A8 382F 0110"            /* /�0.�C?.Bg��8/.. */
	$"362A 0002 B644 6C0C 2F0A 3004 9043 3F00"            /* 6*..�Dl./�0.�C?. */
	$"4267 A8A8 594F 2F0B AA2B 205F 2648 200B"            /* Bg��YO/.�+ _&H . */
	$"6600 FF62 4267 A887 4267 A88A 4FEF 0116"            /* f.�bBg��Bg��O�.. */
	$"4CDF 4C78 4E74 0012"                                /* L�LxNt.. */
};

resource 'MENU' (mFileMenu) {
	mFileMenu,
	textMenuProc,
	0x7FEFB77F,
	enabled,
	"File",
	{	/* array: 22 elements */
		/* [1] */
		"New...", noIcon, "N", noMark, plain,
		/* [2] */
		"Open...", noIcon, "O", noMark, plain,
		/* [3] */
		"Save", noIcon, "S", noMark, plain,
		/* [4] */
		"Save as...", noIcon, noKey, noMark, plain,
		/* [5] */
		"Close", noIcon, "W", noMark, plain,
		/* [6] */
		"Export as...", noIcon, hierarchicalMenu, "�", plain,
		/* [7] */
		"Reset", noIcon, "R", noMark, plain,
		/* [8] */
		"-", noIcon, noKey, noMark, plain,
		/* [9] */
		"Music List", noIcon, "M", noMark, plain,
		/* [10] */
		"Save Music List...", noIcon, noKey, noMark, plain,
		/* [11] */
		"Clear Music List", noIcon, noKey, noMark, plain,
		/* [12] */
		"-", noIcon, noKey, noMark, plain,
		/* [13] */
		"Page SetUp...", noIcon, noKey, noMark, plain,
		/* [14] */
		"Print...", noIcon, noKey, noMark, plain,
		/* [15] */
		"-", noIcon, noKey, noMark, plain,
		/* [16] */
		"Preferences...", noIcon, hierarchicalMenu, "�", plain,
		/* [17] */
		"Windows Sets...", noIcon, hierarchicalMenu, "�", plain,
		/* [18] */
		"Sound Control Panel...", noIcon, noKey, noMark, plain,
		/* [19] */
		"OMS Studio Setup...", noIcon, noKey, noMark, plain,
		/* [20] */
		"OMS MIDI Setup...", noIcon, noKey, noMark, plain,
		/* [21] */
		"-", noIcon, noKey, noMark, plain,
		/* [22] */
		"Quit", noIcon, "Q", noMark, plain
	}
};

resource 'MENU' (129) {
	129,
	textMenuProc,
	allEnabled,
	enabled,
	"Quality",
	{	/* array: 6 elements */
		/* [1] */
		"5 Khz", noIcon, noKey, noMark, plain,
		/* [2] */
		"11 Khz", noIcon, noKey, noMark, plain,
		/* [3] */
		"16 Khz", noIcon, noKey, noMark, plain,
		/* [4] */
		"22 Khz", noIcon, noKey, noMark, plain,
		/* [5] */
		"44 Khz", noIcon, noKey, noMark, plain,
		/* [6] */
		"48 Khz", noIcon, noKey, noMark, plain
	}
};

resource 'MENU' (mInstrumentsMenu) {
	mInstrumentsMenu,
	textMenuProc,
	0x7FFFFFDD,
	enabled,
	"Instruments",
	{	/* array: 7 elements */
		/* [1] */
		"Instruments List", noIcon, "L", noMark, plain,
		/* [2] */
		"-", noIcon, noKey, noMark, plain,
		/* [3] */
		"New...", noIcon, hierarchicalMenu, "�", plain,
		/* [4] */
		"Export As...", noIcon, noKey, noMark, plain,
		/* [5] */
		"Delete", noIcon, noKey, noMark, plain,
		/* [6] */
		"-", noIcon, noKey, noMark, plain,
		/* [7] */
		"Save Instruments List...", noIcon, noKey, noMark, plain
	}
};

resource 'MENU' (mViewsMenu, "Views Menu") {
	mViewsMenu,
	textMenuProc,
	AllItems & ~MenuItem2 & ~MenuItem5,
	enabled,
	"Views",
	{	/* array: 11 elements */
		/* [1] */
//		"Show Online Help", noIcon, noKey, noMark, plain,
		/* [2] */
//		"-", noIcon, noKey, noMark, plain,
		/* [3] */
		"Tools", noIcon, "T", noMark, plain,
		/* [4] */
		"-", noIcon, noKey, noMark, plain,
		/* [5] */
		"Oscillo View", noIcon, noKey, noMark, plain,
		/* [6] */
		"Spectrum View", noIcon, noKey, noMark, plain,
		/* [7] */
		"-", noIcon, noKey, noMark, plain,
		/* [8] */
		"Mixer", noIcon, "B", noMark, plain,
		/* [9] */
		"Equalizer", noIcon, noKey, noMark, plain,
		/* [10] */
		"Pattern View", noIcon, noKey, noMark, plain,
		/* [11] */
		"Piano", noIcon, "P", noMark, plain
	}
};

resource 'MENU' (mEditMenu) {
	mEditMenu,
	textMenuProc,
	0x7FFFFBBC,
	enabled,
	"Edit",
	{	/* array: 12 elements */
		/* [1] */
		"Undo", noIcon, "Z", noMark, plain,
		/* [2] */
		"-", noIcon, noKey, noMark, plain,
		/* [3] */
		"Cut", noIcon, "X", noMark, plain,
		/* [4] */
		"Copy", noIcon, "C", noMark, plain,
		/* [5] */
		"Paste", noIcon, "V", noMark, plain,
		/* [6] */
		"Clear", noIcon, noKey, noMark, plain,
		/* [7] */
		"-", noIcon, noKey, noMark, plain,
		/* [8] */
		"Duplicate", noIcon, noKey, noMark, plain,
		/* [9] */
		"Select All", noIcon, "A", noMark, plain,
		/* [10] */
		"Get Info...", noIcon, "I", noMark, plain,
		/* [11] */
		"-", noIcon, noKey, noMark, plain,
		/* [12] */
		"Use Second Music Buffer", noIcon, "Y", noMark, plain
	}
};

resource 'MENU' (mAppleMenu) {
	mAppleMenu,
	textMenuProc,
	0x7FFFFFFB,
	enabled,
	apple,
	{	/* array: 3 elements */
		/* [1] */
		"About PlayerPRO", noIcon, noKey, noMark, plain,
		/* [2] */
		"General Information...", noIcon, noKey, noMark, plain,
		/* [3] */
		"-", noIcon, noKey, noMark, plain
	}
};

resource 'MENU' (134) {
	134,
	textMenuProc,
	allEnabled,
	enabled,
	"Instru",
	{	/* array: 0 elements */
	}
};

resource 'MENU' (135) {
	135,
	textMenuProc,
	allEnabled,
	enabled,
	"Effect:",
	{	/* array: 19 elements */
		/* [1] */
		"0-Normal/Arpeggio", noIcon, noKey, noMark, plain,
		/* [2] */
		"1-Slide Up", noIcon, noKey, noMark, plain,
		/* [3] */
		"2-Slide Down", noIcon, noKey, noMark, plain,
		/* [4] */
		"3-Portamento", noIcon, noKey, noMark, plain,
		/* [5] */
		"4-\0x00Vibrato", noIcon, noKey, noMark, plain,
		/* [6] */
		"5-Portamento+Vol Slide", noIcon, noKey, noMark, plain,
		/* [7] */
		"6-Vibrato+Vol Slide", noIcon, noKey, noMark, plain,
		/* [8] */
		"7-Tremolo", noIcon, noKey, noMark, plain,
		/* [9] */
		"8-Set Panning", noIcon, noKey, noMark, plain,
		/* [10] */
		"9-Set Sample Offset", noIcon, noKey, noMark, plain,
		/* [11] */
		"A-VolumeSlide", noIcon, noKey, noMark, plain,
		/* [12] */
		"B-Position Jump", noIcon, noKey, noMark, plain,
		/* [13] */
		"C-Set Volume", noIcon, noKey, noMark, plain,
		/* [14] */
		"D-Pattern Break", noIcon, noKey, noMark, plain,
		/* [15] */
		"E-E Commands", noIcon, noKey, noMark, plain,
		/* [16] */
		"F-Set Speed", noIcon, noKey, noMark, plain,
		/* [17] */
		"G-Note OFF (Multi-Channel Tracks)", noIcon, noKey, noMark, plain,
		/* [18] */
		"L-Move Loop", noIcon, noKey, noMark, plain,
		/* [19] */
		"O-Set Sample Offset in %", noIcon, noKey, noMark, plain
	}
};

resource 'MENU' (136) {
	136,
	textMenuProc,
	0x7FFFFFB7,
	enabled,
	"Effect",
	{	/* array: 7 elements */
		/* [1] */
		"Selection->Loop", noIcon, noKey, noMark, plain,
		/* [2] */
		"Loop->Selection", noIcon, noKey, noMark, plain,
		/* [3] */
		"Delete", noIcon, noKey, noMark, plain,
		/* [4] */
		"-", noIcon, noKey, noMark, plain,
		/* [5] */
		"FFT - Hz Filter", noIcon, noKey, noMark, plain,
		/* [6] */
		"FFT - Hz Shift", noIcon, noKey, noMark, plain,
		/* [7] */
		"-", noIcon, noKey, noMark, plain
	}
};

resource 'MENU' (137) {
	137,
	textMenuProc,
	allEnabled,
	enabled,
	"Title",
	{	/* array: 0 elements */
	}
};

resource 'MENU' (138, "Import Special Format...") {
	138,
	textMenuProc,
	allEnabled,
	enabled,
	"Import",
	{	/* array: 0 elements */
	}
};

resource 'MENU' (139) {
	139,
	textMenuProc,
	0x7FFFFFFB,
	enabled,
	"Export",
	{	/* array: 3 elements */
		/* [1] */
		"AIFF File", noIcon, noKey, noMark, plain,
		/* [2] */
		"MPEG4 File", noIcon, noKey, noMark, plain,
		/* [3] */
		"-", noIcon, noKey, noMark, plain
	}
};

resource 'MENU' (mEditorMenu) {
	mEditorMenu,
	textMenuProc,
	0x7FFFFB6F,
	enabled,
	"Editor",
	{	/* array: 12 elements */
		/* [1] */
		"Digital Editor", noIcon, "D", noMark, plain,
		/* [2] */
		"Box Editor", noIcon, noKey, noMark, plain,
		/* [3] */
		"Classic Editor", noIcon, noKey, noMark, plain,
		/* [4] */
		"Wave Preview", noIcon, noKey, noMark, plain,
		/* [5] */
		"-", noIcon, noKey, noMark, plain,
		/* [6] */
		"Find...", noIcon, "F", noMark, plain,
		/* [7] */
		"Find Current Note", noIcon, "E", noMark, plain,
		/* [8] */
		"-", noIcon, noKey, noMark, plain,
		/* [9] */
		"Minimize", noIcon, noKey, noMark, plain,
		/* [10] */
		"Convert all Patterns to 64 rows", noIcon, noKey, noMark, plain,
		/* [11] */
		"-", noIcon, noKey, noMark, plain,
		/* [12] */
		"General Information", noIcon, noKey, noMark, plain
	}
};

resource 'MENU' (141) {
	141,
	1972,
	allEnabled,
	enabled,
	"Title",
	{	/* array: 1 elements */
		/* [1] */
		"\0x00", noIcon, noKey, noMark, plain
	}
};

resource 'MENU' (142) {
	142,
	textMenuProc,
	allEnabled,
	enabled,
	"Tracks Number",
	{	/* array: 0 elements */
	}
};

resource 'MENU' (143) {
	143,
	textMenuProc,
	allEnabled,
	enabled,
	"Quality",
	{	/* array: 3 elements */
		/* [1] */
		"F#5 - Good", noIcon, noKey, noMark, plain,
		/* [2] */
		"F#4 - Better", noIcon, noKey, noMark, plain,
		/* [3] */
		"F#3 - Best", noIcon, noKey, noMark, plain
	}
};

resource 'MENU' (144) {
	144,
	textMenuProc,
	allEnabled,
	enabled,
	"Title",
	{	/* array: 2 elements */
		/* [1] */
		"Off", noIcon, noKey, noMark, plain,
		/* [2] */
		"On", noIcon, noKey, noMark, plain
	}
};

resource 'MENU' (mPatternsMenu) {
	mPatternsMenu,
	textMenuProc,
	0x7FFFFFFB,
	enabled,
	"Patterns",
	{	/* array: 8 elements */
		/* [1] */
		"Partition List", noIcon, noKey, noMark, plain,
		/* [2] */
		"Patterns List", noIcon, noKey, noMark, plain,
		/* [3] */
		"-", noIcon, noKey, noMark, plain,
		/* [4] */
		"Create new pattern", noIcon, noKey, noMark, plain,
		/* [5] */
		"Open...", noIcon, noKey, noMark, plain,
		/* [6] */
		"Save As...", noIcon, noKey, noMark, plain,
		/* [7] */
		"Purge", noIcon, noKey, noMark, plain,
		/* [8] */
		"Delete", noIcon, noKey, noMark, plain
	}
};

resource 'MENU' (146) {
	146,
	textMenuProc,
	allEnabled,
	enabled,
	"Title",
	{	/* array: 0 elements */
	}
};

resource 'MENU' (147) {
	147,
	textMenuProc,
	allEnabled,
	enabled,
	"Compression",
	{	/* array: 2 elements */
		/* [1] */
		"NONE", noIcon, noKey, noMark, plain,
		/* [2] */
		"MAD1", noIcon, noKey, noMark, plain
	}
};

resource 'MENU' (148) {
	148,
	textMenuProc,
	0x7FFFFFFB,
	enabled,
	"Amplitude",
	{	/* array: 3 elements */
		/* [1] */
		"8 bits", noIcon, noKey, noMark, plain,
		/* [2] */
		"16 bits", noIcon, noKey, noMark, plain,
		/* [3] */
		"24 bits", noIcon, noKey, noMark, plain
	}
};

resource 'MENU' (149) {
	149,
	textMenuProc,
	allEnabled,
	enabled,
	"Special Keys",
	{	/* array: 17 elements */
		/* [1] */
		"Esc", noIcon, noKey, noMark, plain,
		/* [2] */
		"Tab", noIcon, noKey, noMark, plain,
		/* [3] */
		"F1", noIcon, noKey, noMark, plain,
		/* [4] */
		"F2", noIcon, noKey, noMark, plain,
		/* [5] */
		"F3", noIcon, noKey, noMark, plain,
		/* [6] */
		"F4", noIcon, noKey, noMark, plain,
		/* [7] */
		"F5", noIcon, noKey, noMark, plain,
		/* [8] */
		"F6", noIcon, noKey, noMark, plain,
		/* [9] */
		"F7", noIcon, noKey, noMark, plain,
		/* [10] */
		"F8", noIcon, noKey, noMark, plain,
		/* [11] */
		"F9", noIcon, noKey, noMark, plain,
		/* [12] */
		"F10", noIcon, noKey, noMark, plain,
		/* [13] */
		"F12", noIcon, noKey, noMark, plain,
		/* [14] */
		"F11", noIcon, noKey, noMark, plain,
		/* [15] */
		"F13", noIcon, noKey, noMark, plain,
		/* [16] */
		"F14", noIcon, noKey, noMark, plain,
		/* [17] */
		"F15", noIcon, noKey, noMark, plain
	}
};

resource 'MENU' (150) {
	150,
	textMenuProc,
	0x7FFFFFF7,
	enabled,
	"\0x00",
	{	/* array: 4 elements */
		/* [1] */
		"Audio OutPut", noIcon, noKey, noMark, plain,
		/* [2] */
		"Driver Tracks", noIcon, noKey, noMark, plain,
		/* [3] */
		"Audio InPut", noIcon, noKey, noMark, plain,
		/* [4] */
		"Quicktime Channel", noIcon, noKey, noMark, plain
	}
};

resource 'MENU' (151) {
	151,
	textMenuProc,
	allEnabled,
	enabled,
	"0",
	{	/* array: 5 elements */
		/* [1] */
		"16", noIcon, noKey, noMark, plain,
		/* [2] */
		"32", noIcon, noKey, noMark, plain,
		/* [3] */
		"64", noIcon, noKey, noMark, plain,
		/* [4] */
		"128", noIcon, noKey, noMark, plain,
		/* [5] */
		"256", noIcon, noKey, noMark, plain
	}
};

resource 'MENU' (152) {
	152,
	textMenuProc,
	allEnabled,
	enabled,
	"MIDI GM Instruments",
	{	/* array: 17 elements */
		/* [1] */
		"Piano", noIcon, noKey, noMark, plain,
		/* [2] */
		"Chromatic Percussion", noIcon, noKey, noMark, plain,
		/* [3] */
		"Organ", noIcon, noKey, noMark, plain,
		/* [4] */
		"Guitar", noIcon, noKey, noMark, plain,
		/* [5] */
		"Bass", noIcon, noKey, noMark, plain,
		/* [6] */
		"Strings", noIcon, noKey, noMark, plain,
		/* [7] */
		"Ensemble", noIcon, noKey, noMark, plain,
		/* [8] */
		"Brass", noIcon, noKey, noMark, plain,
		/* [9] */
		"Reed", noIcon, noKey, noMark, plain,
		/* [10] */
		"Pipe", noIcon, noKey, noMark, plain,
		/* [11] */
		"Synth Lead", noIcon, noKey, noMark, plain,
		/* [12] */
		"Synth Pad", noIcon, noKey, noMark, plain,
		/* [13] */
		"Synth Effects", noIcon, noKey, noMark, plain,
		/* [14] */
		"Ethnic", noIcon, noKey, noMark, plain,
		/* [15] */
		"Percussive", noIcon, noKey, noMark, plain,
		/* [16] */
		"Sound Effects", noIcon, noKey, noMark, plain,
		/* [17] */
		"Drums", noIcon, noKey, noMark, plain
	}
};

resource 'MENU' (153) {
	150,
	textMenuProc,
	allEnabled,
	enabled,
	"\0x00",
	{	/* array: 2 elements */
		/* [1] */
		"Audio OutPut", noIcon, noKey, noMark, plain,
		/* [2] */
		"Audio InPut", noIcon, noKey, noMark, plain
	}
};

resource 'MENU' (154) {
	154,
	textMenuProc,
	allEnabled,
	enabled,
	"Title",
	{	/* array: 7 elements */
		/* [1] */
		"Import a File...", noIcon, noKey, noMark, plain,
		/* [2] */
		"Silence/Tone Generator", noIcon, noKey, noMark, plain,
		/* [3] */
		"Record (Audio Input)", noIcon, noKey, noMark, plain,
		/* [4] */
		"Quicktime Instrument", noIcon, noKey, noMark, plain,
		/* [5] */
		"RAW Data Import...", noIcon, noKey, noMark, plain,
		/* [6] */
		"Audio CD Import...", noIcon, noKey, noMark, plain,
		/* [7] */
		"Selection in Digital Editor...", noIcon, noKey, noMark, plain
	}
};

resource 'MENU' (155) {
	155,
	textMenuProc,
	allEnabled,
	enabled,
	"Creator",
	{	/* array: 4 elements */
		/* [1] */
		"SNPL - PlayerPRO", noIcon, noKey, noMark, plain,
		/* [2] */
		"MMKR - MacModPro", noIcon, noKey, noMark, plain,
		/* [3] */
		"STrk - Sound-Trecker", noIcon, noKey, noMark, plain,
		/* [4] */
		"SCPL - SoundApp", noIcon, noKey, noMark, plain
	}
};

resource 'MENU' (156) {
	156,
	textMenuProc,
	0x7FFFFFFB,
	enabled,
	"Samples",
	{	/* array: 3 elements */
		/* [1] */
		"Volume Envelope", noIcon, noKey, noMark, plain,
		/* [2] */
		"Panning Envelope", noIcon, noKey, noMark, plain,
		/* [3] */
		"-", noIcon, noKey, noMark, plain
	}
};

resource 'MENU' (157) {
	157,
	textMenuProc,
	allEnabled,
	enabled,
	"MODs",
	{	/* array: 5 elements */
		/* [1] */
		"PlayerPRO", noIcon, noKey, noMark, plain,
		/* [2] */
		"SoundApp", noIcon, noKey, noMark, plain,
		/* [3] */
		"SoundTrecker", noIcon, noKey, noMark, plain,
		/* [4] */
		"MacMod Pro", noIcon, noKey, noMark, plain,
		/* [5] */
		"MDelta", noIcon, noKey, noMark, plain
	}
};

resource 'MENU' (158) {
	158,
	textMenuProc,
	allEnabled,
	enabled,
	"Type Files",
	{	/* array: 12 elements */
		/* [1] */
		"MADK", noIcon, noKey, noMark, plain,
		/* [2] */
		"MADI", noIcon, noKey, noMark, plain,
		/* [3] */
		"MADH", noIcon, noKey, noMark, plain,
		/* [4] */
		"MADF", noIcon, noKey, noMark, plain,
		/* [5] */
		"STrk - MOD", noIcon, noKey, noMark, plain,
		/* [6] */
		"MTM ", noIcon, noKey, noMark, plain,
		/* [7] */
		"S3M ", noIcon, noKey, noMark, plain,
		/* [8] */
		"OKTA", noIcon, noKey, noMark, plain,
		/* [9] */
		"6669", noIcon, noKey, noMark, plain,
		/* [10] */
		"Midi", noIcon, noKey, noMark, plain,
		/* [11] */
		"XM  ", noIcon, noKey, noMark, plain,
		/* [12] */
		"IT  ", noIcon, noKey, noMark, plain
	}
};

resource 'MENU' (159) {
	159,
	textMenuProc,
	allEnabled,
	enabled,
	"PPDG",
	{	/* array: 0 elements */
	}
};

resource 'MENU' (160) {
	160,
	textMenuProc,
	allEnabled,
	enabled,
	"scale",
	{	/* array: 2 elements */
		/* [1] */
		"linear", noIcon, noKey, noMark, plain,
		/* [2] */
		"logarithmic", noIcon, noKey, noMark, plain
	}
};

resource 'MENU' (161) {
	2000,
	200,
	allEnabled,
	enabled,
	"Sample",
	{	/* array: 256 elements */
		/* [1] */
		"0", noIcon, noKey, noMark, plain,
		/* [2] */
		"1", noIcon, noKey, noMark, plain,
		/* [3] */
		"2", noIcon, noKey, noMark, plain,
		/* [4] */
		"3", noIcon, noKey, noMark, plain,
		/* [5] */
		"4", noIcon, noKey, noMark, plain,
		/* [6] */
		"5", noIcon, noKey, noMark, plain,
		/* [7] */
		"6", noIcon, noKey, noMark, plain,
		/* [8] */
		"7", noIcon, noKey, noMark, plain,
		/* [9] */
		"8", noIcon, noKey, noMark, plain,
		/* [10] */
		"9", noIcon, noKey, noMark, plain,
		/* [11] */
		"A", noIcon, noKey, noMark, plain,
		/* [12] */
		"B", noIcon, noKey, noMark, plain,
		/* [13] */
		"C", noIcon, noKey, noMark, plain,
		/* [14] */
		"D", noIcon, noKey, noMark, plain,
		/* [15] */
		"E", noIcon, noKey, noMark, plain,
		/* [16] */
		"F", noIcon, noKey, noMark, plain,
		/* [17] */
		"0", noIcon, noKey, noMark, plain,
		/* [18] */
		"1", noIcon, noKey, noMark, plain,
		/* [19] */
		"2", noIcon, noKey, noMark, plain,
		/* [20] */
		"3", noIcon, noKey, noMark, plain,
		/* [21] */
		"4", noIcon, noKey, noMark, plain,
		/* [22] */
		"5", noIcon, noKey, noMark, plain,
		/* [23] */
		"6", noIcon, noKey, noMark, plain,
		/* [24] */
		"7", noIcon, noKey, noMark, plain,
		/* [25] */
		"8", noIcon, noKey, noMark, plain,
		/* [26] */
		"9", noIcon, noKey, noMark, plain,
		/* [27] */
		"A", noIcon, noKey, noMark, plain,
		/* [28] */
		"B", noIcon, noKey, noMark, plain,
		/* [29] */
		"C", noIcon, noKey, noMark, plain,
		/* [30] */
		"D", noIcon, noKey, noMark, plain,
		/* [31] */
		"E", noIcon, noKey, noMark, plain,
		/* [32] */
		"F", noIcon, noKey, noMark, plain,
		/* [33] */
		"0", noIcon, noKey, noMark, plain,
		/* [34] */
		"1", noIcon, noKey, noMark, plain,
		/* [35] */
		"2", noIcon, noKey, noMark, plain,
		/* [36] */
		"3", noIcon, noKey, noMark, plain,
		/* [37] */
		"4", noIcon, noKey, noMark, plain,
		/* [38] */
		"5", noIcon, noKey, noMark, plain,
		/* [39] */
		"6", noIcon, noKey, noMark, plain,
		/* [40] */
		"7", noIcon, noKey, noMark, plain,
		/* [41] */
		"8", noIcon, noKey, noMark, plain,
		/* [42] */
		"9", noIcon, noKey, noMark, plain,
		/* [43] */
		"A", noIcon, noKey, noMark, plain,
		/* [44] */
		"B", noIcon, noKey, noMark, plain,
		/* [45] */
		"C", noIcon, noKey, noMark, plain,
		/* [46] */
		"D", noIcon, noKey, noMark, plain,
		/* [47] */
		"E", noIcon, noKey, noMark, plain,
		/* [48] */
		"F", noIcon, noKey, noMark, plain,
		/* [49] */
		"0", noIcon, noKey, noMark, plain,
		/* [50] */
		"1", noIcon, noKey, noMark, plain,
		/* [51] */
		"2", noIcon, noKey, noMark, plain,
		/* [52] */
		"3", noIcon, noKey, noMark, plain,
		/* [53] */
		"4", noIcon, noKey, noMark, plain,
		/* [54] */
		"5", noIcon, noKey, noMark, plain,
		/* [55] */
		"6", noIcon, noKey, noMark, plain,
		/* [56] */
		"7", noIcon, noKey, noMark, plain,
		/* [57] */
		"8", noIcon, noKey, noMark, plain,
		/* [58] */
		"9", noIcon, noKey, noMark, plain,
		/* [59] */
		"A", noIcon, noKey, noMark, plain,
		/* [60] */
		"B", noIcon, noKey, noMark, plain,
		/* [61] */
		"C", noIcon, noKey, noMark, plain,
		/* [62] */
		"D", noIcon, noKey, noMark, plain,
		/* [63] */
		"E", noIcon, noKey, noMark, plain,
		/* [64] */
		"F", noIcon, noKey, noMark, plain,
		/* [65] */
		"0", noIcon, noKey, noMark, plain,
		/* [66] */
		"1", noIcon, noKey, noMark, plain,
		/* [67] */
		"2", noIcon, noKey, noMark, plain,
		/* [68] */
		"3", noIcon, noKey, noMark, plain,
		/* [69] */
		"4", noIcon, noKey, noMark, plain,
		/* [70] */
		"5", noIcon, noKey, noMark, plain,
		/* [71] */
		"6", noIcon, noKey, noMark, plain,
		/* [72] */
		"7", noIcon, noKey, noMark, plain,
		/* [73] */
		"8", noIcon, noKey, noMark, plain,
		/* [74] */
		"9", noIcon, noKey, noMark, plain,
		/* [75] */
		"A", noIcon, noKey, noMark, plain,
		/* [76] */
		"B", noIcon, noKey, noMark, plain,
		/* [77] */
		"C", noIcon, noKey, noMark, plain,
		/* [78] */
		"D", noIcon, noKey, noMark, plain,
		/* [79] */
		"E", noIcon, noKey, noMark, plain,
		/* [80] */
		"F", noIcon, noKey, noMark, plain,
		/* [81] */
		"0", noIcon, noKey, noMark, plain,
		/* [82] */
		"1", noIcon, noKey, noMark, plain,
		/* [83] */
		"2", noIcon, noKey, noMark, plain,
		/* [84] */
		"3", noIcon, noKey, noMark, plain,
		/* [85] */
		"4", noIcon, noKey, noMark, plain,
		/* [86] */
		"5", noIcon, noKey, noMark, plain,
		/* [87] */
		"6", noIcon, noKey, noMark, plain,
		/* [88] */
		"7", noIcon, noKey, noMark, plain,
		/* [89] */
		"8", noIcon, noKey, noMark, plain,
		/* [90] */
		"9", noIcon, noKey, noMark, plain,
		/* [91] */
		"A", noIcon, noKey, noMark, plain,
		/* [92] */
		"B", noIcon, noKey, noMark, plain,
		/* [93] */
		"C", noIcon, noKey, noMark, plain,
		/* [94] */
		"D", noIcon, noKey, noMark, plain,
		/* [95] */
		"E", noIcon, noKey, noMark, plain,
		/* [96] */
		"F", noIcon, noKey, noMark, plain,
		/* [97] */
		"0", noIcon, noKey, noMark, plain,
		/* [98] */
		"1", noIcon, noKey, noMark, plain,
		/* [99] */
		"2", noIcon, noKey, noMark, plain,
		/* [100] */
		"3", noIcon, noKey, noMark, plain,
		/* [101] */
		"4", noIcon, noKey, noMark, plain,
		/* [102] */
		"5", noIcon, noKey, noMark, plain,
		/* [103] */
		"6", noIcon, noKey, noMark, plain,
		/* [104] */
		"7", noIcon, noKey, noMark, plain,
		/* [105] */
		"8", noIcon, noKey, noMark, plain,
		/* [106] */
		"9", noIcon, noKey, noMark, plain,
		/* [107] */
		"A", noIcon, noKey, noMark, plain,
		/* [108] */
		"B", noIcon, noKey, noMark, plain,
		/* [109] */
		"C", noIcon, noKey, noMark, plain,
		/* [110] */
		"D", noIcon, noKey, noMark, plain,
		/* [111] */
		"E", noIcon, noKey, noMark, plain,
		/* [112] */
		"F", noIcon, noKey, noMark, plain,
		/* [113] */
		"0", noIcon, noKey, noMark, plain,
		/* [114] */
		"1", noIcon, noKey, noMark, plain,
		/* [115] */
		"2", noIcon, noKey, noMark, plain,
		/* [116] */
		"3", noIcon, noKey, noMark, plain,
		/* [117] */
		"4", noIcon, noKey, noMark, plain,
		/* [118] */
		"5", noIcon, noKey, noMark, plain,
		/* [119] */
		"6", noIcon, noKey, noMark, plain,
		/* [120] */
		"7", noIcon, noKey, noMark, plain,
		/* [121] */
		"8", noIcon, noKey, noMark, plain,
		/* [122] */
		"9", noIcon, noKey, noMark, plain,
		/* [123] */
		"A", noIcon, noKey, noMark, plain,
		/* [124] */
		"B", noIcon, noKey, noMark, plain,
		/* [125] */
		"C", noIcon, noKey, noMark, plain,
		/* [126] */
		"D", noIcon, noKey, noMark, plain,
		/* [127] */
		"E", noIcon, noKey, noMark, plain,
		/* [128] */
		"F", noIcon, noKey, noMark, plain,
		/* [129] */
		"0", noIcon, noKey, noMark, plain,
		/* [130] */
		"1", noIcon, noKey, noMark, plain,
		/* [131] */
		"2", noIcon, noKey, noMark, plain,
		/* [132] */
		"3", noIcon, noKey, noMark, plain,
		/* [133] */
		"4", noIcon, noKey, noMark, plain,
		/* [134] */
		"5", noIcon, noKey, noMark, plain,
		/* [135] */
		"6", noIcon, noKey, noMark, plain,
		/* [136] */
		"7", noIcon, noKey, noMark, plain,
		/* [137] */
		"8", noIcon, noKey, noMark, plain,
		/* [138] */
		"9", noIcon, noKey, noMark, plain,
		/* [139] */
		"A", noIcon, noKey, noMark, plain,
		/* [140] */
		"B", noIcon, noKey, noMark, plain,
		/* [141] */
		"C", noIcon, noKey, noMark, plain,
		/* [142] */
		"D", noIcon, noKey, noMark, plain,
		/* [143] */
		"E", noIcon, noKey, noMark, plain,
		/* [144] */
		"F", noIcon, noKey, noMark, plain,
		/* [145] */
		"0", noIcon, noKey, noMark, plain,
		/* [146] */
		"1", noIcon, noKey, noMark, plain,
		/* [147] */
		"2", noIcon, noKey, noMark, plain,
		/* [148] */
		"3", noIcon, noKey, noMark, plain,
		/* [149] */
		"4", noIcon, noKey, noMark, plain,
		/* [150] */
		"5", noIcon, noKey, noMark, plain,
		/* [151] */
		"6", noIcon, noKey, noMark, plain,
		/* [152] */
		"7", noIcon, noKey, noMark, plain,
		/* [153] */
		"8", noIcon, noKey, noMark, plain,
		/* [154] */
		"9", noIcon, noKey, noMark, plain,
		/* [155] */
		"A", noIcon, noKey, noMark, plain,
		/* [156] */
		"B", noIcon, noKey, noMark, plain,
		/* [157] */
		"C", noIcon, noKey, noMark, plain,
		/* [158] */
		"D", noIcon, noKey, noMark, plain,
		/* [159] */
		"E", noIcon, noKey, noMark, plain,
		/* [160] */
		"F", noIcon, noKey, noMark, plain,
		/* [161] */
		"0", noIcon, noKey, noMark, plain,
		/* [162] */
		"1", noIcon, noKey, noMark, plain,
		/* [163] */
		"2", noIcon, noKey, noMark, plain,
		/* [164] */
		"3", noIcon, noKey, noMark, plain,
		/* [165] */
		"4", noIcon, noKey, noMark, plain,
		/* [166] */
		"5", noIcon, noKey, noMark, plain,
		/* [167] */
		"6", noIcon, noKey, noMark, plain,
		/* [168] */
		"7", noIcon, noKey, noMark, plain,
		/* [169] */
		"8", noIcon, noKey, noMark, plain,
		/* [170] */
		"9", noIcon, noKey, noMark, plain,
		/* [171] */
		"A", noIcon, noKey, noMark, plain,
		/* [172] */
		"B", noIcon, noKey, noMark, plain,
		/* [173] */
		"C", noIcon, noKey, noMark, plain,
		/* [174] */
		"D", noIcon, noKey, noMark, plain,
		/* [175] */
		"E", noIcon, noKey, noMark, plain,
		/* [176] */
		"F", noIcon, noKey, noMark, plain,
		/* [177] */
		"0", noIcon, noKey, noMark, plain,
		/* [178] */
		"1", noIcon, noKey, noMark, plain,
		/* [179] */
		"2", noIcon, noKey, noMark, plain,
		/* [180] */
		"3", noIcon, noKey, noMark, plain,
		/* [181] */
		"4", noIcon, noKey, noMark, plain,
		/* [182] */
		"5", noIcon, noKey, noMark, plain,
		/* [183] */
		"6", noIcon, noKey, noMark, plain,
		/* [184] */
		"7", noIcon, noKey, noMark, plain,
		/* [185] */
		"8", noIcon, noKey, noMark, plain,
		/* [186] */
		"9", noIcon, noKey, noMark, plain,
		/* [187] */
		"A", noIcon, noKey, noMark, plain,
		/* [188] */
		"B", noIcon, noKey, noMark, plain,
		/* [189] */
		"C", noIcon, noKey, noMark, plain,
		/* [190] */
		"D", noIcon, noKey, noMark, plain,
		/* [191] */
		"E", noIcon, noKey, noMark, plain,
		/* [192] */
		"F", noIcon, noKey, noMark, plain,
		/* [193] */
		"0", noIcon, noKey, noMark, plain,
		/* [194] */
		"1", noIcon, noKey, noMark, plain,
		/* [195] */
		"2", noIcon, noKey, noMark, plain,
		/* [196] */
		"3", noIcon, noKey, noMark, plain,
		/* [197] */
		"4", noIcon, noKey, noMark, plain,
		/* [198] */
		"5", noIcon, noKey, noMark, plain,
		/* [199] */
		"6", noIcon, noKey, noMark, plain,
		/* [200] */
		"7", noIcon, noKey, noMark, plain,
		/* [201] */
		"8", noIcon, noKey, noMark, plain,
		/* [202] */
		"9", noIcon, noKey, noMark, plain,
		/* [203] */
		"A", noIcon, noKey, noMark, plain,
		/* [204] */
		"B", noIcon, noKey, noMark, plain,
		/* [205] */
		"C", noIcon, noKey, noMark, plain,
		/* [206] */
		"D", noIcon, noKey, noMark, plain,
		/* [207] */
		"E", noIcon, noKey, noMark, plain,
		/* [208] */
		"F", noIcon, noKey, noMark, plain,
		/* [209] */
		"0", noIcon, noKey, noMark, plain,
		/* [210] */
		"1", noIcon, noKey, noMark, plain,
		/* [211] */
		"2", noIcon, noKey, noMark, plain,
		/* [212] */
		"3", noIcon, noKey, noMark, plain,
		/* [213] */
		"4", noIcon, noKey, noMark, plain,
		/* [214] */
		"5", noIcon, noKey, noMark, plain,
		/* [215] */
		"6", noIcon, noKey, noMark, plain,
		/* [216] */
		"7", noIcon, noKey, noMark, plain,
		/* [217] */
		"8", noIcon, noKey, noMark, plain,
		/* [218] */
		"9", noIcon, noKey, noMark, plain,
		/* [219] */
		"A", noIcon, noKey, noMark, plain,
		/* [220] */
		"B", noIcon, noKey, noMark, plain,
		/* [221] */
		"C", noIcon, noKey, noMark, plain,
		/* [222] */
		"D", noIcon, noKey, noMark, plain,
		/* [223] */
		"E", noIcon, noKey, noMark, plain,
		/* [224] */
		"F", noIcon, noKey, noMark, plain,
		/* [225] */
		"0", noIcon, noKey, noMark, plain,
		/* [226] */
		"1", noIcon, noKey, noMark, plain,
		/* [227] */
		"2", noIcon, noKey, noMark, plain,
		/* [228] */
		"3", noIcon, noKey, noMark, plain,
		/* [229] */
		"4", noIcon, noKey, noMark, plain,
		/* [230] */
		"5", noIcon, noKey, noMark, plain,
		/* [231] */
		"6", noIcon, noKey, noMark, plain,
		/* [232] */
		"7", noIcon, noKey, noMark, plain,
		/* [233] */
		"8", noIcon, noKey, noMark, plain,
		/* [234] */
		"9", noIcon, noKey, noMark, plain,
		/* [235] */
		"A", noIcon, noKey, noMark, plain,
		/* [236] */
		"B", noIcon, noKey, noMark, plain,
		/* [237] */
		"C", noIcon, noKey, noMark, plain,
		/* [238] */
		"D", noIcon, noKey, noMark, plain,
		/* [239] */
		"E", noIcon, noKey, noMark, plain,
		/* [240] */
		"F", noIcon, noKey, noMark, plain,
		/* [241] */
		"0", noIcon, noKey, noMark, plain,
		/* [242] */
		"1", noIcon, noKey, noMark, plain,
		/* [243] */
		"2", noIcon, noKey, noMark, plain,
		/* [244] */
		"3", noIcon, noKey, noMark, plain,
		/* [245] */
		"4", noIcon, noKey, noMark, plain,
		/* [246] */
		"5", noIcon, noKey, noMark, plain,
		/* [247] */
		"6", noIcon, noKey, noMark, plain,
		/* [248] */
		"7", noIcon, noKey, noMark, plain,
		/* [249] */
		"8", noIcon, noKey, noMark, plain,
		/* [250] */
		"9", noIcon, noKey, noMark, plain,
		/* [251] */
		"A", noIcon, noKey, noMark, plain,
		/* [252] */
		"B", noIcon, noKey, noMark, plain,
		/* [253] */
		"C", noIcon, noKey, noMark, plain,
		/* [254] */
		"D", noIcon, noKey, noMark, plain,
		/* [255] */
		"E", noIcon, noKey, noMark, plain,
		/* [256] */
		"F", noIcon, noKey, noMark, plain
	}
};

resource 'MENU' (162) {
	162,
	textMenuProc,
	allEnabled,
	enabled,
	"Title",
	{	/* array: 3 elements */
		/* [1] */
		"Selection->loop", noIcon, noKey, noMark, plain,
		/* [2] */
		"Loop->Selection", noIcon, noKey, noMark, plain,
		/* [3] */
		"Set Sustain Point", noIcon, noKey, noMark, plain
	}
};

resource 'MENU' (163) {
	163,
	textMenuProc,
	0x7FFFFFDF,
	enabled,
	"\0x00",
	{	/* array: 22 elements */
		/* [1] */
		"11025 Hz", noIcon, noKey, noMark, plain,
		/* [2] */
		"11127 Hz", noIcon, noKey, noMark, plain,
		/* [3] */
		"22050 hz", noIcon, noKey, noMark, plain,
		/* [4] */
		"22254 Hz", noIcon, noKey, noMark, plain,
		/* [5] */
		"44100 Hz", noIcon, noKey, noMark, plain,
		/* [6] */
		"-", noIcon, noKey, noMark, plain,
		/* [7] */
		"8363 Hz (Amiga +0)", noIcon, noKey, noMark, plain,
		/* [8] */
		"8413 Hz (Amiga +1)", noIcon, noKey, noMark, plain,
		/* [9] */
		"8463 Hz (Amiga +2)", noIcon, noKey, noMark, plain,
		/* [10] */
		"8529 Hz (Amiga +3)", noIcon, noKey, noMark, plain,
		/* [11] */
		"8581 Hz (Amiga +4)", noIcon, noKey, noMark, plain,
		/* [12] */
		"8651 Hz (Amiga +5)", noIcon, noKey, noMark, plain,
		/* [13] */
		"8723 Hz (Amiga +6)", noIcon, noKey, noMark, plain,
		/* [14] */
		"8757 Hz (Amiga +7)", noIcon, noKey, noMark, plain,
		/* [15] */
		"8280 Hz (Amiga -1)", noIcon, noKey, noMark, plain,
		/* [16] */
		"8232 Hz (Amiga -2)", noIcon, noKey, noMark, plain,
		/* [17] */
		"8169 Hz (Amiga -3)", noIcon, noKey, noMark, plain,
		/* [18] */
		"8107 Hz (Amiga -4)", noIcon, noKey, noMark, plain,
		/* [19] */
		"8046 Hz (Amiga -5)", noIcon, noKey, noMark, plain,
		/* [20] */
		"7985 Hz (Amiga -6)", noIcon, noKey, noMark, plain,
		/* [21] */
		"7941 Hz (Amiga -7)", noIcon, noKey, noMark, plain,
		/* [22] */
		"7895 Hz (Amiga -8)", noIcon, noKey, noMark, plain
	}
};

resource 'MENU' (164) {
	164,
	textMenuProc,
	0x7FFFF77F,
	enabled,
	"Title",
	{	/* array: 13 elements */
		/* [1] */
		"USA", noIcon, noKey, noMark, plain,
		/* [2] */
		"Canada", noIcon, noKey, noMark, plain,
		/* [3] */
		"China", noIcon, noKey, noMark, plain,
		/* [4] */
		"Hong Kong", noIcon, noKey, noMark, plain,
		/* [5] */
		"South America", noIcon, noKey, noMark, plain,
		/* [6] */
		"Taiwan", noIcon, noKey, noMark, plain,
		/* [7] */
		"Thailand", noIcon, noKey, noMark, plain,
		/* [8] */
		"-", noIcon, noKey, noMark, plain,
		/* [9] */
		"England", noIcon, noKey, noMark, plain,
		/* [10] */
		"France", noIcon, noKey, noMark, plain,
		/* [11] */
		"Other European Countries", noIcon, noKey, noMark, plain,
		/* [12] */
		"-", noIcon, noKey, noMark, plain,
		/* [13] */
		"Other countries", noIcon, noKey, noMark, plain
	}
};

resource 'MENU' (165) {
	165,
	textMenuProc,
	allEnabled,
	enabled,
	"MS",
	{	/* array: 14 elements */
		/* [1] */
		"1 ms", noIcon, noKey, noMark, plain,
		/* [2] */
		"10 ms", noIcon, noKey, noMark, plain,
		/* [3] */
		"25 ms", noIcon, noKey, noMark, plain,
		/* [4] */
		"30 ms", noIcon, noKey, noMark, plain,
		/* [5] */
		"35 ms", noIcon, noKey, noMark, plain,
		/* [6] */
		"40 ms", noIcon, noKey, noMark, plain,
		/* [7] */
		"45 ms", noIcon, noKey, noMark, plain,
		/* [8] */
		"50 ms", noIcon, noKey, noMark, plain,
		/* [9] */
		"60 ms", noIcon, noKey, noMark, plain,
		/* [10] */
		"70 ms", noIcon, noKey, noMark, plain,
		/* [11] */
		"80 ms", noIcon, noKey, noMark, plain,
		/* [12] */
		"90 ms", noIcon, noKey, noMark, plain,
		/* [13] */
		"100 ms", noIcon, noKey, noMark, plain,
		/* [14] */
		"200 ms", noIcon, noKey, noMark, plain
	}
};

resource 'MENU' (166) {
	166,
	textMenuProc,
	allEnabled,
	enabled,
	"PPIN",
	{	/* array: 0 elements */
	}
};

resource 'MENU' (167) {
	167,
	textMenuProc,
	allEnabled,
	enabled,
	"PPIN",
	{	/* array: 0 elements */
	}
};

resource 'MENU' (168) {
	168,
	textMenuProc,
	allEnabled,
	enabled,
	"Osci Mode",
	{	/* array: 2 elements */
		/* [1] */
		"Stack", noIcon, noKey, noMark, plain,
		/* [2] */
		"Tile", noIcon, noKey, noMark, plain
	}
};

resource 'MENU' (169) {
	169,
	textMenuProc,
	allEnabled,
	enabled,
	"Title",
	{	/* array: 7 elements */
		/* [1] */
		"10 %", noIcon, noKey, noMark, plain,
		/* [2] */
		"20 %", noIcon, noKey, noMark, plain,
		/* [3] */
		"30 %", noIcon, noKey, noMark, plain,
		/* [4] */
		"40 %", noIcon, noKey, noMark, plain,
		/* [5] */
		"50 %", noIcon, noKey, noMark, plain,
		/* [6] */
		"60 %", noIcon, noKey, noMark, plain,
		/* [7] */
		"70 %", noIcon, noKey, noMark, plain
	}
};

resource 'MENU' (170) {
	170,
	textMenuProc,
	allEnabled,
	enabled,
	"MS",
	{	/* array: 14 elements */
		/* [1] */
		"25 ms", noIcon, noKey, noMark, plain,
		/* [2] */
		"30 ms", noIcon, noKey, noMark, plain,
		/* [3] */
		"35 ms", noIcon, noKey, noMark, plain,
		/* [4] */
		"40 ms", noIcon, noKey, noMark, plain,
		/* [5] */
		"45 ms", noIcon, noKey, noMark, plain,
		/* [6] */
		"50 ms", noIcon, noKey, noMark, plain,
		/* [7] */
		"60 ms", noIcon, noKey, noMark, plain,
		/* [8] */
		"70 ms", noIcon, noKey, noMark, plain,
		/* [9] */
		"80 ms", noIcon, noKey, noMark, plain,
		/* [10] */
		"90 ms", noIcon, noKey, noMark, plain,
		/* [11] */
		"100 ms", noIcon, noKey, noMark, plain,
		/* [12] */
		"200 ms", noIcon, noKey, noMark, plain,
		/* [13] */
		"300 ms", noIcon, noKey, noMark, plain,
		/* [14] */
		"400 ms", noIcon, noKey, noMark, plain
	}
};

resource 'MENU' (171) {
	171,
	textMenuProc,
	allEnabled,
	enabled,
	"Title",
	{	/* array: 8 elements */
		/* [1] */
		"100", noIcon, noKey, noMark, plain,
		/* [2] */
		"110", noIcon, noKey, noMark, plain,
		/* [3] */
		"120", noIcon, noKey, noMark, plain,
		/* [4] */
		"130", noIcon, noKey, noMark, plain,
		/* [5] */
		"140", noIcon, noKey, noMark, plain,
		/* [6] */
		"160", noIcon, noKey, noMark, plain,
		/* [7] */
		"180", noIcon, noKey, noMark, plain,
		/* [8] */
		"200", noIcon, noKey, noMark, plain
	}
};

resource 'MENU' (172) {
	165,
	textMenuProc,
	0x7FFFFFFB,
	enabled,
	"Mode",
	{	/* array: 6 elements */
		/* [1] */
		"Mono (Mix)", noIcon, noKey, noMark, plain,
		/* [2] */
		"Stereo", noIcon, noKey, noMark, plain,
		/* [3] */
		"-", noIcon, noKey, noMark, plain,
		/* [4] */
		"Mono (Delete Left)", noIcon, noKey, noMark, plain,
		/* [5] */
		"Mono (Delete Right)", noIcon, noKey, noMark, plain,
		/* [6] */
		"Mono (Add)", noIcon, noKey, noMark, plain
	}
};

resource 'MENU' (173) {
	173,
	textMenuProc,
	allEnabled,
	enabled,
	"\0x00",
	{	/* array: 2 elements */
		/* [1] */
		"Classic Loop", noIcon, noKey, noMark, plain,
		/* [2] */
		"Ping-Pong Loop", noIcon, noKey, noMark, plain
	}
};

resource 'MENU' (174) {
	174,
	textMenuProc,
	allEnabled,
	enabled,
	"Title",
	{	/* array: 6 elements */
		/* [1] */
		"No Editor", noIcon, noKey, noMark, plain,
		/* [2] */
		"Digital Editor", noIcon, noKey, noMark, plain,
		/* [3] */
		"Box Editor", noIcon, noKey, noMark, plain,
		/* [4] */
		"Classic Editor", noIcon, noKey, noMark, plain,
		/* [5] */
		"Wave Preview", noIcon, noKey, noMark, plain,
		/* [6] */
		"Pattern View", noIcon, noKey, noMark, plain
	}
};

resource 'MENU' (175) {
	175,
	textMenuProc,
	allEnabled,
	enabled,
	"OverSampling",
	{	/* array: 10 elements */
		/* [1] */
		"2 x", noIcon, noKey, noMark, plain,
		/* [2] */
		"3 x", noIcon, noKey, noMark, plain,
		/* [3] */
		"4 x", noIcon, noKey, noMark, plain,
		/* [4] */
		"6 x", noIcon, noKey, noMark, plain,
		/* [5] */
		"8 x", noIcon, noKey, noMark, plain,
		/* [6] */
		"10 x", noIcon, noKey, noMark, plain,
		/* [7] */
		"16 x", noIcon, noKey, noMark, plain,
		/* [8] */
		"20 x", noIcon, noKey, noMark, plain,
		/* [9] */
		"25 x", noIcon, noKey, noMark, plain,
		/* [10] */
		"30 x", noIcon, noKey, noMark, plain
	}
};

resource 'MENU' (176) {
	176,
	textMenuProc,
	0x7FFFFFF7,
	enabled,
	"Windows",
	{	/* array: 5 elements */
		/* [1] */
		"\0x00", noIcon, "1", noMark, plain,
		/* [2] */
		"\0x00", noIcon, "2", noMark, plain,
		/* [3] */
		"\0x00", noIcon, "3", noMark, plain,
		/* [4] */
		"-", noIcon, noKey, noMark, plain,
		/* [5] */
		"Settings", noIcon, noKey, noMark, plain
	}
};

resource 'MENU' (177) {
	177,
	textMenuProc,
	allEnabled,
	enabled,
	"No compression",
	{	/* array: 8 elements */
		/* [1] */
		"No compression", noIcon, noKey, noMark, plain,
		/* [2] */
		"MAC3", noIcon, noKey, noMark, plain,
		/* [3] */
		"MAC6", noIcon, noKey, noMark, plain,
		/* [4] */
		"ima4", noIcon, noKey, noMark, plain,
		/* [5] */
		"ulaw", noIcon, noKey, noMark, plain,
		/* [6] */
		"alaw", noIcon, noKey, noMark, plain,
		/* [7] */
		"QDMC", noIcon, noKey, noMark, plain,
		/* [8] */
		"QDM2", noIcon, noKey, noMark, plain
	}
};

resource 'MENU' (178) {
	178,
	textMenuProc,
	allEnabled,
	enabled,
	"Step",
	{	/* array: 9 elements */
		/* [1] */
		"1", noIcon, noKey, noMark, plain,
		/* [2] */
		"2", noIcon, noKey, noMark, plain,
		/* [3] */
		"3", noIcon, noKey, noMark, plain,
		/* [4] */
		"4", noIcon, noKey, noMark, plain,
		/* [5] */
		"5", noIcon, noKey, noMark, plain,
		/* [6] */
		"6", noIcon, noKey, noMark, plain,
		/* [7] */
		"7", noIcon, noKey, noMark, plain,
		/* [8] */
		"8", noIcon, noKey, noMark, plain,
		/* [9] */
		"9", noIcon, noKey, noMark, plain
	}
};

resource 'MENU' (mInternetMenu) {
	mInternetMenu,
	textMenuProc,
	0x7FFFFFAF,
	enabled,
	"Internet",
	{	/* array: 7 elements */
		/* [1] */
		"PlayerPRO Official Web Site", noIcon, noKey, noMark, plain,
		/* [2] */
		"MIT Archive for PlayerPRO", noIcon, noKey, noMark, plain,
		/* [3] */
		"Email to the author of PlayerPRO", noIcon, noKey, noMark, plain,
		/* [4] */
		"CASIMAGE Web Site", noIcon, noKey, noMark, plain,
		/* [5] */
		"-", noIcon, noKey, noMark, plain,
		/* [6] */
		"Add/Remove bookmarks...", noIcon, noKey, noMark, plain,
		/* [7] */
		"-", noIcon, noKey, noMark, plain
	}
};

resource 'MENU' (180) {
	180,
	textMenuProc,
	0x7FFFFFFD,
	enabled,
	"VST",
	{	/* array: 2 elements */
		/* [1] */
		"No Effect", noIcon, noKey, noMark, plain,
		/* [2] */
		"-", noIcon, noKey, noMark, plain
	}
};

resource 'MENU' (181) {
	181,
	textMenuProc,
	0x7FFFFFFB,
	enabled,
	"VSTMenuDest",
	{	/* array: 3 elements */
		/* [1] */
		"Send to Global Effects", noIcon, noKey, noMark, plain,
		/* [2] */
		"Bypass Global Effects", noIcon, noKey, noMark, plain,
		/* [3] */
		"-", noIcon, noKey, noMark, plain
	}
};

resource 'MENU' (mWindowMenu, "Window Menu") {
	182,
	textMenuProc,
	allEnabled,
	enabled,
	"Window",
	{
		"Minimize",				noIcon, "M", noMark, plain,
		"Zoom",					noIcon, noKey, noMark, plain,
		"-",					noIcon, noKey, noMark, plain,
		"Bring All to Front",	noIcon, noKey, noMark, plain,
		"-",					noIcon, noKey, noMark, plain
	}
};

resource 'MENU' (mHelpMenu, "Help Menu") {
	183,
	textMenuProc,
	allEnabled,
	enabled,
	"Help",
	{
		"Online Help",		noIcon, noKey, noMark, plain,
		"PlayerPRO Help",	noIcon, noKey, noMark, plain,
		"-",				noIcon, noKey, noMark, plain
	}
};


resource 'MENU' (200, "Prefs") {
	200,
	textMenuProc,
	allEnabled,
	enabled,
	"Preferences",
	{	/* array: 10 elements */
		/* [1] */
		"Driver - Output", noIcon, noKey, noMark, plain,
		/* [2] */
		"Piano", noIcon, noKey, noMark, plain,
		/* [3] */
		"Music List", noIcon, noKey, noMark, plain,
		/* [4] */
		"Color", noIcon, noKey, noMark, plain,
		/* [5] */
		"Misc", noIcon, noKey, noMark, plain,
		/* [6] */
		"Box Editor", noIcon, noKey, noMark, plain,
		/* [7] */
		"Digital Editor", noIcon, noKey, noMark, plain,
		/* [8] */
		"Classic Editor", noIcon, noKey, noMark, plain,
		/* [9] */
		"FKEYs", noIcon, noKey, noMark, plain,
		/* [10] */
		"MIDI Hardware", noIcon, noKey, noMark, plain
	}
};

resource 'MENU' (500) {
	500,
	textMenuProc,
	0x7FFFFFD7,
	enabled,
	"Show",
	{	/* array: 6 elements */
		/* [1] */
		"All Available formats", noIcon, noKey, noMark, plain,
		/* [2] */
		"All Readable files", noIcon, noKey, noMark, plain,
		/* [3] */
		"All files", noIcon, noKey, noMark, plain,
		/* [4] */
		"-", noIcon, noKey, noMark, plain,
		/* [5] */
		"MADK PlayerPRO", noIcon, noKey, noMark, plain,
		/* [6] */
		"-", noIcon, noKey, noMark, plain
	}
};

data 'Mcmd' (1) {
	$"0000"                                               /* .. */
};

data 'Mcmd' (mFileMenu) {
	$"0016 0000 0000 0000 0000 0000 0000 0000"            /* ................ */
	$"0000 0000 0000 0000 0000 0000 0000 0000"            /* ................ */
	$"0000 0000 0000 0000 0000 0000 0000 0000"            /* ................ */
	$"0000 0000 0000 0000 0000 0000 0000 0000"            /* ................ */
	$"0000 0000 0000 0000 0000 0000 0000 0000"            /* ................ */
	$"0000 0000 0000 0000 0000"                           /* .......... */
};

data 'Mcmd' (mInstrumentsMenu) {
	$"0007 0000 0000 0000 0000 0000 0000 0000"            /* ................ */
	$"0000 0000 0000 0000 0000 0000 0000"                 /* .............. */
};

data 'Mcmd' (mViewsMenu) {
	$"000B 0000 0000 0000 0000 0000 0000 0000"            /* ................ */
	$"0000 0000 0000 0000 0000 0000 0000 0000"            /* ................ */
	$"0000 0000 0000 0000 0000 0000 0000"                 /* .............. */
};

data 'Mcmd' (mEditMenu) {
	$"000C 0000 0000 0000 0000 0000 0000 0000"            /* ................ */
	$"0000 0000 0000 0000 0000 0000 0000 0000"            /* ................ */
	$"0000 0000 0000 0000 0000 0000 0000 0000"            /* ................ */
	$"0000"                                               /* .. */
};

data 'Mcmd' (mAppleMenu) {
	$"0003 0000 0000 0000 0000 0000 0000"                 /* .............. */
};

data 'Mcmd' (135) {
	$"0000"                                               /* .. */
};

data 'Mcmd' (150) {
	$"0000"                                               /* .. */
};

data 'Mcmd' (154) {
	$"0004 0000 0000 0000 0000 0000 0000 0000"            /* ................ */
	$"0000"                                               /* .. */
};

data 'Mcmd' (158) {
	$"0000"                                               /* .. */
};

data 'Mcmd' (161) {
	$"0100 0000 0000 0000 0000 0000 0000 0000"            /* ................ */
	$"0000 0000 0000 0000 0000 0000 0000 0000"            /* ................ */
	$"0000 0000 0000 0000 0000 0000 0000 0000"            /* ................ */
	$"0000 0000 0000 0000 0000 0000 0000 0000"            /* ................ */
	$"0000 0000 0000 0000 0000 0000 0000 0000"            /* ................ */
	$"0000 0000 0000 0000 0000 0000 0000 0000"            /* ................ */
	$"0000 0000 0000 0000 0000 0000 0000 0000"            /* ................ */
	$"0000 0000 0000 0000 0000 0000 0000 0000"            /* ................ */
	$"0000 0000 0000 0000 0000 0000 0000 0000"            /* ................ */
	$"0000 0000 0000 0000 0000 0000 0000 0000"            /* ................ */
	$"0000 0000 0000 0000 0000 0000 0000 0000"            /* ................ */
	$"0000 0000 0000 0000 0000 0000 0000 0000"            /* ................ */
	$"0000 0000 0000 0000 0000 0000 0000 0000"            /* ................ */
	$"0000 0000 0000 0000 0000 0000 0000 0000"            /* ................ */
	$"0000 0000 0000 0000 0000 0000 0000 0000"            /* ................ */
	$"0000 0000 0000 0000 0000 0000 0000 0000"            /* ................ */
	$"0000 0000 0000 0000 0000 0000 0000 0000"            /* ................ */
	$"0000 0000 0000 0000 0000 0000 0000 0000"            /* ................ */
	$"0000 0000 0000 0000 0000 0000 0000 0000"            /* ................ */
	$"0000 0000 0000 0000 0000 0000 0000 0000"            /* ................ */
	$"0000 0000 0000 0000 0000 0000 0000 0000"            /* ................ */
	$"0000 0000 0000 0000 0000 0000 0000 0000"            /* ................ */
	$"0000 0000 0000 0000 0000 0000 0000 0000"            /* ................ */
	$"0000 0000 0000 0000 0000 0000 0000 0000"            /* ................ */
	$"0000 0000 0000 0000 0000 0000 0000 0000"            /* ................ */
	$"0000 0000 0000 0000 0000 0000 0000 0000"            /* ................ */
	$"0000 0000 0000 0000 0000 0000 0000 0000"            /* ................ */
	$"0000 0000 0000 0000 0000 0000 0000 0000"            /* ................ */
	$"0000 0000 0000 0000 0000 0000 0000 0000"            /* ................ */
	$"0000 0000 0000 0000 0000 0000 0000 0000"            /* ................ */
	$"0000 0000 0000 0000 0000 0000 0000 0000"            /* ................ */
	$"0000 0000 0000 0000 0000 0000 0000 0000"            /* ................ */
	$"0000 0000 0000 0000 0000 0000 0000 0000"            /* ................ */
	$"0000 0000 0000 0000 0000 0000 0000 0000"            /* ................ */
	$"0000 0000 0000 0000 0000 0000 0000 0000"            /* ................ */
	$"0000 0000 0000 0000 0000 0000 0000 0000"            /* ................ */
	$"0000 0000 0000 0000 0000 0000 0000 0000"            /* ................ */
	$"0000 0000 0000 0000 0000 0000 0000 0000"            /* ................ */
	$"0000 0000 0000 0000 0000 0000 0000 0000"            /* ................ */
	$"0000 0000 0000 0000 0000 0000 0000 0000"            /* ................ */
	$"0000 0000 0000 0000 0000 0000 0000 0000"            /* ................ */
	$"0000 0000 0000 0000 0000 0000 0000 0000"            /* ................ */
	$"0000 0000 0000 0000 0000 0000 0000 0000"            /* ................ */
	$"0000 0000 0000 0000 0000 0000 0000 0000"            /* ................ */
	$"0000 0000 0000 0000 0000 0000 0000 0000"            /* ................ */
	$"0000 0000 0000 0000 0000 0000 0000 0000"            /* ................ */
	$"0000 0000 0000 0000 0000 0000 0000 0000"            /* ................ */
	$"0000 0000 0000 0000 0000 0000 0000 0000"            /* ................ */
	$"0000 0000 0000 0000 0000 0000 0000 0000"            /* ................ */
	$"0000 0000 0000 0000 0000 0000 0000 0000"            /* ................ */
	$"0000 0000 0000 0000 0000 0000 0000 0000"            /* ................ */
	$"0000 0000 0000 0000 0000 0000 0000 0000"            /* ................ */
	$"0000 0000 0000 0000 0000 0000 0000 0000"            /* ................ */
	$"0000 0000 0000 0000 0000 0000 0000 0000"            /* ................ */
	$"0000 0000 0000 0000 0000 0000 0000 0000"            /* ................ */
	$"0000 0000 0000 0000 0000 0000 0000 0000"            /* ................ */
	$"0000 0000 0000 0000 0000 0000 0000 0000"            /* ................ */
	$"0000 0000 0000 0000 0000 0000 0000 0000"            /* ................ */
	$"0000 0000 0000 0000 0000 0000 0000 0000"            /* ................ */
	$"0000 0000 0000 0000 0000 0000 0000 0000"            /* ................ */
	$"0000 0000 0000 0000 0000 0000 0000 0000"            /* ................ */
	$"0000 0000 0000 0000 0000 0000 0000 0000"            /* ................ */
	$"0000 0000 0000 0000 0000 0000 0000 0000"            /* ................ */
	$"0000 0000 0000 0000 0000 0000 0000 0000"            /* ................ */
	$"0000"                                               /* .. */
};

data 'Mcmd' (162) {
	$"0007 0000 0000 0000 0000 0000 0000 0000"            /* ................ */
	$"0000 0000 0000 0000 0000 0000 0000"                 /* .............. */
};

data 'Mcmd' (175) {
	$"0006 0000 0000 0000 0000 0000 0000 0000"            /* ................ */
	$"0000 0000 0000 0000 0000"                           /* .......... */
};

data 'Mcmd' (177) {
	$"0007 0000 0000 0000 0000 0000 0000 0000"            /* ................ */
	$"0000 0000 0000 0000 0000 0000 0000"                 /* .............. */
};

data 'Mcmd' (178) {
	$"0009 0000 0000 0000 0000 0000 0000 0000"            /* .�.............. */
	$"0000 0000 0000 0000 0000 0000 0000 0000"            /* ................ */
	$"0000 0000 0000"                                     /* ...... */
};

data 'Mcmd' (mInternetMenu) {
	$"0007 0000 0000 0000 0000 0000 0000 0000"            /* ................ */
	$"0000 0000 0000 0000 0000 0000 0000"                 /* .............. */
};

data 'Mcmd' (181) {
	$"0000"                                               /* .. */
};

