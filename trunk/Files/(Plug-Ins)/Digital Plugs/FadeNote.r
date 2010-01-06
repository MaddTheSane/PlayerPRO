#include <Carbon/Carbon.r>

resource 'DITL' (128) {
	{	/* array DITLarray: 8 elements */
		/* [1] */
		{10, 130, 27, 188},
		Button {
			enabled,
			"OK"
		},
		/* [2] */
		{40, 130, 57, 188},
		Button {
			enabled,
			"Cancel"
		},
		/* [3] */
		{10, 60, 26, 92},
		EditText {
			enabled,
			""
		},
		/* [4] */
		{40, 60, 56, 92},
		EditText {
			enabled,
			""
		},
		/* [5] */
		{10, 10, 26, 54},
		StaticText {
			disabled,
			"From:"
		},
		/* [6] */
		{40, 10, 56, 54},
		StaticText {
			disabled,
			"To:"
		},
		/* [7] */
		{12, 100, 25, 120},
		Picture {
			enabled,
			128
		},
		/* [8] */
		{42, 100, 55, 120},
		Picture {
			enabled,
			128
		}
	}
};

data 'DLGX' (128) {
	$"0843 6861 7263 6F61 6C00 0000 0000 0000"            /* .Charcoal....... */
	$"0000 0000 0000 0000 0000 0000 0000 0000"            /* ................ */
	$"0000 0000 0000 0000 0000 0000 0000 0000"            /* ................ */
	$"0000 0000 0000 0000 0000 0000 0000 0000"            /* ................ */
	$"000C 0000 0000 0000 0004 0004 0000 0000"            /* ................ */
	$"0008 0000 0000 0000 0000 0000 0000 0001"            /* ................ */
	$"0000 0000 0000 0000 0000 0007 0000 0000"            /* ................ */
	$"0000 0000 0000 0007 0000 0000 0000 0000"            /* ................ */
	$"0000 0006 0000 0000 0000 0000 0000 0006"            /* ................ */
	$"0000 0000 0000 0000 0000 0009 0000 0000"            /* ...........�.... */
	$"0000 0000 0000 0009 0000 0000 0000 0000"            /* .......�........ */
	$"0000"                                               /* .. */
};

resource 'DLOG' (128) {
	{84, 134, 151, 340},
	movableDBoxProc,
	invisible,
	goAway,
	0x0,
	128,
	"Note Fade",
	centerMainScreen
};

