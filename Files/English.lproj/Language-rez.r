#include <Carbon/Carbon.r>

data 'TEXT' (200, "Arpeggio") {
	$"4172 7065 6767 696F 0D57 6865 7265 205B"            /* Arpeggio.Where [ */
	$"305D 5B78 5D5B 795D 206D 6561 6E73 2022"            /* 0][x][y] means " */
	$"706C 6179 206E 6F74 652C 206E 6F74 652B"            /* play note, note+ */
	$"7820 7365 6D69 746F 6E65 732C 206E 6F74"            /* x semitones, not */
	$"652B 7920 7365 6D69 746F 6E65 732C 2074"            /* e+y semitones, t */
	$"6865 6E20 7265 7475 726E 2074 6F20 6F72"            /* hen return to or */
	$"6967 696E 616C 206E 6F74 6522 2E20 5468"            /* iginal note". Th */
	$"6520 666C 7563 7475 6174 696F 6E73 2061"            /* e fluctuations a */
	$"7265 2063 6172 7269 6564 206F 7574 2065"            /* re carried out e */
	$"7665 6E6C 7920 7370 6163 6564 2069 6E20"            /* venly spaced in  */
	$"6F6E 6520 7061 7474 6572 6E20 6469 7669"            /* one pattern divi */
	$"7369 6F6E 2E20 5468 6579 2061 7265 2075"            /* sion. They are u */
	$"7375 616C 6C79 2075 7365 6420 746F 2073"            /* sually used to s */
	$"696D 756C 6174 6520 6368 6F72 6473 2C20"            /* imulate chords,  */
	$"6275 7420 7468 6973 2064 6F65 736E 2774"            /* but this doesn't */
	$"2077 6F72 6B20 746F 6F20 7765 6C6C 2E20"            /*  work too well.  */
	$"5468 6579 2061 7265 2061 6C73 6F20 7573"            /* They are also us */
	$"6564 2074 6F20 7072 6F64 7563 6520 6865"            /* ed to produce he */
	$"6176 7920 7669 6272 6174 6F2E 2041 206D"            /* avy vibrato. A m */
	$"616A 6F72 2063 686F 7264 2069 7320 7768"            /* ajor chord is wh */
	$"656E 2078 3D34 2C20 793D 372E 2041 206D"            /* en x=4, y=7. A m */
	$"696E 6F72 2063 686F 7264 2069 7320 7768"            /* inor chord is wh */
	$"656E 2078 3D33 2C20 793D 372E"                      /* en x=3, y=7. */
};

data 'TEXT' (201, "SlideUp") {
	$"536C 6964 6520 7570 0D57 6865 7265 205B"            /* Slide up.Where [ */
	$"315D 5B78 5D5B 795D 206D 6561 6E73 2022"            /* 1][x][y] means " */
	$"736D 6F6F 7468 6C79 2064 6563 7265 6173"            /* smoothly decreas */
	$"6520 7468 6520 7065 7269 6F64 206F 6620"            /* e the period of  */
	$"6375 7272 656E 7420 7361 6D70 6C65 2062"            /* current sample b */
	$"7920 782A 3136 2B79 2061 6674 6572 2065"            /* y x*16+y after e */
	$"6163 6820 7469 636B 2069 6E20 7468 6520"            /* ach tick in the  */
	$"6469 7669 7369 6F6E 222E 2054 6865 2074"            /* division". The t */
	$"6963 6B73 2F64 6976 6973 696F 6E20 6172"            /* icks/division ar */
	$"6520 7365 7420 7769 7468 2074 6865 2027"            /* e set with the ' */
	$"7365 7420 7370 6565 6427 2065 6666 6563"            /* set speed' effec */
	$"7420 2873 6565 2062 656C 6F77 292E 2049"            /* t (see below). I */
	$"6620 7468 6520 7065 7269 6F64 206F 6620"            /* f the period of  */
	$"7468 6520 6E6F 7465 2062 6569 6E67 2070"            /* the note being p */
	$"6C61 7965 6420 6973 207A 2C20 7468 656E"            /* layed is z, then */
	$"2074 6865 2066 696E 616C 2070 6572 696F"            /*  the final perio */
	$"6420 7769 6C6C 2062 6520 7A20 2D20 2878"            /* d will be z - (x */
	$"2A31 3620 2B20 7929 2A28 7469 636B 7320"            /* *16 + y)*(ticks  */
	$"2D20 3129 2E20 4173 2074 6865 2073 6C69"            /* - 1). As the sli */
	$"6465 2072 6174 6520 6465 7065 6E64 7320"            /* de rate depends  */
	$"6F6E 2074 6865 2073 7065 6564 2C20 6368"            /* on the speed, ch */
	$"616E 6769 6E67 2074 6865 2073 7065 6564"            /* anging the speed */
	$"2077 696C 6C20 6368 616E 6765 2074 6865"            /*  will change the */
	$"2073 6C69 6465 2E20 596F 7520 6361 6E6E"            /*  slide. You cann */
	$"6F74 2073 6C69 6465 2062 6579 6F6E 6420"            /* ot slide beyond  */
	$"7468 6520 6E6F 7465 2042 3320 2870 6572"            /* the note B3 (per */
	$"696F 6420 3131 3329 2E"                             /* iod 113). */
};

data 'TEXT' (206, "Vibrato + Volume Slide") {
	$"436F 6E74 696E 7565 2027 5669 6272 6174"            /* Continue 'Vibrat */
	$"6F27 2C20 6275 7420 616C 736F 2064 6F20"            /* o', but also do  */
	$"566F 6C75 6D65 2073 6C69 6465 0D57 6865"            /* Volume slide.Whe */
	$"7265 205B 365D 5B78 5D5B 795D 206D 6561"            /* re [6][x][y] mea */
	$"6E73 2022 6569 7468 6572 2073 6C69 6465"            /* ns "either slide */
	$"2074 6865 2076 6F6C 756D 6520 7570 2078"            /*  the volume up x */
	$"2A28 7469 636B 7320 2D20 3129 206F 7220"            /* *(ticks - 1) or  */
	$"736C 6964 6520 7468 6520 766F 6C75 6D65"            /* slide the volume */
	$"2064 6F77 6E20 792A 2874 6963 6B73 202D"            /*  down y*(ticks - */
	$"2031 292C 2061 7420 7468 6520 7361 6D65"            /*  1), at the same */
	$"2074 696D 6520 6173 2063 6F6E 7469 6E75"            /*  time as continu */
	$"696E 6720 7468 6520 6C61 7374 2027 5669"            /* ing the last 'Vi */
	$"6272 6174 6F27 222E 2049 7420 6973 2069"            /* brato'". It is i */
	$"6C6C 6567 616C 2066 6F72 2062 6F74 6820"            /* llegal for both  */
	$"7820 616E 6420 7920 746F 2062 6520 6E6F"            /* x and y to be no */
	$"6E2D 7A65 726F 2E20 596F 7520 6361 6E6E"            /* n-zero. You cann */
	$"6F74 2073 6C69 6465 206F 7574 7369 6465"            /* ot slide outside */
	$"2074 6865 2076 6F6C 756D 6520 7261 6E67"            /*  the volume rang */
	$"6520 302E 2E36 342E 0D"                             /* e 0..64.. */
};

data 'TEXT' (205, "Slide to note + Volume Slide") {
	$"436F 6E74 696E 7565 2027 536C 6964 6520"            /* Continue 'Slide  */
	$"746F 206E 6F74 6527 2C20 6275 7420 616C"            /* to note', but al */
	$"736F 2064 6F20 566F 6C75 6D65 2073 6C69"            /* so do Volume sli */
	$"6465 0D57 6865 7265 205B 355D 5B78 5D5B"            /* de.Where [5][x][ */
	$"795D 206D 6561 6E73 2022 6569 7468 6572"            /* y] means "either */
	$"2073 6C69 6465 2074 6865 2076 6F6C 756D"            /*  slide the volum */
	$"6520 7570 2078 2A28 7469 636B 7320 2D20"            /* e up x*(ticks -  */
	$"3129 206F 7220 736C 6964 6520 7468 6520"            /* 1) or slide the  */
	$"766F 6C75 6D65 2064 6F77 6E20 792A 2874"            /* volume down y*(t */
	$"6963 6B73 202D 2031 292C 2061 7420 7468"            /* icks - 1), at th */
	$"6520 7361 6D65 2074 696D 6520 6173 2063"            /* e same time as c */
	$"6F6E 7469 6E75 696E 6720 7468 6520 6C61"            /* ontinuing the la */
	$"7374 2027 536C 6964 6520 746F 206E 6F74"            /* st 'Slide to not */
	$"6527 222E 2049 7420 6973 2069 6C6C 6567"            /* e'". It is illeg */
	$"616C 2066 6F72 2062 6F74 6820 7820 616E"            /* al for both x an */
	$"6420 7920 746F 2062 6520 6E6F 6E2D 7A65"            /* d y to be non-ze */
	$"726F 2E20 596F 7520 6361 6E6E 6F74 2073"            /* ro. You cannot s */
	$"6C69 6465 206F 7574 7369 6465 2074 6865"            /* lide outside the */
	$"2076 6F6C 756D 6520 7261 6E67 6520 302E"            /*  volume range 0. */
	$"2E36 342E 2054 6865 2070 6572 696F 642D"            /* .64. The period- */
	$"6C65 6E67 7468 2069 6E20 7468 6973 2063"            /* length in this c */
	$"6861 6E6E 656C 2773 2064 6976 6973 696F"            /* hannel's divisio */
	$"6E20 6973 2061 2070 6172 616D 6574 6572"            /* n is a parameter */
	$"2074 6F20 7468 6973 2065 6666 6563 742C"            /*  to this effect, */
	$"2061 6E64 2068 656E 6365 2069 7320 6E6F"            /*  and hence is no */
	$"7420 706C 6179 6564 2E"                             /* t played. */
};

data 'TEXT' (204, "Vibrato") {
	$"5669 6272 6174 6F0D 5768 6572 6520 5B34"            /* Vibrato.Where [4 */
	$"5D5B 785D 5B79 5D20 6D65 616E 7320 226F"            /* ][x][y] means "o */
	$"7363 696C 6C61 7465 2074 6865 2073 616D"            /* scillate the sam */
	$"706C 6520 7069 7463 6820 7573 696E 6720"            /* ple pitch using  */
	$"6120 7061 7274 6963 756C 6172 2077 6176"            /* a particular wav */
	$"6566 6F72 6D20 7769 7468 2061 6D70 6C69"            /* eform with ampli */
	$"7475 6465 2079 2F31 3620 7365 6D69 746F"            /* tude y/16 semito */
	$"6E65 732C 2073 7563 6820 7468 6174 2028"            /* nes, such that ( */
	$"7820 2A20 7469 636B 7329 2F36 3420 6379"            /* x * ticks)/64 cy */
	$"636C 6573 206F 6363 7572 2069 6E20 7468"            /* cles occur in th */
	$"6520 6469 7669 7369 6F6E 222E 2054 6865"            /* e division". The */
	$"2077 6176 6566 6F72 6D20 6973 2073 6574"            /*  waveform is set */
	$"2075 7369 6E67 2065 6666 6563 7420 5B31"            /*  using effect [1 */
	$"345D 5B34 5D2E 2042 7920 706C 6163 696E"            /* 4][4]. By placin */
	$"6720 7669 6272 6174 6F20 6566 6665 6374"            /* g vibrato effect */
	$"7320 6F6E 2063 6F6E 7365 6375 7469 7665"            /* s on consecutive */
	$"2064 6976 6973 696F 6E73 2C20 7468 6520"            /*  divisions, the  */
	$"7669 6272 6174 6F20 6566 6665 6374 2063"            /* vibrato effect c */
	$"616E 2062 6520 6D61 696E 7461 696E 6564"            /* an be maintained */
	$"2E20 4966 2065 6974 6865 7220 7820 6F72"            /* . If either x or */
	$"2079 2061 7265 2030 2C20 7468 656E 2074"            /*  y are 0, then t */
	$"6865 206F 6C64 2076 6962 7261 746F 2076"            /* he old vibrato v */
	$"616C 7565 7320 7769 6C6C 2062 6520 7573"            /* alues will be us */
	$"6564 2E"                                            /* ed. */
};

data 'TEXT' (203, "Slide to Note") {
	$"536C 6964 6520 746F 206E 6F74 650D 5768"            /* Slide to note.Wh */
	$"6572 6520 5B33 5D5B 785D 5B79 5D20 6D65"            /* ere [3][x][y] me */
	$"616E 7320 2273 6D6F 6F74 686C 7920 6368"            /* ans "smoothly ch */
	$"616E 6765 2074 6865 2070 6572 696F 6420"            /* ange the period  */
	$"6F66 2063 7572 7265 6E74 2073 616D 706C"            /* of current sampl */
	$"6520 6279 2078 2A31 362B 7920 6166 7465"            /* e by x*16+y afte */
	$"7220 6561 6368 2074 6963 6B20 696E 2074"            /* r each tick in t */
	$"6865 2064 6976 6973 696F 6E2C 206E 6576"            /* he division, nev */
	$"6572 2073 6C69 6469 6E67 2062 6579 6F6E"            /* er sliding beyon */
	$"6420 6375 7272 656E 7420 7065 7269 6F64"            /* d current period */
	$"222E 2054 6865 2070 6572 696F 642D 6C65"            /* ". The period-le */
	$"6E67 7468 2069 6E20 7468 6973 2063 6861"            /* ngth in this cha */
	$"6E6E 656C 2773 2064 6976 6973 696F 6E20"            /* nnel's division  */
	$"6973 2061 2070 6172 616D 6574 6572 2074"            /* is a parameter t */
	$"6F20 7468 6973 2065 6666 6563 742C 2061"            /* o this effect, a */
	$"6E64 2068 656E 6365 2069 7320 6E6F 7420"            /* nd hence is not  */
	$"706C 6179 6564 2E20 536C 6964 696E 6720"            /* played. Sliding  */
	$"746F 2061 206E 6F74 6520 6973 2073 696D"            /* to a note is sim */
	$"696C 6172 2074 6F20 6566 6665 6374 7320"            /* ilar to effects  */
	$"5B31 5D20 616E 6420 5B32 5D2C 2062 7574"            /* [1] and [2], but */
	$"2074 6865 2073 6C69 6465 2077 696C 6C20"            /*  the slide will  */
	$"6E6F 7420 676F 2062 6579 6F6E 6420 7468"            /* not go beyond th */
	$"6520 6769 7665 6E20 7065 7269 6F64 2C20"            /* e given period,  */
	$"616E 6420 7468 6520 6469 7265 6374 696F"            /* and the directio */
	$"6E20 6973 2069 6D70 6C69 6564 2062 7920"            /* n is implied by  */
	$"7468 6174 2070 6572 696F 642E 2049 6620"            /* that period. If  */
	$"7820 616E 6420 7920 6172 6520 626F 7468"            /* x and y are both */
	$"2030 2C20 7468 656E 2074 6865 206F 6C64"            /*  0, then the old */
	$"2073 6C69 6465 2077 696C 6C20 636F 6E74"            /*  slide will cont */
	$"696E 7565 2E"                                       /* inue. */
};

data 'TEXT' (202, "Slide Down") {
	$"536C 6964 6520 646F 776E 0D57 6865 7265"            /* Slide down.Where */
	$"205B 325D 5B78 5D5B 795D 206D 6561 6E73"            /*  [2][x][y] means */
	$"2022 736D 6F6F 7468 6C79 2069 6E63 7265"            /*  "smoothly incre */
	$"6173 6520 7468 6520 7065 7269 6F64 206F"            /* ase the period o */
	$"6620 6375 7272 656E 7420 7361 6D70 6C65"            /* f current sample */
	$"2062 7920 782A 3136 2B79 2061 6674 6572"            /*  by x*16+y after */
	$"2065 6163 6820 7469 636B 2069 6E20 7468"            /*  each tick in th */
	$"6520 6469 7669 7369 6F6E 222E 2053 696D"            /* e division". Sim */
	$"696C 6172 2074 6F20 5B31 5D2C 2062 7574"            /* ilar to [1], but */
	$"206C 6F77 6572 7320 7468 6520 7069 7463"            /*  lowers the pitc */
	$"682E 2059 6F75 2063 616E 6E6F 7420 736C"            /* h. You cannot sl */
	$"6964 6520 6265 796F 6E64 2074 6865 206E"            /* ide beyond the n */
	$"6F74 6520 4331 2028 7065 7269 6F64 2038"            /* ote C1 (period 8 */
	$"3536 292E"                                          /* 56). */
};

data 'TEXT' (207, "Tremolo") {
	$"5472 656D 6F6C 6F0D 5768 6572 6520 5B37"            /* Tremolo.Where [7 */
	$"5D5B 785D 5B79 5D20 6D65 616E 7320 226F"            /* ][x][y] means "o */
	$"7363 696C 6C61 7465 2074 6865 2073 616D"            /* scillate the sam */
	$"706C 6520 766F 6C75 6D65 2075 7369 6E67"            /* ple volume using */
	$"2061 2070 6172 7469 6375 6C61 7220 7761"            /*  a particular wa */
	$"7665 666F 726D 2077 6974 6820 616D 706C"            /* veform with ampl */
	$"6974 7564 6520 792A 2874 6963 6B73 202D"            /* itude y*(ticks - */
	$"2031 292C 2073 7563 6820 7468 6174 2028"            /*  1), such that ( */
	$"7820 2A20 7469 636B 7329 2F36 3420 6379"            /* x * ticks)/64 cy */
	$"636C 6573 206F 6363 7572 2069 6E20 7468"            /* cles occur in th */
	$"6520 6469 7669 7369 6F6E 222E 2054 6865"            /* e division". The */
	$"2077 6176 6566 6F72 6D20 6973 2073 6574"            /*  waveform is set */
	$"2075 7369 6E67 2065 6666 6563 7420 5B31"            /*  using effect [1 */
	$"345D 5B37 5D2E 2053 696D 696C 6172 2074"            /* 4][7]. Similar t */
	$"6F20 5B34 5D2E"                                     /* o [4]. */
};

data 'TEXT' (208, "Set Panning") {
	$"5365 7420 7061 6E6E 696E 670D 5768 6572"            /* Set panning.Wher */
	$"6520 5B43 5D5B 785D 5B79 5D20 6D65 616E"            /* e [C][x][y] mean */
	$"7320 2273 6574 2063 7572 7265 6E74 2073"            /* s "set current s */
	$"616D 706C 6527 7320 7061 6E6E 696E 6720"            /* ample's panning  */
	$"746F 2078 2A31 362B 7922 2E20 4C65 6761"            /* to x*16+y". Lega */
	$"6C20 7661 6C75 6573 2061 7265 2030 2E2E"            /* l values are 0.. */
	$"4646 2E20 4D69 646C 6520 3A20 3830 2C20"            /* FF. Midle : 80,  */
	$"4C65 6674 203A 2030 302C 2052 6967 6874"            /* Left : 00, Right */
	$"203A 2046 46"                                       /*  : FF */
};

data 'TEXT' (215, "Set speed") {
	$"5365 7420 7370 6565 640D 5768 6572 6520"            /* Set speed.Where  */
	$"5B46 5D5B 785D 5B79 5D20 6D65 616E 7320"            /* [F][x][y] means  */
	$"2273 6574 2073 7065 6564 2074 6F20 782A"            /* "set speed to x* */
	$"3136 2B79 222E 2054 686F 7567 6820 6974"            /* 16+y". Though it */
	$"2069 7320 6E6F 7768 6572 6520 6E65 6172"            /*  is nowhere near */
	$"2074 6861 7420 7369 6D70 6C65 2E20 4C65"            /*  that simple. Le */
	$"7420 7A20 3D20 782A 3136 2B79 2E20 4465"            /* t z = x*16+y. De */
	$"7065 6E64 696E 6720 6F6E 2077 6861 7420"            /* pending on what  */
	$"7661 6C75 6573 207A 2074 616B 6573 2C20"            /* values z takes,  */
	$"6469 6666 6572 656E 7420 756E 6974 7320"            /* different units  */
	$"6F66 2073 7065 6564 2061 7265 2073 6574"            /* of speed are set */
	$"2C20 7468 6572 6520 6265 696E 6720 7477"            /* , there being tw */
	$"6F3A 2074 6963 6B73 2F64 6976 6973 696F"            /* o: ticks/divisio */
	$"6E20 616E 6420 6265 6174 732F 6D69 6E75"            /* n and beats/minu */
	$"7465 2E49 6620 7A3C 3D33 322C 2074 6865"            /* te.If z<=32, the */
	$"6E20 6974 206D 6561 6E73 2022 7365 7420"            /* n it means "set  */
	$"7469 636B 732F 6469 7669 7369 6F6E 2074"            /* ticks/division t */
	$"6F20 7A22 206F 7468 6572 7769 7365 2069"            /* o z" otherwise i */
	$"7420 6D65 616E 7320 2273 6574 2062 6561"            /* t means "set bea */
	$"7473 2F6D 696E 7574 6520 746F 207A 222E"            /* ts/minute to z". */
	$"2044 6566 6175 6C74 2076 616C 7565 7320"            /*  Default values  */
	$"6172 6520 3620 7469 636B 732F 6469 7669"            /* are 6 ticks/divi */
	$"7369 6F6E 2C20 616E 6420 3132 3520 6265"            /* sion, and 125 be */
	$"6174 732F 6D69 6E75 7465 2E"                        /* ats/minute. */
};

data 'TEXT' (214, "E - Effects") {
	$"4520 4566 6665 6374 73"                             /* E Effects */
};

data 'TEXT' (213, "Pattern Break") {
	$"5061 7474 6572 6E20 4272 6561 6B0D 5768"            /* Pattern Break.Wh */
	$"6572 6520 5B44 5D5B 785D 5B79 5D20 6D65"            /* ere [D][x][y] me */
	$"616E 7320 2273 746F 7020 7468 6520 7061"            /* ans "stop the pa */
	$"7474 6572 6E20 6166 7465 7220 7468 6973"            /* ttern after this */
	$"2064 6976 6973 696F 6E2C 2061 6E64 2063"            /*  division, and c */
	$"6F6E 7469 6E75 6520 7468 6520 736F 6E67"            /* ontinue the song */
	$"2061 7420 7468 6520 6E65 7874 2070 6174"            /*  at the next pat */
	$"7465 726E 2061 7420 6469 7669 7369 6F6E"            /* tern at division */
	$"2078 2A31 302B 7922 2028 7468 6520 3130"            /*  x*10+y" (the 10 */
	$"2069 7320 6E6F 7420 6120 7479 706F 292E"            /*  is not a typo). */
	$"204C 6567 616C 2064 6976 6973 696F 6E73"            /*  Legal divisions */
	$"2061 7265 2066 726F 6D20 3020 746F 2036"            /*  are from 0 to 6 */
	$"3320 286E 6F74 6520 5072 6F74 7261 636B"            /* 3 (note Protrack */
	$"6572 2065 7863 6570 7469 6F6E 2061 626F"            /* er exception abo */
	$"7665 292E"                                          /* ve). */
};

data 'TEXT' (212, "Set volume") {
	$"5365 7420 766F 6C75 6D65 0D57 6865 7265"            /* Set volume.Where */
	$"205B 435D 5B78 5D5B 795D 206D 6561 6E73"            /*  [C][x][y] means */
	$"2022 7365 7420 6375 7272 656E 7420 7361"            /*  "set current sa */
	$"6D70 6C65 2773 2076 6F6C 756D 6520 746F"            /* mple's volume to */
	$"2078 2A31 362B 7922 2E20 4C65 6761 6C20"            /*  x*16+y". Legal  */
	$"766F 6C75 6D65 7320 6172 6520 302E 2E36"            /* volumes are 0..6 */
	$"342E"                                               /* 4. */
};

data 'TEXT' (210, "Volume slide") {
	$"566F 6C75 6D65 2073 6C69 6465 0D57 6865"            /* Volume slide.Whe */
	$"7265 205B 415D 5B78 5D5B 795D 206D 6561"            /* re [A][x][y] mea */
	$"6E73 2022 6569 7468 6572 2073 6C69 6465"            /* ns "either slide */
	$"2074 6865 2076 6F6C 756D 6520 7570 2078"            /*  the volume up x */
	$"2A28 7469 636B 7320 2D20 3129 206F 7220"            /* *(ticks - 1) or  */
	$"736C 6964 6520 7468 6520 766F 6C75 6D65"            /* slide the volume */
	$"2064 6F77 6E20 792A 2874 6963 6B73 202D"            /*  down y*(ticks - */
	$"2031 2922 2E20 4966 2062 6F74 6820 7820"            /*  1)". If both x  */
	$"616E 6420 7920 6172 6520 6E6F 6E2D 7A65"            /* and y are non-ze */
	$"726F 2C20 7468 656E 2074 6865 2079 2076"            /* ro, then the y v */
	$"616C 7565 2069 7320 6967 6E6F 7265 6420"            /* alue is ignored  */
	$"2861 7373 756D 6564 2074 6F20 6265 2030"            /* (assumed to be 0 */
	$"292E 2059 6F75 2063 616E 6E6F 7420 736C"            /* ). You cannot sl */
	$"6964 6520 6F75 7473 6964 6520 7468 6520"            /* ide outside the  */
	$"766F 6C75 6D65 2072 616E 6765 2030 2E2E"            /* volume range 0.. */
	$"3634 2E"                                            /* 64. */
};

data 'TEXT' (209, "Set sample offset") {
	$"5365 7420 7361 6D70 6C65 206F 6666 7365"            /* Set sample offse */
	$"740D 5768 6572 6520 5B39 5D5B 785D 5B79"            /* t.Where [9][x][y */
	$"5D20 6D65 616E 7320 2270 6C61 7920 7468"            /* ] means "play th */
	$"6520 7361 6D70 6C65 2066 726F 6D20 6F66"            /* e sample from of */
	$"6673 6574 2078 2A34 3039 3620 2B20 792A"            /* fset x*4096 + y* */
	$"3235 3622 2E20 5468 6520 6F66 6673 6574"            /* 256". The offset */
	$"2069 7320 6D65 6173 7572 6564 2069 6E20"            /*  is measured in  */
	$"776F 7264 732E 2049 6620 6E6F 2073 616D"            /* words. If no sam */
	$"706C 6520 6973 2067 6976 656E 2C20 7965"            /* ple is given, ye */
	$"7420 6F6E 6520 6973 2073 7469 6C6C 2070"            /* t one is still p */
	$"6C61 7969 6E67 206F 6E20 7468 6973 2063"            /* laying on this c */
	$"6861 6E6E 656C 2C20 6974 2073 686F 756C"            /* hannel, it shoul */
	$"6420 6265 2072 6574 7269 6767 6572 6564"            /* d be retriggered */
	$"2074 6F20 7468 6520 6E65 7720 6F66 6673"            /*  to the new offs */
	$"6574 2075 7369 6E67 2074 6865 2063 7572"            /* et using the cur */
	$"7265 6E74 2076 6F6C 756D 652E"                      /* rent volume. */
};

data 'TEXT' (315, "Invert loop") {
	$"496E 7665 7274 206C 6F6F 700D 5768 6572"            /* Invert loop.Wher */
	$"6520 5B45 5D5B 465D 5B78 5D20 6D65 616E"            /* e [E][F][x] mean */
	$"7320 2269 6620 7820 6973 2067 7265 6174"            /* s "if x is great */
	$"6572 2074 6861 6E20 302C 2074 6865 6E20"            /* er than 0, then  */
	$"706C 6179 2074 6865 2063 7572 7265 6E74"            /* play the current */
	$"2073 616D 706C 6527 7320 6C6F 6F70 2075"            /*  sample's loop u */
	$"7073 6964 6520 646F 776E 2061 7420 7370"            /* pside down at sp */
	$"6565 6420 7822 2E20 4561 6368 2062 7974"            /* eed x". Each byt */
	$"6520 696E 2074 6865 2073 616D 706C 6527"            /* e in the sample' */
	$"7320 6C6F 6F70 2077 696C 6C20 6861 7665"            /* s loop will have */
	$"2069 7473 2073 6967 6E20 6368 616E 6765"            /*  its sign change */
	$"6420 286E 6567 6174 6564 292E 2049 7420"            /* d (negated). It  */
	$"7769 6C6C 206F 6E6C 7920 776F 726B 2069"            /* will only work i */
	$"6620 7468 6520 7361 6D70 6C65 2773 206C"            /* f the sample's l */
	$"6F6F 7020 2864 6566 696E 6564 2070 7265"            /* oop (defined pre */
	$"7669 6F75 736C 7929 2069 7320 6E6F 7420"            /* viously) is not  */
	$"746F 6F20 6269 672E 2054 6865 2073 7065"            /* too big. The spe */
	$"6564 2069 7320 6261 7365 6420 6F6E 2061"            /* ed is based on a */
	$"6E20 696E 7465 726E 616C 2074 6162 6C65"            /* n internal table */
	$"2E"                                                 /* . */
};

data 'TEXT' (314, "Delay pattern") {
	$"4465 6C61 7920 7061 7474 6572 6E0D 5768"            /* Delay pattern.Wh */
	$"6572 6520 5B45 5D5B 455D 5B78 5D20 6D65"            /* ere [E][E][x] me */
	$"616E 7320 2261 6674 6572 2074 6869 7320"            /* ans "after this  */
	$"6469 7669 7369 6F6E 2074 6865 7265 2077"            /* division there w */
	$"696C 6C20 6265 2061 2064 656C 6179 2065"            /* ill be a delay e */
	$"7175 6976 616C 656E 7420 746F 2074 6865"            /* quivalent to the */
	$"2074 696D 6520 7461 6B65 6E20 746F 2070"            /*  time taken to p */
	$"6C61 7920 7820 6469 7669 7369 6F6E 7320"            /* lay x divisions  */
	$"6166 7465 7220 7768 6963 6820 7468 6520"            /* after which the  */
	$"7061 7474 6572 6E20 7769 6C6C 2062 6520"            /* pattern will be  */
	$"7265 7375 6D65 6422 2E20 5468 6520 6465"            /* resumed". The de */
	$"6C61 7920 6F6E 6C79 2072 656C 6174 6573"            /* lay only relates */
	$"2074 6F20 7468 6520 696E 7465 7270 7265"            /*  to the interpre */
	$"7469 6E67 206F 6620 6E65 7720 6469 7669"            /* ting of new divi */
	$"7369 6F6E 732C 2061 6E64 2061 6C6C 2065"            /* sions, and all e */
	$"6666 6563 7473 2061 6E64 2070 7265 7669"            /* ffects and previ */
	$"6F75 7320 6E6F 7465 7320 636F 6E74 696E"            /* ous notes contin */
	$"7565 2064 7572 696E 6720 6465 6C61 792E"            /* ue during delay. */
};

data 'TEXT' (313, "Delay sample") {
	$"4465 6C61 7920 7361 6D70 6C65 0D57 6865"            /* Delay sample.Whe */
	$"7265 205B 455D 5B44 5D5B 785D 206D 6561"            /* re [E][D][x] mea */
	$"6E73 2022 646F 206E 6F74 2073 7461 7274"            /* ns "do not start */
	$"2074 6869 7320 6469 7669 7369 6F6E 2773"            /*  this division's */
	$"2073 616D 706C 6520 666F 7220 7468 6520"            /*  sample for the  */
	$"6669 7273 7420 7820 7469 636B 7320 696E"            /* first x ticks in */
	$"2074 6869 7320 6469 7669 7369 6F6E 2C20"            /*  this division,  */
	$"706C 6179 2074 6865 2073 616D 706C 6520"            /* play the sample  */
	$"6166 7465 7220 7468 6973 222E 2054 6869"            /* after this". Thi */
	$"7320 696D 706C 6965 7320 7468 6174 2069"            /* s implies that i */
	$"6620 7820 6973 2030 2C20 7468 656E 2079"            /* f x is 0, then y */
	$"6F75 2077 696C 6C20 6865 6172 206E 6F20"            /* ou will hear no  */
	$"6465 6C61 792C 2062 7574 2061 6374 7561"            /* delay, but actua */
	$"6C6C 7920 7468 6572 6520 7769 6C6C 2062"            /* lly there will b */
	$"6520 6120 5645 5259 2073 6D61 6C6C 2064"            /* e a VERY small d */
	$"656C 6179 2E20 4E6F 7465 2074 6861 7420"            /* elay. Note that  */
	$"7468 6973 2065 6666 6563 7420 6F6E 6C79"            /* this effect only */
	$"2069 6E66 6C75 656E 6365 7320 6120 7361"            /*  influences a sa */
	$"6D70 6C65 2069 6620 6974 2077 6173 2073"            /* mple if it was s */
	$"7461 7274 6564 2069 6E20 7468 6973 2064"            /* tarted in this d */
	$"6976 6973 696F 6E2E"                                /* ivision. */
};

data 'TEXT' (312, "Cut sample") {
	$"4375 7420 7361 6D70 6C65 0D57 6865 7265"            /* Cut sample.Where */
	$"205B 455D 5B43 5D5B 785D 206D 6561 6E73"            /*  [E][C][x] means */
	$"2022 6166 7465 7220 7468 6520 6375 7272"            /*  "after the curr */
	$"656E 7420 7361 6D70 6C65 2068 6173 2062"            /* ent sample has b */
	$"6565 6E20 706C 6179 6564 2066 6F72 2078"            /* een played for x */
	$"2074 6963 6B73 2069 6E20 7468 6973 2064"            /*  ticks in this d */
	$"6976 6973 696F 6E2C 2069 7473 2076 6F6C"            /* ivision, its vol */
	$"756D 6520 7769 6C6C 2062 6520 7365 7420"            /* ume will be set  */
	$"746F 2030 222E 2054 6869 7320 696D 706C"            /* to 0". This impl */
	$"6965 7320 7468 6174 2069 6620 7820 6973"            /* ies that if x is */
	$"2030 2C20 7468 656E 2079 6F75 2077 696C"            /*  0, then you wil */
	$"6C20 6E6F 7420 6865 6172 2061 6E79 206F"            /* l not hear any o */
	$"6620 7468 6520 7361 6D70 6C65 2E20 4966"            /* f the sample. If */
	$"2079 6F75 2077 6973 6820 746F 2069 6E73"            /*  you wish to ins */
	$"6572 7420 2273 696C 656E 6365 2220 696E"            /* ert "silence" in */
	$"2061 2070 6174 7465 726E 2C20 6974 2069"            /*  a pattern, it i */
	$"7320 6265 7474 6572 2074 6F20 7573 6520"            /* s better to use  */
	$"6120 2273 696C 656E 6365 222D 7361 6D70"            /* a "silence"-samp */
	$"6C65 2028 7365 6520 6162 6F76 6529 2064"            /* le (see above) d */
	$"7565 2074 6F20 7468 6520 6C61 636B 206F"            /* ue to the lack o */
	$"6620 7072 6F70 6572 2073 7570 706F 7274"            /* f proper support */
	$"2066 6F72 2074 6869 7320 6566 6665 6374"            /*  for this effect */
	$"2E"                                                 /* . */
};

data 'TEXT' (311, "Fine volume slide down") {
	$"4669 6E65 2076 6F6C 756D 6520 736C 6964"            /* Fine volume slid */
	$"6520 646F 776E 0D57 6865 7265 205B 455D"            /* e down.Where [E] */
	$"5B42 5D5B 785D 206D 6561 6E73 2022 6465"            /* [B][x] means "de */
	$"6372 656D 656E 7420 7468 6520 766F 6C75"            /* crement the volu */
	$"6D65 206F 6620 7468 6520 6375 7272 656E"            /* me of the curren */
	$"7420 7361 6D70 6C65 2062 7920 7822 2E20"            /* t sample by x".  */
	$"5369 6D69 6C61 7220 746F 205B 3134 5D5B"            /* Similar to [14][ */
	$"3130 5D20 6275 7420 6C6F 7765 7273 2076"            /* 10] but lowers v */
	$"6F6C 756D 652E 2059 6F75 2063 616E 6E6F"            /* olume. You canno */
	$"7420 736C 6964 6520 6265 796F 6E64 2076"            /* t slide beyond v */
	$"6F6C 756D 6520 302E"                                /* olume 0. */
};

data 'TEXT' (310, "Fine volume slide up") {
	$"4669 6E65 2076 6F6C 756D 6520 736C 6964"            /* Fine volume slid */
	$"6520 7570 0D57 6865 7265 205B 455D 5B41"            /* e up.Where [E][A */
	$"5D5B 785D 206D 6561 6E73 2022 696E 6372"            /* ][x] means "incr */
	$"656D 656E 7420 7468 6520 766F 6C75 6D65"            /* ement the volume */
	$"206F 6620 7468 6520 6375 7272 656E 7420"            /*  of the current  */
	$"7361 6D70 6C65 2062 7920 7822 2E20 5468"            /* sample by x". Th */
	$"6520 696E 6372 656D 656E 7469 6E67 2074"            /* e incrementing t */
	$"616B 6573 2070 6C61 6365 2061 7420 7468"            /* akes place at th */
	$"6520 6265 6769 6E6E 696E 6720 6F66 2074"            /* e beginning of t */
	$"6865 2064 6976 6973 696F 6E2C 2061 6E64"            /* he division, and */
	$"2068 656E 6365 2074 6865 7265 2069 7320"            /*  hence there is  */
	$"6E6F 2073 6C69 6469 6E67 2E20 596F 7520"            /* no sliding. You  */
	$"6361 6E6E 6F74 2073 6C69 6465 2062 6579"            /* cannot slide bey */
	$"6F6E 6420 766F 6C75 6D65 2036 342E"                 /* ond volume 64. */
};

data 'TEXT' (309, "Retrigger sample") {
	$"5265 7472 6967 6765 7220 7361 6D70 6C65"            /* Retrigger sample */
	$"0D57 6865 7265 205B 455D 5B39 5D5B 785D"            /* .Where [E][9][x] */
	$"206D 6561 6E73 2022 7472 6967 6765 7220"            /*  means "trigger  */
	$"6375 7272 656E 7420 7361 6D70 6C65 2065"            /* current sample e */
	$"7665 7279 2078 2074 6963 6B73 2069 6E20"            /* very x ticks in  */
	$"7468 6973 2064 6976 6973 696F 6E22 2E20"            /* this division".  */
	$"4966 2078 2069 7320 302C 2074 6865 6E20"            /* If x is 0, then  */
	$"6E6F 2072 6574 7269 6767 6572 696E 6720"            /* no retriggering  */
	$"6973 2064 6F6E 6520 2861 6374 7320 6173"            /* is done (acts as */
	$"2069 6620 6E6F 2065 6666 6563 7420 7761"            /*  if no effect wa */
	$"7320 6368 6F73 656E 292C 206F 7468 6572"            /* s chosen), other */
	$"7769 7365 2074 6865 2072 6574 7269 6767"            /* wise the retrigg */
	$"6572 696E 6720 6265 6769 6E73 206F 6E20"            /* ering begins on  */
	$"7468 6520 6669 7273 7420 7469 636B 2061"            /* the first tick a */
	$"6E64 2074 6865 6E20 7820 7469 636B 7320"            /* nd then x ticks  */
	$"6166 7465 7220 7468 6174 2C20 6574 632E"            /* after that, etc. */
};

data 'TEXT' (307, "Set tremolo waveform") {
	$"5365 7420 7472 656D 6F6C 6F20 7761 7665"            /* Set tremolo wave */
	$"666F 726D 0D57 6865 7265 205B 455D 5B37"            /* form.Where [E][7 */
	$"5D5B 785D 206D 6561 6E73 2022 7365 7420"            /* ][x] means "set  */
	$"7468 6520 7761 7665 666F 726D 206F 6620"            /* the waveform of  */
	$"7375 6363 6565 6469 6E67 2027 7472 656D"            /* succeeding 'trem */
	$"6F6C 6F27 2065 6666 6563 7473 2074 6F20"            /* olo' effects to  */
	$"7761 7665 2023 7822 2E20 5369 6D69 6C61"            /* wave #x". Simila */
	$"7220 746F 205B 3134 5D5B 345D 2C20 6275"            /* r to [14][4], bu */
	$"7420 616C 7465 7273 2065 6666 6563 7420"            /* t alters effect  */
	$"5B37 5D20 2D20 7468 6520 2774 7265 6D6F"            /* [7] - the 'tremo */
	$"6C6F 2720 6566 6665 6374 2E"                        /* lo' effect. */
};

data 'TEXT' (306, "Loop pattern") {
	$"4C6F 6F70 2070 6174 7465 726E 0D57 6865"            /* Loop pattern.Whe */
	$"7265 205B 455D 5B36 5D5B 785D 206D 6561"            /* re [E][6][x] mea */
	$"6E73 2022 7365 7420 7468 6520 7374 6172"            /* ns "set the star */
	$"7420 6F66 2061 206C 6F6F 7020 746F 2074"            /* t of a loop to t */
	$"6869 7320 6469 7669 7369 6F6E 2069 6620"            /* his division if  */
	$"7820 6973 2030 2C20 6F74 6865 7277 6973"            /* x is 0, otherwis */
	$"6520 6166 7465 7220 7468 6973 2064 6976"            /* e after this div */
	$"6973 696F 6E2C 206A 756D 7020 6261 636B"            /* ision, jump back */
	$"2074 6F20 7468 6520 7374 6172 7420 6F66"            /*  to the start of */
	$"2061 206C 6F6F 7020 616E 6420 706C 6179"            /*  a loop and play */
	$"2069 7420 616E 6F74 6865 7220 7820 7469"            /*  it another x ti */
	$"6D65 7320 6265 666F 7265 2063 6F6E 7469"            /* mes before conti */
	$"6E75 696E 6722 2E20 4966 2074 6865 2073"            /* nuing". If the s */
	$"7461 7274 206F 6620 7468 6520 6C6F 6F70"            /* tart of the loop */
	$"2077 6173 206E 6F74 2073 6574 2C20 6974"            /*  was not set, it */
	$"2077 696C 6C20 6465 6661 756C 7420 746F"            /*  will default to */
	$"2074 6865 2073 7461 7274 206F 6620 7468"            /*  the start of th */
	$"6520 6375 7272 656E 7420 7061 7474 6572"            /* e current patter */
	$"6E2E 2048 656E 6365 2027 6C6F 6F70 2070"            /* n. Hence 'loop p */
	$"6174 7465 726E 2720 6361 6E6E 6F74 2062"            /* attern' cannot b */
	$"6520 7065 7266 6F72 6D65 6420 6163 726F"            /* e performed acro */
	$"7373 206D 756C 7469 706C 6520 7061 7474"            /* ss multiple patt */
	$"6572 6E73 2E20 4E6F 7465 2074 6861 7420"            /* erns. Note that  */
	$"6C6F 6F70 7320 646F 206E 6F74 2073 7570"            /* loops do not sup */
	$"706F 7274 206E 6573 7469 6E67 2C20 616E"            /* port nesting, an */
	$"6420 796F 7520 6D61 7920 6765 6E65 7261"            /* d you may genera */
	$"7465 2061 6E20 696E 6669 6E69 7465 206C"            /* te an infinite l */
	$"6F6F 7020 6966 2079 6F75 2074 7279 2074"            /* oop if you try t */
	$"6F20 6E65 7374 2027 6C6F 6F70 2070 6174"            /* o nest 'loop pat */
	$"7465 726E 2773 2E"                                  /* tern's. */
};

data 'TEXT' (305, "Set finetune value") {
	$"5365 7420 6669 6E65 7475 6E65 2076 616C"            /* Set finetune val */
	$"7565 0D57 6865 7265 205B 455D 5B35 5D5B"            /* ue.Where [E][5][ */
	$"785D 206D 6561 6E73 2022 7365 7473 2074"            /* x] means "sets t */
	$"6865 2066 696E 6574 756E 6520 7661 6C75"            /* he finetune valu */
	$"6520 6F66 2074 6865 2063 7572 7265 6E74"            /* e of the current */
	$"2073 616D 706C 6520 746F 2074 6865 2073"            /*  sample to the s */
	$"6967 6E65 6420 6E69 6262 6C65 2078 222E"            /* igned nibble x". */
	$"2078 2068 6173 206C 6567 616C 2076 616C"            /*  x has legal val */
	$"7565 7320 6F66 2030 2E2E 3135 2C20 636F"            /* ues of 0..15, co */
	$"7272 6573 706F 6E64 696E 6720 746F 2073"            /* rresponding to s */
	$"6967 6E65 6420 6E69 6262 6C65 7320 302E"            /* igned nibbles 0. */
	$"2E37 2C2D 382E 2E2D 3120 2873 6565 2073"            /* .7,-8..-1 (see s */
	$"7461 7274 206F 6620 7465 7874 2066 6F72"            /* tart of text for */
	$"206D 6F72 6520 696E 666F 206F 6E20 6669"            /*  more info on fi */
	$"6E65 7475 6E65 2076 616C 7565 7329 2E"              /* netune values). */
};

data 'TEXT' (304, "Set vibrato waveform") {
	$"5365 7420 7669 6272 6174 6F20 7761 7665"            /* Set vibrato wave */
	$"666F 726D 0D57 6865 7265 205B 455D 5B34"            /* form.Where [E][4 */
	$"5D5B 785D 206D 6561 6E73 2022 7365 7420"            /* ][x] means "set  */
	$"7468 6520 7761 7665 666F 726D 206F 6620"            /* the waveform of  */
	$"7375 6363 6565 6469 6E67 2027 7669 6272"            /* succeeding 'vibr */
	$"6174 6F27 2065 6666 6563 7473 2074 6F20"            /* ato' effects to  */
	$"7761 7665 2023 7822 2E20 5B34 5D20 6973"            /* wave #x". [4] is */
	$"2074 6865 2027 7669 6272 6174 6F27 2065"            /*  the 'vibrato' e */
	$"6666 6563 742E 2020 506F 7373 6962 6C65"            /* ffect.  Possible */
	$"2076 616C 7565 7320 666F 7220 7820 6172"            /*  values for x ar */
	$"653A 2030 202D 2073 696E 6520 2864 6566"            /* e: 0 - sine (def */
	$"6175 6C74 2920 3420 2028 7769 7468 6F75"            /* ault) 4  (withou */
	$"7420 7265 7472 6967 6765 7229 2031 202D"            /* t retrigger) 1 - */
	$"2072 616D 7020 646F 776E 2035 2020 2877"            /*  ramp down 5  (w */
	$"6974 686F 7574 2072 6574 7269 6767 6572"            /* ithout retrigger */
	$"2920 3220 2D20 7371 7561 7265 2036 2020"            /* ) 2 - square 6   */
	$"2877 6974 686F 7574 2072 6574 7269 6767"            /* (without retrigg */
	$"6572 2920 3320 2D20 7261 6E64 6F6D 3A20"            /* er) 3 - random:  */
	$"6120 7261 6E64 6F6D 2063 686F 6963 6520"            /* a random choice  */
	$"6F66 206F 6E65 206F 6620 7468 6520 6162"            /* of one of the ab */
	$"6F76 652E 2037 2020 2877 6974 686F 7574"            /* ove. 7  (without */
	$"2072 6574 7269 6767 6572 292E 2049 6620"            /*  retrigger). If  */
	$"7468 6520 7761 7665 666F 726D 2069 7320"            /* the waveform is  */
	$"7365 6C65 6374 6564 2022 7769 7468 6F75"            /* selected "withou */
	$"7420 7265 7472 6967 6765 7222 2C20 7468"            /* t retrigger", th */
	$"656E 2069 7420 7769 6C6C 206E 6F74 2062"            /* en it will not b */
	$"6520 7265 7472 6967 6765 7265 6420 6672"            /* e retriggered fr */
	$"6F6D 2074 6865 2062 6567 696E 6E69 6E67"            /* om the beginning */
	$"2061 7420 7468 6520 7374 6172 7420 6F66"            /*  at the start of */
	$"2065 6163 6820 6E65 7720 6E6F 7465 2E"              /*  each new note. */
};

data 'TEXT' (303, "Set glissando on/off") {
	$"5365 7420 676C 6973 7361 6E64 6F20 6F6E"            /* Set glissando on */
	$"2F6F 6666 0D57 6865 7265 205B 455D 5B33"            /* /off.Where [E][3 */
	$"5D5B 785D 206D 6561 6E73 2022 7365 7420"            /* ][x] means "set  */
	$"676C 6973 7361 6E64 6F20 4F4E 2069 6620"            /* glissando ON if  */
	$"7820 6973 2031 2C20 4F46 4620 6966 2078"            /* x is 1, OFF if x */
	$"2069 7320 3022 2E20 5573 6564 2069 6E20"            /*  is 0". Used in  */
	$"636F 6E6A 756E 6374 696F 6E20 7769 7468"            /* conjunction with */
	$"205B 335D 2028 2753 6C69 6465 2074 6F20"            /*  [3] ('Slide to  */
	$"6E6F 7465 2729 2E20 4966 2067 6C69 7373"            /* note'). If gliss */
	$"616E 646F 2069 7320 6F6E 2C20 7468 656E"            /* ando is on, then */
	$"2027 536C 6964 6520 746F 206E 6F74 6527"            /*  'Slide to note' */
	$"2077 696C 6C20 736C 6964 6520 696E 2073"            /*  will slide in s */
	$"656D 6974 6F6E 6573 2C20 6F74 6865 7277"            /* emitones, otherw */
	$"6973 6520 7769 6C6C 2070 6572 666F 726D"            /* ise will perform */
	$"2074 6865 2064 6566 6175 6C74 2073 6D6F"            /*  the default smo */
	$"6F74 6820 736C 6964 652E"                           /* oth slide. */
};

data 'TEXT' (302, "Fineslide down") {
	$"4669 6E65 736C 6964 6520 646F 776E 0D57"            /* Fineslide down.W */
	$"6865 7265 205B 455D 5B32 5D5B 785D 206D"            /* here [E][2][x] m */
	$"6561 6E73 2022 696E 6372 656D 656E 7420"            /* eans "increment  */
	$"7468 6520 7065 7269 6F64 206F 6620 7468"            /* the period of th */
	$"6520 6375 7272 656E 7420 7361 6D70 6C65"            /* e current sample */
	$"2062 7920 7822 2E20 5369 6D69 6C61 7220"            /*  by x". Similar  */
	$"746F 205B 3134 5D5B 315D 2062 7574 2073"            /* to [14][1] but s */
	$"6869 6674 7320 7468 6520 7069 7463 6820"            /* hifts the pitch  */
	$"646F 776E 2E20 596F 7520 6361 6E6E 6F74"            /* down. You cannot */
	$"2073 6C69 6465 2062 6579 6F6E 6420 7468"            /*  slide beyond th */
	$"6520 6E6F 7465 2043 3120 2870 6572 696F"            /* e note C1 (perio */
	$"6420 3835 3629 2E"                                  /* d 856). */
};

data 'TEXT' (301, "Fineslide up") {
	$"4669 6E65 736C 6964 6520 7570 0D57 6865"            /* Fineslide up.Whe */
	$"7265 205B 455D 5B31 5D5B 785D 206D 6561"            /* re [E][1][x] mea */
	$"6E73 2022 6465 6372 656D 656E 7420 7468"            /* ns "decrement th */
	$"6520 7065 7269 6F64 206F 6620 7468 6520"            /* e period of the  */
	$"6375 7272 656E 7420 7361 6D70 6C65 2062"            /* current sample b */
	$"7920 7822 2E20 5468 6520 696E 6372 656D"            /* y x". The increm */
	$"656E 7469 6E67 2074 616B 6573 2070 6C61"            /* enting takes pla */
	$"6365 2061 7420 7468 6520 6265 6769 6E6E"            /* ce at the beginn */
	$"696E 6720 6F66 2074 6865 2064 6976 6973"            /* ing of the divis */
	$"696F 6E2C 2061 6E64 2068 656E 6365 2074"            /* ion, and hence t */
	$"6865 7265 2069 7320 6E6F 2061 6374 7561"            /* here is no actua */
	$"6C20 736C 6964 696E 672E 2059 6F75 2063"            /* l sliding. You c */
	$"616E 6E6F 7420 736C 6964 6520 6265 796F"            /* annot slide beyo */
	$"6E64 2074 6865 206E 6F74 6520 4233 2028"            /* nd the note B3 ( */
	$"7065 7269 6F64 2031 3133 292E"                      /* period 113). */
};

data 'TEXT' (300, "Set filter on/off") {
	$"5365 7420 6669 6C74 6572 206F 6E2F 6F66"            /* Set filter on/of */
	$"660D 5768 6572 6520 5B45 5D5B 305D 5B78"            /* f.Where [E][0][x */
	$"5D20 6D65 616E 7320 2273 6574 2073 6F75"            /* ] means "set sou */
	$"6E64 2066 696C 7465 7220 4F4E 2069 6620"            /* nd filter ON if  */
	$"7820 6973 2030 2C20 616E 6420 4F46 4620"            /* x is 0, and OFF  */
	$"6973 2078 2069 7320 3122 2E20 5468 6973"            /* is x is 1". This */
	$"2069 7320 6120 6861 7264 7761 7265 2063"            /*  is a hardware c */
	$"6F6D 6D61 6E64 2066 6F72 2073 6F6D 6520"            /* ommand for some  */
	$"416D 6967 6173 2C20 736F 2069 6620 796F"            /* Amigas, so if yo */
	$"7520 646F 6E27 7420 756E 6465 7273 7461"            /* u don't understa */
	$"6E64 2069 742C 2069 7420 6973 2062 6574"            /* nd it, it is bet */
	$"7465 7220 6E6F 7420 746F 2075 7365 2069"            /* ter not to use i */
	$"742E"                                               /* t. */
};

data 'TEXT' (211, "Position Jump") {
	$"506F 7369 7469 6F6E 204A 756D 700D 5768"            /* Position Jump.Wh */
	$"6572 6520 5B42 5D5B 785D 5B79 5D20 6D65"            /* ere [B][x][y] me */
	$"616E 7320 2273 746F 7020 7468 6520 7061"            /* ans "stop the pa */
	$"7474 6572 6E20 6166 7465 7220 7468 6973"            /* ttern after this */
	$"2064 6976 6973 696F 6E2C 2061 6E64 2063"            /*  division, and c */
	$"6F6E 7469 6E75 6520 7468 6520 736F 6E67"            /* ontinue the song */
	$"2061 7420 736F 6E67 2D70 6F73 6974 696F"            /*  at song-positio */
	$"6E20 782A 3136 2B79 222E 2054 6869 7320"            /* n x*16+y". This  */
	$"7368 6966 7473 2074 6865 2027 7061 7474"            /* shifts the 'patt */
	$"6572 6E2D 6375 7273 6F72 2720 696E 2074"            /* ern-cursor' in t */
	$"6865 2070 6174 7465 726E 2074 6162 6C65"            /* he pattern table */
	$"2028 7365 6520 6162 6F76 6529 2E20 4C65"            /*  (see above). Le */
	$"6761 6C20 7661 6C75 6573 2066 6F72 2078"            /* gal values for x */
	$"2A31 362B 7920 6172 6520 6672 6F6D 2030"            /* *16+y are from 0 */
	$"2074 6F20 3132 372E 2053 6565 2050 6172"            /*  to 127. See Par */
	$"7469 7469 6F6E 2057 696E 646F 7720 3A20"            /* tition Window :  */
	$"7468 6520 6172 6775 6D65 6E74 2069 7320"            /* the argument is  */
	$"706F 7320 4944 202D 312E"                           /* pos ID -1. */
};

data 'TEXT' (308, "Set Panning") {
	$"5468 6973 2065 6666 6563 7420 6C65 7473"            /* This effect lets */
	$"2079 6F75 2064 6F20 3136 2070 6F73 6974"            /*  you do 16 posit */
	$"696F 6E20 7061 6E6E 696E 672E 0D0D 4538"            /* ion panning...E8 */
	$"3020 2853 6574 2074 6865 2063 6861 6E6E"            /* 0 (Set the chann */
	$"656C 2773 2070 616E 2076 616C 7565 2074"            /* el's pan value t */
	$"6F20 7468 6520 6661 7220 6C65 6674 290D"            /* o the far left). */
	$"4538 4620 2853 6574 2074 6865 2063 6861"            /* E8F (Set the cha */
	$"6E6E 656C 2773 2070 616E 2076 616C 7565"            /* nnel's pan value */
	$"2074 6F20 7468 6520 6661 7220 7269 6768"            /*  to the far righ */
	$"7429 0D0D 5261 6E67 653A 2078 3D70 6F73"            /* t)..Range: x=pos */
	$"6974 696F 6E20 746F 2070 616E 2074 6F6F"            /* ition to pan too */
	$"2028 3068 2D30 4668 29"                             /*  (0h-0Fh) */
};

data 'TEXT' (401, "Internet Pub") {
	$"0D59 6F75 2068 6176 6520 616E 2069 6E74"            /* .You have an int */
	$"6572 6573 7469 6E67 2068 7474 7020 6164"            /* eresting http ad */
	$"6472 6573 7320 6162 6F75 7420 536F 756E"            /* dress about Soun */
	$"6454 7261 636B 6572 7320 6F72 2050 6C61"            /* dTrackers or Pla */
	$"7965 7250 524F 3F0D 596F 7520 7761 6E74"            /* yerPRO?.You want */
	$"2074 6F20 696E 636C 7564 6520 7468 6973"            /*  to include this */
	$"2061 6464 7265 7373 2069 6E20 7468 6520"            /*  address in the  */
	$"6465 6661 756C 7420 6C69 7374 3F0D 4A75"            /* default list?.Ju */
	$"7374 2073 656E 6420 6974 2074 6F20 726F"            /* st send it to ro */
	$"7373 6574 616E 746F 696E 6540 626C 7565"            /* ssetantoine@blue */
	$"7769 6E2E 6368 210D"                                /* win.ch!. */
};

data 'TEXT' (216, "Note OFF") {
	$"4E6F 7465 204F 4646 0D57 6865 7265 205B"            /* Note OFF.Where [ */
	$"475D 5B78 5D5B 795D 206D 6561 6E73 2074"            /* G][x][y] means t */
	$"6861 7420 796F 7520 646F 2061 204B 6579"            /* hat you do a Key */
	$"204F 4646 206F 6E20 6C61 7374 2069 6E73"            /*  OFF on last ins */
	$"7472 756D 656E 7420 7573 6564 2077 6974"            /* trument used wit */
	$"6820 7468 6973 2074 7261 636B 2E20 4966"            /* h this track. If */
	$"205B 785D 5B79 5D20 6973 2065 7175 616C"            /*  [x][y] is equal */
	$"2074 6F20 302C 2069 7420 6D65 616E 7320"            /*  to 0, it means  */
	$"7374 6F70 2069 6D6D 6564 6961 7465 6C79"            /* stop immediately */
	$"2074 6865 2070 7265 7669 6F75 7320 696E"            /*  the previous in */
	$"7374 7275 6D65 6E74 2E20 4966 205B 785D"            /* strument. If [x] */
	$"5B79 5D20 6973 2065 7175 616C 2074 6F20"            /* [y] is equal to  */
	$"312C 2069 7420 6D65 616E 7320 6765 6E65"            /* 1, it means gene */
	$"7261 7465 2061 204E 6F74 652D 4F66 6620"            /* rate a Note-Off  */
	$"6F6E 2074 6865 2070 7265 7669 6F75 7320"            /* on the previous  */
	$"696E 7374 7275 6D65 6E74 2E54 6869 7320"            /* instrument.This  */
	$"6566 6665 6374 2069 7320 6F6E 6C79 2075"            /* effect is only u */
	$"7365 6675 6C6C 2069 6E20 7468 6520 D24D"            /* sefull in the “M */
	$"756C 7469 2D43 6861 6E6E 656C 2054 7261"            /* ulti-Channel Tra */
	$"636B 73D3 206D 6F64 652E 2049 6620 D24D"            /* cks” mode. If “M */
	$"756C 7469 2D43 6861 6E6E 656C 2054 7261"            /* ulti-Channel Tra */
	$"636B 73D3 2069 7320 6E6F 7420 6163 7469"            /* cks” is not acti */
	$"7665 2C20 7468 6973 2065 6666 6563 7420"            /* ve, this effect  */
	$"6861 7320 6E6F 2065 6666 6563 742E"                 /* has no effect. */
};

data 'TEXT' (5632, "Help TEXT") {
	$"0D50 6C61 7965 7250 524F 2035 2E35 0D0D"            /* .PlayerPRO 5.5.. */
	$"6874 7470 3A2F 2F77 7777 2E71 7561 646D"            /* http://www.quadm */
	$"6174 696F 6E2E 636F 6D2F 0D0D A920 416E"            /* ation.com/..© An */
	$"746F 696E 6520 524F 5353 4554 2031 3939"            /* toine ROSSET 199 */
	$"352D 3230 3030 0D0D 2D2D 2D2D 2D2D 2D2D"            /* 5-2000..-------- */
	$"2D2D 2D2D 2D2D 2D2D 2D2D 2D2D 2D2D 2D2D"            /* ---------------- */
	$"2D2D 2D2D 2D2D 2D2D 2D2D 2D2D 2D2D 2D2D"            /* ---------------- */
	$"2D2D 2D2D 2D2D 2D2D 2D2D 2D2D 2D2D 2D2D"            /* ---------------- */
	$"0D50 4C41 5945 5220 5052 4F20 4D41 4320"            /* .PLAYER PRO MAC  */
	$"4344 2D52 4F4D 2050 6163 6B61 6765 0D36"            /* CD-ROM Package.6 */
	$"3920 5553 240D 506C 6179 6572 5052 4F20"            /* 9 US$.PlayerPRO  */
	$"636F 6D6D 6572 6369 616C 2076 6572 7369"            /* commercial versi */
	$"6F6E 2C20 6465 762E 206B 6974 2C20 3430"            /* on, dev. kit, 40 */
	$"3030 206D 7573 6963 732C 2035 3030 2069"            /* 00 musics, 500 i */
	$"6E73 7472 756D 656E 7473 2C20 6F74 6865"            /* nstruments, othe */
	$"7220 6D75 7369 6320 7368 6172 6577 6172"            /* r music sharewar */
	$"6573 2C20 646F 6375 6D65 6E74 6174 696F"            /* es, documentatio */
	$"6E2E 2028 3535 3020 4D42 290D 0D57 6879"            /* n. (550 MB)..Why */
	$"2062 7579 2074 6865 2063 6F6D 6D65 7263"            /*  buy the commerc */
	$"6961 6C20 7665 7273 696F 6E3F 0D0D 2D20"            /* ial version?..-  */
	$"416C 6C20 6675 6E63 7469 6F6E 7320 6176"            /* All functions av */
	$"6169 6C61 626C 653A 2073 6176 6520 796F"            /* ailable: save yo */
	$"7572 206D 7573 6963 733B 2065 7870 6F72"            /* ur musics; expor */
	$"7420 796F 7572 206D 7573 6963 7320 696E"            /* t your musics in */
	$"2041 4946 463B 2069 6D70 6F72 7420 616E"            /*  AIFF; import an */
	$"6420 6578 706F 7274 206D 7573 6963 7320"            /* d export musics  */
	$"6672 6F6D 204D 4944 492C 2058 4D2C 2049"            /* from MIDI, XM, I */
	$"542C 204D 4544 2C20 3636 392C 204D 544D"            /* T, MED, 669, MTM */
	$"2C20 4F4B 5441 2C20 5333 4D3B 2065 7870"            /* , OKTA, S3M; exp */
	$"6F72 7420 796F 7572 206D 7573 6963 7320"            /* ort your musics  */
	$"696E 2061 7574 6F2D 6578 6563 7574 6162"            /* in auto-executab */
	$"6C65 2066 6F72 6D61 7420 4150 504C 3B20"            /* le format APPL;  */
	$"7072 6576 6965 7720 796F 7572 206D 7573"            /* preview your mus */
	$"6963 2077 6974 6820 7468 6520 D257 4156"            /* ic with the “WAV */
	$"4520 5072 6576 6965 77D3 3B20 6564 6974"            /* E Preview”; edit */
	$"2074 6865 6D20 7769 7468 2074 6865 20D2"            /*  them with the “ */
	$"426F 7820 4564 6974 6F72 D30D 0D2D 2034"            /* Box Editor”..- 4 */
	$"3030 3020 6D75 7369 6373 2066 6F72 2065"            /* 000 musics for e */
	$"7861 6D70 6C65 732C 2077 6974 6820 616C"            /* xamples, with al */
	$"6C20 696E 7374 7275 6D65 6E74 7320 616E"            /* l instruments an */
	$"6420 7361 6D70 6C65 7320 6176 6169 6C61"            /* d samples availa */
	$"626C 6520 666F 7220 6372 6561 7469 6E67"            /* ble for creating */
	$"2079 6F75 7220 6D75 7369 6373 0D0D 2D20"            /*  your musics..-  */
	$"696E 7374 7275 6D65 6E74 7320 616E 6420"            /* instruments and  */
	$"7361 6D70 6C65 7320 746F 2069 6E63 6C75"            /* samples to inclu */
	$"6465 2069 6E20 796F 7572 206D 7573 6963"            /* de in your music */
	$"730D 0D2D 2061 6C6C 2043 2F43 2B2B 2074"            /* s..- all C/C++ t */
	$"6F6F 6C6B 6974 7320 746F 2063 7265 6174"            /* oolkits to creat */
	$"6520 796F 7572 2073 6F66 7477 6172 6520"            /* e your software  */
	$"7769 7468 2050 6C61 7965 7250 524F 206D"            /* with PlayerPRO m */
	$"7573 6963 730D 0D2D 2066 7265 6520 7570"            /* usics..- free up */
	$"6772 6164 6573 2066 6F72 2031 2079 6561"            /* grades for 1 yea */
	$"722C 2061 7661 696C 6162 6C65 206F 6E20"            /* r, available on  */
	$"6874 7470 3A2F 2F77 7777 2E71 7561 646D"            /* http://www.quadm */
	$"6174 696F 6E2E 636F 6D0D 2D2D 2D2D 2D2D"            /* ation.com.------ */
	$"2D2D 2D2D 2D2D 2D2D 2D2D 2D2D 2D2D 2D2D"            /* ---------------- */
	$"2D2D 2D2D 2D2D 2D2D 2D2D 2D2D 2D2D 2D2D"            /* ---------------- */
	$"2D2D 2D2D 2D2D 2D2D 2D2D 2D2D 2D2D 2D2D"            /* ---------------- */
	$"2D2D 0D48 6F77 2074 6F20 6F72 6465 7220"            /* --.How to order  */
	$"796F 7572 2063 6F6D 706C 6574 6520 506C"            /* your complete Pl */
	$"6179 6572 5052 4F3A 0D0D 4669 6C6C 2061"            /* ayerPRO:..Fill a */
	$"6E20 6F72 6465 7220 666F 726D 2066 726F"            /* n order form fro */
	$"6D20 7468 6520 224F 7264 6572 2046 6F72"            /* m the "Order For */
	$"6D22 2046 6F6C 6465 720D 0D6F 720D 0D47"            /* m" Folder..or..G */
	$"6F20 746F 3A0D 6874 7470 3A2F 2F77 7777"            /* o to:.http://www */
	$"2E71 7561 646D 6174 696F 6E2E 636F 6D2F"            /* .quadmation.com/ */
	$"7070 686F 6D65 2E68 746D 0D0D 2D2D 2D2D"            /* pphome.htm..---- */
	$"2D2D 2D2D 2D2D 2D2D 2D2D 2D2D 2D2D 2D2D"            /* ---------------- */
	$"2D2D 2D2D 2D2D 2D2D 2D2D 2D2D 2D2D 2D2D"            /* ---------------- */
	$"2D2D 2D2D 2D2D 2D2D 2D2D 2D2D 2D2D 2D2D"            /* ---------------- */
	$"2D2D 2D2D 0D55 5341 2F43 414E 4144 412F"            /* ----.USA/CANADA/ */
	$"536F 7574 6820 416D 6572 6963 6120 4469"            /* South America Di */
	$"7374 7269 6275 746F 723A 0D0D 5175 6164"            /* stributor:..Quad */
	$"6D61 7469 6F6E 2C20 496E 632E 0D53 616C"            /* mation, Inc..Sal */
	$"6573 2044 6570 6172 746D 656E 740D 3436"            /* es Department.46 */
	$"3020 5065 7273 6961 6E20 4472 6976 652C"            /* 0 Persian Drive, */
	$"2053 7569 7465 204E 6F2E 2035 0D53 756E"            /*  Suite No. 5.Sun */
	$"6E79 7661 6C65 2C20 4341 2039 3430 3839"            /* nyvale, CA 94089 */
	$"2D31 3730 380D 552E 532E 412E 0D0D 566F"            /* -1708.U.S.A...Vo */
	$"6963 653A 2034 3038 2D37 3437 2D31 3131"            /* ice: 408-747-111 */
	$"370D 4661 783A 2034 3038 2D37 3437 2D31"            /* 7.Fax: 408-747-1 */
	$"3337 370D 456D 6169 6C3A 2073 616C 6573"            /* 377.Email: sales */
	$"4071 7561 646D 6174 696F 6E2E 636F 6D0D"            /* @quadmation.com. */
	$"5757 573A 2068 7474 703A 2F2F 7777 772E"            /* WWW: http://www. */
	$"7175 6164 6D61 7469 6F6E 2E63 6F6D 2F70"            /* quadmation.com/p */
	$"7068 6F6D 652E 6874 6D0D 2D2D 2D2D 2D2D"            /* phome.htm.------ */
	$"2D2D 2D2D 2D2D 2D2D 2D2D 2D2D 2D2D 2D2D"            /* ---------------- */
	$"2D2D 2D2D 2D2D 2D2D 2D2D 2D2D 2D2D 2D2D"            /* ---------------- */
	$"2D2D 2D2D 2D2D 2D2D 2D2D 2D2D 2D2D 2D2D"            /* ---------------- */
	$"2D2D 2D0D 4575 726F 7065 2026 204A 6170"            /* ---.Europe & Jap */
	$"616E 2026 204F 7468 6572 2043 6F75 6E74"            /* an & Other Count */
	$"7269 6573 2044 6973 7472 6962 7574 6F72"            /* ries Distributor */
	$"3A0D 0D41 6E74 6F69 6E65 2052 4F53 5345"            /* :..Antoine ROSSE */
	$"540D 3230 204D 6963 6865 6C69 2D44 752D"            /* T.20 Micheli-Du- */
	$"4372 6573 740D 3132 3035 2047 454E 4556"            /* Crest.1205 GENEV */
	$"410D 5357 4954 5A45 524C 414E 440D 4555"            /* A.SWITZERLAND.EU */
	$"524F 5045 0D0D 456D 6169 6C3A 2072 6F73"            /* ROPE..Email: ros */
	$"7365 7461 6E74 6F69 6E65 4062 6C75 6577"            /* setantoine@bluew */
	$"696E 2E63 680D 5068 6F6E 653A 2028 2B34"            /* in.ch.Phone: (+4 */
	$"3120 3739 2920 3230 3320 3734 2036 320D"            /* 1 79) 203 74 62. */
	$"4661 783A 2028 2B34 3120 3232 2920 3334"            /* Fax: (+41 22) 34 */
	$"3620 3131 2039 370D 5553 454E 4554 3A20"            /* 6 11 97.USENET:  */
	$"616C 742E 6269 6E61 7269 6573 2E73 6F75"            /* alt.binaries.sou */
	$"6E64 732E 6D6F 6473 0D57 5757 2053 6974"            /* nds.mods.WWW Sit */
	$"6573 3A0D 2020 2020 A520 6874 7470 3A2F"            /* es:.    • http:/ */
	$"2F68 7970 6572 6172 6368 6976 652E 6C63"            /* /hyperarchive.lc */
	$"732E 6D69 742E 6564 752F 5265 6365 6E74"            /* s.mit.edu/Recent */
	$"2D53 756D 6D61 7279 2E68 746D 6C20 2753"            /* -Summary.html 'S */
	$"6561 7263 6820 506C 6179 6572 5052 4F27"            /* earch PlayerPRO' */
	$"0D20 2020 20A5 2068 7474 703A 2F2F 7777"            /* .    • http://ww */
	$"772E 7175 6164 6D61 7469 6F6E 2E63 6F6D"            /* w.quadmation.com */
	$"2F70 7068 6F6D 652E 6874 6D0D 4242 533A"            /* /pphome.htm.BBS: */
	$"0D20 2020 20A5 2046 5241 4E43 4520 2D20"            /* .    • FRANCE -  */
	$"456C 6C69 7320 3A20 5061 7269 7320 282B"            /* Ellis : Paris (+ */
	$"3333 2031 2034 3720 3331 2032 3120 3231"            /* 33 1 47 31 21 21 */
	$"290D 2D2D 2D2D 2D2D 2D2D 2D2D 2D2D 2D2D"            /* ).-------------- */
	$"2D2D 2D2D 2D2D 2D2D 2D2D 2D2D 2D2D 2D2D"            /* ---------------- */
	$"2D2D 2D2D 2D2D 2D2D 2D2D 2D2D 2D2D 2D2D"            /* ---------------- */
	$"2D2D 2D2D 2D2D 2D2D 2D2D 2D0D 0D54 6869"            /* -----------..Thi */
	$"7320 7072 6F67 7261 6D20 7761 7320 636F"            /* s program was co */
	$"6E63 6569 7665 640D 6279 2041 6E74 6F69"            /* nceived.by Antoi */
	$"6E65 2052 4F53 5345 5420 2868 616E 646C"            /* ne ROSSET (handl */
	$"653A 2042 7961 6B68 6565 292E 0D0D 5468"            /* e: Byakhee)...Th */
	$"6520 7465 7874 2061 6E64 2067 7261 7068"            /* e text and graph */
	$"6963 7320 7765 7265 206D 6164 650D 6279"            /* ics were made.by */
	$"204D 696B 6520 5645 4E54 5552 4920 2868"            /*  Mike VENTURI (h */
	$"616E 646C 653A 2043 7962 6F72 6729 200D"            /* andle: Cyborg) . */
	$"616E 6420 4A6F 686E 2053 5449 4C45 5320"            /* and John STILES  */
	$"2868 616E 646C 653A 20AE 4F4E 2046 4C55"            /* (handle: ÆON FLU */
	$"5829 0D0D 5468 616E 6B73 2074 6F20 4A61"            /* X)..Thanks to Ja */
	$"6D61 6C20 4861 6E6E 6168 2066 6F72 2068"            /* mal Hannah for h */
	$"6973 2073 7570 706F 7274 2026 2062 6574"            /* is support & bet */
	$"612D 7465 7374 696E 6720 2620 6D61 696C"            /* a-testing & mail */
	$"696E 672D 6C69 7374 0D0D 5468 616E 6B73"            /* ing-list..Thanks */
	$"2074 6F20 4875 6265 7274 2047 6172 7269"            /*  to Hubert Garri */
	$"646F 2066 6F72 2068 6973 2070 6C75 6773"            /* do for his plugs */
	$"210D 2853 6F75 7263 6520 636F 6465 2061"            /* !.(Source code a */
	$"7661 696C 6162 6C65 2069 6E20 506C 6179"            /* vailable in Play */
	$"6572 5052 4F20 4465 7665 6C6F 7070 6572"            /* erPRO Developper */
	$"2054 6F6F 6C4B 6974 290D 0D54 6861 6E6B"            /*  ToolKit)..Thank */
	$"7320 746F 204A 2E42 2E20 5769 6C73 6F6E"            /* s to J.B. Wilson */
	$"2066 6F72 2068 6973 206E 6577 2069 636F"            /*  for his new ico */
	$"6E73 2021 0D0D 5468 6973 2070 726F 6772"            /* ns !..This progr */
	$"616D 2069 7320 6120 536F 756E 642D 5472"            /* am is a Sound-Tr */
	$"6163 6B65 7220 706C 6179 6572 2E0D 4974"            /* acker player..It */
	$"2070 6572 6D69 7473 2079 6F75 2074 6F3A"            /*  permits you to: */
	$"0D2D 2050 6C61 7920 6D75 7369 6320 6672"            /* .- Play music fr */
	$"6F6D 204D 4F44 2064 6174 6120 6669 6C65"            /* om MOD data file */
	$"732E 0D2D 2045 6469 7420 7468 6520 696E"            /* s..- Edit the in */
	$"7374 7275 6D65 6E74 7320 616E 6420 7061"            /* struments and pa */
	$"7274 6974 696F 6E73 2069 6E20 4D4F 4420"            /* rtitions in MOD  */
	$"6669 6C65 732E 0D2D 2053 6176 6520 4D4F"            /* files..- Save MO */
	$"4420 6669 6C65 732E 0D0D 506C 6179 6572"            /* D files...Player */
	$"2050 524F 2069 7320 7468 6520 6265 7374"            /*  PRO is the best */
	$"2053 6F75 6E64 2D54 7261 636B 6572 2070"            /*  Sound-Tracker p */
	$"726F 6772 616D 206F 6E20 4D61 6321 0D0D"            /* rogram on Mac!.. */
	$"4974 2069 7320 616C 736F 2061 6476 6973"            /* It is also advis */
	$"6564 2074 6F20 7573 6520 7370 6561 6B65"            /* ed to use speake */
	$"7273 2061 6E64 2073 7465 7265 6F20 746F"            /* rs and stereo to */
	$"2067 6574 206D 6178 696D 756D 2073 6F75"            /*  get maximum sou */
	$"6E64 2071 7561 6C69 7479 2E0D 0D54 6865"            /* nd quality...The */
	$"204D 7573 6963 2064 7269 7665 7273 2061"            /*  Music drivers a */
	$"7265 2046 5245 454C 5920 6176 6169 6C61"            /* re FREELY availa */
	$"626C 6520 666F 7220 7072 6F67 7261 6D6D"            /* ble for programm */
	$"6572 732E 2054 6865 7920 636F 6E73 6973"            /* ers. They consis */
	$"7420 6F66 2061 206C 6962 7261 7279 2070"            /* t of a library p */
	$"6163 6B20 696E 2043 2066 6F72 6D61 7420"            /* ack in C format  */
	$"284D 5057 2C20 436F 6465 5761 7272 696F"            /* (MPW, CodeWarrio */
	$"7220 2620 5468 696E 6B29 2E0D 5468 6520"            /* r & Think)..The  */
	$"7573 6520 616E 6420 7468 6520 696E 636F"            /* use and the inco */
	$"7270 6F72 6174 696F 6E20 6F66 2074 6865"            /* rporation of the */
	$"7365 2072 6F75 7469 6E65 7320 696E 2061"            /* se routines in a */
	$"2070 726F 6772 616D 2069 7320 616C 736F"            /*  program is also */
	$"2074 6F74 616C 6C79 2066 7265 652C 2073"            /*  totally free, s */
	$"6F20 7461 6B65 2061 6476 616E 7461 6765"            /* o take advantage */
	$"206F 6620 6974 210D 4176 6169 6C61 626C"            /*  of it!.Availabl */
	$"6520 6576 6572 7977 6865 7265 2021 210D"            /* e everywhere !!. */
	$"0D54 6865 2061 7574 686F 723A 0D41 6E74"            /* .The author:.Ant */
	$"6F69 6E65 2052 4F53 5345 543A 2042 6F72"            /* oine ROSSET: Bor */
	$"6E20 696E 2031 3937 322C 2073 7475 6465"            /* n in 1972, stude */
	$"6E74 2069 6E20 6875 6D61 6E20 6D65 6469"            /* nt in human medi */
	$"6369 6E65 2061 7420 7468 6520 556E 6976"            /* cine at the Univ */
	$"6572 7369 7479 2048 6F73 7069 7461 6C20"            /* ersity Hospital  */
	$"6F66 2047 656E 6576 6120 696E 2053 7769"            /* of Geneva in Swi */
	$"747A 6572 6C61 6E64 2061 6E64 206E 6F77"            /* tzerland and now */
	$"2061 206D 6564 6963 616C 2064 6F63 746F"            /*  a medical docto */
	$"7221 2046 6173 6369 6E61 7465 6420 7369"            /* r! Fascinated si */
	$"6E63 6520 7468 6520 6167 6520 6F66 2031"            /* nce the age of 1 */
	$"3020 6279 2041 7070 6C65 2063 6F6D 7075"            /* 0 by Apple compu */
	$"7465 7273 3A20 4170 706C 6520 4949 2B2C"            /* ters: Apple II+, */
	$"2041 7070 6C65 2049 4965 2C20 4170 706C"            /*  Apple IIe, Appl */
	$"6520 4949 6773 2C20 4D41 4320 4949 6378"            /* e IIgs, MAC IIcx */
	$"2C20 5175 6164 7261 2037 3030 2C20 506F"            /* , Quadra 700, Po */
	$"7765 724D 6163 2038 3130 3020 616E 6420"            /* werMac 8100 and  */
	$"6E6F 7720 506F 7765 724D 6163 2039 3630"            /* now PowerMac 960 */
	$"3021 2049 2068 6F70 6520 746F 2062 7579"            /* 0! I hope to buy */
	$"2061 2047 3420 696E 2066 6577 206D 6F6E"            /*  a G4 in few mon */
	$"7468 732E 2E2E 2041 7574 686F 7220 6F66"            /* ths... Author of */
	$"2064 6976 6572 7365 2073 6861 7265 7761"            /*  diverse sharewa */
	$"7265 2070 726F 6772 616D 7320 616E 6420"            /* re programs and  */
	$"6761 6D65 732E 2050 726F 6772 616D 6D65"            /* games. Programme */
	$"7220 696E 2044 6967 6974 616C 2049 6D61"            /* r in Digital Ima */
	$"6769 6E67 2055 6E69 7420 2855 6E69 7665"            /* ging Unit (Unive */
	$"7273 6974 7920 486F 7370 6974 616C 206F"            /* rsity Hospital o */
	$"6620 4765 6E65 7661 292C 2043 6F6E 7375"            /* f Geneva), Consu */
	$"6C74 696E 6720 5072 6F67 7261 6D6D 6572"            /* lting Programmer */
	$"2C20 7370 6563 6961 6C69 7A65 6420 696E"            /* , specialized in */
	$"204D 6163 696E 746F 7368 2061 7263 6869"            /*  Macintosh archi */
	$"7465 6374 7572 652E 2049 276D 2061 6374"            /* tecture. I'm act */
	$"7561 6C6C 7920 776F 726B 696E 6720 696E"            /* ually working in */
	$"2052 6164 696F 6C6F 6779 2061 6E64 2049"            /*  Radiology and I */
	$"6D61 6769 6E67 2C20 696E 2074 6865 2052"            /* maging, in the R */
	$"6164 696F 6C6F 6779 2044 6570 6172 746D"            /* adiology Departm */
	$"656E 7420 6F66 2074 6865 2047 656E 6576"            /* ent of the Genev */
	$"6120 486F 7370 6974 616C 2E0D 0D2D 2D2D"            /* a Hospital...--- */
	$"2D2D 2D2D 2D2D 2D2D 2D2D 2D2D 2D2D 2D2D"            /* ---------------- */
	$"2D2D 2D2D 2D2D 2D2D 2D2D 2D2D 2D2D 2D2D"            /* ---------------- */
	$"2D2D 2D2D 2D2D 2D2D 2D2D 2D2D 2D2D 2D2D"            /* ---------------- */
	$"2D2D 2D2D 2D2D 0D0D 4172 6520 796F 7520"            /* ------..Are you  */
	$"776F 726B 696E 6720 696E 2061 6E20 686F"            /* working in an ho */
	$"7370 6974 616C 3F20 446F 2079 6F75 206B"            /* spital? Do you k */
	$"6E6F 7720 7768 6174 20D2 5041 4353 D320"            /* now what “PACS”  */
	$"2850 6963 7475 7265 2061 7263 6869 7669"            /* (Picture archivi */
	$"6E67 2061 6E64 2063 6F6D 6D75 6E69 6361"            /* ng and communica */
	$"7469 6F6E 2073 7973 7465 6D29 2069 733F"            /* tion system) is? */
	$"2120 4865 7265 2069 6E20 7468 6520 4765"            /* ! Here in the Ge */
	$"6E65 7661 2048 6F73 7069 7461 6C2C 2077"            /* neva Hospital, w */
	$"6520 6172 6520 6465 7665 6C6F 7069 6E67"            /* e are developing */
	$"206D 616E 7920 4D61 6369 6E74 6F73 6820"            /*  many Macintosh  */
	$"736F 6674 7761 7265 2070 726F 6772 616D"            /* software program */
	$"7320 666F 7220 5041 4353 210D 4F73 6972"            /* s for PACS!.Osir */
	$"6973 202D 2050 686F 746F 7368 6F70 2066"            /* is - Photoshop f */
	$"6F72 206D 6564 6963 616C 2069 6D61 6765"            /* or medical image */
	$"7279 2021 210D 4574 632E 2045 7463 2E0D"            /* ry !!.Etc. Etc.. */
	$"466F 7220 6D6F 7265 2069 6E66 6F72 6D61"            /* For more informa */
	$"7469 6F6E 7320 6162 6F75 7420 7468 6573"            /* tions about thes */
	$"6520 6D65 6469 6361 6C20 6672 6565 7761"            /* e medical freewa */
	$"7265 2C20 636F 6E74 6163 743A 0D0D 5757"            /* re, contact:..WW */
	$"5720 5369 7465 3A20 6874 7470 3A2F 2F77"            /* W Site: http://w */
	$"7777 2E67 656E 6973 6973 2E63 682F 5549"            /* ww.genisis.ch/UI */
	$"4E2E 6874 6D6C 0D2D 2D2D 2D2D 2D2D 2D2D"            /* N.html.--------- */
	$"2D2D 2D2D 2D2D 2D2D 2D2D 2D2D 2D2D 2D2D"            /* ---------------- */
	$"2D2D 2D2D 2D2D 2D2D 2D2D 2D2D 2D2D 2D2D"            /* ---------------- */
	$"2D2D 2D2D 2D2D 2D2D 2D2D 2D2D 2D2D 2D2D"            /* ---------------- */
	$"0D0D 466F 7220 6D6F 7265 2069 6E66 6F72"            /* ..For more infor */
	$"6D61 7469 6F6E 7320 6162 6F75 7420 7468"            /* mations about th */
	$"6520 6175 7468 6F72 732C 2061 626F 7574"            /* e authors, about */
	$"2050 6C61 7965 7250 524F 3A0D 0D41 6E74"            /*  PlayerPRO:..Ant */
	$"6F69 6E65 2052 4F53 5345 540D 3230 204D"            /* oine ROSSET.20 M */
	$"6963 6865 6C69 2D44 752D 4372 6573 740D"            /* icheli-Du-Crest. */
	$"3132 3035 2047 454E 4556 410D 5357 4954"            /* 1205 GENEVA.SWIT */
	$"5A45 524C 414E 440D 4555 524F 5045 0D0D"            /* ZERLAND.EUROPE.. */
	$"456D 6169 6C3A 2072 6F73 7365 7461 6E74"            /* Email: rossetant */
	$"6F69 6E65 4062 6C75 6577 696E 2E63 680D"            /* oine@bluewin.ch. */
	$"5068 6F6E 653A 2028 2B34 3120 3739 2920"            /* Phone: (+41 79)  */
	$"3230 3320 3734 2036 320D 4661 783A 2028"            /* 203 74 62.Fax: ( */
	$"2B34 3120 3232 2920 3334 3620 3131 2039"            /* +41 22) 346 11 9 */
	$"370D 4242 533A 0D45 6C6C 6973 2046 5241"            /* 7.BBS:.Ellis FRA */
	$"4E43 4520 2D20 5061 7269 7320 282B 3333"            /* NCE - Paris (+33 */
	$"2031 2034 3720 3331 2032 3120 3231 290D"            /*  1 47 31 21 21). */
	$"0D59 6F75 2068 6176 6520 6163 6365 7373"            /* .You have access */
	$"2074 6F20 494E 5445 524E 4554 203F 0D57"            /*  to INTERNET ?.W */
	$"5757 2053 6974 6573 3A0D 2020 2020 A520"            /* WW Sites:.    •  */
	$"6874 7470 3A2F 2F77 7777 2E71 7561 646D"            /* http://www.quadm */
	$"6174 696F 6E2E 636F 6D2F 7070 686F 6D65"            /* ation.com/pphome */
	$"2E68 746D 0D55 5345 4E45 543A 0D20 2020"            /* .htm.USENET:.    */
	$"20A5 204A 6F69 6E20 4D4F 4473 2066 6F72"            /*  • Join MODs for */
	$"756D 203A 2061 6C74 2E62 696E 6172 6965"            /* um : alt.binarie */
	$"732E 736F 756E 6473 2E6D 6F64 730D"                 /* s.sounds.mods. */
};

data 'TEXT' (400, "Default Bookmarks") {
	$"6874 7470 3A2F 2F77 7777 2E71 7561 646D"            /* http://www.quadm */
	$"6174 696F 6E2E 636F 6D2F 5050 2F44 6F77"            /* ation.com/PP/Dow */
	$"6E6C 6F61 642D 5573 6572 2E4D 7573 6963"            /* nload-User.Music */
	$"2E68 746D 6C0D 506C 6179 6572 5052 4F20"            /* .html.PlayerPRO  */
	$"4D75 7369 6320 4172 6368 6976 6520 2D20"            /* Music Archive -  */
	$"5072 697A 6573 2074 6F20 7769 6E0D 0D68"            /* Prizes to win..h */
	$"7474 703A 2F2F 7777 772E 6D6F 6461 7263"            /* ttp://www.modarc */
	$"6869 7665 2E63 6F6D 0D54 6865 204D 4F44"            /* hive.com.The MOD */
	$"2041 7263 6869 7665 202D 204D 7573 6963"            /*  Archive - Music */
	$"2066 696C 6573 0D0D 6874 7470 3A2F 2F77"            /*  files..http://w */
	$"7777 2E6D 6F64 626F 742E 636F 6D2F 0D4D"            /* ww.modbot.com/.M */
	$"4F44 2042 4F54 202D 204D 6F72 6520 4D75"            /* OD BOT - More Mu */
	$"7369 6320 6669 6C65 730D 0D66 7470 3A2F"            /* sic files..ftp:/ */
	$"2F66 7470 2E6C 7574 682E 7365 2F70 7562"            /* /ftp.luth.se/pub */
	$"2F6D 7364 6F73 2F64 656D 6F73 2F6D 7573"            /* /msdos/demos/mus */
	$"6963 2F0D 4654 5020 7369 7465 2066 6F72"            /* ic/.FTP site for */
	$"206D 6F72 6520 4D75 7369 6320 6669 6C65"            /*  more Music file */
	$"730D 0D66 7470 3A2F 2F66 7470 2E63 6472"            /* s..ftp://ftp.cdr */
	$"6F6D 2E63 6F6D 2F70 7562 2F64 656D 6F73"            /* om.com/pub/demos */
	$"2F6D 7573 6963 2F0D 4344 524F 4D2E 434F"            /* /music/.CDROM.CO */
	$"4D27 7320 6D75 7369 6320 6469 7265 6374"            /* M's music direct */
	$"6F72 790D 0D66 7470 3A2F 2F66 7470 2E63"            /* ory..ftp://ftp.c */
	$"6472 6F6D 2E63 6F6D 2F70 7562 2F64 656D"            /* drom.com/pub/dem */
	$"6F73 2F6D 7573 6963 2F73 616D 706C 6573"            /* os/music/samples */
	$"2F0D 5361 6D70 6C65 732C 2073 616D 706C"            /* /.Samples, sampl */
	$"6573 2061 6E64 2073 616D 706C 6573 0D0D"            /* es and samples.. */
	$"6874 7470 3A2F 2F75 6E69 7465 642D 7472"            /* http://united-tr */
	$"6163 6B65 7273 2E6F 7267 2F0D 4D75 7369"            /* ackers.org/.Musi */
	$"6320 6F6E 2069 6E74 6572 6E65 740D 0D68"            /* c on internet..h */
	$"7474 703A 2F2F 7777 772E 6D61 7A2D 736F"            /* ttp://www.maz-so */
	$"756E 642E 636F 6D2F 0D53 6F75 6E64 7472"            /* und.com/.Soundtr */
	$"6163 6B65 7273 2066 6F72 2057 696E 7465"            /* ackers for Winte */
	$"6C20 776F 726C 640D 0D68 7474 703A 2F2F"            /* l world..http:// */
	$"7777 772E 756E 6974 6564 2D74 7261 636B"            /* www.united-track */
	$"6572 732E 6F72 672F 0D49 6E66 6F72 6D61"            /* ers.org/.Informa */
	$"7469 6F6E 7320 6162 6F75 7420 7468 6520"            /* tions about the  */
	$"736F 756E 6474 7261 636B 6572 2077 6F72"            /* soundtracker wor */
	$"6C64 210D 0D68 7474 703A 2F2F 696C 6162"            /* ld!..http://ilab */
	$"732E 6479 6E69 702E 636F 6D2F 0D53 616D"            /* s.dynip.com/.Sam */
	$"706C 6573 2061 6E64 2073 6F75 6E64 730D"            /* ples and sounds. */
	$"0D68 7474 703A 2F2F 7777 772E 6368 6970"            /* .http://www.chip */
	$"7475 6E65 2E63 6F6D 2F0D 4368 6970 2074"            /* tune.com/.Chip t */
	$"756E 6573"                                          /* unes */
};

data 'styl' (200, "Arpeggio") {
	$"0002 0000 0000 000C 000A 0003 0100 0009"            /* .........¬.....∆ */
	$"0000 0000 0000 0000 0008 000C 000A 0003"            /* .............¬.. */
	$"0000 0009 0000 0000 0000"                           /* ...∆...... */
};

data 'styl' (201, "SlideUp") {
	$"0002 0000 0000 000C 000A 0003 0100 0009"            /* .........¬.....∆ */
	$"0000 0000 0000 0000 0008 000C 000A 0003"            /* .............¬.. */
	$"0000 0009 0000 0000 0000"                           /* ...∆...... */
};

data 'styl' (202, "Slide Down") {
	$"0002 0000 0000 000C 000A 0003 0100 0009"            /* .........¬.....∆ */
	$"0000 0000 0000 0000 000A 000C 000A 0003"            /* .........¬...¬.. */
	$"0000 0009 0000 0000 0000"                           /* ...∆...... */
};

data 'styl' (203, "Slide to Note") {
	$"0002 0000 0000 000C 000A 0001 0100 0009"            /* .........¬.....∆ */
	$"0000 0000 0000 0000 000D 000C 000A 0001"            /* .............¬.. */
	$"0000 0009 0000 0000 0000"                           /* ...∆...... */
};

data 'styl' (204, "Vibrato") {
	$"0002 0000 0000 000C 000A 0001 0100 0009"            /* .........¬.....∆ */
	$"0000 0000 0000 0000 0007 000C 000A 0001"            /* .............¬.. */
	$"0000 0009 0000 0000 0000"                           /* ...∆...... */
};

data 'styl' (205, "Slide to note + Volume Slide") {
	$"0002 0000 0000 000C 000A 0001 0100 0009"            /* .........¬.....∆ */
	$"0000 0000 0000 0000 0032 000C 000A 0001"            /* .........2...¬.. */
	$"0000 0009 0000 0000 0000"                           /* ...∆...... */
};

data 'styl' (206, "Vibrato + Volume Slide") {
	$"0002 0000 0000 000C 000A 0001 0100 0009"            /* .........¬.....∆ */
	$"0000 0000 0000 0000 002C 000C 000A 0001"            /* .........,...¬.. */
	$"0000 0009 0000 0000 0000"                           /* ...∆...... */
};

data 'styl' (207, "Tremolo") {
	$"0002 0000 0000 000C 000A 0001 0100 0009"            /* .........¬.....∆ */
	$"0000 0000 0000 0000 0007 000C 000A 0001"            /* .............¬.. */
	$"0000 0009 0000 0000 0000"                           /* ...∆...... */
};

data 'styl' (208, "Unused") {
	$"0002 0000 0000 000C 000A 0001 0100 0009"            /* .........¬.....∆ */
	$"0000 0000 0000 0000 000B 000C 000A 0001"            /* .............¬.. */
	$"0000 0009 0000 0000 0000"                           /* ...∆...... */
};

data 'styl' (209, "Set sample offset") {
	$"0002 0000 0000 000C 000A 0001 0100 0009"            /* .........¬.....∆ */
	$"0000 0000 0000 0000 0011 000C 000A 0001"            /* .............¬.. */
	$"0000 0009 0000 0000 0000"                           /* ...∆...... */
};

data 'styl' (210, "Volume slide") {
	$"0002 0000 0000 000C 000A 0001 0100 0009"            /* .........¬.....∆ */
	$"0000 0000 0000 0000 000C 000C 000A 0001"            /* .............¬.. */
	$"0000 0009 0000 0000 0000"                           /* ...∆...... */
};

data 'styl' (211, "Position Jump") {
	$"0002 0000 0000 000C 000A 0001 0100 0009"            /* .........¬.....∆ */
	$"0000 0000 0000 0000 000D 000C 000A 0001"            /* .............¬.. */
	$"0000 0009 0000 0000 0000"                           /* ...∆...... */
};

data 'styl' (212, "Set volume") {
	$"0002 0000 0000 000C 000A 0001 0100 0009"            /* .........¬.....∆ */
	$"0000 0000 0000 0000 000A 000C 000A 0001"            /* .........¬...¬.. */
	$"0000 0009 0000 0000 0000"                           /* ...∆...... */
};

data 'styl' (213, "Pattern Break") {
	$"0002 0000 0000 000C 000A 0001 0100 0009"            /* .........¬.....∆ */
	$"0000 0000 0000 0000 000D 000C 000A 0001"            /* .............¬.. */
	$"0000 0009 0000 0000 0000"                           /* ...∆...... */
};

data 'styl' (214, "E - Effects") {
	$"0001 0000 0000 000C 000A 0001 0100 0009"            /* .........¬.....∆ */
	$"0000 0000 0000"                                     /* ...... */
};

data 'styl' (215, "Set speed") {
	$"0002 0000 0000 000C 000A 0001 0100 0009"            /* .........¬.....∆ */
	$"0000 0000 0000 0000 0009 000C 000A 0001"            /* .........∆...¬.. */
	$"0000 0009 0000 0000 0000"                           /* ...∆...... */
};

data 'styl' (300, "Set filter on/off") {
	$"0002 0000 0000 000C 000A 0001 0100 0009"            /* .........¬.....∆ */
	$"0000 0000 0000 0000 0011 000C 000A 0001"            /* .............¬.. */
	$"0000 0009 0000 0000 0000"                           /* ...∆...... */
};

data 'styl' (301, "Fineslide up") {
	$"0002 0000 0000 000C 000A 0001 0100 0009"            /* .........¬.....∆ */
	$"0000 0000 0000 0000 000C 000C 000A 0001"            /* .............¬.. */
	$"0000 0009 0000 0000 0000"                           /* ...∆...... */
};

data 'styl' (302, "Fineslide down") {
	$"0002 0000 0000 000C 000A 0001 0100 0009"            /* .........¬.....∆ */
	$"0000 0000 0000 0000 000E 000C 000A 0001"            /* .............¬.. */
	$"0000 0009 0000 0000 0000"                           /* ...∆...... */
};

data 'styl' (303, "Set glissando on/off") {
	$"0002 0000 0000 000C 000A 0001 0100 0009"            /* .........¬.....∆ */
	$"0000 0000 0000 0000 0014 000C 000A 0001"            /* .............¬.. */
	$"0000 0009 0000 0000 0000"                           /* ...∆...... */
};

data 'styl' (304, "Set vibrato waveform") {
	$"0002 0000 0000 000C 000A 0001 0100 0009"            /* .........¬.....∆ */
	$"0000 0000 0000 0000 0014 000C 000A 0001"            /* .............¬.. */
	$"0000 0009 0000 0000 0000"                           /* ...∆...... */
};

data 'styl' (305, "Set finetune value") {
	$"0002 0000 0000 000C 000A 0001 0100 0009"            /* .........¬.....∆ */
	$"0000 0000 0000 0000 0012 000C 000A 0001"            /* .............¬.. */
	$"0000 0009 0000 0000 0000"                           /* ...∆...... */
};

data 'styl' (306, "Loop pattern") {
	$"0002 0000 0000 000C 000A 0001 0100 0009"            /* .........¬.....∆ */
	$"0000 0000 0000 0000 000C 000C 000A 0001"            /* .............¬.. */
	$"0000 0009 0000 0000 0000"                           /* ...∆...... */
};

data 'styl' (307, "Set tremolo waveform") {
	$"0002 0000 0000 000C 000A 0001 0100 0009"            /* .........¬.....∆ */
	$"0000 0000 0000 0000 0014 000C 000A 0001"            /* .............¬.. */
	$"0000 0009 0000 0000 0000"                           /* ...∆...... */
};

data 'styl' (309, "Retrigger sample") {
	$"0002 0000 0000 000C 000A 0001 0100 0009"            /* .........¬.....∆ */
	$"0000 0000 0000 0000 0010 000C 000A 0001"            /* .............¬.. */
	$"0000 0009 0000 0000 0000"                           /* ...∆...... */
};

data 'styl' (310, "Fine volume slide up") {
	$"0002 0000 0000 000C 000A 0001 0100 0009"            /* .........¬.....∆ */
	$"0000 0000 0000 0000 0014 000C 000A 0001"            /* .............¬.. */
	$"0000 0009 0000 0000 0000"                           /* ...∆...... */
};

data 'styl' (311, "Fine volume slide down") {
	$"0002 0000 0000 000C 000A 0001 0100 0009"            /* .........¬.....∆ */
	$"0000 0000 0000 0000 0016 000C 000A 0001"            /* .............¬.. */
	$"0000 0009 0000 0000 0000"                           /* ...∆...... */
};

data 'styl' (312, "Cut sample") {
	$"0002 0000 0000 000C 000A 0001 0100 0009"            /* .........¬.....∆ */
	$"0000 0000 0000 0000 000A 000C 000A 0001"            /* .........¬...¬.. */
	$"0000 0009 0000 0000 0000"                           /* ...∆...... */
};

data 'styl' (313, "Delay sample") {
	$"0002 0000 0000 000C 000A 0001 0100 0009"            /* .........¬.....∆ */
	$"0000 0000 0000 0000 000C 000C 000A 0001"            /* .............¬.. */
	$"0000 0009 0000 0000 0000"                           /* ...∆...... */
};

data 'styl' (314, "Delay pattern") {
	$"0002 0000 0000 000C 000A 0001 0100 0009"            /* .........¬.....∆ */
	$"0000 0000 0000 0000 000D 000C 000A 0001"            /* .............¬.. */
	$"0000 0009 0000 0000 0000"                           /* ...∆...... */
};

data 'styl' (315, "Invert loop") {
	$"0002 0000 0000 000C 000A 0001 0100 0009"            /* .........¬.....∆ */
	$"0000 0000 0000 0000 000B 000C 000A 0001"            /* .............¬.. */
	$"0000 0009 0000 0000 0000"                           /* ...∆...... */
};

data 'styl' (257) {
	$"0001 0000 0000 000C 000A 0001 0000 0009"            /* .........¬.....∆ */
	$"0000 0000 0000"                                     /* ...... */
};

data 'styl' (308, "Unused") {
	$"0001 0000 0000 000C 000A 0001 0100 0009"            /* .........¬.....∆ */
	$"0000 0000 0000"                                     /* ...... */
};

data 'styl' (401) {
	$"0004 0000 0000 000C 000A 0003 0100 0009"            /* .........¬.....∆ */
	$"0000 0000 0000 0000 008E 000C 000A 0003"            /* .........é...¬.. */
	$"0500 0009 1818 2222 CDCD 0000 00A6 000C"            /* ...∆..""ÕÕ...¶.. */
	$"000A 0003 0100 0009 2929 1A1A 1010 0000"            /* .¬.....∆))...... */
	$"00A7 000C 000A 0003 0100 0009 0000 0000"            /* .ß...¬.....∆.... */
	$"0000"                                               /* .. */
};

data 'styl' (216, "Set speed") {
	$"0002 0000 0000 000C 000A 0001 0100 0009"            /* .........¬.....∆ */
	$"0000 0000 0000 0000 0008 000C 000A 0001"            /* .............¬.. */
	$"0000 0009 0000 0000 0000"                           /* ...∆...... */
};

data 'styl' (5632) {
	$"0070 0000 0000 000E 000A 0015 4100 000C"            /* .p.......¬..A... */
	$"DD6B 08C2 06A2 0000 0001 0024 001C 0015"            /* ›k.¬.¢.....$.... */
	$"6100 0024 DD6B 08C2 06A2 0000 000E 0019"            /* a..$›k.¬.¢...... */
	$"0014 0015 4100 0018 DD6B 08C2 06A2 0000"            /* ....A...›k.¬.¢.. */
	$"000F 000E 000A 0015 4100 000C DD6B 08C2"            /* .....¬..A...›k.¬ */
	$"06A2 0000 002B 0010 000C 0002 0000 000C"            /* .¢...+.......... */
	$"DD6B 08C2 06A2 0000 002C 000C 000A 0003"            /* ›k.¬.¢...,...¬.. */
	$"0000 0009 0000 0000 D400 0000 002E 000C"            /* ...∆....‘....... */
	$"000A 0003 0000 0009 DD6B 08C2 06A2 0000"            /* .¬.....∆›k.¬.¢.. */
	$"003C 000C 000A 0003 0000 0009 0000 0000"            /* .<...¬.....∆.... */
	$"D400 0000 0046 000C 000A 0003 0000 0009"            /* ‘....F...¬.....∆ */
	$"0000 0000 0000 0000 0081 000C 0009 0015"            /* .........Å...∆.. */
	$"0100 000A DD6B 08C2 06A2 0000 00A5 0010"            /* ...¬›k.¬.¢...•.. */
	$"000C 0003 0100 000C 0000 0000 D400 0000"            /* ............‘... */
	$"00A6 000C 000A 0003 0100 0009 0000 0000"            /* .¶...¬.....∆.... */
	$"D400 0000 011D 000C 000A 0003 0100 0009"            /* ‘........¬.....∆ */
	$"EFEF 1F1F 1D1D 0000 013C 000C 000A 0003"            /* ÔÔ.......<...¬.. */
	$"0000 0009 0000 0000 0000 0000 0369 0010"            /* ...∆.........i.. */
	$"000C 0003 0100 000C DD6B 08C2 06A2 0000"            /* ........›k.¬.¢.. */
	$"036A 000C 000A 0003 0000 0009 0000 0000"            /* .j...¬.....∆.... */
	$"0000 0000 03A2 0010 000C 0003 0000 000C"            /* .....¢.......... */
	$"0000 0000 0000 0000 03A3 000C 000A 0003"            /* .........£...¬.. */
	$"0100 0009 0000 0000 D400 0000 03C8 000C"            /* ...∆....‘....».. */
	$"000A 0003 0000 0009 0000 0000 0000 0000"            /* .¬.....∆........ */
	$"0465 000C 000A 0003 0100 0009 0000 0000"            /* .e...¬.....∆.... */
	$"0000 0000 047D 000C 000A 0003 0000 0009"            /* .....}...¬.....∆ */
	$"0000 0000 0000 0000 048C 000C 000A 0003"            /* .........å...¬.. */
	$"0100 0009 DD6B 08C2 06A2 0000 049C 000C"            /* ...∆›k.¬.¢...ú.. */
	$"000A 0003 0000 0009 0000 0000 0000 0000"            /* .¬.....∆........ */
	$"04AE 000B 0009 0004 0000 0009 0000 0000"            /* .Æ...∆.....∆.... */
	$"0000 0000 04E5 000C 000A 0003 0000 0009"            /* .....Â...¬.....∆ */
	$"0000 0000 0000 0000 04E6 000C 000A 0003"            /* .........Ê...¬.. */
	$"0500 0009 0000 0000 0000 0000 04EC 000C"            /* ...∆.........Ï.. */
	$"000A 0003 0000 0009 0000 0000 0000 0000"            /* .¬.....∆........ */
	$"04EE 000C 000A 0003 0100 0009 0000 0000"            /* .Ó...¬.....∆.... */
	$"0000 0000 04F3 000C 000A 0003 0000 0009"            /* .....Û...¬.....∆ */
	$"0000 0000 0000 0000 04F5 000B 0009 0004"            /* .........ı...∆.. */
	$"0000 0009 0000 0000 0000 0000 0501 000C"            /* ...∆............ */
	$"000A 0003 0000 0009 0000 0000 0000 0000"            /* .¬.....∆........ */
	$"0502 000C 000A 0003 0100 0009 0000 0000"            /* .....¬.....∆.... */
	$"0000 0000 0505 000C 000A 0003 0000 0009"            /* .........¬.....∆ */
	$"0000 0000 0000 0000 0507 000B 0009 0004"            /* .............∆.. */
	$"0000 0009 0000 0000 0000 0000 0513 000C"            /* ...∆............ */
	$"000A 0003 0000 0009 0000 0000 0000 0000"            /* .¬.....∆........ */
	$"0514 000C 000A 0003 0100 0009 0000 0000"            /* .....¬.....∆.... */
	$"0000 0000 0519 000C 000A 0003 0000 0009"            /* .........¬.....∆ */
	$"0000 0000 0000 0000 0530 000C 000A 0003"            /* .........0...¬.. */
	$"0100 0009 0000 0000 0000 0000 0533 000C"            /* ...∆.........3.. */
	$"000A 0003 0000 0009 0000 0000 0000 0000"            /* .¬.....∆........ */
	$"0594 000C 000A 0003 0100 0009 0000 0000"            /* .î...¬.....∆.... */
	$"0000 0000 05B4 000C 000A 0003 0000 0009"            /* .....¥...¬.....∆ */
	$"0000 0000 0000 0000 05C3 000C 000A 0003"            /* .........√...¬.. */
	$"0100 0009 DD6B 08C2 06A2 0000 05D1 000C"            /* ...∆›k.¬.¢...—.. */
	$"000A 0003 0000 0009 0000 0000 0000 0000"            /* .¬.....∆........ */
	$"05F2 000C 000A 0003 0500 0009 0000 0000"            /* .Ú...¬.....∆.... */
	$"0000 0000 0604 000C 000A 0003 0000 0009"            /* .........¬.....∆ */
	$"0000 0000 0000 0000 0606 000C 000A 0003"            /* .............¬.. */
	$"0100 0009 0000 0000 D400 0000 060B 000C"            /* ...∆....‘....... */
	$"000A 0003 0000 0009 0000 0000 0000 0000"            /* .¬.....∆........ */
	$"0626 000C 000A 0003 0100 0009 0000 0000"            /* .&...¬.....∆.... */
	$"D400 0000 062B 000C 000A 0003 0000 0009"            /* ‘....+...¬.....∆ */
	$"0000 0000 0000 0000 0640 000C 000A 0003"            /* .........@...¬.. */
	$"0100 0009 0000 0000 D400 0000 0643 000C"            /* ...∆....‘....C.. */
	$"000A 0003 0000 0009 0000 0000 0000 0000"            /* .¬.....∆........ */
	$"0658 000C 000A 0003 0100 0009 0000 0000"            /* .X...¬.....∆.... */
	$"D400 0000 0660 000C 000A 0003 0000 0009"            /* ‘....`...¬.....∆ */
	$"0000 0000 0000 0000 0679 000C 000A 0003"            /* .........y...¬.. */
	$"0100 0009 0000 0000 D400 0000 0682 000C"            /* ...∆....‘....Ç.. */
	$"000A 0003 0000 0009 0000 0000 0000 0000"            /* .¬.....∆........ */
	$"0688 000C 000A 0001 0000 0009 0000 0000"            /* .à...¬.....∆.... */
	$"0000 0000 06D0 000C 000A 0003 0000 0009"            /* .....–...¬.....∆ */
	$"0000 0000 0000 0000 06FC 000C 000A 0003"            /* .........¸...¬.. */
	$"0100 0009 0000 0000 D400 0000 06FF 000C"            /* ...∆....‘....ˇ.. */
	$"000A 0003 0000 0009 0000 0000 0000 0000"            /* .¬.....∆........ */
	$"071E 000C 000A 0001 0000 0009 0000 0000"            /* .....¬.....∆.... */
	$"0000 0000 0731 000B 0008 0015 0000 0009"            /* .....1.........∆ */
	$"0000 0000 0000 0000 0732 000C 000A 0003"            /* .........2...¬.. */
	$"0000 0009 0000 0000 0000 0000 078B 000C"            /* ...∆.........ã.. */
	$"000A 0003 0100 0009 DD6B 08C2 06A2 0000"            /* .¬.....∆›k.¬.¢.. */
	$"0799 000C 000A 0003 0000 0009 0000 0000"            /* .ô...¬.....∆.... */
	$"0000 0000 07D1 000C 000A 0003 0100 0009"            /* .....—...¬.....∆ */
	$"DD6B 08C2 06A2 0000 07DD 000C 000A 0003"            /* ›k.¬.¢...›...¬.. */
	$"0000 0009 0000 0000 0000 0000 07F4 000C"            /* ...∆.........Ù.. */
	$"000A 0003 0100 0009 DD6B 08C2 06A2 0000"            /* .¬.....∆›k.¬.¢.. */
	$"07FF 000C 000A 0003 0000 0009 0000 0000"            /* .ˇ...¬.....∆.... */
	$"0000 0000 081E 000C 000A 0003 0100 0009"            /* .........¬.....∆ */
	$"DD6B 08C2 06A2 0000 082A 000C 000A 0003"            /* ›k.¬.¢...*...¬.. */
	$"0000 0009 0000 0000 0000 0000 0864 000C"            /* ...∆.........d.. */
	$"000A 0003 0100 0009 DD6B 08C2 06A2 0000"            /* .¬.....∆›k.¬.¢.. */
	$"0872 000C 000A 0003 0000 0009 0000 0000"            /* .r...¬.....∆.... */
	$"0000 0000 08C5 000C 000A 0003 0100 0009"            /* .....≈...¬.....∆ */
	$"DD6B 08C2 06A2 0000 08D0 000C 000A 0003"            /* ›k.¬.¢...–...¬.. */
	$"0000 0009 0000 0000 0000 0000 08F8 000C"            /* ...∆.........¯.. */
	$"000A 0003 0100 0009 0000 0000 D400 0000"            /* .¬.....∆....‘... */
	$"0905 000C 000A 0003 0000 0009 0000 0000"            /* ∆....¬.....∆.... */
	$"0000 0000 098A 000C 000A 0003 0100 0009"            /* ....∆ä...¬.....∆ */
	$"0000 0000 D400 0000 09BE 000C 000A 0003"            /* ....‘...∆æ...¬.. */
	$"0000 0009 0000 0000 0000 0000 0A23 000C"            /* ...∆........¬#.. */
	$"000A 0003 0100 0009 F2D7 0856 84EC 0000"            /* .¬.....∆Ú◊.VÑÏ.. */
	$"0A29 000C 000A 0003 0000 0009 0000 0000"            /* ¬)...¬.....∆.... */
	$"0000 0000 0B11 000C 000A 0003 0100 0009"            /* .........¬.....∆ */
	$"0000 0000 D400 0000 0B1C 000C 000A 0003"            /* ....‘........¬.. */
	$"0000 0009 0000 0000 0000 0000 0B1D 000C"            /* ...∆............ */
	$"000A 0003 0100 0009 DD6B 08C2 06A2 0000"            /* .¬.....∆›k.¬.¢.. */
	$"0B2B 000C 000A 0003 0000 0009 0000 0000"            /* .+...¬.....∆.... */
	$"0000 0000 0DA8 000C 000A 0003 0100 0009"            /* .....®...¬.....∆ */
	$"0000 0000 D400 0000 0DC7 000C 000A 0003"            /* ....‘....«...¬.. */
	$"0000 0009 0000 0000 0000 0000 0DDA 000C"            /* ...∆.........⁄.. */
	$"000A 0003 0100 0009 0000 0000 D400 0000"            /* .¬.....∆....‘... */
	$"0DDE 000C 000A 0003 0000 0009 0000 0000"            /* .ﬁ...¬.....∆.... */
	$"0000 0000 0EDE 000C 000A 0003 0100 0009"            /* .....ﬁ...¬.....∆ */
	$"0000 0000 D400 0000 0EE6 000C 000A 0003"            /* ....‘....Ê...¬.. */
	$"0000 0009 0000 0000 0000 0000 0F7D 000C"            /* ...∆.........}.. */
	$"000A 0003 0100 0009 DD6B 08C2 06A2 0000"            /* .¬.....∆›k.¬.¢.. */
	$"0F8B 000C 000A 0003 0000 0009 0000 0000"            /* .ã...¬.....∆.... */
	$"0000 0000 0FAC 000C 000A 0003 0500 0009"            /* .....¨...¬.....∆ */
	$"0000 0000 0000 0000 0FBE 000C 000A 0003"            /* .........æ...¬.. */
	$"0000 0009 0000 0000 0000 0000 0FBF 000E"            /* ...∆.........ø.. */
	$"000A 0015 0000 000C 0000 0000 0000 0000"            /* .¬.............. */
	$"0FC0 000C 000A 0003 0100 0009 0000 0000"            /* .¿...¬.....∆.... */
	$"D400 0000 0FC5 000C 000A 0003 0000 0009"            /* ‘....≈...¬.....∆ */
	$"0000 0000 0000 0000 0FE0 000C 000A 0003"            /* .........‡...¬.. */
	$"0100 0009 0000 0000 D400 0000 0FE5 000C"            /* ...∆....‘....Â.. */
	$"000A 0003 0000 0009 0000 0000 0000 0000"            /* .¬.....∆........ */
	$"0FFA 000C 000A 0003 0100 0009 0000 0000"            /* .˙...¬.....∆.... */
	$"D400 0000 0FFD 000C 000A 0003 0000 0009"            /* ‘....˝...¬.....∆ */
	$"0000 0000 0000 0000 1011 000E 000A 0015"            /* .............¬.. */
	$"0000 000C 0000 0000 0000 0000 1012 000C"            /* ................ */
	$"000A 0003 0100 0009 0000 0000 D400 0000"            /* .¬.....∆....‘... */
	$"1015 000C 000A 0003 0000 0009 0000 0000"            /* .....¬.....∆.... */
	$"0000 0000 102C 000C 000A 0001 0000 0009"            /* .....,...¬.....∆ */
	$"0000 0000 0000 0000 103F 000C 000A 0003"            /* .........?...¬.. */
	$"0000 0009 0000 0000 0000 0000 1054 000C"            /* ...∆.........T.. */
	$"000A 0003 0100 0009 0000 0000 D400 0000"            /* .¬.....∆....‘... */
	$"105C 000C 000A 0003 0000 0009 0000 0000"            /* .\...¬.....∆.... */
	$"0000 0000 105F 000C 000A 0003 0100 0009"            /* ....._...¬.....∆ */
	$"0000 0000 D400 0000 1068 000C 000A 0003"            /* ....‘....h...¬.. */
	$"0000 0009 0000 0000 0000 0000 1095 000C"            /* ...∆.........ï.. */
	$"000A 0003 0100 0009 0000 0000 D400 0000"            /* .¬.....∆....‘... */
	$"109B 000C 000A 0003 0000 0009 0000 0000"            /* .õ...¬.....∆.... */
	$"0000"                                               /* .. */
};

data 'styl' (400) {
	$"0001 0000 0000 0010 000C 0000 000C 000C"            /* ................ */
	$"0000 0000 0000"                                     /* ...... */
};

resource 'STR#' (128, "Errors")
{
	{
		"PlayerPRO can only read 11Khz or 22Khz sounds.",
		"There isn’t enough memory for this sound. Allocate more memory for PlayerPRO using the Finder’s “Get Info.”",
		"This program requires 32-bit addressing. Use the Memory Control Panel to set your system to 32-bit mode and restart.",
		"No sounds/music could be found in this file. Check this file.",
		"A file error has occured. There probably wasn’t enough room.",
		"MacOS 8.0 or higher is required by PlayerPRO.",
		"This sound uses a compression format which can't be read by Player PRO. Please refer to the documentation.",
		"Your changes will not be in effect until the next MOD file is opened.",
		"Memory is getting critically low.  Allocate more memory for Player PRO using the Finder’s “Get Info.”",
		"This music couldn't be found.",
		"This driver is only available on the AV series Macintoshs.",
		"Sorry, but this is the un-registered version of PlayerPRO! You need to register PlayerPRO to do this!",
		"To do this, you will need to select an instrument in the “Instrument List” window.",
		"Do you really want to replace this instrument?",
		"This MOD file is out of date. Should I update it?",
		"This Macintosh is able to use a higher-performance driver. It's better to use the ROSSET Routines MOD 2.",
		"Your Macintosh is unable to play sound in stereo. Buy another Macintosh!",
		"Where is the Apple Sound Chip (ASC)? What is this computer?",
		"Sorry, but you don’t have a microphone.",
		"The instrument loop is incorrect. Please fix it.",
		"Are you sure that you want to delete this instrument?",
		"Sorry, but you can't use this driver without an Apple Sound Chip.",
		"You must be in 256 color mode and have a 640x480 monitor to use this function.",
		"Sorry, but that sound is too large. Instruments must be smaller than 128K.",
		"Please rebuild your Desktop! (Reboot while holding the Command and Option keys.)",
		"Sorry, but that response was invalid. Please enter a number between 0 and the highest pattern.",
		"PlayerPRO requires the Sound Manager 3.0 to run.",
		"Sorry, but the length of the partition must be between 1 and 128.",
		"Sorry, but this registration code is incorrect.",
		"Please enter your name.",
		"This program has already been registered.",
		"The un-registered version of PlayerPRO is limited to only 20 minutes! Please support PlayerPRO development!",
		"Sorry, but PlayerPRO requires AppleEvents.",
		"Sorry, but there’s nothing in this window to print.",
		"This MIDI file is unrecognizable. It doesn’t seem to be a SMF (MThd) file.",
		"In your pattern list, the higher pattern number MUST appear at least one time!",
		"Are you sure that you want to replace the current partition with this partition of the MIDI file?",
		"Sorry, but nothing is selected.",
		"This MIDI file has no samples. Samples are required if you want to hear something!",
		"Sorry, but the “Plug-In” modules returned an error. Please refer to the documentation.",
		"Sorry, but PlayerPRO is UNREGISTERED, and your 15 day trial period is over. You must either purchase this program or delete it!",
		"This function doesn't work in this version.",
		"It's not a valid value!",
		"I cannot open this file.\nFile Type Error.",
		"I don't have Import Methode to open this file.",
		"Are you sure that you want to delete notes of this track and pattern?",
		"WARNING - This is a beta version of this Import Plug!",
		"Are you sure that you want to purge this pattern?",
		"I cannot delete this pattern. Your music file MUST have 1 pattern at least.",
		"Are you sure that you want to delete this pattern?",
		"Do you want to replace the selected pattern or load it as a new pattern?",
		"This PlayerPRO version supports pattern size from 1 to 999. Change it, please!",
		"Do you really want to change the amplitude of this sound?",
		"Printer Driver Error...",
		"Use PtMid program to convert Midi files into Player PRO files.",
		"I cannot find any help file.",
		"MIDI Hardware Error. Check PatchBay Control Panel and your connections.",
		"MIDI Factor value must be between 0% and 500% of the velocity value.",
		"Error in 'argument' value. It's a hexadecimal from 0 to FF.",
		"Correct the file type & creator of this file before opening?",
		"This SoundInPut device doesn't support options dialog.",
		"File not found!\nI cannot find this music on your hard disk.",
		"Not enough memory to do this operation. Allocate more memory to PlayerPRO.",
		"The name “^2” is already taken by another file. Please use a different name.",
		"Give a name to this sound, if you want to create a file!",
		"Sorry, but sample rate (c2spd) must be between 1 Hz and 50 Khz.",
		"PLUG ERROR: This function is not supported by this plug. (orderNotImplemented)",
		"PLUG ERROR: Not enough memory to do this operation. Allocate more memory to PlayerPRO. (needMoreMemory)",
		"PLUG ERROR: This file format isn't supported by this plug. (fileNotSupportedByThisPlug)",
		"PLUG ERROR: Unknown Error. (unknowError)",
		"PLUG ERROR: PlayerPRO cannot find a plug to open this file format.\n(cannotFindPlug)",
		"You don't have 'QuickTime™ Musical Instruments' extension. You cannot use this function.",
		"Error: PlayerPRO cannot open 'Help PP' resources.",
		"Error: I cannot find an input device.",
		"Writing Error: volume write protected.",
		"Sorry, but RealNote must be between -96 and 96.",
		"Sorry, but Volume must be between 0 and 64.",
		"Sorry, but Volume Fade must be between 0 and 32767.",
		"Sorry, maximum points for volume envelope is 12.",
		"Error in 'volume' value. It's a hexadecimal from 0 to FF.",
		"Sorry, but the unregistered version cannot open this kind of files. Only registered version can open all sound/music files.",
		"You have to turn OFF Recording state. See Tools Window.",
		"Writing Error. I cannot write this file on this hard disk.",
		"Please enter your country.",
		"Please select the distributor, by clicking on the pop-up menu.",
		"PLUG ERROR: PlayerPRO cannot find the plug to do this operation.\n(cannotFindPlug)",
		"Your 'PlayerPRO Pref' file was CORRUPTED. It has been deleted!",
		"WARNING - This function doesn't support UNDO. This function will remove all unused data of this music. Continue?",
		"I'm sorry, this function needs Drag Manager Extension. It is available on your system disks.",
		"PlayerPRO has a problem to identify your name & your registration ID. Check it!",
		"Do you really want to change the mode of this sound?",
		"PlayerPRO requires Appearance Manager extension. Please update your MacOS to version 8.0 or higher.",
		"This beta version has expired. Please download final version on www.quadmation.com/pphome.htm",
		"WARNING !\nYou are entering in a protected area.",
		"PlayerPRO requires Quicktime extension version 2.5 or higher.",
		"PlayerPRO is not able to open this kind of file.",
		"There are NO space between letters in the registration code! Remove them please.",
		"Not enough memory to load your internet browser. Closing applications can make more memory available.",
		"Internet Connection Error.",
		"Are you sure that you want to reset the URLs list to the default one?",
		"This version of PlayerPRO requires a PowerPC G4 with Altivec support.",
		"Are you sure that you want to delete this music file?\nYou cannot undo this operation.",
		"You need to open “Digital Editor” window to use this function.",
		"Are you sure that you want to delete this VST settings?",
		"Are you sure that you want to replace these settings with current one?",
		"This file is compressed in “.zip” format. You need to uncompress it before PlayerPRO can use it.",
		"Some VST Plugs are not available: ",
		"MPEG4 requires Quicktime 6 or higher. You can download it on http://www.apple.com/quicktime",
		
	}
};

data 'STR#' (132, "Note") {
	$"0024 0342 2036 0342 2336 0341 2036 0341"            /* .$.B 6.B#6.A 6.A */
	$"2336 0347 2336 0347 2036 0346 2336 0346"            /* #6.G#6.G 6.F#6.F */
	$"2036 0345 2036 0344 2336 0344 2036 0343"            /*  6.E 6.D#6.D 6.C */
	$"2336 0343 2036 0342 2035 0341 2335 0347"            /* #6.C 6.B 5.A#5.G */
	$"2335 0347 2035 0346 2335 0346 2035 0345"            /* #5.G 5.F#5.F 5.E */
	$"2035 0344 2335 0344 2035 0343 2335 0343"            /*  5.D#5.D 5.C#5.C */
	$"2035 0342 2034 0341 2334 0341 2034 0347"            /*  5.B 4.A#4.A 4.G */
	$"2334 0347 2034 0346 2334 0346 2034 0345"            /* #4.G 4.F#4.F 4.E */
	$"2034 0344 2334 0344 2034 0343 2334 0343"            /*  4.D#4.D 4.C#4.C */
	$"2034"                                               /*  4 */
};

data 'STR#' (129, "ScreenSaver") {
	$"0007 0E50 6C61 7965 7220 5052 4F20 352E"            /* ...Player PRO 5. */
	$"300B 596F 7520 6361 6E20 6E6F 770A 6F72"            /* 0.You can now¬or */
	$"6465 7220 796F 7572 1050 6C61 7965 7250"            /* der your.PlayerP */
	$"524F 2043 442D 526F 6D10 7769 7468 2033"            /* RO CD-Rom.with 3 */
	$"3530 3020 6D75 7369 6373 0F35 3030 2069"            /* 500 musics.500 i */
	$"6E73 7472 756D 656E 7473 0F61 6E64 206D"            /* nstruments.and m */
	$"7573 6963 2074 6F6F 6C73"                           /* usic tools */
};

data 'STR#' (200, "Instruments list help") {
	$"0009 4843 7265 6174 6520 6120 6E65 7720"            /* .∆HCreate a new  */
	$"736F 756E 6420 696E 746F 2074 6865 2073"            /* sound into the s */
	$"656C 6563 7465 6420 696E 7374 7275 6D65"            /* elected instrume */
	$"6E74 2063 656C 6C2E 2043 686F 6F73 6520"            /* nt cell. Choose  */
	$"7468 6520 6D65 7468 6F64 2E29 5361 7665"            /* the method.)Save */
	$"2074 6865 2073 656C 6563 7465 6420 696E"            /*  the selected in */
	$"7374 7275 6D65 6E74 2069 6E74 6F20 6120"            /* strument into a  */
	$"6669 6C65 2E34 4C6F 6164 2061 2073 6F75"            /* file.4Load a sou */
	$"6E64 2066 696C 6520 696E 746F 2074 6865"            /* nd file into the */
	$"2073 656C 6563 7465 6420 696E 7374 7275"            /*  selected instru */
	$"6D65 6E74 2063 656C 6C2E 1F44 656C 6574"            /* ment cell..Delet */
	$"6520 7468 6520 7365 6C65 6374 6564 2069"            /* e the selected i */
	$"6E73 7472 756D 656E 742E 2F4D 6F72 6520"            /* nstrument./More  */
	$"696E 666F 726D 6174 696F 6E20 6162 6F75"            /* information abou */
	$"7420 7468 6520 7365 6C65 6374 6564 2069"            /* t the selected i */
	$"6E73 7472 756D 656E 742E 3E54 6865 2063"            /* nstrument.>The c */
	$"6F6C 6F72 2069 6E64 6963 6174 6573 2074"            /* olor indicates t */
	$"6865 2074 7261 636B 206F 6E20 7768 6963"            /* he track on whic */
	$"6820 7468 6520 696E 7374 7275 6D65 6E74"            /* h the instrument */
	$"2069 7320 7573 6564 2E2D 4C69 7374 206F"            /*  is used.-List o */
	$"6620 696E 7374 7275 6D65 6E74 7320 7573"            /* f instruments us */
	$"6564 2069 6E20 7468 6520 6375 7272 656E"            /* ed in the curren */
	$"7420 736F 6E67 2E24 506C 6179 2074 6865"            /* t song.$Play the */
	$"2073 656C 6563 7465 6420 696E 7374 7275"            /*  selected instru */
	$"6D65 6E74 2F73 616D 706C 652E 2D53 686F"            /* ment/sample.-Sho */
	$"7720 7468 6520 7761 7665 666F 726D 206F"            /* w the waveform o */
	$"6620 7468 6520 7365 6C65 6374 6564 2069"            /* f the selected i */
	$"6E73 7472 756D 656E 742E"                           /* nstrument. */
};

data 'STR#' (1000, "PatList Help") {
	$"0007 2C43 7265 6174 6520 6120 6E65 7720"            /* ..,Create a new  */
	$"7061 7474 6572 6E20 616E 6420 6164 6420"            /* pattern and add  */
	$"6974 2074 6F20 7468 6520 6C69 7374 2E2B"            /* it to the list.+ */
	$"4C6F 6164 2061 2070 6174 7465 726E 2066"            /* Load a pattern f */
	$"696C 6520 616E 6420 6164 6420 6974 2074"            /* ile and add it t */
	$"6F20 7468 6520 6C69 7374 2E26 5361 7665"            /* o the list.&Save */
	$"2074 6865 2073 656C 6563 7465 6420 7061"            /*  the selected pa */
	$"7474 6572 6E20 696E 746F 2061 2066 696C"            /* ttern into a fil */
	$"652E 1C44 656C 6574 6520 7468 6520 7365"            /* e..Delete the se */
	$"6C65 6374 6564 2070 6174 7465 726E 2E30"            /* lected pattern.0 */
	$"4765 7420 6D6F 7265 2069 6E66 6F72 6D61"            /* Get more informa */
	$"7469 6F6E 2061 626F 7574 2074 6865 2073"            /* tion about the s */
	$"656C 6563 7465 6420 7061 7474 6572 6E2E"            /* elected pattern. */
	$"1D44 6973 706C 6179 2074 6865 2073 656C"            /* .Display the sel */
	$"6563 7465 6420 7061 7474 6572 6E2E 2A54"            /* ected pattern.*T */
	$"6865 2070 6174 7465 726E 206C 6973 7420"            /* he pattern list  */
	$"7573 6564 2069 6E20 7468 6520 6375 7272"            /* used in the curr */
	$"656E 7420 736F 6E67 2E"                             /* ent song. */
};

data 'STR#' (1200, "Adaptors Help") {
	$"000A 354C 6F61 6420 616E 2027 6164 6170"            /* .¬5Load an 'adap */
	$"746F 7273 2720 6669 6C65 2061 6E64 2072"            /* tors' file and r */
	$"6570 6C61 6365 2063 7572 7265 6E74 2073"            /* eplace current s */
	$"6574 7469 6E67 732E 2D53 6176 6520 6375"            /* ettings.-Save cu */
	$"7272 656E 7420 7365 7474 696E 6773 2069"            /* rrent settings i */
	$"6E74 6F20 6120 2761 6461 7074 6F72 7327"            /* nto a 'adaptors' */
	$"2066 696C 652E 2952 6573 6574 2073 7065"            /*  file.)Reset spe */
	$"6564 2073 6574 7469 6E67 2074 6F20 7374"            /* ed setting to st */
	$"616E 6461 7264 2073 6574 7469 6E67 732E"            /* andard settings. */
	$"2D53 6574 2075 7020 6C65 6674 2F72 6967"            /* -Set up left/rig */
	$"6874 2076 6F6C 756D 6520 6261 7365 6420"            /* ht volume based  */
	$"6F6E 2074 7261 636B 206E 6F20 312E 2643"            /* on track no 1.&C */
	$"6861 6E67 6520 7468 6520 706C 6179 696E"            /* hange the playin */
	$"6720 7069 7463 682E 2043 6C69 636B 206F"            /* g pitch. Click o */
	$"6E20 6974 2E26 4368 616E 6765 2074 6865"            /* n it.&Change the */
	$"2070 6C61 7969 6E67 2073 7065 6564 2E20"            /*  playing speed.  */
	$"436C 6963 6B20 6F6E 2069 742E 3943 6861"            /* Click on it.9Cha */
	$"6E67 6520 7468 6520 706C 6179 696E 6720"            /* nge the playing  */
	$"766F 6C75 6D65 2028 4841 5244 5741 5245"            /* volume (HARDWARE */
	$"2076 6F6C 756D 6529 2E20 436C 6963 6B20"            /*  volume). Click  */
	$"6F6E 2069 742E 5943 6861 6E67 6520 766F"            /* on it.YChange vo */
	$"6C75 6D65 2F70 616E 6E69 6E67 2066 6F72"            /* lume/panning for */
	$"2065 6163 6820 7472 6163 6B2E 2043 6C69"            /*  each track. Cli */
	$"636B 206F 6E20 6974 2E20 5573 6520 276F"            /* ck on it. Use 'o */
	$"7074 696F 6E27 206B 6579 2074 6F20 6368"            /* ption' key to ch */
	$"616E 6765 2061 6C6C 2074 7261 636B 732E"            /* ange all tracks. */
	$"3943 6861 6E67 6520 7468 6520 706C 6179"            /* 9Change the play */
	$"696E 6720 766F 6C75 6D65 2028 534F 4654"            /* ing volume (SOFT */
	$"5741 5245 2076 6F6C 756D 6529 2E20 436C"            /* WARE volume). Cl */
	$"6963 6B20 6F6E 2069 742E 2952 6573 6574"            /* ick on it.)Reset */
	$"2070 6974 6368 2073 6574 7469 6E67 2074"            /*  pitch setting t */
	$"6F20 7374 616E 6461 7264 2073 6574 7469"            /* o standard setti */
	$"6E67 732E"                                          /* ngs. */
};

data 'STR#' (1400, "Music List Help") {
	$"0009 2353 6F72 7420 7468 6520 6D75 7369"            /* .∆#Sort the musi */
	$"6320 6C69 7374 2061 6C70 6861 6265 7469"            /* c list alphabeti */
	$"6361 6C6C 792E 2741 6464 2061 206E 6577"            /* cally.'Add a new */
	$"206D 7573 6963 2066 696C 6520 746F 2074"            /*  music file to t */
	$"6865 206D 7573 6963 206C 6973 742E 2D52"            /* he music list.-R */
	$"656D 6F76 6520 7468 6520 7365 6C65 6374"            /* emove the select */
	$"6564 206D 7573 6963 2066 696C 6520 6672"            /* ed music file fr */
	$"6F6D 2074 6865 206C 6973 742E 2E43 6861"            /* om the list..Cha */
	$"6E67 6520 696E 666F 726D 6174 696F 6E20"            /* nge information  */
	$"6F66 2074 6865 2073 656C 6563 7465 6420"            /* of the selected  */
	$"6D75 7369 6320 6669 6C65 2E35 4C6F 6164"            /* music file.5Load */
	$"2074 6865 2073 656C 6563 7465 6420 6D75"            /*  the selected mu */
	$"7369 6320 6669 6C65 2069 6E74 6F20 6D65"            /* sic file into me */
	$"6D6F 7279 2061 6E64 2070 6C61 7920 6974"            /* mory and play it */
	$"2E2E 4F70 656E 2074 6865 2070 7265 6665"            /* ..Open the prefe */
	$"7265 6E63 6520 7769 6E64 6F77 2066 6F72"            /* rence window for */
	$"2074 6865 206D 7573 6963 206C 6973 742E"            /*  the music list. */
	$"2F4D 6F72 6520 696E 666F 726D 6174 696F"            /* /More informatio */
	$"6E20 6162 6F75 7420 7468 6520 7365 6C65"            /* n about the sele */
	$"6374 6564 206D 7573 6963 2066 696C 652E"            /* cted music file. */
	$"4154 6865 206D 7573 6963 206C 6973 7420"            /* AThe music list  */
	$"2D20 746F 206F 7065 6E20 6120 6D75 7369"            /* - to open a musi */
	$"6320 6669 6C65 2C20 646F 7562 6C65 2063"            /* c file, double c */
	$"6C69 636B 206F 6E20 6974 7320 7469 746C"            /* lick on its titl */
	$"652E 3A54 6865 2070 6F73 6974 696F 6E20"            /* e.:The position  */
	$"6F66 2074 6865 2073 656C 6563 7465 6420"            /* of the selected  */
	$"6D75 7369 6320 6669 6C65 206F 6E20 796F"            /* music file on yo */
	$"7572 2068 6172 6420 6469 736B 2E"                   /* ur hard disk. */
};

data 'STR#' (1600, "Tools Help") {
	$"000F 1747 6F20 746F 2070 7265 7669 6F75"            /* ...Go to previou */
	$"7320 7061 7474 6572 6E2E 1450 6C61 7920"            /* s pattern..Play  */
	$"736F 6E67 2062 6163 6B77 6172 6473 2E16"            /* song backwards.. */
	$"5374 6F70 2074 6865 2063 7572 7265 6E74"            /* Stop the current */
	$"2073 6F6E 672E 1650 6C61 7920 7468 6520"            /*  song..Play the  */
	$"6375 7272 656E 7420 736F 6E67 2E0D 4661"            /* current song..Fa */
	$"7374 2066 6F72 7761 7264 2E13 476F 2074"            /* st forward..Go t */
	$"6F20 6E65 7874 2070 6174 7465 726E 2E49"            /* o next pattern.I */
	$"496E 6469 6361 7465 2074 6865 2070 726F"            /* Indicate the pro */
	$"6772 6573 7369 6F6E 206F 6620 7468 6520"            /* gression of the  */
	$"736F 6E67 2E20 436C 6963 6B20 696E 2069"            /* song. Click in i */
	$"7420 746F 2063 6861 6E67 6520 7468 6520"            /* t to change the  */
	$"706F 7369 7469 6F6E 2E27 4669 6C65 6E61"            /* position.'Filena */
	$"6D65 206F 6620 7468 6520 6375 7272 656E"            /* me of the curren */
	$"7420 736F 6E67 2069 6E20 6D65 6D6F 7279"            /* t song in memory */
	$"2E35 496E 7374 7275 6D65 6E74 206F 6620"            /* .5Instrument of  */
	$"7468 6520 7365 6C65 6374 6564 2063 656C"            /* the selected cel */
	$"6C20 2873 6565 2044 6967 6974 616C 2045"            /* l (see Digital E */
	$"6469 746F 7229 2E2F 4E6F 7465 206F 6620"            /* ditor)./Note of  */
	$"7468 6520 7365 6C65 6374 6564 2063 656C"            /* the selected cel */
	$"6C20 2873 6565 2044 6967 6974 616C 2045"            /* l (see Digital E */
	$"6469 746F 7229 2E31 4566 6665 6374 206F"            /* ditor).1Effect o */
	$"6620 7468 6520 7365 6C65 6374 6564 2063"            /* f the selected c */
	$"656C 6C20 2873 6565 2044 6967 6974 616C"            /* ell (see Digital */
	$"2045 6469 746F 7229 2E33 4172 6775 6D65"            /*  Editor).3Argume */
	$"6E74 206F 6620 7468 6520 7365 6C65 6374"            /* nt of the select */
	$"6564 2063 656C 6C20 2873 6565 2044 6967"            /* ed cell (see Dig */
	$"6974 616C 2045 6469 746F 7229 2E2C 506C"            /* ital Editor).,Pl */
	$"6179 2074 6865 2073 656C 6563 7465 6420"            /* ay the selected  */
	$"6365 6C6C 2028 7365 6520 4469 6769 7461"            /* cell (see Digita */
	$"6C20 4564 6974 6F72 292E 2E44 656C 6574"            /* l Editor)..Delet */
	$"6520 7468 6520 7365 6C65 6374 6564 2063"            /* e the selected c */
	$"656C 6C20 2873 6565 2044 6967 6974 616C"            /* ell (see Digital */
	$"2045 6469 746F 7229 2E3F 4170 706C 7920"            /*  Editor).?Apply  */
	$"6D6F 6469 6669 6361 7469 6F6E 7320 746F"            /* modifications to */
	$"2061 6C6C 2073 656C 6563 7465 6420 6365"            /*  all selected ce */
	$"6C6C 7320 2873 6565 2044 6967 6974 616C"            /* lls (see Digital */
	$"2045 6469 746F 7229 2E"                             /*  Editor). */
};

data 'STR#' (1800, "Instrument Help") {
	$"0009 2943 6861 6E67 6520 696E 666F 726D"            /* .∆)Change inform */
	$"6174 696F 6E20 6162 6F75 7420 7468 6973"            /* ation about this */
	$"2069 6E73 7472 756D 656E 742E 4B53 7769"            /*  instrument.KSwi */
	$"7463 6820 746F 2073 656C 6563 7469 6F6E"            /* tch to selection */
	$"206D 6F64 653A 2074 6F20 6170 706C 7920"            /*  mode: to apply  */
	$"706C 7567 732C 2063 7574 2C20 636F 7079"            /* plugs, cut, copy */
	$"2061 6E64 2070 6173 7465 2074 6865 2077"            /*  and paste the w */
	$"6176 6566 6F72 6D2E 2E53 7769 7463 6820"            /* aveform..Switch  */
	$"746F 2070 656E 6369 6C20 6D6F 6465 3A20"            /* to pencil mode:  */
	$"746F 2072 6564 7261 7720 7468 6520 7761"            /* to redraw the wa */
	$"7665 666F 726D 2E51 5377 6974 6368 2074"            /* veform.QSwitch t */
	$"6F20 7A6F 6F6D 2049 4E2F 4F55 5420 6D6F"            /* o zoom IN/OUT mo */
	$"6465 2E20 546F 207A 6F6F 6D20 494E 3A20"            /* de. To zoom IN:  */
	$"636C 6963 6B2C 2074 6F20 7A6F 6F6D 204F"            /* click, to zoom O */
	$"5554 3A20 636C 6963 6B20 2B20 276F 7074"            /* UT: click + 'opt */
	$"696F 6E27 206B 6579 2E4D 4170 706C 7920"            /* ion' key.MApply  */
	$"6120 706C 7567 2074 6F20 7468 6520 7365"            /* a plug to the se */
	$"6C65 6374 696F 6E2E 2049 6620 6E6F 7468"            /* lection. If noth */
	$"696E 6720 6973 2073 656C 6563 7465 642C"            /* ing is selected, */
	$"2074 6F20 7468 6520 7768 6F6C 6520 7761"            /*  to the whole wa */
	$"7665 666F 726D 2E1F 5468 6520 7761 7665"            /* veform..The wave */
	$"666F 726D 206F 6620 7468 6520 696E 7374"            /* form of the inst */
	$"7275 6D65 6E74 2E16 5377 6974 6368 2045"            /* rument..Switch E */
	$"6E76 656C 6F70 6520 4F4E 2F4F 4646 1B53"            /* nvelope ON/OFF.S */
	$"7769 7463 6820 5375 7374 6169 6E20 506F"            /* witch Sustain Po */
	$"696E 7420 4F4E 2F4F 4646 1B53 7769 7463"            /* int ON/OFF.Switc */
	$"6820 456E 7665 6C6F 7065 204C 6F6F 7020"            /* h Envelope Loop  */
	$"4F4E 2F4F 4646"                                     /* ON/OFF */
};

data 'STR#' (2200, "Classic Help") {
	$"0006 244D 6F72 6520 696E 666F 726D 6174"            /* ..$More informat */
	$"696F 6E20 6162 6F75 7420 7468 6973 2070"            /* ion about this p */
	$"6174 7465 726E 2E32 5377 6974 6368 2074"            /* attern.2Switch t */
	$"6F20 706C 6179 206D 6F64 652E 2043 6C69"            /* o play mode. Cli */
	$"636B 2074 6865 2070 6174 7465 726E 2074"            /* ck the pattern t */
	$"6F20 706C 6179 2069 742E 4C53 7769 7463"            /* o play it.LSwitc */
	$"6820 746F 207A 6F6F 6D20 494E 2F4F 5554"            /* h to zoom IN/OUT */
	$"2E20 546F 207A 6F6F 6D20 494E 3A20 636C"            /* . To zoom IN: cl */
	$"6963 6B2C 2074 6F20 7A6F 6F6D 204F 5554"            /* ick, to zoom OUT */
	$"3A20 636C 6963 6B20 2B20 276F 7074 696F"            /* : click + 'optio */
	$"6E27 206B 6579 2E22 5365 6C65 6374 2074"            /* n' key."Select t */
	$"6865 2074 7261 636B 7320 746F 2062 6520"            /* he tracks to be  */
	$"6469 7370 6C61 7965 642E 2753 656C 6563"            /* displayed.'Selec */
	$"7420 7468 6520 696E 7374 7275 6D65 6E74"            /* t the instrument */
	$"7320 746F 2062 6520 6469 7370 6C61 7965"            /* s to be displaye */
	$"642E 0C54 6865 2070 6174 7465 726E 2E"              /* d..The pattern. */
};

data 'STR#' (2400, "Oscillo Help") {
	$"0003 1A43 6861 6E67 6520 7468 6520 6F73"            /* ...Change the os */
	$"6369 6C6C 6F20 736F 7572 6365 2E1A 4368"            /* cillo source..Ch */
	$"616E 6765 2074 6865 206F 7363 696C 6C6F"            /* ange the oscillo */
	$"2068 6569 6768 742E 1154 6865 206F 7363"            /*  height..The osc */
	$"696C 6C6F 7363 6F70 652E"                           /* illoscope. */
};

data 'STR#' (2600, "Classical Editor Help") {
	$"000C 1C50 6F70 2D75 7020 6D65 6E75 2074"            /* ...Pop-up menu t */
	$"6F20 6368 616E 6765 2074 7261 636B 2E4E"            /* o change track.N */
	$"506F 702D 7570 206D 656E 7520 746F 2073"            /* Pop-up menu to s */
	$"656C 6563 7420 616E 2069 6E73 7472 756D"            /* elect an instrum */
	$"656E 743A 2075 7365 6420 696E 204E 6F74"            /* ent: used in Not */
	$"6520 6D6F 6465 2077 6865 6E20 6372 6561"            /* e mode when crea */
	$"7469 6E67 206E 6577 206E 6F74 652E 4A50"            /* ting new note.JP */
	$"6F70 2D75 7020 6D65 6E75 2074 6F20 7365"            /* op-up menu to se */
	$"6C65 6374 2061 6E20 6566 6665 6374 3A20"            /* lect an effect:  */
	$"7573 6564 2069 6E20 4E6F 7465 206D 6F64"            /* used in Note mod */
	$"6520 7768 656E 2063 7265 6174 696E 6720"            /* e when creating  */
	$"6E65 7720 6E6F 7465 2E2F 4765 7420 6D6F"            /* new note./Get mo */
	$"7265 2069 6E66 6F72 6D61 7469 6F6E 2061"            /* re information a */
	$"626F 7574 2074 6865 2063 7572 7265 6E74"            /* bout the current */
	$"2070 6174 7465 726E 2E34 4F70 656E 2074"            /*  pattern.4Open t */
	$"6865 2070 7265 6665 7265 6E63 6520 7769"            /* he preference wi */
	$"6E64 6F77 2066 6F72 2074 6865 2063 6C61"            /* ndow for the cla */
	$"7373 6963 616C 2065 6469 746F 722E 2C53"            /* ssical editor.,S */
	$"686F 7720 616C 6C20 7472 6163 6B73 2C20"            /* how all tracks,  */
	$"6E6F 7420 6F6E 6C79 2074 6865 2063 7572"            /* not only the cur */
	$"7265 6E74 2074 7261 636B 2E42 5377 6974"            /* rent track.BSwit */
	$"6368 2074 6F20 4E6F 7465 206D 6F64 652E"            /* ch to Note mode. */
	$"2059 6F75 2063 616E 206E 6F77 2064 726F"            /*  You can now dro */
	$"702F 6D6F 7665 206E 6F74 6573 206F 6E20"            /* p/move notes on  */
	$"7468 6520 7061 7274 6974 696F 6E2E 4953"            /* the partition.IS */
	$"7769 7463 6820 746F 2053 656C 6563 7469"            /* witch to Selecti */
	$"6F6E 206D 6F64 652E 2059 6F75 2063 616E"            /* on mode. You can */
	$"206E 6F77 2073 656C 6563 742F 6D6F 7665"            /*  now select/move */
	$"206E 6F74 6573 206F 6E20 7468 6520 7061"            /*  notes on the pa */
	$"7274 6974 696F 6E2E 6253 7769 7463 6820"            /* rtition.bSwitch  */
	$"746F 2050 6C61 7920 6D6F 6465 2E20 596F"            /* to Play mode. Yo */
	$"7520 6361 6E20 6E6F 7720 636C 6963 6B20"            /* u can now click  */
	$"6F6E 2070 6172 7469 7469 6F6E 2061 6E64"            /* on partition and */
	$"2068 6561 7220 6E6F 7465 7320 6D61 6465"            /*  hear notes made */
	$"2062 7920 7468 6520 6375 7272 656E 7420"            /*  by the current  */
	$"696E 7374 7275 6D65 6E74 2E35 5377 6974"            /* instrument.5Swit */
	$"6368 2074 6F20 4465 6C65 7465 206D 6F64"            /* ch to Delete mod */
	$"653A 2063 6C69 636B 206F 6E20 6E6F 7465"            /* e: click on note */
	$"7320 746F 2064 656C 6574 6520 7468 656D"            /* s to delete them */
	$"2E36 4120 736D 616C 6C20 7669 6577 206F"            /* .6A small view o */
	$"6620 7468 6520 7061 7274 6974 696F 6E2E"            /* f the partition. */
	$"2043 6C69 636B 206F 6E20 6974 2074 6F20"            /*  Click on it to  */
	$"706C 6179 2069 742E 3E53 7769 7463 6820"            /* play it.>Switch  */
	$"746F 205A 6F6F 6D20 6D6F 6465 2E20 596F"            /* to Zoom mode. Yo */
	$"7520 6361 6E20 6E6F 7720 7A6F 6F6D 2069"            /* u can now zoom i */
	$"6E2F 6F75 7420 696E 2074 6865 2070 6172"            /* n/out in the par */
	$"7469 7469 6F6E 2E"                                  /* tition. */
};

data 'STR#' (4000, "Spectrum Help") {
	$"0004 1B43 6861 6E67 6520 7468 6520 7370"            /* ...Change the sp */
	$"6563 7472 756D 2073 6F75 7263 652E 1B43"            /* ectrum source..C */
	$"6861 6E67 6520 7468 6520 7370 6563 7472"            /* hange the spectr */
	$"756D 2068 6569 6768 742E 3754 6865 2073"            /* um height.7The s */
	$"7065 6374 7275 6D2E 2043 6C69 636B 206F"            /* pectrum. Click o */
	$"6E20 6974 2074 6F20 6669 6E64 2061 2073"            /* n it to find a s */
	$"7065 6369 6669 6320 6672 6571 7565 6E63"            /* pecific frequenc */
	$"792E 3243 6861 6E67 6520 7468 6520 7370"            /* y.2Change the sp */
	$"6563 7472 756D 2073 6361 6C65 203A 206C"            /* ectrum scale : l */
	$"696E 6561 7220 6F72 206C 6F67 6172 6974"            /* inear or logarit */
	$"686D 6963 2E"                                       /* hmic. */
};

data 'STR#' (5400, "Classical Editor Help") {
	$"000A 2553 7769 7463 6820 746F 206E 6F74"            /* .¬%Switch to not */
	$"6520 6564 6974 696E 6720 6D6F 6465 2028"            /* e editing mode ( */
	$"5461 6220 6B65 7929 1F53 7769 7463 6820"            /* Tab key).Switch  */
	$"746F 2073 656C 6563 7420 6D6F 6465 2028"            /* to select mode ( */
	$"5461 6220 6B65 7929 324C 6F61 6420 6120"            /* Tab key)2Load a  */
	$"5063 6D64 2066 696C 6520 616E 6420 7061"            /* Pcmd file and pa */
	$"7374 6520 6974 2061 7420 6375 7272 656E"            /* ste it at curren */
	$"7420 706F 7369 7469 6F6E 2E29 506C 6179"            /* t position.)Play */
	$"2063 7572 7265 6E74 2073 656C 6563 7469"            /*  current selecti */
	$"6F6E 2028 5573 6520 7365 6C65 6374 206D"            /* on (Use select m */
	$"6F64 6529 2E26 5361 7665 2063 7572 7265"            /* ode).&Save curre */
	$"6E74 2073 656C 6563 7469 6F6E 2069 6E20"            /* nt selection in  */
	$"6120 5063 6D64 2066 696C 652E 2647 6574"            /* a Pcmd file.&Get */
	$"2069 6E66 6F72 6D61 7469 6F6E 2061 626F"            /*  information abo */
	$"7574 2063 7572 7265 6E74 2070 6174 7465"            /* ut current patte */
	$"726E 2E24 4170 706C 7920 6120 6669 6C74"            /* rn.$Apply a filt */
	$"6572 2074 6F20 6375 7272 656E 7420 7365"            /* er to current se */
	$"6C65 6374 696F 6E2E 184F 7065 6E20 5072"            /* lection..Open Pr */
	$"6566 6572 656E 6365 7320 7769 6E64 6F77"            /* eferences window */
	$"2E2F 5365 6C65 6374 206E 6F74 6520 6C65"            /* ./Select note le */
	$"6E67 7468 2074 6F20 7573 6520 696E 206E"            /* ngth to use in n */
	$"6F74 6520 6564 6974 696E 6720 6D6F 6465"            /* ote editing mode */
	$"2E2E 5365 6C65 6374 2069 6E73 7472 756D"            /* ..Select instrum */
	$"656E 7420 746F 2075 7365 2069 6E20 6E6F"            /* ent to use in no */
	$"7465 2065 6469 7469 6E67 206D 6F64 652E"            /* te editing mode. */
};

data 'STR#' (2000, "Piano Help") {
	$"0003 4D54 6865 2050 6961 6E6F 3A20 636C"            /* ..MThe Piano: cl */
	$"6963 6B20 6F6E 2069 7420 746F 2068 6561"            /* ick on it to hea */
	$"7220 7468 6520 7365 6C65 6374 6564 2069"            /* r the selected i */
	$"6E73 7472 756D 656E 7420 2873 6565 2069"            /* nstrument (see i */
	$"6E73 7472 756D 656E 7420 6C69 7374 292E"            /* nstrument list). */
	$"1553 6869 6674 204B 6579 2D4D 6170 2074"            /* .Shift Key-Map t */
	$"6F20 6C65 6674 234F 7065 6E20 7468 6520"            /* o left#Open the  */
	$"5069 616E 6F27 7320 5072 6566 6572 656E"            /* Piano's Preferen */
	$"6365 2077 696E 646F 772E"                           /* ce window. */
};

data 'STR#' (4200, "Partition Help") {
	$"0007 1A4F 7065 6E20 7468 6520 7365 6C65"            /* ...Open the sele */
	$"6374 6564 2070 6174 7465 726E 2E31 496E"            /* cted pattern.1In */
	$"6469 6361 7465 2070 6172 7469 7469 6F6E"            /* dicate partition */
	$"206C 656E 6774 682E 2043 6C69 636B 206F"            /*  length. Click o */
	$"6E20 6974 2074 6F20 6368 616E 6765 2E39"            /* n it to change.9 */
	$"506F 7055 702D 6D65 6E75 2074 6F20 6368"            /* PopUp-menu to ch */
	$"616E 6765 2074 6865 2070 6174 7465 726E"            /* ange the pattern */
	$"206E 756D 6265 7220 6174 2074 6869 7320"            /*  number at this  */
	$"706F 7369 7469 6F6E 2E38 5468 6520 7061"            /* position.8The pa */
	$"7274 6974 696F 6E20 7769 7468 2070 6174"            /* rtition with pat */
	$"7465 726E 206E 756D 6265 7220 2873 6565"            /* tern number (see */
	$"2027 5061 7474 6572 6E73 206C 6973 7427"            /*  'Patterns list' */
	$"292E 3047 6574 206D 6F72 6520 696E 666F"            /* ).0Get more info */
	$"726D 6174 696F 6E20 6162 6F75 7420 7468"            /* rmation about th */
	$"6520 7365 6C65 6374 6564 2070 6174 7465"            /* e selected patte */
	$"726E 2E1A 4164 6420 6120 726F 7720 696E"            /* rn..Add a row in */
	$"2074 6865 2070 6172 7469 7469 6F6E 2052"            /*  the partition R */
	$"656D 6F76 6520 7468 6973 2072 6F77 2069"            /* emove this row i */
	$"6E20 7468 6520 7061 7274 6974 696F 6E"              /* n the partition */
};

data 'STR#' (400, "Digital Editor Help") {
	$"0015 2C4F 7065 6E20 7468 6520 4469 6769"            /* ..,Open the Digi */
	$"7461 6C20 4564 6974 6F72 2773 2050 7265"            /* tal Editor's Pre */
	$"6665 7265 6E63 6520 7769 6E64 6F77 2E33"            /* ference window.3 */
	$"4C6F 6164 2061 2050 636D 6420 6669 6C65"            /* Load a Pcmd file */
	$"2061 6E64 2070 6173 7465 2069 7420 6173"            /*  and paste it as */
	$"2063 7572 7265 6E74 2073 656C 6563 7469"            /*  current selecti */
	$"6F6E 2E28 5361 7665 2063 7572 7265 6E74"            /* on.(Save current */
	$"2073 656C 6563 7469 6F6E 2069 6E74 6F20"            /*  selection into  */
	$"6120 5063 6D64 2066 696C 652E 6254 7261"            /* a Pcmd file.bTra */
	$"6E73 706F 7365 2075 7020 6E6F 7465 7320"            /* nspose up notes  */
	$"6F66 2063 7572 7265 6E74 2073 656C 6563"            /* of current selec */
	$"7469 6F6E 2062 7920 312E 2057 6974 6820"            /* tion by 1. With  */
	$"7368 6966 7420 6B65 793A 206F 6E6C 7920"            /* shift key: only  */
	$"6375 7272 656E 7420 696E 7374 7275 6D65"            /* current instrume */
	$"6E74 2E20 283D 204B 6579 2027 2A27 292F"            /* nt. (= Key '*')/ */
	$"4765 7420 6D6F 7265 2069 6E66 6F72 6D61"            /* Get more informa */
	$"7469 6F6E 2061 626F 7574 2074 6865 2063"            /* tion about the c */
	$"7572 7265 6E74 2070 6174 7465 726E 2E1B"            /* urrent pattern.. */
	$"506C 6179 2074 6865 2063 7572 7265 6E74"            /* Play the current */
	$"2073 656C 6563 7469 6F6E 2E45 5377 6974"            /*  selection.ESwit */
	$"6368 2069 6E20 7265 636F 7264 206D 6F64"            /* ch in record mod */
	$"653A 2070 7265 7373 206F 6E20 796F 7572"            /* e: press on your */
	$"204D 6163 206B 6579 626F 6172 6420 746F"            /*  Mac keyboard to */
	$"2065 6E74 6572 206E 6577 206E 6F74 6573"            /*  enter new notes */
	$"2E32 4F70 656E 2074 6865 2063 6F6D 6D61"            /* .2Open the comma */
	$"6E64 2065 6469 746F 7220 746F 2065 6469"            /* nd editor to edi */
	$"7420 7468 6520 7365 6C65 6374 6564 2063"            /* t the selected c */
	$"656C 6C2E 1354 7261 636B 2049 4420 616E"            /* ell..Track ID an */
	$"6420 636F 6C6F 722E 0C50 6F73 6974 696F"            /* d color..Positio */
	$"6E20 4944 2E5B 5468 6520 6375 7272 656E"            /* n ID.[The curren */
	$"7420 7061 7474 6572 6E3A 2063 6C69 636B"            /* t pattern: click */
	$"2074 6F20 7365 6C65 6374 2061 206E 6F74"            /*  to select a not */
	$"652C 2075 7365 2063 6C69 636B 202B 2027"            /* e, use click + ' */
	$"7368 6966 7427 206B 6579 2074 6F20 6578"            /* shift' key to ex */
	$"7061 6E64 2061 2073 656C 6563 7469 6F6E"            /* pand a selection */
	$"2E2B 4170 706C 7920 616E 2063 6F6D 6D61"            /* .+Apply an comma */
	$"6E64 2070 6C75 6720 746F 2063 7572 7265"            /* nd plug to curre */
	$"6E74 2073 656C 6563 7469 6F6E 2E1D 5377"            /* nt selection..Sw */
	$"6974 6368 2050 6174 7465 726E 204C 6F6F"            /* itch Pattern Loo */
	$"7069 6E67 204F 4E2F 4F46 4610 4F70 656E"            /* ping ON/OFF.Open */
	$"2046 696E 6420 5769 6E64 6F77 6354 7261"            /*  Find WindowcTra */
	$"6E73 706F 7365 2064 6F77 6E20 6E6F 7465"            /* nspose down note */
	$"7320 6F66 2063 7572 7265 6E74 2073 656C"            /* s of current sel */
	$"6563 7469 6F6E 2062 7920 312E 2057 6974"            /* ection by 1. Wit */
	$"6820 7368 6966 7420 6B65 793A 206F 6E6C"            /* h shift key: onl */
	$"7920 6375 7272 656E 7420 696E 7374 7275"            /* y current instru */
	$"6D65 6E74 2E28 3D20 4B65 7920 272F 2729"            /* ment.(= Key '/') */
	$"2646 696C 6C20 7365 6C65 6374 6564 2061"            /* &Fill selected a */
	$"7265 6120 7769 7468 2064 6566 6175 6C74"            /* rea with default */
	$"2076 616C 7565 7311 5365 7420 6465 6661"            /*  values.Set defa */
	$"756C 7420 5661 6C75 6511 5365 7420 6465"            /* ult Value.Set de */
	$"6661 756C 7420 5661 6C75 6511 5365 7420"            /* fault Value.Set  */
	$"6465 6661 756C 7420 5661 6C75 6511 5365"            /* default Value.Se */
	$"7420 6465 6661 756C 7420 5661 6C75 6527"            /* t default Value' */
	$"5365 7420 7374 6570 2077 6865 6E20 7072"            /* Set step when pr */
	$"6573 7369 6E67 2061 7272 6F77 7320 6F72"            /* essing arrows or */
	$"2072 6574 7572 6E"                                  /*  return */
};

data 'STR#' (133, "Internet Menu") {
	$"0003 2468 7474 703A 2F2F 7777 772E 7175"            /* ..$http://www.qu */
	$"6164 6D61 7469 6F6E 2E63 6F6D 2F70 7068"            /* admation.com/pph */
	$"6F6D 652E 6874 6D3F 6874 7470 3A2F 2F68"            /* ome.htm?http://h */
	$"7970 6572 6172 6368 6976 652E 6C63 732E"            /* yperarchive.lcs. */
	$"6D69 742E 6564 752F 6367 692D 6269 6E2F"            /* mit.edu/cgi-bin/ */
	$"4E65 7753 6561 7263 683F 6B65 793D 506C"            /* NewSearch?key=Pl */
	$"6179 6572 5052 4F1F 6D61 696C 746F 3A72"            /* ayerPRO.mailto:r */
	$"6F73 7365 7461 6E74 6F69 6E65 4062 6C75"            /* ossetantoine@blu */
	$"6577 696E 2E63 68"                                  /* ewin.ch */
};

data 'STR#' (130, "Effects") {
	$"0011 1130 2D4E 6F72 6D61 6C2F 4172 7065"            /* ...0-Normal/Arpe */
	$"6767 696F 0A31 2D53 6C69 6465 2055 700C"            /* ggio¬1-Slide Up. */
	$"322D 536C 6964 6520 446F 776E 0C33 2D50"            /* 2-Slide Down.3-P */
	$"6F72 7461 6D65 6E74 6F0A 342D 0056 6962"            /* ortamento¬4-.Vib */
	$"7261 746F 1035 2D50 6F72 7461 6D65 6E74"            /* rato.5-Portament */
	$"6F2B 566F 6C13 362D 5669 6272 6174 6F2B"            /* o+Vol.6-Vibrato+ */
	$"566F 6C20 536C 6964 6509 372D 5472 656D"            /* Vol Slide∆7-Trem */
	$"6F6C 6F0D 382D 5365 7420 5061 6E6E 696E"            /* olo.8-Set Pannin */
	$"6712 392D 5365 7420 5361 6D70 6C65 4F66"            /* g.9-Set SampleOf */
	$"6673 6574 0D41 2D56 6F6C 756D 6553 6C69"            /* fset.A-VolumeSli */
	$"6465 0F42 2D50 6F73 6974 696F 6E20 4A75"            /* de.B-Position Ju */
	$"6D70 0C43 2D53 6574 2056 6F6C 756D 650F"            /* mp.C-Set Volume. */
	$"442D 5061 7474 6572 6E20 4272 6561 6B0C"            /* D-Pattern Break. */
	$"452D 4520 436F 6D6D 616E 6473 0B46 2D53"            /* E-E Commands.F-S */
	$"6574 2053 7065 6564 0A47 2D4E 6F74 6520"            /* et Speed¬G-Note  */
	$"4F66 66"                                            /* Off */
};

data 'STR#' (131) {
	$"0046 0B50 6C61 7965 7250 524F 2035 1968"            /* .F.PlayerPRO 5.h */
	$"7474 703A 2F2F 7777 772E 7175 6164 6D61"            /* ttp://www.quadma */
	$"7469 6F6E 2E63 6F6D 0E53 7065 6369 616C"            /* tion.com.Special */
	$"2074 6861 6E6B 7301 260B 5361 6C75 7461"            /*  thanks.&.Saluta */
	$"7469 6F6E 7302 746F 1C43 6861 6E74 616C"            /* tions.to.Chantal */
	$"204D 756C 6861 7570 7420 2843 6861 6E2D"            /*  Mulhaupt (Chan- */
	$"4368 616E 290F 4461 7269 6F20 4163 636F"            /* Chan).Dario Acco */
	$"726E 6572 6F0F 5361 6D75 656C 204C 696E"            /* rnero.Samuel Lin */
	$"6465 6D61 6E0C 4D61 726B 2043 6F6F 6B73"            /* deman.Mark Cooks */
	$"6F6E 1A4A 6F91 6C20 4261 7272 698F 7265"            /* on.Joël Barrière */
	$"2028 4672 656E 6368 204D 616E 291D 5065"            /*  (French Man).Pe */
	$"7474 6572 6920 4C61 6A75 6E65 6E20 2850"            /* tteri Lajunen (P */
	$"6573 742F 5472 616E 5A65 291B 5374 6576"            /* est/TranZe).Stev */
	$"6520 4A6F 6273 2028 4D61 6320 5370 6563"            /* e Jobs (Mac Spec */
	$"6961 6C69 7374 290B 4A6F 686E 2053 7469"            /* ialist).John Sti */
	$"6C65 7310 416C 6C20 4A61 7061 6E20 7573"            /* les.All Japan us */
	$"6572 7321 0E46 8E6C 6978 204C 6170 6F69"            /* ers!.Félix Lapoi */
	$"6E74 650E 4875 6265 7274 2047 6172 7269"            /* nte.Hubert Garri */
	$"646F 0D4C 6176 616C 6C65 6520 4D61 7263"            /* do.Lavallee Marc */
	$"0B44 6961 7A20 4D69 6775 656C 1143 6872"            /* .Diaz Miguel.Chr */
	$"6973 7469 616E 204C 6163 726F 6978 0E4C"            /* istian Lacroix.L */
	$"6175 7265 6E74 2043 7269 6164 6F07 5374"            /* aurent Criado.St */
	$"7978 2E2E 2E06 546F 7061 7A65 0B44 6F63"            /* yx....Topaze.Doc */
	$"746F 7220 5965 6C6D 0E4D 6173 7465 7220"            /* tor Yelm.Master  */
	$"4279 616B 6865 6509 446F 6374 6F72 2050"            /* Byakhee∆Doctor P */
	$"4D14 4D79 206E 6577 2048 6F6E 6461 2056"            /* M.My new Honda V */
	$"5452 3130 3030 0B4C 6520 4368 6E69 6E6B"            /* TR1000.Le Chnink */
	$"656C 114F 736D 616E 2052 6174 6962 2026"            /* el.Osman Ratib & */
	$"2055 494E 0B59 7665 7320 4C69 6769 6572"            /*  UIN.Yves Ligier */
	$"1043 6872 6973 7469 616E 2047 6972 6172"            /* .Christian Girar */
	$"6404 4572 6963 0947 8E72 616C 6469 6E65"            /* d.Eric∆Géraldine */
	$"0743 6F72 696E 6E65 074C 6175 7265 6E74"            /* .Corinne.Laurent */
	$"0E43 8E64 7269 6320 4C6F 6D62 6172 640C"            /* .Cédric Lombard. */
	$"4572 6963 204C 6170 6F72 7465 1944 6F72"            /* Eric Laporte.Dor */
	$"6F74 688E 6520 7769 7468 2047 7565 756C"            /* othée with Gueul */
	$"6544 6552 616B 0F42 756E 6769 6520 536F"            /* eDeRak.Bungie So */
	$"6674 7761 7265 1244 616E 6965 6C20 4C65"            /* ftware.Daniel Le */
	$"7363 686E 7A69 6E65 7207 4761 682D 4761"            /* schnziner.Gah-Ga */
	$"680D 4361 726C 6F73 2053 6572 616E 6F0D"            /* h.Carlos Serano. */
	$"5472 6D20 4949 2F34 202D 2D20 350E 4672"            /* Trm II/4 -- 5.Fr */
	$"616E 8D6F 6973 2052 6F73 7369 0E4C 7563"            /* ançois Rossi.Luc */
	$"2044 6573 6368 656E 6175 780E 4170 706C"            /*  Deschenaux.Appl */
	$"6520 436F 6D70 7574 6572 0E47 696C 6C65"            /* e Computer.Gille */
	$"7320 4275 726E 6965 7207 4361 6E61 6C20"            /* s Burnier.Canal  */
	$"2B10 4A61 6371 7565 6C69 6E65 204D 6579"            /* +.Jacqueline Mey */
	$"6572 0A48 4355 4720 2620 434D 551C 416C"            /* er¬HCUG & CMU.Al */
	$"6C20 5278 206C 6F76 6572 7320 6F66 2074"            /* l Rx lovers of t */
	$"6869 7320 706C 616E 6574 104D 6172 6375"            /* his planet.Marcu */
	$"7320 5363 686C 6170 6665 7212 5374 6570"            /* s Schlapfer.Step */
	$"6865 6E20 416C 7472 6963 6874 6572 1B41"            /* hen Altrichter.A */
	$"6C6C 2074 6865 2074 686F 6E73 206F 6620"            /* ll the thons of  */
	$"7468 6520 766F 6C6C 8E65 1A53 7964 6E65"            /* the vollée.Sydne */
	$"7920 4368 696C 6472 656E D573 2048 6F73"            /* y Children’s Hos */
	$"7069 7461 6C14 4899 7069 7461 6C20 436F"            /* pital.Hôpital Co */
	$"6368 696E 2050 6172 6973 0C4C 6573 204C"            /* chin Paris.Les L */
	$"6575 6368 7465 720A 4C65 7320 4265 6E61"            /* euchter¬Les Bena */
	$"6E69 1452 6573 7461 7572 616E 7420 486F"            /* ni.Restaurant Ho */
	$"6E67 2D4B 6F6E 670D 426F 7373 6579 2046"            /* ng-Kong.Bossey F */
	$"7261 6E63 6508 4D6F 746F 726F 6C61 1A43"            /* rance.Motorola.C */
	$"6174 204D 6174 6869 6C64 6520 284E 6F77"            /* at Mathilde (Now */
	$"2064 6561 642E 2E2E 291D 4361 7420 446F"            /*  dead...).Cat Do */
	$"7578 2D44 6F75 7820 2853 6D6F 6F74 682D"            /* ux-Doux (Smooth- */
	$"536D 6F6F 7468 2918 4D79 204D 6163 2063"            /* Smooth).My Mac c */
	$"6F6D 7075 7465 7220 2847 332F 3330 3029"            /* omputer (G3/300) */
	$"0744 6179 7374 6172 104E 616E 7475 636B"            /* .Daystar.Nantuck */
	$"6574 2049 736C 616E 6414 4368 6172 6C65"            /* et Island.Charle */
	$"7320 486F 7465 6C20 426F 7374 6F6E 0B43"            /* s Hotel Boston.C */
	$"6872 6F6E 6F6C 6F67 6965 0C45 6C6C 6973"            /* hronologie.Ellis */
	$"2046 5241 4E43 4512 282B 3333 2920 3134"            /*  FRANCE.(+33) 14 */
	$"3320 3333 2031 3520 3437"                           /* 3 33 15 47 */
};

data 'MENU' (128) {
	$"0080 0000 0000 0000 0000 FFEF B6FF 0446"            /* .Ä........ˇÔ∂ˇ.F */
	$"696C 6506 4E65 772E 2E2E 004E 0000 074F"            /* ile.New....N...O */
	$"7065 6E2E 2E2E 004F 0000 0453 6176 6500"            /* pen....O...Save. */
	$"5300 000A 5361 7665 2061 732E 2E2E 0000"            /* S..¬Save as..... */
	$"0000 0543 6C6F 7365 0057 0000 0C45 7870"            /* ...Close.W...Exp */
	$"6F72 7420 6173 2E2E 2E00 1B8B 0005 5265"            /* ort as.....ã..Re */
	$"7365 7400 5200 0001 2D00 0000 0012 5361"            /* set.R...-.....Sa */
	$"7665 204D 7573 6963 204C 6973 742E 2E2E"            /* ve Music List... */
	$"0000 0000 1043 6C65 6172 204D 7573 6963"            /* .....Clear Music */
	$"204C 6973 7400 0000 0001 2D00 0000 000D"            /*  List.....-..... */
	$"5061 6765 2053 6574 5570 2E2E 2E00 0000"            /* Page SetUp...... */
	$"0008 5072 696E 742E 2E2E 0000 0000 012D"            /* ..Print........- */
	$"0000 0000 0E50 7265 6665 7265 6E63 6573"            /* .....Preferences */
	$"2E2E 2E00 1BC8 000F 5769 6E64 6F77 7320"            /* .....»..Windows  */
	$"5365 7473 2E2E 2E00 1BB0 0016 536F 756E"            /* Sets.....∞..Soun */
	$"6420 436F 6E74 726F 6C20 5061 6E65 6C2E"            /* d Control Panel. */
	$"2E2E 0000 0000 134F 4D53 2053 7475 6469"            /* .......OMS Studi */
	$"6F20 5365 7475 702E 2E2E 0000 0000 114F"            /* o Setup........O */
	$"4D53 204D 4944 4920 5365 7475 702E 2E2E"            /* MS MIDI Setup... */
	$"0000 0000 012D 0000 0000 0451 7569 7400"            /* .....-.....Quit. */
	$"5100 0000"                                          /* Q... */
};

data 'MENU' (129) {
	$"0081 0000 0000 0000 0000 FFFF FFFF 0751"            /* .Å........ˇˇˇˇ.Q */
	$"7561 6C69 7479 0535 204B 687A 0000 0000"            /* uality.5 Khz.... */
	$"0631 3120 4B68 7A00 0000 0006 3136 204B"            /* .11 Khz.....16 K */
	$"687A 0000 0000 0632 3220 4B68 7A00 0000"            /* hz.....22 Khz... */
	$"0006 3434 204B 687A 0000 0000 0634 3820"            /* ..44 Khz.....48  */
	$"4B68 7A00 0000 0000"                                /* Khz..... */
};

data 'MENU' (137) {
	$"0089 0000 0000 0000 0000 FFFF FFFF 0554"            /* .â........ˇˇˇˇ.T */
	$"6974 6C65 00"                                       /* itle. */
};

data 'MENU' (134) {
	$"0086 0000 0000 0000 0000 FFFF FFFF 0649"            /* .Ü........ˇˇˇˇ.I */
	$"6E73 7472 7500"                                     /* nstru. */
};

data 'MENU' (138, "Import Special Format...") {
	$"008A 0000 0000 0000 0000 FFFF FFFF 0649"            /* .ä........ˇˇˇˇ.I */
	$"6D70 6F72 7400"                                     /* mport. */
};

data 'MENU' (139) {
	$"008B 0000 0000 0000 0000 FFFF FFF3 0645"            /* .ã........ˇˇˇÛ.E */
	$"7870 6F72 740A 4149 4646 2053 6F75 6E64"            /* xport¬AIFF Sound */
	$"0000 0000 0B4D 6163 4F53 204D 7573 6963"            /* .....MacOS Music */
	$"0000 0000 012D 0000 0000 00"                        /* .....-..... */
};

data 'MENU' (143) {
	$"008F 0000 0000 0000 0000 FFFF FFFF 0751"            /* .è........ˇˇˇˇ.Q */
	$"7561 6C69 7479 0A46 2335 202D 2047 6F6F"            /* uality¬F#5 - Goo */
	$"6400 0000 000C 4623 3420 2D20 4265 7474"            /* d.....F#4 - Bett */
	$"6572 0000 0000 0A46 2333 202D 2042 6573"            /* er....¬F#3 - Bes */
	$"7400 0000 0000"                                     /* t..... */
};

data 'MENU' (144) {
	$"0090 0000 0000 0000 0000 FFFF FFFF 0554"            /* .ê........ˇˇˇˇ.T */
	$"6974 6C65 034F 6666 0000 0000 024F 6E00"            /* itle.Off.....On. */
	$"0000 0000"                                          /* .... */
};

data 'MENU' (145) {
	$"0091 0000 0000 0000 0000 FFFF FFF7 0850"            /* .ë........ˇˇˇ˜.P */
	$"6174 7465 726E 730E 5061 7274 6974 696F"            /* atterns.Partitio */
	$"6E20 4C69 7374 0000 0000 0D50 6174 7465"            /* n List.....Patte */
	$"726E 7320 4C69 7374 0000 0000 012D 0000"            /* rns List.....-.. */
	$"0000 1243 7265 6174 6520 6E65 7720 7061"            /* ...Create new pa */
	$"7474 6572 6E00 0000 0007 4F70 656E 2E2E"            /* ttern.....Open.. */
	$"2E00 0000 000A 5361 7665 2041 732E 2E2E"            /* .....¬Save As... */
	$"0000 0000 0550 7572 6765 0000 0000 0644"            /* .....Purge.....D */
	$"656C 6574 6500 0000 0000"                           /* elete..... */
};

data 'MENU' (147) {
	$"0093 0000 0000 0000 0000 FFFF FFFF 0B43"            /* .ì........ˇˇˇˇ.C */
	$"6F6D 7072 6573 7369 6F6E 044E 4F4E 4500"            /* ompression.NONE. */
	$"0000 0004 4D41 4431 0000 0000 00"                   /* ....MAD1..... */
};

data 'MENU' (148) {
	$"0094 0000 0000 0000 0000 FFFF FFF7 0941"            /* .î........ˇˇˇ˜∆A */
	$"6D70 6C69 7475 6465 0638 2062 6974 7300"            /* mplitude.8 bits. */
	$"0000 0007 3136 2062 6974 7300 0000 0007"            /* ....16 bits..... */
	$"3234 2062 6974 7300 0000 0000"                      /* 24 bits..... */
};

data 'MENU' (146) {
	$"0092 0000 0000 0000 0000 FFFF FFFF 0554"            /* .í........ˇˇˇˇ.T */
	$"6974 6C65 00"                                       /* itle. */
};

data 'MENU' (149) {
	$"0095 0000 0000 0000 0000 FFFF FFFF 0C53"            /* .ï........ˇˇˇˇ.S */
	$"7065 6369 616C 204B 6579 7303 4573 6300"            /* pecial Keys.Esc. */
	$"0000 0003 5461 6200 0000 0002 4631 0000"            /* ....Tab.....F1.. */
	$"0000 0246 3200 0000 0002 4633 0000 0000"            /* ...F2.....F3.... */
	$"0246 3400 0000 0002 4635 0000 0000 0246"            /* .F4.....F5.....F */
	$"3600 0000 0002 4637 0000 0000 0246 3800"            /* 6.....F7.....F8. */
	$"0000 0002 4639 0000 0000 0346 3130 0000"            /* ....F9.....F10.. */
	$"0000 0346 3132 0000 0000 0346 3131 0000"            /* ...F12.....F11.. */
	$"0000 0346 3133 0000 0000 0346 3134 0000"            /* ...F13.....F14.. */
	$"0000 0346 3135 0000 0000 00"                        /* ...F15..... */
};

data 'MENU' (151) {
	$"0097 0000 0000 0000 0000 FFFF FFFF 0130"            /* .ó........ˇˇˇˇ.0 */
	$"0231 3600 0000 0002 3332 0000 0000 0236"            /* .16.....32.....6 */
	$"3400 0000 0003 3132 3800 0000 0003 3235"            /* 4.....128.....25 */
	$"3600 0000 0000"                                     /* 6..... */
};

data 'MENU' (152) {
	$"0098 0000 0000 0000 0000 FFFF FFFF 134D"            /* .ò........ˇˇˇˇ.M */
	$"4944 4920 474D 2049 6E73 7472 756D 656E"            /* IDI GM Instrumen */
	$"7473 0550 6961 6E6F 0000 0000 1443 6872"            /* ts.Piano.....Chr */
	$"6F6D 6174 6963 2050 6572 6375 7373 696F"            /* omatic Percussio */
	$"6E00 0000 0005 4F72 6761 6E00 0000 0006"            /* n.....Organ..... */
	$"4775 6974 6172 0000 0000 0442 6173 7300"            /* Guitar.....Bass. */
	$"0000 0007 5374 7269 6E67 7300 0000 0008"            /* ....Strings..... */
	$"456E 7365 6D62 6C65 0000 0000 0542 7261"            /* Ensemble.....Bra */
	$"7373 0000 0000 0452 6565 6400 0000 0004"            /* ss.....Reed..... */
	$"5069 7065 0000 0000 0A53 796E 7468 204C"            /* Pipe....¬Synth L */
	$"6561 6400 0000 0009 5379 6E74 6820 5061"            /* ead....∆Synth Pa */
	$"6400 0000 000D 5379 6E74 6820 4566 6665"            /* d.....Synth Effe */
	$"6374 7300 0000 0006 4574 686E 6963 0000"            /* cts.....Ethnic.. */
	$"0000 0A50 6572 6375 7373 6976 6500 0000"            /* ..¬Percussive... */
	$"000D 536F 756E 6420 4566 6665 6374 7300"            /* ..Sound Effects. */
	$"0000 0005 4472 756D 7300 0000 0000"                 /* ....Drums..... */
};

data 'MENU' (153) {
	$"0096 0000 0000 0000 0000 FFFF FFFF 0100"            /* .ñ........ˇˇˇˇ.. */
	$"0C41 7564 696F 204F 7574 5075 7400 0000"            /* .Audio OutPut... */
	$"000B 4175 6469 6F20 496E 5075 7400 0000"            /* ..Audio InPut... */
	$"0000"                                               /* .. */
};

data 'MENU' (155) {
	$"009B 0000 0000 0000 0000 FFFF FFFF 0743"            /* .õ........ˇˇˇˇ.C */
	$"7265 6174 6F72 1053 4E50 4C20 2D20 506C"            /* reator.SNPL - Pl */
	$"6179 6572 5052 4F00 0000 0010 4D4D 4B52"            /* ayerPRO.....MMKR */
	$"202D 204D 6163 4D6F 6450 726F 0000 0000"            /*  - MacModPro.... */
	$"1453 5472 6B20 2D20 536F 756E 642D 5472"            /* .STrk - Sound-Tr */
	$"6563 6B65 7200 0000 000F 5343 504C 202D"            /* ecker.....SCPL - */
	$"2053 6F75 6E64 4170 7000 0000 0000"                 /*  SoundApp..... */
};

data 'MENU' (141) {
	$"008D 0000 0000 07B4 0000 FFFF FFFF 0554"            /* .ç.....¥..ˇˇˇˇ.T */
	$"6974 6C65 0100 0000 0000 00"                        /* itle....... */
};

data 'MENU' (157) {
	$"009D 0000 0000 0000 0000 FFFF FFFF 044D"            /* .ù........ˇˇˇˇ.M */
	$"4F44 7309 506C 6179 6572 5052 4F00 0000"            /* ODs∆PlayerPRO... */
	$"0008 536F 756E 6441 7070 0000 0000 0C53"            /* ..SoundApp.....S */
	$"6F75 6E64 5472 6563 6B65 7200 0000 000A"            /* oundTrecker....¬ */
	$"4D61 634D 6F64 2050 726F 0000 0000 064D"            /* MacMod Pro.....M */
	$"4465 6C74 6100 0000 0000"                           /* Delta..... */
};

data 'MENU' (158) {
	$"009E 0000 0000 0000 0000 FFFF FFFF 0A54"            /* .û........ˇˇˇˇ¬T */
	$"7970 6520 4669 6C65 7304 4D41 4449 0000"            /* ype Files.MADI.. */
	$"0000 044D 4144 4800 0000 0004 4D41 4446"            /* ...MADH.....MADF */
	$"0000 0000 0A53 5472 6B20 2D20 4D4F 4400"            /* ....¬STrk - MOD. */
	$"0000 0004 4D54 4D20 0000 0000 0453 334D"            /* ....MTM .....S3M */
	$"2000 0000 0004 4F4B 5441 0000 0000 0436"            /*  .....OKTA.....6 */
	$"3636 3900 0000 0004 4D69 6469 0000 0000"            /* 669.....Midi.... */
	$"0458 4D20 2000 0000 0004 4954 2020 0000"            /* .XM  .....IT  .. */
	$"0000 00"                                            /* ... */
};

data 'MENU' (156) {
	$"009C 0000 0000 0000 0000 FFFF FFF7 0753"            /* .ú........ˇˇˇ˜.S */
	$"616D 706C 6573 0F56 6F6C 756D 6520 456E"            /* amples.Volume En */
	$"7665 6C6F 7065 0000 0000 1050 616E 6E69"            /* velope.....Panni */
	$"6E67 2045 6E76 656C 6F70 6500 0000 0001"            /* ng Envelope..... */
	$"2D00 0000 0000"                                     /* -..... */
};

data 'MENU' (159) {
	$"009F 0000 0000 0000 0000 FFFF FFFF 0450"            /* .ü........ˇˇˇˇ.P */
	$"5044 4700"                                          /* PDG. */
};

data 'MENU' (160) {
	$"00A0 0000 0000 0000 0000 FFFF FFFF 0573"            /* .†........ˇˇˇˇ.s */
	$"6361 6C65 066C 696E 6561 7200 0000 000B"            /* cale.linear..... */
	$"6C6F 6761 7269 7468 6D69 6300 0000 0000"            /* logarithmic..... */
};

data 'MENU' (163) {
	$"00A3 0000 0000 0000 0000 FFFF FFBF 0100"            /* .£........ˇˇˇø.. */
	$"0831 3130 3235 2048 7A00 0000 0008 3131"            /* .11025 Hz.....11 */
	$"3132 3720 487A 0000 0000 0832 3230 3530"            /* 127 Hz.....22050 */
	$"2068 7A00 0000 0008 3232 3235 3420 487A"            /*  hz.....22254 Hz */
	$"0000 0000 0834 3431 3030 2048 7A00 0000"            /* .....44100 Hz... */
	$"0001 2D00 0000 0012 3833 3633 2048 7A20"            /* ..-.....8363 Hz  */
	$"2841 6D69 6761 202B 3029 0000 0000 1238"            /* (Amiga +0).....8 */
	$"3431 3320 487A 2028 416D 6967 6120 2B31"            /* 413 Hz (Amiga +1 */
	$"2900 0000 0012 3834 3633 2048 7A20 2841"            /* ).....8463 Hz (A */
	$"6D69 6761 202B 3229 0000 0000 1238 3532"            /* miga +2).....852 */
	$"3920 487A 2028 416D 6967 6120 2B33 2900"            /* 9 Hz (Amiga +3). */
	$"0000 0012 3835 3831 2048 7A20 2841 6D69"            /* ....8581 Hz (Ami */
	$"6761 202B 3429 0000 0000 1238 3635 3120"            /* ga +4).....8651  */
	$"487A 2028 416D 6967 6120 2B35 2900 0000"            /* Hz (Amiga +5)... */
	$"0012 3837 3233 2048 7A20 2841 6D69 6761"            /* ..8723 Hz (Amiga */
	$"202B 3629 0000 0000 1238 3735 3720 487A"            /*  +6).....8757 Hz */
	$"2028 416D 6967 6120 2B37 2900 0000 0012"            /*  (Amiga +7)..... */
	$"3832 3830 2048 7A20 2841 6D69 6761 202D"            /* 8280 Hz (Amiga - */
	$"3129 0000 0000 1238 3233 3220 487A 2028"            /* 1).....8232 Hz ( */
	$"416D 6967 6120 2D32 2900 0000 0012 3831"            /* Amiga -2).....81 */
	$"3639 2048 7A20 2841 6D69 6761 202D 3329"            /* 69 Hz (Amiga -3) */
	$"0000 0000 1238 3130 3720 487A 2028 416D"            /* .....8107 Hz (Am */
	$"6967 6120 2D34 2900 0000 0012 3830 3436"            /* iga -4).....8046 */
	$"2048 7A20 2841 6D69 6761 202D 3529 0000"            /*  Hz (Amiga -5).. */
	$"0000 1237 3938 3520 487A 2028 416D 6967"            /* ...7985 Hz (Amig */
	$"6120 2D36 2900 0000 0012 3739 3431 2048"            /* a -6).....7941 H */
	$"7A20 2841 6D69 6761 202D 3729 0000 0000"            /* z (Amiga -7).... */
	$"1237 3839 3520 487A 2028 416D 6967 6120"            /* .7895 Hz (Amiga  */
	$"2D38 2900 0000 0000"                                /* -8)..... */
};

data 'MENU' (164) {
	$"00A4 0000 0000 0000 0000 FFFF EEFF 0554"            /* .§........ˇˇÓˇ.T */
	$"6974 6C65 0355 5341 0000 0000 0643 616E"            /* itle.USA.....Can */
	$"6164 6100 0000 0005 4368 696E 6100 0000"            /* ada.....China... */
	$"0009 486F 6E67 204B 6F6E 6700 0000 000D"            /* .∆Hong Kong..... */
	$"536F 7574 6820 416D 6572 6963 6100 0000"            /* South America... */
	$"0006 5461 6977 616E 0000 0000 0854 6861"            /* ..Taiwan.....Tha */
	$"696C 616E 6400 0000 0001 2D00 0000 0007"            /* iland.....-..... */
	$"456E 676C 616E 6400 0000 0006 4672 616E"            /* England.....Fran */
	$"6365 0000 0000 184F 7468 6572 2045 7572"            /* ce.....Other Eur */
	$"6F70 6561 6E20 436F 756E 7472 6965 7300"            /* opean Countries. */
	$"0000 0001 2D00 0000 000F 4F74 6865 7220"            /* ....-.....Other  */
	$"636F 756E 7472 6965 7300 0000 0000"                 /* countries..... */
};

data 'MENU' (165) {
	$"00A5 0000 0000 0000 0000 FFFF FFFF 024D"            /* .•........ˇˇˇˇ.M */
	$"5304 3120 6D73 0000 0000 0531 3020 6D73"            /* S.1 ms.....10 ms */
	$"0000 0000 0532 3520 6D73 0000 0000 0533"            /* .....25 ms.....3 */
	$"3020 6D73 0000 0000 0533 3520 6D73 0000"            /* 0 ms.....35 ms.. */
	$"0000 0534 3020 6D73 0000 0000 0534 3520"            /* ...40 ms.....45  */
	$"6D73 0000 0000 0535 3020 6D73 0000 0000"            /* ms.....50 ms.... */
	$"0536 3020 6D73 0000 0000 0537 3020 6D73"            /* .60 ms.....70 ms */
	$"0000 0000 0538 3020 6D73 0000 0000 0539"            /* .....80 ms.....9 */
	$"3020 6D73 0000 0000 0631 3030 206D 7300"            /* 0 ms.....100 ms. */
	$"0000 0006 3230 3020 6D73 0000 0000 00"              /* ....200 ms..... */
};

data 'MENU' (166) {
	$"00A6 0000 0000 0000 0000 FFFF FFFF 0450"            /* .¶........ˇˇˇˇ.P */
	$"5049 4E00"                                          /* PIN. */
};

data 'MENU' (167) {
	$"00A7 0000 0000 0000 0000 FFFF FFFF 0450"            /* .ß........ˇˇˇˇ.P */
	$"5049 4E00"                                          /* PIN. */
};

data 'MENU' (168) {
	$"00A8 0000 0000 0000 0000 FFFF FFFF 094F"            /* .®........ˇˇˇˇ∆O */
	$"7363 6920 4D6F 6465 0553 7461 636B 0000"            /* sci Mode.Stack.. */
	$"0000 0454 696C 6500 0000 0000"                      /* ...Tile..... */
};

data 'MENU' (169) {
	$"00A9 0000 0000 0000 0000 FFFF FFFF 0554"            /* .©........ˇˇˇˇ.T */
	$"6974 6C65 0431 3020 2500 0000 0004 3230"            /* itle.10 %.....20 */
	$"2025 0000 0000 0433 3020 2500 0000 0004"            /*  %.....30 %..... */
	$"3430 2025 0000 0000 0435 3020 2500 0000"            /* 40 %.....50 %... */
	$"0004 3630 2025 0000 0000 0437 3020 2500"            /* ..60 %.....70 %. */
	$"0000 0000"                                          /* .... */
};

data 'MENU' (170) {
	$"00AA 0000 0000 0000 0000 FFFF FFFF 024D"            /* .™........ˇˇˇˇ.M */
	$"5305 3235 206D 7300 0000 0005 3330 206D"            /* S.25 ms.....30 m */
	$"7300 0000 0005 3335 206D 7300 0000 0005"            /* s.....35 ms..... */
	$"3430 206D 7300 0000 0005 3435 206D 7300"            /* 40 ms.....45 ms. */
	$"0000 0005 3530 206D 7300 0000 0005 3630"            /* ....50 ms.....60 */
	$"206D 7300 0000 0005 3730 206D 7300 0000"            /*  ms.....70 ms... */
	$"0005 3830 206D 7300 0000 0005 3930 206D"            /* ..80 ms.....90 m */
	$"7300 0000 0006 3130 3020 6D73 0000 0000"            /* s.....100 ms.... */
	$"0632 3030 206D 7300 0000 0006 3330 3020"            /* .200 ms.....300  */
	$"6D73 0000 0000 0634 3030 206D 7300 0000"            /* ms.....400 ms... */
	$"0000"                                               /* .. */
};

data 'MENU' (500) {
	$"01F4 0000 0000 0000 0000 FFFF FFAF 0453"            /* .Ù........ˇˇˇØ.S */
	$"686F 7715 416C 6C20 4176 6169 6C61 626C"            /* how.All Availabl */
	$"6520 666F 726D 6174 7300 0000 0012 416C"            /* e formats.....Al */
	$"6C20 5265 6164 6162 6C65 2066 696C 6573"            /* l Readable files */
	$"0000 0000 0941 6C6C 2066 696C 6573 0000"            /* ....∆All files.. */
	$"0000 012D 0000 0000 0E4D 4144 4920 506C"            /* ...-.....MADI Pl */
	$"6179 6572 5052 4F00 0000 0001 2D00 0000"            /* ayerPRO.....-... */
	$"0000"                                               /* .. */
};

data 'MENU' (171) {
	$"00AB 0000 0000 0000 0000 FFFF FFFF 0554"            /* .´........ˇˇˇˇ.T */
	$"6974 6C65 0331 3030 0000 0000 0331 3130"            /* itle.100.....110 */
	$"0000 0000 0331 3230 0000 0000 0331 3330"            /* .....120.....130 */
	$"0000 0000 0331 3430 0000 0000 0331 3630"            /* .....140.....160 */
	$"0000 0000 0331 3830 0000 0000 0332 3030"            /* .....180.....200 */
	$"0000 0000 00"                                       /* ..... */
};

data 'MENU' (172) {
	$"00A5 0000 0000 0000 0000 FFFF FFFF 044D"            /* .•........ˇˇˇˇ.M */
	$"6F64 6504 4D6F 6E6F 0000 0000 0653 7465"            /* ode.Mono.....Ste */
	$"7265 6F00 0000 0000"                                /* reo..... */
};

data 'MENU' (173) {
	$"00AD 0000 0000 0000 0000 FFFF FFFF 0100"            /* .≠........ˇˇˇˇ.. */
	$"0C43 6C61 7373 6963 204C 6F6F 7000 0000"            /* .Classic Loop... */
	$"000E 5069 6E67 2D50 6F6E 6720 4C6F 6F70"            /* ..Ping-Pong Loop */
	$"0000 0000 00"                                       /* ..... */
};

data 'MENU' (142) {
	$"008E 0000 0000 0000 0000 FFFF FFFF 0D54"            /* .é........ˇˇˇˇ.T */
	$"7261 636B 7320 4E75 6D62 6572 00"                   /* racks Number. */
};

data 'MENU' (174) {
	$"00AE 0000 0000 0000 0000 FFFF FFFF 0554"            /* .Æ........ˇˇˇˇ.T */
	$"6974 6C65 094E 6F20 4564 6974 6F72 0000"            /* itle∆No Editor.. */
	$"0000 0E44 6967 6974 616C 2045 6469 746F"            /* ...Digital Edito */
	$"7200 0000 000A 426F 7820 4564 6974 6F72"            /* r....¬Box Editor */
	$"0000 0000 0E43 6C61 7373 6963 2045 6469"            /* .....Classic Edi */
	$"746F 7200 0000 000C 5761 7665 2050 7265"            /* tor.....Wave Pre */
	$"7669 6577 0000 0000 0C50 6174 7465 726E"            /* view.....Pattern */
	$"2056 6965 7700 0000 0000"                           /*  View..... */
};

data 'MENU' (130) {
	$"0082 0000 0000 0000 0000 FFFF FFBB 0B49"            /* .Ç........ˇˇˇª.I */
	$"6E73 7472 756D 656E 7473 1049 6E73 7472"            /* nstruments.Instr */
	$"756D 656E 7473 204C 6973 7400 4C00 0001"            /* uments List.L... */
	$"2D00 0000 0006 4E65 772E 2E2E 001B 9A00"            /* -.....New.....ö. */
	$"0C45 7870 6F72 7420 4173 2E2E 2E00 0000"            /* .Export As...... */
	$"0006 4465 6C65 7465 0000 0000 012D 0000"            /* ..Delete.....-.. */
	$"0000 1853 6176 6520 496E 7374 7275 6D65"            /* ...Save Instrume */
	$"6E74 7320 4C69 7374 2E2E 2E00 0000 0000"            /* nts List........ */
};

data 'MENU' (200, "Prefs") {
	$"00C8 0000 0000 0000 0000 FFFF FFFF 0B50"            /* .»........ˇˇˇˇ.P */
	$"7265 6665 7265 6E63 6573 0F44 7269 7665"            /* references.Drive */
	$"7220 2D20 4F75 7470 7574 0000 0000 0550"            /* r - Output.....P */
	$"6961 6E6F 0000 0000 0A4D 7573 6963 204C"            /* iano....¬Music L */
	$"6973 7400 0000 0005 436F 6C6F 7200 0000"            /* ist.....Color... */
	$"0004 4D69 7363 0000 0000 0A42 6F78 2045"            /* ..Misc....¬Box E */
	$"6469 746F 7200 0000 000E 4469 6769 7461"            /* ditor.....Digita */
	$"6C20 4564 6974 6F72 0000 0000 0E43 6C61"            /* l Editor.....Cla */
	$"7373 6963 2045 6469 746F 7200 0000 0005"            /* ssic Editor..... */
	$"464B 4559 7300 0000 000D 4D49 4449 2048"            /* FKEYs.....MIDI H */
	$"6172 6477 6172 6500 0000 0000"                      /* ardware..... */
};

data 'MENU' (133) {
	$"0085 0000 0000 0000 0000 FFFF FFF7 0114"            /* .Ö........ˇˇˇ˜.. */
	$"0F41 626F 7574 2050 6C61 7965 7250 524F"            /* .About PlayerPRO */
	$"0000 0000 1647 656E 6572 616C 2049 6E66"            /* .....General Inf */
	$"6F72 6D61 7469 6F6E 2E2E 2E00 0000 0001"            /* ormation........ */
	$"2D00 0000 0000"                                     /* -..... */
};

data 'MENU' (175) {
	$"00AF 0000 0000 0000 0000 FFFF FFFF 0C4F"            /* .Ø........ˇˇˇˇ.O */
	$"7665 7253 616D 706C 696E 6703 3220 7800"            /* verSampling.2 x. */
	$"0000 0003 3320 7800 0000 0003 3420 7800"            /* ....3 x.....4 x. */
	$"0000 0003 3620 7800 0000 0003 3820 7800"            /* ....6 x.....8 x. */
	$"0000 0004 3130 2078 0000 0000 00"                   /* ....10 x..... */
};

data 'MENU' (154) {
	$"009A 0000 0000 0000 0000 FFFF FFFF 0554"            /* .ö........ˇˇˇˇ.T */
	$"6974 6C65 1049 6D70 6F72 7420 6120 4669"            /* itle.Import a Fi */
	$"6C65 2E2E 2E00 0000 0016 5369 6C65 6E63"            /* le........Silenc */
	$"652F 546F 6E65 2047 656E 6572 6174 6F72"            /* e/Tone Generator */
	$"0000 0000 1452 6563 6F72 6420 2841 7564"            /* .....Record (Aud */
	$"696F 2049 6E70 7574 2900 0000 0014 5175"            /* io Input).....Qu */
	$"6963 6B74 696D 6520 496E 7374 7275 6D65"            /* icktime Instrume */
	$"6E74 0000 0000 1252 4157 2044 6174 6120"            /* nt.....RAW Data  */
	$"496D 706F 7274 2E2E 2E00 0000 0012 4175"            /* Import........Au */
	$"6469 6F20 4344 2049 6D70 6F72 742E 2E2E"            /* dio CD Import... */
	$"0000 0000 00"                                       /* ..... */
};

data 'MENU' (132) {
	$"0084 0000 0000 0000 0000 FFFF FF79 0445"            /* .Ñ........ˇˇˇy.E */
	$"6469 7404 556E 646F 005A 0000 012D 0000"            /* dit.Undo.Z...-.. */
	$"0000 0343 7574 0058 0000 0443 6F70 7900"            /* ...Cut.X...Copy. */
	$"4300 0005 5061 7374 6500 5600 0005 436C"            /* C...Paste.V...Cl */
	$"6561 7200 0000 0001 2D00 0000 0009 4475"            /* ear.....-....∆Du */
	$"706C 6963 6174 6500 0000 000A 5365 6C65"            /* plicate....¬Sele */
	$"6374 2041 6C6C 0041 0000 0B47 6574 2049"            /* ct All.A...Get I */
	$"6E66 6F2E 2E2E 0049 0000 00"                        /* nfo....I... */
};

data 'MENU' (140) {
	$"008C 0000 0000 0000 0000 FFFF F6DF 0645"            /* .å........ˇˇˆﬂ.E */
	$"6469 746F 720E 4469 6769 7461 6C20 4564"            /* ditor.Digital Ed */
	$"6974 6F72 0044 0000 0A42 6F78 2045 6469"            /* itor.D..¬Box Edi */
	$"746F 7200 0000 000E 436C 6173 7369 6320"            /* tor.....Classic  */
	$"4564 6974 6F72 0000 0000 0C57 6176 6520"            /* Editor.....Wave  */
	$"5072 6576 6965 7700 0000 0001 2D00 0000"            /* Preview.....-... */
	$"0007 4669 6E64 2E2E 2E00 4600 0011 4669"            /* ..Find....F...Fi */
	$"6E64 2043 7572 7265 6E74 204E 6F74 6500"            /* nd Current Note. */
	$"4500 0001 2D00 0000 0008 4D69 6E69 6D69"            /* E...-.....Minimi */
	$"7A65 0000 0000 1F43 6F6E 7665 7274 2061"            /* ze.....Convert a */
	$"6C6C 2050 6174 7465 726E 7320 746F 2036"            /* ll Patterns to 6 */
	$"3420 726F 7773 0000 0000 012D 0000 0000"            /* 4 rows.....-.... */
	$"1347 656E 6572 616C 2049 6E66 6F72 6D61"            /* .General Informa */
	$"7469 6F6E 0000 0000 00"                             /* tion..... */
};

data 'MENU' (176) {
	$"00B0 0000 0000 0000 0000 FFFF FFEF 0757"            /* .∞........ˇˇˇÔ.W */
	$"696E 646F 7773 0100 0031 0000 0100 0032"            /* indows...1.....2 */
	$"0000 0100 0033 0000 012D 0000 0000 0853"            /* .....3...-.....S */
	$"6574 7469 6E67 7300 0000 0000"                      /* ettings..... */
};

data 'MENU' (178) {
	$"00B2 0000 0000 0000 0000 FFFF FFFF 0453"            /* .≤........ˇˇˇˇ.S */
	$"7465 7001 3100 0000 0001 3200 0000 0001"            /* tep.1.....2..... */
	$"3300 0000 0001 3400 0000 0001 3500 0000"            /* 3.....4.....5... */
	$"0001 3600 0000 0001 3700 0000 0001 3800"            /* ..6.....7.....8. */
	$"0000 0001 3900 0000 0000"                           /* ....9..... */
};

data 'MENU' (177) {
	$"00B1 0000 0000 0000 0000 FFFF FFFF 0E4E"            /* .±........ˇˇˇˇ.N */
	$"6F20 636F 6D70 7265 7373 696F 6E0E 4E6F"            /* o compression.No */
	$"2063 6F6D 7072 6573 7369 6F6E 0000 0000"            /*  compression.... */
	$"044D 4143 3300 0000 0004 4D41 4336 0000"            /* .MAC3.....MAC6.. */
	$"0000 0469 6D61 3400 0000 0004 756C 6177"            /* ...ima4.....ulaw */
	$"0000 0000 0461 6C61 7700 0000 0004 5144"            /* .....alaw.....QD */
	$"4D43 0000 0000 0451 444D 3200 0000 0000"            /* MC.....QDM2..... */
};

data 'MENU' (150) {
	$"0096 0000 0000 0000 0000 FFFF FFEF 0100"            /* .ñ........ˇˇˇÔ.. */
	$"0C41 7564 696F 204F 7574 5075 7400 0000"            /* .Audio OutPut... */
	$"000D 4472 6976 6572 2054 7261 636B 7300"            /* ..Driver Tracks. */
	$"0000 000B 4175 6469 6F20 496E 5075 7400"            /* ....Audio InPut. */
	$"0000 0011 5175 6963 6B74 696D 6520 4368"            /* ....Quicktime Ch */
	$"616E 6E65 6C00 0000 0000"                           /* annel..... */
};

data 'MENU' (131) {
	$"0083 0000 0000 0000 0000 FFFF FDBB 0556"            /* .É........ˇˇ˝ª.V */
	$"6965 7773 1053 686F 7720 4F6E 6C69 6E65"            /* iews.Show Online */
	$"2048 656C 7000 0000 0001 2D00 0000 000C"            /*  Help.....-..... */
	$"5061 7474 6572 6E20 5669 6577 0000 0000"            /* Pattern View.... */
	$"0C44 6967 6974 616C 2056 6965 7700 0000"            /* .Digital View... */
	$"000F 496E 7374 7275 6D65 6E74 2056 6965"            /* ..Instrument Vie */
	$"7700 0000 0001 2D00 0000 000C 4F73 6369"            /* w.....-.....Osci */
	$"6C6C 6F20 5669 6577 0000 0000 0D53 7065"            /* llo View.....Spe */
	$"6374 7275 6D20 5669 6577 0000 0000 012D"            /* ctrum View.....- */
	$"0000 0000 0554 6F6F 6C73 0054 0000 0A4D"            /* .....Tools.T..¬M */
	$"7573 6963 204C 6973 7400 4D00 0005 4D69"            /* usic List.M...Mi */
	$"7865 7200 4200 0009 4571 7561 6C69 7A65"            /* xer.B..∆Equalize */
	$"7200 0000 0005 5069 616E 6F00 5000 000B"            /* r.....Piano.P... */
	$"4D65 6D6F 7279 2056 6965 7700 0000 0000"            /* Memory View..... */
};

data 'MENU' (162) {
	$"00A2 0000 0000 0000 0000 FFFF FFFF 0554"            /* .¢........ˇˇˇˇ.T */
	$"6974 6C65 0F53 656C 6563 7469 6F6E 2D3E"            /* itle.Selection-> */
	$"6C6F 6F70 0000 0000 0F4C 6F6F 702D 3E53"            /* loop.....Loop->S */
	$"656C 6563 7469 6F6E 0000 0000 1153 6574"            /* election.....Set */
	$"2053 7573 7461 696E 2050 6F69 6E74 0000"            /*  Sustain Point.. */
	$"0000 00"                                            /* ... */
};

data 'MENU' (136) {
	$"0088 0000 0000 0000 0000 FFFF FF6F 0645"            /* .à........ˇˇˇo.E */
	$"6666 6563 740F 5365 6C65 6374 696F 6E2D"            /* ffect.Selection- */
	$"3E4C 6F6F 7000 0000 000F 4C6F 6F70 2D3E"            /* >Loop.....Loop-> */
	$"5365 6C65 6374 696F 6E00 0000 0006 4465"            /* Selection.....De */
	$"6C65 7465 0000 0000 012D 0000 0000 0F46"            /* lete.....-.....F */
	$"4654 202D 2048 7A20 4669 6C74 6572 0000"            /* FT - Hz Filter.. */
	$"0000 0E46 4654 202D 2048 7A20 5368 6966"            /* ...FFT - Hz Shif */
	$"7400 0000 0001 2D00 0000 0000"                      /* t.....-..... */
};

data 'MENU' (135) {
	$"0087 0000 0000 0000 0000 FFFF FFFF 0745"            /* .á........ˇˇˇˇ.E */
	$"6666 6563 743A 1130 2D4E 6F72 6D61 6C2F"            /* ffect:.0-Normal/ */
	$"4172 7065 6767 696F 0000 0000 0A31 2D53"            /* Arpeggio....¬1-S */
	$"6C69 6465 2055 7000 0000 000C 322D 536C"            /* lide Up.....2-Sl */
	$"6964 6520 446F 776E 0000 0000 0C33 2D50"            /* ide Down.....3-P */
	$"6F72 7461 6D65 6E74 6F00 0000 000A 342D"            /* ortamento....¬4- */
	$"0056 6962 7261 746F 0000 0000 1635 2D50"            /* .Vibrato.....5-P */
	$"6F72 7461 6D65 6E74 6F2B 566F 6C20 536C"            /* ortamento+Vol Sl */
	$"6964 6500 0000 0013 362D 5669 6272 6174"            /* ide.....6-Vibrat */
	$"6F2B 566F 6C20 536C 6964 6500 0000 0009"            /* o+Vol Slide....∆ */
	$"372D 5472 656D 6F6C 6F00 0000 000D 382D"            /* 7-Tremolo.....8- */
	$"5365 7420 5061 6E6E 696E 6700 0000 0012"            /* Set Panning..... */
	$"392D 5365 7420 5361 6D70 6C65 4F66 6673"            /* 9-Set SampleOffs */
	$"6574 0000 0000 0D41 2D56 6F6C 756D 6553"            /* et.....A-VolumeS */
	$"6C69 6465 0000 0000 0F42 2D50 6F73 6974"            /* lide.....B-Posit */
	$"696F 6E20 4A75 6D70 0000 0000 0C43 2D53"            /* ion Jump.....C-S */
	$"6574 2056 6F6C 756D 6500 0000 000F 442D"            /* et Volume.....D- */
	$"5061 7474 6572 6E20 4272 6561 6B00 0000"            /* Pattern Break... */
	$"000C 452D 4520 436F 6D6D 616E 6473 0000"            /* ..E-E Commands.. */
	$"0000 0B46 2D53 6574 2053 7065 6564 0000"            /* ...F-Set Speed.. */
	$"0000 2147 2D4E 6F74 6520 4F46 4620 284D"            /* ..!G-Note OFF (M */
	$"756C 7469 2D43 6861 6E6E 656C 2054 7261"            /* ulti-Channel Tra */
	$"636B 7329 0000 0000 00"                             /* cks)..... */
};

data 'MENU' (179) {
	$"00B3 0000 0000 0000 0000 FFFF FFAF 0849"            /* .≥........ˇˇˇØ.I */
	$"6E74 6572 6E65 7417 506C 6179 6572 5052"            /* nternet.PlayerPR */
	$"4F20 4F66 6669 6369 616C 2053 6974 6500"            /* O Official Site. */
	$"0000 0019 4D49 5420 4172 6368 6976 6520"            /* ....MIT Archive  */
	$"666F 7220 506C 6179 6572 5052 4F00 0000"            /* for PlayerPRO... */
	$"0020 456D 6169 6C20 746F 2074 6865 2061"            /* . Email to the a */
	$"7574 686F 7220 6F66 2050 6C61 7965 7250"            /* uthor of PlayerP */
	$"524F 0000 0000 012D 0000 0000 1741 6464"            /* RO.....-.....Add */
	$"2F52 656D 6F76 6520 626F 6F6B 6D61 726B"            /* /Remove bookmark */
	$"732E 2E2E 0000 0000 012D 0000 0000 00"              /* s........-..... */
};

data 'MENU' (161) {
	$"07D0 0000 0000 00C8 0000 FFFF FFFF 0653"            /* .–.....»..ˇˇˇˇ.S */
	$"616D 706C 6501 3000 0000 0001 3100 0000"            /* ample.0.....1... */
	$"0001 3200 0000 0001 3300 0000 0001 3400"            /* ..2.....3.....4. */
	$"0000 0001 3500 0000 0001 3600 0000 0001"            /* ....5.....6..... */
	$"3700 0000 0001 3800 0000 0001 3900 0000"            /* 7.....8.....9... */
	$"0001 4100 0000 0001 4200 0000 0001 4300"            /* ..A.....B.....C. */
	$"0000 0001 4400 0000 0001 4500 0000 0001"            /* ....D.....E..... */
	$"4600 0000 0001 3000 0000 0001 3100 0000"            /* F.....0.....1... */
	$"0001 3200 0000 0001 3300 0000 0001 3400"            /* ..2.....3.....4. */
	$"0000 0001 3500 0000 0001 3600 0000 0001"            /* ....5.....6..... */
	$"3700 0000 0001 3800 0000 0001 3900 0000"            /* 7.....8.....9... */
	$"0001 4100 0000 0001 4200 0000 0001 4300"            /* ..A.....B.....C. */
	$"0000 0001 4400 0000 0001 4500 0000 0001"            /* ....D.....E..... */
	$"4600 0000 0001 3000 0000 0001 3100 0000"            /* F.....0.....1... */
	$"0001 3200 0000 0001 3300 0000 0001 3400"            /* ..2.....3.....4. */
	$"0000 0001 3500 0000 0001 3600 0000 0001"            /* ....5.....6..... */
	$"3700 0000 0001 3800 0000 0001 3900 0000"            /* 7.....8.....9... */
	$"0001 4100 0000 0001 4200 0000 0001 4300"            /* ..A.....B.....C. */
	$"0000 0001 4400 0000 0001 4500 0000 0001"            /* ....D.....E..... */
	$"4600 0000 0001 3000 0000 0001 3100 0000"            /* F.....0.....1... */
	$"0001 3200 0000 0001 3300 0000 0001 3400"            /* ..2.....3.....4. */
	$"0000 0001 3500 0000 0001 3600 0000 0001"            /* ....5.....6..... */
	$"3700 0000 0001 3800 0000 0001 3900 0000"            /* 7.....8.....9... */
	$"0001 4100 0000 0001 4200 0000 0001 4300"            /* ..A.....B.....C. */
	$"0000 0001 4400 0000 0001 4500 0000 0001"            /* ....D.....E..... */
	$"4600 0000 0001 3000 0000 0001 3100 0000"            /* F.....0.....1... */
	$"0001 3200 0000 0001 3300 0000 0001 3400"            /* ..2.....3.....4. */
	$"0000 0001 3500 0000 0001 3600 0000 0001"            /* ....5.....6..... */
	$"3700 0000 0001 3800 0000 0001 3900 0000"            /* 7.....8.....9... */
	$"0001 4100 0000 0001 4200 0000 0001 4300"            /* ..A.....B.....C. */
	$"0000 0001 4400 0000 0001 4500 0000 0001"            /* ....D.....E..... */
	$"4600 0000 0001 3000 0000 0001 3100 0000"            /* F.....0.....1... */
	$"0001 3200 0000 0001 3300 0000 0001 3400"            /* ..2.....3.....4. */
	$"0000 0001 3500 0000 0001 3600 0000 0001"            /* ....5.....6..... */
	$"3700 0000 0001 3800 0000 0001 3900 0000"            /* 7.....8.....9... */
	$"0001 4100 0000 0001 4200 0000 0001 4300"            /* ..A.....B.....C. */
	$"0000 0001 4400 0000 0001 4500 0000 0001"            /* ....D.....E..... */
	$"4600 0000 0001 3000 0000 0001 3100 0000"            /* F.....0.....1... */
	$"0001 3200 0000 0001 3300 0000 0001 3400"            /* ..2.....3.....4. */
	$"0000 0001 3500 0000 0001 3600 0000 0001"            /* ....5.....6..... */
	$"3700 0000 0001 3800 0000 0001 3900 0000"            /* 7.....8.....9... */
	$"0001 4100 0000 0001 4200 0000 0001 4300"            /* ..A.....B.....C. */
	$"0000 0001 4400 0000 0001 4500 0000 0001"            /* ....D.....E..... */
	$"4600 0000 0001 3000 0000 0001 3100 0000"            /* F.....0.....1... */
	$"0001 3200 0000 0001 3300 0000 0001 3400"            /* ..2.....3.....4. */
	$"0000 0001 3500 0000 0001 3600 0000 0001"            /* ....5.....6..... */
	$"3700 0000 0001 3800 0000 0001 3900 0000"            /* 7.....8.....9... */
	$"0001 4100 0000 0001 4200 0000 0001 4300"            /* ..A.....B.....C. */
	$"0000 0001 4400 0000 0001 4500 0000 0001"            /* ....D.....E..... */
	$"4600 0000 0001 3000 0000 0001 3100 0000"            /* F.....0.....1... */
	$"0001 3200 0000 0001 3300 0000 0001 3400"            /* ..2.....3.....4. */
	$"0000 0001 3500 0000 0001 3600 0000 0001"            /* ....5.....6..... */
	$"3700 0000 0001 3800 0000 0001 3900 0000"            /* 7.....8.....9... */
	$"0001 4100 0000 0001 4200 0000 0001 4300"            /* ..A.....B.....C. */
	$"0000 0001 4400 0000 0001 4500 0000 0001"            /* ....D.....E..... */
	$"4600 0000 0001 3000 0000 0001 3100 0000"            /* F.....0.....1... */
	$"0001 3200 0000 0001 3300 0000 0001 3400"            /* ..2.....3.....4. */
	$"0000 0001 3500 0000 0001 3600 0000 0001"            /* ....5.....6..... */
	$"3700 0000 0001 3800 0000 0001 3900 0000"            /* 7.....8.....9... */
	$"0001 4100 0000 0001 4200 0000 0001 4300"            /* ..A.....B.....C. */
	$"0000 0001 4400 0000 0001 4500 0000 0001"            /* ....D.....E..... */
	$"4600 0000 0001 3000 0000 0001 3100 0000"            /* F.....0.....1... */
	$"0001 3200 0000 0001 3300 0000 0001 3400"            /* ..2.....3.....4. */
	$"0000 0001 3500 0000 0001 3600 0000 0001"            /* ....5.....6..... */
	$"3700 0000 0001 3800 0000 0001 3900 0000"            /* 7.....8.....9... */
	$"0001 4100 0000 0001 4200 0000 0001 4300"            /* ..A.....B.....C. */
	$"0000 0001 4400 0000 0001 4500 0000 0001"            /* ....D.....E..... */
	$"4600 0000 0001 3000 0000 0001 3100 0000"            /* F.....0.....1... */
	$"0001 3200 0000 0001 3300 0000 0001 3400"            /* ..2.....3.....4. */
	$"0000 0001 3500 0000 0001 3600 0000 0001"            /* ....5.....6..... */
	$"3700 0000 0001 3800 0000 0001 3900 0000"            /* 7.....8.....9... */
	$"0001 4100 0000 0001 4200 0000 0001 4300"            /* ..A.....B.....C. */
	$"0000 0001 4400 0000 0001 4500 0000 0001"            /* ....D.....E..... */
	$"4600 0000 0001 3000 0000 0001 3100 0000"            /* F.....0.....1... */
	$"0001 3200 0000 0001 3300 0000 0001 3400"            /* ..2.....3.....4. */
	$"0000 0001 3500 0000 0001 3600 0000 0001"            /* ....5.....6..... */
	$"3700 0000 0001 3800 0000 0001 3900 0000"            /* 7.....8.....9... */
	$"0001 4100 0000 0001 4200 0000 0001 4300"            /* ..A.....B.....C. */
	$"0000 0001 4400 0000 0001 4500 0000 0001"            /* ....D.....E..... */
	$"4600 0000 0001 3000 0000 0001 3100 0000"            /* F.....0.....1... */
	$"0001 3200 0000 0001 3300 0000 0001 3400"            /* ..2.....3.....4. */
	$"0000 0001 3500 0000 0001 3600 0000 0001"            /* ....5.....6..... */
	$"3700 0000 0001 3800 0000 0001 3900 0000"            /* 7.....8.....9... */
	$"0001 4100 0000 0001 4200 0000 0001 4300"            /* ..A.....B.....C. */
	$"0000 0001 4400 0000 0001 4500 0000 0001"            /* ....D.....E..... */
	$"4600 0000 0001 3000 0000 0001 3100 0000"            /* F.....0.....1... */
	$"0001 3200 0000 0001 3300 0000 0001 3400"            /* ..2.....3.....4. */
	$"0000 0001 3500 0000 0001 3600 0000 0001"            /* ....5.....6..... */
	$"3700 0000 0001 3800 0000 0001 3900 0000"            /* 7.....8.....9... */
	$"0001 4100 0000 0001 4200 0000 0001 4300"            /* ..A.....B.....C. */
	$"0000 0001 4400 0000 0001 4500 0000 0001"            /* ....D.....E..... */
	$"4600 0000 0001 3000 0000 0001 3100 0000"            /* F.....0.....1... */
	$"0001 3200 0000 0001 3300 0000 0001 3400"            /* ..2.....3.....4. */
	$"0000 0001 3500 0000 0001 3600 0000 0001"            /* ....5.....6..... */
	$"3700 0000 0001 3800 0000 0001 3900 0000"            /* 7.....8.....9... */
	$"0001 4100 0000 0001 4200 0000 0001 4300"            /* ..A.....B.....C. */
	$"0000 0001 4400 0000 0001 4500 0000 0001"            /* ....D.....E..... */
	$"4600 0000 0000"                                     /* F..... */
};

data 'DLOG' (1002, "Open Dialog") {
	$"0036 002A 013C 0180 0005 0000 0000 0000"            /* .6.*.<.Ä........ */
	$"0000 0081 0B4F 7065 6E20 4469 616C 6F67"            /* ...Å.Open Dialog */
};

data 'DLOG' (144, "PatList") {
	$"0064 0108 013B 01D0 0008 0000 0100 0000"            /* .d...;.–........ */
	$"0005 0090 0D50 6174 7465 726E 7320 4C69"            /* ...ê.Patterns Li */
	$"7374"                                               /* st */
};

data 'DLOG' (147, "Classic") {
	$"004C 0018 00D8 01DB 0008 0000 0100 0000"            /* .L...ÿ.€........ */
	$"000B 0093 0950 6172 7469 7469 6F6E"                 /* ...ì∆Partition */
};

data 'DLOG' (1003, "Save Dialog Music") {
	$"0046 0026 0168 017D 0005 0000 0000 0000"            /* .F.&.h.}........ */
	$"0057 0083 0B53 6176 6520 4469 616C 6F67"            /* .W.É.Save Dialog */
};

data 'DLOG' (129, "Instrument Viewer") {
	$"0034 0031 0078 0104 000C 0000 0100 0000"            /* .4.1.x.......... */
	$"000E 034D 1149 6E73 7472 756D 656E 7420"            /* ...M.Instrument  */
	$"5669 6577 6572"                                     /* Viewer */
};

data 'DLOG' (150, "Digital Info") {
	$"004E 0064 0141 01C4 0008 0000 0100 0000"            /* .N.d.A.ƒ........ */
	$"000F 00C1 1344 6967 6974 616C 2049 6E66"            /* ...¡.Digital Inf */
	$"6F72 6D61 7469 6F6E"                                /* ormation */
};

data 'DLOG' (152, "Find/Replace") {
	$"0034 0028 00D4 01DA 0005 0000 0100 0000"            /* .4.(.‘.⁄........ */
	$"0000 0098 0E46 696E 6420 2F20 5265 706C"            /* ...ò.Find / Repl */
	$"6163 65"                                            /* ace */
};

data 'DLOG' (135, "Box Editor") {
	$"0036 000A 0229 0346 0008 0000 0100 0000"            /* .6.¬.).F........ */
	$"000D 0087 1043 6C61 7373 6963 616C 2045"            /* ...á.Classical E */
	$"6469 746F 72"                                       /* ditor */
};

data 'DLOG' (155, "Color") {
	$"006A 0012 00E6 01F6 0005 0000 0100 0000"            /* .j...Ê.ˆ........ */
	$"2703 009B 1243 686F 6F73 6520 6120 6E65"            /* '..õ.Choose a ne */
	$"7720 636F 6C6F 72"                                  /* w color */
};

data 'DLOG' (157, "Track Viewer") {
	$"0046 002F 008A 0103 000C 0000 0100 0000"            /* .F./.ä.......... */
	$"0012 00AA 0C54 7261 636B 2056 6965 7765"            /* ...™.Track Viewe */
	$"72"                                                 /* r */
};

data 'DLOG' (158, "Debug") {
	$"0074 004E 00E0 018B 0005 0100 0100 0000"            /* .t.N.‡.ã........ */
	$"0000 026F 1250 6572 666F 726D 616E 6365"            /* ...o.Performance */
	$"2057 696E 646F 77A5 280A"                           /*  Window•(¬ */
};

data 'DLOG' (163, "Memory") {
	$"004A 0096 00C3 01C6 000C 0000 0100 0000"            /* .J.ñ.√.∆........ */
	$"0013 016C 064D 656D 6F72 79"                        /* ...l.Memory */
};

data 'DLOG' (164, "Spectrum") {
	$"0070 006A 00F0 01DC 0008 0000 0100 0000"            /* .p.j..‹........ */
	$"0014 0151 0853 7065 6374 7275 6D"                   /* ...Q.Spectrum */
};

data 'DLOG' (167, "Pattern Info") {
	$"0060 00A4 00DD 019E 0005 0000 0100 0000"            /* .`.§.›.û........ */
	$"0000 00D9 0C50 6174 7465 726E 2049 6E66"            /* ...Ÿ.Pattern Inf */
	$"6FF9 280A"                                          /* o˘(¬ */
};

data 'DLOG' (168, "File information") {
	$"0065 0083 010B 01DF 0005 0000 0100 0000"            /* .e.É...ﬂ........ */
	$"03C6 013D 1046 696C 6520 496E 666F 726D"            /* .∆.=.File Inform */
	$"6174 696F 6E"                                       /* ation */
};

data 'DLOG' (169, "Help PP") {
	$"004C 006E 0153 024F 0008 0000 0100 0000"            /* .L.n.S.O........ */
	$"0016 00C3 0448 656C 7072 280A"                      /* ...√.Helpr(¬ */
};

data 'DLOG' (161, "SoundFormat Export") {
	$"0051 004D 00A5 0155 0005 0000 0100 0000"            /* .Q.M.•.U........ */
	$"1C8F 00AB 1153 6F75 6E64 2046 696C 6520"            /* .è.´.Sound File  */
	$"4578 706F 7274 280A"                                /* Export(¬ */
};

data 'DLOG' (170, "MIDI Channels") {
	$"009C 0124 01E4 0348 0005 0100 0100 0000"            /* .ú.$.‰.H........ */
	$"0000 0156 134D 4944 4920 4368 616E 6E65"            /* ...V.MIDI Channe */
	$"6C73 2053 6574 7570"                                /* ls Setup */
};

data 'DLOG' (172, "Move") {
	$"005C 000C 00E5 0217 0005 0100 0100 0000"            /* .\...Â.......... */
	$"0000 00AC 0A4D 6F76 6520 6E6F 7465 736E"            /* ...¨¬Move notesn */
	$"280A"                                               /* (¬ */
};

data 'DLOG' (179, "OpenDialog Sample/Instrument") {
	$"002C 002A 0198 0181 0005 0000 0000 0000"            /* .,.*.ò.Å........ */
	$"0000 00A1 0B4F 7065 6E20 4469 616C 6F67"            /* ...°.Open Dialog */
};

data 'DLOG' (171, "StartUp Dialog") {
	$"0056 0049 012D 01CA 0001 0100 0100 0000"            /* .V.I.-. ........ */
	$"0000 015A 0000 280A"                                /* ...Z..(¬ */
};

data 'DLOG' (128, "Oscilloscope") {
	$"002A 0002 015B 01E2 0008 0000 0100 0000"            /* .*...[.‚........ */
	$"000C 0080 0C4F 7363 696C 6C6F 7363 6F70"            /* ...Ä.Oscilloscop */
	$"65"                                                 /* e */
};

data 'DLOG' (176, "Quicktime Instruments") {
	$"0044 0036 00DC 01FF 0005 0000 0100 0000"            /* .D.6.‹.ˇ........ */
	$"0FB7 00B0 1551 7569 636B 7469 6D65 2049"            /* .∑.∞.Quicktime I */
	$"6E73 7472 756D 656E 7473"                           /* nstruments */
};

data 'DLOG' (173, "Active Help") {
	$"0014 0000 0024 0208 0002 0000 0100 0000"            /* .....$.......... */
	$"0019 00AD 00"                                       /* ...≠. */
};

data 'DLOG' (3000, "General Error") {
	$"0034 0031 00C2 0188 0005 0000 0100 0000"            /* .4.1.¬.à........ */
	$"270C 010C 0545 7272 6F72 700A"                      /* '....Errorp¬ */
};

data 'DLOG' (3001, "Info") {
	$"0034 0031 00A7 0188 0005 0000 0100 0000"            /* .4.1.ß.à........ */
	$"270C 01C9 0449 6E66 6F8E 700A"                      /* '..….Infoép¬ */
};

data 'DLOG' (3002, "Save Or Not?") {
	$"0034 0031 00B4 0149 0005 0000 0100 0000"            /* .4.1.¥.I........ */
	$"270C 012C 0B53 6176 6520 4469 616C 6F67"            /* '..,.Save Dialog */
	$"700A"                                               /* p¬ */
};

data 'DLOG' (153, "Wave") {
	$"002C 0006 0134 01EC 0008 0000 0100 0000"            /* .,...4.Ï........ */
	$"001A 0099 0C50 6174 7465 726E 3A20 3030"            /* ...ô.Pattern: 00 */
	$"3030 280A"                                          /* 00(¬ */
};

data 'DLOG' (174, "Print Order Form") {
	$"003C 0055 0191 01A3 0005 0000 0100 0000"            /* .<.U.ë.£........ */
	$"0060 00AE 1050 7269 6E74 204F 7264 6572"            /* .`.Æ.Print Order */
	$"2046 6F72 6D0E 280A"                                /*  Form.(¬ */
};

data 'DLOG' (151, "OpenDialog2") {
	$"0052 002C 01A9 0183 0005 0000 0000 0000"            /* .R.,.©.É........ */
	$"0288 0085 0B4F 7065 6E20 4469 616C 6F67"            /* .à.Ö.Open Dialog */
};

data 'DLOG' (154, "About Plugs") {
	$"0062 0050 010F 01FB 0005 0100 0100 0001"            /* .b.P...˚........ */
	$"85DA 009A 0B41 626F 7574 2050 6C75 6773"            /* Ö⁄.ö.About Plugs */
};

data 'DLOG' (1004, "Save Dialog") {
	$"0046 0026 0167 017D 0005 0000 0000 0000"            /* .F.&.g.}........ */
	$"0057 0089 0B53 6176 6520 4469 616C 6F67"            /* .W.â.Save Dialog */
};

data 'DLOG' (3003, "File Not Found") {
	$"0034 0031 00B4 0149 0005 0000 0100 0000"            /* .4.1.¥.I........ */
	$"270C 012D 0E46 696C 6520 4E6F 7420 466F"            /* '..-.File Not Fo */
	$"756E 6405 700A"                                     /* und.p¬ */
};

data 'DLOG' (3004, "Music List Save Or Not?") {
	$"0034 0031 00B4 0149 0005 0000 0100 0000"            /* .4.1.¥.I........ */
	$"270C 0BBC 164D 7573 6963 204C 6973 7420"            /* '..º.Music List  */
	$"5361 7665 2044 6961 6C6F 67FF 700A"                 /* Save Dialogˇp¬ */
};

data 'DLOG' (149, "Minimize Results") {
	$"00C6 00BE 0139 0215 0005 0000 0100 0000"            /* .∆.æ.9.......... */
	$"0000 012E 104D 696E 696D 697A 6520 5265"            /* .....Minimize Re */
	$"7375 6C74 73"                                       /* sults */
};

data 'DLOG' (134, "Classic Editor") {
	$"002C 0006 01E8 03D0 0008 0000 0100 0000"            /* .,...Ë.–........ */
	$"001B 0086 0C50 6174 7465 726E 3A20 3030"            /* ...Ü.Pattern: 00 */
	$"3030 280A"                                          /* 00(¬ */
};

data 'DLOG' (2007, "Pref: CLASSIC PREFS") {
	$"002B 004B 014A 01D9 0000 0100 0100 0000"            /* .+.K.J.Ÿ........ */
	$"0000 07D8 00"                                       /* ...ÿ. */
};

data 'DLOG' (2008, "Pref: FKEY PREFS") {
	$"002B 004B 0148 01BB 0000 0100 0100 0000"            /* .+.K.H.ª........ */
	$"0000 07D9 00"                                       /* ...Ÿ. */
};

data 'DLOG' (2003, "Pref: COLOR") {
	$"002C 008C 0137 01ED 0001 0100 0100 0000"            /* .,.å.7.Ì........ */
	$"0011 07D3 00"                                       /* ...”. */
};

data 'DLOG' (133, "Preferences") {
	$"0032 003A 0166 019A 0005 0000 0100 0000"            /* .2.:.f.ö........ */
	$"0011 0084 0B50 7265 6665 7265 6E63 6573"            /* ...Ñ.Preferences */
	$"280A"                                               /* (¬ */
};

data 'DLOG' (177, "Copyright") {
	$"00B1 00F7 01A9 024B 0005 0100 0000 0000"            /* .±.˜.©.K........ */
	$"0000 00B1 2743 6F70 7972 6967 6874 2069"            /* ...±'Copyright i */
	$"6E66 6F72 6D61 7469 6F6E 7320 6162 6F75"            /* nformations abou */
	$"7420 7468 6973 206D 7573 6963 280A"                 /* t this music(¬ */
};

data 'DLOG' (175, "Registration") {
	$"0057 007F 017A 0200 0005 0000 0100 0000"            /* .W...z.......... */
	$"0000 00AF 0C52 6567 6973 7472 6174 696F"            /* ...Ø.Registratio */
	$"6E00 280A"                                          /* n.(¬ */
};

data 'DLOG' (2002, "Pref: MUSIC LIST") {
	$"0010 0025 0150 01FD 0001 0100 0100 0000"            /* ...%.P.˝........ */
	$"0011 07D2 0000 0000"                                /* ...“.... */
};

data 'DLOG' (131, "About") {
	$"0036 000A 011A 018B 0001 0100 0100 0000"            /* .6.¬...ã........ */
	$"0000 00C8 0000 280A"                                /* ...»..(¬ */
};

data 'DLOG' (2005, "Pref: CLASSICAL") {
	$"0039 0048 0258 02BC 0000 0100 0100 0000"            /* .9.H.X.º........ */
	$"0000 07D6 0000 0000"                                /* ...÷.... */
};

data 'DLOG' (165, "Microphone") {
	$"003F 0040 012A 016A 0005 0000 0100 0000"            /* .?.@.*.j........ */
	$"19F8 02F9 0B41 7564 696F 2049 6E50 7574"            /* .¯.˘.Audio InPut */
	$"05DA"                                               /* .⁄ */
};

data 'DLOG' (2000, "Pref: DRIVER") {
	$"002A 0028 015C 01AA 0001 0100 0100 0000"            /* .*.(.\.™........ */
	$"0011 07D0 0000 0000"                                /* ...–.... */
};

data 'DLOG' (178, "RAW Import") {
	$"0106 006E 021C 0203 0005 0000 0100 0000"            /* ...n............ */
	$"0000 00B2 0F52 4157 2044 6174 6120 496D"            /* ...≤.RAW Data Im */
	$"706F 7274 0000"                                     /* port.. */
};

data 'DLOG' (156, "ProgressWindow") {
	$"008E 008E 00F3 01BF 0004 0000 0100 0000"            /* .é.é.Û.ø........ */
	$"001C 00A3 1553 6F75 6E64 2044 6174 6120"            /* ...£.Sound Data  */
	$"4765 6E65 7261 7469 6E67 300A"                      /* Generating0¬ */
};

data 'DLOG' (136, "Windows State") {
	$"0048 006B 00B1 0196 0005 0000 0100 0000"            /* .H.k.±.ñ........ */
	$"0000 0088 0D57 696E 646F 7773 2053 7461"            /* ...à.Windows Sta */
	$"7465 020D"                                          /* te.. */
};

data 'DLOG' (138, "Reset Window") {
	$"0138 00ED 01B2 023C 0005 0000 0100 0000"            /* .8.Ì.≤.<........ */
	$"0000 008A 0552 6573 6574 0000"                      /* ...ä.Reset.. */
};

data 'DLOG' (139, "Editor") {
	$"002C 0006 01B8 041F 0008 0000 0100 0000"            /* .,...∏.......... */
	$"0002 008B 0C50 6174 7465 726E 3A20 3030"            /* ...ã.Pattern: 00 */
	$"3000 280A"                                          /* 0.(¬ */
};

data 'DLOG' (160, "Export AIFF/SND") {
	$"0108 00CA 01E1 0270 0005 0100 0100 0000"            /* ... .·.p........ */
	$"0000 013F 1253 6F75 6E64 2046 696C 6520"            /* ...?.Sound File  */
	$"5175 616C 6974 7900 280A"                           /* Quality.(¬ */
};

data 'DLOG' (2006, "Pref: DIGITAL EDITOR") {
	$"0031 0023 01F3 0253 0000 0100 0100 0000"            /* .1.#.Û.S........ */
	$"0000 07D6 0000 0000"                                /* ...÷.... */
};

data 'DLOG' (2009, "Pref: MIDI OMS") {
	$"00CE FD76 01B8 FED6 0000 0100 0100 0000"            /* .Œ˝v.∏˛÷........ */
	$"0000 07D9 0000 0000"                                /* ...Ÿ.... */
};

data 'DLOG' (2001, "Pref: PIANO") {
	$"0029 003D 0161 01A1 0001 0100 0100 0000"            /* .).=.a.°........ */
	$"0011 07D1 0000 0000"                                /* ...—.... */
};

data 'DLOG' (145, "Piano") {
	$"0094 00BA 01D7 0378 0008 0000 0100 0000"            /* .î.∫.◊.x........ */
	$"000A 0091 0550 6961 6E6F 3CB0"                      /* .¬.ë.Piano<∞ */
};

data 'DLOG' (166, "Partition") {
	$"0062 005E 0265 02BA 0008 0000 0100 0000"            /* .b.^.e.∫........ */
	$"0015 0330 0950 6172 7469 7469 6F6E DA70"            /* ...0∆Partition⁄p */
};

data 'DLOG' (180, "Quicktime Window") {
	$"016F 003A 0221 0169 0004 0000 0100 0000"            /* .o.:.!.i........ */
	$"001D 00B4 1051 7569 636B 7469 6D65 2057"            /* ...¥.Quicktime W */
	$"696E 646F 7700 0000"                                /* indow... */
};

data 'DLOG' (132, "PUB") {
	$"002D 0053 01D8 022C 0004 0100 0100 0000"            /* .-.S.ÿ.,........ */
	$"0018 0368 1648 6F77 2074 6F20 6F72 6465"            /* ...h.How to orde */
	$"7220 506C 6179 6572 5052 4F00 280A"                 /* r PlayerPRO.(¬ */
};

data 'DLOG' (181, "Bookmarks") {
	$"01B1 0095 0309 0231 0005 0000 0100 0000"            /* .±.ï.∆.1........ */
	$"0F2D 00B5 1441 6464 2F52 656D 6F76 6520"            /* .-.µ.Add/Remove  */
	$"426F 6F6B 6D61 726B 7300 0000"                      /* Bookmarks... */
};

data 'DLOG' (2004, "Pref: MISC") {
	$"002D 01AF 0162 03A7 0000 0100 0100 0000"            /* .-.Ø.b.ß........ */
	$"0000 07D4 0000 0000"                                /* ...‘.... */
};

data 'DLOG' (182, "Fine Speed/Pitch Settings") {
	$"0114 011A 015A 01F2 0005 0000 0100 0000"            /* .....Z.Ú........ */
	$"0000 00B6 1946 696E 6520 5370 6565 642F"            /* ...∂.Fine Speed/ */
	$"5069 7463 6820 5365 7474 696E 6773 0000"            /* Pitch Settings.. */
};

data 'DLOG' (130, "Info") {
	$"0030 000E 015C 01F9 0001 0100 0100 0000"            /* .0...\.˘........ */
	$"0000 01A5 0C49 6E66 6F72 6D61 7469 6F6E"            /* ...•.Information */
	$"7300 280A"                                          /* s.(¬ */
};

data 'DLOG' (140, "Instruments List") {
	$"0075 00C0 018F 025D 0008 0000 0100 0000"            /* .u.¿.è.]........ */
	$"0001 02AC 1049 6E73 7472 756D 656E 7473"            /* ...¨.Instruments */
	$"204C 6973 7400 0251"                                /*  List..Q */
};

data 'DLOG' (137, "Samples") {
	$"005E 0028 00F6 0240 0008 0000 0100 0000"            /* .^.(.ˆ.@........ */
	$"0009 00CA 0000 0000"                                /* .∆. .... */
};

data 'DLOG' (148, "General Informations") {
	$"0084 007C 0175 01DA 0005 0000 0000 0000"            /* .Ñ.|.u.⁄........ */
	$"2247 0094 1347 656E 6572 616C 2049 6E66"            /* "G.î.General Inf */
	$"6F72 6D61 7469 6F6E 0000"                           /* ormation.. */
};

data 'DLOG' (183, "Equalizer") {
	$"0052 00A1 0110 0184 0004 0000 0100 0000"            /* .R.°...Ñ........ */
	$"001E 00B7 0945 7175 616C 697A 6572 0000"            /* ...∑∆Equalizer.. */
};

data 'DLOG' (141, "Mixer") {
	$"003A 000F 0212 02C8 0008 0000 0100 0000"            /* .:.....»........ */
	$"0006 008D 054D 6978 6572 0000"                      /* ...ç.Mixer.. */
};

data 'DLOG' (184, "Hz Shift") {
	$"004F 00C2 0159 0222 0005 0000 0100 0000"            /* .O.¬.Y."........ */
	$"0000 00B8 0848 7A20 5368 6966 7400 0000"            /* ...∏.Hz Shift... */
};

data 'DLOG' (146, "EditInfo Instrument") {
	$"0095 0106 01E0 02CD 0005 0000 0100 0000"            /* .ï...‡.Õ........ */
	$"24FB 0092 0F49 6E73 7472 756D 656E 7420"            /* $˚.í.Instrument  */
	$"496E 666F 280A"                                     /* Info(¬ */
};

data 'DLOG' (143, "Tools") {
	$"00FC 01F8 019C 02B0 000C 0000 0100 0000"            /* .¸.¯.ú.∞........ */
	$"0008 008F 0554 6F6F 6C73 0000"                      /* ...è.Tools.. */
};

data 'DLOG' (142, "MusicList") {
	$"002E 0006 01E7 02D3 0008 0000 0100 0000"            /* .....Á.”........ */
	$"0007 008E 0A4D 7573 6963 204C 6973 7400"            /* ...é¬Music List. */
	$"0EEA"                                               /* .Í */
};

data 'DITL' (128, "The Player") {
	$"000C 0000 0000 0010 0000 0024 0014 8000"            /* ...........$..Ä. */
	$"0000 0000 0001 0190 000E 01BD 8000 0000"            /* .......ê...ΩÄ... */
	$"0000 0001 01BC 000E 0215 8000 0000 0000"            /* .....º....Ä..... */
	$"0001 0003 000E 0036 8808 4469 7370 6C61"            /* .......6à.Displa */
	$"793A 0000 0000 0001 0095 000E 00A9 4002"            /* y:.......ï...©@. */
	$"0384 0000 0000 0001 0037 000E 0093 8800"            /* .Ñ.......7...ìà. */
	$"0000 0000 0001 00B4 000E 00D5 8805 5369"            /* .......¥...’à.Si */
	$"7A65 3A02 0000 0000 0001 00F3 000E 0107"            /* ze:........Û.... */
	$"4002 0384 0000 0000 0001 00D6 000E 00F2"            /* @..Ñ.......÷...Ú */
	$"8800 0000 0000 0010 0000 07E0 07D0 0000"            /* à..........‡.–.. */
	$"0000 0000 0001 0114 000E 0135 8805 4D6F"            /* ...........5à.Mo */
	$"6465 3A00 0000 0000 0001 0159 000E 016D"            /* de:........Y...m */
	$"4002 0384 0000 0000 0001 0136 000E 0158"            /* @..Ñ.......6...X */
	$"8800"                                               /* à. */
};

data 'DITL' (1002, purgeable) {
	$"000A 0000 0000 0093 012C 00A7 017C 0404"            /* .¬.....ì.,.ß.|.. */
	$"4F70 656E 0000 0000 0000 02BB 0050 02CD"            /* Open.......ª.P.Õ */
	$"0406 4869 6464 656E 0000 0000 00AF 012C"            /* ..Hidden.....Ø., */
	$"00C3 017C 0406 4361 6E63 656C 0000 0000"            /* .√.|..Cancel.... */
	$"0032 0111 0046 0184 8000 0000 0000 004D"            /* .2...F.ÑÄ......M */
	$"012C 0061 017C 0405 456A 6563 746F 0000"            /* .,.a.|..Ejecto.. */
	$"0000 0066 012C 007A 017C 0407 4465 736B"            /* ...f.,.z.|..Desk */
	$"746F 7032 0000 0000 0032 000A 0122 010F"            /* top2.....2.¬.".. */
	$"0000 0000 0000 0032 010E 0122 011F 0000"            /* .......2...".... */
	$"0000 0000 0085 0128 0086 0180 8000 0000"            /* .....Ö.(.Ü.ÄÄ... */
	$"0000 0000 0294 0065 02F4 8800 0000 0000"            /* .....î.e.Ùà..... */
	$"000A 000A 001A 0140 8825 506C 6561 7365"            /* .¬.¬...@à%Please */
	$"2C20 6368 6F6F 7365 2061 2073 6F75 6E64"            /* , choose a sound */
	$"2066 696C 6520 746F 206F 7065 6E20 3A00"            /*  file to open :. */
};

data 'DITL' (268) {
	$"0007 0000 0000 0067 010E 007B 0148 0402"            /* .......g...{.H.. */
	$"4F4B 0000 0000 005A 000A 006A 0070 880D"            /* OK.....Z.¬.j.pà. */
	$"4572 726F 7220 4944 203A 205E 3100 0000"            /* Error ID : ^1... */
	$"0000 000A 003C 0055 014B 8802 5E30 0000"            /* ...¬.<.U.Kà.^0.. */
	$"0000 000A 000A 002A 002A C002 00DC 0000"            /* ...¬.¬.*.*¿..‹.. */
	$"0000 002E 000C 003E 002B 8800 0000 0000"            /* .......>.+à..... */
	$"003F 000C 004F 002B 8800 0000 0000 006B"            /* .?...O.+à......k */
	$"000A 007B 0070 880B 4D65 6D45 7272 203A"            /* .¬.{.pà.MemErr : */
	$"205E 3200 0000 0000 007C 0013 008C 0070"            /*  ^2......|...å.p */
	$"880B 5265 7345 7272 203A 205E 3300"                 /* à.ResErr : ^3. */
};

data 'DITL' (144, "Inst. List") {
	$"0007 0000 0000 0025 0000 07F5 00C8 0000"            /* .......%...ı.».. */
	$"0000 0000 0019 0000 0024 00DC 8813 2049"            /* .........$.‹à. I */
	$"4420 2020 2053 697A 6520 2020 204E 616D"            /* D    Size    Nam */
	$"6500 0000 0000 0002 0029 0016 003D 0000"            /* e........)...=.. */
	$"0000 0000 0002 0047 0016 005B 0000 0000"            /* .......G...[.... */
	$"0000 0002 0065 0016 0079 0000 0000 0000"            /* .....e...y...... */
	$"0002 0083 0016 0097 0000 0000 0000 0002"            /* ...É...ó........ */
	$"000B 0016 001F 0000 0000 0000 0002 00A1"            /* ...............° */
	$"0016 00B5 0000"                                     /* ...µ.. */
};

data 'DITL' (457) {
	$"0003 0000 0000 0051 0114 0065 014E 0402"            /* .......Q...e.N.. */
	$"4F4B 0000 0000 0051 00CC 0065 0106 0406"            /* OK.....Q.Ã.e.... */
	$"4361 6E63 656C 0000 0000 000A 003C 004C"            /* Cancel.....¬.<.L */
	$"014C 8802 5E30 0000 0000 000A 000A 002A"            /* .Là.^0.....¬.¬.* */
	$"002A C002 00DC"                                     /* .*¿..‹ */
};

data 'DITL' (147) {
	$"000E 0000 0000 001D 0000 07ED 07D0 0000"            /* ...........Ì.–.. */
	$"0000 0000 0002 0050 000E 0072 8805 5A6F"            /* .......P...rà.Zo */
	$"6F6D 3A00 0000 0000 000E 0050 001B 0071"            /* om:........P...q */
	$"8805 5369 7A65 3A00 0000 0000 0002 00C6"            /* à.Size:........∆ */
	$"0010 00EE 8806 5472 6163 6B3A 0000 0000"            /* ...Óà.Track:.... */
	$"0002 011D 0010 0174 8800 0000 0000 000F"            /* .......tà....... */
	$"00F0 001B 0176 8800 0000 0000 0001 0178"            /* ....và........x */
	$"000E 018C 4002 00C8 0000 0000 000E 0178"            /* ...å@..».......x */
	$"001B 018C 4002 00C8 0000 0000 0002 0071"            /* ...å@..».......q */
	$"000E 00C3 8800 0000 0000 000E 0071 001B"            /* ...√à........q.. */
	$"00A3 8800 0000 0000 000F 00A8 001B 00EE"            /* .£à........®...Ó */
	$"880B 496E 7374 7275 6D65 6E74 3A00 0000"            /* à.Instrument:... */
	$"0000 0004 0005 0018 0019 0000 0000 0000"            /* ................ */
	$"0002 00F3 000C 0119 8000 0000 0000 0004"            /* ...Û....Ä....... */
	$"001E 0018 0032 0000 0000 0000 0004 0037"            /* .....2.........7 */
	$"0018 004B 0000"                                     /* ...K.. */
};

data 'DITL' (300, purgeable) {
	$"0004 0000 0000 003E 0009 0052 004F 0403"            /* .......>.∆.R.O.. */
	$"5965 7300 0000 0000 005C 0009 0070 004F"            /* Yes......\.∆.p.O */
	$"0402 4E6F 0000 0000 005C 00C8 0070 010E"            /* ..No.....\.».p.. */
	$"0406 4361 6E63 656C 0000 0000 0006 003C"            /* ..Cancel.......< */
	$"0036 010E 8819 5361 7665 20D2 5E30 D320"            /* .6..à.Save “^0”  */
	$"6265 666F 7265 2063 6C6F 7369 6E67 3F00"            /* before closing?. */
	$"0000 0000 000A 000A 002A 002A C002 00DC"            /* .....¬.¬.*.*¿..‹ */
};

data 'DITL' (129) {
	$"0009 0000 0000 0087 00FC 009B 014C 0404"            /* .∆.....á.¸.õ.L.. */
	$"4F70 656E 0000 0000 0068 00FC 007C 014C"            /* Open.....h.¸.|.L */
	$"0406 4361 6E63 656C 0000 0000 0000 0000"            /* ..Cancel........ */
	$"0000 0000 8000 0000 0000 0008 00EB 0018"            /* ....Ä........Î.. */
	$"0151 0000 0000 0000 0020 00FC 0034 014C"            /* .Q....... .¸.4.L */
	$"0405 456A 6563 7400 0000 0000 003C 00FC"            /* ..Eject......<.¸ */
	$"0050 014C 0407 4465 736B 746F 7000 0000"            /* .P.L..Desktop... */
	$"0000 001D 000C 00FF 00E6 0000 0000 0000"            /* .......ˇ.Ê...... */
	$"0006 000C 0019 00E6 0000 0000 0000 005B"            /* .......Ê.......[ */
	$"00FB 005C 014D C002 000B 0000 0000 00A9"            /* .˚.\.M¿........© */
	$"00FA 00FF 0150 C002 0085"                           /* .˙.ˇ.P¿..Ö */
};

data 'DITL' (131, purgeable) {
	$"000C 0000 0000 00A1 00FC 00B5 014C 0404"            /* .......°.¸.µ.L.. */
	$"5361 7665 0000 0000 0082 00FC 0096 014C"            /* Save.....Ç.¸.ñ.L */
	$"0406 4361 6E63 656C 0000 0000 0000 0000"            /* ..Cancel........ */
	$"0000 0000 8000 0000 0000 0008 00EB 0018"            /* ....Ä........Î.. */
	$"0151 0000 0000 0000 0020 00FC 0034 014C"            /* .Q....... .¸.4.L */
	$"0405 456A 6563 7400 0000 0000 003C 00FC"            /* ..Eject......<.¸ */
	$"0050 014C 0407 4465 736B 746F 7000 0000"            /* .P.L..Desktop... */
	$"0000 001D 000C 00EA 00E6 0000 0000 0000"            /* .......Í.Ê...... */
	$"0007 000C 001A 00E6 0000 0000 0000 0077"            /* .......Ê.......w */
	$"00FA 0078 014E C002 000B 0000 0000 0106"            /* .˙.x.N¿......... */
	$"0010 0116 00E4 1000 0000 0000 00F1 0010"            /* .....‰.......Ò.. */
	$"0101 00B6 8811 5361 7665 2064 6F63 756D"            /* ...∂à.Save docum */
	$"656E 7420 6173 3A00 0000 0000 0058 00FC"            /* ent as:......X.¸ */
	$"006C 014C 8000 0000 0000 00C3 00FA 0119"            /* .l.LÄ......√.˙.. */
	$"0150 C002 0085"                                     /* .P¿..Ö */
};

data 'DITL' (150) {
	$"0001 0000 0000 0009 0048 0019 00C8 900B"            /* .......∆.H...»ê. */
	$"496E 7374 7275 6D65 6E74 2000 0000 0000"            /* Instrument ..... */
	$"000A 000A 001A 0038 8805 4E61 6D65 3A00"            /* .¬.¬...8à.Name:. */
};

data 'DITL' (193) {
	$"0002 0000 0000 0010 0063 0085 0170 8000"            /* .........c.Ö.pÄ. */
	$"0000 0000 0008 0007 0140 0017 0702 0099"            /* .........@.....ô */
	$"0000 0000 0077 0063 012C 13EB 0000"                 /* .....w.c.,.Î.. */
};

data 'DITL' (133) {
	$"0013 0000 0000 0087 00FC 009B 014C 0404"            /* .......á.¸.õ.L.. */
	$"4F70 656E 0000 0000 0068 00FC 007C 014C"            /* Open.....h.¸.|.L */
	$"0406 4361 6E63 656C 0000 0000 0000 0000"            /* ..Cancel........ */
	$"0000 0000 8000 0000 0000 0008 00EB 0018"            /* ....Ä........Î.. */
	$"0151 0000 0000 0000 0020 00FC 0034 014C"            /* .Q....... .¸.4.L */
	$"0405 456A 6563 7400 0000 0000 003C 00FC"            /* ..Eject......<.¸ */
	$"0050 014C 0407 4465 736B 746F 7000 0000"            /* .P.L..Desktop... */
	$"0000 001D 000C 00FF 00E6 0000 0000 0000"            /* .......ˇ.Ê...... */
	$"0006 000C 0019 00E6 0000 0000 0000 005B"            /* .......Ê.......[ */
	$"00FB 005C 014D C002 000B 0000 0000 00AF"            /* .˚.\.M¿........Ø */
	$"00FC 00C3 014C 0403 4164 6400 0000 0000"            /* .¸.√.L..Add..... */
	$"00FF 0112 010F 0137 0800 0000 0000 00FF"            /* .ˇ.....7.......ˇ */
	$"000C 0110 0038 0806 5368 6F77 3A20 0000"            /* .....8..Show: .. */
	$"0000 0115 006B 0126 00EB 8813 4176 6169"            /* .....k.&.Îà.Avai */
	$"6C61 626C 6520 666F 726D 6174 733A 2012"            /* lable formats: . */
	$"0000 0000 0126 000C 0156 0150 8800 0000"            /* .....&...V.Pà... */
	$"0000 0112 FFFF 0113 015F C002 000B 0000"            /* ....ˇˇ..._¿..... */
	$"0000 00CB 00FC 00DF 014C 0407 4164 6420"            /* ...À.¸.ﬂ.L..Add  */
	$"416C 6C65 0000 0000 00E7 00FC 00FB 014C"            /* Alle.....Á.¸.˚.L */
	$"0407 5072 6576 6965 77F0 0000 0000 00A4"            /* ..Preview.....§ */
	$"00FB 00A5 014D C002 000B 0000 0000 00FF"            /* .˚.•.M¿........ˇ */
	$"0053 010F 00E4 0800 0000 0000 0101 003A"            /* .S...‰.........: */
	$"010E 004E 4002 0385"                                /* ...N@..Ö */
};

data 'DITL' (152) {
	$"002E 0000 0000 0086 00EC 009A 013B 0404"            /* .......Ü.Ï.ö.;.. */
	$"4669 6E64 0000 0000 0086 0156 009A 01A5"            /* Find.....Ü.V.ö.• */
	$"0404 446F 6E65 0000 0000 0011 0003 0023"            /* ..Done.........# */
	$"0062 050A 496E 7374 7275 6D65 6E74 0000"            /* .b.¬Instrument.. */
	$"0000 0025 0003 0037 005C 0504 4E6F 7465"            /* ...%...7.\..Note */
	$"0000 0000 0039 0003 004B 0052 0506 4566"            /* .....9...K.R..Ef */
	$"6665 6374 0000 0000 004C 0003 005E 005E"            /* fect.....L...^.^ */
	$"0508 4172 6775 6D65 6E74 0000 0000 0000"            /* ..Argument...... */
	$"0034 0010 0059 8805 4669 6E64 3A63 0000"            /* .4...Yà.Find:c.. */
	$"0000 0000 00BD 0010 0101 8808 5265 706C"            /* .....Ω....à.Repl */
	$"6163 653A 0000 0000 0000 0152 0010 018E"            /* ace:.......R...é */
	$"8806 5768 6572 653A 0000 0000 0012 0130"            /* à.Where:.......0 */
	$"0024 01AC 060F 4375 7272 656E 7420 7061"            /* .$.¨..Current pa */
	$"7474 6572 6E74 0000 0000 0026 0130 0038"            /* tternt.....&.0.8 */
	$"01C0 0613 5365 6C65 6374 696F 6E20 2844"            /* .¿..Selection (D */
	$"6967 6974 616C 2977 0000 0000 003A 0130"            /* igital)w.....:.0 */
	$"004C 019A 060A 4576 6572 7977 6865 7265"            /* .L.ö.¬Everywhere */
	$"0000 0000 0032 0212 0044 02A9 060C 5261"            /* .....2...D.©..Ra */
	$"6469 6F20 4275 7474 6F6E 0000 0000 0014"            /* dio Button...... */
	$"007E 0021 0092 4002 0384 0000 0000 0013"            /* .~.!.í@..Ñ...... */
	$"0061 0023 007D 8803 3838 3800 0000 0000"            /* .a.#.}à.888..... */
	$"0028 007E 0035 0092 4002 0384 0000 0000"            /* .(.~.5.í@..Ñ.... */
	$"0027 0061 0037 007D 8803 3838 3800 0000"            /* .'.a.7.}à.888... */
	$"0000 004E 0064 005D 0076 1002 3030 0000"            /* ...N.d.].v..00.. */
	$"0000 003C 007E 0049 0092 4002 0384 0000"            /* ...<.~.I.í@..Ñ.. */
	$"0000 003B 0064 004A 007A 8802 3939 0000"            /* ...;.d.J.zà.99.. */
	$"0000 0010 0099 0022 00F8 050A 496E 7374"            /* .....ô.".¯.¬Inst */
	$"7275 6D65 6E74 0000 0000 0024 0099 0036"            /* rument.....$.ô.6 */
	$"00F2 0504 4E6F 7465 0000 0000 0038 0099"            /* .Ú..Note.....8.ô */
	$"004A 00E4 0506 4566 6665 6374 0000 0000"            /* .J.‰..Effect.... */
	$"004C 0099 005E 00F4 0508 4172 6775 6D65"            /* .L.ô.^.Ù..Argume */
	$"6E74 0000 0000 0014 0114 0021 0128 4002"            /* nt.........!.(@. */
	$"0384 0000 0000 0012 00F7 0022 0113 8803"            /* .Ñ.......˜."..à. */
	$"3838 3800 0000 0000 0028 0114 0035 0128"            /* 888......(...5.( */
	$"4002 0384 0000 0000 0026 00F7 0036 0113"            /* @..Ñ.....&.˜.6.. */
	$"8803 3939 3900 0000 0000 004E 00FA 005D"            /* à.999......N.˙.] */
	$"010C 1002 3939 0000 0000 003C 0114 0049"            /* ....99.....<...I */
	$"0128 4002 0384 0000 0000 0039 00FA 0049"            /* .(@..Ñ.....9.˙.I */
	$"0110 8802 3939 0000 0000 FFF6 0095 0078"            /* ..à.99....ˇˆ.ï.x */
	$"0096 C002 000B 0000 0000 FFF6 012B 0078"            /* .ñ¿.......ˇˆ.+.x */
	$"012C C002 000B 0000 0000 00D7 0047 00E1"            /* .,¿........◊.G.· */
	$"0051 8000 0000 0000 0079 0000 007A 0320"            /* .QÄ......y...z.  */
	$"C002 000B 0000 0000 00B9 010A 00CD 0173"            /* ¿........π.¬.Õ.s */
	$"040E 5265 706C 6163 6520 2620 4669 6E64"            /* ..Replace & Find */
	$"0000 0000 0086 007F 009A 00CE 0407 5265"            /* .....Ü...ö.Œ..Re */
	$"706C 6163 656E 0000 0000 0086 0011 009A"            /* placen.....Ü...ö */
	$"0060 040B 5265 706C 6163 6520 416C 6C6E"            /* .`..Replace Alln */
	$"0000 0000 0062 0003 0074 005E 0506 566F"            /* .....b...t.^..Vo */
	$"6C75 6D65 0000 0000 0064 0064 0073 0076"            /* lume.....d.d.s.v */
	$"1002 3030 0000 0000 0062 0099 0074 00F4"            /* ..00.....b.ô.t.Ù */
	$"0506 566F 6C75 6D65 0000 0000 0064 00FA"            /* ..Volume.....d.˙ */
	$"0073 010C 1002 3030 0000 0000 0064 0130"            /* .s....00.....d.0 */
	$"0074 0181 0609 4F6E 2074 7261 636B 3A00"            /* .t.Å.∆On track:. */
	$"0000 0000 0065 0196 0072 01AA 4002 0384"            /* .....e.ñ.r.™@..Ñ */
	$"0000 0000 0064 0182 0074 0195 8801 3100"            /* .....d.Ç.t.ïà.1. */
	$"0000 0000 0052 0130 0062 0181 060A 416C"            /* .....R.0.b.Å.¬Al */
	$"6C20 7472 6163 6B73 0000 0000 004E 012C"            /* l tracks.....N., */
	$"004F 044C C002 000B"                                /* .O.L¿... */
};

data 'DITL' (2003) {
	$"0003 0000 0000 0039 0004 008C 015F 0000"            /* .......9...å._.. */
	$"0000 0000 0026 0003 0036 009E 880D 5472"            /* .....&...6.ûà.Tr */
	$"6163 6B73 2063 6F6C 6F72 3A00 0000 0000"            /* acks color:..... */
	$"0098 0003 00A8 00C6 8815 436C 6963 6B20"            /* .ò...®.∆à.Click  */
	$"746F 2063 6861 6E67 6520 636F 6C6F 7200"            /* to change color. */
	$"0000 0000 001E 00B4 0039 014C 8000"                 /* .......¥.9.LÄ. */
};

data 'DITL' (155) {
	$"0000 0000 0000 FFFF FFFF 007D 01E5 0000"            /* ......ˇˇˇˇ.}.Â.. */
};

data 'DITL' (140) {
	$"0009 0000 0000 0087 00FC 009B 014C 0404"            /* .∆.....á.¸.õ.L.. */
	$"4F70 656E 0000 0000 0068 00FC 007C 014C"            /* Open.....h.¸.|.L */
	$"0406 4361 6E63 656C 0000 0000 0000 0000"            /* ..Cancel........ */
	$"0000 0000 8000 0000 0000 0008 00EB 0018"            /* ....Ä........Î.. */
	$"0151 0000 0000 0000 0020 00FC 0034 014C"            /* .Q....... .¸.4.L */
	$"0405 456A 6563 7400 0000 0000 003C 00FC"            /* ..Eject......<.¸ */
	$"0050 014C 0407 4465 736B 746F 7000 0000"            /* .P.L..Desktop... */
	$"0000 001D 000C 009F 00E6 0000 0000 0000"            /* .......ü.Ê...... */
	$"0006 000C 0019 00E6 0000 0000 0000 005B"            /* .......Ê.......[ */
	$"00FB 005C 014D C002 000B 0000 0000 00A4"            /* .˚.\.M¿........§ */
	$"000C 00B7 00C6 0702 008D"                           /* ...∑.∆...ç */
};

data 'DITL' (151, purgeable) {
	$"000C 0000 0000 00A1 00FC 00B5 014C 0404"            /* .......°.¸.µ.L.. */
	$"5361 7665 0000 0000 0082 00FC 0096 014C"            /* Save.....Ç.¸.ñ.L */
	$"0406 4361 6E63 656C 0000 0000 0000 0000"            /* ..Cancel........ */
	$"0000 0000 8000 0000 0000 0008 00EB 0018"            /* ....Ä........Î.. */
	$"0151 0000 0000 0000 0020 00FC 0034 014C"            /* .Q....... .¸.4.L */
	$"0405 456A 6563 7400 0000 0000 003C 00FC"            /* ..Eject......<.¸ */
	$"0050 014C 0407 4465 736B 746F 7000 0000"            /* .P.L..Desktop... */
	$"0000 001D 000C 00B6 00E6 0000 0000 0000"            /* .......∂.Ê...... */
	$"0007 000C 001A 00E6 0000 0000 0000 0077"            /* .......Ê.......w */
	$"00FA 0078 014E C002 000B 0000 0000 00D1"            /* .˙.x.N¿........— */
	$"000E 00E1 00E2 1000 0000 0000 00BC 000E"            /* ...·.‚.......º.. */
	$"00CC 00E2 8809 4669 6C65 6E61 6D65 3A00"            /* .Ã.‚à∆Filename:. */
	$"0000 0000 0058 00FC 006C 014C 8000 0000"            /* .....X.¸.l.LÄ... */
	$"0000 00EB 000B 00FE 00F7 0702 008D"                 /* ...Î...˛.˜...ç */
};

data 'DITL' (156, "Music list") {
	$"0010 0000 0000 0000 0000 00C4 00C4 0000"            /* ...........ƒ.ƒ.. */
	$"0000 0000 0005 00CD 0012 0118 8805 526F"            /* .......Õ....à.Ro */
	$"7773 3A00 0000 0000 0014 00CD 0021 0137"            /* ws:........Õ.!.7 */
	$"8808 436F 6C75 6D6E 733A 0000 0000 0050"            /* à.Columns:.....P */
	$"00CD 005D 014F 8815 536D 616C 6C65 7374"            /* .Õ.].Oà.Smallest */
	$"2050 6978 656C 2056 616C 7565 3A00 0000"            /*  Pixel Value:... */
	$"0000 0023 00CD 0030 0118 8806 4465 7074"            /* ...#.Õ.0..à.Dept */
	$"683A 0000 0000 005F 00CD 006C 0150 880E"            /* h:....._.Õ.l.Pà. */
	$"5769 6E64 6F77 2043 656E 7465 723A 0000"            /* Window Center:.. */
	$"0000 0041 00CD 004E 014E 8814 4C61 7267"            /* ...A.Õ.N.Nà.Larg */
	$"6573 7420 5069 7865 6C20 5661 6C75 653A"            /* est Pixel Value: */
	$"0000 0000 0032 00CD 003F 0147 8810 5369"            /* .....2.Õ.?.Gà.Si */
	$"7A65 2028 696E 2062 7974 6573 293A 0000"            /* ze (in bytes):.. */
	$"0000 0005 0151 0012 01A1 8800 0000 0000"            /* .....Q...°à..... */
	$"0014 0151 0021 01A1 0800 0000 0000 0050"            /* ...Q.!.°.......P */
	$"0151 005D 01A1 8800 0000 0000 0032 0151"            /* .Q.].°à......2.Q */
	$"003F 01A1 8800 0000 0000 005F 0151 006C"            /* .?.°à......_.Q.l */
	$"01A1 8800 0000 0000 0041 0151 004E 01A1"            /* .°à......A.Q.N.° */
	$"8800 0000 0000 0023 0151 0030 01A1 8800"            /* à......#.Q.0.°à. */
	$"0000 0000 006E 00CD 007B 014E 880D 5769"            /* .....n.Õ.{.Nà.Wi */
	$"6E64 6F77 2057 6964 7468 3A00 0000 0000"            /* ndow Width:..... */
	$"006E 0151 007A 01A0 8800"                           /* .n.Q.z.†à. */
};

data 'DITL' (157) {
	$"0001 0000 0000 FF9E FF6E 002B 0014 8702"            /* ......ˇûˇn.+..á. */
	$"0080 0000 0000 FFC2 FFB1 0017 0012 8702"            /* .Ä....ˇ¬ˇ±....á. */
	$"0081"                                               /* .Å */
};

data 'DITL' (158) {
	$"0009 0000 0000 0007 0043 000F 0075 0000"            /* .∆.......C...u.. */
	$"0000 0000 0022 0100 002F 0126 8000 0000"            /* .....".../.&Ä... */
	$"0000 0038 00EC 0044 013E 8000 0000 0000"            /* ...8.Ï.D.>Ä..... */
	$"0016 0000 0036 0020 8000 0000 0000 0000"            /* .....6. Ä....... */
	$"0000 0020 0020 0702 0089 0000 0000 0000"            /* ... . ...â...... */
	$"0023 0020 0043 0702 0084 0000 0000 0002"            /* .#. .C...Ñ...... */
	$"00E9 001E 010A 8000 0000 0000 0002 010E"            /* .È...¬Ä......... */
	$"001E 012F 8000 0000 0000 0000 0046 0020"            /* .../Ä........F.  */
	$"0066 0000 0000 0000 0004 00BF 001A 0126"            /* .f.........ø...& */
	$"8000"                                               /* Ä. */
};

data 'DITL' (159) {
	$"0003 0000 0000 0050 010E 0064 0148 0402"            /* .......P...d.H.. */
	$"4F4B 0000 0000 005A 000A 006A 00A3 880C"            /* OK.....Z.¬.j.£à. */
	$"4572 726F 7220 4944 3A20 5E31 0000 0000"            /* Error ID: ^1.... */
	$"000A 003C 004C 014C 8802 5E30 0000 0000"            /* .¬.<.L.Là.^0.... */
	$"000A 000A 002A 002A C002 00DC"                      /* .¬.¬.*.*¿..‹ */
};

data 'DITL' (160) {
	$"0003 0000 0000 0051 0114 0065 014E 0402"            /* .......Q...e.N.. */
	$"4F4B 0000 0000 0051 00CC 0065 0106 0406"            /* OK.....Q.Ã.e.... */
	$"4361 6E63 656C 0000 0000 000A 003C 004C"            /* Cancel.....¬.<.L */
	$"014C 8802 5E30 0000 0000 000A 000A 002A"            /* .Là.^0.....¬.¬.* */
	$"002A C002 00DC"                                     /* .*¿..‹ */
};

data 'DITL' (162) {
	$"0001 0000 0000 0018 FFFF 07E8 00C7 8000"            /* ........ˇˇ.Ë.«Ä. */
	$"0000 0000 0003 0005 0013 00BE 0702 0082"            /* ...........æ...Ç */
};

data 'DITL' (167, "OSIRIS StdFile Open", purgeable) {
	$"0009 0000 0000 007B 011B 008D 016B 0402"            /* .∆.....{...ç.k.. */
	$"4F4B 0000 0000 0094 011B 00A6 016B 0406"            /* OK.....î...¶.k.. */
	$"4361 6E63 656C 0000 0000 0000 0000 0000"            /* Cancel.......... */
	$"0000 8000 0000 0000 0010 0110 0020 0176"            /* ..Ä.......... .v */
	$"8000 0000 0000 0035 011B 0047 016B 0405"            /* Ä......5...G.k.. */
	$"456A 6563 7400 0000 0000 004E 011B 0060"            /* Eject......N...` */
	$"016B 0407 4465 736B 746F 7000 0000 0000"            /* .k..Desktop..... */
	$"0027 000C 0128 0103 0000 0000 0000 000E"            /* .'...(.......... */
	$"000C 0021 0103 0000 0000 0000 006D 0117"            /* ...!.........m.. */
	$"006E 016F 8000 0000 0000 0414 0014 0479"            /* .n.oÄ..........y */
	$"0074 8800"                                          /* .tà. */
};

data 'DITL' (168) {
	$"000A 0000 0000 00A1 0117 00B5 0167 0404"            /* .¬.....°...µ.g.. */
	$"5361 7665 0000 0000 0082 0117 0096 0167"            /* Save.....Ç...ñ.g */
	$"0406 4361 6E63 656C 0000 0000 0008 010C"            /* ..Cancel........ */
	$"0018 0172 0000 0000 0000 0020 0117 0034"            /* ...r....... ...4 */
	$"0167 0405 456A 6563 7400 0000 0000 003C"            /* .g..Eject......< */
	$"0117 0050 0167 0407 4465 736B 746F 7000"            /* ...P.g..Desktop. */
	$"0000 0000 001D 000C 011E 0103 0000 0000"            /* ................ */
	$"0000 0007 000C 001A 0103 0000 0000 0000"            /* ................ */
	$"0077 0115 0078 0169 C002 000B 0000 0000"            /* .w...x.i¿....... */
	$"013A 001B 014A 00EF 1000 0000 0000 0125"            /* .:...J.Ô.......% */
	$"001B 0135 00EF 8809 4669 6C65 6E61 6D65"            /* ...5.Ôà∆Filename */
	$"3A00 0000 0000 0058 0117 006C 0167 8000"            /* :......X...l.gÄ. */
};

data 'DITL' (169) {
	$"000C 0000 0000 0000 0052 0011 00AB 8000"            /* .........R...´Ä. */
	$"0000 0000 00CB 001A 00DE 00E2 8000 0000"            /* .....À...ﬁ.‚Ä... */
	$"0000 00DD 0001 00F0 006B 8000 0000 0000"            /* ...›....kÄ..... */
	$"00DD 0090 00F0 00FA 8000 0000 0000 0000"            /* .›.ê..˙Ä....... */
	$"0000 0064 00FA C002 0082 0000 0000 0045"            /* ...d.˙¿..Ç.....E */
	$"0000 00C5 0100 C002 0083 0000 0000 0048"            /* ...≈..¿..É.....H */
	$"0000 0057 00F3 8000 0000 0000 005A 0000"            /* ...W.ÛÄ......Z.. */
	$"0069 00F3 8000 0000 0000 006C 0000 007B"            /* .i.ÛÄ......l...{ */
	$"00F3 8000 0000 0000 007E 0000 008D 00F3"            /* .ÛÄ......~...ç.Û */
	$"8000 0000 0000 0090 0000 009F 00F3 8000"            /* Ä......ê...ü.ÛÄ. */
	$"0000 0000 00A2 0000 00B1 00F3 8000 0000"            /* .....¢...±.ÛÄ... */
	$"0000 00B4 0000 00C3 00F3 8000"                      /* ...¥...√.ÛÄ. */
};

data 'DITL' (170) {
	$"0003 0000 0000 0003 0003 0043 00D1 8000"            /* ...........C.—Ä. */
	$"0000 0000 0046 0003 0086 00D1 8000 0000"            /* .....F...Ü.—Ä... */
	$"0000 0003 0002 0043 000F 8000 0000 0000"            /* .......C..Ä..... */
	$"0046 0002 0086 000F 8000"                           /* .F...Ü..Ä. */
};

data 'DITL' (623) {
	$"0008 0000 0000 0008 00FC 001C 0136 0405"            /* .........¸...6.. */
	$"5374 6172 7400 0000 0000 0028 00FC 003C"            /* Start......(.¸.< */
	$"0136 0403 4279 6500 0000 0000 0008 000C"            /* .6..Bye......... */
	$"0018 007E 8812 5265 7375 6C74 2028 696E"            /* ...~à.Result (in */
	$"2074 6963 6B73 293A 0000 0000 0008 0084"            /*  ticks):.......Ñ */
	$"0018 00CF 8800 0000 0000 0020 000C 0030"            /* ...œà...... ...0 */
	$"007E 880F 5072 6F63 6573 736F 7220 5469"            /* .~à.Processor Ti */
	$"6D65 3A00 0000 0000 0020 0084 0030 00CF"            /* me:...... .Ñ.0.œ */
	$"8800 0000 0000 0046 000A 005A 0058 0408"            /* à......F.¬.Z.X.. */
	$"4465 6275 6720 4F4E 0000 0000 0046 0064"            /* Debug ON.....F.d */
	$"005A 00B2 0409 4465 6275 6720 4F46 4600"            /* .Z.≤.∆Debug OFF. */
	$"0000 0000 0045 00BC 0059 012A 040C 4D65"            /* .....E.º.Y.*..Me */
	$"6D6F 7279 2050 7572 6765"                           /* mory Purge */
};

data 'DITL' (364) {
	$"0012 0000 0000 0002 0002 0012 012D 8000"            /* .............-Ä. */
	$"0000 0000 0013 0002 0020 004A 880C 5573"            /* ......... .Jà.Us */
	$"6564 206D 656D 6F72 793A 0000 0000 0013"            /* ed memory:...... */
	$"004C 0020 007C 8807 3133 3030 204B 6200"            /* .L. .|à.1300 Kb. */
	$"0000 0000 002E 00D0 003B 00DD 8000 0000"            /* .......–.;.›Ä... */
	$"0000 0013 00A9 0020 00F4 880C 4672 6565"            /* .....©. .Ùà.Free */
	$"206D 656D 6F72 793A 0000 0000 0013 00F5"            /*  memory:.......ı */
	$"0020 012D 8800 0000 0000 0034 00D7 0041"            /* . .-à......4.◊.A */
	$"00E4 8000 0000 0000 0021 001F 002E 0065"            /* .‰Ä......!.....e */
	$"880D 536F 756E 6420 4472 6976 6572 3A00"            /* à.Sound Driver:. */
	$"0000 0000 0021 0067 002E 0097 8800 0000"            /* .....!.g...óà... */
	$"0000 0021 0002 002E 001E 8803 3230 2500"            /* ...!......à.20%. */
	$"0000 0000 002F 0024 003C 0065 880C 496E"            /* ...../.$.<.eà.In */
	$"7374 7275 6D65 6E74 733A 0000 0000 002F"            /* struments:...../ */
	$"0067 003C 0097 8800 0000 0000 002F 0002"            /* .g.<.óà....../.. */
	$"003C 001E 8803 3230 2500 0000 0000 003D"            /* .<..à.20%......= */
	$"0032 004B 0065 880A 5061 7274 6974 696F"            /* .2.K.eà¬Partitio */
	$"6E3A 0000 0000 003D 0067 004A 0097 8800"            /* n:.....=.g.J.óà. */
	$"0000 0000 003D 0002 004A 001F 8803 3230"            /* .....=...J..à.20 */
	$"2500 0000 0000 004B 0029 0058 0065 880B"            /* %......K.).X.eà. */
	$"506C 6179 6572 2050 726F 3A00 0000 0000"            /* Player Pro:..... */
	$"004B 0067 0058 0097 8800 0000 0000 004B"            /* .K.g.X.óà......K */
	$"0002 0058 001D 8803 3230 2500"                      /* ...X..à.20%. */
};

data 'DITL' (217) {
	$"0015 0000 0000 0023 00B8 0037 00F2 0402"            /* .......#.∏.7.Ú.. */
	$"4F4B 0000 0000 0041 00B8 0055 00F2 0406"            /* OK.....A.∏.U.Ú.. */
	$"4361 6E63 656C 0000 0000 0006 004C 0013"            /* Cancel.......L.. */
	$"00F1 1000 0000 0000 001A 004C 0027 007B"            /* .Ò.........L.'.{ */
	$"1000 0000 0000 0006 0027 0013 0049 8805"            /* .........'...Ià. */
	$"4E61 6D65 3A00 0000 0000 001A 0027 0028"            /* Name:........'.( */
	$"0049 8805 526F 7773 3A00 0000 0000 006F"            /* .Ià.Rows:......o */
	$"003B 007C 0064 8807 5472 6163 6B73 3A00"            /* .;.|.dà.Tracks:. */
	$"0000 0000 006F 0064 007C 007C 8800 0000"            /* .....o.d.|.|à... */
	$"0000 006F 0084 007C 00C8 880E 5369 7A65"            /* ...o.Ñ.|.»à.Size */
	$"2069 6E20 6279 7465 733A 0000 0000 006F"            /*  in bytes:.....o */
	$"00C8 007C 00F9 8800 0000 0000 006A FFED"            /* .».|.˘à......jˇÌ */
	$"006B 017D C002 000B 0000 0000 006F 0006"            /* .k.}¿........o.. */
	$"007C 001F 8803 4944 3A00 0000 0000 006F"            /* .|..à.ID:......o */
	$"0016 007C 0031 8800 0000 0000 002E 0006"            /* ...|.1à......... */
	$"003B 0049 880C 436F 6D70 7265 7373 696F"            /* .;.Ià.Compressio */
	$"6E3A 0000 0000 002E 0072 003B 0086 4002"            /* n:.......r.;.Ü@. */
	$"00C8 0000 0000 002E 004C 003B 006F 8800"            /* .».......L.;.oà. */
	$"0000 0000 0042 000C 004F 0040 0506 5370"            /* .....B...O.@..Sp */
	$"6565 643A 0000 0000 0056 000C 0063 0049"            /* eed:.....V...c.I */
	$"0506 5465 6D70 6F3A 0000 0000 0042 004C"            /* ..Tempo:.....B.L */
	$"004F 007B 1000 0000 0000 0056 004C 0063"            /* .O.{.......V.L.c */
	$"007B 1000 0000 0000 0042 0081 004F 00AB"            /* .{.......B.Å.O.´ */
	$"8808 2830 2E2E 2E33 3129 0000 0000 0056"            /* à.(0...31).....V */
	$"0081 0063 00B5 880A 2833 322E 2E2E 3235"            /* .Å.c.µà¬(32...25 */
	$"3529"                                               /* 5) */
};

data 'DITL' (317) {
	$"0018 0000 0000 000A 0118 001E 0152 0402"            /* .......¬.....R.. */
	$"4F4B 0000 0000 0028 0118 003C 0152 0406"            /* OK.....(...<.R.. */
	$"4361 6E63 656C 0000 0000 000A 0014 0018"            /* Cancel.....¬.... */
	$"0049 880A 4669 6C65 204E 616D 653A 0000"            /* .Ià¬File Name:.. */
	$"0000 000A 0052 0017 0102 1000 0000 0000"            /* ...¬.R.......... */
	$"0022 0029 0030 0049 8805 5479 7065 3A00"            /* .".).0.Ià.Type:. */
	$"0000 0000 003A 001C 0048 0049 8808 4372"            /* .....:...H.Ià.Cr */
	$"6561 746F 723A 0000 0000 0022 0052 002F"            /* eator:.....".R./ */
	$"0073 1000 0000 0000 0022 007A 002F 008E"            /* .s.......".z./.é */
	$"4002 00C8 0000 0000 003A 0052 0047 0073"            /* @..».....:.R.G.s */
	$"1000 0000 0000 003A 007A 0047 008E 4002"            /* .......:.z.G.é@. */
	$"00C8 0000 0000 001D 00BC 003D 00DC 8000"            /* .».......º.=.‹Ä. */
	$"0000 0000 003D 009F 004A 00F9 8000 0000"            /* .....=.ü.J.˘Ä... */
	$"0000 004F FFF4 0050 024C C002 000B 0000"            /* ...OˇÙ.P.L¿..... */
	$"0000 0054 0001 0061 0049 880E 496E 7465"            /* ...T...a.Ià.Inte */
	$"726E 616C 204E 616D 653A 0000 0000 0072"            /* rnal Name:.....r */
	$"00C0 007F 00F1 8809 5061 7474 6572 6E73"            /* .¿...Òà∆Patterns */
	$"3A00 0000 0000 0072 0013 007F 0049 880A"            /* :......r.....Ià¬ */
	$"506C 7567 2054 7970 653A 0000 0000 0063"            /* Plug Type:.....c */
	$"00B1 0070 00F1 880C 496E 7374 7275 6D65"            /* .±.p.Òà.Instrume */
	$"6E74 733A 0000 0000 0063 002C 0070 0049"            /* nts:.....c.,.p.I */
	$"8805 5369 7A65 3A00 0000 0000 0054 004F"            /* à.Size:......T.O */
	$"0061 0122 0801 2D00 0000 0000 0072 00F7"            /* .a."..-......r.˜ */
	$"007F 0156 8801 2D00 0000 0000 0072 004F"            /* ...Và.-......r.O */
	$"007F 00AE 8801 2D00 0000 0000 0063 00F7"            /* ...Æà.-......c.˜ */
	$"0070 0156 8801 2D00 0000 0000 0063 004F"            /* .p.Và.-......c.O */
	$"0070 00AE 8801 2D00 0000 0000 0081 0023"            /* .p.Æà.-......Å.# */
	$"008E 0049 8806 5768 6572 653A 0000 0000"            /* .é.Ià.Where:.... */
	$"0081 004F 0112 00E4 8801 2D00"                      /* .Å.O...‰à.-. */
};

data 'DITL' (172) {
	$"0019 0000 0000 006E 0172 0082 01AC 0404"            /* .......n.r.Ç.¨.. */
	$"4D6F 7665 0000 0000 006E 01C2 0082 01FC"            /* Move.....n.¬.Ç.¸ */
	$"0406 4361 6E63 656C 0000 0000 0014 000A"            /* ..Cancel.......¬ */
	$"0026 0074 050A 496E 7374 7275 6D65 6E74"            /* .&.t.¬Instrument */
	$"0000 0000 0028 000A 003A 0063 0504 4E6F"            /* .....(.¬.:.c..No */
	$"7465 0000 0000 003C 000A 004E 0074 0506"            /* te.....<.¬.N.t.. */
	$"4566 6665 6374 0000 0000 0050 000A 0062"            /* Effect.....P.¬.b */
	$"0065 0508 4172 6775 6D65 6E74 0000 0000"            /* .e..Argument.... */
	$"0000 0046 0010 006B 8804 5748 4154 0000"            /* ...F...kà.WHAT.. */
	$"0000 0000 0104 0010 0129 8802 4259 0000"            /* .........)à.BY.. */
	$"0000 0000 019A 0010 01D2 8805 5748 4552"            /* .....ö...“à.WHER */
	$"4500 0000 0000 0014 0172 0026 01EE 060F"            /* E........r.&.Ó.. */
	$"4375 7272 656E 7420 7061 7474 6572 6E00"            /* Current pattern. */
	$"0000 0000 0028 0172 003A 0202 0613 5365"            /* .....(.r.:....Se */
	$"6C65 6374 696F 6E20 2844 6967 6974 616C"            /* lection (Digital */
	$"2900 0000 0000 0050 0172 0062 01DC 060A"            /* )......P.r.b.‹.¬ */
	$"4576 6572 7977 6865 7265 0000 0000 003C"            /* Everywhere.....< */
	$"0172 004E 0209 0615 5365 6C65 6374 696F"            /* .r.N.∆..Selectio */
	$"6E20 2843 6C61 7373 6963 616C 2900 0000"            /* n (Classical)... */
	$"0000 0014 0096 0021 00AA 4002 0384 0000"            /* .....ñ.!.™@..Ñ.. */
	$"0000 0014 0078 0024 008E 8802 3030 0000"            /* .....x.$.éà.00.. */
	$"0000 0028 0096 0035 00AA 4002 0384 0000"            /* ...(.ñ.5.™@..Ñ.. */
	$"0000 0028 0078 0038 008E 8802 3030 0000"            /* ...(.x.8.éà.00.. */
	$"0000 0050 0078 0060 008A 1002 3030 0000"            /* ...P.x.`.ä..00.. */
	$"0000 003C 0096 0049 00AA 4002 0384 0000"            /* ...<.ñ.I.™@..Ñ.. */
	$"0000 003C 0078 004C 008E 8802 3030 0000"            /* ...<.x.L.éà.00.. */
	$"0000 001E 00D2 0030 0155 0811 4E6F 206F"            /* .....“.0.U..No o */
	$"6620 7365 6D69 2D74 6F6E 6573 3A00 0000"            /* f semi-tones:... */
	$"0000 003C 0104 004C 0116 1002 3030 0000"            /* ...<...L....00.. */
	$"0000 0000 00B4 0064 00B5 C002 000B 0000"            /* .....¥.d.µ¿..... */
	$"0000 0000 0168 0064 0169 C002 000B 0000"            /* .....h.d.i¿..... */
	$"0000 0000 0000 0001 0320 C002 000B 0000"            /* ......... ¿..... */
	$"0000 0064 0000 0065 0320 C002 000B"                 /* ...d...e. ¿... */
};

data 'DITL' (2005) {
	$"000C 0000 0000 00A0 000A 00B0 0051 0000"            /* .......†.¬.∞.Q.. */
	$"0000 0000 00A0 0064 00B0 00AA 0000 0000"            /* .....†.d.∞.™.... */
	$"0000 00B4 006E 00C4 00B1 0000 0000 0000"            /* ...¥.n.ƒ.±...... */
	$"0082 000A 0094 0101 0510 4E6F 7465 7320"            /* .Ç.¬.î....Notes  */
	$"7072 6F6A 6563 7469 6F6E 0000 0000 0028"            /* projection.....( */
	$"000A 003A 005B 0508 4D61 726B 6572 733A"            /* .¬.:.[..Markers: */
	$"0000 0000 0046 001E 0056 0053 8807 4F66"            /* .....F...V.Sà.Of */
	$"6673 6574 3A1E 0000 0000 0046 005A 0056"            /* fset:......F.Z.V */
	$"00A5 1000 0000 0000 0046 00C8 0056 00FC"            /* .•.......F.».V.¸ */
	$"8805 4C6F 6F70 3AAF 0000 0000 0046 0104"            /* à.Loop:Ø.....F.. */
	$"0056 014F 1000 0000 0000 00AA 00C8 00BE"            /* .V.O.......™.».æ */
	$"00DC 8000 0000 0000 006E 000A 0080 009B"            /* .‹Ä......n.¬.Ä.õ */
	$"050F 4F63 7461 7665 7320 4D61 726B 6572"            /* ..Octaves Marker */
	$"735A 0000 0000 0096 0000 009E 0028 8000"            /* sZ.....ñ...û.(Ä. */
	$"0000 0000 0064 0000 0065 0160 C002 0386"            /* .....d...e.`¿..Ü */
};

data 'DITL' (161) {
	$"000F 0000 0000 0087 00FC 009B 014C 0404"            /* .......á.¸.õ.L.. */
	$"4F70 656E 0000 0000 0068 00FC 007C 014C"            /* Open.....h.¸.|.L */
	$"0406 4361 6E63 656C 0000 0000 0000 0000"            /* ..Cancel........ */
	$"0000 0000 8000 0000 0000 0008 00EB 0018"            /* ....Ä........Î.. */
	$"0151 0000 0000 0000 0020 00FC 0034 014C"            /* .Q....... .¸.4.L */
	$"0405 456A 6563 7400 0000 0000 003C 00FC"            /* ..Eject......<.¸ */
	$"0050 014C 0407 4465 736B 746F 7000 0000"            /* .P.L..Desktop... */
	$"0000 001D 000C 00FF 00E6 0000 0000 0000"            /* .......ˇ.Ê...... */
	$"0006 000C 0019 00E6 0000 0000 0000 005B"            /* .......Ê.......[ */
	$"00FB 005C 014D C002 000B 0000 0000 00AA"            /* .˚.\.M¿........™ */
	$"00FA 0100 0150 C002 0085 0000 0000 00FF"            /* .˙...P¿..Ö.....ˇ */
	$"000A 0111 00B1 0511 506C 6179 2077 6865"            /* .¬...±..Play whe */
	$"6E20 636C 6963 6B65 6400 0000 0000 00FF"            /* n clicked......ˇ */
	$"00BD 010F 00E6 8800 0000 0000 0114 000A"            /* .Ω...Êà........¬ */
	$"0125 0082 050E 5368 6F77 2041 6C6C 2046"            /* .%.Ç..Show All F */
	$"696C 6573 0000 0000 012A 006C 013B 00EC"            /* iles.....*.l.;.Ï */
	$"8813 4176 6169 6C61 626C 6520 666F 726D"            /* à.Available form */
	$"6174 733A 2000 0000 0000 013B 000D 016B"            /* ats: ......;...k */
	$"0151 8805 790D 790D 7900 0000 0000 0127"            /* .Qà.y.y.y......' */
	$"0000 0128 0160 C002 000B"                           /* ...(.`¿... */
};

data 'DITL' (346) {
	$"0002 0000 0000 0000 0000 005F 0181 C002"            /* ..........._.Å¿. */
	$"0080 0000 0000 0002 0068 0015 00E7 C002"            /* .Ä.......h...Á¿. */
	$"0097 0000 0000 005F 0000 00D7 0181 C002"            /* .ó....._...◊.Å¿. */
	$"008D"                                               /* .ç */
};

data 'DITL' (934) {
	$"0002 0000 0000 0049 00AE 005D 00E8 0402"            /* .......I.Æ.].Ë.. */
	$"4F4B 0000 0000 0002 0029 0044 00EB 8862"            /* OK.......).D.Îàb */
	$"4375 7272 656E 746C 792C 205E 3025 206F"            /* Currently, ^0% o */
	$"6620 7468 6520 7072 6F63 6573 7369 6E67"            /* f the processing */
	$"2074 696D 6520 6973 2075 7365 6420 666F"            /*  time is used fo */
	$"7220 6D75 7369 6320 6765 6E65 7261 7469"            /* r music generati */
	$"6F6E 2061 6E64 206F 7468 6572 2062 6163"            /* on and other bac */
	$"6B67 726F 756E 6420 7072 6F63 6573 7365"            /* kground processe */
	$"732E 0000 0000 0003 0004 0023 0024 C002"            /* s..........#.$¿. */
	$"00DC"                                               /* .‹ */
};

data 'DITL' (337, "Spectre") {
	$"000A 0000 0000 0010 0000 0024 0014 8000"            /* .¬.........$..Ä. */
	$"0000 0000 0001 0113 000E 013B 8806 5363"            /* ...........;à.Sc */
	$"616C 653A 0000 0000 0001 013C 000E 0164"            /* ale:.......<...d */
	$"8800 0000 0000 0001 0003 000E 0036 8808"            /* à............6à. */
	$"4469 7370 6C61 793A 0000 0000 0001 008B"            /* Display:.......ã */
	$"000E 009F 4002 0384 0000 0000 0001 0037"            /* ...ü@..Ñ.......7 */
	$"000E 008A 8800 0000 0000 0001 00AB 000E"            /* ...äà........´.. */
	$"00CC 8805 5369 7A65 3A0E 0000 0000 0001"            /* .Ãà.Size:....... */
	$"00EC 000E 0100 4002 0384 0000 0000 0001"            /* .Ï....@..Ñ...... */
	$"00CD 000E 00E9 8800 0000 0000 0010 0000"            /* .Õ...Èà......... */
	$"07E0 07D0 0000 0000 0000 0001 0167 000E"            /* .‡.–.........g.. */
	$"017B 4002 0384"                                     /* .{@..Ñ */
};

data 'DITL' (176) {
	$"0014 0000 0000 000A 017D 001E 01B7 0402"            /* .......¬.}...∑.. */
	$"4F4B 0000 0000 0028 017D 003C 01B7 0406"            /* OK.....(.}.<.∑.. */
	$"4361 6E63 656C 0000 0000 0004 0014 0011"            /* Cancel.......... */
	$"0048 8809 4361 7465 676F 7279 3A00 0000"            /* .Hà∆Category:... */
	$"0000 0014 000C 0021 0048 880B 496E 7374"            /* .......!.Hà.Inst */
	$"7275 6D65 6E74 3A00 0000 0000 0004 004B"            /* rument:........K */
	$"0011 005F 4002 00C8 0000 0000 0014 004B"            /* ..._@..».......K */
	$"0021 005F 4002 00C8 0000 0000 0004 0064"            /* .!._@..».......d */
	$"0011 00DE 8800 0000 0000 0014 0064 0021"            /* ...ﬁà........d.! */
	$"00DE 8800 0000 0000 004A 0029 0057 0048"            /* .ﬁà......J.).W.H */
	$"8805 4E6F 7465 3A00 0000 0000 004A 0064"            /* à.Note:......J.d */
	$"0057 0081 0800 0000 0000 004A 004B 0057"            /* .W.Å.......J.K.W */
	$"005F 4002 00C8 0000 0000 0024 002A 0031"            /* ._@..».....$.*.1 */
	$"0048 8805 5369 7A65 3A00 0000 0000 0059"            /* .Hà.Size:......Y */
	$"0004 0093 01C5 4002 00E1 0000 0000 0024"            /* ...ì.≈@..·.....$ */
	$"0064 0031 00DE 0800 0000 0000 0046 FFEE"            /* .d.1.ﬁ.......FˇÓ */
	$"0047 030E C002 000B 0000 0000 0005 01DF"            /* .G..¿..........ﬂ */
	$"0015 0233 8000 0000 0000 0034 0064 0041"            /* ...3Ä......4.d.A */
	$"00DE 0800 0000 0000 0034 000A 0041 0048"            /* .ﬁ.......4.¬.A.H */
	$"080B 4D49 4449 2D47 4D20 4944 3A00 0000"            /* ..MIDI-GM ID:... */
	$"0000 0035 022D 0045 0289 0000 0000 0000"            /* ...5.-.E.â...... */
	$"0035 0291 0045 02C5 8000 0000 0000 004A"            /* .5.ë.E.≈Ä......J */
	$"015E 0057 01C6 8815 5175 6963 6B74 696D"            /* .^.W.∆à.Quicktim */
	$"6520 7665 7273 696F 6E20 322E 3200"                 /* e version 2.2. */
};

data 'DITL' (173) {
	$"0000 0000 0000 0001 0001 000F 07D1 8800"            /* .............—à. */
};

data 'DITL' (153) {
	$"000A 0000 0000 0026 000E 07F6 07DE 0000"            /* .¬.....&...ˆ.ﬁ.. */
	$"0000 0000 0018 0000 0026 07D0 0000 0000"            /* .........&.–.... */
	$"0000 0001 013E 0015 0152 0000 0000 0000"            /* .....>...R...... */
	$"0026 0000 07F6 000E 0000 0000 0000 0002"            /* .&...ˆ.......... */
	$"0018 0016 002C 0000 0000 0000 0002 002C"            /* .....,........., */
	$"0016 0040 0000 0000 0000 0002 0004 0016"            /* ...@............ */
	$"0018 0000 0000 0000 0002 00E0 0016 00F4"            /* ...........‡...Ù */
	$"0000 0000 0000 0005 004D 0012 006E 8805"            /* .........M...nà. */
	$"5369 7A65 3A00 0000 0000 0005 008C 0012"            /* Size:........å.. */
	$"00A0 4002 0384 0000 0000 0005 006F 0012"            /* .†@..Ñ.......o.. */
	$"008B 8800"                                          /* .ãà. */
};

data 'DITL' (174) {
	$"0004 0000 0000 0139 00B7 014D 00F1 0405"            /* .......9.∑.M.Ò.. */
	$"5072 696E 7400 0000 0000 0139 010B 014D"            /* Print......9...M */
	$"0145 0404 446F 6E65 0000 0000 0085 FFF2"            /* .E..Done.....ÖˇÚ */
	$"0086 01E6 C002 000B 0000 0000 0006 000B"            /* .Ü.Ê¿........... */
	$"007F 0143 4002 0087 0000 0000 0131 FFDF"            /* ...C@..á.....1ˇﬂ */
	$"0132 01D3 C002 000B"                                /* .2.”¿... */
};

data 'DITL' (154) {
	$"000D 0000 0000 0005 016D 0019 01A7 0402"            /* .........m...ß.. */
	$"4F4B 0000 0000 000A 000A 00A8 00A0 0000"            /* OK.....¬.¬.®.†.. */
	$"0000 0000 000A 00AA 001A 00EB 880D 5369"            /* .....¬.™...Îà.Si */
	$"7A65 206F 6E20 6469 736B 3A00 0000 0000"            /* ze on disk:..... */
	$"001E 00AA 002E 00EB 880C 5369 7A65 2069"            /* ...™...Îà.Size i */
	$"6E20 5241 4D3A 0000 0000 0032 00AA 0042"            /* n RAM:.....2.™.B */
	$"00EB 8805 436F 6465 3A00 0000 0000 0046"            /* .Îà.Code:......F */
	$"00AA 0056 00EB 8805 5479 7065 3A00 0000"            /* .™.V.Îà.Type:... */
	$"0000 005A 00AA 006A 00EB 880A 436F 7079"            /* ...Z.™.j.Îà¬Copy */
	$"7269 6768 743A 0000 0000 0078 00AA 0088"            /* right:.....x.™.à */
	$"00EB 8806 5768 6572 653A 0000 0000 000A"            /* .Îà.Where:.....¬ */
	$"00F0 001A 015A 8800 0000 0000 001E 00F0"            /* ....Zà........ */
	$"002E 015A 8800 0000 0000 0032 00F0 0042"            /* ...Zà......2..B */
	$"015A 8800 0000 0000 0046 00F0 0056 015A"            /* .Zà......F..V.Z */
	$"8800 0000 0000 005A 00F0 0074 01A6 8800"            /* à......Z..t.¶à. */
	$"0000 0000 0078 00F0 00A8 01A6 8800"                 /* .....x..®.¶à. */
};

data 'DITL' (137, purgeable) {
	$"000C 0000 0000 00A1 00FC 00B5 014C 0404"            /* .......°.¸.µ.L.. */
	$"5361 7665 0000 0000 0082 00FC 0096 014C"            /* Save.....Ç.¸.ñ.L */
	$"0406 4361 6E63 656C 0000 0000 0000 0000"            /* ..Cancel........ */
	$"0000 0000 8000 0000 0000 0008 00EB 0018"            /* ....Ä........Î.. */
	$"0151 0000 0000 0000 0020 00FC 0034 014C"            /* .Q....... .¸.4.L */
	$"0405 456A 6563 7400 0000 0000 003C 00FC"            /* ..Eject......<.¸ */
	$"0050 014C 0407 4465 736B 746F 7000 0000"            /* .P.L..Desktop... */
	$"0000 001D 000C 00EA 00E6 0000 0000 0000"            /* .......Í.Ê...... */
	$"0007 000C 001A 00E6 0000 0000 0000 0077"            /* .......Ê.......w */
	$"00FA 0078 014E C002 000B 0000 0000 0106"            /* .˙.x.N¿......... */
	$"0010 0116 00E4 1000 0000 0000 00F1 0010"            /* .....‰.......Ò.. */
	$"0101 00B6 8811 5361 7665 2064 6F63 756D"            /* ...∂à.Save docum */
	$"656E 7420 6173 3A00 0000 0000 0058 00FC"            /* ent as:......X.¸ */
	$"006C 014C 8000 0000 0000 00C3 00FA 0119"            /* .l.LÄ......√.˙.. */
	$"0150 C002 0085"                                     /* .P¿..Ö */
};

data 'DITL' (171) {
	$"000B 0000 0000 0005 00C8 0019 0102 0402"            /* .........»...... */
	$"4F4B 0000 0000 0023 00C8 0037 0102 0406"            /* OK.....#.».7.... */
	$"4361 6E63 656C 0000 0000 0003 000D 0010"            /* Cancel.......... */
	$"002D 8805 5479 7065 3A00 0000 0000 0003"            /* .-à.Type:....... */
	$"0030 0010 0077 0800 0000 0000 002B 0003"            /* .0...w.......+.. */
	$"0038 002D 8807 466F 726D 6174 3A00 0000"            /* .8.-à.Format:... */
	$"0000 002B 0030 0038 009B 0800 0000 0000"            /* ...+.0.8.õ...... */
	$"002B 009D 0038 00B1 4002 00C8 0000 0000"            /* .+.ù.8.±@..».... */
	$"003F 000B 004C 002D 8805 4E61 6D65 3ACC"            /* .?...L.-à.Name:Ã */
	$"0000 0000 003F 0030 004C 00CE 1000 0000"            /* .....?.0.L.Œ.... */
	$"0000 0017 0010 0024 002D 8805 5369 7A65"            /* .......$.-à.Size */
	$"3A00 0000 0000 0017 0030 0024 0087 0800"            /* :........0.$.á.. */
	$"0000 0000 00AE 0224 00BE 0297 0800"                 /* .....Æ.$.æ.ó.. */
};

data 'DITL' (301, purgeable) {
	$"0004 0000 0000 003C 0014 0050 0064 0409"            /* .......<...P.d.∆ */
	$"5265 6D6F 7665 2069 7400 0000 0000 005C"            /* Remove it......\ */
	$"0014 0070 0064 0407 4669 6E64 2069 7400"            /* ...p.d..Find it. */
	$"0000 0000 005C 00BE 0070 010E 0406 4361"            /* .....\.æ.p....Ca */
	$"6E63 656C 0000 0000 0006 003C 0036 010E"            /* ncel.......<.6.. */
	$"8824 4920 6361 6E6E 6F74 2066 696E 6420"            /* à$I cannot find  */
	$"D25E 30D3 2E0D 5768 6174 2073 6861 6C6C"            /* “^0”..What shall */
	$"2049 2064 6F3F 0000 0000 000A 000A 002A"            /*  I do?.....¬.¬.* */
	$"002A C002 00DC"                                     /* .*¿..‹ */
};

data 'DITL' (3004, purgeable) {
	$"0004 0000 0000 003C 0014 0050 005A 0403"            /* .......<...P.Z.. */
	$"5965 7300 0000 0000 005C 0014 0070 005A"            /* Yes......\...p.Z */
	$"0402 4E6F 0000 0000 005C 00C8 0070 010E"            /* ..No.....\.».p.. */
	$"0406 4361 6E63 656C 0000 0000 0006 003C"            /* ..Cancel.......< */
	$"0036 010E 882C 5361 7665 2063 7572 7265"            /* .6..à,Save curre */
	$"6E74 204D 7573 6963 204C 6973 7420 D25E"            /* nt Music List “^ */
	$"30D3 2062 6566 6F72 6520 636C 6F73 696E"            /* 0” before closin */
	$"673F 0000 0000 000A 000A 002A 002A C002"            /* g?.....¬.¬.*.*¿. */
	$"00DC"                                               /* .‹ */
};

data 'DITL' (302) {
	$"0002 0000 0000 0051 010E 0065 0148 0402"            /* .......Q...e.H.. */
	$"4F4B 0000 0000 000A 003C 0043 014B 8831"            /* OK.....¬.<.C.Kà1 */
	$"596F 7572 206D 7573 6963 2069 7320 6E6F"            /* Your music is no */
	$"7720 5E30 2062 7974 6573 2073 6D61 6C6C"            /* w ^0 bytes small */
	$"6572 2074 6861 6E20 6265 666F 7265 2E2E"            /* er than before.. */
	$"2E00 0000 0000 000A 000A 002A 002A C002"            /* .......¬.¬.*.*¿. */
	$"00DC"                                               /* .‹ */
};

data 'DITL' (134) {
	$"0018 0000 0000 0028 003E 07F8 080E 0000"            /* .......(.>.¯.... */
	$"0000 0000 001A 0000 0028 07D0 0000 0000"            /* .........(.–.... */
	$"0000 0003 0016 0017 002A 0000 0000 0000"            /* .........*...... */
	$"0028 0000 07F8 003E 0000 0000 0000 0003"            /* .(...¯.>........ */
	$"0002 0017 0016 0000 0000 0000 0003 00AC"            /* ...............¨ */
	$"0017 00C0 0000 0000 0000 0003 0030 0017"            /* ...¿.........0.. */
	$"0044 0000 0000 0000 0003 0049 0017 005D"            /* .D.........I...] */
	$"0000 0000 0000 0005 0170 0015 018B 8804"            /* .........p...ãà. */
	$"496E 733A 0000 0000 0006 01A8 0013 01BC"            /* Ins:.......®...º */
	$"4002 00C8 0000 0000 0005 018C 0015 01A6"            /* @..».......å...¶ */
	$"0803 3030 3000 0000 0000 001A 0000 0028"            /* ..000..........( */
	$"003E 8000 0000 0000 0001 00C4 0019 00D7"            /* .>Ä........ƒ...◊ */
	$"0000 0000 0000 0001 00D6 0019 00E9 0000"            /* .........÷...È.. */
	$"0000 0000 0001 00E8 0019 00FB 0000 0000"            /* .......Ë...˚.... */
	$"0000 0001 00FA 0019 010D 0000 0000 0000"            /* .....˙.......... */
	$"0001 010C 0019 011F 0000 0000 0000 0001"            /* ................ */
	$"0136 0019 0149 0000 0000 0000 0001 0148"            /* .6...I.........H */
	$"0019 015B 0000 0000 0000 0001 015A 0019"            /* ...[.........Z.. */
	$"016D 0000 0000 0000 0003 0062 0017 0076"            /* .m.........b...v */
	$"0000 0000 0000 0003 007B 0017 008F 0000"            /* .........{...è.. */
	$"0000 0000 0003 0094 0017 00A8 0000 0000"            /* .......î...®.... */
	$"0000 0005 01BF 0015 02E9 0800 0000 0000"            /* .....ø...È...... */
	$"0001 011E 0019 0131 0000"                           /* .......1.. */
};

data 'DITL' (2007) {
	$"000F 0000 0000 0028 000A 0038 0088 0808"            /* .......(.¬.8.à.. */
	$"4765 6E65 7261 6C3A 0000 0000 00AE 001F"            /* General:.....Æ.. */
	$"00C6 010B 8000 0000 0000 0058 001D 0068"            /* .∆..Ä......X...h */
	$"0069 0508 4D61 726B 6572 733A 0000 0000"            /* .i..Markers:.... */
	$"0058 0075 0068 00A3 8807 4F66 6673 6574"            /* .X.u.h.£à.Offset */
	$"3A00 0000 0000 0058 0117 0068 0149 1000"            /* :......X...h.I.. */
	$"0000 0000 0058 00EC 0068 0111 8805 4C6F"            /* .....X.Ï.h..à.Lo */
	$"6F70 3A00 0000 0000 0058 00A9 0068 00DB"            /* op:......X.©.h.€ */
	$"1000 0000 0000 0041 001D 0051 00B4 0511"            /* .......A...Q.¥.. */
	$"5368 6F77 206E 6F74 6573 206C 656E 6774"            /* Show notes lengt */
	$"6806 0000 0000 0070 001D 0080 0069 0806"            /* h......p...Ä.i.. */
	$"5465 6D70 6F3A 0000 0000 0070 008D 0080"            /* Tempo:.....p.ç.Ä */
	$"00A3 8803 4E6F 3A00 0000 0000 0070 0117"            /* .£à.No:......p.. */
	$"0080 0149 1000 0000 0000 0070 00F2 0080"            /* .Ä.I.......p.Ú.Ä */
	$"0111 8805 556E 6974 3A00 0000 0000 0070"            /* ..à.Unit:......p */
	$"00A9 0080 00DB 1000 0000 0000 0088 001C"            /* .©.Ä.€.......à.. */
	$"0098 007C 880D 5472 6163 6B20 4865 6967"            /* .ò.|à.Track Heig */
	$"6874 3A03 0000 0000 0088 00A9 0098 00CF"            /* ht:......à.©.ò.œ */
	$"0803 3132 3802 0000 0000 0089 00D1 0096"            /* ..128......â.—.ñ */
	$"00E5 4002 00C8"                                     /* .Â@..» */
};

data 'DITL' (2008) {
	$"0027 0000 0000 0028 000A 0038 0088 0808"            /* .'.....(.¬.8.à.. */
	$"4765 6E65 7261 6C3A 0000 0000 003D 0014"            /* General:.....=.. */
	$"004D 0045 0502 4631 0000 0000 004D 0014"            /* .M.E..F1.....M.. */
	$"005D 0045 0502 4632 0000 0000 005D 0014"            /* .].E..F2.....].. */
	$"006D 0045 0502 4633 0000 0000 006D 0014"            /* .m.E..F3.....m.. */
	$"007D 0045 0502 4634 0000 0000 007D 0014"            /* .}.E..F4.....}.. */
	$"008D 0045 0502 4635 0000 0000 008D 0014"            /* .ç.E..F5.....ç.. */
	$"009D 0045 0502 4636 0000 0000 009D 0014"            /* .ù.E..F6.....ù.. */
	$"00AD 0045 0502 4637 0000 0000 00AD 0014"            /* .≠.E..F7.....≠.. */
	$"00BD 0045 0502 4638 0000 0000 00BD 0014"            /* .Ω.E..F8.....Ω.. */
	$"00CD 0045 0502 4639 0000 0000 00CD 0014"            /* .Õ.E..F9.....Õ.. */
	$"00DD 0045 0503 4631 3000 0000 0000 00DD"            /* .›.E..F10......› */
	$"0014 00ED 0045 0503 4631 3100 0000 0000"            /* ...Ì.E..F11..... */
	$"00ED 0014 00FD 0045 0503 4631 3200 0000"            /* .Ì...˝.E..F12... */
	$"0000 00FD 0014 010D 0045 0503 4631 3300"            /* ...˝.....E..F13. */
	$"0000 0000 003E 0047 004B 005B 4002 00C8"            /* .....>.G.K.[@..» */
	$"0000 0000 004E 0047 005B 005B 4002 00C8"            /* .....N.G.[.[@..» */
	$"0000 0000 005E 0047 006B 005B 4002 00C8"            /* .....^.G.k.[@..» */
	$"0000 0000 006E 0047 007B 005B 4002 00C8"            /* .....n.G.{.[@..» */
	$"0000 0000 007E 0047 008B 005B 4002 00C8"            /* .....~.G.ã.[@..» */
	$"0000 0000 008E 0047 009B 005B 4002 00C8"            /* .....é.G.õ.[@..» */
	$"0000 0000 009E 0047 00AB 005B 4002 00C8"            /* .....û.G.´.[@..» */
	$"0000 0000 00AE 0047 00BB 005B 4002 00C8"            /* .....Æ.G.ª.[@..» */
	$"0000 0000 00BE 0047 00CB 005B 4002 00C8"            /* .....æ.G.À.[@..» */
	$"0000 0000 00CE 0047 00DB 005B 4002 00C8"            /* .....Œ.G.€.[@..» */
	$"0000 0000 00DE 0047 00EB 005B 4002 00C8"            /* .....ﬁ.G.Î.[@..» */
	$"0000 0000 00EE 0047 00FB 005B 4002 00C8"            /* .....Ó.G.˚.[@..» */
	$"0000 0000 00FE 0047 010B 005B 4002 00C8"            /* .....˛.G...[@..» */
	$"0000 0000 003D 005F 004D 0159 8800 0000"            /* .....=._.M.Yà... */
	$"0000 004D 005F 005D 0159 8800 0000 0000"            /* ...M._.].Yà..... */
	$"005D 005F 006D 0159 8800 0000 0000 006D"            /* .]._.m.Yà......m */
	$"005F 007D 0159 8800 0000 0000 007D 005F"            /* ._.}.Yà......}._ */
	$"008D 0159 8800 0000 0000 008D 005F 009D"            /* .ç.Yà......ç._.ù */
	$"0159 8800 0000 0000 009D 005F 00AD 0159"            /* .Yà......ù._.≠.Y */
	$"8800 0000 0000 00AD 005F 00BD 0159 8800"            /* à......≠._.Ω.Yà. */
	$"0000 0000 00BD 005F 00CD 0159 8800 0000"            /* .....Ω._.Õ.Yà... */
	$"0000 00CD 005F 00DD 0159 8800 0000 0000"            /* ...Õ._.›.Yà..... */
	$"00DD 005F 00ED 0159 8800 0000 0000 00ED"            /* .›._.Ì.Yà......Ì */
	$"005F 00FD 0159 8800 0000 0000 00FD 005F"            /* ._.˝.Yà......˝._ */
	$"010D 0159 8800"                                     /* ...Yà. */
};

data 'DITL' (195) {
	$"0000 0000 0000 FFFB FFFC 7CFB 7CFC 0000"            /* ......ˇ˚ˇ¸|˚|¸.. */
};

data 'DITL' (149, "Custom open controls") {
	$"0009 0000 0000 FFFE 0005 000B 000E 8000"            /* .∆....ˇ˛......Ä. */
	$"0000 0000 000D 0006 001E 0032 0806 5368"            /* ...........2..Sh */
	$"6F77 3A20 0000 0000 FFFF 0009 000C 0017"            /* ow: ....ˇˇ.∆.... */
	$"8000 0000 0000 FFFE 0005 0009 0012 8000"            /* Ä.....ˇ˛...∆..Ä. */
	$"0000 0000 0003 0000 000A 000D 8000 0000"            /* .........¬..Ä... */
	$"0000 000D 004D 001D 00DE 0800 0000 0000"            /* .....M...ﬁ...... */
	$"000E 0034 001B 0048 4002 0385 0000 0000"            /* ...4...H@..Ö.... */
	$"0000 FF38 0014 FF88 8403 4164 6400 0000"            /* ..ˇ8..ˇàÑ.Add... */
	$"0000 0000 0003 000C 0012 8000 0000 0000"            /* ..........Ä..... */
	$"0000 FF38 0014 FF88 8407 5072 6576 6965"            /* ..ˇ8..ˇàÑ.Previe */
	$"7700"                                               /* w. */
};

data 'DITL' (135) {
	$"0015 0000 0000 0002 0003 0016 0017 0000"            /* ................ */
	$"0000 0000 0002 006D 0016 0081 0000 0000"            /* .......m...Å.... */
	$"0000 0002 0081 0016 0095 0000 0000 0000"            /* .....Å...ï...... */
	$"0002 0095 0016 00A9 0000 0000 0000 0002"            /* ...ï...©........ */
	$"00A9 0016 00BD 0000 0000 0000 0002 00F3"            /* .©...Ω.........Û */
	$"0016 0107 0000 0000 0000 0002 00C3 0016"            /* .............√.. */
	$"00D7 0000 0000 0000 0002 00DB 0016 00EF"            /* .◊.........€...Ô */
	$"0000 0000 0000 0026 0033 07F6 0803 0000"            /* .......&.3.ˆ.... */
	$"0000 0000 0006 011F 0016 0128 8000 0000"            /* ...........(Ä... */
	$"0000 0026 001F 07F6 0033 0000 0000 0000"            /* ...&...ˆ.3...... */
	$"FFFE 028B 000E 02C7 0000 0000 0000 0026"            /* ˇ˛.ã...«.......& */
	$"000F 07F6 001F 0000 0000 0000 0018 0000"            /* ...ˆ............ */
	$"0026 07D0 0000 0000 0000 0002 012F 0016"            /* .&.–........./.. */
	$"0143 0000 0000 0000 0026 0000 07F6 000F"            /* .C.......&...ˆ.. */
	$"8000 0000 0000 0003 0020 0013 0039 0000"            /* Ä........ ...9.. */
	$"0000 0000 0003 0043 0013 0056 0000 0000"            /* .......C...V.... */
	$"0000 0003 0055 0013 0066 0000 0000 0000"            /* .....U...f...... */
	$"0003 0038 0013 0044 0000 0000 0000 0003"            /* ...8...D........ */
	$"0020 0013 0066 0000 0000 0000 0018 0000"            /* . ...f.......... */
	$"0026 0033 8000"                                     /* .&.3Ä. */
};

data 'DITL' (845) {
	$"000F 0000 0000 0003 0003 0043 00D1 8000"            /* ...........C.—Ä. */
	$"0000 0000 0003 0002 0043 000F 8000 0000"            /* .........C..Ä... */
	$"0000 0046 0002 0086 000F 8000 0000 0000"            /* ...F...Ü..Ä..... */
	$"0089 0002 00C9 000F 8000 0000 0000 00CC"            /* .â...…..Ä......Ã */
	$"0002 010C 000F 8000 0000 0000 0046 0003"            /* ......Ä......F.. */
	$"0086 00D1 8000 0000 0000 0089 0003 00C9"            /* .Ü.—Ä......â...… */
	$"00D1 8000 0000 0000 00CC 0003 010C 00D1"            /* .—Ä......Ã.....— */
	$"8000 0000 0000 010F 0002 014F 00D0 8000"            /* Ä..........O.–Ä. */
	$"0000 0000 010F 0001 014F 000E 8000 0000"            /* .........O..Ä... */
	$"0000 0152 0001 0192 000E 8000 0000 0000"            /* ...R...í..Ä..... */
	$"0195 0001 01D5 000E 8000 0000 0000 01D8"            /* .ï...’..Ä......ÿ */
	$"0001 0218 000E 8000 0000 0000 0152 0002"            /* ......Ä......R.. */
	$"0192 00D0 8000 0000 0000 0195 0002 01D5"            /* .í.–Ä......ï...’ */
	$"00D0 8000 0000 0000 01D8 0002 0218 00D0"            /* .–Ä......ÿ.....– */
	$"8000"                                               /* Ä. */
};

data 'DITL' (132, "MOD Options") {
	$"0003 0000 0000 0005 00E1 0019 011B 0402"            /* .........·...... */
	$"4F4B 0000 0000 0005 0122 0019 015C 0406"            /* OK......."...\.. */
	$"4361 6E63 656C 0000 0000 0006 0014 0019"            /* Cancel.......... */
	$"00CE 0702 008D 0000 0000 0020 0000 0021"            /* .Œ...ç..... ...! */
	$"0168 C002 00C9"                                     /* .h¿..… */
};

data 'DITL' (177, purgeable) {
	$"0002 0000 0000 00DC 00FC 00F0 014C 0408"            /* .......‹.¸..L.. */
	$"436F 6E74 696E 7565 0000 0000 0018 0006"            /* Continue........ */
	$"00D4 014E 0000 0000 0000 0000 0008 0014"            /* .‘.N............ */
	$"0110 8822 5265 6164 2074 6869 7320 6265"            /* ..à"Read this be */
	$"666F 7265 2075 7369 6E67 2074 6869 7320"            /* fore using this  */
	$"6D75 7369 633A"                                     /* music: */
};

data 'DITL' (175) {
	$"0009 0000 0000 00E5 013E 00F9 0178 0402"            /* .∆.....Â.>.˘.x.. */
	$"4F4B 0000 0000 0106 013E 011A 0178 0404"            /* OK.......>...x.. */
	$"5175 6974 0000 0000 0107 0016 0117 00FC"            /* Quit...........¸ */
	$"1000 0000 0000 00A2 0016 00B2 00FC 1000"            /* .......¢...≤.¸.. */
	$"0000 0000 00F0 0016 0102 012C 8824 5365"            /* ..........,à$Se */
	$"7269 616C 204E 756D 6265 7220 2869 6E63"            /* rial Number (inc */
	$"6C75 6465 6420 696E 2079 6F75 7220 4344"            /* luded in your CD */
	$"293A 0000 0000 008A 0016 009C 006F 8805"            /* ):.....ä...ú.oà. */
	$"4E61 6D65 3A02 0000 0000 006D 004A 0080"            /* Name:......m.J.Ä */
	$"0170 8824 506C 6561 7365 2065 6E74 6572"            /* .pà$Please enter */
	$"2066 6F6C 6C6F 7769 6E67 2069 6E66 6F72"            /*  following infor */
	$"6D61 7469 6F6E 733A 0000 0000 0000 0000"            /* mations:........ */
	$"005F 0181 4002 0080 0000 0000 00D5 0016"            /* ._.Å@..Ä.....’.. */
	$"00E5 00FC 1000 0000 0000 00BE 0016 00D0"            /* .Â.¸.......æ...– */
	$"0098 880D 4F72 6761 6E69 7A61 7469 6F6E"            /* .òà.Organization */
	$"3A22"                                               /* :" */
};

data 'DITL' (2002) {
	$"000E 0000 0000 0050 0014 0062 00A4 060A"            /* .......P...b.§.¬ */
	$"4C6F 6F70 206D 7573 6963 0000 0000 003C"            /* Loop music.....< */
	$"00AB 004E 0130 060F 4C6F 6164 206E 6578"            /* .´.N.0..Load nex */
	$"7420 6D75 7369 6302 0000 0000 0050 00AB"            /* t music......P.´ */
	$"0064 0160 0615 4C6F 6164 2061 206D 7573"            /* .d.`..Load a mus */
	$"6963 2072 616E 646F 6D6C 7902 0000 0000"            /* ic randomly..... */
	$"0028 0008 0038 00C9 881C 4166 7465 7220"            /* .(...8.…à.After  */
	$"6861 7669 6E67 2070 6C61 7965 6420 6120"            /* having played a  */
	$"6D75 7369 633A 0000 0000 0000 0000 0023"            /* music:.........# */
	$"0103 8000 0000 0000 00BD 0015 00CF 0134"            /* ..Ä......Ω...œ.4 */
	$"0516 5368 6F77 2072 6561 6C20 4669 6E64"            /* ..Show real Find */
	$"6572 2069 636F 6E73 0000 0000 003C 0014"            /* er icons.....<.. */
	$"004E 00A4 060C 5374 6F70 2070 6C61 7969"            /* .N.§..Stop playi */
	$"6E67 0000 0000 0095 0015 00A7 0150 052A"            /* ng.....ï...ß.P.* */
	$"4C6F 6164 2066 6972 7374 206D 7573 6963"            /* Load first music */
	$"2077 6865 6E20 6F70 656E 696E 6720 6120"            /*  when opening a  */
	$"6D75 7369 6320 6C69 7374 0000 0000 0068"            /* music list.....h */
	$"0000 0069 0160 C002 0386 0000 0000 006E"            /* ...i.`¿..Ü.....n */
	$"00DC 007D 00F4 8000 0000 0000 00A9 0015"            /* .‹.}.ÙÄ......©.. */
	$"00BB 0142 0528 476F 2074 6F20 5374 6172"            /* .ª.B.(Go to Star */
	$"7475 7020 706F 7369 7469 6F6E 2077 6865"            /* tup position whe */
	$"6E20 646F 6E65 2070 6C61 7969 6E67 0000"            /* n done playing.. */
	$"0000 00D1 0015 00E3 0134 0525 4173 6B20"            /* ...—...„.4.%Ask  */
	$"746F 2073 6176 6520 6D6F 6469 6669 6564"            /* to save modified */
	$"204D 7573 6963 204C 6973 7420 6669 6C65"            /*  Music List file */
	$"7302 0000 0000 00E5 0015 00F7 0134 0526"            /* s......Â...˜.4.& */
	$"4175 746F 6D61 7469 6320 2750 6C61 7927"            /* Automatic 'Play' */
	$"2061 6674 6572 206F 7065 6E69 6E67 2061"            /*  after opening a */
	$"206D 7573 6963 0000 0000 006D 0008 007D"            /*  music.....m...} */
	$"00C9 8814 4D75 7369 6320 4C69 7374 2053"            /* .…à.Music List S */
	$"6574 7469 6E67 733A 0000 0000 0081 0015"            /* ettings:.....Å.. */
	$"0093 0135 0524 5265 6D65 6D62 6572 204D"            /* .ì.5.$Remember M */
	$"7573 6963 204C 6973 7420 666F 7220 6E65"            /* usic List for ne */
	$"7874 2073 7461 7274 7570"                           /* xt startup */
};

data 'DITL' (200) {
	$"0007 0000 0000 0000 0000 005F 0181 C002"            /* ..........._.Å¿. */
	$"0080 0000 0000 006D 008F 007B 00F2 8000"            /* .Ä.....m.è.{.ÚÄ. */
	$"0000 0000 00F7 005C 0105 00BC 8000 0000"            /* .....˜.\...ºÄ... */
	$"0000 0095 005C 00A3 0124 8000 0000 0000"            /* ...ï.\.£.$Ä..... */
	$"0000 0000 00E4 0181 8000 0000 0000 0100"            /* .....‰.ÅÄ....... */
	$"001C 0120 00E4 8000 0000 0000 00B0 0188"            /* ... .‰Ä......∞.à */
	$"00D0 0250 C002 0089 0000 0000 00D0 005C"            /* .–.P¿..â.....–.\ */
	$"00E2 0124 C002 009C"                                /* .‚.$¿..ú */
};

data 'DITL' (761) {
	$"002D 0000 0000 0005 0008 0019 0042 0402"            /* .-...........B.. */
	$"4F4B 0000 0000 0069 00AC 0077 00DC 8809"            /* OK.....i.¨.w.‹à∆ */
	$"506C 6179 7468 7275 3A02 0000 0000 0069"            /* Playthru:......i */
	$"00DC 0077 010B 0804 7472 7565 0000 0000"            /* .‹.w....true.... */
	$"0069 010F 0076 0123 4002 00C8 0000 0000"            /* .i...v.#@..».... */
	$"0026 0005 0034 00A1 880C 4175 6469 6F20"            /* .&...4.°à.Audio  */
	$"496E 5075 743A 0000 0000 0046 0005 0077"            /* InPut:.....F...w */
	$"007E 0000 0000 0000 0035 0005 0043 004C"            /* .~.......5...C.L */
	$"080D 536F 756E 6420 4472 6976 6572 3A02"            /* ..Sound Driver:. */
	$"0000 0000 004D 0081 006D 00A1 8000 0000"            /* .....M.Å.m.°Ä... */
	$"0000 0047 00BF 0055 00DD 8805 5261 7465"            /* ...G.ø.U.›à.Rate */
	$"3A02 0000 0000 0036 00C1 0044 00DD 8805"            /* :......6.¡.D.›à. */
	$"4269 7473 3A02 0000 0000 0036 00DC 0044"            /* Bits:......6.‹.D */
	$"010B 0801 3802 0000 0000 0036 010F 0043"            /* ....8......6...C */
	$"0123 4002 00C8 0000 0000 0047 00DC 0055"            /* .#@..».....G.‹.U */
	$"010B 0808 3434 3130 3020 487A 0000 0000"            /* ....44100 Hz.... */
	$"0047 010F 0054 0123 4002 00C8 0000 0000"            /* .G...T.#@..».... */
	$"0021 FFBA 0022 01AE C002 000B 0000 0000"            /* .!ˇ∫.".Æ¿....... */
	$"0005 00BF 0019 00F9 0406 4361 6E63 656C"            /* ...ø...˘..Cancel */
	$"0000 0000 007A FFB0 007B 01A4 C002 000B"            /* .....zˇ∞.{.§¿... */
	$"0000 0000 0096 0008 00AA 0024 0000 0000"            /* .....ñ...™.$.... */
	$"0000 0096 0031 00AA 004D 0000 0000 0000"            /* ...ñ.1.™.M...... */
	$"0096 0058 00AA 0074 0000 0000 0000 0096"            /* .ñ.X.™.t.......ñ */
	$"0080 00AA 009C 0000 0000 0000 0083 0005"            /* .Ä.™.ú.......É.. */
	$"0093 0029 8805 4E61 6D65 3A02 0000 0000"            /* .ì.)à.Name:..... */
	$"00AA 0005 00B8 0026 8806 5265 636F 7264"            /* .™...∏.&à.Record */
	$"0000 0000 00AA 0034 00B8 004A 8804 5374"            /* .....™.4.∏.Jà.St */
	$"6F70 0000 0000 00AA 0058 00B8 0074 8805"            /* op.....™.X.∏.tà. */
	$"5061 7573 6502 0000 0000 00AA 0082 00B8"            /* Pause......™.Ç.∏ */
	$"0097 8804 506C 6179 0000 0000 00BA 0008"            /* .óà.Play.....∫.. */
	$"00CB 009C 8000 0000 0000 0089 00D1 0097"            /* .À.úÄ......â.—.ó */
	$"00FD 8809 4176 6169 6C61 626C 6502 0000"            /* .˝à∆Available... */
	$"0000 00A5 00B3 00B3 00DD 8807 4B42 7974"            /* ...•.≥.≥.›à.KByt */
	$"6573 3A02 0000 0000 00B7 00D2 00C5 00FD"            /* es:......∑.“.≈.˝ */
	$"8808 5265 636F 7264 6564 0000 0000 00D3"            /* à.Recorded.....” */
	$"00B3 00E1 00DD 8807 4B42 7974 6573 3A02"            /* .≥.·.›à.KBytes:. */
	$"0000 0000 0097 00BE 00A5 00DD 8805 5365"            /* .....ó.æ.•.›à.Se */
	$"6373 3A02 0000 0000 00C5 00BE 00D3 00DD"            /* cs:......≈.æ.”.› */
	$"8805 5365 6373 3A02 0000 0000 00A5 00DD"            /* à.Secs:......•.› */
	$"00B3 0123 8800 0000 0000 0097 00DD 00A5"            /* .≥.#à......ó.›.• */
	$"0123 8800 0000 0000 00D3 00DD 00E1 0123"            /* .#à......”.›.·.# */
	$"8800 0000 0000 00C5 00DD 00D3 0123 8800"            /* à......≈.›.”.#à. */
	$"0000 0000 00CE 0008 00DF 009C 8000 0000"            /* .....Œ...ﬂ.úÄ... */
	$"0000 0083 002C 0090 009B 1000 0000 0000"            /* ...É.,.ê.õ...... */
	$"00E2 0008 00EA 009C 8000 0000 0000 0035"            /* .‚...Í.úÄ......5 */
	$"004F 0043 0093 0808 4275 696C 742D 696E"            /* .O.C.ì..Built-in */
	$"0000 0000 0035 0095 0042 00A9 4002 00C8"            /* .....5.ï.B.©@..» */
	$"0000 0000 0058 00BC 0066 00DA 8805 4D6F"            /* .....X.º.f.⁄à.Mo */
	$"6465 3A02 0000 0000 0058 00DC 0066 010B"            /* de:......X.‹.f.. */
	$"0804 6D6F 6E6F 0000 0000 0058 010F 0065"            /* ..mono.....X...e */
	$"0123 4002 00C8 0000 0000 0005 0067 0019"            /* .#@..».......g.. */
	$"00A1 0406 4164 6420 6974"                           /* .°..Add it */
};

data 'DITL' (2000) {
	$"0022 0000 0000 0000 0000 0024 0160 8000"            /* .".........$.`Ä. */
	$"0000 0000 0025 003C 0035 013B 061A 536F"            /* .....%.<.5.;..So */
	$"756E 6420 4D61 6E61 6765 7220 782E 7820"            /* und Manager x.x  */
	$"284D 6163 204F 5329 0000 0000 00D3 0082"            /* (Mac OS).....”.Ç */
	$"00E8 013E 8000 0000 0000 0079 00B8 0089"            /* .Ë.>Ä......y.∏.â */
	$"00ED 8800 0000 0000 0037 003C 0047 00F5"            /* .Ìà......7.<.G.ı */
	$"0615 4D49 4449 204F 7574 7075 7420 2D20"            /* ..MIDI Output -  */
	$"284F 4D53 2920 3A02 0000 0000 004E 003C"            /* (OMS) :......N.< */
	$"005E 0086 0606 3131 204B 687A 0000 0000"            /* .^.Ü..11 Khz.... */
	$"004E 008C 005E 00CF 0606 3232 204B 687A"            /* .N.å.^.œ..22 Khz */
	$"0000 0000 004E 00DC 005E 0131 0606 3434"            /* .....N.‹.^.1..44 */
	$"204B 687A 0000 0000 0060 003C 0070 0084"            /*  Khz.....`.<.p.Ñ */
	$"0606 3820 4269 7473 0000 0000 0060 008C"            /* ..8 Bits.....`.å */
	$"0070 00D6 0607 3136 2042 6974 7302 0000"            /* .p.÷..16 Bits... */
	$"0000 0060 00DC 0070 0126 8607 3234 2042"            /* ...`.‹.p.&Ü.24 B */
	$"6974 7302 0000 0000 00B5 003C 00C5 00B6"            /* its......µ.<.≈.∂ */
	$"050E 4F76 6572 2053 616D 706C 696E 673A"            /* ..Over Sampling: */
	$"0000 0000 00B6 00B8 00C5 0118 880E 3939"            /* .....∂.∏.≈..à.99 */
	$"3920 4B68 7A20 2D20 3230 2078 0000 0000"            /* 9 Khz - 20 x.... */
	$"00E0 00D8 00F0 013C 8000 0000 0000 008D"            /* .‡.ÿ..<Ä......ç */
	$"010E 009D 0132 8800 0000 0000 00E0 00AC"            /* ...ù.2à......‡.¨ */
	$"00ED 00C0 8000 0000 0000 00D0 00D5 00E0"            /* .Ì.¿Ä......–.’.‡ */
	$"00EC 8000 0000 0000 00D8 0080 00E8 0097"            /* .ÏÄ......ÿ.Ä.Ë.ó */
	$"8000 0000 0000 0049 0000 004A 0160 C002"            /* Ä......I...J.`¿. */
	$"0386 0000 0000 0079 003C 0089 00AF 050D"            /* .Ü.....y.<.â.Ø.. */
	$"5374 6572 656F 2044 656C 6179 3A02 0000"            /* Stereo Delay:... */
	$"0000 00DC 0055 00ED 0074 8000 0000 0000"            /* ...‹.U.Ì.tÄ..... */
	$"0025 0002 0035 0036 8807 4472 6976 6572"            /* .%...5.6à.Driver */
	$"3A02 0000 0000 004E 000A 005E 0033 8805"            /* :......N.¬.^.3à. */
	$"5261 7465 3A02 0000 0000 0060 000F 0070"            /* Rate:......`...p */
	$"0033 8805 4269 7473 3A02 0000 0000 00DB"            /* .3à.Bits:......€ */
	$"002E 00EB 005D 8000 0000 0000 007B 00F2"            /* ...Î.]Ä......{.Ú */
	$"0088 0106 4002 0384 0000 0000 0074 0000"            /* .à..@..Ñ.....t.. */
	$"0075 0160 4002 0386 0000 0000 0036 00FB"            /* .u.`@..Ü.....6.˚ */
	$"0048 0149 0406 5365 7420 5570 0000 0000"            /* .H.I..Set Up.... */
	$"0079 000A 0089 0033 8805 4D69 7363 3A02"            /* .y.¬.â.3à.Misc:. */
	$"0000 0000 00A1 003C 00B1 00AB 0508 5375"            /* .....°.<.±.´..Su */
	$"7272 6F75 6E64 0000 0000 008D 00B8 009D"            /* rround.....ç.∏.ù */
	$"00ED 8800 0000 0000 008D 003C 009D 009C"            /* .Ìà......ç.<.ù.ú */
	$"0507 5265 7665 7262 3A02 0000 0000 008F"            /* ..Reverb:......è */
	$"00F2 009C 0106 4002 0384 0000 0000 008F"            /* .Ú.ú..@..Ñ.....è */
	$"0134 009C 0148 4002 0384 0000 0000 00B7"            /* .4.ú.H@..Ñ.....∑ */
	$"011C 00C4 0130 4002 0384"                           /* ...ƒ.0@..Ñ */
};

data 'DITL' (164, "Custom open INSTRU controls") {
	$"0001 0000 0000 000C 0008 001C 00A8 0511"            /* .............®.. */
	$"506C 6179 2077 6865 6E20 636C 6963 6B65"            /* Play when clicke */
	$"6402 0000 0000 000C 00A8 001C 0148 050E"            /* d........®...H.. */
	$"5368 6F77 2041 6C6C 2046 696C 6573"                 /* Show All Files */
};

data 'DITL' (178, purgeable) {
	$"0020 0000 0000 000C 0146 0020 018A 0402"            /* . .......F. .ä.. */
	$"4F4B 0000 0000 002D 0148 0041 0188 0406"            /* OK.....-.H.A.à.. */
	$"4361 6E63 656C 0000 0000 0005 0003 0015"            /* Cancel.......... */
	$"004E 8807 4865 6164 6572 3A02 0000 0000"            /* .Nà.Header:..... */
	$"001D 0003 002D 0047 8805 4269 7473 3A02"            /* .....-.Gà.Bits:. */
	$"0000 0000 0035 0003 0045 0047 8805 4D6F"            /* .....5...E.Gà.Mo */
	$"6465 3A02 0000 0000 004D 0003 005D 0047"            /* de:......M...].G */
	$"8805 5479 7065 3A02 0000 0000 007C 0003"            /* à.Type:......|.. */
	$"008B 0047 8807 4C65 6E67 7468 3A02 0000"            /* .ã.Gà.Length:... */
	$"0000 001D 0064 002D 00A6 0606 3820 4269"            /* .....d.-.¶..8 Bi */
	$"7473 0000 0000 001D 00D3 002D 0115 0607"            /* ts.......”.-.... */
	$"3136 2042 6974 7302 0000 0000 0035 0064"            /* 16 Bits......5.d */
	$"0045 00A6 0604 4D6F 6E6F 0000 0000 0035"            /* .E.¶..Mono.....5 */
	$"00D3 0045 0115 0606 5374 6572 656F 0000"            /* .”.E....Stereo.. */
	$"0000 004D 0064 005D 00A6 0606 5369 676E"            /* ...M.d.].¶..Sign */
	$"6564 0000 0000 004D 00D3 005D 013F 0608"            /* ed.....M.”.].?.. */
	$"556E 7369 676E 6564 0000 0000 0065 0003"            /* Unsigned.....e.. */
	$"0075 004F 8807 436F 6469 6E67 3A02 0000"            /* .u.Oà.Coding:... */
	$"0000 0065 0064 0075 00D1 060D 4C69 7474"            /* ...e.d.u.—..Litt */
	$"6C65 2045 6E64 6961 6E02 0000 0000 0065"            /* le Endian......e */
	$"00D3 0075 013F 060A 4269 6720 456E 6469"            /* .”.u.?.¬Big Endi */
	$"616E 0000 0000 0005 0064 0015 00C8 1000"            /* an.......d...».. */
	$"0000 0000 0005 00D3 0015 0107 8805 4279"            /* .......”....à.By */
	$"7465 7302 0000 0000 007B 0157 008B 018B"            /* tes......{.W.ã.ã */
	$"8805 4279 7465 7302 0000 0000 007B 00EB"            /* à.Bytes......{.Î */
	$"008B 014F 1000 0000 0000 00BF 0003 00CF"            /* .ã.O.......ø...œ */
	$"0046 8808 5072 6576 6965 773A 0000 0000"            /* .Fà.Preview:.... */
	$"0094 0003 00A3 0047 8805 5261 7465 3A02"            /* .î...£.Gà.Rate:. */
	$"0000 0000 0094 0098 00A4 00AE 8802 487A"            /* .....î.ò.§.Æà.Hz */
	$"0000 0000 0094 0064 00A4 0093 1000 0000"            /* .....î.d.§.ì.... */
	$"0000 00BF 0047 00FF 0192 0000 0000 0000"            /* ...ø.G.ˇ.í...... */
	$"007B 0064 008B 00BC 060B 456E 6420 4F66"            /* .{.d.ã.º..End Of */
	$"2046 696C 6502 0000 0000 007B 00D3 008B"            /*  File......{.”.ã */
	$"00E5 0600 0000 0000 00B2 FFF2 00B3 024A"            /* .Â.......≤ˇÚ.≥.J */
	$"C002 000B 0000 0000 0096 00B0 00A3 00C4"            /* ¿........ñ.∞.£.ƒ */
	$"4002 0385 0000 0000 0104 0003 0113 0041"            /* @..Ö...........A */
	$"8808 4469 7370 6C61 793A 0000 0000 0104"            /* à.Display:...... */
	$"0047 0114 0111 8800 0000 0000 0104 0115"            /* .G....à......... */
	$"0113 0192 8800 0000 0000 00DD 0003 00F1"            /* ...íà......›...Ò */
	$"003D 0404 506C 6179"                                /* .=..Play */
};

data 'DITL' (163) {
	$"0008 0000 0000 0004 0007 0014 012D 8000"            /* .............-Ä. */
	$"0000 0000 0017 0007 0028 0080 8811 4B42"            /* .........(.Äà.KB */
	$"7974 6573 2067 656E 6572 6174 6564 3A00"            /* ytes generated:. */
	$"0000 0000 0017 0080 0028 012C 8800 0000"            /* .......Ä.(.,à... */
	$"0000 003F 0007 004F 0069 880D 456C 6170"            /* ...?...O.ià.Elap */
	$"7365 6420 7469 6D65 3A02 0000 0000 003F"            /* sed time:......? */
	$"00B9 004F 012D 8800 0000 0000 0053 0007"            /* .π.O.-à......S.. */
	$"0063 00B9 8819 4573 7469 6D61 7465 6420"            /* .c.πà.Estimated  */
	$"7265 6D61 696E 696E 6720 7469 6D65 3A02"            /* remaining time:. */
	$"0000 0000 0053 00B5 0063 012D 8800 0000"            /* .....S.µ.c.-à... */
	$"0000 002B 0007 003B 006A 880B 5468 726F"            /* ...+...;.jà.Thro */
	$"7567 6870 7574 3A02 0000 0000 002B 007A"            /* ughput:......+.z */
	$"003B 012D 8800"                                     /* .;.-à. */
};

data 'DITL' (136) {
	$"0008 0000 0000 0018 00EB 002C 0125 0404"            /* .........Î.,.%.. */
	$"5361 7665 0000 0000 0034 00EB 0048 0125"            /* Save.....4.Î.H.% */
	$"0406 4361 6E63 656C 0000 0000 0002 0003"            /* ..Cancel........ */
	$"0012 0100 881E 5361 7665 2063 7572 7265"            /* ....à.Save curre */
	$"6E74 2077 696E 646F 7773 2073 7461 7465"            /* nt windows state */
	$"2069 6E3A 0000 0000 0036 003F 0046 00DB"            /*  in:.....6.?.F.€ */
	$"1000 0000 0000 0036 0008 0046 0039 8805"            /* .......6...F.9à. */
	$"4E61 6D65 3A02 0000 0000 001E 0008 002E"            /* Name:........... */
	$"0023 8803 4944 3A02 0000 0000 001E 003F"            /* .#à.ID:........? */
	$"002E 0053 8801 3002 0000 0000 001F 0055"            /* ...Sà.0........U */
	$"002C 0069 4002 0384 0000 0000 0051 0006"            /* .,.i@..Ñ.....Q.. */
	$"0065 013A 882A 4469 7265 6374 2073 6176"            /* .e.:à*Direct sav */
	$"696E 6720 6279 2070 7265 7373 696E 6720"            /* ing by pressing  */
	$"3A20 4374 726C 202B 2020 1120 2B20 4944"            /* : Ctrl +  . + ID */
};

data 'DITL' (138) {
	$"0006 0000 0000 0045 0109 0059 0143 0405"            /* .......E.∆.Y.C.. */
	$"5265 7365 7402 0000 0000 0046 00BE 005A"            /* Reset......F.æ.Z */
	$"00F8 0406 4361 6E63 656C 0000 0000 0016"            /* .¯..Cancel...... */
	$"0053 0028 0174 0521 4465 6C65 7465 2061"            /* .S.(.t.!Delete a */
	$"6C6C 2070 6174 7465 726E 7320 616E 6420"            /* ll patterns and  */
	$"7061 7274 6974 696F 6E02 0000 0000 0002"            /* partition....... */
	$"0003 0012 004E 8808 446F 2074 6869 733A"            /* .....Nà.Do this: */
	$"0000 0000 002B 0053 003D 0174 0520 4465"            /* .....+.S.=.t. De */
	$"6C65 7465 2061 6C6C 2069 6E73 7472 756D"            /* lete all instrum */
	$"656E 7473 2026 2073 616D 706C 6573 0000"            /* ents & samples.. */
	$"0000 0001 0053 0013 0174 0512 5075 7267"            /* .....S...t..Purg */
	$"6520 616C 6C20 7061 7474 6572 6E73 0000"            /* e all patterns.. */
	$"0000 0062 0002 0075 014E 8831 5761 726E"            /* ...b...u.Nà1Warn */
	$"696E 673A 2059 6F75 2077 696C 6C20 6E6F"            /* ing: You will no */
	$"7420 6265 2061 626C 6520 746F 2075 6E64"            /* t be able to und */
	$"6F20 7468 6973 2072 6573 6574 2100"                 /* o this reset!. */
};

data 'DITL' (139) {
	$"0027 0000 0000 0038 0015 13C0 139D 0000"            /* .'.....8...¿.ù.. */
	$"0000 0000 0002 005B 0016 006F 0000 0000"            /* .......[...o.... */
	$"0000 0002 0021 0016 0035 0000 0000 0000"            /* .....!...5...... */
	$"0002 003E 0016 0052 0000 0000 0000 002B"            /* ...>...R.......+ */
	$"0000 0038 03E8 0000 0000 0000 0002 0004"            /* ...8.Ë.......... */
	$"0016 0018 0000 0000 0000 0002 0078 0016"            /* .............x.. */
	$"008C 0000 0000 0000 0002 00CF 0016 00E3"            /* .å.........œ...„ */
	$"0000 0000 0000 0038 0000 0420 0015 0000"            /* .......8... .... */
	$"0000 0000 0002 0126 0016 013A 0000 0000"            /* .......&...:.... */
	$"0000 0002 0095 0016 00A9 0000 0000 0000"            /* .....ï...©...... */
	$"0002 00B2 0016 00C6 0000 0000 0000 0002"            /* ...≤...∆........ */
	$"00EC 0016 0100 0000 0000 0000 0002 0109"            /* .Ï.............∆ */
	$"0016 011D 0000 0000 0000 0004 01C7 0014"            /* .............«.. */
	$"01E2 8000 0000 0000 0004 01E3 0014 01FC"            /* .‚Ä........„...¸ */
	$"0000 0000 0000 001B 004D 0028 0061 4002"            /* .........M.(.a@. */
	$"00C8 0000 0000 001A 0176 002A 01A1 0404"            /* .».......v.*.°.. */
	$"4669 6C6C 0000 0000 0004 015A 0014 017E"            /* Fill.......Z...~ */
	$"8805 5374 6570 3A02 0000 0000 0004 017D"            /* à.Step:........} */
	$"0014 018A 0801 3122 0000 0000 0005 018C"            /* ...ä..1".......å */
	$"0012 01A0 4002 00C8 0000 0000 0018 FFFF"            /* ...†@..»......ˇˇ */
	$"0019 07CF C002 0089 0000 0000 0002 0143"            /* ...œ¿..â.......C */
	$"0016 0157 0000 0000 0000 0004 022C 0014"            /* ...W.........,.. */
	$"0245 8000 0000 0000 0004 0282 0014 029F"            /* .EÄ........Ç...ü */
	$"8000 0000 0000 0004 02E1 0014 02FD 8000"            /* Ä........·...˝Ä. */
	$"0000 0000 0004 0246 0014 0255 0000 0000"            /* .......F...U.... */
	$"0000 0004 02A1 0014 02B5 0000 0000 0000"            /* .....°...µ...... */
	$"0004 02FC 0014 030F 0000 0000 0000 001B"            /* ...¸............ */
	$"015B 0028 016F 4002 00C8 0000 0000 001B"            /* .[.(.o@..»...... */
	$"00FF 0028 0113 4002 00C8 0000 0000 001B"            /* .ˇ.(..@..»...... */
	$"00A1 0028 00B5 4002 00C8 0000 0000 0018"            /* .°.(.µ@..»...... */
	$"0173 002C 0174 C002 0089 0000 0000 0018"            /* .s.,.t¿..â...... */
	$"00B7 002C 00B8 C002 0089 0000 0000 0018"            /* .∑.,.∏¿..â...... */
	$"0116 002C 0117 C002 0089 0000 0000 0018"            /* ...,..¿..â...... */
	$"0064 002C 0065 C002 0089 0000 0000 001A"            /* .d.,.e¿..â...... */
	$"0066 002A 009F 0000 0000 0000 001A 00BB"            /* .f.*.ü.........ª */
	$"002A 00FD 0000 0000 0000 001A 011A 002A"            /* .*.˝...........* */
	$"0159 0000 0000 0000 001A 0003 002A 004A"            /* .Y...........*.J */
	$"0000"                                               /* .. */
};

data 'DITL' (319) {
	$"0025 0000 0000 008A 0158 009E 0192 0402"            /* .%.....ä.X.û.í.. */
	$"4F4B 0000 0000 00A6 0158 00BA 0192 0406"            /* OK.....¶.X.∫.í.. */
	$"4361 6E63 656C 0000 0000 002F 015B 003F"            /* Cancel...../.[.? */
	$"019E 0606 3438 204B 687A 0000 0000 00C2"            /* .û..48 Khz.....¬ */
	$"00BC 00D2 011C 8800 0000 0000 0121 0084"            /* .º.“..à......!.Ñ */
	$"0131 00C2 8000 0000 0000 0013 00CF 0023"            /* .1.¬Ä........œ.# */
	$"01D6 0800 0000 0000 006A 0033 007A 0062"            /* .÷.......j.3.z.b */
	$"8805 4D6F 6465 3A02 0000 0000 0086 00BC"            /* à.Mode:......Ü.º */
	$"0096 00F1 8800 0000 0000 002F 0062 003F"            /* .ñ.Òà....../.b.? */
	$"00AC 0606 3131 204B 687A 0000 0000 002F"            /* .¨..11 Khz...../ */
	$"00B2 003F 00F5 0606 3232 204B 687A 0000"            /* .≤.?.ı..22 Khz.. */
	$"0000 002F 0102 003F 0157 0606 3434 204B"            /* .../...?.W..44 K */
	$"687A 0000 0000 0041 0062 0051 00AA 0606"            /* hz.....A.b.Q.™.. */
	$"3820 4269 7473 0000 0000 0041 00B2 0051"            /* 8 Bits.....A.≤.Q */
	$"00FC 0607 3136 2042 6974 7302 0000 0000"            /* .¸..16 Bits..... */
	$"0041 0102 0051 014C 8607 3234 2042 6974"            /* .A...Q.LÜ.24 Bit */
	$"7302 0000 0000 0014 00B5 0021 00C9 4002"            /* s........µ.!.…@. */
	$"0385 0000 0000 006A 00B2 007A 00FA 0606"            /* .Ö.....j.≤.z.˙.. */
	$"5374 6572 656F 0000 0000 006A 0062 007A"            /* Stereo.....j.b.z */
	$"00A9 0604 4D6F 6E6F 0000 0000 009A 0110"            /* .©..Mono.....ö.. */
	$"00AA 0134 8800 0000 0000 0086 003C 0096"            /* .™.4à......Ü.<.ñ */
	$"00A4 050D 5374 6572 656F 2044 656C 6179"            /* .§..Stereo Delay */
	$"3A02 0000 0000 00C3 011E 00D0 0132 4002"            /* :......√...–.2@. */
	$"0385 0000 0000 002F 0039 003F 0062 8805"            /* .Ö...../.9.?.bà. */
	$"5261 7465 3A02 0000 0000 0041 003E 0051"            /* Rate:......A.>.Q */
	$"0062 8805 4269 7473 3A02 0000 0000 00C2"            /* .bà.Bits:......¬ */
	$"003C 00D2 00B7 050E 4F76 6572 2053 616D"            /* .<.“.∑..Over Sam */
	$"706C 696E 673A 0000 0000 0088 00F4 0095"            /* pling:.....à.Ù.ï */
	$"0108 4002 0385 0000 0000 0026 FFE2 0027"            /* ..@..Ö.....&ˇ‚.' */
	$"0221 4002 0386 0000 0000 0086 000E 0096"            /* .!@..Ü.....Ü...ñ */
	$"0037 8805 4D69 7363 3A02 0000 0000 00AE"            /* .7à.Misc:......Æ */
	$"003C 00BE 00AB 0508 5375 7272 6F75 6E64"            /* .<.æ.´..Surround */
	$"0000 0000 009A 00BC 00AA 00F1 8800 0000"            /* .....ö.º.™.Òà... */
	$"0000 009A 003C 00AA 0087 0507 5265 7665"            /* ...ö.<.™.á..Reve */
	$"7262 3A02 0000 0000 009C 00F4 00A9 0108"            /* rb:......ú.Ù.©.. */
	$"4002 0385 0000 0000 009C 0136 00A9 014A"            /* @..Ö.....ú.6.©.J */
	$"4002 0385 0000 0000 0057 0062 0064 0076"            /* @..Ö.....W.b.d.v */
	$"4002 0385 0000 0000 0055 0003 0065 0062"            /* @..Ö.....U...e.b */
	$"880C 436F 6D70 7265 7373 696F 6E3A 0000"            /* à.Compression:.. */
	$"0000 0055 007F 0065 0169 0800 0000 0000"            /* ...U...e.i...... */
	$"0081 FFE2 0082 0221 4002 0386 0000 0000"            /* .Åˇ‚.Ç.!@..Ü.... */
	$"0001 001B 0011 0062 880A 5365 6C65 6374"            /* .......bà¬Select */
	$"696F 6E3A 0000 0000 0001 0062 0011 00D0"            /* ion:.......b...– */
	$"060C 456E 7469 7265 204D 7573 6963 0000"            /* ..Entire Music.. */
	$"0000 0013 0062 0023 00B1 0608 5061 7474"            /* .....b.#.±..Patt */
	$"6572 6E3A"                                          /* ern: */
};

data 'DITL' (2006) {
	$"001F 0000 0000 003C 001E 004C 007F 050A"            /* .......<...L...¬ */
	$"496E 7374 7275 6D65 6E74 0000 0000 0050"            /* Instrument.....P */
	$"001E 0060 0057 0504 4E6F 7465 0000 0000"            /* ...`.W..Note.... */
	$"003C 0096 004C 00D7 0506 4566 6665 6374"            /* .<.ñ.L.◊..Effect */
	$"0000 0000 0050 0096 0060 00EB 0508 4172"            /* .....P.ñ.`.Î..Ar */
	$"6775 6D65 6E74 0000 0000 0028 000A 0038"            /* gument.....(.¬.8 */
	$"0088 0812 4365 6C6C 2069 6E66 6F72 6D61"            /* .à..Cell informa */
	$"7469 6F6E 733A 0000 0000 003C 0104 004C"            /* tions:.....<...L */
	$"0151 0506 566F 6C75 6D65 0000 0000 0064"            /* .Q..Volume.....d */
	$"0000 0065 0160 C002 0386 0000 0000 006E"            /* ...e.`¿..Ü.....n */
	$"001E 007E 006A 0508 4D61 726B 6572 733A"            /* ...~.j..Markers: */
	$"0000 0000 006E 0078 007E 00A6 8807 4F66"            /* .....n.x.~.¶à.Of */
	$"6673 6574 3A02 0000 0000 006E 0118 007E"            /* fset:......n...~ */
	$"014A 1000 0000 0000 006E 00F0 007E 0115"            /* .J.......n..~.. */
	$"8805 4C6F 6F70 3A02 0000 0000 006E 00AA"            /* à.Loop:......n.™ */
	$"007E 00DC 1000 0000 0000 009D 0000 009E"            /* .~.‹.......ù...û */
	$"0160 C002 0386 0000 0000 0085 00AA 0095"            /* .`¿..Ü.....Ö.™.ï */
	$"00DC 0000 0000 0000 00A4 000A 00B4 00E2"            /* .‹.......§.¬.¥.‚ */
	$"881A 506F 702D 7570 206B 6579 733A 206D"            /* à.Pop-up keys: m */
	$"6F75 7365 2063 6C69 636B 202B 0000 0000"            /* ouse click +.... */
	$"00B8 001E 00C8 007F 0507 636F 6E74 726F"            /* .∏...»....contro */
	$"6C02 0000 0000 00CC 001E 00DC 0057 0505"            /* l......Ã...‹.W.. */
	$"7368 6966 7402 0000 0000 00B8 0096 00C8"            /* shift......∏.ñ.» */
	$"00EB 0507 636F 6D6D 616E 6402 0000 0000"            /* .Î..command..... */
	$"00CC 0096 00DC 00E3 0506 6F70 7469 6F6E"            /* .Ã.ñ.‹.„..option */
	$"0000 0000 00E6 0000 00E7 0160 C002 0386"            /* .....Ê...Á.`¿..Ü */
	$"0000 0000 00EC 000A 00FC 0062 880D 4C69"            /* .....Ï.¬.¸.bà.Li */
	$"6E65 7320 6865 6967 6874 3A02 0000 0000"            /* nes height:..... */
	$"00EC 0096 00FC 00DA 0606 4E6F 726D 616C"            /* .Ï.ñ.¸.⁄..Normal */
	$"0000 0000 00EC 00F8 00FC 0137 0605 536D"            /* .....Ï.¯.¸.7..Sm */
	$"616C 6C02 0000 0000 00BE 00F0 00CE 015B"            /* all......æ..Œ.[ */
	$"8000 0000 0000 0086 0079 0096 00A4 8806"            /* Ä......Ü.y.ñ.§à. */
	$"436F 6C6F 723A 0000 0000 0128 0000 0129"            /* Color:.....(...) */
	$"0160 C002 0386 0000 0000 0101 00F8 0111"            /* .`¿..Ü.......¯.. */
	$"0137 0603 4F66 6602 0000 0000 0101 0096"            /* .7..Off........ñ */
	$"0111 00DA 0602 4F6E 0000 0000 0101 000A"            /* ...⁄..On.......¬ */
	$"0111 0062 880C 4D75 7369 6320 5472 6163"            /* ...bà.Music Trac */
	$"653A 0000 0000 0116 0096 0126 00DA 0604"            /* e:.......ñ.&.⁄.. */
	$"4C6F 6F70 0000 0000 0116 00F8 0126 0163"            /* Loop.......¯.&.c */
	$"0609 5061 7274 6974 696F 6E00 0000 0000"            /* .∆Partition..... */
	$"0116 000A 0126 008B 8811 5061 7474 6572"            /* ...¬.&.ãà.Patter */
	$"6E20 5772 6170 7069 6E67 3A02"                      /* n Wrapping:. */
};

data 'DITL' (2009) {
	$"000A 0000 0000 0000 0000 0024 0160 8000"            /* .¬.........$.`Ä. */
	$"0000 0000 0045 008B 0059 00D9 0406 5365"            /* .....E.ã.Y.Ÿ..Se */
	$"7420 5570 0000 0000 0073 001C 0085 00F5"            /* t Up.....s...Ö.ı */
	$"051C 4D49 4449 2043 6861 6E6E 656C 202D"            /* ..MIDI Channel - */
	$"3E20 496E 7320 2620 5472 6163 6B73 0000"            /* > Ins & Tracks.. */
	$"0000 0087 001C 0099 00F5 0517 4D49 4449"            /* ...á...ô.ı..MIDI */
	$"2056 656C 6F63 6974 7920 2D3E 2056 6F6C"            /*  Velocity -> Vol */
	$"756D 6502 0000 0000 009B 001C 00AD 010A"            /* ume......õ...≠.¬ */
	$"0521 5573 6520 4520 2620 4420 4566 6665"            /* .!Use E & D Effe */
	$"6374 2066 6F72 2070 6F73 6974 696F 6E6E"            /* ct for positionn */
	$"696E 6702 0000 0000 0046 000A 0057 008A"            /* ing......F.¬.W.ä */
	$"050E 5573 6520 4D49 4449 2049 6E70 7574"            /* ..Use MIDI Input */
	$"0000 0000 0027 0007 0039 00A2 8814 4D49"            /* .....'...9.¢à.MI */
	$"4449 2049 6E70 7574 202D 2028 4F4D 5329"            /* DI Input - (OMS) */
	$"203A 0000 0000 005D 001C 006F 008E 8810"            /*  :.....]...o.éà. */
	$"4E6F 7465 2043 6F6E 7665 7273 696F 6E3A"            /* Note Conversion: */
	$"0000 0000 0025 00AB 0039 00FB 8000 0000"            /* .....%.´.9.˚Ä... */
	$"0000 00BF 000A 00D0 0116 0524 5365 6E64"            /* ...ø.¬.–...$Send */
	$"204D 4944 4920 5469 6D69 6E67 2043 6C6F"            /*  MIDI Timing Clo */
	$"636B 2028 4D49 4449 204F 7574 7075 7429"            /* ck (MIDI Output) */
	$"0000 0000 00D6 008B 00EA 00D9 0406 5365"            /* .....÷.ã.Í.Ÿ..Se */
	$"7420 5570"                                          /* t Up */
};

data 'DITL' (130) {
	$"000B 0000 0000 0119 0005 012D 0058 0408"            /* ...........-.X.. */
	$"436F 6E74 696E 7565 0000 0000 005A 0003"            /* Continue.....Z.. */
	$"008A 0106 8802 5E30 0000 0000 0048 0003"            /* .ä..à.^0.....H.. */
	$"0058 0070 880C 4465 7363 7269 7074 696F"            /* .X.pà.Descriptio */
	$"6E3A 0000 0000 0000 0018 0035 00F6 884B"            /* n:.........5.ˆàK */
	$"4641 5441 4C20 4552 524F 5220 210D 436F"            /* FATAL ERROR !.Co */
	$"6E74 6163 7420 7468 6520 6175 7468 6F72"            /* ntact the author */
	$"2C20 706C 6561 7365 2C20 7769 7468 2074"            /* , please, with t */
	$"6865 2066 6F6C 6C6F 7769 6E67 2069 6E66"            /* he following inf */
	$"6F72 6D61 7469 6F6E 7320 3A02 0000 0000"            /* ormations :..... */
	$"0119 005C 012D 00AF 0408 4465 6275 6767"            /* ...\.-.Ø..Debugg */
	$"6572 0000 0000 0119 00B1 012D 0104 0404"            /* er.......±.-.... */
	$"4578 6974 0000 0000 00A3 0004 00B5 0108"            /* Exit.....£...µ.. */
	$"8802 5E31 0000 0000 0091 0004 00A1 0071"            /* à.^1.....ë...°.q */
	$"880C 536F 7572 6365 2043 6F64 653A 0000"            /* à.Source Code:.. */
	$"0000 00D1 0004 00E3 0108 8802 5E32 0000"            /* ...—...„..à.^2.. */
	$"0000 00BF 0004 00CF 0071 8808 4C69 6E65"            /* ...ø...œ.qà.Line */
	$"204E 6F3A 0000 0000 0100 0004 0112 0108"            /*  No:............ */
	$"8802 5E33 0000 0000 00EE 0004 00FE 0097"            /* à.^3.....Ó...˛.ó */
	$"8811 436F 6D70 696C 6174 696F 6E20 6461"            /* à.Compilation da */
	$"7465 3A19"                                          /* te:. */
};

data 'DITL' (2001) {
	$"001B 0000 0000 00AA 0052 00BC 00F3 8816"            /* .......™.R.º.Ûà. */
	$"4375 7272 656E 7420 4F63 7461 7665 206F"            /* Current Octave o */
	$"6666 7365 743A 0000 0000 004B 0058 005D"            /* ffset:.....K.X.] */
	$"00F8 0510 5573 6520 4D61 6320 4B65 7962"            /* .¯..Use Mac Keyb */
	$"6F61 7264 0000 0000 0092 00E0 00A4 0164"            /* oard.....í.‡.§.d */
	$"050F 4F63 7461 7665 7320 6D61 726B 6572"            /* ..Octaves marker */
	$"7302 0000 0000 0077 0111 0089 0149 0604"            /* s......w...â.I.. */
	$"5374 6F70 0000 0000 0000 0000 0024 0160"            /* Stop.........$.` */
	$"8000 0000 0000 0028 0005 00C3 0050 0000"            /* Ä......(...√.P.. */
	$"0000 0000 0077 00B7 0089 0101 0607 4B65"            /* .....w.∑.â....Ke */
	$"7920 4F46 4602 0000 0000 0028 0052 0048"            /* y OFF......(.R.H */
	$"0137 882E 5072 6573 7320 6F6E 2079 6F75"            /* .7à.Press on you */
	$"7220 4D61 6320 6B65 7962 6F61 7264 2074"            /* r Mac keyboard t */
	$"6F20 6368 616E 6765 2073 6574 7469 6E67"            /* o change setting */
	$"732E 0000 0000 0092 007F 00A4 00E0 050B"            /* s......í...§.‡.. */
	$"536D 616C 6C20 5069 616E 6F02 0000 0000"            /* Small Piano..... */
	$"00AA 00FA 00BC 0148 8800 0000 0000 008D"            /* .™.˙.º.Hà......ç */
	$"0052 008E 015B C002 0386 0000 0000 0062"            /* .R.é.[¿..Ü.....b */
	$"0052 0074 00B4 880C 5768 656E 206B 6579"            /* .R.t.¥à.When key */
	$"2055 503A 0000 0000 0077 0058 0089 00A2"            /*  UP:.....w.X.â.¢ */
	$"0607 4E6F 7468 696E 6702 0000 0000 0102"            /* ..Nothing....... */
	$"0057 0114 00DB 0610 4F6E 2043 7572 7265"            /* .W...€..On Curre */
	$"6E74 2074 7261 636B 0000 0000 0093 0052"            /* nt track.....ì.R */
	$"00A5 007F 8805 5669 6577 3A02 0000 0000"            /* .•..à.View:..... */
	$"0102 0015 010E 0032 8000 0000 0000 00F1"            /* .......2Ä......Ò */
	$"FFC8 0105 0050 8000 0000 0000 00FE 001E"            /* ˇ»...PÄ......˛.. */
	$"010E 004C 8000 0000 0000 00C9 00F2 012B"            /* ...LÄ......….Ú.+ */
	$"015A 0000 0000 0000 0100 002A 0114 0046"            /* .Z.........*...F */
	$"8000 0000 0000 00C6 0005 00EA 0055 8815"            /* Ä......∆...Í.Uà. */
	$"506C 6179 696E 6720 2620 5265 636F 7264"            /* Playing & Record */
	$"696E 6720 3A02 0000 0000 00DA 0057 00EC"            /* ing :......⁄.W.Ï */
	$"00A8 0609 4F6E 2074 7261 636B 3A02 0000"            /* .®.∆On track:... */
	$"0000 00DD 00BC 00EA 00D0 4002 00C8 0000"            /* ...›.º.Í.–@..».. */
	$"0000 00DB 00A9 00EB 00BC 8802 3233 0000"            /* ...€.©.Î.ºà.23.. */
	$"0000 00EE 0057 0100 00C5 060D 4F6E 2061"            /* ...Ó.W...≈..On a */
	$"6C6C 2074 7261 636B 7302 0000 0000 005E"            /* ll tracks......^ */
	$"0051 005F 015B C002 0386 0000 0000 00C3"            /* .Q._.[¿..Ü.....√ */
	$"0050 00C4 015B C002 0386 0000 0000 00C6"            /* .P.ƒ.[¿..Ü.....∆ */
	$"0057 00D8 00F0 0614 4F6E 2066 6F6C 6C6F"            /* .W.ÿ...On follo */
	$"7769 6E67 2074 7261 636B 733A"                      /* wing tracks: */
};

data 'DITL' (145) {
	$"0006 0000 0000 0000 001A 0064 07EA 0000"            /* ...........d.Í.. */
	$"0000 0000 007B FFB8 008B 0594 0702 0088"            /* .....{ˇ∏.ã.î...à */
	$"0000 0000 003C 0019 005C 07E9 0000 0000"            /* .....<...\.È.... */
	$"0000 0021 0003 002B 0017 0000 0000 0000"            /* ...!...+........ */
	$"0001 0003 0015 0017 0000 0000 0000 0030"            /* ...............0 */
	$"0000 003F 001A 8000 0000 0000 0016 0003"            /* ...?..Ä......... */
	$"0020 0017 0000"                                     /* . .... */
};

data 'DITL' (816) {
	$"000B 0000 0000 0025 0008 07F5 02C4 0000"            /* .......%...ı.ƒ.. */
	$"0000 0000 0018 0008 0024 0024 8803 506F"            /* .........$.$à.Po */
	$"7302 0000 0000 0008 00D8 001C 00F8 0000"            /* s........ÿ...¯.. */
	$"0000 0000 0008 0114 001C 0128 0000 0000"            /* ...........(.... */
	$"0000 0025 0026 07F5 0032 0000 0000 0000"            /* ...%.&.ı.2...... */
	$"0025 FFFF 07F5 0007 0000 0000 0000 0002"            /* .%ˇˇ.ı.......... */
	$"0038 0016 004C 0000 0000 0000 0018 0034"            /* .8...L.........4 */
	$"0024 0050 8802 4944 0000 0000 0018 0050"            /* .$.Pà.ID.......P */
	$"0023 00AC 8804 4E61 6D65 0000 0000 FFF9"            /* .#.¨à.Name....ˇ˘ */
	$"004F 0019 006F 0000 0000 0000 0002 0004"            /* .O...o.......... */
	$"0016 0018 0000 0000 0000 0002 001E 0016"            /* ................ */
	$"0032 0000"                                          /* .2.. */
};

data 'DITL' (180, purgeable) {
	$"001F 0000 0000 0047 0025 0056 003E 0805"            /* .......G.%.V.>.. */
	$"5369 7A65 3A02 0000 0000 0021 00DA 0041"            /* Size:......!.⁄.A */
	$"00FA 0000 0000 0000 008B 0001 1413 1389"            /* .˙.......ã.....â */
	$"0000 0000 0000 0047 003E 0056 0097 0800"            /* .......G.>.V.ó.. */
	$"0000 0000 0038 0012 0047 003E 0809 4475"            /* .....8...G.>.∆Du */
	$"7261 7469 6F6E 3A02 0000 0000 001B 0005"            /* ration:......... */
	$"002A 003E 080B 536F 756E 6420 5479 7065"            /* .*.>..Sound Type */
	$"3A02 0000 0000 0038 003E 0047 00A6 0800"            /* :......8.>.G.¶.. */
	$"0000 0000 001B 003E 002A 00D6 0800 0000"            /* .......>.*.÷.... */
	$"0000 0041 00AC 004E 0129 0000 0000 0000"            /* ...A.¨.N.)...... */
	$"002A 0002 0039 003E 080B 436F 6D70 7265"            /* .*...9.>..Compre */
	$"7373 6F72 3A02 0000 0000 002A 003E 0039"            /* ssor:......*.>.9 */
	$"00D3 0800 0000 0000 0002 0008 0016 001C"            /* .”.............. */
	$"0000 0000 0000 0060 0005 006E 0028 0807"            /* .......`...n.(.. */
	$"4C65 7665 6C73 3A02 0000 0000 0069 0083"            /* Levels:......i.É */
	$"0078 00A6 0807 5472 6562 6C65 3A02 0000"            /* .x.¶..Treble:... */
	$"0000 005A 008B 0069 00A6 0805 4261 7373"            /* ...Z.ã.i.¶..Bass */
	$"3A02 0000 0000 0069 00C2 0077 0112 0000"            /* :......i.¬.w.... */
	$"0000 0000 005A 00C2 0068 0112 0000 0000"            /* .....Z.¬.h...... */
	$"0000 0069 00A7 0078 00C1 0800 0000 0000"            /* ...i.ß.x.¡...... */
	$"005A 00A7 0069 00C1 0800 0000 0000 0002"            /* .Z.ß.i.¡........ */
	$"0026 0016 003A 0000 0000 0000 0079 0114"            /* .&...:.......y.. */
	$"0088 0128 0000 0000 0000 005B 002A 0065"            /* .à.(.......[.*.e */
	$"007A 0000 0000 0000 006A 002A 0074 007A"            /* .z.......j.*.t.z */
	$"0000 0000 0000 0069 0114 0078 0128 0000"            /* .......i...x.(.. */
	$"0000 0000 005A 0114 0069 0128 0000 0000"            /* .....Z...i.(.... */
	$"0000 0079 007E 0088 00A6 0808 4261 6C61"            /* ...y.~.à.¶..Bala */
	$"6E63 653A 0000 0000 0079 00C2 0089 0112"            /* nce:.....y.¬.â.. */
	$"0000 0000 0000 0077 0002 0086 0028 0807"            /* .......w...Ü.(.. */
	$"566F 6C75 6D65 3A00 0000 0000 0079 002A"            /* Volume:......y.* */
	$"0083 007A 0000 0000 0000 0079 00A7 0088"            /* .É.z.......y.ß.à */
	$"00C1 0800 0000 0000 0018 FFD5 0019 0F75"            /* .¡........ˇ’...u */
	$"C002 0089 0000 0000 0056 FFD0 0057 0F70"            /* ¿..â.....Vˇ–.W.p */
	$"C002 0089"                                          /* ¿..â */
};

data 'DITL' (872) {
	$"0001 0000 0000 0151 00BF 0166 01A5 0000"            /* .......Q.ø.f.•.. */
	$"0000 0000 0000 0000 01AB 01D9 C002 0084"            /* .........´.Ÿ¿..Ñ */
};

data 'DITL' (181, purgeable) {
	$"000B 0000 0000 000B 0154 001F 0198 0402"            /* .........T...ò.. */
	$"4F4B 0000 0000 002D 0156 0041 0196 0406"            /* OK.....-.V.A.ñ.. */
	$"4361 6E63 656C 0000 0000 005B 0156 006F"            /* Cancel.....[.V.o */
	$"0196 0403 4164 6402 0000 0000 007E 0156"            /* .ñ..Add......~.V */
	$"0092 0196 0406 5265 6D6F 7665 0000 0000"            /* .í.ñ..Remove.... */
	$"00DF 0045 00EC 0147 900C 4465 7363 7269"            /* .ﬂ.E.Ï.Gê.Descri */
	$"7074 696F 6E3A 0000 0000 00F5 0045 0102"            /* ption:.....ı.E.. */
	$"0147 9004 5445 5854 0000 0000 00F5 0028"            /* .Gê.TEXT.....ı.( */
	$"0102 003F 8804 5552 4C3A 0000 0000 00DF"            /* ...?à.URL:.....ﬂ */
	$"0006 00EC 003F 880C 4465 7363 7269 7074"            /* ...Ï.?à.Descript */
	$"696F 6E3A 0000 0000 0001 0003 00D2 014A"            /* ion:.........“.J */
	$"0000 0000 0000 00E5 0154 00F9 0194 0406"            /* .......Â.T.˘.î.. */
	$"4368 616E 6765 0000 0000 010C 0006 0155"            /* Change.........U */
	$"0197 0000 0000 0000 00A1 0156 00B5 0196"            /* .ó.......°.V.µ.ñ */
	$"0405 5265 7365 7465"                                /* ..Resete */
};

data 'DITL' (2004) {
	$"0012 0000 0000 003C 0013 004E 015A 0527"            /* .......<...N.Z.' */
	$"4175 746F 6D61 7469 6320 4D41 4431 2063"            /* Automatic MAD1 c */
	$"6F6D 7072 6573 7369 6F6E 2066 6F72 2070"            /* ompression for p */
	$"6174 7465 726E 7302 0000 0000 00DE 00C7"            /* atterns......ﬁ.« */
	$"00EB 00DB 4002 00C8 0000 0000 0028 0008"            /* .Î.€@..».....(.. */
	$"0038 0106 8822 4175 746F 6D61 7469 6320"            /* .8..à"Automatic  */
	$"636F 6D70 7265 7373 696F 6E20 7768 656E"            /* compression when */
	$"2073 6176 696E 673A 0000 0000 0052 0000"            /*  saving:.....R.. */
	$"0053 0160 C002 0386 0000 0000 0097 0008"            /* .S.`¿..Ü.....ó.. */
	$"00A7 014F 080D 4F73 6369 6C6C 6F73 636F"            /* .ß.O..Oscillosco */
	$"7065 3A02 0000 0000 00AB 0013 00BD 015A"            /* pe:......´...Ω.Z */
	$"0519 4472 6177 206C 696E 6573 2062 6574"            /* ..Draw lines bet */
	$"7765 656E 2070 6F69 6E74 7302 0000 0000"            /* ween points..... */
	$"0094 0000 0095 0160 C002 0386 0000 0000"            /* .î...ï.`¿..Ü.... */
	$"00C7 0008 00D9 015C 082A 5061 7474 6572"            /* .«...Ÿ.\.*Patter */
	$"6E73 204C 6973 7420 5769 6E64 6F77 2061"            /* ns List Window a */
	$"6E64 2050 6172 7469 7469 6F6E 2057 696E"            /* nd Partition Win */
	$"646F 773A 0000 0000 009F 0125 00B7 0159"            /* dow:.....ü.%.∑.Y */
	$"8000 0000 0000 0069 0013 007B 015D 052E"            /* Ä......i...{.].. */
	$"4164 6420 6578 7465 6E73 696F 6E20 746F"            /* Add extension to */
	$"2066 696C 6520 6E61 6D65 7320 2865 7861"            /*  file names (exa */
	$"6D70 6C65 203A 2022 2E4D 4F44 2229 0000"            /* mple : ".MOD").. */
	$"0000 00A6 0134 00B4 0147 8000 0000 0000"            /* ...¶.4.¥.GÄ..... */
	$"00C1 0000 00C2 0160 C002 0386 0000 0000"            /* .¡...¬.`¿..Ü.... */
	$"00DD 0013 00ED 00C4 881A 5573 6520 7468"            /* .›...Ì.ƒà.Use th */
	$"6973 2065 6469 746F 7220 7768 656E 206F"            /* is editor when o */
	$"7065 6E3A 0000 0000 00DD 00DF 00ED 015F"            /* pen:.....›.ﬂ.Ì._ */
	$"0800 0000 0000 0055 0008 0065 0106 8806"            /* .......U...e..à. */
	$"4669 6C65 733A 0000 0000 0108 0013 011A"            /* Files:.......... */
	$"015A 0520 506C 6179 2061 2073 6F75 6E64"            /* .Z. Play a sound */
	$"2077 6865 6E20 636C 6963 6B20 696E 2026"            /*  when click in & */
	$"206F 7574 0000 0000 00F4 0008 0104 014F"            /*  out.....Ù.....O */
	$"080F 5769 6E64 6F77 2042 7574 746F 6E73"            /* ..Window Buttons */
	$"3A02 0000 0000 00F1 0000 00F2 0160 C002"            /* :......Ò...Ú.`¿. */
	$"0386 0000 0000 007B 0013 008D 015D 052A"            /* .Ü.....{...ç.].* */
	$"446F 6ED5 7420 6C6F 6164 204D 6978 6572"            /* Don’t load Mixer */
	$"2073 6574 7469 6E67 7320 6672 6F6D 206D"            /*  settings from m */
	$"7573 6963 2066 696C 6573"                           /* usic files */
};

data 'DITL' (182, purgeable) {
	$"0005 0000 0000 0008 0087 001C 00CB 0402"            /* .........á...À.. */
	$"4F4B 0000 0000 002D 0089 0041 00C9 0406"            /* OK.....-.â.A.….. */
	$"4361 6E63 656C 0000 0000 002D 0005 003D"            /* Cancel.....-...= */
	$"0032 8806 5370 6565 643A 0000 0000 000A"            /* .2à.Speed:.....¬ */
	$"000B 001A 0032 8806 5069 7463 683A 0000"            /* .....2à.Pitch:.. */
	$"0000 000A 003C 001A 006E 1005 3136 3030"            /* ...¬.<...n..1600 */
	$"3002 0000 0000 002D 003C 003D 006E 1005"            /* 0......-.<.=.n.. */
	$"3236 3030 3002"                                     /* 26000. */
};

data 'DITL' (421) {
	$"0005 0000 0000 0105 015C 0119 01C0 0408"            /* .........\...¿.. */
	$"436F 6E74 696E 7565 0000 0000 0001 0002"            /* Continue........ */
	$"00EE 0128 8000 0000 0000 0001 0139 00ED"            /* .Ó.(Ä........9.Ì */
	$"01E3 C002 0082 0000 0000 0073 012A 007C"            /* .„¿..Ç.....s.*.| */
	$"0135 0000 0000 0000 00F4 0062 0108 00C6"            /* .5.......Ù.b...∆ */
	$"040C 486F 7720 746F 204F 7264 6572 0000"            /* ..How to Order.. */
	$"0000 010F 0008 0123 012E 0421 476F 2074"            /* .......#...!Go t */
	$"6F20 506C 6179 6572 5052 4F20 4F66 6669"            /* o PlayerPRO Offi */
	$"6369 616C 2057 6562 2053 6974 6500"                 /* cial Web Site. */
};

data 'DITL' (684) {
	$"001F 0000 0000 0019 000C 07E9 00C8 0000"            /* ...........È.».. */
	$"0000 0000 001B 00CF 0028 0106 880B 496E"            /* .......œ.(..à.In */
	$"7374 7275 6D65 6E74 3A02 0000 0000 0039"            /* strument:......9 */
	$"00CF 0046 0101 8805 5369 7A65 3A02 0000"            /* .œ.F..à.Size:... */
	$"0000 0051 00DA 005E 0102 8806 5374 6172"            /* ...Q.⁄.^..à.Star */
	$"743A 0000 0000 005D 00DA 006A 0103 8805"            /* t:.....].⁄.j..à. */
	$"5369 7A65 3A02 0000 0000 0069 00CF 0076"            /* Size:......i.œ.v */
	$"0101 8807 566F 6C75 6D65 3A02 0000 0000"            /* ..à.Volume:..... */
	$"0075 00CF 0082 0111 8805 5261 7465 3A3A"            /* .u.œ.Ç..à.Rate:: */
	$"0000 0000 001B 0108 0028 0153 8800 0000"            /* .........(.Sà... */
	$"0000 0039 0108 0046 0153 8800 0000 0000"            /* ...9...F.Sà..... */
	$"0051 0108 005E 0153 8800 0000 0000 005D"            /* .Q...^.Sà......] */
	$"0108 006A 0153 8800 0000 0000 0069 0108"            /* ...j.Sà......i.. */
	$"0076 0153 8800 0000 0000 0075 0108 0082"            /* .v.Sà......u...Ç */
	$"0153 8800 0000 0000 0081 00CF 008E 0105"            /* .Sà......Å.œ.é.. */
	$"880A 5265 616C 204E 6F74 653A 0000 0000"            /* à¬Real Note:.... */
	$"0081 0108 008E 0153 8800 0000 0000 008D"            /* .Å...é.Sà......ç */
	$"00CF 009A 00F9 8805 4269 7473 3A02 0000"            /* .œ.ö.˘à.Bits:... */
	$"0000 008D 0108 009A 0153 8800 0000 0000"            /* ...ç...ö.Sà..... */
	$"0002 009F 0016 00B3 0000 0000 0000 0002"            /* ...ü...≥........ */
	$"0045 0016 0059 0000 0000 0000 0002 0009"            /* .E...Y.........∆ */
	$"0016 001D 0000 0000 0000 0002 0027 0016"            /* .............'.. */
	$"003B 0000 0000 0000 0002 0063 0016 0077"            /* .;.........c...w */
	$"0000 0000 0000 0002 0081 0016 0095 0000"            /* .........Å...ï.. */
	$"0000 0000 FFF9 00B9 0019 00D9 0000 0000"            /* ....ˇ˘.π...Ÿ.... */
	$"0000 0019 0000 02D5 000B 0000 0000 0000"            /* .......’........ */
	$"0045 00CF 0052 00F9 8805 4C6F 6F70 3A02"            /* .E.œ.R.˘à.Loop:. */
	$"0000 0000 0036 00C9 0037 0229 C002 000B"            /* .....6.….7.)¿... */
	$"0000 0000 0027 00CF 0034 0108 880B 4E6F"            /* .....'.œ.4..à.No */
	$"2073 616D 706C 6573 3A02 0000 0000 0027"            /*  samples:......' */
	$"0108 0034 0153 8800 0000 0000 00AA 00CA"            /* ...4.Sà......™.  */
	$"00CA 0130 8000 0000 0000 0099 00CF 00A6"            /* . .0Ä......ô.œ.¶ */
	$"00F9 8805 4D6F 6465 3A65 0000 0000 0099"            /* .˘à.Mode:e.....ô */
	$"0108 00A6 0153 8800"                                /* ...¶.Sà. */
};

data 'DITL' (202) {
	$"0027 0000 0000 0038 0000 0808 07D0 0000"            /* .'.....8.....–.. */
	$"0000 0000 000E 005F 001A 007F 8805 5369"            /* ......._....à.Si */
	$"7A65 3A02 0000 0000 000E 0080 001A 00C6"            /* ze:........Ä...∆ */
	$"8800 0000 0000 0001 00A9 000D 00E8 880A"            /* à........©...Ëà¬ */
	$"5365 6C65 6374 696F 6E3A 0000 0000 0001"            /* Selection:...... */
	$"00E9 000D 011B 8800 0000 0000 0001 011C"            /* .È....à......... */
	$"000D 0130 8803 746F 3A02 0000 0000 0001"            /* ...0à.to:....... */
	$"0131 000D 0163 8800 0000 0000 000E 00C7"            /* .1...cà........« */
	$"001A 00E8 8805 4C6F 6F70 3A02 0000 0000"            /* ...Ëà.Loop:..... */
	$"000E 00E9 001A 011B 8800 0000 0000 000E"            /* ...È....à....... */
	$"011C 001A 0130 8803 746F 3A02 0000 0000"            /* .....0à.to:..... */
	$"000E 0131 001A 0163 8800 0000 0000 0002"            /* ...1...cà....... */
	$"0003 000E 0047 080B 446F 2046 696C 7465"            /* .....G..Do Filte */
	$"7273 3A02 0000 0000 0029 0051 0036 0065"            /* rs:......).Q.6.e */
	$"0000 0000 0000 0002 0048 000F 005C 4002"            /* .........H...\@. */
	$"0384 0000 0000 001B 0065 0027 007E 8804"            /* .Ñ.......e.'.~à. */
	$"416D 703A 0000 0000 002A 001A 0036 004D"            /* Amp:.....*...6.M */
	$"8807 5361 6D70 6C65 3A02 0000 0000 001B"            /* à.Sample:....... */
	$"0080 0027 00B2 8800 0000 0000 0011 0048"            /* .Ä.'.≤à........H */
	$"0025 005C 0000 0000 0000 001B 00B5 0027"            /* .%.\.........µ.' */
	$"00E8 8808 4469 7370 6C61 793A 0000 0000"            /* .Ëà.Display:.... */
	$"001B 00E9 0027 011B 8800 0000 0000 001B"            /* ...È.'..à....... */
	$"011C 0027 0130 8803 746F 3A02 0000 0000"            /* ...'.0à.to:..... */
	$"001B 0131 0027 0163 8800 0000 0000 0001"            /* ...1.'.cà....... */
	$"005F 000D 007F 8805 5A6F 6F6D 3A02 0000"            /* ._....à.Zoom:... */
	$"0000 0001 0080 000D 00A8 8800 0000 0000"            /* .....Ä...®à..... */
	$"0011 0003 0025 0017 0000 0000 0000 0011"            /* .....%.......... */
	$"0034 0025 0048 0000 0000 0000 0011 0020"            /* .4.%.H.........  */
	$"0025 0034 0000 0000 0000 0029 0003 0036"            /* .%.4.......)...6 */
	$"0017 4002 0384 0000 0000 0027 FFFD 0028"            /* ..@..Ñ.....'ˇ˝.( */
	$"07CD C002 000B 0000 0000 002A 00AE 0037"            /* .Õ¿........*.Æ.7 */
	$"1FEE 8000 0000 0000 0001 0165 000D 01B6"            /* .ÓÄ........e...∂ */
	$"880D 5261 7465 2028 6334 7370 6429 3A02"            /* à.Rate (c4spd):. */
	$"0000 0000 0001 01B7 000D 01E9 8800 0000"            /* .......∑...Èà... */
	$"0000 000E 01B7 001A 01EE 8800 0000 0000"            /* .....∑...Óà..... */
	$"000E 0177 001A 01B6 880A 5265 616C 204E"            /* ...w...∂à¬Real N */
	$"6F74 653A 0000 0000 0099 02D6 00B9 02F6"            /* ote:.....ô.÷.π.ˆ */
	$"8000 0000 0000 0029 0068 0036 007C 0000"            /* Ä......).h.6.|.. */
	$"0000 0000 0029 007F 0036 0093 0000 0000"            /* .....)...6.ì.... */
	$"0000 001B 01B7 0027 0225 8800 0000 0000"            /* .....∑.'.%à..... */
	$"001B 0165 0027 01B6 880D 582F 5920 506F"            /* ...e.'.∂à.X/Y Po */
	$"7369 7469 6F6E 3A00 0000 0000 0029 0096"            /* sition:......).ñ */
	$"0036 00AA 0000"                                     /* .6.™.. */
};

data 'DITL' (148) {
	$"0015 0000 0000 0005 0118 0019 0152 0402"            /* .............R.. */
	$"4F4B 0000 0000 0023 0118 0037 0152 0406"            /* OK.....#...7.R.. */
	$"4361 6E63 656C 0000 0000 0014 005A 0021"            /* Cancel.......Z.! */
	$"010D 1000 0000 0000 0014 000E 0021 0051"            /* .............!.Q */
	$"880E 496E 7465 726E 616C 206E 616D 653A"            /* à.Internal name: */
	$"0000 0000 0028 000D 0035 0051 880E 4465"            /* .....(...5.Qà.De */
	$"6661 756C 7420 7465 6D70 6F3A 0000 0000"            /* fault tempo:.... */
	$"003C 000F 0049 0051 880E 4465 6661 756C"            /* .<...I.Qà.Defaul */
	$"7420 7370 6565 643A 0000 0000 0028 005A"            /* t speed:.....(.Z */
	$"0035 0096 1000 0000 0000 003C 005A 0049"            /* .5.ñ.......<.Z.I */
	$"0096 1000 0000 0000 0050 0007 0060 0051"            /* .ñ.......P...`.Q */
	$"880F 436F 7079 7269 6768 7420 5465 7874"            /* à.Copyright Text */
	$"3A02 0000 0000 0050 005A 0084 010D 1000"            /* :......P.Z.Ñ.... */
	$"0000 0000 0028 00A0 0038 00EB 8803 4250"            /* .....(.†.8.Îà.BP */
	$"4D02 0000 0000 003C 00A0 004C 010E 8816"            /* M......<.†.L..à. */
	$"5469 6D69 6E67 2070 756C 7365 7320 7065"            /* Timing pulses pe */
	$"7220 6C69 6E65 0000 0000 0000 002C 000D"            /* r line.......,.. */
	$"0051 8807 5472 6163 6B73 3A02 0000 0000"            /* .Qà.Tracks:..... */
	$"0000 005A 000D 007A 8800 0000 0000 008B"            /* ...Z...zà......ã */
	$"005A 0098 014E 051B 5368 6F77 2043 6F70"            /* .Z.ò.N..Show Cop */
	$"7972 6967 6874 2077 6865 6E20 6F70 656E"            /* yright when open */
	$"696E 6702 0000 0000 0000 007F 000D 0093"            /* ing............ì */
	$"4002 0385 0000 0000 00DD 006B 00ED 00FF"            /* @..Ö.....›.k.Ì.ˇ */
	$"8814 4D61 7820 6D69 7869 6E67 2063 6861"            /* à.Max mixing cha */
	$"6E6E 656C 733A 0000 0000 00A0 005A 00AD"            /* nnels:.....†.Z.≠ */
	$"00FF 0514 4F6C 6420 4D4F 4473 2070 6974"            /* .ˇ..Old MODs pit */
	$"6368 2074 6162 6C65 0000 0000 00B5 005A"            /* ch table.....µ.Z */
	$"00C2 014E 050C 4C69 6E65 6172 2054 6162"            /* .¬.N..Linear Tab */
	$"6C65 0000 0000 00CA 005A 00D7 011D 0514"            /* le..... .Z.◊.... */
	$"4D75 6C74 692D 4368 616E 6E65 6C20 5472"            /* Multi-Channel Tr */
	$"6163 6B73 0000 0000 00DF 0124 00EC 0138"            /* acks.....ﬂ.$.Ï.8 */
	$"4002 0385 0000 0000 00DF 00FF 00EC 011F"            /* @..Ö.....ﬂ.ˇ.Ï.. */
	$"8800"                                               /* à. */
};

data 'DITL' (183, purgeable) {
	$"0028 0000 0000 0003 007D 0012 00A4 0000"            /* .(.......}...§.. */
	$"0000 0000 001C 0030 002C 00B2 0000 0000"            /* .......0.,.≤.... */
	$"0000 0030 0030 0040 00B2 0000 0000 0000"            /* ...0.0.@.≤...... */
	$"0044 0030 0054 00B2 0000 0000 0000 0058"            /* .D.0.T.≤.......X */
	$"0030 0068 00B2 0000 0000 0000 006C 0030"            /* .0.h.≤.......l.0 */
	$"007C 00B2 0000 0000 0000 0080 0030 0090"            /* .|.≤.......Ä.0.ê */
	$"00B2 0000 0000 0000 0094 0030 00A4 00B2"            /* .≤.......î.0.§.≤ */
	$"0000 0000 0000 00A8 0030 00B8 00B2 0000"            /* .......®.0.∏.≤.. */
	$"0000 0000 00CE 000B 01CE 010B 0000 0000"            /* .....Œ...Œ...... */
	$"0000 016B 015E 017B 01E0 0000 0000 0000"            /* ...k.^.{.‡...... */
	$"017F 015E 018F 01E0 0000 0000 0000 0193"            /* ...^.è.‡.......ì */
	$"015E 01A3 01E0 0000 0000 0000 0002 0025"            /* .^.£.‡.........% */
	$"0016 0039 0000 0000 0000 0002 0043 0016"            /* ...9.........C.. */
	$"0057 0000 0000 0000 0002 0061 0016 0075"            /* .W.........a...u */
	$"0000 0000 0000 0002 0007 0016 001B 0000"            /* ................ */
	$"0000 0000 001C 0003 002C 002F 880B 5374"            /* .........,./à.St */
	$"6174 6963 2054 6578 7402 0000 0000 0030"            /* atic Text......0 */
	$"0003 0040 002F 880B 5374 6174 6963 2054"            /* ...@./à.Static T */
	$"6578 7402 0000 0000 0044 0003 0054 002F"            /* ext......D...T./ */
	$"880B 5374 6174 6963 2054 6578 7402 0000"            /* à.Static Text... */
	$"0000 0058 0003 0068 002F 880B 5374 6174"            /* ...X...h./à.Stat */
	$"6963 2054 6578 7402 0000 0000 006C 0003"            /* ic Text......l.. */
	$"007C 002F 880B 5374 6174 6963 2054 6578"            /* .|./à.Static Tex */
	$"7402 0000 0000 0080 0003 0090 002F 880B"            /* t......Ä...ê./à. */
	$"5374 6174 6963 2054 6578 7402 0000 0000"            /* Static Text..... */
	$"0094 0003 00A4 002F 880B 5374 6174 6963"            /* .î...§./à.Static */
	$"2054 6578 7402 0000 0000 00A8 0003 00B8"            /*  Text......®...∏ */
	$"002F 880B 5374 6174 6963 2054 6578 7402"            /* ./à.Static Text. */
	$"0000 0000 0157 0131 0167 015D 880B 5374"            /* .....W.1.g.]à.St */
	$"6174 6963 2054 6578 7402 0000 0000 016B"            /* atic Text......k */
	$"0131 017B 015D 880B 5374 6174 6963 2054"            /* .1.{.]à.Static T */
	$"6578 7402 0000 0000 017F 0131 018F 015D"            /* ext........1.è.] */
	$"880B 5374 6174 6963 2054 6578 7402 0000"            /* à.Static Text... */
	$"0000 0193 0131 01A3 015D 880B 5374 6174"            /* ...ì.1.£.]à.Stat */
	$"6963 2054 6578 7402 0000 0000 001C 00B4"            /* ic Text........¥ */
	$"002C 00D5 880B 5374 6174 6963 2054 6578"            /* .,.’à.Static Tex */
	$"7402 0000 0000 0030 00B4 0040 00D5 880B"            /* t......0.¥.@.’à. */
	$"5374 6174 6963 2054 6578 7402 0000 0000"            /* Static Text..... */
	$"0044 00B4 0054 00D5 880B 5374 6174 6963"            /* .D.¥.T.’à.Static */
	$"2054 6578 7402 0000 0000 0058 00B4 0068"            /*  Text......X.¥.h */
	$"00D5 880B 5374 6174 6963 2054 6578 7402"            /* .’à.Static Text. */
	$"0000 0000 006C 00B4 007C 00D5 880B 5374"            /* .....l.¥.|.’à.St */
	$"6174 6963 2054 6578 7402 0000 0000 0080"            /* atic Text......Ä */
	$"00B4 0090 00D5 880B 5374 6174 6963 2054"            /* .¥.ê.’à.Static T */
	$"6578 7402 0000 0000 0094 00B4 00A4 00D5"            /* ext......î.¥.§.’ */
	$"880B 5374 6174 6963 2054 6578 7402 0000"            /* à.Static Text... */
	$"0000 00A8 00B4 00B8 00D5 880B 5374 6174"            /* ...®.¥.∏.’à.Stat */
	$"6963 2054 6578 7402 0000 0000 0157 01E2"            /* ic Text......W.‚ */
	$"0167 0203 880B 5374 6174 6963 2054 6578"            /* .g..à.Static Tex */
	$"7402 0000 0000 016B 01E2 017B 0203 880B"            /* t......k.‚.{..à. */
	$"5374 6174 6963 2054 6578 7402 0000 0000"            /* Static Text..... */
	$"017F 01E2 018F 0203 880B 5374 6174 6963"            /* ...‚.è..à.Static */
	$"2054 6578 7402 0000 0000 0193 01E2 01A3"            /*  Text......ì.‚.£ */
	$"0203 880B 5374 6174 6963 2054 6578 748B"            /* ..à.Static Textã */
};

data 'DITL' (141, "Adaptator") {
	$"002B 0000 0000 0071 01E4 007D 021A 8000"            /* .+.....q.‰.}..Ä. */
	$"0000 0000 0068 002D 0078 006D 0000 0000"            /* .....h.-.x.m.... */
	$"0000 0013 01CA 0023 01F9 8000 0000 0000"            /* ..... .#.˘Ä..... */
	$"0066 019E 0076 01C6 0800 0000 0000 002B"            /* .f.û.v.∆.......+ */
	$"01CA 003B 01F9 8000 0000 0000 0077 014E"            /* . .;.˘Ä......w.N */
	$"0084 017B 8000 0000 0000 004D 0188 005D"            /* .Ñ.{Ä......M.à.] */
	$"01AB 8804 4250 4D3A 0000 0000 002D 00D2"            /* .´à.BPM:.....-.“ */
	$"003D 010E 0000 0000 0000 005B 01CA 006B"            /* .=.........[. .k */
	$"01F9 8000 0000 0000 001B 00D2 002B 010E"            /* .˘Ä........“.+.. */
	$"0000 0000 0000 00CF 019E 00DF 01CD 8000"            /* .......œ.û.ﬂ.ÕÄ. */
	$"0000 0000 001B 001A 002B 009C 0000 0000"            /* .........+.ú.... */
	$"0000 0070 019D 0080 01AD C002 0081 0000"            /* ...p.ù.Ä.≠¿..Å.. */
	$"0000 002D 001A 003D 009C 0000 0000 0000"            /* ...-...=.ú...... */
	$"00AB 019E 00BB 01CD 8000 0000 0000 0066"            /* .´.û.ª.ÕÄ......f */
	$"0001 1006 012D 0000 0000 0000 0002 0025"            /* .....-.........% */
	$"0016 0039 0000 0000 0000 003B 0224 004B"            /* ...9.......;.$.K */
	$"0244 0000 0000 0000 0042 014D 0076 016D"            /* .D.......B.M.v.m */
	$"8000 0000 0000 002C 00C1 003C 00D1 C002"            /* Ä......,.¡.<.—¿. */
	$"0081 0000 0000 00D3 0167 00E1 0175 8000"            /* .Å.....”.g.·.uÄ. */
	$"0000 0000 002E 009D 0042 00B1 0000 0000"            /* .......ù.B.±.... */
	$"0000 001C 009D 0030 00B1 0000 0000 0000"            /* .....ù.0.±...... */
	$"00A8 0179 00B8 01DD 0702 0098 0000 0000"            /* .®.y.∏.›...ò.... */
	$"0002 0007 0016 001B 0000 0000 0000 0002"            /* ................ */
	$"0043 0016 0057 0000 0000 0000 000E 0174"            /* .C...W.........t */
	$"001A 01A0 8000 0000 0000 001A 00C1 002A"            /* ...†Ä........¡.* */
	$"00D1 C002 0081 0000 0000 001B 0005 002B"            /* .—¿..Å.........+ */
	$"0015 C002 008A 0000 0000 002B 0005 003B"            /* ..¿..ä.....+...; */
	$"0015 C002 008B 0000 0000 000C 01A0 001C"            /* ..¿..ã.......†.. */
	$"01B0 8000 0000 0000 000C 0163 001C 0173"            /* .∞Ä........c...s */
	$"8000 0000 0000 0029 0229 0035 0255 8800"            /* Ä......).).5.Uà. */
	$"0000 0000 0056 0037 0062 0060 8806 566F"            /* .....V.7.b.`à.Vo */
	$"6C75 6D65 0000 0000 0056 0086 0062 00B2"            /* lume.....V.Ü.b.≤ */
	$"8807 5061 6E6E 696E 6702 0000 0000 0055"            /* à.Panning......U */
	$"FFDC 0056 016C C002 0089 0000 0000 001C"            /* ˇ‹.V.l¿..â...... */
	$"00A8 0027 00C0 C002 0086 0000 0000 0056"            /* .®.'.¿¿..Ü.....V */
	$"0001 0062 002F 8808 5472 6163 6B20 4944"            /* ...b./à.Track ID */
	$"0000 0000 002E 00A8 0039 00C0 C002 0099"            /* .......®.9.¿¿..ô */
	$"0000 0000 0004 00CE 0014 0102 8800 0000"            /* .......Œ....à... */
	$"0000 0002 005F 0016 0073 0000 0000 0000"            /* ....._...s...... */
	$"0041 0002 004B 0017 C002 009D 0000 0000"            /* .A...K..¿..ù.... */
	$"003F 001A 004F 009C 0000 0000 0000 0040"            /* .?...O.ú.......@ */
	$"009D 0054 00B1 0000"                                /* .ù.T.±.. */
};

data 'DITL' (184, purgeable) {
	$"0008 0000 0000 0007 0006 001B 004A 0402"            /* .............J.. */
	$"4F4B 0000 0000 002A 0008 003E 0048 0406"            /* OK.....*...>.H.. */
	$"4361 6E63 656C 0000 0000 0004 0059 0104"            /* Cancel.......Y.. */
	$"0159 0000 0000 0000 0081 0008 0095 0048"            /* .Y.......Å...ï.H */
	$"0404 506C 6179 0000 0000 0055 0008 0069"            /* ..Play.....U...i */
	$"0048 0405 5265 7365 7402 0000 0000 00F4"            /* .H..Reset......Ù */
	$"0006 0104 0052 0507 5072 6576 6965 7702"            /* .....R..Preview. */
	$"0000 0000 00C7 0006 00D7 0052 0606 4C69"            /* .....«...◊.R..Li */
	$"6E65 6172 0000 0000 00D7 0006 00E7 0052"            /* near.....◊...Á.R */
	$"0603 4C6F 6702 0000 0000 00B7 0006 00C6"            /* ..Log......∑...∆ */
	$"0034 8805 4D6F 6465 3A02"                           /* .4à.Mode:. */
};

data 'DITL' (146) {
	$"003C 0000 0000 000A 0144 001E 017E 0402"            /* .<.....¬.D...~.. */
	$"4F4B 0000 0000 000A 0186 001E 01C0 0406"            /* OK.....¬.Ü...¿.. */
	$"4361 6E63 656C 0000 0000 0014 0038 0021"            /* Cancel.......8.! */
	$"00F6 1005 696A 696A 0D02 0000 0000 00BA"            /* .ˆ..ijij.......∫ */
	$"00F4 00C7 011C 1000 0000 0000 00BB 0162"            /* .Ù.«.........ª.b */
	$"00C8 018A 1000 0000 0000 00CD 0053 00DA"            /* .».ä.......Õ.S.⁄ */
	$"007B 1000 0000 0000 00E1 0053 00EE 007B"            /* .{.......·.S.Ó.{ */
	$"1000 0000 0000 0014 0013 0021 0035 8805"            /* ...........!.5à. */
	$"4E61 6D65 3A02 0000 0000 00BA 00B8 00C7"            /* Name:......∫.∏.« */
	$"00F1 880B 4C6F 6F70 2053 7461 7274 3A02"            /* .Òà.Loop Start:. */
	$"0000 0000 00BB 012B 00C8 015F 880A 4C6F"            /* .....ª.+.»._à¬Lo */
	$"6F70 2053 697A 653A 0000 0000 00CD 000D"            /* op Size:.....Õ.. */
	$"00DA 0050 880D 5261 7465 2028 6334 7370"            /* .⁄.Pà.Rate (c4sp */
	$"6429 3A02 0000 0000 00E1 000E 00EE 0050"            /* d):......·...Ó.P */
	$"880C 4261 7365 2056 6F6C 756D 653A 0000"            /* à.Base Volume:.. */
	$"0000 0090 0006 009D 01C4 8000 0000 0000"            /* ...ê...ù.ƒÄ..... */
	$"013C 019F 0149 01C4 8000 0000 0000 0069"            /* .<.ü.I.ƒÄ......i */
	$"0038 0076 014B 0620 4368 616E 6E65 6C20"            /* .8.v.K. Channel  */
	$"5661 6C75 6520 2853 6565 204D 6978 6572"            /* Value (See Mixer */
	$"2077 696E 646F 7729 0000 0000 002E 000B"            /*  window)........ */
	$"003B 0035 0807 566F 6C75 6D65 3A02 0000"            /* .;.5..Volume:... */
	$"0000 00E1 008E 00EE 00A3 8000 0000 0000"            /* ...·.é.Ó.£Ä..... */
	$"00E1 00F4 00EE 010E 1000 0000 0000 00E1"            /* .·.Ù.Ó.........· */
	$"00BC 00EE 00F1 880A 5265 616C 204E 6F74"            /* .º.Ó.Òà¬Real Not */
	$"653A 0000 0000 00B9 0019 00C6 0050 880A"            /* e:.....π...∆.Pà¬ */
	$"416D 706C 6974 7564 653A 0000 0000 00B9"            /* Amplitude:.....π */
	$"0053 00C6 0067 8802 3136 0000 0000 0052"            /* .S.∆.gà.16.....R */
	$"0120 005F 0181 050B 4669 7865 6420 5370"            /* . ._.Å..Fixed Sp */
	$"6565 6402 0000 0000 00F8 0000 00F9 0258"            /* eed......¯...˘.X */
	$"C002 000B 0000 0000 00B9 008E 00C6 00A2"            /* ¿........π.é.∆.¢ */
	$"4002 0385 0000 0000 0002 0038 000F 00F6"            /* @..Ö.......8...ˆ */
	$"8000 0000 0000 013D 0003 014A 015C 884B"            /* Ä......=...J.\àK */
	$"5361 6D70 6C65 2073 6574 7469 6E67 733A"            /* Sample settings: */
	$"2063 6C69 636B 202B 206F 7074 696F 6E20"            /*  click + option  */
	$"6B65 7920 746F 2063 6861 6E67 6520 6173"            /* key to change as */
	$"7369 676E 6174 696F 6E20 6F66 2063 7572"            /* signation of cur */
	$"7265 6E74 2073 616D 706C 6502 0000 0000"            /* rent sample..... */
	$"00E1 0113 00EE 0143 8800 0000 0000 00B9"            /* .·...Ó.Cà......π */
	$"006C 00C6 0083 8804 6269 7473 0000 0000"            /* .l.∆.Éà.bits.... */
	$"00FF 0003 0139 01C4 4002 00E1 0000 0000"            /* .ˇ...9.ƒ@..·.... */
	$"015F 0103 016C 0114 8802 3939 0000 0000"            /* ._...l..à.99.... */
	$"008B FFF4 008C 024C C002 000B 0000 0000"            /* .ãˇÙ.å.L¿....... */
	$"002E 00BB 003B 00DD 1000 0000 0000 0052"            /* ...ª.;.›.......R */
	$"0038 005F 008E 0609 456E 7665 6C6F 7065"            /* .8._.é.∆Envelope */
	$"3A02 0000 0000 00CD 0080 00DA 008D 0802"            /* :......Õ.Ä.⁄.ç.. */
	$"487A 0000 0000 0040 0038 004D 015B 061E"            /* Hz.....@.8.M.[.. */
	$"5361 6D70 6C65 2042 6173 6520 566F 6C75"            /* Sample Base Volu */
	$"6D65 2028 5365 6520 6265 6C6F 7729 0000"            /* me (See below).. */
	$"0000 0052 00C6 005F 011C 050A 5375 7374"            /* ...R.∆._...¬Sust */
	$"6169 6E20 7074 0000 0000 00CD 008E 00DA"            /* ain pt.....Õ.é.⁄ */
	$"00A2 4002 0385 0000 0000 00A1 0196 00B1"            /* .¢@..Ö.....°.ñ.± */
	$"01B0 0000 0000 0000 0052 008E 005F 00C6"            /* .∞.......R.é._.∆ */
	$"0504 4C6F 6F70 0000 0000 007B 008E 0088"            /* ..Loop.....{.é.à */
	$"00C6 0504 4C6F 6F70 0000 0000 007C 0120"            /* .∆..Loop.....|.  */
	$"0089 0183 050B 4669 7865 6420 5370 6565"            /* .â.É..Fixed Spee */
	$"6402 0000 0000 0068 0009 0075 0035 8808"            /* d......h.∆.u.5à. */
	$"5061 6E6E 696E 673A 0000 0000 007B 00C6"            /* Panning:.....{.∆ */
	$"0088 0120 050A 5375 7374 6169 6E20 7074"            /* .à. .¬Sustain pt */
	$"0000 0000 009E 01B4 00F6 01C4 0000 0000"            /* .....û.¥.ˆ.ƒ.... */
	$"0000 00A6 00F4 00B3 018A 1000 0000 0000"            /* ...¶.Ù.≥.ä...... */
	$"00A6 00CF 00B3 00F1 8805 4E61 6D65 3A02"            /* .¶.œ.≥.Òà.Name:. */
	$"0000 0000 002E 0039 003B 00B8 0819 566F"            /* .......9.;.∏..Vo */
	$"6C75 6D65 2046 6164 654F 7574 2028 4B65"            /* lume FadeOut (Ke */
	$"7920 4F46 4629 3A02 0000 0000 007B 0038"            /* y OFF):......{.8 */
	$"0088 008D 0609 456E 7665 6C6F 7065 3A02"            /* .à.ç.∆Envelope:. */
	$"0000 0000 0028 FFEC 0029 0244 C002 000B"            /* .....(ˇÏ.).D¿... */
	$"0000 0000 0062 FFEF 0063 0247 C002 000B"            /* .....bˇÔ.c.G¿... */
	$"0000 0000 00A6 002F 00B3 0050 8805 4D6F"            /* .....¶./.≥.Pà.Mo */
	$"6465 3A02 0000 0000 00A6 0053 00B3 007E"            /* de:......¶.S.≥.~ */
	$"8800 0000 0000 00A6 008E 00B3 00A2 4002"            /* à......¶.é.≥.¢@. */
	$"0385 0000 0000 00CD 00BA 00DA 00F1 880A"            /* .Ö.....Õ.∫.⁄.Òà¬ */
	$"4C6F 6F70 2054 7970 653A 0000 0000 00CD"            /* Loop Type:.....Õ */
	$"00F4 00DA 0125 8800 0000 0000 00CD 0128"            /* .Ù.⁄.%à......Õ.( */
	$"00DA 013C 4002 0385 0000 0000 0183 0098"            /* .⁄.<@..Ö.....É.ò */
	$"0190 00AC 8804 6269 7473 0000 0000 0030"            /* .ê.¨à.bits.....0 */
	$"0184 0044 01C2 0408 5072 6576 696F 7573"            /* .Ñ.D.¬..Previous */
	$"0000 0000 004A 0184 005E 01C2 0404 4E65"            /* .....J.Ñ.^.¬..Ne */
	$"7874 0000 0000 0050 000A 0060 0032 0404"            /* xt.....P.¬.`.2.. */
	$"4564 6974 0000 0000 0079 000A 0089 0032"            /* Edit.....y.¬.â.2 */
	$"0404 4564 6974"                                     /* ..Edit */
};

data 'DITL' (143, "Controls") {
	$"002C 0000 0000 0024 0001 002F 00B5 0000"            /* .,.....$.../.µ.. */
	$"0000 0000 0001 0007 0019 001F 0000 0000"            /* ................ */
	$"0000 0001 0020 0019 0038 0000 0000 0000"            /* ..... ...8...... */
	$"0001 0052 0019 006A 0000 0000 0000 0001"            /* ...R...j........ */
	$"006B 0019 0083 0000 0000 0000 0001 0084"            /* .k...É.........Ñ */
	$"0019 009C 0000 0000 0000 0001 009D 0019"            /* ...ú.........ù.. */
	$"00B5 0000 0000 0000 0017 0002 0023 0023"            /* .µ...........#.# */
	$"8805 3030 3A30 3002 0000 0000 0017 0093"            /* à.00:00........ì */
	$"0023 00B5 8805 3030 3A30 3002 0000 0000"            /* .#.µà.00:00..... */
	$"0016 0024 0024 0091 0000 0000 0000 002A"            /* ...$.$.ë.......* */
	$"0063 003A 0073 0000 0000 0000 0032 0036"            /* .c.:.s.......2.6 */
	$"003E 004F 8804 506F 733A 0000 0000 0032"            /* .>.Oà.Pos:.....2 */
	$"0001 003E 001A 8804 5061 743A 0000 0000"            /* ...>..à.Pat:.... */
	$"0032 0074 003E 0099 8806 5472 6163 6B3A"            /* .2.t.>.ôà.Track: */
	$"0000 0000 0032 001A 003E 0029 8802 3939"            /* .....2...>.)à.99 */
	$"0000 0000 0032 004F 003E 0063 8803 3634"            /* .....2.O.>.cà.64 */
	$"3202 0000 0000 0032 0099 003E 00A8 8802"            /* 2......2.ô.>.®à. */
	$"3939 0000 0000 0042 009F 004F 00B3 4002"            /* 99.....B.ü.O.≥@. */
	$"0385 0000 0000 0054 009F 0061 00B3 4002"            /* .Ö.....T.ü.a.≥@. */
	$"0385 0000 0000 0079 0046 0089 007D 8809"            /* .Ö.....y.F.â.}à∆ */
	$"4172 6775 6D65 6E74 3A02 0000 0000 0057"            /* Argument:......W */
	$"0006 006B 001A 0000 0000 0000 0040 0006"            /* ...k.........@.. */
	$"0054 001A 0000 0000 0000 0034 00A8 003C"            /* .T.........4.®.< */
	$"00B3 4002 008C 0000 0000 0034 002A 003C"            /* .≥@..å.....4.*.< */
	$"0034 4002 008C 0000 0000 0037 0063 0047"            /* .4@..å.....7.c.G */
	$"0073 0000 0000 0000 0078 009F 0085 00B3"            /* .s.......x.ü.Ö.≥ */
	$"4002 0385 0000 0000 003E FFFF 003F 00B7"            /* @..Ö.....>ˇˇ.?.∑ */
	$"C002 000B 0000 0000 0055 005E 0065 007D"            /* ¿........U.^.e.} */
	$"8805 4E6F 7465 3A02 0000 0000 0043 003A"            /* à.Note:......C.: */
	$"0053 007D 880B 496E 7374 7275 6D65 6E74"            /* .S.}à.Instrument */
	$"3A02 0000 0000 0067 0052 0077 007D 8807"            /* :......g.R.w.}à. */
	$"4566 6665 6374 3A02 0000 0000 0043 007F"            /* Effect:......C.. */
	$"004F 009C 0000 0000 0000 0055 007F 0061"            /* .O.ú.......U...a */
	$"009C 0000 0000 0000 0067 007F 0073 009C"            /* .ú.......g...s.ú */
	$"0000 0000 0000 0079 007F 0085 009C 0000"            /* .......y...Ö.ú.. */
	$"0000 0000 0067 0046 0074 0050 0000 0000"            /* .....g.F.t.P.... */
	$"0000 FFFE 00B7 00C6 00B8 C002 000B 0000"            /* ..ˇ˛.∑.∆.∏¿..... */
	$"0000 0001 00BA 0087 01C2 0000 0000 0000"            /* .....∫.á.¬...... */
	$"0030 FFFF 0031 00B7 C002 000B 0000 0000"            /* .0ˇˇ.1.∑¿....... */
	$"008B 0052 009B 007D 8807 566F 6C75 6D65"            /* .ã.R.õ.}à.Volume */
	$"3AFC 0000 0000 006E 0006 0082 001A 0000"            /* :¸.....n...Ç.... */
	$"0000 0000 0066 009F 0073 00B3 4002 0385"            /* .....f.ü.s.≥@..Ö */
	$"0000 0000 008B 007F 0097 009C 0000 0000"            /* .....ã...ó.ú.... */
	$"0000 008A 009F 0097 00B3 4002 0385 0000"            /* ...ä.ü.ó.≥@..Ö.. */
	$"0000 0001 0039 0019 0051 0000 0000 0000"            /* .....9...Q...... */
	$"0054 0050 0061 005A 0000"                           /* .T.P.a.Z.. */
};

data 'DITL' (142, "Music list") {
	$"0020 0000 0000 0031 026F 0041 02BA 8000"            /* . .....1.o.A.∫Ä. */
	$"0000 0000 0019 0000 07E9 00C8 0000 0000"            /* .........È.».... */
	$"0000 0015 024F 0029 0289 0000 0000 0000"            /* .....O.).â...... */
	$"0020 023E 0030 0289 8000 0000 0000 001D"            /* . .>.0.âÄ....... */
	$"00CE 002A 010B 880A 4669 6C65 204E 616D"            /* .Œ.*..à¬File Nam */
	$"653A 0000 0000 002C 00CE 0039 0113 880E"            /* e:.....,.Œ.9..à. */
	$"496E 7465 726E 616C 204E 616D 653A 0000"            /* Internal Name:.. */
	$"0000 0068 00CE 0075 010A 8809 5061 7474"            /* ...h.Œ.u.¬à∆Patt */
	$"6572 6E73 3A02 0000 0000 004A 00CE 0057"            /* erns:......J.Œ.W */
	$"010C 880A 5369 676E 6174 7572 653A 0000"            /* ..à¬Signature:.. */
	$"0000 0077 00CD 0084 0108 880A 506C 7567"            /* ...w.Õ.Ñ..à¬Plug */
	$"2054 7970 653A 0000 0000 0059 00CE 0066"            /*  Type:.....Y.Œ.f */
	$"010D 880C 496E 7374 7275 6D65 6E74 733A"            /* ..à.Instruments: */
	$"0000 0000 003B 00CE 0048 0107 8805 5369"            /* .....;.Œ.H..à.Si */
	$"7A65 3A02 0000 0000 001D 0114 002A 019F"            /* ze:..........*.ü */
	$"8801 2D02 0000 0000 002C 0114 0039 019F"            /* à.-......,...9.ü */
	$"0801 2D02 0000 0000 0068 0114 0075 0199"            /* ..-......h...u.ô */
	$"8801 2D02 0000 0000 004A 0114 0057 0196"            /* à.-......J...W.ñ */
	$"8801 2D02 0000 0000 0077 0114 0084 019A"            /* à.-......w...Ñ.ö */
	$"8801 2D02 0000 0000 0059 0114 0066 0197"            /* à.-......Y...f.ó */
	$"8801 2D02 0000 0000 003B 0114 0048 0195"            /* à.-......;...H.ï */
	$"8801 2D02 0000 0000 0002 009D 0016 00B1"            /* à.-........ù...± */
	$"0000 0000 0000 0144 00E8 0158 0122 0702"            /* .......D.Ë.X.".. */
	$"00C8 0000 0000 0035 01E4 0045 024A 8000"            /* .».....5.‰.E.JÄ. */
	$"0000 0000 0045 01E4 0055 027E 8000 0000"            /* .....E.‰.U.~Ä... */
	$"0000 001A 00C9 012F 01DD 8000 0000 0000"            /* .....…./.›Ä..... */
	$"0016 01FC 0037 02B0 8000 0000 0000 0086"            /* ...¸.7.∞Ä......Ü */
	$"00CD 0093 00F8 8806 5768 6572 653A 0000"            /* .Õ.ì.¯à.Where:.. */
	$"0000 0086 0114 0117 01A9 8801 2D80 0000"            /* ...Ü.....©à.-Ä.. */
	$"0000 0002 0061 0016 0075 0000 0000 0000"            /* .....a...u...... */
	$"0002 007F 0016 0093 0000 0000 0000 0002"            /* .......ì........ */
	$"0025 0016 0039 0000 0000 0000 FFF9 00B9"            /* .%...9......ˇ˘.π */
	$"0019 00D9 0000 0000 0000 0002 0043 0016"            /* ...Ÿ.........C.. */
	$"0057 0000 0000 0000 0002 0007 0016 001B"            /* .W.............. */
	$"0702 00C8 0000 0000 009F 00E1 00BF 0101"            /* ...».....ü.·.ø.. */
	$"8000"                                               /* Ä. */
};

data 'ALRT' (131) {
	$"0028 0028 008A 0116 03A6 4444 300A"                 /* .(.(.ä...¶DD0¬ */
};

data 'ALRT' (128, "General Error") {
	$"006A 0056 00DD 01AD 010C 4444 680A"                 /* .j.V.›.≠..DDh¬ */
};

data 'ALRT' (129, "Info") {
	$"006A 0056 00D9 01AD 01C9 4444 680A"                 /* .j.V.Ÿ.≠.…DDh¬ */
};

data 'ALRT' (300, purgeable) {
	$"0050 006E 00C7 0183 012C 4444 680A"                 /* .P.n.«.É.,DDh¬ */
};

data 'ALRT' (132, "Look for music", purgeable) {
	$"0050 006E 00D0 0186 012D 4444 680A"                 /* .P.n.–.Ü.-DDh¬ */
};

data 'ALRT' (133, "Minimize", purgeable) {
	$"0050 006E 00D2 01C5 012E 4444 680A"                 /* .P.n.“.≈..DDh¬ */
};

data 'ALRT' (130, "FATAL ERROR") {
	$"0028 0028 015A 0134 0082 5555 280A"                 /* .(.(.Z.4.ÇUU(¬ */
};

data 'PKEY' (128) {
	$"000F 1354 6F6F 6C73 2057 696E 646F 7720"            /* ...Tools Window  */
	$"2D20 506C 6179 0000 0008 0000 0005 1454"            /* - Play.........T */
	$"6F6F 6C73 2057 696E 646F 7720 2D20 5061"            /* ools Window - Pa */
	$"7573 6500 0000 0800 0000 0415 546F 6F6C"            /* use.........Tool */
	$"7320 5769 6E64 6F77 202D 2052 6563 6F72"            /* s Window - Recor */
	$"6400 0000 0800 0000 2C02 282D 0000 0000"            /* d.......,.(-.... */
	$"0000 0000 2054 6F6F 6C73 2057 696E 646F"            /* .... Tools Windo */
	$"7720 2D20 5365 6C65 6374 2069 6E73 7472"            /* w - Select instr */
	$"756D 656E 7400 0000 0800 0000 1F1A 546F"            /* ument.........To */
	$"6F6C 7320 5769 6E64 6F77 202D 2053 656C"            /* ols Window - Sel */
	$"6563 7420 6E6F 7465 0000 0008 0000 0020"            /* ect note.......  */
	$"1C54 6F6F 6C73 2057 696E 646F 7720 2D20"            /* .Tools Window -  */
	$"5365 6C65 6374 2065 6666 6563 7400 0000"            /* Select effect... */
	$"0800 0000 211E 546F 6F6C 7320 5769 6E64"            /* ....!.Tools Wind */
	$"6F77 202D 2053 656C 6563 7420 6172 6775"            /* ow - Select argu */
	$"6D65 6E74 0000 0008 0000 0022 1C54 6F6F"            /* ment.......".Too */
	$"6C73 2057 696E 646F 7720 2D20 5365 6C65"            /* ls Window - Sele */
	$"6374 2076 6F6C 756D 6500 0000 0800 0000"            /* ct volume....... */
	$"2A26 546F 6F6C 7320 5769 6E64 6F77 202D"            /* *&Tools Window - */
	$"2053 7769 7463 6820 4170 706C 7920 416C"            /*  Switch Apply Al */
	$"6C20 6275 7474 6F6E 0000 0008 0000 0028"            /* l button.......( */
	$"2054 6F6F 6C73 2057 696E 646F 7720 2D20"            /*  Tools Window -  */
	$"506C 6179 2063 7572 7265 6E74 2063 656C"            /* Play current cel */
	$"6C00 0000 0800 0000 1602 282D 0000 0000"            /* l.........(-.... */
	$"0000 0000 2143 6C61 7373 6963 2045 6469"            /* ....!Classic Edi */
	$"746F 7220 2D20 4C6F 6164 2061 2050 636D"            /* tor - Load a Pcm */
	$"6420 6669 6C65 0000 001B 0000 0008 2743"            /* d file........'C */
	$"6C61 7373 6963 2045 6469 746F 7220 2D20"            /* lassic Editor -  */
	$"5361 7665 2063 7572 7265 6E74 2073 656C"            /* Save current sel */
	$"6563 7469 6F6E 0000 001B 0000 0015 0228"            /* ection.........( */
	$"2D00 0000 0000 0000 0020 4D75 7369 6320"            /* -........ Music  */
	$"4C69 7374 2057 696E 646F 7720 2D20 4C6F"            /* List Window - Lo */
	$"6164 2061 206D 7573 6963 0000 0007 0000"            /* ad a music...... */
	$"001D"                                               /* .. */
};

data 'dctb' (128) {
	$"0000 0000 0000 0004 0000 DDDD DDDD DDDD"            /* ..........›››››› */
	$"0001 0000 0000 0000 0002 0000 0000 0000"            /* ................ */
	$"0003 0000 0000 0000 0004 FFFF FFFF FFFF"            /* ..........ˇˇˇˇˇˇ */
};

data 'dctb' (500) {
	$"0000 0000 0000 0004 0000 AAAA AAAA AAAA"            /* ..........™™™™™™ */
	$"0001 0000 0000 0000 0002 0000 0000 0000"            /* ................ */
	$"0003 0000 0000 0000 0004 FFFF FFFF FFFF"            /* ..........ˇˇˇˇˇˇ */
};

data 'dctb' (141, "Adaptators") {
	$"0000 0000 0000 0004 0000 DDDD DDDD DDDD"            /* ..........›››››› */
	$"0001 0000 0000 0000 0002 0000 0000 0000"            /* ................ */
	$"0003 0000 0000 0000 0004 FFFF FFFF FFFF"            /* ..........ˇˇˇˇˇˇ */
};

data 'dctb' (144, "PatList") {
	$"0000 0000 0000 0004 0000 DDDD DDDD DDDD"            /* ..........›››››› */
	$"0001 0000 0000 0000 0002 0000 0000 0000"            /* ................ */
	$"0003 0000 0000 0000 0004 FFFF FFFF FFFF"            /* ..........ˇˇˇˇˇˇ */
};

data 'dctb' (129) {
	$"0000 0000 0000 0004 0000 DDDD DDDD DDDD"            /* ..........›››››› */
	$"0001 0000 0000 0000 0002 0000 0000 0000"            /* ................ */
	$"0003 0000 0000 0000 0004 FFFF FFFF FFFF"            /* ..........ˇˇˇˇˇˇ */
};

data 'dctb' (137, "Samples") {
	$"0000 0000 0000 0004 0000 DDDD DDDD DDDD"            /* ..........›››››› */
	$"0001 0000 0000 0000 0002 0000 0000 0000"            /* ................ */
	$"0003 0000 0000 0000 0004 FFFF FFFF FFFF"            /* ..........ˇˇˇˇˇˇ */
};

data 'dctb' (150, "Digi") {
	$"0000 0000 0000 0004 0000 DDDD DDDD DDDD"            /* ..........›››››› */
	$"0001 0000 0000 0000 0002 0000 0000 0000"            /* ................ */
	$"0003 0000 0000 0000 0004 FFFF FFFF FFFF"            /* ..........ˇˇˇˇˇˇ */
};

data 'dctb' (143, "MusicList") {
	$"0000 0000 0000 0004 0000 DDDD DDDD DDDD"            /* ..........›››››› */
	$"0001 0000 0000 0000 0002 0000 0000 0000"            /* ................ */
	$"0003 0000 0000 0000 0004 FFFF FFFF FFFF"            /* ..........ˇˇˇˇˇˇ */
};

data 'dctb' (153, "Digi") {
	$"0000 0000 0000 0004 0000 DDDD DDDD DDDD"            /* ..........›››››› */
	$"0001 0000 0000 0000 0002 0000 0000 0000"            /* ................ */
	$"0003 0000 0000 0000 0004 FFFF FFFF FFFF"            /* ..........ˇˇˇˇˇˇ */
};

data 'dctb' (154) {
	$"0000 0000 0000 0004 0000 FFFF FFFF FFFF"            /* ..........ˇˇˇˇˇˇ */
	$"0001 0000 0000 0000 0002 0000 0000 0000"            /* ................ */
	$"0003 0000 0000 0000 0004 FFFF FFFF FFFF"            /* ..........ˇˇˇˇˇˇ */
};

data 'dctb' (133, "Registration") {
	$"0000 0000 0000 0004 0000 DDDD DDDD DDDD"            /* ..........›››››› */
	$"0001 0000 0000 0000 0002 0000 0000 0000"            /* ................ */
	$"0003 0000 0000 0000 0004 FFFF FFFF FFFF"            /* ..........ˇˇˇˇˇˇ */
};

data 'dctb' (157) {
	$"0000 0000 0000 0004 0000 DDDD DDDD DDDD"            /* ..........›››››› */
	$"0001 0000 0000 0000 0002 0000 0000 0000"            /* ................ */
	$"0003 0000 0000 0000 0004 FFFF FFFF FFFF"            /* ..........ˇˇˇˇˇˇ */
};

data 'dctb' (134, "Registration") {
	$"0000 0000 0000 0004 0000 DDDD DDDD DDDD"            /* ..........›››››› */
	$"0001 0000 0000 0000 0002 0000 0000 0000"            /* ................ */
	$"0003 0000 0000 0000 0004 FFFF FFFF FFFF"            /* ..........ˇˇˇˇˇˇ */
};

data 'dctb' (163, "MusicList") {
	$"0000 0000 0000 0004 0000 DDDD DDDD DDDD"            /* ..........›››››› */
	$"0001 0000 0000 0000 0002 0000 0000 0000"            /* ................ */
	$"0003 0000 0000 0000 0004 FFFF FFFF FFFF"            /* ..........ˇˇˇˇˇˇ */
};

data 'dctb' (135, "Samples") {
	$"0000 0000 0000 0004 0000 DDDD DDDD DDDD"            /* ..........›››››› */
	$"0001 0000 0000 0000 0002 0000 0000 0000"            /* ................ */
	$"0003 0000 0000 0000 0004 FFFF FFFF FFFF"            /* ..........ˇˇˇˇˇˇ */
};

data 'dctb' (147, "Samples") {
	$"0000 0000 0000 0004 0000 DDDD DDDD DDDD"            /* ..........›››››› */
	$"0001 0000 0000 0000 0002 0000 0000 0000"            /* ................ */
	$"0003 0000 0000 0000 0004 FFFF FFFF FFFF"            /* ..........ˇˇˇˇˇˇ */
};

data 'dctb' (166, "PatList") {
	$"0000 0000 0000 0004 0000 DDDD DDDD DDDD"            /* ..........›››››› */
	$"0001 0000 0000 0000 0002 0000 0000 0000"            /* ................ */
	$"0003 0000 0000 0000 0004 FFFF FFFF FFFF"            /* ..........ˇˇˇˇˇˇ */
};

data 'dctb' (145, "Piano") {
	$"0000 0000 0000 0004 0000 DDDD DDDD DDDD"            /* ..........›››››› */
	$"0001 0000 0000 0000 0002 0000 0000 0000"            /* ................ */
	$"0003 0000 0000 0000 0004 FFFF FFFF FFFF"            /* ..........ˇˇˇˇˇˇ */
};

data 'dctb' (140, "Samples") {
	$"0000 0000 0000 0004 0000 DDDD DDDD DDDD"            /* ..........›››››› */
	$"0001 0000 0000 0000 0002 0000 0000 0000"            /* ................ */
	$"0003 0000 0000 0000 0004 FFFF FFFF FFFF"            /* ..........ˇˇˇˇˇˇ */
};

data 'dctb' (155, "Color") {
	$"0000 0000 0000 0004 0000 0000 0000 0000"            /* ................ */
	$"0001 0000 0000 0000 0002 0000 0000 0000"            /* ................ */
	$"0003 0000 0000 0000 0004 FFFF FFFF FFFF"            /* ..........ˇˇˇˇˇˇ */
};

data 'dctb' (171, "StartUp Dialog") {
	$"0000 0000 0000 0004 0000 0000 0000 0000"            /* ................ */
	$"0001 0000 0000 0000 0002 0000 0000 0000"            /* ................ */
	$"0003 0000 0000 0000 0004 FFFF FFFF FFFF"            /* ..........ˇˇˇˇˇˇ */
};

data 'dctb' (164, "Spectrum") {
	$"0000 0000 0000 0004 0000 DDDD DDDD DDDD"            /* ..........›››››› */
	$"0001 0000 0000 0000 0002 0000 0000 0000"            /* ................ */
	$"0003 0000 0000 0000 0004 FFFF FFFF FFFF"            /* ..........ˇˇˇˇˇˇ */
};

data 'dctb' (173) {
	$"0000 0000 0000 0004 0000 DDDD DDDD DDDD"            /* ..........›››››› */
	$"0001 0000 0000 0000 0002 0000 0000 0000"            /* ................ */
	$"0003 0000 0000 0000 0004 FFFF FFFF FFFF"            /* ..........ˇˇˇˇˇˇ */
};

data 'dctb' (165, "Microphone") {
	$"0000 0000 0000 0004 0000 EEEE EEEE EEEE"            /* ..........ÓÓÓÓÓÓ */
	$"0001 0000 0000 0000 0002 0000 0000 0000"            /* ................ */
	$"0003 0000 0000 0000 0004 FFFF FFFF FFFF"            /* ..........ˇˇˇˇˇˇ */
};

data 'dctb' (168, "File informations") {
	$"0000 0000 0000 0004 0000 DDDD DDDD DDDD"            /* ..........›››››› */
	$"0001 0000 0000 0000 0002 0000 0000 0000"            /* ................ */
	$"0003 0000 0000 0000 0004 FFFF FFFF FFFF"            /* ..........ˇˇˇˇˇˇ */
};

data 'dctb' (176, "Quicktime 2.0 Instruments") {
	$"0000 0000 0000 0004 0000 DDDD DDDD DDDD"            /* ..........›››››› */
	$"0001 0000 0000 0000 0002 0000 0000 0000"            /* ................ */
	$"0003 0000 0000 0000 0004 FFFF FFFF FFFF"            /* ..........ˇˇˇˇˇˇ */
};

data 'dctb' (156, "ProgressWindow") {
	$"0000 0000 0000 0004 0000 DDDD DDDD DDDD"            /* ..........›››››› */
	$"0001 0000 0000 0000 0002 0000 0000 0000"            /* ................ */
	$"0003 0000 0000 0000 0004 0000 0000 0000"            /* ................ */
};

data 'dctb' (160, "Export AIFF/SND") {
	$"0000 0000 0000 0004 0000 DDDD DDDD DDDD"            /* ..........›››››› */
	$"0001 0000 0000 0000 0002 0000 0000 0000"            /* ................ */
	$"0003 0000 0000 0000 0004 FFFF FFFF FFFF"            /* ..........ˇˇˇˇˇˇ */
};

data 'dctb' (167, "Pattern Info") {
	$"0000 0000 0000 0004 0000 DDDD DDDD DDDD"            /* ..........›››››› */
	$"0001 0000 0000 0000 0002 0000 0000 0000"            /* ................ */
	$"0003 0000 0000 0000 0004 FFFF FFFF FFFF"            /* ..........ˇˇˇˇˇˇ */
};

data 'dctb' (161, "SoundFormat Export") {
	$"0000 0000 0000 0004 0000 DDDD DDDD DDDD"            /* ..........›››››› */
	$"0001 0000 0000 0000 0002 0000 0000 0000"            /* ................ */
	$"0003 0000 0000 0000 0004 FFFF FFFF FFFF"            /* ..........ˇˇˇˇˇˇ */
};

data 'dctb' (172, "Move") {
	$"0000 0000 0000 0004 0000 DDDD DDDD DDDD"            /* ..........›››››› */
	$"0001 0000 0000 0000 0002 0000 0000 0000"            /* ................ */
	$"0003 0000 0000 0000 0004 FFFF FFFF FFFF"            /* ..........ˇˇˇˇˇˇ */
};

data 'dctb' (174, "Print Order Form") {
	$"0000 0000 0000 0004 0000 DDDD DDDD DDDD"            /* ..........›››››› */
	$"0001 0000 0000 0000 0002 0000 0000 0000"            /* ................ */
	$"0003 0000 0000 0000 0004 FFFF FFFF FFFF"            /* ..........ˇˇˇˇˇˇ */
};

data 'dctb' (175, "Registration") {
	$"0000 0000 0000 0004 0000 FFFF FFFF FFFF"            /* ..........ˇˇˇˇˇˇ */
	$"0001 0000 0000 0000 0002 0000 0000 0000"            /* ................ */
	$"0003 0000 0000 0000 0004 FFFF FFFF FFFF"            /* ..........ˇˇˇˇˇˇ */
};

data 'dctb' (2000, "Pref: DRIVER") {
	$"0000 0000 0000 0004 0000 DDDD DDDD DDDD"            /* ..........›››››› */
	$"0001 0000 0000 0000 0002 0000 0000 0000"            /* ................ */
	$"0003 0000 0000 0000 0004 FFFF FFFF FFFF"            /* ..........ˇˇˇˇˇˇ */
};

data 'dctb' (3000, "General Error") {
	$"0000 0000 0000 0004 0000 DDDD DDDD DDDD"            /* ..........›››››› */
	$"0001 0000 0000 0000 0002 0000 0000 0000"            /* ................ */
	$"0003 0000 0000 0000 0004 FFFF FFFF FFFF"            /* ..........ˇˇˇˇˇˇ */
};

data 'dctb' (3001, "Info") {
	$"0000 0000 0000 0004 0000 DDDD DDDD DDDD"            /* ..........›››››› */
	$"0001 0000 0000 0000 0002 0000 0000 0000"            /* ................ */
	$"0003 0000 0000 0000 0004 FFFF FFFF FFFF"            /* ..........ˇˇˇˇˇˇ */
};

data 'dctb' (3002, "Save Or Not?") {
	$"0000 0000 0000 0004 0000 DDDD DDDD DDDD"            /* ..........›››››› */
	$"0001 0000 0000 0000 0002 0000 0000 0000"            /* ................ */
	$"0003 0000 0000 0000 0004 FFFF FFFF FFFF"            /* ..........ˇˇˇˇˇˇ */
};

data 'dctb' (3003, "File Not Found") {
	$"0000 0000 0000 0004 0000 DDDD DDDD DDDD"            /* ..........›››››› */
	$"0001 0000 0000 0000 0002 0000 0000 0000"            /* ................ */
	$"0003 0000 0000 0000 0004 FFFF FFFF FFFF"            /* ..........ˇˇˇˇˇˇ */
};

data 'dctb' (3004, "Music List Save Or Not?") {
	$"0000 0000 0000 0004 0000 DDDD DDDD DDDD"            /* ..........›››››› */
	$"0001 0000 0000 0000 0002 0000 0000 0000"            /* ................ */
	$"0003 0000 0000 0000 0004 FFFF FFFF FFFF"            /* ..........ˇˇˇˇˇˇ */
};

data 'dctb' (149, "Minimize Results") {
	$"0000 0000 0000 0004 0000 DDDD DDDD DDDD"            /* ..........›››››› */
	$"0001 0000 0000 0000 0002 0000 0000 0000"            /* ................ */
	$"0003 0000 0000 0000 0004 FFFF FFFF FFFF"            /* ..........ˇˇˇˇˇˇ */
};

data 'dctb' (152, "Find/Replace") {
	$"0000 0000 0000 0004 0000 DDDD DDDD DDDD"            /* ..........›››››› */
	$"0001 0000 0000 0000 0002 0000 0000 0000"            /* ................ */
	$"0003 0000 0000 0000 0004 FFFF FFFF FFFF"            /* ..........ˇˇˇˇˇˇ */
};

data 'dctb' (2001) {
	$"0000 0000 0000 0004 0000 FFFF FFFF FFFF"            /* ..........ˇˇˇˇˇˇ */
	$"0001 0000 0000 0000 0002 0000 0000 0000"            /* ................ */
	$"0003 0000 0000 0000 0004 FFFF FFFF FFFF"            /* ..........ˇˇˇˇˇˇ */
};

data 'dctb' (177) {
	$"0000 0000 0000 0004 0000 DDDD DDDD DDDD"            /* ..........›››››› */
	$"0001 0000 0000 0000 0002 0000 0000 0000"            /* ................ */
	$"0003 0000 0000 0000 0004 FFFF FFFF FFFF"            /* ..........ˇˇˇˇˇˇ */
};

data 'dctb' (131, "Intro") {
	$"0000 0000 0000 0004 0000 0000 0000 0000"            /* ................ */
	$"0001 0000 0000 0000 0002 0000 0000 0000"            /* ................ */
	$"0003 0000 0000 0000 0004 FFFF FFFF FFFF"            /* ..........ˇˇˇˇˇˇ */
};

data 'dctb' (2005) {
	$"0000 0000 0000 0004 0000 FFFF FFFF FFFF"            /* ..........ˇˇˇˇˇˇ */
	$"0001 0000 0000 0000 0002 0000 0000 0000"            /* ................ */
	$"0003 0000 0000 0000 0004 FFFF FFFF FFFF"            /* ..........ˇˇˇˇˇˇ */
};

data 'dctb' (2004) {
	$"0000 0000 0000 0004 0000 FFFF FFFF FFFF"            /* ..........ˇˇˇˇˇˇ */
	$"0001 0000 0000 0000 0002 0000 0000 0000"            /* ................ */
	$"0003 0000 0000 0000 0004 FFFF FFFF FFFF"            /* ..........ˇˇˇˇˇˇ */
};

data 'dctb' (178) {
	$"0000 0000 0000 0004 0000 DDDD DDDD DDDD"            /* ..........›››››› */
	$"0001 0000 0000 0000 0002 0000 0000 0000"            /* ................ */
	$"0003 0000 0000 0000 0004 FFFF FFFF FFFF"            /* ..........ˇˇˇˇˇˇ */
};

data 'dctb' (139, "Editor") {
	$"0000 0000 0000 0004 0000 FFFF FFFF FFFF"            /* ..........ˇˇˇˇˇˇ */
	$"0001 0000 0000 0000 0002 0000 0000 0000"            /* ................ */
	$"0003 0000 0000 0000 0004 FFFF FFFF FFFF"            /* ..........ˇˇˇˇˇˇ */
};

data 'dctb' (136, "Windows State") {
	$"0000 0000 0000 0004 0000 DDDD DDDD DDDD"            /* ..........›››››› */
	$"0001 0000 0000 0000 0002 0000 0000 0000"            /* ................ */
	$"0003 0000 0000 0000 0004 FFFF FFFF FFFF"            /* ..........ˇˇˇˇˇˇ */
};

data 'dctb' (138, "Reset Window") {
	$"0000 0000 0000 0004 0000 DDDD DDDD DDDD"            /* ..........›››››› */
	$"0001 0000 0000 0000 0002 0000 0000 0000"            /* ................ */
	$"0003 0000 0000 0000 0004 FFFF FFFF FFFF"            /* ..........ˇˇˇˇˇˇ */
};

data 'dctb' (2009) {
	$"0000 0000 0000 0004 0000 FFFF FFFF FFFF"            /* ..........ˇˇˇˇˇˇ */
	$"0001 0000 0000 0000 0002 0000 0000 0000"            /* ................ */
	$"0003 0000 0000 0000 0004 FFFF FFFF FFFF"            /* ..........ˇˇˇˇˇˇ */
};

data 'dctb' (2006) {
	$"0000 0000 0000 0004 0000 FFFF FFFF FFFF"            /* ..........ˇˇˇˇˇˇ */
	$"0001 0000 0000 0000 0002 0000 0000 0000"            /* ................ */
	$"0003 0000 0000 0000 0004 FFFF FFFF FFFF"            /* ..........ˇˇˇˇˇˇ */
};

data 'dctb' (180) {
	$"0000 0000 0000 0004 0000 DDDD DDDD DDDD"            /* ..........›››››› */
	$"0001 0000 0000 0000 0002 0000 0000 0000"            /* ................ */
	$"0003 0000 0000 0000 0004 FFFF FFFF FFFF"            /* ..........ˇˇˇˇˇˇ */
};

data 'dctb' (181) {
	$"0000 0000 0000 0004 0000 DDDD DDDD DDDD"            /* ..........›››››› */
	$"0001 0000 0000 0000 0002 0000 0000 0000"            /* ................ */
	$"0003 0000 0000 0000 0004 FFFF FFFF FFFF"            /* ..........ˇˇˇˇˇˇ */
};

data 'dctb' (132) {
	$"0000 0000 0000 0004 0000 FFFF FFFF FFFF"            /* ..........ˇˇˇˇˇˇ */
	$"0001 0000 0000 0000 0002 0000 0000 0000"            /* ................ */
	$"0003 0000 0000 0000 0004 FFFF FFFF FFFF"            /* ..........ˇˇˇˇˇˇ */
};

data 'dctb' (130) {
	$"0000 0000 0000 0004 0000 FFFF FFFF FFFF"            /* ..........ˇˇˇˇˇˇ */
	$"0001 0000 0000 0000 0002 0000 0000 0000"            /* ................ */
	$"0003 0000 0000 0000 0004 FFFF FFFF FFFF"            /* ..........ˇˇˇˇˇˇ */
};

data 'dctb' (182) {
	$"0000 0000 0000 0004 0000 DDDD DDDD DDDD"            /* ..........›››››› */
	$"0001 0000 0000 0000 0002 0000 0000 0000"            /* ................ */
	$"0003 0000 0000 0000 0004 FFFF FFFF FFFF"            /* ..........ˇˇˇˇˇˇ */
};

data 'dctb' (148, "General Informations") {
	$"0000 0000 0000 0004 0000 DDDD DDDD DDDD"            /* ..........›››››› */
	$"0001 0000 0000 0000 0002 0000 0000 0000"            /* ................ */
	$"0003 0000 0000 0000 0004 FFFF FFFF FFFF"            /* ..........ˇˇˇˇˇˇ */
};

data 'dctb' (183) {
	$"0000 0000 0000 0004 0000 DDDD DDDD DDDD"            /* ..........›››››› */
	$"0001 0000 0000 0000 0002 0000 0000 0000"            /* ................ */
	$"0003 0000 0000 0000 0004 FFFF FFFF FFFF"            /* ..........ˇˇˇˇˇˇ */
};

data 'dctb' (184) {
	$"0000 0000 0000 0004 0000 DDDD DDDD DDDD"            /* ..........›››››› */
	$"0001 0000 0000 0000 0002 0000 0000 0000"            /* ................ */
	$"0003 0000 0000 0000 0004 FFFF FFFF FFFF"            /* ..........ˇˇˇˇˇˇ */
};

data 'dctb' (146, "EditInfo Instrument") {
	$"0000 0000 0000 0004 0000 DDDD DDDD DDDD"            /* ..........›››››› */
	$"0001 0000 0000 0000 0002 0000 0000 0000"            /* ................ */
	$"0003 0000 0000 0000 0004 FFFF FFFF FFFF"            /* ..........ˇˇˇˇˇˇ */
};

data 'dctb' (142, "MusicList") {
	$"0000 0000 0000 0004 0000 DDDD DDDD DDDD"            /* ..........›››››› */
	$"0001 0000 0000 0000 0002 0000 0000 0000"            /* ................ */
	$"0003 0000 0000 0000 0004 FFFF FFFF FFFF"            /* ..........ˇˇˇˇˇˇ */
};

data 'ictb' (177) {
	$"0000 0000 0000 0000 0000 0000"                      /* ............ */
};

data 'ictb' (200) {
	$"0000 0000 0000 0000 0000 0000 0000 0000"            /* ................ */
	$"0000 0000 0000 0000 0000 0000 0000 0000"            /* ................ */
};

data 'ictb' (164) {
	$"0000 0000"                                          /* .... */
};

data 'ictb' (2006) {
	$"0000 0000 0000 0000 0000 0000 0000 0000"            /* ................ */
	$"0000 0000 0000 0000 0000 0000 0000 0000"            /* ................ */
	$"0000 0000 0000 0000 0000 0000 0000 0000"            /* ................ */
	$"0000 0000 0000 0000 0000 0000 0000 0000"            /* ................ */
	$"0000 0000 0000 0000 0000 0000 0000 0000"            /* ................ */
	$"0000 0000 0000 0000 0000 0000 0000 0000"            /* ................ */
	$"0000 0000 0000 0000 0000 0000 0000 0000"            /* ................ */
	$"0000 0000"                                          /* .... */
};

data 'ictb' (684) {
	$"0000 0000 0000 0000 0000 0000 0000 0000"            /* ................ */
	$"0000 0000 0000 0000 0000 0000 0000 0000"            /* ................ */
	$"0000 0000 0000 0000 0000 0000 0000 0000"            /* ................ */
	$"0000 0000 0000 0000 0000 0000 0000 0000"            /* ................ */
	$"0000 0000 0000 0000 0000 0000 0000 0000"            /* ................ */
	$"0000 0000 0000 0000 0000 0000 0000 0000"            /* ................ */
	$"0000 0000 0000 0000 0000 0000 0000 0000"            /* ................ */
	$"0000 0000 0000 0000 0000 0000 0000 0000"            /* ................ */
};

data 'ictb' (761) {
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
	$"0000 0000 0000 0000"                                /* ........ */
};

data 'ictb' (2000) {
	$"0000 0000 0000 0000 0000 0000 0000 0000"            /* ................ */
	$"0000 0000 0000 0000 0000 0000 0000 0000"            /* ................ */
	$"0000 0000 0000 0000 0000 0000 0000 0000"            /* ................ */
	$"0000 0000 0000 0000 0000 0000 0000 0000"            /* ................ */
	$"0000 0000 0000 0000 0000 0000 0000 0000"            /* ................ */
	$"0000 0000 0000 0000 0000 0000 0000 0000"            /* ................ */
	$"0000 0000 0000 0000 0000 0000 0000 0000"            /* ................ */
	$"0000 0000 0000 0000 0000 0000 0000 0000"            /* ................ */
	$"0000 0000 0000 0000 0000 0000"                      /* ............ */
};

data 'ictb' (178) {
	$"0000 0000 0000 0000 0000 0000 0000 0000"            /* ................ */
	$"0000 0000 0000 0000 0000 0000 0000 0000"            /* ................ */
	$"0000 0000 0000 0000 0000 0000 0000 0000"            /* ................ */
	$"0000 0000 0000 0000 0000 0000 0000 0000"            /* ................ */
	$"0000 0000 0000 0000 0000 0000 0000 0000"            /* ................ */
	$"0000 0000 0000 0000 0000 0000 0000 0000"            /* ................ */
	$"0000 0000 0000 0000 0000 0000 0000 0000"            /* ................ */
	$"0000 0000 0000 0000 0000 0000 0000 0000"            /* ................ */
};

data 'ictb' (163) {
	$"0000 0000 0000 0000 0000 0000 0000 0000"            /* ................ */
	$"0000 0000 0000 0000 0000 0000 0000 0000"            /* ................ */
	$"0000 0000"                                          /* .... */
};

data 'ictb' (136) {
	$"0000 0000 0000 0000 0000 0000 0000 0000"            /* ................ */
	$"0000 0000 0000 0000 0000 0000 0000 0000"            /* ................ */
	$"0000 0000"                                          /* .... */
};

data 'ictb' (139) {
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
};

data 'ictb' (2001) {
	$"0000 0000 0000 0000 0000 0000 0000 0000"            /* ................ */
	$"0000 0000 0000 0000 0000 0000 0000 0000"            /* ................ */
	$"0000 0000 0000 0000 0000 0000 0000 0000"            /* ................ */
	$"0000 0000 0000 0000 0000 0000 0000 0000"            /* ................ */
	$"0000 0000 0000 0000 0000 0000 0000 0000"            /* ................ */
	$"0000 0000 0000 0000 0000 0000 0000 0000"            /* ................ */
	$"0000 0000 0000 0000 0000 0000 0000 0000"            /* ................ */
};

data 'ictb' (319) {
	$"0000 0000 0000 0000 0000 0000 0000 0000"            /* ................ */
	$"0000 0000 0000 0000 0000 0000 0000 0000"            /* ................ */
	$"0000 0000 0000 0000 0000 0000 0000 0000"            /* ................ */
	$"0000 0000 0000 0000 0000 0000 0000 0000"            /* ................ */
	$"0000 0000 0000 0000 0000 0000 0000 0000"            /* ................ */
	$"0000 0000 0000 0000 0000 0000 0000 0000"            /* ................ */
	$"0000 0000 0000 0000 0000 0000 0000 0000"            /* ................ */
	$"0000 0000 0000 0000 0000 0000 0000 0000"            /* ................ */
	$"0000 0000 0000 0000 0000 0000 0000 0000"            /* ................ */
	$"0000 0000 0000 0000"                                /* ........ */
};

data 'ictb' (2009) {
	$"0000 0000 0000 0000 0000 0000 0000 0000"            /* ................ */
	$"0000 0000 0000 0000 0000 0000 0000 0000"            /* ................ */
	$"0000 0000 0000 0000 0000 0000"                      /* ............ */
};

data 'ictb' (130) {
	$"0000 0000 0000 0000 0000 0000 0000 0000"            /* ................ */
	$"0000 0000 0000 0000 0000 0000 0000 0000"            /* ................ */
	$"0000 0000 0000 0000 0000 0000 0000 0000"            /* ................ */
};

data 'ictb' (145) {
	$"0000 0000 0000 0000 0000 0000 0000 0000"            /* ................ */
	$"0000 0000 0000 0000 0000 0000"                      /* ............ */
};

data 'ictb' (180) {
	$"0000 0000 0000 0000 0000 0000 0000 0000"            /* ................ */
	$"0000 0000 0000 0000 0000 0000 0000 0000"            /* ................ */
	$"0000 0000 0000 0000 0000 0000 0000 0000"            /* ................ */
	$"0000 0000 0000 0000 0000 0000 0000 0000"            /* ................ */
	$"0000 0000 0000 0000 0000 0000 0000 0000"            /* ................ */
	$"0000 0000 0000 0000 0000 0000 0000 0000"            /* ................ */
	$"0000 0000 0000 0000 0000 0000 0000 0000"            /* ................ */
	$"0000 0000 0000 0000 0000 0000 0000 0000"            /* ................ */
};

data 'ictb' (816) {
	$"0000 0000 0000 0000 0000 0000 0000 0000"            /* ................ */
	$"0000 0000 0000 0000 0000 0000 0000 0000"            /* ................ */
	$"0000 0000 0000 0000 0000 0000 0000 0000"            /* ................ */
};

data 'ictb' (143) {
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
	$"0000 0000"                                          /* .... */
};

data 'ictb' (872) {
	$"0000 0000 0000 0000"                                /* ........ */
};

data 'ictb' (421) {
	$"0000 0000 0000 0000 0000 0000 0000 0000"            /* ................ */
	$"0000 0000 0000 0000"                                /* ........ */
};

data 'ictb' (181) {
	$"0000 0000 0000 0000 0000 0000 0000 0000"            /* ................ */
	$"0000 0000 0000 0000 0000 0000 0000 0000"            /* ................ */
	$"0000 0000 0000 0000 0000 0000 0000 0000"            /* ................ */
};

data 'ictb' (2004) {
	$"0000 0000 0000 0000 0000 0000 0000 0000"            /* ................ */
	$"0000 0000 0000 0000 0000 0000 0000 0000"            /* ................ */
	$"0000 0000 0000 0000 0000 0000 0000 0000"            /* ................ */
	$"0000 0000 0000 0000 0000 0000 0000 0000"            /* ................ */
	$"0000 0000 0000 0000 0000 0000"                      /* ............ */
};

data 'ictb' (182) {
	$"0000 0000 0000 0000 0000 0000 0000 0000"            /* ................ */
	$"0000 0000 0000 0000"                                /* ........ */
};

data 'ictb' (202) {
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
};

data 'ictb' (183) {
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
	$"0000 0000"                                          /* .... */
};

data 'ictb' (148) {
	$"0000 0000 0000 0000 0000 0000 0000 0000"            /* ................ */
	$"0000 0000 0000 0000 0000 0000 0000 0000"            /* ................ */
	$"0000 0000 0000 0000 0000 0000 0000 0000"            /* ................ */
	$"0000 0000 0000 0000 0028 0058 0000 0000"            /* .........(.X.... */
	$"0000 0000 0000 0000 0000 0000 0000 0000"            /* ................ */
	$"0000 0000 0000 0000 0000 0000 0000 0003"            /* ................ */
	$"0000 0000 0000 0000 0001 FFFF FFFF FFFF"            /* ..........ˇˇˇˇˇˇ */
	$"0002 0000 0000 0000 0003 FFFF FFFF FFFF"            /* ..........ˇˇˇˇˇˇ */
};

data 'ictb' (141) {
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
};

data 'ictb' (184) {
	$"0000 0000 0000 0000 0000 0000 0000 0000"            /* ................ */
	$"0000 0000 0028 0024 0028 004C 0028 0074"            /* .....(.$.(.L.(.t */
	$"8007 009C 0000 0000 0000 0003 0000 0000"            /* Ä..ú............ */
	$"0000 0000 0001 FFFF FFFF FFFF 0002 0000"            /* ......ˇˇˇˇˇˇ.... */
	$"0000 0000 0003 FFFF FFFF FFFF 0000 0000"            /* ......ˇˇˇˇˇˇ.... */
	$"0000 0003 0000 0000 0000 0000 0001 FFFF"            /* ..............ˇˇ */
	$"FFFF FFFF 0002 0000 0000 0000 0003 FFFF"            /* ˇˇˇˇ..........ˇˇ */
	$"FFFF FFFF 0000 0000 0000 0003 0000 0000"            /* ˇˇˇˇ............ */
	$"0000 0000 0001 FFFF FFFF FFFF 0002 0000"            /* ......ˇˇˇˇˇˇ.... */
	$"0000 0000 0003 FFFF FFFF FFFF 00B0 0000"            /* ......ˇˇˇˇˇˇ.∞.. */
	$"0009 0000 0000 0000 FFFF FFFF FFFF 0000"            /* .∆......ˇˇˇˇˇˇ.. */
	$"0647 656E 6576 61"                                  /* .Geneva */
};

data 'ictb' (146) {
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
	$"0000 0000"                                          /* .... */
};

data 'ictb' (142) {
	$"0000 0000 0000 0000 0000 0000 0000 0000"            /* ................ */
	$"0000 0000 0000 0000 0000 0000 0000 0000"            /* ................ */
	$"0000 0000 0000 0000 0000 0000 0000 0000"            /* ................ */
	$"0000 0000 0000 0000 0000 0000 0000 0000"            /* ................ */
	$"0000 0000 0000 0000 0000 0000 0000 0000"            /* ................ */
	$"0000 0000 0000 0000 0000 0000 0000 0000"            /* ................ */
	$"0000 0000 0000 0000 0000 0000 0000 0000"            /* ................ */
	$"0000 0000 0000 0000 0000 0000 0000 0000"            /* ................ */
	$"0000 0000"                                          /* .... */
};

data 'Mcmd' (130) {
	$"0007 0000 0000 0000 0000 0000 0000 0000"            /* ................ */
	$"0000 0000 0000 0000 0000 0000 0000"                 /* .............. */
};

data 'Mcmd' (154) {
	$"0004 0000 0000 0000 0000 0000 0000 0000"            /* ................ */
	$"0000"                                               /* .. */
};

data 'Mcmd' (133) {
	$"0003 0000 0000 0000 0000 0000 0000"                 /* .............. */
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
	$"0009 0000 0000 0000 0000 0000 0000 0000"            /* .∆.............. */
	$"0000 0000 0000 0000 0000 0000 0000 0000"            /* ................ */
	$"0000 0000 0000"                                     /* ...... */
};

data 'Mcmd' (135) {
	$"0000"                                               /* .. */
};

data 'Mcmd' (162) {
	$"0007 0000 0000 0000 0000 0000 0000 0000"            /* ................ */
	$"0000 0000 0000 0000 0000 0000 0000"                 /* .............. */
};

data 'Mcmd' (179) {
	$"0006 0000 0000 0000 0000 0000 0000 0000"            /* ................ */
	$"0000 0000 0000 0000 0000"                           /* .......... */
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

data 'mctb' (178) {
	$"0001 FF9D 0000 0000 0000 0000 0000 0000"            /* ..ˇù............ */
	$"0000 0000 0000 0000 0000 0000 0000 0000"            /* ................ */
};

data 'mctb' (177) {
	$"0001 FF9D 0000 0000 0000 0000 0000 0000"            /* ..ˇù............ */
	$"0000 0000 0000 0000 0000 0000 0000 0000"            /* ................ */
};

data 'mctb' (135) {
	$"0001 FF9D 0000 0000 0000 0000 0000 0000"            /* ..ˇù............ */
	$"0000 0000 0000 0000 0000 0000 0000 0000"            /* ................ */
};

data 'actb' (130) {
	$"0000 0000 0000 0004 0000 FFFF FFFF FFFF"            /* ..........ˇˇˇˇˇˇ */
	$"0001 0000 0000 0000 0002 0000 0000 0000"            /* ................ */
	$"0003 0000 0000 0000 0004 FFFF FFFF FFFF"            /* ..........ˇˇˇˇˇˇ */
};

data 'DLGX' (148) {
	$"0843 6861 7263 6F61 6C00 0000 0000 0000"            /* .Charcoal....... */
	$"0000 0000 0000 0000 0000 0000 0000 0000"            /* ................ */
	$"0000 0000 0000 0000 0000 0000 0000 0000"            /* ................ */
	$"0000 0000 0000 0000 0000 0000 0000 0000"            /* ................ */
	$"000C 0000 0000 0000 0004 0004 0000 0000"            /* ................ */
	$"0016 0000 0000 0000 0000 0000 0000 0001"            /* ................ */
	$"0000 0000 0000 0000 0000 0007 0000 0000"            /* ................ */
	$"0000 0000 0000 0006 0000 0000 0000 0000"            /* ................ */
	$"0000 0006 0000 0000 0000 0000 0000 0006"            /* ................ */
	$"0000 0000 0000 0000 0000 0007 0000 0000"            /* ................ */
	$"0000 0000 0000 0007 0000 0000 0000 0000"            /* ................ */
	$"0000 0006 0000 0000 0000 0000 0000 0007"            /* ................ */
	$"0000 0000 0000 0000 0000 0006 0000 0000"            /* ................ */
	$"0000 0000 0000 0006 0000 0000 0000 0000"            /* ................ */
	$"0000 0006 0000 0000 0000 0000 0000 0006"            /* ................ */
	$"0000 0000 0000 0000 0000 0003 0000 0000"            /* ................ */
	$"0000 0000 0000 0009 0000 0000 0000 0000"            /* .......∆........ */
	$"0000 0006 0000 0000 0000 0000 0000 0003"            /* ................ */
	$"0000 0000 0000 0000 0000 0003 0000 0000"            /* ................ */
	$"0000 0000 0000 0003 0000 0000 0000 0000"            /* ................ */
	$"0000 0009 0000 0000 0000 0000 0000 0006"            /* ...∆............ */
	$"0000 0000 0000 0000 0000"                           /* .......... */
};

data 'DLGX' (183) {
	$"0843 6861 7263 6F61 6C00 0000 0000 0000"            /* .Charcoal....... */
	$"0000 0000 0000 0000 0000 0000 0000 0000"            /* ................ */
	$"0000 0000 0000 0000 0000 0000 0000 0000"            /* ................ */
	$"0000 0000 0000 0000 0000 0000 0000 0000"            /* ................ */
	$"000C 0000 0000 0000 0004 0004 0000 0000"            /* ................ */
	$"0029 000A 0000 0000 0000 0000 0000 000A"            /* .).¬...........¬ */
	$"0000 0000 0000 0000 0000 000A 0000 0000"            /* ...........¬.... */
	$"0000 0000 0000 000A 0000 0000 0000 0000"            /* .......¬........ */
	$"0000 000A 0000 0000 0000 0000 0000 000A"            /* ...¬...........¬ */
	$"0000 0000 0000 0000 0000 000A 0000 0000"            /* ...........¬.... */
	$"0000 0000 0000 000A 0000 0000 0000 0000"            /* .......¬........ */
	$"0000 000A 0000 0000 0000 0000 0000 000A"            /* ...¬...........¬ */
	$"0000 0000 0000 0000 0000 000A 0000 0000"            /* ...........¬.... */
	$"0000 0000 0000 000A 0000 0000 0000 0000"            /* .......¬........ */
	$"0000 000A 0000 0000 0000 0000 0000 000A"            /* ...¬...........¬ */
	$"0000 0000 0000 0000 0000 000A 0000 0000"            /* ...........¬.... */
	$"0000 0000 0000 000A 0000 0000 0000 0000"            /* .......¬........ */
	$"0000 000A 0000 0000 0000 0000 0000 0006"            /* ...¬............ */
	$"0000 0000 0000 0000 0000 0006 0000 0000"            /* ................ */
	$"0000 0000 0000 0006 0000 0000 0000 0000"            /* ................ */
	$"0000 0006 0000 0000 0000 0000 0000 0006"            /* ................ */
	$"0000 0000 0000 0000 0000 0006 0000 0000"            /* ................ */
	$"0000 0000 0000 0006 0000 0000 0000 0000"            /* ................ */
	$"0000 0006 0000 0000 0000 0000 0000 0006"            /* ................ */
	$"0000 0000 0000 0000 0000 0006 0000 0000"            /* ................ */
	$"0000 0000 0000 0006 0000 0000 0000 0000"            /* ................ */
	$"0000 0006 0000 0000 0000 0000 0000 0006"            /* ................ */
	$"0000 0000 0000 0000 0000 0006 0000 0000"            /* ................ */
	$"0000 0000 0000 0006 0000 0000 0000 0000"            /* ................ */
	$"0000 0006 0000 0000 0000 0000 0000 0006"            /* ................ */
	$"0000 0000 0000 0000 0000 0006 0000 0000"            /* ................ */
	$"0000 0000 0000 0006 0000 0000 0000 0000"            /* ................ */
	$"0000 0006 0000 0000 0000 0000 0000 0006"            /* ................ */
	$"0000 0000 0000 0000 0000 0006 0000 0000"            /* ................ */
	$"0000 0000 0000 0006 0000 0000 0000 0000"            /* ................ */
	$"0000 0006 0000 0000 0000 0000 0000"                 /* .............. */
};

data 'DLGX' (141) {
	$"0843 6861 7263 6F61 6C00 0000 0000 0000"            /* .Charcoal....... */
	$"0000 0000 0000 0000 0000 0000 0000 0000"            /* ................ */
	$"0000 0000 0000 0000 0000 0000 0000 0000"            /* ................ */
	$"0000 0000 0000 0000 0000 0000 0000 0000"            /* ................ */
	$"000C 0000 0000 0000 0004 0004 0000 0000"            /* ................ */
	$"002C 000A 0000 0000 0000 0000 0000 000A"            /* .,.¬...........¬ */
	$"0000 0000 0000 0000 0000 000A 0000 0000"            /* ...........¬.... */
	$"0000 0000 0000 0006 0000 0000 0000 0000"            /* ................ */
	$"0000 000A 0000 0000 0000 0000 0000 000A"            /* ...¬...........¬ */
	$"0000 0000 0000 0000 0000 0006 0000 0000"            /* ................ */
	$"0000 0000 0000 000A 0000 0000 0000 0000"            /* .......¬........ */
	$"0000 000A 0000 0000 0000 0000 0000 000A"            /* ...¬...........¬ */
	$"0000 0000 0000 0000 0000 000A 0000 0000"            /* ...........¬.... */
	$"0000 0000 0000 000A 0000 0000 0000 0000"            /* .......¬........ */
	$"0000 0009 0000 0000 0000 0000 0000 000A"            /* ...∆...........¬ */
	$"0000 0000 0000 0000 0000 000A 0000 0000"            /* ...........¬.... */
	$"0000 0000 0000 000A 0000 0000 0000 0000"            /* .......¬........ */
	$"0000 000A 0000 0000 0000 0000 0000 000A"            /* ...¬...........¬ */
	$"0000 0000 0000 0000 0000 000A 0000 0000"            /* ...........¬.... */
	$"0000 0000 0000 0009 0000 0000 0000 0000"            /* .......∆........ */
	$"0000 000A 0000 0000 0000 0000 0000 000A"            /* ...¬...........¬ */
	$"0000 0000 0000 0000 0000 000A 0000 0000"            /* ...........¬.... */
	$"0000 0000 0000 0005 0000 0000 0000 0000"            /* ................ */
	$"0000 000A 0000 0000 0000 0000 0000 000A"            /* ...¬...........¬ */
	$"0000 0000 0000 0000 0000 000A 0000 0000"            /* ...........¬.... */
	$"0000 0000 0000 0009 0000 0000 0000 0000"            /* .......∆........ */
	$"0000 0009 0000 0000 0000 0000 0000 0009"            /* ...∆...........∆ */
	$"0000 0000 0000 0000 0000 000A 0000 0000"            /* ...........¬.... */
	$"0000 0000 0000 000A 0000 0000 0000 0000"            /* .......¬........ */
	$"0000 0006 0000 0000 0000 0000 0000 0006"            /* ................ */
	$"0000 0000 0000 0000 0000 0006 0000 0000"            /* ................ */
	$"0000 0000 0000 0009 0000 0000 0000 0000"            /* .......∆........ */
	$"0000 0009 0000 0000 0000 0000 0000 0006"            /* ...∆............ */
	$"0000 0000 0000 0000 0000 0009 0000 0000"            /* ...........∆.... */
	$"0000 0000 0000 0006 0000 0000 0000 0000"            /* ................ */
	$"0000 000A 0000 0000 0000 0000 0000 0009"            /* ...¬...........∆ */
	$"0000 0000 0000 0000 0000 000A 0000 0000"            /* ...........¬.... */
	$"0000 0000 0000 000A 0000 0000 0000 0000"            /* .......¬........ */
	$"0000"                                               /* .. */
};

data 'DLGX' (184) {
	$"0843 6861 7263 6F61 6C00 0000 0000 0000"            /* .Charcoal....... */
	$"0000 0000 0000 0000 0000 0000 0000 0000"            /* ................ */
	$"0000 0000 0000 0000 0000 0000 0000 0000"            /* ................ */
	$"0000 0000 0000 0000 0000 0000 0000 0000"            /* ................ */
	$"000C 0000 0000 0000 0004 0004 0000 0000"            /* ................ */
	$"0009 0000 0000 0000 0000 0000 0000 0001"            /* .∆.............. */
	$"0000 0000 0000 0000 0000 000A 0000 0000"            /* ...........¬.... */
	$"0000 0000 0000 0002 0000 0000 0000 0000"            /* ................ */
	$"0000 0002 0000 0000 0000 0000 0000 0003"            /* ................ */
	$"0000 0000 0000 0000 0000 0004 0000 0000"            /* ................ */
	$"0000 0000 0000 0004 0000 0000 0000 0000"            /* ................ */
	$"0000 0006 0000 0000 0000 0000 0000"                 /* .............. */
};

data 'DLGX' (146) {
	$"0647 656E 6576 6100 0000 0000 0000 0000"            /* .Geneva......... */
	$"0000 0000 0000 0000 0000 0000 0000 0000"            /* ................ */
	$"0000 0000 0000 0000 0000 0000 0000 0000"            /* ................ */
	$"0000 0000 0000 0000 0000 0000 0000 0000"            /* ................ */
	$"0009 0000 0000 0000 0004 0004 0000 0000"            /* .∆.............. */
	$"003D 0000 0000 0000 0000 0000 0000 0001"            /* .=.............. */
	$"0000 0000 0000 0000 0000 0007 0000 0000"            /* ................ */
	$"0000 0000 0000 0007 0000 0000 0000 0000"            /* ................ */
	$"0000 0007 0000 0000 0000 0000 0000 0007"            /* ................ */
	$"0000 0000 0000 0000 0000 0007 0000 0000"            /* ................ */
	$"0000 0000 0000 0006 0000 0000 0000 0000"            /* ................ */
	$"0000 0006 0000 0000 0000 0000 0000 0006"            /* ................ */
	$"0000 0000 0000 0000 0000 0006 0000 0000"            /* ................ */
	$"0000 0000 0000 0006 0000 0000 0000 0000"            /* ................ */
	$"0000 000A 0000 0000 0000 0000 0000 000A"            /* ...¬...........¬ */
	$"0000 0000 0000 0000 0000 0004 0000 0000"            /* ................ */
	$"0000 0000 0000 0006 0000 0000 0000 0000"            /* ................ */
	$"0000 000A 0000 0000 0000 0000 0000 0007"            /* ...¬............ */
	$"0000 0000 0000 0000 0000 0006 0000 0000"            /* ................ */
	$"0000 0000 0000 0006 0000 0000 0000 0000"            /* ................ */
	$"0000 0006 0000 0000 0000 0000 0000 0003"            /* ................ */
	$"0000 0000 0000 0000 0000 0009 0000 0000"            /* ...........∆.... */
	$"0000 0000 0000 0009 0000 0000 0000 0000"            /* .......∆........ */
	$"0000 000A 0000 0000 0000 0000 0000 0006"            /* ...¬............ */
	$"0000 0000 0000 0000 0000 0006 0000 0000"            /* ................ */
	$"0000 0000 0000 0006 0000 0000 0000 0000"            /* ................ */
	$"0000 0009 0000 0000 0000 0000 0000 0006"            /* ...∆............ */
	$"0000 0000 0000 0000 0000 0009 0000 0000"            /* ...........∆.... */
	$"0000 0000 0000 0007 0000 0000 0000 0000"            /* ................ */
	$"0000 0004 0000 0000 0000 0000 0000 0006"            /* ................ */
	$"0000 0000 0000 0000 0000 0004 0000 0000"            /* ................ */
	$"0000 0000 0000 0003 0000 0000 0000 0000"            /* ................ */
	$"0000 0009 0000 0000 0000 0000 0000 000A"            /* ...∆...........¬ */
	$"0000 0000 0000 0000 0000 0003 0000 0000"            /* ................ */
	$"0000 0000 0000 0003 0000 0000 0000 0000"            /* ................ */
	$"0000 0003 0000 0000 0000 0000 0000 0006"            /* ................ */
	$"0000 0000 0000 0000 0000 0003 0000 0000"            /* ................ */
	$"0000 0000 0000 000A 0000 0000 0000 0000"            /* .......¬........ */
	$"0000 0007 0000 0000 0000 0000 0000 0006"            /* ................ */
	$"0000 0000 0000 0000 0000 0006 0000 0000"            /* ................ */
	$"0000 0000 0000 0004 0000 0000 0000 0000"            /* ................ */
	$"0000 0009 0000 0000 0000 0000 0000 0009"            /* ...∆...........∆ */
	$"0000 0000 0000 0000 0000 0006 0000 0000"            /* ................ */
	$"0000 0000 0000 0006 0000 0000 0000 0000"            /* ................ */
	$"0000 0009 0000 0000 0000 0000 0000 0006"            /* ...∆............ */
	$"0000 0000 0000 0000 0000 0006 0000 0000"            /* ................ */
	$"0000 0000 0000 0009 0000 0000 0000 0000"            /* .......∆........ */
	$"0000 0006 0000 0000 0000 0000 0000 0002"            /* ................ */
	$"0000 0000 0000 0000 0000 0002 0000 0000"            /* ................ */
	$"0000 0000 0000 0002 0000 0000 0000 0000"            /* ................ */
	$"0000 0002 0000 0000 0000 0000 0000"                 /* .............. */
};

data 'DLGX' (143) {
	$"0843 6861 7263 6F61 6C00 0000 0000 0000"            /* .Charcoal....... */
	$"0000 0000 0000 0000 0000 0000 0000 0000"            /* ................ */
	$"0000 0000 0000 0000 0000 0000 0000 0000"            /* ................ */
	$"0000 0000 0000 0000 0000 0000 0000 0000"            /* ................ */
	$"000C 0000 0000 0000 0004 0004 0000 0000"            /* ................ */
	$"002D 000A 0000 0000 0000 0000 0000 000A"            /* .-.¬...........¬ */
	$"0000 0000 0000 0000 0000 000A 0000 0000"            /* ...........¬.... */
	$"0000 0000 0000 000A 0000 0000 0000 0000"            /* .......¬........ */
	$"0000 000A 0000 0000 0000 0000 0000 000A"            /* ...¬...........¬ */
	$"0000 0000 0000 0000 0000 000A 0000 0000"            /* ...........¬.... */
	$"0000 0000 0000 0006 0000 0000 0000 0000"            /* ................ */
	$"0000 0006 0000 0000 0000 0000 0000 000A"            /* ...............¬ */
	$"0000 0000 0000 0000 0000 000A 0000 0000"            /* ...........¬.... */
	$"0000 0000 0000 0006 0000 0000 0000 0000"            /* ................ */
	$"0000 0006 0000 0000 0000 0000 0000 0006"            /* ................ */
	$"0000 0000 0000 0000 0000 0006 0000 0000"            /* ................ */
	$"0000 0000 0000 0006 0000 0000 0000 0000"            /* ................ */
	$"0000 0006 0000 0000 0000 0000 0000 0009"            /* ...............∆ */
	$"0000 0000 0000 0000 0000 0009 0000 0000"            /* ...........∆.... */
	$"0000 0000 0000 0006 0000 0000 0000 0000"            /* ................ */
	$"0000 000A 0000 0000 0000 0000 0000 000A"            /* ...¬...........¬ */
	$"0000 0000 0000 0000 0000 0009 0000 0000"            /* ...........∆.... */
	$"0000 0000 0000 0009 0000 0000 0000 0000"            /* .......∆........ */
	$"0000 000A 0000 0000 0000 0000 0000 0009"            /* ...¬...........∆ */
	$"0000 0000 0000 0000 0000 0009 0000 0000"            /* ...........∆.... */
	$"0000 0000 0000 0006 0000 0000 0000 0000"            /* ................ */
	$"0000 0006 0000 0000 0000 0000 0000 0006"            /* ................ */
	$"0000 0000 0000 0000 0000 000A 0000 0000"            /* ...........¬.... */
	$"0000 0000 0000 000A 0000 0000 0000 0000"            /* .......¬........ */
	$"0000 000A 0000 0000 0000 0000 0000 000A"            /* ...¬...........¬ */
	$"0000 0000 0000 0000 0000 000A 0000 0000"            /* ...........¬.... */
	$"0000 0000 0000 0009 0000 0000 0000 0000"            /* .......∆........ */
	$"0000 000A 0000 0000 0000 0000 0000 0009"            /* ...¬...........∆ */
	$"0000 0000 0000 0000 0000 0006 0000 0000"            /* ................ */
	$"0000 0000 0000 000A 0000 0000 0000 0000"            /* .......¬........ */
	$"0000 0009 0000 0000 0000 0000 0000 000A"            /* ...∆...........¬ */
	$"0000 0000 0000 0000 0000 0009 0000 0000"            /* ...........∆.... */
	$"0000 0000 0000 000A 0000 0000 0000 0000"            /* .......¬........ */
	$"0000 000A 0000 0000 0000 0000 0000"                 /* ...¬.......... */
};

data 'DLGX' (142) {
	$"0843 6861 7263 6F61 6C00 0000 0000 0000"            /* .Charcoal....... */
	$"0000 0000 0000 0000 0000 0000 0000 0000"            /* ................ */
	$"0000 0000 0000 0000 0000 0000 0000 0000"            /* ................ */
	$"0000 0000 0000 0000 0000 0000 0000 0000"            /* ................ */
	$"000C 0000 0000 0000 0004 0004 0000 0000"            /* ................ */
	$"0021 000A 0000 0000 0000 0000 0000 000A"            /* .!.¬...........¬ */
	$"0000 0000 0000 0000 0000 000A 0000 0000"            /* ...........¬.... */
	$"0000 0000 0000 000A 0000 0000 0000 0000"            /* .......¬........ */
	$"0000 0006 0000 0000 0000 0000 0000 0006"            /* ................ */
	$"0000 0000 0000 0000 0000 0006 0000 0000"            /* ................ */
	$"0000 0000 0000 0006 0000 0000 0000 0000"            /* ................ */
	$"0000 0006 0000 0000 0000 0000 0000 0006"            /* ................ */
	$"0000 0000 0000 0000 0000 0006 0000 0000"            /* ................ */
	$"0000 0000 0000 0006 0000 0000 0000 0000"            /* ................ */
	$"0000 0006 0000 0000 0000 0000 0000 0006"            /* ................ */
	$"0000 0000 0000 0000 0000 0006 0000 0000"            /* ................ */
	$"0000 0000 0000 0006 0000 0000 0000 0000"            /* ................ */
	$"0000 0006 0000 0000 0000 0000 0000 0006"            /* ................ */
	$"0000 0000 0000 0000 0000 000A 0000 0000"            /* ...........¬.... */
	$"0000 0000 0000 0005 0000 0000 0000 0000"            /* ................ */
	$"0000 000A 0000 0000 0000 0000 0000 000A"            /* ...¬...........¬ */
	$"0000 0000 0000 0000 0000 000A 0000 0000"            /* ...........¬.... */
	$"0000 0000 0000 000A 0000 0000 0000 0000"            /* .......¬........ */
	$"0000 0006 0000 0000 0000 0000 0000 0006"            /* ................ */
	$"0000 0000 0000 0000 0000 000A 0000 0000"            /* ...........¬.... */
	$"0000 0000 0000 000A 0000 0000 0000 0000"            /* .......¬........ */
	$"0000 000A 0000 0000 0000 0000 0000 000A"            /* ...¬...........¬ */
	$"0000 0000 0000 0000 0000 000A 0000 0000"            /* ...........¬.... */
	$"0000 0000 0000 0005 0000 0000 0000 0000"            /* ................ */
	$"0000 000A 0000 0000 0000 0000 0000"                 /* ...¬.......... */
};

data 'cctb' (200) {
	$"0000 0000 0000 0003 0000 DEF7 DEF7 DEF7"            /* ..........ﬁ˜ﬁ˜ﬁ˜ */
	$"0001 FFFF FFFF FFFF 0002 0000 0000 0000"            /* ..ˇˇˇˇˇˇ........ */
	$"0003 FFFF FFFF FFFF"                                /* ..ˇˇˇˇˇˇ */
};

data 'CNTL' (200, purgeable) {
	$"0073 0203 0087 0217 0000 0100 00AB 0301"            /* .s...á.......´.. */
	$"0021 0000 0000 00"                                  /* .!..... */
};