data 'MDEF' (1972, locked, preload) {
	$"600E 0000 4D44 4546 07B4 0000 0000 0000"            /* `...MDEF.�...... */
	$"41FA FFEE 4E71 4E71 6000 006A 4E56 0000"            /* A���NqNq`..jNV.. */
	$"48E7 0730 3E2E 0010 266E 0008 246E 000C"            /* H�.0>...&n..$n.. */
	$"5347 2007 48C0 81FC 000C C1FC 000C 3C07"            /* SG .H���..��..<. */
	$"9C40 2A07 48C5 8BFC 000C 7019 C1C6 D06A"            /* �@*.Hŋ�..p.���j */
	$"0002 3740 0002 7001 D046 C1FC 0019 D06A"            /* ..7@..p.�F��..�j */
	$"0002 3740 0006 700B C1C5 D052 3680 7001"            /* ..7@..p.���R6�p. */
	$"D045 C1FC 000B D052 3740 0004 4CDF 0CE0"            /* �E��..�R7@..L�.� */
	$"4E5E 4E75 4E56 FEEE 48E7 1F30 266E 0008"            /* N^NuNV��H�.0&n.. */
	$"246E 0010 4267 2F2E 0014 A950 3C1F 3F3C"            /* $n..Bg/...�P<.?< */
	$"0004 A887 3F3C 0009 A88A 302E 0018 6716"            /* ..��?<.ƨ�0...g. */
	$"6B00 0192 5740 6700 012C 6A00 0188 5240"            /* k..�W@g..,j..�R@ */
	$"6A00 00D6 6044 7A01 6038 2F2E 0014 3F05"            /* j..�`Dz.`8/...?. */
	$"486E FEFE A946 3F05 2F0A 486E FEF6 4EBA"            /* Hn���F?./�Hn��N� */
	$"FF3C 536E FEFA 486E FEFF 7000 102E FEFE"            /* �<Sn��Hn��p...�� */
	$"2F00 486E FEF6 3F3C 0001 A9CE 4FEF 000A"            /* /.Hn��?<..��O�.� */
	$"5245 BC45 6CC4 6000 013C 4227 2F2E 000C"            /* RE�El�`..<B'/... */
	$"2F0A A8AD 4A1F 6604 7800 6028 382E 000C"            /* /¨�J.f.x.`(8... */
	$"9852 48C4 89FC 000B C9FC 000C 302E 000E"            /* �RHĉ�..��..0... */
	$"906A 0002 48C0 81FC 0019 D840 5244 BC44"            /* �j..H���..�@RD�D */
	$"6E02 3806 4A44 661C 3F13 2F0A 486E FEF6"            /* n.8.JDf.?./�Hn�� */
	$"4EBA FECA 486E FEF6 A8A4 4253 4FEF 000A"            /* N���Hn����BSO�.� */
	$"6000 00E2 B853 6700 00DC 3F13 2F0A 486E"            /* `..�Sg..�?./�Hn */
	$"FEF6 4EBA FEA8 486E FEF6 A8A4 3E84 2F0A"            /* ��N���Hn����>�/� */
	$"486E FEF6 4EBA FE96 486E FEF6 A8A4 3684"            /* Hn��N���Hn����6� */
	$"4FEF 0012 6000 00AE 7600 7A01 6024 2F2E"            /* O�..`..�v.z.`$/. */
	$"0014 3F05 486E FEFE A946 4267 486E FEFE"            /* ..?.Hn���FBgHn�� */
	$"A88C B65F 6C0A 4267 486E FEFE A88C 361F"            /* ���_l�BgHn����6. */
	$"5245 BC45 6CD8 206E 0014 7002 D043 2050"            /* RE�El� n..p.�C P */
	$"3140 0002 700B C1C6 206E 0014 2050 3140"            /* 1@..p.�� n.. P1@ */
	$"0004 6060 3013 5340 48C0 81FC 000C C1FC"            /* ..``0.S@H���..�� */
	$"000C 72FF D253 9240 3D41 FFFE 3E13 5347"            /* ..r��S�@=A��>.SG */
	$"48C7 8FFC 000C 700B C1C7 342E 000E 9440"            /* HǏ�..p.��4...�@ */
	$"3482 C3FC 0019 302E 000C 9041 3540 0002"            /* 4���..0...�A5@.. */
	$"2E06 5347 48C7 8FFC 000C 302A 0002 0640"            /* ..SGHǏ�..0*...@ */
	$"012C 3540 0006 7001 D047 C1FC 000B D052"            /* .,5@..p.�G��..�R */
	$"3540 0004 4267 A887 4267 A88A 4CDF 0CF8"            /* 5@..Bg��Bg��L�.� */
	$"4E5E 205F 4FEF 0012 4ED0"                           /* N^ _O�..N� */
};

data 'MENU' (141) {
	$"008D 0000 0000 07B4 0000 FFFF FFFF 0554"            /* .�.....�..����.T */
	$"6974 6C65 0100 0000 0000 00"                        /* itle....... */
};

data 'PICT' (128) {
	$"08EE 0000 0000 000D 0014 0011 02FF 0C00"            /* .�...........�.. */
	$"FFFE 0000 0048 0000 0048 0000 0000 0000"            /* ��...H...H...... */
	$"000D 0014 0000 0000 0001 000A 0000 0000"            /* ...........�.... */
	$"000D 0014 0098 8014 0000 0000 000D 0014"            /* .....��......... */
	$"0000 0000 0000 0000 0048 0000 0048 0000"            /* .........H...H.. */
	$"0000 0008 0001 0008 0000 0000 0000 0000"            /* ................ */
	$"0000 0000 0047 6AD1 0000 00FF 0000 FFFF"            /* .....Gj�...�..�� */
	$"FFFF FFFF 0001 0101 0101 0101 0002 0202"            /* ����............ */
	$"0202 0202 0003 0303 0303 0303 0004 0404"            /* ................ */
	$"0404 0404 0005 0505 0505 0505 0006 0606"            /* ................ */
	$"0606 0606 0007 0707 0707 0707 0008 0808"            /* ................ */
	$"0808 0808 0009 0909 0909 0909 000A 0A0A"            /* .....�������.��� */
	$"0A0A 0A0A 000B 0B0B 0B0B 0B0B 000C 0C0C"            /* ����............ */
	$"0C0C 0C0C 000D 0D0D 0D0D 0D0D 000E 0E0E"            /* ................ */
	$"0E0E 0E0E 000F 0F0F 0F0F 0F0F 0010 1010"            /* ................ */
	$"1010 1010 0011 1111 1111 1111 0012 1212"            /* ................ */
	$"1212 1212 0013 1313 1313 1313 0014 1414"            /* ................ */
	$"1414 1414 0015 1515 1515 1515 0016 1616"            /* ................ */
	$"1616 1616 0017 1717 1717 1717 0018 1818"            /* ................ */
	$"1818 1818 0019 1919 1919 1919 001A 1A1A"            /* ................ */
	$"1A1A 1A1A 001B 1B1B 1B1B 1B1B 001C 1C1C"            /* ................ */
	$"1C1C 1C1C 001D 1D1D 1D1D 1D1D 001E 1E1E"            /* ................ */
	$"1E1E 1E1E 001F 1F1F 1F1F 1F1F 0020 2020"            /* .............    */
	$"2020 2020 0021 2121 2121 2121 0022 2222"            /*     .!!!!!!!.""" */
	$"2222 2222 0023 2323 2323 2323 0024 2424"            /* """".#######.$$$ */
	$"2424 2424 0025 2525 2525 2525 0026 2626"            /* $$$$.%%%%%%%.&&& */
	$"2626 2626 0027 2727 2727 2727 0028 2828"            /* &&&&.'''''''.((( */
	$"2828 2828 0029 2929 2929 2929 002A 2A2A"            /* ((((.))))))).*** */
	$"2A2A 2A2A 002B 2B2B 2B2B 2B2B 002C 2C2C"            /* ****.+++++++.,,, */
	$"2C2C 2C2C 002D 2D2D 2D2D 2D2D 002E 2E2E"            /* ,,,,.-------.... */
	$"2E2E 2E2E 002F 2F2F 2F2F 2F2F 0030 3030"            /* .....///////.000 */
	$"3030 3030 0031 3131 3131 3131 0032 3232"            /* 0000.1111111.222 */
	$"3232 3232 0033 3333 3333 3333 0034 3434"            /* 2222.3333333.444 */
	$"3434 3434 0035 3535 3535 3535 0036 3636"            /* 4444.5555555.666 */
	$"3636 3636 0037 3737 3737 3737 0038 3838"            /* 6666.7777777.888 */
	$"3838 3838 0039 3939 3939 3939 003A 3A3A"            /* 8888.9999999.::: */
	$"3A3A 3A3A 003B 3B3B 3B3B 3B3B 003C 3C3C"            /* ::::.;;;;;;;.<<< */
	$"3C3C 3C3C 003D 3D3D 3D3D 3D3D 003E 3E3E"            /* <<<<.=======.>>> */
	$"3E3E 3E3E 003F 3F3F 3F3F 3F3F 0040 4040"            /* >>>>.???????.@@@ */
	$"4040 4040 0041 4141 4141 4141 0042 4242"            /* @@@@.AAAAAAA.BBB */
	$"4242 4242 0043 4343 4343 4343 0044 4444"            /* BBBB.CCCCCCC.DDD */
	$"4444 4444 0045 4545 4545 4545 0046 4646"            /* DDDD.EEEEEEE.FFF */
	$"4646 4646 0047 4747 4747 4747 0048 4848"            /* FFFF.GGGGGGG.HHH */
	$"4848 4848 0049 4949 4949 4949 004A 4A4A"            /* HHHH.IIIIIII.JJJ */
	$"4A4A 4A4A 004B 4B4B 4B4B 4B4B 004C 4C4C"            /* JJJJ.KKKKKKK.LLL */
	$"4C4C 4C4C 004D 4D4D 4D4D 4D4D 004E 4E4E"            /* LLLL.MMMMMMM.NNN */
	$"4E4E 4E4E 004F 4F4F 4F4F 4F4F 0050 5050"            /* NNNN.OOOOOOO.PPP */
	$"5050 5050 0051 5151 5151 5151 0052 5252"            /* PPPP.QQQQQQQ.RRR */
	$"5252 5252 0053 5353 5353 5353 0054 5454"            /* RRRR.SSSSSSS.TTT */
	$"5454 5454 0055 5555 5555 5555 0056 5656"            /* TTTT.UUUUUUU.VVV */
	$"5656 5656 0057 5757 5757 5757 0058 5858"            /* VVVV.WWWWWWW.XXX */
	$"5858 5858 0059 5959 5959 5959 005A 5A5A"            /* XXXX.YYYYYYY.ZZZ */
	$"5A5A 5A5A 005B 5B5B 5B5B 5B5B 005C 5C5C"            /* ZZZZ.[[[[[[[.\\\ */
	$"5C5C 5C5C 005D 5D5D 5D5D 5D5D 005E 5E5E"            /* \\\\.]]]]]]].^^^ */
	$"5E5E 5E5E 005F 5F5F 5F5F 5F5F 0060 6060"            /* ^^^^._______.``` */
	$"6060 6060 0061 6161 6161 6161 0062 6262"            /* ````.aaaaaaa.bbb */
	$"6262 6262 0063 6363 6363 6363 0064 6464"            /* bbbb.ccccccc.ddd */
	$"6464 6464 0065 6565 6565 6565 0066 6666"            /* dddd.eeeeeee.fff */
	$"6666 6666 0067 6767 6767 6767 0068 6868"            /* ffff.ggggggg.hhh */
	$"6868 6868 0069 6969 6969 6969 006A 6A6A"            /* hhhh.iiiiiii.jjj */
	$"6A6A 6A6A 006B 6B6B 6B6B 6B6B 006C 6C6C"            /* jjjj.kkkkkkk.lll */
	$"6C6C 6C6C 006D 6D6D 6D6D 6D6D 006E 6E6E"            /* llll.mmmmmmm.nnn */
	$"6E6E 6E6E 006F 6F6F 6F6F 6F6F 0070 7070"            /* nnnn.ooooooo.ppp */
	$"7070 7070 0071 7171 7171 7171 0072 7272"            /* pppp.qqqqqqq.rrr */
	$"7272 7272 0073 7373 7373 7373 0074 7474"            /* rrrr.sssssss.ttt */
	$"7474 7474 0075 7575 7575 7575 0076 7676"            /* tttt.uuuuuuu.vvv */
	$"7676 7676 0077 7777 7777 7777 0078 7878"            /* vvvv.wwwwwww.xxx */
	$"7878 7878 0079 7979 7979 7979 007A 7A7A"            /* xxxx.yyyyyyy.zzz */
	$"7A7A 7A7A 007B 7B7B 7B7B 7B7B 007C 7C7C"            /* zzzz.{{{{{{{.||| */
	$"7C7C 7C7C 007D 7D7D 7D7D 7D7D 007E 7E7E"            /* ||||.}}}}}}}.~~~ */
	$"7E7E 7E7E 007F 7F7F 7F7F 7F7F 0080 8080"            /* ~~~~.........��� */
	$"8080 8080 0081 8181 8181 8181 0082 8282"            /* ����.�������.��� */
	$"8282 8282 0083 8383 8383 8383 0084 8484"            /* ����.�������.��� */
	$"8484 8484 0085 8585 8585 8585 0086 8686"            /* ����.�������.��� */
	$"8686 8686 0087 8787 8787 8787 0088 8888"            /* ����.�������.��� */
	$"8888 8888 0089 8989 8989 8989 008A 8A8A"            /* ����.�������.��� */
	$"8A8A 8A8A 008B 8B8B 8B8B 8B8B 008C 8C8C"            /* ����.�������.��� */
	$"8C8C 8C8C 008D 8D8D 8D8D 8D8D 008E 8E8E"            /* ����.�������.��� */
	$"8E8E 8E8E 008F 8F8F 8F8F 8F8F 0090 9090"            /* ����.�������.��� */
	$"9090 9090 0091 9191 9191 9191 0092 9292"            /* ����.�������.��� */
	$"9292 9292 0093 9393 9393 9393 0094 9494"            /* ����.�������.��� */
	$"9494 9494 0095 9595 9595 9595 0096 9696"            /* ����.�������.��� */
	$"9696 9696 0097 9797 9797 9797 0098 9898"            /* ����.�������.��� */
	$"9898 9898 0099 9999 9999 9999 009A 9A9A"            /* ����.�������.��� */
	$"9A9A 9A9A 009B 9B9B 9B9B 9B9B 009C 9C9C"            /* ����.�������.��� */
	$"9C9C 9C9C 009D 9D9D 9D9D 9D9D 009E 9E9E"            /* ����.�������.��� */
	$"9E9E 9E9E 009F 9F9F 9F9F 9F9F 00A0 A0A0"            /* ����.�������.��� */
	$"A0A0 A0A0 00A1 A1A1 A1A1 A1A1 00A2 A2A2"            /* ����.�������.��� */
	$"A2A2 A2A2 00A3 A3A3 A3A3 A3A3 00A4 A4A4"            /* ����.�������.��� */
	$"A4A4 A4A4 00A5 A5A5 A5A5 A5A5 00A6 A6A6"            /* ����.�������.��� */
	$"A6A6 A6A6 00A7 A7A7 A7A7 A7A7 00A8 A8A8"            /* ����.�������.��� */
	$"A8A8 A8A8 00A9 A9A9 A9A9 A9A9 00AA AAAA"            /* ����.�������.��� */
	$"AAAA AAAA 00AB ABAB ABAB ABAB 00AC ACAC"            /* ����.�������.��� */
	$"ACAC ACAC 00AD ADAD ADAD ADAD 00AE AEAE"            /* ����.�������.��� */
	$"AEAE AEAE 00AF AFAF AFAF AFAF 00B0 B0B0"            /* ����.�������.��� */
	$"B0B0 B0B0 00B1 B1B1 B1B1 B1B1 00B2 B2B2"            /* ����.�������.��� */
	$"B2B2 B2B2 00B3 B3B3 B3B3 B3B3 00B4 B4B4"            /* ����.�������.��� */
	$"B4B4 B4B4 00B5 B5B5 B5B5 B5B5 00B6 B6B6"            /* ����.�������.��� */
	$"B6B6 B6B6 00B7 B7B7 B7B7 B7B7 00B8 B8B8"            /* ����.�������.��� */
	$"B8B8 B8B8 00B9 B9B9 B9B9 B9B9 00BA BABA"            /* ����.�������.��� */
	$"BABA BABA 00BB BBBB BBBB BBBB 00BC BCBC"            /* ����.�������.��� */
	$"BCBC BCBC 00BD BDBD BDBD BDBD 00BE BEBE"            /* ����.�������.��� */
	$"BEBE BEBE 00BF BFBF BFBF BFBF 00C0 C0C0"            /* ����.�������.��� */
	$"C0C0 C0C0 00C1 C1C1 C1C1 C1C1 00C2 C2C2"            /* ����.�������.��� */
	$"C2C2 C2C2 00C3 C3C3 C3C3 C3C3 00C4 C4C4"            /* ����.�������.��� */
	$"C4C4 C4C4 00C5 C5C5 C5C5 C5C5 00C6 C6C6"            /* ����.�������.��� */
	$"C6C6 C6C6 00C7 C7C7 C7C7 C7C7 00C8 C8C8"            /* ����.�������.��� */
	$"C8C8 C8C8 00C9 C9C9 C9C9 C9C9 00CA CACA"            /* ����.�������.��� */
	$"CACA CACA 00CB CBCB CBCB CBCB 00CC CCCC"            /* ����.�������.��� */
	$"CCCC CCCC 00CD CDCD CDCD CDCD 00CE CECE"            /* ����.�������.��� */
	$"CECE CECE 00CF CFCF CFCF CFCF 00D0 D0D0"            /* ����.�������.��� */
	$"D0D0 D0D0 00D1 D1D1 D1D1 D1D1 00D2 D2D2"            /* ����.�������.��� */
	$"D2D2 D2D2 00D3 D3D3 D3D3 D3D3 00D4 D4D4"            /* ����.�������.��� */
	$"D4D4 D4D4 00D5 D5D5 D5D5 D5D5 00D6 D6D6"            /* ����.�������.��� */
	$"D6D6 D6D6 00D7 D7D7 D7D7 D7D7 00D8 D8D8"            /* ����.�������.��� */
	$"D8D8 D8D8 00D9 D9D9 D9D9 D9D9 00DA DADA"            /* ����.�������.��� */
	$"DADA DADA 00DB DBDB DBDB DBDB 00DC DCDC"            /* ����.�������.��� */
	$"DCDC DCDC 00DD DDDD DDDD DDDD 00DE DEDE"            /* ����.�������.��� */
	$"DEDE DEDE 00DF DFDF DFDF DFDF 00E0 E0E0"            /* ����.�������.��� */
	$"E0E0 E0E0 00E1 E1E1 E1E1 E1E1 00E2 E2E2"            /* ����.�������.��� */
	$"E2E2 E2E2 00E3 E3E3 E3E3 E3E3 00E4 E4E4"            /* ����.�������.��� */
	$"E4E4 E4E4 00E5 E5E5 E5E5 E5E5 00E6 E6E6"            /* ����.�������.��� */
	$"E6E6 E6E6 00E7 E7E7 E7E7 E7E7 00E8 E8E8"            /* ����.�������.��� */
	$"E8E8 E8E8 00E9 E9E9 E9E9 E9E9 00EA EAEA"            /* ����.�������.��� */
	$"EAEA EAEA 00EB EBEB EBEB EBEB 00EC ECEC"            /* ����.�������.��� */
	$"ECEC ECEC 00ED EDED EDED EDED 00EE EEEE"            /* ����.�������.��� */
	$"EEEE EEEE 00EF EFEF EFEF EFEF 00F0 F0F0"            /* ����.�������.��� */
	$"F0F0 F0F0 00F1 F1F1 F1F1 F1F1 00F2 F2F2"            /* ����.�������.��� */
	$"F2F2 F2F2 00F3 F3F3 F3F3 F3F3 00F4 F4F4"            /* ����.�������.��� */
	$"F4F4 F4F4 00F5 F5F5 F5F5 F5F5 00F6 F6F6"            /* ����.�������.��� */
	$"F6F6 F6F6 00F7 F7F7 F7F7 F7F7 00F8 F8F8"            /* ����.�������.��� */
	$"F8F8 F8F8 00F9 F9F9 F9F9 F9F9 00FA FAFA"            /* ����.�������.��� */
	$"FAFA FAFA 00FB FBFB FBFB FBFB 00FC FCFC"            /* ����.�������.��� */
	$"FCFC FCFC 00FD FDFD FDFD FDFD 00FE FEFE"            /* ����.�������.��� */
	$"FEFE FEFE 00FF 0000 0000 0000 0000 0000"            /* ����.�.......... */
	$"000D 0014 0000 0000 000D 0014 0000 04EE"            /* ...............� */
	$"FF00 AA07 00FF F000 0105 AA06 00FF F000"            /* �.�..��...�..��. */
	$"FFFF 0A00 FFFE 00F6 FFFE 00FF FF0A 00FF"            /* ���.��.���.���.� */
	$"FD00 F8FF FD00 FFFF 0B00 FFFC 00FA FFFC"            /* �.���.��..��.��� */
	$"0001 FF09 0A00 FFFB 00FC FFFB 00FF FF0A"            /* ..���.��.���.��� */
	$"00FF FA00 FEFF FA00 FFFF 0A00 FFF9 0000"            /* .��.���.���.��.. */
	$"FFF9 00FF FF06 00FF F000 FFFF 0600 FFF0"            /* ��.��..��.��..�� */
	$"00FF FF02 EDFF 04FF AAEF FF00 00FF"                 /* .��.��.����..� */
};

resource 'STR#' (1000) {
	{	/* array StringArray: 1 elements */
		/* [1] */
		"Fade Note"
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
	$"0000 0000 0000 0000 0000 0000 0000 0000"            /* ................ */
};

