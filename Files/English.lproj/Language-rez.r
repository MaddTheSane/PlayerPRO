#error this Rez file is outdated. Please don't use
#error unless you update it.
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
	$"6E65 732C 2073 7563 6820 7468 6174 2028"            /* nes, such that (*/
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
	$"2031 292C 2073 7563 6820 7468 6174 2028"            /*  1), such that (*/
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
	$"6E64 2074 6865 206E 6F74 6520 4233 2028"            /* nd the note B3 (*/
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
	$"3230 3320 3734 2036 320D 4661 783A 2028"            /* 203 74 62.Fax: (*/
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

resource 'STR#' (132, "Note") {
	{	/* array StringArray: 36 elements */
		/* [1] */
		"B 6",
		/* [2] */
		"B#6",
		/* [3] */
		"A 6",
		/* [4] */
		"A#6",
		/* [5] */
		"G#6",
		/* [6] */
		"G 6",
		/* [7] */
		"F#6",
		/* [8] */
		"F 6",
		/* [9] */
		"E 6",
		/* [10] */
		"D#6",
		/* [11] */
		"D 6",
		/* [12] */
		"C#6",
		/* [13] */
		"C 6",
		/* [14] */
		"B 5",
		/* [15] */
		"A#5",
		/* [16] */
		"G#5",
		/* [17] */
		"G 5",
		/* [18] */
		"F#5",
		/* [19] */
		"F 5",
		/* [20] */
		"E 5",
		/* [21] */
		"D#5",
		/* [22] */
		"D 5",
		/* [23] */
		"C#5",
		/* [24] */
		"C 5",
		/* [25] */
		"B 4",
		/* [26] */
		"A#4",
		/* [27] */
		"A 4",
		/* [28] */
		"G#4",
		/* [29] */
		"G 4",
		/* [30] */
		"F#4",
		/* [31] */
		"F 4",
		/* [32] */
		"E 4",
		/* [33] */
		"D#4",
		/* [34] */
		"D 4",
		/* [35] */
		"C#4",
		/* [36] */
		"C 4"
	}
};

resource 'STR#' (129, "ScreenSaver") {
	{	/* array StringArray: 7 elements */
		/* [1] */
		"PlayerPRO 5.0",
		/* [2] */
		"You can now",
		/* [3] */
		"order your",
		/* [4] */
		"PlayerPRO CD-Rom",
		/* [5] */
		"with 3500 musics",
		/* [6] */
		"500 instruments",
		/* [7] */
		"and music tools"
	}
};

resource 'STR#' (200, "Instruments list help") {
	{	/* array StringArray: 9 elements */
		/* [1] */
		"Create a new sound into the selected ins"
		"trument cell. Choose the method.",
		/* [2] */
		"Save the selected instrument into a file"
		".",
		/* [3] */
		"Load a sound file into the selected inst"
		"rument cell.",
		/* [4] */
		"Delete the selected instrument.",
		/* [5] */
		"More information about the selected inst"
		"rument.",
		/* [6] */
		"The color indicates the track on which t"
		"he instrument is used.",
		/* [7] */
		"List of instruments used in the current "
		"song.",
		/* [8] */
		"Play the selected instrument/sample.",
		/* [9] */
		"Show the waveform of the selected instru"
		"ment."
	}
};

resource 'STR#' (1000, "PatList Help") {
	{	/* array StringArray: 7 elements */
		/* [1] */
		"Create a new pattern and add it to the l"
		"ist.",
		/* [2] */
		"Load a pattern file and add it to the li"
		"st.",
		/* [3] */
		"Save the selected pattern into a file.",
		/* [4] */
		"Delete the selected pattern.",
		/* [5] */
		"Get more information about the selected "
		"pattern.",
		/* [6] */
		"Display the selected pattern.",
		/* [7] */
		"The pattern list used in the current son"
		"g."
	}
};

resource 'STR#' (1200, "Adaptors Help") {
	{	/* array StringArray: 10 elements */
		/* [1] */
		"Load an 'adaptors' file and replace curr"
		"ent settings.",
		/* [2] */
		"Save current settings into a 'adaptors' "
		"file.",
		/* [3] */
		"Reset speed setting to standard settings"
		".",
		/* [4] */
		"Set up left/right volume based on track "
		"no 1.",
		/* [5] */
		"Change the playing pitch. Click on it.",
		/* [6] */
		"Change the playing speed. Click on it.",
		/* [7] */
		"Change the playing volume (HARDWARE volu"
		"me). Click on it.",
		/* [8] */
		"Change volume/panning for each track. Cl"
		"ick on it. Use 'option' key to change al"
		"l tracks.",
		/* [9] */
		"Change the playing volume (SOFTWARE volu"
		"me). Click on it.",
		/* [10] */
		"Reset pitch setting to standard settings"
		"."
	}
};

resource 'STR#' (1400, "Music List Help") {
	{	/* array StringArray: 9 elements */
		/* [1] */
		"Sort the music list alphabetically.",
		/* [2] */
		"Add a new music file to the music list.",
		/* [3] */
		"Remove the selected music file from the "
		"list.",
		/* [4] */
		"Change information of the selected music"
		" file.",
		/* [5] */
		"Load the selected music file into memory"
		" and play it.",
		/* [6] */
		"Open the preference window for the music"
		" list.",
		/* [7] */
		"More information about the selected musi"
		"c file.",
		/* [8] */
		"The music list - to open a music file, d"
		"ouble click on its title.",
		/* [9] */
		"The position of the selected music file "
		"on your hard disk."
	}
};

resource 'STR#' (1600, "Tools Help") {
	{	/* array StringArray: 15 elements */
		/* [1] */
		"Go to previous pattern.",
		/* [2] */
		"Play song backwards.",
		/* [3] */
		"Stop the current song.",
		/* [4] */
		"Play the current song.",
		/* [5] */
		"Fast forward.",
		/* [6] */
		"Go to next pattern.",
		/* [7] */
		"Indicate the progression of the song. Cl"
		"ick in it to change the position.",
		/* [8] */
		"Filename of the current song in memory.",
		/* [9] */
		"Instrument of the selected cell (see Dig"
		"ital Editor).",
		/* [10] */
		"Note of the selected cell (see Digital E"
		"ditor).",
		/* [11] */
		"Effect of the selected cell (see Digital"
		" Editor).",
		/* [12] */
		"Argument of the selected cell (see Digit"
		"al Editor).",
		/* [13] */
		"Play the selected cell (see Digital Edit"
		"or).",
		/* [14] */
		"Delete the selected cell (see Digital Ed"
		"itor).",
		/* [15] */
		"Apply modifications to all selected cell"
		"s (see Digital Editor)."
	}
};

resource 'STR#' (1800, "Instrument Help") {
	{	/* array StringArray: 9 elements */
		/* [1] */
		"Change information about this instrument"
		".",
		/* [2] */
		"Switch to selection mode: to apply plugs"
		", cut, copy and paste the waveform.",
		/* [3] */
		"Switch to pencil mode: to redraw the wav"
		"eform.",
		/* [4] */
		"Switch to zoom IN/OUT mode. To zoom IN: "
		"click, to zoom OUT: click + 'option' key"
		".",
		/* [5] */
		"Apply a plug to the selection. If nothin"
		"g is selected, to the whole waveform.",
		/* [6] */
		"The waveform of the instrument.",
		/* [7] */
		"Switch Envelope ON/OFF",
		/* [8] */
		"Switch Sustain Point ON/OFF",
		/* [9] */
		"Switch Envelope Loop ON/OFF"
	}
};

resource 'STR#' (2200, "Classic Help") {
	{	/* array StringArray: 6 elements */
		/* [1] */
		"More information about this pattern.",
		/* [2] */
		"Switch to play mode. Click the pattern t"
		"o play it.",
		/* [3] */
		"Switch to zoom IN/OUT. To zoom IN: click"
		", to zoom OUT: click + 'option' key.",
		/* [4] */
		"Select the tracks to be displayed.",
		/* [5] */
		"Select the instruments to be displayed.",
		/* [6] */
		"The pattern."
	}
};

resource 'STR#' (2400, "Oscillo Help") {
	{	/* array StringArray: 3 elements */
		/* [1] */
		"Change the oscillo source.",
		/* [2] */
		"Change the oscillo height.",
		/* [3] */
		"The oscilloscope."
	}
};

resource 'STR#' (2600, "Classical Editor Help") {
	{	/* array StringArray: 12 elements */
		/* [1] */
		"Pop-up menu to change track.",
		/* [2] */
		"Pop-up menu to select an instrument: use"
		"d in Note mode when creating new note.",
		/* [3] */
		"Pop-up menu to select an effect: used in"
		" Note mode when creating new note.",
		/* [4] */
		"Get more information about the current p"
		"attern.",
		/* [5] */
		"Open the preference window for the class"
		"ical editor.",
		/* [6] */
		"Show all tracks, not only the current tr"
		"ack.",
		/* [7] */
		"Switch to Note mode. You can now drop/mo"
		"ve notes on the partition.",
		/* [8] */
		"Switch to Selection mode. You can now se"
		"lect/move notes on the partition.",
		/* [9] */
		"Switch to Play mode. You can now click o"
		"n partition and hear notes made by the c"
		"urrent instrument.",
		/* [10] */
		"Switch to Delete mode: click on notes to"
		" delete them.",
		/* [11] */
		"A small view of the partition. Click on "
		"it to play it.",
		/* [12] */
		"Switch to Zoom mode. You can now zoom in"
		"/out in the partition."
	}
};

resource 'STR#' (4000, "Spectrum Help") {
	{	/* array StringArray: 4 elements */
		/* [1] */
		"Change the spectrum source.",
		/* [2] */
		"Change the spectrum height.",
		/* [3] */
		"The spectrum. Click on it to find a spec"
		"ific frequency.",
		/* [4] */
		"Change the spectrum scale : linear or lo"
		"garithmic."
	}
};

resource 'STR#' (5400, "Classical Editor Help") {
	{	/* array StringArray: 10 elements */
		/* [1] */
		"Switch to note editing mode (Tab key)",
		/* [2] */
		"Switch to select mode (Tab key)",
		/* [3] */
		"Load a Pcmd file and paste it at current"
		" position.",
		/* [4] */
		"Play current selection (Use select mode)"
		".",
		/* [5] */
		"Save current selection in a Pcmd file.",
		/* [6] */
		"Get information about current pattern.",
		/* [7] */
		"Apply a filter to current selection.",
		/* [8] */
		"Open Preferences window.",
		/* [9] */
		"Select note length to use in note editin"
		"g mode.",
		/* [10] */
		"Select instrument to use in note editing"
		" mode."
	}
};

resource 'STR#' (2000, "Piano Help") {
	{	/* array StringArray: 3 elements */
		/* [1] */
		"The Piano: click on it to hear the selec"
		"ted instrument (see instrument list).",
		/* [2] */
		"Shift Key-Map to left",
		/* [3] */
		"Open the Piano's Preference window."
	}
};

resource 'STR#' (4200, "Partition Help") {
	{	/* array StringArray: 7 elements */
		/* [1] */
		"Open the selected pattern.",
		/* [2] */
		"Indicate partition length. Click on it t"
		"o change.",
		/* [3] */
		"PopUp-menu to change the pattern number "
		"at this position.",
		/* [4] */
		"The partition with pattern number (see '"
		"Patterns list').",
		/* [5] */
		"Get more information about the selected "
		"pattern.",
		/* [6] */
		"Add a row in the partition",
		/* [7] */
		"Remove this row in the partition"
	}
};

resource 'STR#' (400, "Digital Editor Help") {
	{	/* array StringArray: 21 elements */
		/* [1] */
		"Open the Digital Editor's Preference win"
		"dow.",
		/* [2] */
		"Load a Pcmd file and paste it as current"
		" selection.",
		/* [3] */
		"Save current selection into a Pcmd file.",
		/* [4] */
		"Transpose up notes of current selection "
		"by 1. With shift key: only current instr"
		"ument. (= Key '*')",
		/* [5] */
		"Get more information about the current p"
		"attern.",
		/* [6] */
		"Play the current selection.",
		/* [7] */
		"Switch in record mode: press on your Mac"
		" keyboard to enter new notes.",
		/* [8] */
		"Open the command editor to edit the sele"
		"cted cell.",
		/* [9] */
		"Track ID and color.",
		/* [10] */
		"Position ID.",
		/* [11] */
		"The current pattern: click to select a n"
		"ote, use click + 'shift' key to expand a"
		" selection.",
		/* [12] */
		"Apply an command plug to current selecti"
		"on.",
		/* [13] */
		"Switch Pattern Looping ON/OFF",
		/* [14] */
		"Open Find Window",
		/* [15] */
		"Transpose down notes of current selectio"
		"n by 1. With shift key: only current ins"
		"trument.(= Key '/')",
		/* [16] */
		"Fill selected area with default values",
		/* [17] */
		"Set default Value",
		/* [18] */
		"Set default Value",
		/* [19] */
		"Set default Value",
		/* [20] */
		"Set default Value",
		/* [21] */
		"Set step when pressing arrows or return"
	}
};

resource 'STR#' (133, "Internet Menu") {
	{	/* array StringArray: 3 elements */
		/* [1] */
		"http://www.quadmation.com/pphome.htm",
		/* [2] */
		"http://hyperarchive.lcs.mit.edu/cgi-bin/"
		"NewSearch?key=PlayerPRO",
		/* [3] */
		"mailto:rossetantoine@bluewin.ch"
	}
};

resource 'STR#' (130, "Effects") {
	{	/* array StringArray: 17 elements */
		/* [1] */
		"0-Normal/Arpeggio",
		/* [2] */
		"1-Slide Up",
		/* [3] */
		"2-Slide Down",
		/* [4] */
		"3-Portamento",
		/* [5] */
		"4-\0x00Vibrato",
		/* [6] */
		"5-Portamento+Vol",
		/* [7] */
		"6-Vibrato+Vol Slide",
		/* [8] */
		"7-Tremolo",
		/* [9] */
		"8-Set Panning",
		/* [10] */
		"9-Set SampleOffset",
		/* [11] */
		"A-VolumeSlide",
		/* [12] */
		"B-Position Jump",
		/* [13] */
		"C-Set Volume",
		/* [14] */
		"D-Pattern Break",
		/* [15] */
		"E-E Commands",
		/* [16] */
		"F-Set Speed",
		/* [17] */
		"G-Note Off"
	}
};

resource 'STR#' (131) {
	{	/* array StringArray: 70 elements */
		/* [1] */
		"PlayerPRO 5",
		/* [2] */
		"http://www.quadmation.com",
		/* [3] */
		"Special thanks",
		/* [4] */
		"&",
		/* [5] */
		"Salutations",
		/* [6] */
		"to",
		/* [7] */
		"Chantal Mulhaupt (Chan-Chan)",
		/* [8] */
		"Dario Accornero",
		/* [9] */
		"Samuel Lindeman",
		/* [10] */
		"Mark Cookson",
		/* [11] */
		"Joël Barrière (French Man)",
		/* [12] */
		"Petteri Lajunen (Pest/TranZe)",
		/* [13] */
		"Steve Jobs (Mac Specialist)",
		/* [14] */
		"John Stiles",
		/* [15] */
		"All Japan users!",
		/* [16] */
		"Félix Lapointe",
		/* [17] */
		"Hubert Garrido",
		/* [18] */
		"Lavallee Marc",
		/* [19] */
		"Diaz Miguel",
		/* [20] */
		"Christian Lacroix",
		/* [21] */
		"Laurent Criado",
		/* [22] */
		"Styx...",
		/* [23] */
		"Topaze",
		/* [24] */
		"Doctor Yelm",
		/* [25] */
		"Master Byakhee",
		/* [26] */
		"Doctor PM",
		/* [27] */
		"My new Honda VTR1000",
		/* [28] */
		"Le Chninkel",
		/* [29] */
		"Osman Ratib & UIN",
		/* [30] */
		"Yves Ligier",
		/* [31] */
		"Christian Girard",
		/* [32] */
		"Eric",
		/* [33] */
		"Géraldine",
		/* [34] */
		"Corinne",
		/* [35] */
		"Laurent",
		/* [36] */
		"Cédric Lombard",
		/* [37] */
		"Eric Laporte",
		/* [38] */
		"Dorothée with GueuleDeRak",
		/* [39] */
		"Bungie Software",
		/* [40] */
		"Daniel Leschnziner",
		/* [41] */
		"Gah-Gah",
		/* [42] */
		"Carlos Serano",
		/* [43] */
		"Trm II/4 -- 5",
		/* [44] */
		"François Rossi",
		/* [45] */
		"Luc Deschenaux",
		/* [46] */
		"Apple Computer",
		/* [47] */
		"Gilles Burnier",
		/* [48] */
		"Canal +",
		/* [49] */
		"Jacqueline Meyer",
		/* [50] */
		"HCUG & CMU",
		/* [51] */
		"All Rx lovers of this planet",
		/* [52] */
		"Marcus Schlapfer",
		/* [53] */
		"Stephen Altrichter",
		/* [54] */
		"All the thons of the vollée",
		/* [55] */
		"Sydney Children’s Hospital",
		/* [56] */
		"Hôpital Cochin Paris",
		/* [57] */
		"Les Leuchter",
		/* [58] */
		"Les Benani",
		/* [59] */
		"Restaurant Hong-Kong",
		/* [60] */
		"Bossey France",
		/* [61] */
		"Motorola",
		/* [62] */
		"Cat Mathilde (Now dead...)",
		/* [63] */
		"Cat Doux-Doux (Smooth-Smooth)",
		/* [64] */
		"My Mac computer (G3/300)",
		/* [65] */
		"Daystar",
		/* [66] */
		"Nantucket Island",
		/* [67] */
		"Charles Hotel Boston",
		/* [68] */
		"Chronologie",
		/* [69] */
		"Ellis FRANCE",
		/* [70] */
		"(+33) 143 33 15 47"
	}
};

resource 'MENU' (128) {
	128,
	textMenuProc,
	0x7FF7DB7F,
	enabled,
	"File",
	{	/* array: 21 elements */
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
		"Export as...", noIcon, hierarchicalMenu, "ã", plain,
		/* [7] */
		"Reset", noIcon, "R", noMark, plain,
		/* [8] */
		"-", noIcon, noKey, noMark, plain,
		/* [9] */
		"Save Music List...", noIcon, noKey, noMark, plain,
		/* [10] */
		"Clear Music List", noIcon, noKey, noMark, plain,
		/* [11] */
		"-", noIcon, noKey, noMark, plain,
		/* [12] */
		"Page SetUp...", noIcon, noKey, noMark, plain,
		/* [13] */
		"Print...", noIcon, noKey, noMark, plain,
		/* [14] */
		"-", noIcon, noKey, noMark, plain,
		/* [15] */
		"Preferences...", noIcon, hierarchicalMenu, "»", plain,
		/* [16] */
		"Windows Sets...", noIcon, hierarchicalMenu, "∞", plain,
		/* [17] */
		"Sound Control Panel...", noIcon, noKey, noMark, plain,
		/* [18] */
		"OMS Studio Setup...", noIcon, noKey, noMark, plain,
		/* [19] */
		"OMS MIDI Setup...", noIcon, noKey, noMark, plain,
		/* [20] */
		"-", noIcon, noKey, noMark, plain,
		/* [21] */
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

resource 'MENU' (137) {
	137,
	textMenuProc,
	allEnabled,
	enabled,
	"Title",
	{	/* array: 0 elements */
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
	0x7FFFFFF9,
	enabled,
	"Export",
	{	/* array: 3 elements */
		/* [1] */
		"AIFF Sound", noIcon, noKey, noMark, plain,
		/* [2] */
		"MacOS Music", noIcon, noKey, noMark, plain,
		/* [3] */
		"-", noIcon, noKey, noMark, plain
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

resource 'MENU' (145) {
	145,
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

resource 'MENU' (146) {
	146,
	textMenuProc,
	allEnabled,
	enabled,
	"Title",
	{	/* array: 0 elements */
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
	{	/* array: 11 elements */
		/* [1] */
		"MADI", noIcon, noKey, noMark, plain,
		/* [2] */
		"MADH", noIcon, noKey, noMark, plain,
		/* [3] */
		"MADF", noIcon, noKey, noMark, plain,
		/* [4] */
		"STrk - MOD", noIcon, noKey, noMark, plain,
		/* [5] */
		"MTM ", noIcon, noKey, noMark, plain,
		/* [6] */
		"S3M ", noIcon, noKey, noMark, plain,
		/* [7] */
		"OKTA", noIcon, noKey, noMark, plain,
		/* [8] */
		"6669", noIcon, noKey, noMark, plain,
		/* [9] */
		"Midi", noIcon, noKey, noMark, plain,
		/* [10] */
		"XM  ", noIcon, noKey, noMark, plain,
		/* [11] */
		"IT  ", noIcon, noKey, noMark, plain
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
		"MADI PlayerPRO", noIcon, noKey, noMark, plain,
		/* [6] */
		"-", noIcon, noKey, noMark, plain
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
	allEnabled,
	enabled,
	"Mode",
	{	/* array: 2 elements */
		/* [1] */
		"Mono", noIcon, noKey, noMark, plain,
		/* [2] */
		"Stereo", noIcon, noKey, noMark, plain
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

resource 'MENU' (142) {
	142,
	textMenuProc,
	allEnabled,
	enabled,
	"Tracks Number",
	{	/* array: 0 elements */
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

resource 'MENU' (130) {
	130,
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
		"New...", noIcon, hierarchicalMenu, "ö", plain,
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

resource 'MENU' (133) {
	133,
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

resource 'MENU' (175) {
	175,
	textMenuProc,
	allEnabled,
	enabled,
	"OverSampling",
	{	/* array: 6 elements */
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
		"10 x", noIcon, noKey, noMark, plain
	}
};

resource 'MENU' (154) {
	154,
	textMenuProc,
	allEnabled,
	enabled,
	"Title",
	{	/* array: 6 elements */
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
		"Audio CD Import...", noIcon, noKey, noMark, plain
	}
};

resource 'MENU' (132) {
	132,
	textMenuProc,
	0x7FFFFFBC,
	enabled,
	"Edit",
	{	/* array: 10 elements */
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
		"Get Info...", noIcon, "I", noMark, plain
	}
};

resource 'MENU' (140) {
	140,
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

resource 'MENU' (135) {
	135,
	textMenuProc,
	allEnabled,
	enabled,
	"Effect:",
	{	/* array: 17 elements */
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
		"9-Set SampleOffset", noIcon, noKey, noMark, plain,
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
		"G-Note OFF (Multi-Channel Tracks)", noIcon, noKey, noMark, plain
	}
};

resource 'MENU' (179) {
	179,
	textMenuProc,
	0x7FFFFFD7,
	enabled,
	"Internet",
	{	/* array: 6 elements */
		/* [1] */
		"PlayerPRO Official Site", noIcon, noKey, noMark, plain,
		/* [2] */
		"MIT Archive for PlayerPRO", noIcon, noKey, noMark, plain,
		/* [3] */
		"Email to the author of PlayerPRO", noIcon, noKey, noMark, plain,
		/* [4] */
		"-", noIcon, noKey, noMark, plain,
		/* [5] */
		"Add/Remove bookmarks...", noIcon, noKey, noMark, plain,
		/* [6] */
		"-", noIcon, noKey, noMark, plain
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

resource 'DLOG' (158, "Debug") {
	{116, 78, 224, 395},
	movableDBoxProc,
	visible,
	goAway,
	0x0,
	623,
	"Performance Window",
	centerMainScreen
};

data 'DLOG' (163, "Memory") {
	$"004A 0096 00C3 01C6 000C 0000 0100 0000"            /* .J.ñ.√.∆........ */
	$"0013 016C 064D 656D 6F72 79"                        /* ...l.Memory */
};

data 'DLOG' (164, "Spectrum") {
	$"0070 006A 00F0 01DC 0008 0000 0100 0000"            /* .p.j..‹........ */
	$"0014 0151 0853 7065 6374 7275 6D"                   /* ...Q.Spectrum */
};

resource 'DLOG' (167, "Pattern Info") {
	{96, 164, 221, 414},
	movableDBoxProc,
	invisible,
	goAway,
	0x0,
	217,
	"Pattern Info",
	centerMainScreen
};

data 'DLOG' (168, "File information") {
	$"0065 0083 010B 01DF 0005 0000 0100 0000"            /* .e.É...ﬂ........ */
	$"03C6 013D 1046 696C 6520 496E 666F 726D"            /* .∆.=.File Inform */
	$"6174 696F 6E"                                       /* ation */
};

resource 'DLOG' (169, "Help PP") {
	{76, 110, 339, 591},
	zoomDocProc,
	invisible,
	goAway,
	0x16,
	195,
	"Help",
	centerMainScreen
};

resource 'DLOG' (161, "SoundFormat Export") {
	{81, 77, 165, 341},
	movableDBoxProc,
	invisible,
	goAway,
	0x1C8F,
	171,
	"Sound File Export",
	centerMainScreen
};

data 'DLOG' (170, "MIDI Channels") {
	$"009C 0124 01E4 0348 0005 0100 0100 0000"            /* .ú.$.‰.H........ */
	$"0000 0156 134D 4944 4920 4368 616E 6E65"            /* ...V.MIDI Channe */
	$"6C73 2053 6574 7570"                                /* ls Setup */
};

resource 'DLOG' (172, "Move") {
	{92, 12, 229, 535},
	movableDBoxProc,
	visible,
	goAway,
	0x0,
	172,
	"Move notes",
	centerMainScreen
};

data 'DLOG' (179, "OpenDialog Sample/Instrument") {
	$"002C 002A 0198 0181 0005 0000 0000 0000"            /* .,.*.ò.Å........ */
	$"0000 00A1 0B4F 7065 6E20 4469 616C 6F67"            /* ...°.Open Dialog */
};

resource 'DLOG' (171, "StartUp Dialog") {
	{86, 73, 301, 458},
	dBoxProc,
	visible,
	goAway,
	0x0,
	346,
	"",
	centerMainScreen
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

resource 'DLOG' (3000, "General Error") {
	{52, 49, 194, 392},
	movableDBoxProc,
	invisible,
	goAway,
	0x270C,
	268,
	"Error",
	alertPositionParentWindowScreen
};

resource 'DLOG' (3001, "Info") {
	{52, 49, 167, 392},
	movableDBoxProc,
	invisible,
	goAway,
	0x270C,
	457,
	"Info",
	alertPositionParentWindowScreen
};

resource 'DLOG' (3002, "Save Or Not?") {
	{52, 49, 180, 329},
	movableDBoxProc,
	invisible,
	goAway,
	0x270C,
	300,
	"Save Dialog",
	alertPositionParentWindowScreen
};

resource 'DLOG' (153, "Wave") {
	{44, 6, 308, 492},
	zoomDocProc,
	invisible,
	goAway,
	0x1A,
	153,
	"Pattern: 000",
	centerMainScreen
};

resource 'DLOG' (174, "Print Order Form") {
	{60, 85, 401, 419},
	movableDBoxProc,
	invisible,
	goAway,
	0x60,
	174,
	"Print Order Form",
	centerMainScreen
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

resource 'DLOG' (3003, "File Not Found") {
	{52, 49, 180, 329},
	movableDBoxProc,
	invisible,
	goAway,
	0x270C,
	301,
	"File Not Found",
	alertPositionParentWindowScreen
};

resource 'DLOG' (3004, "Music List Save Or Not?") {
	{52, 49, 180, 329},
	movableDBoxProc,
	invisible,
	goAway,
	0x270C,
	3004,
	"Music List Save Dialog",
	alertPositionParentWindowScreen
};

data 'DLOG' (149, "Minimize Results") {
	$"00C6 00BE 0139 0215 0005 0000 0100 0000"            /* .∆.æ.9.......... */
	$"0000 012E 104D 696E 696D 697A 6520 5265"            /* .....Minimize Re */
	$"7375 6C74 73"                                       /* sults */
};

resource 'DLOG' (134, "Classic Editor") {
	{44, 6, 488, 976},
	zoomDocProc,
	invisible,
	goAway,
	0x1B,
	134,
	"Pattern: 000",
	centerMainScreen
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

resource 'DLOG' (133, "Preferences") {
	{50, 58, 358, 410},
	movableDBoxProc,
	invisible,
	goAway,
	0x11,
	132,
	"Preferences",
	centerMainScreen
};

resource 'DLOG' (177, "Copyright") {
	{177, 247, 425, 587},
	movableDBoxProc,
	visible,
	noGoAway,
	0x0,
	177,
	"Copyright informations about this music",
	centerMainScreen
};

resource 'DLOG' (175, "Registration") {
	{87, 127, 378, 512},
	movableDBoxProc,
	invisible,
	goAway,
	0x0,
	175,
	"Registration",
	centerMainScreen
};

resource 'DLOG' (2002, "Pref: MUSIC LIST") {
	{16, 37, 336, 509},
	dBoxProc,
	visible,
	goAway,
	0x11,
	2002,
	"",
	noAutoCenter
};

resource 'DLOG' (131, "About") {
	{54, 10, 282, 395},
	dBoxProc,
	visible,
	goAway,
	0x0,
	200,
	"",
	centerMainScreen
};

resource 'DLOG' (2005, "Pref: CLASSICAL") {
	{57, 72, 600, 700},
	documentProc,
	visible,
	goAway,
	0x0,
	2006,
	"",
	noAutoCenter
};

resource 'DLOG' (165, "Microphone") {
	{63, 64, 298, 362},
	movableDBoxProc,
	invisible,
	goAway,
	0x19F8,
	761,
	"Audio InPut",
	1498
};

resource 'DLOG' (2000, "Pref: DRIVER") {
	{42, 40, 348, 426},
	dBoxProc,
	visible,
	goAway,
	0x11,
	2000,
	"",
	noAutoCenter
};

resource 'DLOG' (178, "RAW Import") {
	{262, 110, 540, 515},
	movableDBoxProc,
	invisible,
	goAway,
	0x0,
	178,
	"RAW Data Import",
	noAutoCenter
};

resource 'DLOG' (156, "ProgressWindow") {
	{142, 142, 243, 447},
	noGrowDocProc,
	invisible,
	goAway,
	0x1C,
	163,
	"Sound Data Generating",
	alertPositionMainScreen
};

resource 'DLOG' (136, "Windows State") {
	{72, 107, 177, 406},
	movableDBoxProc,
	invisible,
	goAway,
	0x0,
	136,
	"Windows State",
	525
};

resource 'DLOG' (138, "Reset Window") {
	{312, 237, 434, 572},
	movableDBoxProc,
	invisible,
	goAway,
	0x0,
	138,
	"Reset",
	noAutoCenter
};

resource 'DLOG' (139, "Editor") {
	{44, 6, 440, 1055},
	zoomDocProc,
	invisible,
	goAway,
	0x2,
	139,
	"Pattern: 000",
	centerMainScreen
};

resource 'DLOG' (160, "Export AIFF/SND") {
	{264, 202, 481, 624},
	movableDBoxProc,
	visible,
	goAway,
	0x0,
	319,
	"Sound File Quality",
	centerMainScreen
};

resource 'DLOG' (2006, "Pref: DIGITAL EDITOR") {
	{49, 35, 499, 595},
	documentProc,
	visible,
	goAway,
	0x0,
	2006,
	"",
	noAutoCenter
};

resource 'DLOG' (2009, "Pref: MIDI OMS") {
	{206, -650, 440, -298},
	documentProc,
	visible,
	goAway,
	0x0,
	2009,
	"",
	noAutoCenter
};

resource 'DLOG' (2001, "Pref: PIANO") {
	{41, 61, 353, 417},
	dBoxProc,
	visible,
	goAway,
	0x11,
	2001,
	"",
	noAutoCenter
};

resource 'DLOG' (145, "Piano") {
	{148, 186, 471, 888},
	zoomDocProc,
	invisible,
	goAway,
	0xA,
	145,
	"Piano",
	15536
};

resource 'DLOG' (166, "Partition") {
	{98, 94, 613, 698},
	zoomDocProc,
	invisible,
	goAway,
	0x15,
	816,
	"Partition",
	55920
};

resource 'DLOG' (180, "Quicktime Window") {
	{367, 58, 545, 361},
	noGrowDocProc,
	invisible,
	goAway,
	0x1D,
	180,
	"Quicktime Window",
	noAutoCenter
};

resource 'DLOG' (132, "PUB") {
	{45, 83, 472, 556},
	noGrowDocProc,
	visible,
	goAway,
	0x18,
	872,
	"How to order PlayerPRO",
	centerMainScreen
};

resource 'DLOG' (181, "Bookmarks") {
	{433, 149, 777, 561},
	movableDBoxProc,
	invisible,
	goAway,
	0xF2D,
	181,
	"Add/Remove Bookmarks",
	noAutoCenter
};

resource 'DLOG' (2004, "Pref: MISC") {
	{45, 431, 354, 935},
	documentProc,
	visible,
	goAway,
	0x0,
	2004,
	"",
	noAutoCenter
};

resource 'DLOG' (182, "Fine Speed/Pitch Settings") {
	{276, 282, 346, 498},
	movableDBoxProc,
	invisible,
	goAway,
	0x0,
	182,
	"Fine Speed/Pitch Settings",
	noAutoCenter
};

resource 'DLOG' (130, "Info") {
	{48, 14, 348, 505},
	dBoxProc,
	visible,
	goAway,
	0x0,
	421,
	"Informations",
	centerMainScreen
};

resource 'DLOG' (140, "Instruments List") {
	{117, 192, 399, 605},
	zoomDocProc,
	invisible,
	goAway,
	0x1,
	684,
	"Instruments List",
	593
};

resource 'DLOG' (137, "Samples") {
	{94, 40, 246, 576},
	zoomDocProc,
	invisible,
	goAway,
	0x9,
	202,
	"",
	noAutoCenter
};

resource 'DLOG' (148, "General Informations") {
	{132, 124, 373, 474},
	movableDBoxProc,
	invisible,
	noGoAway,
	0x2247,
	148,
	"General Information",
	noAutoCenter
};

resource 'DLOG' (183, "Equalizer") {
	{82, 161, 272, 388},
	noGrowDocProc,
	invisible,
	goAway,
	0x1E,
	183,
	"Equalizer",
	noAutoCenter
};

resource 'DLOG' (141, "Mixer") {
	{58, 15, 530, 712},
	zoomDocProc,
	invisible,
	goAway,
	0x6,
	141,
	"Mixer",
	noAutoCenter
};

resource 'DLOG' (184, "Hz Shift") {
	{79, 194, 345, 546},
	movableDBoxProc,
	invisible,
	goAway,
	0x0,
	184,
	"Hz Shift",
	noAutoCenter
};

resource 'DLOG' (146, "EditInfo Instrument") {
	{149, 262, 480, 717},
	movableDBoxProc,
	invisible,
	goAway,
	0x24FB,
	146,
	"Instrument Info",
	centerMainScreen
};

resource 'DLOG' (143, "Tools") {
	{252, 504, 412, 688},
	zoomNoGrow,
	invisible,
	goAway,
	0x8,
	143,
	"Tools",
	noAutoCenter
};

resource 'DLOG' (142, "MusicList") {
	{46, 6, 487, 723},
	zoomDocProc,
	invisible,
	goAway,
	0x7,
	142,
	"Music List",
	3818
};

resource 'DITL' (128, "The Player") {
	{	/* array DITLarray: 13 elements */
		/* [1] */
		{16, 0, 36, 20},
		UserItem {
			disabled
		},
		/* [2] */
		{1, 400, 14, 445},
		UserItem {
			disabled
		},
		/* [3] */
		{1, 444, 14, 533},
		UserItem {
			disabled
		},
		/* [4] */
		{1, 3, 14, 54},
		StaticText {
			disabled,
			"Display:"
		},
		/* [5] */
		{1, 149, 14, 169},
		Picture {
			enabled,
			900
		},
		/* [6] */
		{1, 55, 14, 147},
		StaticText {
			disabled,
			""
		},
		/* [7] */
		{1, 180, 14, 213},
		StaticText {
			disabled,
			"Size:"
		},
		/* [8] */
		{1, 243, 14, 263},
		Picture {
			enabled,
			900
		},
		/* [9] */
		{1, 214, 14, 242},
		StaticText {
			disabled,
			""
		},
		/* [10] */
		{16, 0, 2016, 2000},
		UserItem {
			enabled
		},
		/* [11] */
		{1, 276, 14, 309},
		StaticText {
			disabled,
			"Mode:"
		},
		/* [12] */
		{1, 345, 14, 365},
		Picture {
			enabled,
			900
		},
		/* [13] */
		{1, 310, 14, 344},
		StaticText {
			disabled,
			""
		}
	}
};

resource 'DITL' (1002, purgeable) {
	{	/* array DITLarray: 11 elements */
		/* [1] */
		{147, 300, 167, 380},
		Button {
			enabled,
			"Open"
		},
		/* [2] */
		{0, 699, 80, 717},
		Button {
			enabled,
			"Hidden"
		},
		/* [3] */
		{175, 300, 195, 380},
		Button {
			enabled,
			"Cancel"
		},
		/* [4] */
		{50, 273, 70, 388},
		UserItem {
			disabled
		},
		/* [5] */
		{77, 300, 97, 380},
		Button {
			enabled,
			"Eject"
		},
		/* [6] */
		{102, 300, 122, 380},
		Button {
			enabled,
			"Desktop"
		},
		/* [7] */
		{50, 10, 290, 271},
		UserItem {
			enabled
		},
		/* [8] */
		{50, 270, 290, 287},
		UserItem {
			enabled
		},
		/* [9] */
		{133, 296, 134, 384},
		UserItem {
			disabled
		},
		/* [10] */
		{0, 660, 101, 756},
		StaticText {
			disabled,
			""
		},
		/* [11] */
		{10, 10, 26, 320},
		StaticText {
			disabled,
			"Please, choose a sound file to open :"
		}
	}
};

resource 'DITL' (268) {
	{	/* array DITLarray: 8 elements */
		/* [1] */
		{103, 270, 123, 328},
		Button {
			enabled,
			"OK"
		},
		/* [2] */
		{90, 10, 106, 112},
		StaticText {
			disabled,
			"Error ID : ^1"
		},
		/* [3] */
		{10, 60, 85, 331},
		StaticText {
			disabled,
			"^0"
		},
		/* [4] */
		{10, 10, 42, 42},
		Picture {
			disabled,
			220
		},
		/* [5] */
		{46, 12, 62, 43},
		StaticText {
			disabled,
			""
		},
		/* [6] */
		{63, 12, 79, 43},
		StaticText {
			disabled,
			""
		},
		/* [7] */
		{107, 10, 123, 112},
		StaticText {
			disabled,
			"MemErr : ^2"
		},
		/* [8] */
		{124, 19, 140, 112},
		StaticText {
			disabled,
			"ResErr : ^3"
		}
	}
};

resource 'DITL' (144, "Inst. List") {
	{	/* array DITLarray: 8 elements */
		/* [1] */
		{37, 0, 2037, 200},
		UserItem {
			enabled
		},
		/* [2] */
		{25, 0, 36, 220},
		StaticText {
			disabled,
			" ID    Size    Name"
		},
		/* [3] */
		{2, 41, 22, 61},
		UserItem {
			enabled
		},
		/* [4] */
		{2, 71, 22, 91},
		UserItem {
			enabled
		},
		/* [5] */
		{2, 101, 22, 121},
		UserItem {
			enabled
		},
		/* [6] */
		{2, 131, 22, 151},
		UserItem {
			enabled
		},
		/* [7] */
		{2, 11, 22, 31},
		UserItem {
			enabled
		},
		/* [8] */
		{2, 161, 22, 181},
		UserItem {
			enabled
		}
	}
};

resource 'DITL' (457) {
	{	/* array DITLarray: 4 elements */
		/* [1] */
		{81, 276, 101, 334},
		Button {
			enabled,
			"OK"
		},
		/* [2] */
		{81, 204, 101, 262},
		Button {
			enabled,
			"Cancel"
		},
		/* [3] */
		{10, 60, 76, 332},
		StaticText {
			disabled,
			"^0"
		},
		/* [4] */
		{10, 10, 42, 42},
		Picture {
			disabled,
			220
		}
	}
};

resource 'DITL' (147) {
	{	/* array DITLarray: 15 elements */
		/* [1] */
		{29, 0, 2029, 2000},
		UserItem {
			enabled
		},
		/* [2] */
		{2, 80, 14, 114},
		StaticText {
			disabled,
			"Zoom:"
		},
		/* [3] */
		{14, 80, 27, 113},
		StaticText {
			disabled,
			"Size:"
		},
		/* [4] */
		{2, 198, 16, 238},
		StaticText {
			disabled,
			"Track:"
		},
		/* [5] */
		{2, 285, 16, 372},
		StaticText {
			disabled,
			""
		},
		/* [6] */
		{15, 240, 27, 374},
		StaticText {
			disabled,
			""
		},
		/* [7] */
		{1, 376, 14, 396},
		Picture {
			enabled,
			200
		},
		/* [8] */
		{14, 376, 27, 396},
		Picture {
			enabled,
			200
		},
		/* [9] */
		{2, 113, 14, 195},
		StaticText {
			disabled,
			""
		},
		/* [10] */
		{14, 113, 27, 163},
		StaticText {
			disabled,
			""
		},
		/* [11] */
		{15, 168, 27, 238},
		StaticText {
			disabled,
			"Instrument:"
		},
		/* [12] */
		{4, 5, 24, 25},
		UserItem {
			enabled
		},
		/* [13] */
		{2, 243, 12, 281},
		UserItem {
			disabled
		},
		/* [14] */
		{4, 30, 24, 50},
		UserItem {
			enabled
		},
		/* [15] */
		{4, 55, 24, 75},
		UserItem {
			enabled
		}
	}
};

resource 'DITL' (300, purgeable) {
	{	/* array DITLarray: 5 elements */
		/* [1] */
		{62, 9, 82, 79},
		Button {
			enabled,
			"Yes"
		},
		/* [2] */
		{92, 9, 112, 79},
		Button {
			enabled,
			"No"
		},
		/* [3] */
		{92, 200, 112, 270},
		Button {
			enabled,
			"Cancel"
		},
		/* [4] */
		{6, 60, 54, 270},
		StaticText {
			disabled,
			"Save “^0” before closing?"
		},
		/* [5] */
		{10, 10, 42, 42},
		Picture {
			disabled,
			220
		}
	}
};

resource 'DITL' (129) {
	{	/* array DITLarray: 10 elements */
		/* [1] */
		{135, 252, 155, 332},
		Button {
			enabled,
			"Open"
		},
		/* [2] */
		{104, 252, 124, 332},
		Button {
			enabled,
			"Cancel"
		},
		/* [3] */
		{0, 0, 0, 0},
		UserItem {
			disabled
		},
		/* [4] */
		{8, 235, 24, 337},
		UserItem {
			enabled
		},
		/* [5] */
		{32, 252, 52, 332},
		Button {
			enabled,
			"Eject"
		},
		/* [6] */
		{60, 252, 80, 332},
		Button {
			enabled,
			"Desktop"
		},
		/* [7] */
		{29, 12, 255, 230},
		UserItem {
			enabled
		},
		/* [8] */
		{6, 12, 25, 230},
		UserItem {
			enabled
		},
		/* [9] */
		{91, 251, 92, 333},
		Picture {
			disabled,
			11
		},
		/* [10] */
		{169, 250, 255, 336},
		Picture {
			disabled,
			133
		}
	}
};

resource 'DITL' (131, purgeable) {
	{	/* array DITLarray: 13 elements */
		/* [1] */
		{161, 252, 181, 332},
		Button {
			enabled,
			"Save"
		},
		/* [2] */
		{130, 252, 150, 332},
		Button {
			enabled,
			"Cancel"
		},
		/* [3] */
		{0, 0, 0, 0},
		UserItem {
			disabled
		},
		/* [4] */
		{8, 235, 24, 337},
		UserItem {
			enabled
		},
		/* [5] */
		{32, 252, 52, 332},
		Button {
			enabled,
			"Eject"
		},
		/* [6] */
		{60, 252, 80, 332},
		Button {
			enabled,
			"Desktop"
		},
		/* [7] */
		{29, 12, 234, 230},
		UserItem {
			enabled
		},
		/* [8] */
		{7, 12, 26, 230},
		UserItem {
			enabled
		},
		/* [9] */
		{119, 250, 120, 334},
		Picture {
			disabled,
			11
		},
		/* [10] */
		{262, 16, 278, 228},
		EditText {
			enabled,
			""
		},
		/* [11] */
		{241, 16, 257, 182},
		StaticText {
			disabled,
			"Save document as:"
		},
		/* [12] */
		{88, 252, 108, 332},
		UserItem {
			disabled
		},
		/* [13] */
		{195, 250, 281, 336},
		Picture {
			disabled,
			133
		}
	}
};

resource 'DITL' (150) {
	{	/* array DITLarray: 2 elements */
		/* [1] */
		{9, 72, 25, 200},
		EditText {
			disabled,
			"Instrument "
		},
		/* [2] */
		{10, 10, 26, 56},
		StaticText {
			disabled,
			"Name:"
		}
	}
};

resource 'DITL' (193) {
	{	/* array DITLarray: 3 elements */
		/* [1] */
		{16, 99, 133, 368},
		UserItem {
			disabled
		},
		/* [2] */
		{8, 7, 320, 23},
		Control {
			enabled,
			153
		},
		/* [3] */
		{119, 99, 300, 5099},
		UserItem {
			enabled
		}
	}
};

resource 'DITL' (133) {
	{	/* array DITLarray: 20 elements */
		/* [1] */
		{135, 252, 155, 332},
		Button {
			enabled,
			"Open"
		},
		/* [2] */
		{104, 252, 124, 332},
		Button {
			enabled,
			"Cancel"
		},
		/* [3] */
		{0, 0, 0, 0},
		UserItem {
			disabled
		},
		/* [4] */
		{8, 235, 24, 337},
		UserItem {
			enabled
		},
		/* [5] */
		{32, 252, 52, 332},
		Button {
			enabled,
			"Eject"
		},
		/* [6] */
		{60, 252, 80, 332},
		Button {
			enabled,
			"Desktop"
		},
		/* [7] */
		{29, 12, 255, 230},
		UserItem {
			enabled
		},
		/* [8] */
		{6, 12, 25, 230},
		UserItem {
			enabled
		},
		/* [9] */
		{91, 251, 92, 333},
		Picture {
			disabled,
			11
		},
		/* [10] */
		{175, 252, 195, 332},
		Button {
			enabled,
			"Add"
		},
		/* [11] */
		{255, 274, 271, 311},
		StaticText {
			enabled,
			""
		},
		/* [12] */
		{255, 12, 272, 56},
		StaticText {
			enabled,
			"Show: "
		},
		/* [13] */
		{277, 107, 294, 235},
		StaticText {
			disabled,
			"Available formats: "
		},
		/* [14] */
		{294, 12, 342, 336},
		StaticText {
			disabled,
			""
		},
		/* [15] */
		{274, -1, 275, 351},
		Picture {
			disabled,
			11
		},
		/* [16] */
		{203, 252, 223, 332},
		Button {
			enabled,
			"Add All"
		},
		/* [17] */
		{231, 252, 251, 332},
		Button {
			enabled,
			"Preview"
		},
		/* [18] */
		{164, 251, 165, 333},
		Picture {
			disabled,
			11
		},
		/* [19] */
		{255, 83, 271, 228},
		StaticText {
			enabled,
			""
		},
		/* [20] */
		{257, 58, 270, 78},
		Picture {
			enabled,
			901
		}
	}
};

resource 'DITL' (152) {
	{	/* array DITLarray: 47 elements */
		/* [1] */
		{134, 236, 154, 315},
		Button {
			enabled,
			"Find"
		},
		/* [2] */
		{134, 342, 154, 421},
		Button {
			enabled,
			"Done"
		},
		/* [3] */
		{17, 3, 35, 98},
		CheckBox {
			enabled,
			"Instrument"
		},
		/* [4] */
		{37, 3, 55, 92},
		CheckBox {
			enabled,
			"Note"
		},
		/* [5] */
		{57, 3, 75, 82},
		CheckBox {
			enabled,
			"Effect"
		},
		/* [6] */
		{76, 3, 94, 94},
		CheckBox {
			enabled,
			"Argument"
		},
		/* [7] */
		{0, 52, 16, 89},
		StaticText {
			disabled,
			"Find:"
		},
		/* [8] */
		{0, 189, 16, 257},
		StaticText {
			disabled,
			"Replace:"
		},
		/* [9] */
		{0, 338, 16, 398},
		StaticText {
			disabled,
			"Where:"
		},
		/* [10] */
		{18, 304, 36, 428},
		RadioButton {
			enabled,
			"Current pattern"
		},
		/* [11] */
		{38, 304, 56, 448},
		RadioButton {
			enabled,
			"Selection (Digital)"
		},
		/* [12] */
		{58, 304, 76, 410},
		RadioButton {
			enabled,
			"Everywhere"
		},
		/* [13] */
		{50, 530, 68, 681},
		RadioButton {
			enabled,
			"Radio Button"
		},
		/* [14] */
		{20, 126, 33, 146},
		Picture {
			enabled,
			900
		},
		/* [15] */
		{19, 97, 35, 125},
		StaticText {
			disabled,
			"888"
		},
		/* [16] */
		{40, 126, 53, 146},
		Picture {
			enabled,
			900
		},
		/* [17] */
		{39, 97, 55, 125},
		StaticText {
			disabled,
			"888"
		},
		/* [18] */
		{78, 100, 93, 118},
		EditText {
			enabled,
			"00"
		},
		/* [19] */
		{60, 126, 73, 146},
		Picture {
			enabled,
			900
		},
		/* [20] */
		{59, 100, 74, 122},
		StaticText {
			disabled,
			"99"
		},
		/* [21] */
		{16, 153, 34, 248},
		CheckBox {
			enabled,
			"Instrument"
		},
		/* [22] */
		{36, 153, 54, 242},
		CheckBox {
			enabled,
			"Note"
		},
		/* [23] */
		{56, 153, 74, 228},
		CheckBox {
			enabled,
			"Effect"
		},
		/* [24] */
		{76, 153, 94, 244},
		CheckBox {
			enabled,
			"Argument"
		},
		/* [25] */
		{20, 276, 33, 296},
		Picture {
			enabled,
			900
		},
		/* [26] */
		{18, 247, 34, 275},
		StaticText {
			disabled,
			"888"
		},
		/* [27] */
		{40, 276, 53, 296},
		Picture {
			enabled,
			900
		},
		/* [28] */
		{38, 247, 54, 275},
		StaticText {
			disabled,
			"999"
		},
		/* [29] */
		{78, 250, 93, 268},
		EditText {
			enabled,
			"99"
		},
		/* [30] */
		{60, 276, 73, 296},
		Picture {
			enabled,
			900
		},
		/* [31] */
		{57, 250, 73, 272},
		StaticText {
			disabled,
			"99"
		},
		/* [32] */
		{-10, 149, 120, 150},
		Picture {
			disabled,
			11
		},
		/* [33] */
		{-10, 299, 120, 300},
		Picture {
			disabled,
			11
		},
		/* [34] */
		{215, 71, 225, 81},
		UserItem {
			disabled
		},
		/* [35] */
		{121, 0, 122, 800},
		Picture {
			disabled,
			11
		},
		/* [36] */
		{185, 266, 205, 371},
		Button {
			enabled,
			"Replace & Find"
		},
		/* [37] */
		{134, 127, 154, 206},
		Button {
			enabled,
			"Replace"
		},
		/* [38] */
		{134, 17, 154, 96},
		Button {
			enabled,
			"Replace All"
		},
		/* [39] */
		{98, 3, 116, 94},
		CheckBox {
			enabled,
			"Volume"
		},
		/* [40] */
		{100, 100, 115, 118},
		EditText {
			enabled,
			"00"
		},
		/* [41] */
		{98, 153, 116, 244},
		CheckBox {
			enabled,
			"Volume"
		},
		/* [42] */
		{100, 250, 115, 268},
		EditText {
			enabled,
			"00"
		},
		/* [43] */
		{100, 304, 116, 385},
		RadioButton {
			enabled,
			"On track:"
		},
		/* [44] */
		{101, 406, 114, 426},
		Picture {
			enabled,
			900
		},
		/* [45] */
		{100, 386, 116, 405},
		StaticText {
			disabled,
			"1"
		},
		/* [46] */
		{82, 304, 98, 385},
		RadioButton {
			enabled,
			"All tracks"
		},
		/* [47] */
		{78, 300, 79, 1100},
		Picture {
			disabled,
			11
		}
	}
};

resource 'DITL' (2003) {
	{	/* array DITLarray: 4 elements */
		/* [1] */
		{57, 4, 140, 351},
		UserItem {
			enabled
		},
		/* [2] */
		{38, 3, 54, 158},
		StaticText {
			disabled,
			"Tracks color:"
		},
		/* [3] */
		{152, 3, 168, 198},
		StaticText {
			disabled,
			"Click to change color"
		},
		/* [4] */
		{30, 180, 57, 332},
		UserItem {
			disabled
		}
	}
};

resource 'DITL' (155) {
	{	/* array DITLarray: 1 elements */
		/* [1] */
		{-1, -1, 125, 485},
		UserItem {
			enabled
		}
	}
};

resource 'DITL' (140) {
	{	/* array DITLarray: 10 elements */
		/* [1] */
		{135, 252, 155, 332},
		Button {
			enabled,
			"Open"
		},
		/* [2] */
		{104, 252, 124, 332},
		Button {
			enabled,
			"Cancel"
		},
		/* [3] */
		{0, 0, 0, 0},
		UserItem {
			disabled
		},
		/* [4] */
		{8, 235, 24, 337},
		UserItem {
			enabled
		},
		/* [5] */
		{32, 252, 52, 332},
		Button {
			enabled,
			"Eject"
		},
		/* [6] */
		{60, 252, 80, 332},
		Button {
			enabled,
			"Desktop"
		},
		/* [7] */
		{29, 12, 159, 230},
		UserItem {
			enabled
		},
		/* [8] */
		{6, 12, 25, 230},
		UserItem {
			enabled
		},
		/* [9] */
		{91, 251, 92, 333},
		Picture {
			disabled,
			11
		},
		/* [10] */
		{164, 12, 183, 198},
		Control {
			enabled,
			141
		}
	}
};

resource 'DITL' (151, purgeable) {
	{	/* array DITLarray: 13 elements */
		/* [1] */
		{161, 252, 181, 332},
		Button {
			enabled,
			"Save"
		},
		/* [2] */
		{130, 252, 150, 332},
		Button {
			enabled,
			"Cancel"
		},
		/* [3] */
		{0, 0, 0, 0},
		UserItem {
			disabled
		},
		/* [4] */
		{8, 235, 24, 337},
		UserItem {
			enabled
		},
		/* [5] */
		{32, 252, 52, 332},
		Button {
			enabled,
			"Eject"
		},
		/* [6] */
		{60, 252, 80, 332},
		Button {
			enabled,
			"Desktop"
		},
		/* [7] */
		{29, 12, 182, 230},
		UserItem {
			enabled
		},
		/* [8] */
		{7, 12, 26, 230},
		UserItem {
			enabled
		},
		/* [9] */
		{119, 250, 120, 334},
		Picture {
			disabled,
			11
		},
		/* [10] */
		{209, 14, 225, 226},
		EditText {
			enabled,
			""
		},
		/* [11] */
		{188, 14, 204, 226},
		StaticText {
			disabled,
			"Filename:"
		},
		/* [12] */
		{88, 252, 108, 332},
		UserItem {
			disabled
		},
		/* [13] */
		{235, 11, 254, 247},
		Control {
			enabled,
			141
		}
	}
};

resource 'DITL' (156, "Music list") {
	{	/* array DITLarray: 17 elements */
		/* [1] */
		{0, 0, 196, 196},
		UserItem {
			enabled
		},
		/* [2] */
		{5, 205, 18, 280},
		StaticText {
			disabled,
			"Rows:"
		},
		/* [3] */
		{20, 205, 33, 311},
		StaticText {
			disabled,
			"Columns:"
		},
		/* [4] */
		{80, 205, 93, 335},
		StaticText {
			disabled,
			"Smallest Pixel Value:"
		},
		/* [5] */
		{35, 205, 48, 280},
		StaticText {
			disabled,
			"Depth:"
		},
		/* [6] */
		{95, 205, 108, 336},
		StaticText {
			disabled,
			"Window Center:"
		},
		/* [7] */
		{65, 205, 78, 334},
		StaticText {
			disabled,
			"Largest Pixel Value:"
		},
		/* [8] */
		{50, 205, 63, 327},
		StaticText {
			disabled,
			"Size (in bytes):"
		},
		/* [9] */
		{5, 337, 18, 417},
		StaticText {
			disabled,
			""
		},
		/* [10] */
		{20, 337, 33, 417},
		StaticText {
			enabled,
			""
		},
		/* [11] */
		{80, 337, 93, 417},
		StaticText {
			disabled,
			""
		},
		/* [12] */
		{50, 337, 63, 417},
		StaticText {
			disabled,
			""
		},
		/* [13] */
		{95, 337, 108, 417},
		StaticText {
			disabled,
			""
		},
		/* [14] */
		{65, 337, 78, 417},
		StaticText {
			disabled,
			""
		},
		/* [15] */
		{35, 337, 48, 417},
		StaticText {
			disabled,
			""
		},
		/* [16] */
		{110, 205, 123, 334},
		StaticText {
			disabled,
			"Window Width:"
		},
		/* [17] */
		{110, 337, 122, 416},
		StaticText {
			disabled,
			""
		}
	}
};

resource 'DITL' (157) {
	{	/* array DITLarray: 2 elements */
		/* [1] */
		{-98, -146, 43, 20},
		Control {
			disabled,
			128
		},
		/* [2] */
		{-62, -79, 23, 18},
		Control {
			disabled,
			129
		}
	}
};

resource 'DITL' (158) {
	{	/* array DITLarray: 10 elements */
		/* [1] */
		{7, 67, 15, 117},
		UserItem {
			enabled
		},
		/* [2] */
		{34, 256, 47, 294},
		UserItem {
			disabled
		},
		/* [3] */
		{56, 236, 68, 318},
		UserItem {
			disabled
		},
		/* [4] */
		{22, 0, 54, 32},
		UserItem {
			disabled
		},
		/* [5] */
		{0, 0, 32, 32},
		Control {
			enabled,
			137
		},
		/* [6] */
		{0, 35, 32, 67},
		Control {
			enabled,
			132
		},
		/* [7] */
		{2, 233, 30, 266},
		UserItem {
			disabled
		},
		/* [8] */
		{2, 270, 30, 303},
		UserItem {
			disabled
		},
		/* [9] */
		{0, 70, 32, 102},
		UserItem {
			enabled
		},
		/* [10] */
		{4, 191, 26, 294},
		UserItem {
			disabled
		}
	}
};

resource 'DITL' (159) {
	{	/* array DITLarray: 4 elements */
		/* [1] */
		{80, 270, 100, 328},
		Button {
			enabled,
			"OK"
		},
		/* [2] */
		{90, 10, 106, 163},
		StaticText {
			disabled,
			"Error ID: ^1"
		},
		/* [3] */
		{10, 60, 76, 332},
		StaticText {
			disabled,
			"^0"
		},
		/* [4] */
		{10, 10, 42, 42},
		Picture {
			disabled,
			220
		}
	}
};

resource 'DITL' (160) {
	{	/* array DITLarray: 4 elements */
		/* [1] */
		{81, 276, 101, 334},
		Button {
			enabled,
			"OK"
		},
		/* [2] */
		{81, 204, 101, 262},
		Button {
			enabled,
			"Cancel"
		},
		/* [3] */
		{10, 60, 76, 332},
		StaticText {
			disabled,
			"^0"
		},
		/* [4] */
		{10, 10, 42, 42},
		Picture {
			disabled,
			220
		}
	}
};

resource 'DITL' (162) {
	{	/* array DITLarray: 2 elements */
		/* [1] */
		{24, -1, 2024, 199},
		UserItem {
			disabled
		},
		/* [2] */
		{3, 5, 19, 190},
		Control {
			enabled,
			130
		}
	}
};

resource 'DITL' (167, "OSIRIS StdFile Open", purgeable) {
	{	/* array DITLarray: 10 elements */
		/* [1] */
		{123, 283, 141, 363},
		Button {
			enabled,
			"OK"
		},
		/* [2] */
		{148, 283, 166, 363},
		Button {
			enabled,
			"Cancel"
		},
		/* [3] */
		{0, 0, 0, 0},
		UserItem {
			disabled
		},
		/* [4] */
		{16, 272, 32, 374},
		UserItem {
			disabled
		},
		/* [5] */
		{53, 283, 71, 363},
		Button {
			enabled,
			"Eject"
		},
		/* [6] */
		{78, 283, 96, 363},
		Button {
			enabled,
			"Desktop"
		},
		/* [7] */
		{39, 12, 296, 259},
		UserItem {
			enabled
		},
		/* [8] */
		{14, 12, 33, 259},
		UserItem {
			enabled
		},
		/* [9] */
		{109, 279, 110, 367},
		UserItem {
			disabled
		},
		/* [10] */
		{1044, 20, 1145, 116},
		StaticText {
			disabled,
			""
		}
	}
};

resource 'DITL' (168) {
	{	/* array DITLarray: 11 elements */
		/* [1] */
		{161, 279, 181, 359},
		Button {
			enabled,
			"Save"
		},
		/* [2] */
		{130, 279, 150, 359},
		Button {
			enabled,
			"Cancel"
		},
		/* [3] */
		{8, 268, 24, 370},
		UserItem {
			enabled
		},
		/* [4] */
		{32, 279, 52, 359},
		Button {
			enabled,
			"Eject"
		},
		/* [5] */
		{60, 279, 80, 359},
		Button {
			enabled,
			"Desktop"
		},
		/* [6] */
		{29, 12, 286, 259},
		UserItem {
			enabled
		},
		/* [7] */
		{7, 12, 26, 259},
		UserItem {
			enabled
		},
		/* [8] */
		{119, 277, 120, 361},
		Picture {
			disabled,
			11
		},
		/* [9] */
		{314, 27, 330, 239},
		EditText {
			enabled,
			""
		},
		/* [10] */
		{293, 27, 309, 239},
		StaticText {
			disabled,
			"Filename:"
		},
		/* [11] */
		{88, 279, 108, 359},
		UserItem {
			disabled
		}
	}
};

resource 'DITL' (169) {
	{	/* array DITLarray: 13 elements */
		/* [1] */
		{0, 82, 17, 171},
		UserItem {
			disabled
		},
		/* [2] */
		{203, 26, 222, 226},
		UserItem {
			disabled
		},
		/* [3] */
		{221, 1, 240, 107},
		UserItem {
			disabled
		},
		/* [4] */
		{221, 144, 240, 250},
		UserItem {
			disabled
		},
		/* [5] */
		{0, 0, 100, 250},
		Picture {
			disabled,
			130
		},
		/* [6] */
		{69, 0, 197, 256},
		Picture {
			disabled,
			131
		},
		/* [7] */
		{72, 0, 87, 243},
		UserItem {
			disabled
		},
		/* [8] */
		{90, 0, 105, 243},
		UserItem {
			disabled
		},
		/* [9] */
		{108, 0, 123, 243},
		UserItem {
			disabled
		},
		/* [10] */
		{126, 0, 141, 243},
		UserItem {
			disabled
		},
		/* [11] */
		{144, 0, 159, 243},
		UserItem {
			disabled
		},
		/* [12] */
		{162, 0, 177, 243},
		UserItem {
			disabled
		},
		/* [13] */
		{180, 0, 195, 243},
		UserItem {
			disabled
		}
	}
};

resource 'DITL' (170) {
	{	/* array DITLarray: 4 elements */
		/* [1] */
		{3, 3, 67, 209},
		UserItem {
			disabled
		},
		/* [2] */
		{70, 3, 134, 209},
		UserItem {
			disabled
		},
		/* [3] */
		{3, 2, 67, 15},
		UserItem {
			disabled
		},
		/* [4] */
		{70, 2, 134, 15},
		UserItem {
			disabled
		}
	}
};

resource 'DITL' (623) {
	{	/* array DITLarray: 9 elements */
		/* [1] */
		{8, 252, 28, 310},
		Button {
			enabled,
			"Start"
		},
		/* [2] */
		{40, 252, 60, 310},
		Button {
			enabled,
			"Bye"
		},
		/* [3] */
		{8, 12, 24, 126},
		StaticText {
			disabled,
			"Result (in ticks):"
		},
		/* [4] */
		{8, 132, 24, 207},
		StaticText {
			disabled,
			""
		},
		/* [5] */
		{32, 12, 48, 126},
		StaticText {
			disabled,
			"Processor Time:"
		},
		/* [6] */
		{32, 132, 48, 207},
		StaticText {
			disabled,
			""
		},
		/* [7] */
		{70, 10, 90, 88},
		Button {
			enabled,
			"Debug ON"
		},
		/* [8] */
		{70, 100, 90, 178},
		Button {
			enabled,
			"Debug OFF"
		},
		/* [9] */
		{69, 188, 89, 298},
		Button {
			enabled,
			"Memory Purge"
		}
	}
};

resource 'DITL' (364) {
	{	/* array DITLarray: 19 elements */
		/* [1] */
		{2, 2, 18, 301},
		UserItem {
			disabled
		},
		/* [2] */
		{19, 2, 32, 74},
		StaticText {
			disabled,
			"Used memory:"
		},
		/* [3] */
		{19, 76, 32, 124},
		StaticText {
			disabled,
			"1300 Kb"
		},
		/* [4] */
		{46, 208, 59, 221},
		UserItem {
			disabled
		},
		/* [5] */
		{19, 169, 32, 244},
		StaticText {
			disabled,
			"Free memory:"
		},
		/* [6] */
		{19, 245, 32, 301},
		StaticText {
			disabled,
			""
		},
		/* [7] */
		{52, 215, 65, 228},
		UserItem {
			disabled
		},
		/* [8] */
		{33, 31, 46, 101},
		StaticText {
			disabled,
			"Sound Driver:"
		},
		/* [9] */
		{33, 103, 46, 151},
		StaticText {
			disabled,
			""
		},
		/* [10] */
		{33, 2, 46, 30},
		StaticText {
			disabled,
			"20%"
		},
		/* [11] */
		{47, 36, 60, 101},
		StaticText {
			disabled,
			"Instruments:"
		},
		/* [12] */
		{47, 103, 60, 151},
		StaticText {
			disabled,
			""
		},
		/* [13] */
		{47, 2, 60, 30},
		StaticText {
			disabled,
			"20%"
		},
		/* [14] */
		{61, 50, 75, 101},
		StaticText {
			disabled,
			"Partition:"
		},
		/* [15] */
		{61, 103, 74, 151},
		StaticText {
			disabled,
			""
		},
		/* [16] */
		{61, 2, 74, 31},
		StaticText {
			disabled,
			"20%"
		},
		/* [17] */
		{75, 41, 88, 101},
		StaticText {
			disabled,
			"PlayerPRO:"
		},
		/* [18] */
		{75, 103, 88, 151},
		StaticText {
			disabled,
			""
		},
		/* [19] */
		{75, 2, 88, 29},
		StaticText {
			disabled,
			"20%"
		}
	}
};

resource 'DITL' (217) {
	{	/* array DITLarray: 22 elements */
		/* [1] */
		{35, 184, 55, 242},
		Button {
			enabled,
			"OK"
		},
		/* [2] */
		{65, 184, 85, 242},
		Button {
			enabled,
			"Cancel"
		},
		/* [3] */
		{6, 76, 19, 241},
		EditText {
			enabled,
			""
		},
		/* [4] */
		{26, 76, 39, 123},
		EditText {
			enabled,
			""
		},
		/* [5] */
		{6, 39, 19, 73},
		StaticText {
			disabled,
			"Name:"
		},
		/* [6] */
		{26, 39, 40, 73},
		StaticText {
			disabled,
			"Rows:"
		},
		/* [7] */
		{111, 59, 124, 100},
		StaticText {
			disabled,
			"Tracks:"
		},
		/* [8] */
		{111, 100, 124, 124},
		StaticText {
			disabled,
			""
		},
		/* [9] */
		{111, 132, 124, 200},
		StaticText {
			disabled,
			"Size in bytes:"
		},
		/* [10] */
		{111, 200, 124, 249},
		StaticText {
			disabled,
			""
		},
		/* [11] */
		{106, -19, 107, 381},
		Picture {
			disabled,
			11
		},
		/* [12] */
		{111, 6, 124, 31},
		StaticText {
			disabled,
			"ID:"
		},
		/* [13] */
		{111, 22, 124, 49},
		StaticText {
			disabled,
			""
		},
		/* [14] */
		{46, 6, 59, 73},
		StaticText {
			disabled,
			"Compression:"
		},
		/* [15] */
		{46, 114, 59, 134},
		Picture {
			enabled,
			200
		},
		/* [16] */
		{46, 76, 59, 111},
		StaticText {
			disabled,
			""
		},
		/* [17] */
		{66, 12, 79, 64},
		CheckBox {
			enabled,
			"Speed:"
		},
		/* [18] */
		{86, 12, 99, 73},
		CheckBox {
			enabled,
			"Tempo:"
		},
		/* [19] */
		{66, 76, 79, 123},
		EditText {
			enabled,
			""
		},
		/* [20] */
		{86, 76, 99, 123},
		EditText {
			enabled,
			""
		},
		/* [21] */
		{66, 129, 79, 171},
		StaticText {
			disabled,
			"(0...31)"
		},
		/* [22] */
		{86, 129, 99, 181},
		StaticText {
			disabled,
			"(32...255)"
		}
	}
};

resource 'DITL' (317) {
	{	/* array DITLarray: 25 elements */
		/* [1] */
		{10, 280, 30, 338},
		Button {
			enabled,
			"OK"
		},
		/* [2] */
		{40, 280, 60, 338},
		Button {
			enabled,
			"Cancel"
		},
		/* [3] */
		{10, 20, 24, 73},
		StaticText {
			disabled,
			"File Name:"
		},
		/* [4] */
		{10, 82, 23, 258},
		EditText {
			enabled,
			""
		},
		/* [5] */
		{34, 41, 48, 73},
		StaticText {
			disabled,
			"Type:"
		},
		/* [6] */
		{58, 28, 72, 73},
		StaticText {
			disabled,
			"Creator:"
		},
		/* [7] */
		{34, 82, 47, 115},
		EditText {
			enabled,
			""
		},
		/* [8] */
		{34, 122, 47, 142},
		Picture {
			enabled,
			200
		},
		/* [9] */
		{58, 82, 71, 115},
		EditText {
			enabled,
			""
		},
		/* [10] */
		{58, 122, 71, 142},
		Picture {
			enabled,
			200
		},
		/* [11] */
		{29, 188, 61, 220},
		UserItem {
			disabled
		},
		/* [12] */
		{61, 159, 74, 249},
		UserItem {
			disabled
		},
		/* [13] */
		{79, -12, 80, 588},
		Picture {
			disabled,
			11
		},
		/* [14] */
		{84, 1, 97, 73},
		StaticText {
			disabled,
			"Internal Name:"
		},
		/* [15] */
		{114, 192, 127, 241},
		StaticText {
			disabled,
			"Patterns:"
		},
		/* [16] */
		{114, 19, 127, 73},
		StaticText {
			disabled,
			"Plug Type:"
		},
		/* [17] */
		{99, 177, 112, 241},
		StaticText {
			disabled,
			"Instruments:"
		},
		/* [18] */
		{99, 44, 112, 73},
		StaticText {
			disabled,
			"Size:"
		},
		/* [19] */
		{84, 79, 97, 290},
		StaticText {
			enabled,
			"-"
		},
		/* [20] */
		{114, 247, 127, 342},
		StaticText {
			disabled,
			"-"
		},
		/* [21] */
		{114, 79, 127, 174},
		StaticText {
			disabled,
			"-"
		},
		/* [22] */
		{99, 247, 112, 342},
		StaticText {
			disabled,
			"-"
		},
		/* [23] */
		{99, 79, 112, 174},
		StaticText {
			disabled,
			"-"
		},
		/* [24] */
		{129, 35, 142, 73},
		StaticText {
			disabled,
			"Where:"
		},
		/* [25] */
		{129, 79, 274, 228},
		StaticText {
			disabled,
			"-"
		}
	}
};

resource 'DITL' (172) {
	{	/* array DITLarray: 26 elements */
		/* [1] */
		{110, 370, 130, 428},
		Button {
			enabled,
			"Move"
		},
		/* [2] */
		{110, 450, 130, 508},
		Button {
			enabled,
			"Cancel"
		},
		/* [3] */
		{20, 10, 38, 116},
		CheckBox {
			enabled,
			"Instrument"
		},
		/* [4] */
		{40, 10, 58, 99},
		CheckBox {
			enabled,
			"Note"
		},
		/* [5] */
		{60, 10, 78, 116},
		CheckBox {
			enabled,
			"Effect"
		},
		/* [6] */
		{80, 10, 98, 101},
		CheckBox {
			enabled,
			"Argument"
		},
		/* [7] */
		{0, 70, 16, 107},
		StaticText {
			disabled,
			"WHAT"
		},
		/* [8] */
		{0, 260, 16, 297},
		StaticText {
			disabled,
			"BY"
		},
		/* [9] */
		{0, 410, 16, 466},
		StaticText {
			disabled,
			"WHERE"
		},
		/* [10] */
		{20, 370, 38, 494},
		RadioButton {
			enabled,
			"Current pattern"
		},
		/* [11] */
		{40, 370, 58, 514},
		RadioButton {
			enabled,
			"Selection (Digital)"
		},
		/* [12] */
		{80, 370, 98, 476},
		RadioButton {
			enabled,
			"Everywhere"
		},
		/* [13] */
		{60, 370, 78, 521},
		RadioButton {
			enabled,
			"Selection (Classical)"
		},
		/* [14] */
		{20, 150, 33, 170},
		Picture {
			enabled,
			900
		},
		/* [15] */
		{20, 120, 36, 142},
		StaticText {
			disabled,
			"00"
		},
		/* [16] */
		{40, 150, 53, 170},
		Picture {
			enabled,
			900
		},
		/* [17] */
		{40, 120, 56, 142},
		StaticText {
			disabled,
			"00"
		},
		/* [18] */
		{80, 120, 96, 138},
		EditText {
			enabled,
			"00"
		},
		/* [19] */
		{60, 150, 73, 170},
		Picture {
			enabled,
			900
		},
		/* [20] */
		{60, 120, 76, 142},
		StaticText {
			disabled,
			"00"
		},
		/* [21] */
		{30, 210, 48, 341},
		StaticText {
			enabled,
			"No of semi-tones:"
		},
		/* [22] */
		{60, 260, 76, 278},
		EditText {
			enabled,
			"00"
		},
		/* [23] */
		{0, 180, 100, 181},
		Picture {
			disabled,
			11
		},
		/* [24] */
		{0, 360, 100, 361},
		Picture {
			disabled,
			11
		},
		/* [25] */
		{0, 0, 1, 800},
		Picture {
			disabled,
			11
		},
		/* [26] */
		{100, 0, 101, 800},
		Picture {
			disabled,
			11
		}
	}
};

resource 'DITL' (2005) {
	{	/* array DITLarray: 13 elements */
		/* [1] */
		{160, 10, 176, 81},
		UserItem {
			enabled
		},
		/* [2] */
		{160, 100, 176, 170},
		UserItem {
			enabled
		},
		/* [3] */
		{180, 110, 196, 177},
		UserItem {
			enabled
		},
		/* [4] */
		{130, 10, 148, 257},
		CheckBox {
			enabled,
			"Notes projection"
		},
		/* [5] */
		{40, 10, 58, 91},
		CheckBox {
			enabled,
			"Markers:"
		},
		/* [6] */
		{70, 30, 86, 83},
		StaticText {
			disabled,
			"Offset:"
		},
		/* [7] */
		{70, 90, 86, 165},
		EditText {
			enabled,
			""
		},
		/* [8] */
		{70, 200, 86, 252},
		StaticText {
			disabled,
			"Loop:"
		},
		/* [9] */
		{70, 260, 86, 335},
		EditText {
			enabled,
			""
		},
		/* [10] */
		{170, 200, 190, 220},
		UserItem {
			disabled
		},
		/* [11] */
		{110, 10, 128, 155},
		CheckBox {
			enabled,
			"Octaves Markers"
		},
		/* [12] */
		{150, 0, 158, 40},
		UserItem {
			disabled
		},
		/* [13] */
		{100, 0, 101, 352},
		Picture {
			disabled,
			902
		}
	}
};

resource 'DITL' (161) {
	{	/* array DITLarray: 16 elements */
		/* [1] */
		{135, 252, 155, 332},
		Button {
			enabled,
			"Open"
		},
		/* [2] */
		{104, 252, 124, 332},
		Button {
			enabled,
			"Cancel"
		},
		/* [3] */
		{0, 0, 0, 0},
		UserItem {
			disabled
		},
		/* [4] */
		{8, 235, 24, 337},
		UserItem {
			enabled
		},
		/* [5] */
		{32, 252, 52, 332},
		Button {
			enabled,
			"Eject"
		},
		/* [6] */
		{60, 252, 80, 332},
		Button {
			enabled,
			"Desktop"
		},
		/* [7] */
		{29, 12, 255, 230},
		UserItem {
			enabled
		},
		/* [8] */
		{6, 12, 25, 230},
		UserItem {
			enabled
		},
		/* [9] */
		{91, 251, 92, 333},
		Picture {
			disabled,
			11
		},
		/* [10] */
		{170, 250, 256, 336},
		Picture {
			disabled,
			133
		},
		/* [11] */
		{255, 10, 273, 177},
		CheckBox {
			enabled,
			"Play when clicked"
		},
		/* [12] */
		{255, 189, 271, 230},
		StaticText {
			disabled,
			""
		},
		/* [13] */
		{276, 10, 293, 130},
		CheckBox {
			enabled,
			"Show All Files"
		},
		/* [14] */
		{298, 108, 315, 236},
		StaticText {
			disabled,
			"Available formats: "
		},
		/* [15] */
		{315, 13, 363, 337},
		StaticText {
			disabled,
			"y\ny\ny"
		},
		/* [16] */
		{295, 0, 296, 352},
		Picture {
			disabled,
			11
		}
	}
};

resource 'DITL' (346) {
	{	/* array DITLarray: 3 elements */
		/* [1] */
		{0, 0, 95, 385},
		Picture {
			disabled,
			128
		},
		/* [2] */
		{2, 104, 21, 231},
		Picture {
			disabled,
			151
		},
		/* [3] */
		{95, 0, 215, 385},
		Picture {
			disabled,
			141
		}
	}
};

resource 'DITL' (934) {
	{	/* array DITLarray: 3 elements */
		/* [1] */
		{73, 174, 93, 232},
		Button {
			enabled,
			"OK"
		},
		/* [2] */
		{2, 41, 68, 235},
		StaticText {
			disabled,
			"Currently, ^0% of the processing time is"
			" used for music generation and other bac"
			"kground processes."
		},
		/* [3] */
		{3, 4, 35, 36},
		Picture {
			disabled,
			220
		}
	}
};

resource 'DITL' (337, "Spectre") {
	{	/* array DITLarray: 11 elements */
		/* [1] */
		{16, 0, 36, 20},
		UserItem {
			disabled
		},
		/* [2] */
		{1, 275, 14, 315},
		StaticText {
			disabled,
			"Scale:"
		},
		/* [3] */
		{1, 316, 14, 356},
		StaticText {
			disabled,
			""
		},
		/* [4] */
		{1, 3, 14, 54},
		StaticText {
			disabled,
			"Display:"
		},
		/* [5] */
		{1, 139, 14, 159},
		Picture {
			enabled,
			900
		},
		/* [6] */
		{1, 55, 14, 138},
		StaticText {
			disabled,
			""
		},
		/* [7] */
		{1, 171, 14, 204},
		StaticText {
			disabled,
			"Size:"
		},
		/* [8] */
		{1, 236, 14, 256},
		Picture {
			enabled,
			900
		},
		/* [9] */
		{1, 205, 14, 233},
		StaticText {
			disabled,
			""
		},
		/* [10] */
		{16, 0, 2016, 2000},
		UserItem {
			enabled
		},
		/* [11] */
		{1, 359, 14, 379},
		Picture {
			enabled,
			900
		}
	}
};

resource 'DITL' (176) {
	{	/* array DITLarray: 21 elements */
		/* [1] */
		{10, 381, 30, 439},
		Button {
			enabled,
			"OK"
		},
		/* [2] */
		{40, 381, 60, 439},
		Button {
			enabled,
			"Cancel"
		},
		/* [3] */
		{4, 20, 17, 72},
		StaticText {
			disabled,
			"Category:"
		},
		/* [4] */
		{20, 12, 33, 72},
		StaticText {
			disabled,
			"Instrument:"
		},
		/* [5] */
		{4, 75, 17, 95},
		Picture {
			enabled,
			200
		},
		/* [6] */
		{20, 75, 33, 95},
		Picture {
			enabled,
			200
		},
		/* [7] */
		{4, 100, 17, 222},
		StaticText {
			disabled,
			""
		},
		/* [8] */
		{20, 100, 33, 222},
		StaticText {
			disabled,
			""
		},
		/* [9] */
		{74, 41, 87, 72},
		StaticText {
			disabled,
			"Note:"
		},
		/* [10] */
		{74, 100, 87, 129},
		StaticText {
			enabled,
			""
		},
		/* [11] */
		{74, 75, 87, 95},
		Picture {
			enabled,
			200
		},
		/* [12] */
		{36, 42, 49, 72},
		StaticText {
			disabled,
			"Size:"
		},
		/* [13] */
		{89, 4, 147, 453},
		Picture {
			enabled,
			225
		},
		/* [14] */
		{36, 100, 49, 222},
		StaticText {
			enabled,
			""
		},
		/* [15] */
		{70, -18, 71, 782},
		Picture {
			disabled,
			11
		},
		/* [16] */
		{5, 479, 21, 563},
		UserItem {
			disabled
		},
		/* [17] */
		{52, 100, 65, 222},
		StaticText {
			enabled,
			""
		},
		/* [18] */
		{52, 10, 65, 72},
		StaticText {
			enabled,
			"MIDI-GM ID:"
		},
		/* [19] */
		{53, 557, 69, 649},
		UserItem {
			enabled
		},
		/* [20] */
		{53, 657, 69, 709},
		UserItem {
			disabled
		},
		/* [21] */
		{74, 350, 87, 454},
		StaticText {
			disabled,
			"Quicktime version 2.2"
		}
	}
};

resource 'DITL' (173) {
	{	/* array DITLarray: 1 elements */
		/* [1] */
		{1, 1, 15, 2001},
		StaticText {
			disabled,
			""
		}
	}
};

resource 'DITL' (153) {
	{	/* array DITLarray: 11 elements */
		/* [1] */
		{38, 14, 2038, 2014},
		UserItem {
			enabled
		},
		/* [2] */
		{24, 0, 38, 2000},
		UserItem {
			enabled
		},
		/* [3] */
		{1, 318, 21, 338},
		UserItem {
			enabled
		},
		/* [4] */
		{38, 0, 2038, 14},
		UserItem {
			enabled
		},
		/* [5] */
		{2, 24, 22, 44},
		UserItem {
			enabled
		},
		/* [6] */
		{2, 44, 22, 64},
		UserItem {
			enabled
		},
		/* [7] */
		{2, 4, 22, 24},
		UserItem {
			enabled
		},
		/* [8] */
		{2, 224, 22, 244},
		UserItem {
			enabled
		},
		/* [9] */
		{5, 77, 18, 110},
		StaticText {
			disabled,
			"Size:"
		},
		/* [10] */
		{5, 140, 18, 160},
		Picture {
			enabled,
			900
		},
		/* [11] */
		{5, 111, 18, 139},
		StaticText {
			disabled,
			""
		}
	}
};

resource 'DITL' (174) {
	{	/* array DITLarray: 5 elements */
		/* [1] */
		{313, 183, 333, 241},
		Button {
			enabled,
			"Print"
		},
		/* [2] */
		{313, 267, 333, 325},
		Button {
			enabled,
			"Done"
		},
		/* [3] */
		{133, -14, 134, 486},
		Picture {
			disabled,
			11
		},
		/* [4] */
		{6, 11, 127, 323},
		Picture {
			enabled,
			135
		},
		/* [5] */
		{305, -33, 306, 467},
		Picture {
			disabled,
			11
		}
	}
};

resource 'DITL' (154) {
	{	/* array DITLarray: 14 elements */
		/* [1] */
		{5, 365, 25, 423},
		Button {
			enabled,
			"OK"
		},
		/* [2] */
		{10, 10, 168, 160},
		UserItem {
			enabled
		},
		/* [3] */
		{10, 170, 26, 235},
		StaticText {
			disabled,
			"Size on disk:"
		},
		/* [4] */
		{30, 170, 46, 235},
		StaticText {
			disabled,
			"Size in RAM:"
		},
		/* [5] */
		{50, 170, 66, 235},
		StaticText {
			disabled,
			"Code:"
		},
		/* [6] */
		{70, 170, 86, 235},
		StaticText {
			disabled,
			"Type:"
		},
		/* [7] */
		{90, 170, 106, 235},
		StaticText {
			disabled,
			"Copyright:"
		},
		/* [8] */
		{120, 170, 136, 235},
		StaticText {
			disabled,
			"Where:"
		},
		/* [9] */
		{10, 240, 26, 346},
		StaticText {
			disabled,
			""
		},
		/* [10] */
		{30, 240, 46, 346},
		StaticText {
			disabled,
			""
		},
		/* [11] */
		{50, 240, 66, 346},
		StaticText {
			disabled,
			""
		},
		/* [12] */
		{70, 240, 86, 346},
		StaticText {
			disabled,
			""
		},
		/* [13] */
		{90, 240, 116, 422},
		StaticText {
			disabled,
			""
		},
		/* [14] */
		{120, 240, 168, 422},
		StaticText {
			disabled,
			""
		}
	}
};

resource 'DITL' (137, purgeable) {
	{	/* array DITLarray: 13 elements */
		/* [1] */
		{161, 252, 181, 332},
		Button {
			enabled,
			"Save"
		},
		/* [2] */
		{130, 252, 150, 332},
		Button {
			enabled,
			"Cancel"
		},
		/* [3] */
		{0, 0, 0, 0},
		UserItem {
			disabled
		},
		/* [4] */
		{8, 235, 24, 337},
		UserItem {
			enabled
		},
		/* [5] */
		{32, 252, 52, 332},
		Button {
			enabled,
			"Eject"
		},
		/* [6] */
		{60, 252, 80, 332},
		Button {
			enabled,
			"Desktop"
		},
		/* [7] */
		{29, 12, 234, 230},
		UserItem {
			enabled
		},
		/* [8] */
		{7, 12, 26, 230},
		UserItem {
			enabled
		},
		/* [9] */
		{119, 250, 120, 334},
		Picture {
			disabled,
			11
		},
		/* [10] */
		{262, 16, 278, 228},
		EditText {
			enabled,
			""
		},
		/* [11] */
		{241, 16, 257, 182},
		StaticText {
			disabled,
			"Save document as:"
		},
		/* [12] */
		{88, 252, 108, 332},
		UserItem {
			disabled
		},
		/* [13] */
		{195, 250, 281, 336},
		Picture {
			disabled,
			133
		}
	}
};

resource 'DITL' (171) {
	{	/* array DITLarray: 12 elements */
		/* [1] */
		{5, 200, 25, 258},
		Button {
			enabled,
			"OK"
		},
		/* [2] */
		{35, 200, 55, 258},
		Button {
			enabled,
			"Cancel"
		},
		/* [3] */
		{3, 13, 16, 45},
		StaticText {
			disabled,
			"Type:"
		},
		/* [4] */
		{3, 48, 16, 119},
		StaticText {
			enabled,
			""
		},
		/* [5] */
		{43, 3, 56, 45},
		StaticText {
			disabled,
			"Format:"
		},
		/* [6] */
		{43, 48, 56, 155},
		StaticText {
			enabled,
			""
		},
		/* [7] */
		{43, 157, 56, 177},
		Picture {
			enabled,
			200
		},
		/* [8] */
		{63, 11, 76, 45},
		StaticText {
			disabled,
			"Name:"
		},
		/* [9] */
		{63, 48, 76, 206},
		EditText {
			enabled,
			""
		},
		/* [10] */
		{23, 16, 36, 45},
		StaticText {
			disabled,
			"Size:"
		},
		/* [11] */
		{23, 48, 36, 135},
		StaticText {
			enabled,
			""
		},
		/* [12] */
		{174, 548, 190, 663},
		StaticText {
			enabled,
			""
		}
	}
};

resource 'DITL' (301, purgeable) {
	{	/* array DITLarray: 5 elements */
		/* [1] */
		{60, 20, 80, 100},
		Button {
			enabled,
			"Remove it"
		},
		/* [2] */
		{92, 20, 112, 100},
		Button {
			enabled,
			"Find it"
		},
		/* [3] */
		{92, 190, 112, 270},
		Button {
			enabled,
			"Cancel"
		},
		/* [4] */
		{6, 60, 54, 270},
		StaticText {
			disabled,
			"I cannot find “^0”.\nWhat shall I do?"
		},
		/* [5] */
		{10, 10, 42, 42},
		Picture {
			disabled,
			220
		}
	}
};

resource 'DITL' (3004, purgeable) {
	{	/* array DITLarray: 5 elements */
		/* [1] */
		{60, 20, 80, 90},
		Button {
			enabled,
			"Yes"
		},
		/* [2] */
		{92, 20, 112, 90},
		Button {
			enabled,
			"No"
		},
		/* [3] */
		{92, 200, 112, 270},
		Button {
			enabled,
			"Cancel"
		},
		/* [4] */
		{6, 60, 54, 270},
		StaticText {
			disabled,
			"Save current Music List “^0” before clos"
			"ing?"
		},
		/* [5] */
		{10, 10, 42, 42},
		Picture {
			disabled,
			220
		}
	}
};

resource 'DITL' (302) {
	{	/* array DITLarray: 3 elements */
		/* [1] */
		{81, 270, 101, 328},
		Button {
			enabled,
			"OK"
		},
		/* [2] */
		{10, 60, 67, 331},
		StaticText {
			disabled,
			"Your music is now ^0 bytes smaller than "
			"before..."
		},
		/* [3] */
		{10, 10, 42, 42},
		Picture {
			disabled,
			220
		}
	}
};

resource 'DITL' (134) {
	{	/* array DITLarray: 25 elements */
		/* [1] */
		{40, 62, 2040, 2062},
		UserItem {
			enabled
		},
		/* [2] */
		{26, 0, 40, 2000},
		UserItem {
			enabled
		},
		/* [3] */
		{3, 22, 23, 42},
		UserItem {
			enabled
		},
		/* [4] */
		{40, 0, 2040, 62},
		UserItem {
			enabled
		},
		/* [5] */
		{3, 2, 23, 22},
		UserItem {
			enabled
		},
		/* [6] */
		{3, 172, 23, 192},
		UserItem {
			enabled
		},
		/* [7] */
		{3, 48, 23, 68},
		UserItem {
			enabled
		},
		/* [8] */
		{3, 73, 23, 93},
		UserItem {
			enabled
		},
		/* [9] */
		{5, 368, 21, 395},
		StaticText {
			disabled,
			"Ins:"
		},
		/* [10] */
		{6, 424, 19, 444},
		Picture {
			enabled,
			200
		},
		/* [11] */
		{5, 396, 21, 422},
		StaticText {
			enabled,
			"000"
		},
		/* [12] */
		{26, 0, 40, 62},
		UserItem {
			disabled
		},
		/* [13] */
		{1, 196, 25, 215},
		UserItem {
			enabled
		},
		/* [14] */
		{1, 214, 25, 233},
		UserItem {
			enabled
		},
		/* [15] */
		{1, 232, 25, 251},
		UserItem {
			enabled
		},
		/* [16] */
		{1, 250, 25, 269},
		UserItem {
			enabled
		},
		/* [17] */
		{1, 268, 25, 287},
		UserItem {
			enabled
		},
		/* [18] */
		{1, 310, 25, 329},
		UserItem {
			enabled
		},
		/* [19] */
		{1, 328, 25, 347},
		UserItem {
			enabled
		},
		/* [20] */
		{1, 346, 25, 365},
		UserItem {
			enabled
		},
		/* [21] */
		{3, 98, 23, 118},
		UserItem {
			enabled
		},
		/* [22] */
		{3, 123, 23, 143},
		UserItem {
			enabled
		},
		/* [23] */
		{3, 148, 23, 168},
		UserItem {
			enabled
		},
		/* [24] */
		{5, 447, 21, 745},
		StaticText {
			enabled,
			""
		},
		/* [25] */
		{1, 286, 25, 305},
		UserItem {
			enabled
		}
	}
};

resource 'DITL' (2007) {
	{	/* array DITLarray: 16 elements */
		/* [1] */
		{40, 10, 56, 136},
		StaticText {
			enabled,
			"General:"
		},
		/* [2] */
		{174, 31, 198, 267},
		UserItem {
			disabled
		},
		/* [3] */
		{88, 29, 104, 105},
		CheckBox {
			enabled,
			"Markers:"
		},
		/* [4] */
		{88, 117, 104, 163},
		StaticText {
			disabled,
			"Offset:"
		},
		/* [5] */
		{88, 279, 104, 329},
		EditText {
			enabled,
			""
		},
		/* [6] */
		{88, 236, 104, 273},
		StaticText {
			disabled,
			"Loop:"
		},
		/* [7] */
		{88, 169, 104, 219},
		EditText {
			enabled,
			""
		},
		/* [8] */
		{65, 29, 81, 180},
		CheckBox {
			enabled,
			"Show notes length"
		},
		/* [9] */
		{112, 29, 128, 105},
		StaticText {
			enabled,
			"Tempo:"
		},
		/* [10] */
		{112, 141, 128, 163},
		StaticText {
			disabled,
			"No:"
		},
		/* [11] */
		{112, 279, 128, 329},
		EditText {
			enabled,
			""
		},
		/* [12] */
		{112, 242, 128, 273},
		StaticText {
			disabled,
			"Unit:"
		},
		/* [13] */
		{112, 169, 128, 219},
		EditText {
			enabled,
			""
		},
		/* [14] */
		{136, 28, 152, 124},
		StaticText {
			disabled,
			"Track Height:"
		},
		/* [15] */
		{136, 169, 152, 207},
		StaticText {
			enabled,
			"128"
		},
		/* [16] */
		{137, 209, 150, 229},
		Picture {
			enabled,
			200
		}
	}
};

resource 'DITL' (2008) {
	{	/* array DITLarray: 40 elements */
		/* [1] */
		{40, 10, 56, 136},
		StaticText {
			enabled,
			"General:"
		},
		/* [2] */
		{61, 20, 77, 69},
		CheckBox {
			enabled,
			"F1"
		},
		/* [3] */
		{77, 20, 93, 69},
		CheckBox {
			enabled,
			"F2"
		},
		/* [4] */
		{93, 20, 109, 69},
		CheckBox {
			enabled,
			"F3"
		},
		/* [5] */
		{109, 20, 125, 69},
		CheckBox {
			enabled,
			"F4"
		},
		/* [6] */
		{125, 20, 141, 69},
		CheckBox {
			enabled,
			"F5"
		},
		/* [7] */
		{141, 20, 157, 69},
		CheckBox {
			enabled,
			"F6"
		},
		/* [8] */
		{157, 20, 173, 69},
		CheckBox {
			enabled,
			"F7"
		},
		/* [9] */
		{173, 20, 189, 69},
		CheckBox {
			enabled,
			"F8"
		},
		/* [10] */
		{189, 20, 205, 69},
		CheckBox {
			enabled,
			"F9"
		},
		/* [11] */
		{205, 20, 221, 69},
		CheckBox {
			enabled,
			"F10"
		},
		/* [12] */
		{221, 20, 237, 69},
		CheckBox {
			enabled,
			"F11"
		},
		/* [13] */
		{237, 20, 253, 69},
		CheckBox {
			enabled,
			"F12"
		},
		/* [14] */
		{253, 20, 269, 69},
		CheckBox {
			enabled,
			"F13"
		},
		/* [15] */
		{62, 71, 75, 91},
		Picture {
			enabled,
			200
		},
		/* [16] */
		{78, 71, 91, 91},
		Picture {
			enabled,
			200
		},
		/* [17] */
		{94, 71, 107, 91},
		Picture {
			enabled,
			200
		},
		/* [18] */
		{110, 71, 123, 91},
		Picture {
			enabled,
			200
		},
		/* [19] */
		{126, 71, 139, 91},
		Picture {
			enabled,
			200
		},
		/* [20] */
		{142, 71, 155, 91},
		Picture {
			enabled,
			200
		},
		/* [21] */
		{158, 71, 171, 91},
		Picture {
			enabled,
			200
		},
		/* [22] */
		{174, 71, 187, 91},
		Picture {
			enabled,
			200
		},
		/* [23] */
		{190, 71, 203, 91},
		Picture {
			enabled,
			200
		},
		/* [24] */
		{206, 71, 219, 91},
		Picture {
			enabled,
			200
		},
		/* [25] */
		{222, 71, 235, 91},
		Picture {
			enabled,
			200
		},
		/* [26] */
		{238, 71, 251, 91},
		Picture {
			enabled,
			200
		},
		/* [27] */
		{254, 71, 267, 91},
		Picture {
			enabled,
			200
		},
		/* [28] */
		{61, 95, 77, 345},
		StaticText {
			disabled,
			""
		},
		/* [29] */
		{77, 95, 93, 345},
		StaticText {
			disabled,
			""
		},
		/* [30] */
		{93, 95, 109, 345},
		StaticText {
			disabled,
			""
		},
		/* [31] */
		{109, 95, 125, 345},
		StaticText {
			disabled,
			""
		},
		/* [32] */
		{125, 95, 141, 345},
		StaticText {
			disabled,
			""
		},
		/* [33] */
		{141, 95, 157, 345},
		StaticText {
			disabled,
			""
		},
		/* [34] */
		{157, 95, 173, 345},
		StaticText {
			disabled,
			""
		},
		/* [35] */
		{173, 95, 189, 345},
		StaticText {
			disabled,
			""
		},
		/* [36] */
		{189, 95, 205, 345},
		StaticText {
			disabled,
			""
		},
		/* [37] */
		{205, 95, 221, 345},
		StaticText {
			disabled,
			""
		},
		/* [38] */
		{221, 95, 237, 345},
		StaticText {
			disabled,
			""
		},
		/* [39] */
		{237, 95, 253, 345},
		StaticText {
			disabled,
			""
		},
		/* [40] */
		{253, 95, 269, 345},
		StaticText {
			disabled,
			""
		}
	}
};

resource 'DITL' (195) {
	{	/* array DITLarray: 1 elements */
		/* [1] */
		{-5, -4, 31995, 31996},
		UserItem {
			enabled
		}
	}
};

resource 'DITL' (149, "Custom open controls") {
	{	/* array DITLarray: 10 elements */
		/* [1] */
		{-2, 5, 11, 14},
		UserItem {
			disabled
		},
		/* [2] */
		{13, 6, 30, 50},
		StaticText {
			enabled,
			"Show: "
		},
		/* [3] */
		{-1, 9, 12, 23},
		UserItem {
			disabled
		},
		/* [4] */
		{-2, 5, 9, 18},
		UserItem {
			disabled
		},
		/* [5] */
		{3, 0, 10, 13},
		UserItem {
			disabled
		},
		/* [6] */
		{13, 77, 29, 222},
		StaticText {
			enabled,
			""
		},
		/* [7] */
		{14, 52, 27, 72},
		Picture {
			enabled,
			901
		},
		/* [8] */
		{0, -200, 20, -120},
		Button {
			disabled,
			"Add"
		},
		/* [9] */
		{0, 3, 12, 18},
		UserItem {
			disabled
		},
		/* [10] */
		{0, -200, 20, -120},
		Button {
			disabled,
			"Preview"
		}
	}
};

resource 'DITL' (135) {
	{	/* array DITLarray: 22 elements */
		/* [1] */
		{2, 3, 22, 23},
		UserItem {
			enabled
		},
		/* [2] */
		{2, 109, 22, 129},
		UserItem {
			enabled
		},
		/* [3] */
		{2, 129, 22, 149},
		UserItem {
			enabled
		},
		/* [4] */
		{2, 149, 22, 169},
		UserItem {
			enabled
		},
		/* [5] */
		{2, 169, 22, 189},
		UserItem {
			enabled
		},
		/* [6] */
		{2, 243, 22, 263},
		UserItem {
			enabled
		},
		/* [7] */
		{2, 195, 22, 215},
		UserItem {
			enabled
		},
		/* [8] */
		{2, 219, 22, 239},
		UserItem {
			enabled
		},
		/* [9] */
		{38, 51, 2038, 2051},
		UserItem {
			enabled
		},
		/* [10] */
		{6, 287, 22, 296},
		UserItem {
			disabled
		},
		/* [11] */
		{38, 31, 2038, 51},
		UserItem {
			enabled
		},
		/* [12] */
		{-2, 651, 14, 711},
		UserItem {
			enabled
		},
		/* [13] */
		{38, 15, 2038, 31},
		UserItem {
			enabled
		},
		/* [14] */
		{24, 0, 38, 2000},
		UserItem {
			enabled
		},
		/* [15] */
		{2, 303, 22, 323},
		UserItem {
			enabled
		},
		/* [16] */
		{38, 0, 2038, 15},
		UserItem {
			disabled
		},
		/* [17] */
		{3, 32, 19, 57},
		UserItem {
			enabled
		},
		/* [18] */
		{3, 67, 19, 86},
		UserItem {
			enabled
		},
		/* [19] */
		{3, 85, 19, 102},
		UserItem {
			enabled
		},
		/* [20] */
		{3, 56, 19, 68},
		UserItem {
			enabled
		},
		/* [21] */
		{3, 32, 19, 102},
		UserItem {
			enabled
		},
		/* [22] */
		{24, 0, 38, 51},
		UserItem {
			disabled
		}
	}
};

resource 'DITL' (845) {
	{	/* array DITLarray: 16 elements */
		/* [1] */
		{3, 3, 67, 209},
		UserItem {
			disabled
		},
		/* [2] */
		{3, 2, 67, 15},
		UserItem {
			disabled
		},
		/* [3] */
		{70, 2, 134, 15},
		UserItem {
			disabled
		},
		/* [4] */
		{137, 2, 201, 15},
		UserItem {
			disabled
		},
		/* [5] */
		{204, 2, 268, 15},
		UserItem {
			disabled
		},
		/* [6] */
		{70, 3, 134, 209},
		UserItem {
			disabled
		},
		/* [7] */
		{137, 3, 201, 209},
		UserItem {
			disabled
		},
		/* [8] */
		{204, 3, 268, 209},
		UserItem {
			disabled
		},
		/* [9] */
		{271, 2, 335, 208},
		UserItem {
			disabled
		},
		/* [10] */
		{271, 1, 335, 14},
		UserItem {
			disabled
		},
		/* [11] */
		{338, 1, 402, 14},
		UserItem {
			disabled
		},
		/* [12] */
		{405, 1, 469, 14},
		UserItem {
			disabled
		},
		/* [13] */
		{472, 1, 536, 14},
		UserItem {
			disabled
		},
		/* [14] */
		{338, 2, 402, 208},
		UserItem {
			disabled
		},
		/* [15] */
		{405, 2, 469, 208},
		UserItem {
			disabled
		},
		/* [16] */
		{472, 2, 536, 208},
		UserItem {
			disabled
		}
	}
};

resource 'DITL' (132, "MOD Options") {
	{	/* array DITLarray: 4 elements */
		/* [1] */
		{5, 225, 25, 283},
		Button {
			enabled,
			"OK"
		},
		/* [2] */
		{5, 290, 25, 348},
		Button {
			enabled,
			"Cancel"
		},
		/* [3] */
		{6, 20, 25, 206},
		Control {
			enabled,
			141
		},
		/* [4] */
		{32, 0, 33, 360},
		Picture {
			disabled,
			201
		}
	}
};

resource 'DITL' (177, purgeable) {
	{	/* array DITLarray: 3 elements */
		/* [1] */
		{220, 252, 240, 332},
		Button {
			enabled,
			"Continue"
		},
		/* [2] */
		{24, 6, 212, 334},
		UserItem {
			enabled
		},
		/* [3] */
		{0, 8, 20, 272},
		StaticText {
			disabled,
			"Read this before using this music:"
		}
	}
};

resource 'DITL' (175) {
	{	/* array DITLarray: 10 elements */
		/* [1] */
		{229, 318, 249, 376},
		Button {
			enabled,
			"OK"
		},
		/* [2] */
		{262, 318, 282, 376},
		Button {
			enabled,
			"Quit"
		},
		/* [3] */
		{263, 22, 279, 252},
		EditText {
			enabled,
			""
		},
		/* [4] */
		{162, 22, 178, 252},
		EditText {
			enabled,
			""
		},
		/* [5] */
		{240, 22, 258, 300},
		StaticText {
			disabled,
			"Serial Number (included in your CD):"
		},
		/* [6] */
		{138, 22, 156, 111},
		StaticText {
			disabled,
			"Name:"
		},
		/* [7] */
		{109, 74, 128, 368},
		StaticText {
			disabled,
			"Please enter following informations:"
		},
		/* [8] */
		{0, 0, 95, 385},
		Picture {
			enabled,
			128
		},
		/* [9] */
		{213, 22, 229, 252},
		EditText {
			enabled,
			""
		},
		/* [10] */
		{190, 22, 208, 152},
		StaticText {
			disabled,
			"Organization:"
		}
	}
};

resource 'DITL' (2002) {
	{	/* array DITLarray: 15 elements */
		/* [1] */
		{80, 20, 98, 164},
		RadioButton {
			enabled,
			"Loop music"
		},
		/* [2] */
		{60, 171, 78, 304},
		RadioButton {
			enabled,
			"Load next music"
		},
		/* [3] */
		{80, 171, 100, 352},
		RadioButton {
			enabled,
			"Load a music randomly"
		},
		/* [4] */
		{40, 8, 56, 201},
		StaticText {
			disabled,
			"After having played a music:"
		},
		/* [5] */
		{0, 0, 35, 259},
		UserItem {
			disabled
		},
		/* [6] */
		{189, 21, 207, 308},
		CheckBox {
			enabled,
			"Show real Finder icons"
		},
		/* [7] */
		{60, 20, 78, 164},
		RadioButton {
			enabled,
			"Stop playing"
		},
		/* [8] */
		{149, 21, 167, 336},
		CheckBox {
			enabled,
			"Load first music when opening a music li"
			"st"
		},
		/* [9] */
		{104, 0, 105, 352},
		Picture {
			disabled,
			902
		},
		/* [10] */
		{110, 220, 125, 244},
		UserItem {
			disabled
		},
		/* [11] */
		{169, 21, 187, 322},
		CheckBox {
			enabled,
			"Go to Startup position when done playing"
		},
		/* [12] */
		{209, 21, 227, 308},
		CheckBox {
			enabled,
			"Ask to save modified Music List files"
		},
		/* [13] */
		{229, 21, 247, 308},
		CheckBox {
			enabled,
			"Automatic 'Play' after opening a music"
		},
		/* [14] */
		{109, 8, 125, 201},
		StaticText {
			disabled,
			"Music List Settings:"
		},
		/* [15] */
		{129, 21, 147, 309},
		CheckBox {
			enabled,
			"Remember Music List for next startup"
		}
	}
};

resource 'DITL' (200) {
	{	/* array DITLarray: 8 elements */
		/* [1] */
		{0, 0, 95, 385},
		Picture {
			disabled,
			128
		},
		/* [2] */
		{109, 143, 123, 242},
		UserItem {
			disabled
		},
		/* [3] */
		{247, 92, 261, 188},
		UserItem {
			disabled
		},
		/* [4] */
		{149, 92, 163, 292},
		UserItem {
			disabled
		},
		/* [5] */
		{0, 0, 228, 385},
		UserItem {
			disabled
		},
		/* [6] */
		{256, 28, 288, 228},
		UserItem {
			disabled
		},
		/* [7] */
		{176, 392, 208, 592},
		Picture {
			disabled,
			137
		},
		/* [8] */
		{208, 92, 226, 292},
		Picture {
			disabled,
			156
		}
	}
};

resource 'DITL' (761) {
	{	/* array DITLarray: 46 elements */
		/* [1] */
		{5, 8, 25, 66},
		Button {
			enabled,
			"OK"
		},
		/* [2] */
		{105, 172, 119, 220},
		StaticText {
			disabled,
			"Playthru:"
		},
		/* [3] */
		{105, 220, 119, 267},
		StaticText {
			enabled,
			"true"
		},
		/* [4] */
		{105, 271, 118, 291},
		Picture {
			enabled,
			200
		},
		/* [5] */
		{38, 5, 52, 161},
		StaticText {
			disabled,
			"Audio InPut:"
		},
		/* [6] */
		{70, 5, 119, 126},
		UserItem {
			enabled
		},
		/* [7] */
		{53, 5, 67, 76},
		StaticText {
			enabled,
			"Sound Driver:"
		},
		/* [8] */
		{77, 129, 109, 161},
		UserItem {
			disabled
		},
		/* [9] */
		{71, 191, 85, 221},
		StaticText {
			disabled,
			"Rate:"
		},
		/* [10] */
		{54, 193, 68, 221},
		StaticText {
			disabled,
			"Bits:"
		},
		/* [11] */
		{54, 220, 68, 267},
		StaticText {
			enabled,
			"8"
		},
		/* [12] */
		{54, 271, 67, 291},
		Picture {
			enabled,
			200
		},
		/* [13] */
		{71, 220, 85, 267},
		StaticText {
			enabled,
			"44100 Hz"
		},
		/* [14] */
		{71, 271, 84, 291},
		Picture {
			enabled,
			200
		},
		/* [15] */
		{33, -70, 34, 430},
		Picture {
			disabled,
			11
		},
		/* [16] */
		{5, 191, 25, 249},
		Button {
			enabled,
			"Cancel"
		},
		/* [17] */
		{122, -80, 123, 420},
		Picture {
			disabled,
			11
		},
		/* [18] */
		{150, 8, 170, 36},
		UserItem {
			enabled
		},
		/* [19] */
		{150, 49, 170, 77},
		UserItem {
			enabled
		},
		/* [20] */
		{150, 88, 170, 116},
		UserItem {
			enabled
		},
		/* [21] */
		{150, 128, 170, 156},
		UserItem {
			enabled
		},
		/* [22] */
		{131, 5, 147, 41},
		StaticText {
			disabled,
			"Name:"
		},
		/* [23] */
		{170, 5, 184, 38},
		StaticText {
			disabled,
			"Record"
		},
		/* [24] */
		{170, 52, 184, 74},
		StaticText {
			disabled,
			"Stop"
		},
		/* [25] */
		{170, 88, 184, 116},
		StaticText {
			disabled,
			"Pause"
		},
		/* [26] */
		{170, 130, 184, 151},
		StaticText {
			disabled,
			"Play"
		},
		/* [27] */
		{186, 8, 203, 156},
		UserItem {
			disabled
		},
		/* [28] */
		{137, 209, 151, 253},
		StaticText {
			disabled,
			"Available"
		},
		/* [29] */
		{165, 179, 179, 221},
		StaticText {
			disabled,
			"KBytes:"
		},
		/* [30] */
		{183, 210, 197, 253},
		StaticText {
			disabled,
			"Recorded"
		},
		/* [31] */
		{211, 179, 225, 221},
		StaticText {
			disabled,
			"KBytes:"
		},
		/* [32] */
		{151, 190, 165, 221},
		StaticText {
			disabled,
			"Secs:"
		},
		/* [33] */
		{197, 190, 211, 221},
		StaticText {
			disabled,
			"Secs:"
		},
		/* [34] */
		{165, 221, 179, 291},
		StaticText {
			disabled,
			""
		},
		/* [35] */
		{151, 221, 165, 291},
		StaticText {
			disabled,
			""
		},
		/* [36] */
		{211, 221, 225, 291},
		StaticText {
			disabled,
			""
		},
		/* [37] */
		{197, 221, 211, 291},
		StaticText {
			disabled,
			""
		},
		/* [38] */
		{206, 8, 223, 156},
		UserItem {
			disabled
		},
		/* [39] */
		{131, 44, 144, 155},
		EditText {
			enabled,
			""
		},
		/* [40] */
		{226, 8, 234, 156},
		UserItem {
			disabled
		},
		/* [41] */
		{53, 79, 67, 147},
		StaticText {
			enabled,
			"Built-in"
		},
		/* [42] */
		{53, 149, 66, 169},
		Picture {
			enabled,
			200
		},
		/* [43] */
		{88, 188, 102, 218},
		StaticText {
			disabled,
			"Mode:"
		},
		/* [44] */
		{88, 220, 102, 267},
		StaticText {
			enabled,
			"mono"
		},
		/* [45] */
		{88, 271, 101, 291},
		Picture {
			enabled,
			200
		},
		/* [46] */
		{5, 103, 25, 161},
		Button {
			enabled,
			"Add it"
		}
	}
};

resource 'DITL' (2000) {
	{	/* array DITLarray: 35 elements */
		/* [1] */
		{0, 0, 36, 352},
		UserItem {
			disabled
		},
		/* [2] */
		{37, 60, 53, 315},
		RadioButton {
			enabled,
			"Sound Manager x.x (Mac OS)"
		},
		/* [3] */
		{211, 130, 232, 318},
		UserItem {
			disabled
		},
		/* [4] */
		{121, 184, 137, 237},
		StaticText {
			disabled,
			""
		},
		/* [5] */
		{55, 60, 71, 245},
		RadioButton {
			enabled,
			"MIDI Output - (OMS) :"
		},
		/* [6] */
		{78, 60, 94, 134},
		RadioButton {
			enabled,
			"11 Khz"
		},
		/* [7] */
		{78, 140, 94, 207},
		RadioButton {
			enabled,
			"22 Khz"
		},
		/* [8] */
		{78, 220, 94, 305},
		RadioButton {
			enabled,
			"44 Khz"
		},
		/* [9] */
		{96, 60, 112, 132},
		RadioButton {
			enabled,
			"8 Bits"
		},
		/* [10] */
		{96, 140, 112, 214},
		RadioButton {
			enabled,
			"16 Bits"
		},
		/* [11] */
		{96, 220, 112, 294},
		RadioButton {
			disabled,
			"24 Bits"
		},
		/* [12] */
		{181, 60, 197, 182},
		CheckBox {
			enabled,
			"Over Sampling:"
		},
		/* [13] */
		{182, 184, 197, 280},
		StaticText {
			disabled,
			"999 Khz - 20 x"
		},
		/* [14] */
		{224, 216, 240, 316},
		UserItem {
			disabled
		},
		/* [15] */
		{141, 270, 157, 306},
		StaticText {
			disabled,
			""
		},
		/* [16] */
		{224, 172, 237, 192},
		UserItem {
			disabled
		},
		/* [17] */
		{208, 213, 224, 236},
		UserItem {
			disabled
		},
		/* [18] */
		{216, 128, 232, 151},
		UserItem {
			disabled
		},
		/* [19] */
		{73, 0, 74, 352},
		Picture {
			disabled,
			902
		},
		/* [20] */
		{121, 60, 137, 175},
		CheckBox {
			enabled,
			"Stereo Delay:"
		},
		/* [21] */
		{220, 85, 237, 116},
		UserItem {
			disabled
		},
		/* [22] */
		{37, 2, 53, 54},
		StaticText {
			disabled,
			"Driver:"
		},
		/* [23] */
		{78, 10, 94, 51},
		StaticText {
			disabled,
			"Rate:"
		},
		/* [24] */
		{96, 15, 112, 51},
		StaticText {
			disabled,
			"Bits:"
		},
		/* [25] */
		{219, 46, 235, 93},
		UserItem {
			disabled
		},
		/* [26] */
		{123, 242, 136, 262},
		Picture {
			enabled,
			900
		},
		/* [27] */
		{116, 0, 117, 352},
		Picture {
			enabled,
			902
		},
		/* [28] */
		{54, 251, 72, 329},
		Button {
			enabled,
			"Set Up"
		},
		/* [29] */
		{121, 10, 137, 51},
		StaticText {
			disabled,
			"Misc:"
		},
		/* [30] */
		{161, 60, 177, 171},
		CheckBox {
			enabled,
			"Surround"
		},
		/* [31] */
		{141, 184, 157, 237},
		StaticText {
			disabled,
			""
		},
		/* [32] */
		{141, 60, 157, 156},
		CheckBox {
			enabled,
			"Reverb:"
		},
		/* [33] */
		{143, 242, 156, 262},
		Picture {
			enabled,
			900
		},
		/* [34] */
		{143, 308, 156, 328},
		Picture {
			enabled,
			900
		},
		/* [35] */
		{183, 284, 196, 304},
		Picture {
			enabled,
			900
		}
	}
};

resource 'DITL' (164, "Custom open INSTRU controls") {
	{	/* array DITLarray: 2 elements */
		/* [1] */
		{12, 8, 28, 168},
		CheckBox {
			enabled,
			"Play when clicked"
		},
		/* [2] */
		{12, 168, 28, 328},
		CheckBox {
			enabled,
			"Show All Files"
		}
	}
};

resource 'DITL' (178, purgeable) {
	{	/* array DITLarray: 33 elements */
		/* [1] */
		{12, 326, 32, 394},
		Button {
			enabled,
			"OK"
		},
		/* [2] */
		{45, 328, 65, 392},
		Button {
			enabled,
			"Cancel"
		},
		/* [3] */
		{5, 3, 21, 78},
		StaticText {
			disabled,
			"Header:"
		},
		/* [4] */
		{29, 3, 45, 71},
		StaticText {
			disabled,
			"Bits:"
		},
		/* [5] */
		{53, 3, 69, 71},
		StaticText {
			disabled,
			"Mode:"
		},
		/* [6] */
		{77, 3, 93, 71},
		StaticText {
			disabled,
			"Type:"
		},
		/* [7] */
		{124, 3, 139, 71},
		StaticText {
			disabled,
			"Length:"
		},
		/* [8] */
		{29, 100, 45, 166},
		RadioButton {
			enabled,
			"8 Bits"
		},
		/* [9] */
		{29, 211, 45, 277},
		RadioButton {
			enabled,
			"16 Bits"
		},
		/* [10] */
		{53, 100, 69, 166},
		RadioButton {
			enabled,
			"Mono"
		},
		/* [11] */
		{53, 211, 69, 277},
		RadioButton {
			enabled,
			"Stereo"
		},
		/* [12] */
		{77, 100, 93, 166},
		RadioButton {
			enabled,
			"Signed"
		},
		/* [13] */
		{77, 211, 93, 319},
		RadioButton {
			enabled,
			"Unsigned"
		},
		/* [14] */
		{101, 3, 117, 79},
		StaticText {
			disabled,
			"Coding:"
		},
		/* [15] */
		{101, 100, 117, 209},
		RadioButton {
			enabled,
			"Little Endian"
		},
		/* [16] */
		{101, 211, 117, 319},
		RadioButton {
			enabled,
			"Big Endian"
		},
		/* [17] */
		{5, 100, 21, 200},
		EditText {
			enabled,
			""
		},
		/* [18] */
		{5, 211, 21, 263},
		StaticText {
			disabled,
			"Bytes"
		},
		/* [19] */
		{123, 343, 139, 395},
		StaticText {
			disabled,
			"Bytes"
		},
		/* [20] */
		{123, 235, 139, 335},
		EditText {
			enabled,
			""
		},
		/* [21] */
		{191, 3, 207, 70},
		StaticText {
			disabled,
			"Preview:"
		},
		/* [22] */
		{148, 3, 163, 71},
		StaticText {
			disabled,
			"Rate:"
		},
		/* [23] */
		{148, 152, 164, 174},
		StaticText {
			disabled,
			"Hz"
		},
		/* [24] */
		{148, 100, 164, 147},
		EditText {
			enabled,
			""
		},
		/* [25] */
		{191, 71, 255, 402},
		UserItem {
			enabled
		},
		/* [26] */
		{123, 100, 139, 188},
		RadioButton {
			enabled,
			"End Of File"
		},
		/* [27] */
		{123, 211, 139, 229},
		RadioButton {
			enabled,
			""
		},
		/* [28] */
		{178, -14, 179, 586},
		Picture {
			disabled,
			11
		},
		/* [29] */
		{150, 176, 163, 196},
		Picture {
			enabled,
			901
		},
		/* [30] */
		{260, 3, 275, 65},
		StaticText {
			disabled,
			"Display:"
		},
		/* [31] */
		{260, 71, 276, 273},
		StaticText {
			disabled,
			""
		},
		/* [32] */
		{260, 277, 275, 402},
		StaticText {
			disabled,
			""
		},
		/* [33] */
		{221, 3, 241, 61},
		Button {
			enabled,
			"Play"
		}
	}
};

resource 'DITL' (163) {
	{	/* array DITLarray: 9 elements */
		/* [1] */
		{4, 7, 20, 301},
		UserItem {
			disabled
		},
		/* [2] */
		{23, 7, 40, 128},
		StaticText {
			disabled,
			"KBytes generated:"
		},
		/* [3] */
		{23, 128, 40, 300},
		StaticText {
			disabled,
			""
		},
		/* [4] */
		{63, 7, 79, 105},
		StaticText {
			disabled,
			"Elapsed time:"
		},
		/* [5] */
		{63, 185, 79, 301},
		StaticText {
			disabled,
			""
		},
		/* [6] */
		{83, 7, 99, 185},
		StaticText {
			disabled,
			"Estimated remaining time:"
		},
		/* [7] */
		{83, 181, 99, 301},
		StaticText {
			disabled,
			""
		},
		/* [8] */
		{43, 7, 59, 106},
		StaticText {
			disabled,
			"Throughput:"
		},
		/* [9] */
		{43, 122, 59, 301},
		StaticText {
			disabled,
			""
		}
	}
};

resource 'DITL' (136) {
	{	/* array DITLarray: 9 elements */
		/* [1] */
		{24, 235, 44, 293},
		Button {
			enabled,
			"Save"
		},
		/* [2] */
		{52, 235, 72, 293},
		Button {
			enabled,
			"Cancel"
		},
		/* [3] */
		{2, 3, 18, 256},
		StaticText {
			disabled,
			"Save current windows state in:"
		},
		/* [4] */
		{54, 63, 70, 219},
		EditText {
			enabled,
			""
		},
		/* [5] */
		{54, 8, 70, 57},
		StaticText {
			disabled,
			"Name:"
		},
		/* [6] */
		{30, 8, 46, 35},
		StaticText {
			disabled,
			"ID:"
		},
		/* [7] */
		{30, 63, 46, 83},
		StaticText {
			disabled,
			"0"
		},
		/* [8] */
		{31, 85, 44, 105},
		Picture {
			enabled,
			900
		},
		/* [9] */
		{81, 6, 101, 314},
		StaticText {
			disabled,
			"Direct saving by pressing : Ctrl +  \0x11 + "
			"ID"
		}
	}
};

resource 'DITL' (138) {
	{	/* array DITLarray: 7 elements */
		/* [1] */
		{69, 265, 89, 323},
		Button {
			enabled,
			"Reset"
		},
		/* [2] */
		{70, 190, 90, 248},
		Button {
			enabled,
			"Cancel"
		},
		/* [3] */
		{22, 83, 40, 372},
		CheckBox {
			enabled,
			"Delete all patterns and partition"
		},
		/* [4] */
		{2, 3, 18, 78},
		StaticText {
			disabled,
			"Do this:"
		},
		/* [5] */
		{43, 83, 61, 372},
		CheckBox {
			enabled,
			"Delete all instruments & samples"
		},
		/* [6] */
		{1, 83, 19, 372},
		CheckBox {
			enabled,
			"Purge all patterns"
		},
		/* [7] */
		{98, 2, 117, 334},
		StaticText {
			disabled,
			"Warning: You will not be able to undo th"
			"is reset!"
		}
	}
};

resource 'DITL' (139) {
	{	/* array DITLarray: 40 elements */
		/* [1] */
		{56, 21, 5056, 5021},
		UserItem {
			enabled
		},
		/* [2] */
		{2, 91, 22, 111},
		UserItem {
			enabled
		},
		/* [3] */
		{2, 33, 22, 53},
		UserItem {
			enabled
		},
		/* [4] */
		{2, 62, 22, 82},
		UserItem {
			enabled
		},
		/* [5] */
		{43, 0, 56, 1000},
		UserItem {
			enabled
		},
		/* [6] */
		{2, 4, 22, 24},
		UserItem {
			enabled
		},
		/* [7] */
		{2, 120, 22, 140},
		UserItem {
			enabled
		},
		/* [8] */
		{2, 207, 22, 227},
		UserItem {
			enabled
		},
		/* [9] */
		{56, 0, 1056, 21},
		UserItem {
			enabled
		},
		/* [10] */
		{2, 294, 22, 314},
		UserItem {
			enabled
		},
		/* [11] */
		{2, 149, 22, 169},
		UserItem {
			enabled
		},
		/* [12] */
		{2, 178, 22, 198},
		UserItem {
			enabled
		},
		/* [13] */
		{2, 236, 22, 256},
		UserItem {
			enabled
		},
		/* [14] */
		{2, 265, 22, 285},
		UserItem {
			enabled
		},
		/* [15] */
		{4, 455, 20, 482},
		UserItem {
			disabled
		},
		/* [16] */
		{4, 483, 20, 508},
		UserItem {
			enabled
		},
		/* [17] */
		{27, 77, 40, 97},
		Picture {
			enabled,
			200
		},
		/* [18] */
		{26, 374, 42, 417},
		Button {
			enabled,
			"Fill"
		},
		/* [19] */
		{4, 346, 20, 382},
		StaticText {
			disabled,
			"Step:"
		},
		/* [20] */
		{4, 381, 20, 394},
		StaticText {
			enabled,
			"1"
		},
		/* [21] */
		{5, 396, 18, 416},
		Picture {
			enabled,
			200
		},
		/* [22] */
		{24, -1, 25, 1999},
		Picture {
			disabled,
			137
		},
		/* [23] */
		{2, 323, 22, 343},
		UserItem {
			enabled
		},
		/* [24] */
		{4, 556, 20, 581},
		UserItem {
			disabled
		},
		/* [25] */
		{4, 642, 20, 671},
		UserItem {
			disabled
		},
		/* [26] */
		{4, 737, 20, 765},
		UserItem {
			disabled
		},
		/* [27] */
		{4, 582, 20, 597},
		UserItem {
			enabled
		},
		/* [28] */
		{4, 673, 20, 693},
		UserItem {
			enabled
		},
		/* [29] */
		{4, 764, 20, 783},
		UserItem {
			enabled
		},
		/* [30] */
		{27, 347, 40, 367},
		Picture {
			enabled,
			200
		},
		/* [31] */
		{27, 255, 40, 275},
		Picture {
			enabled,
			200
		},
		/* [32] */
		{27, 161, 40, 181},
		Picture {
			enabled,
			200
		},
		/* [33] */
		{24, 371, 44, 372},
		Picture {
			disabled,
			137
		},
		/* [34] */
		{24, 183, 44, 184},
		Picture {
			disabled,
			137
		},
		/* [35] */
		{24, 278, 44, 279},
		Picture {
			disabled,
			137
		},
		/* [36] */
		{24, 100, 44, 101},
		Picture {
			disabled,
			137
		},
		/* [37] */
		{26, 102, 42, 159},
		UserItem {
			enabled
		},
		/* [38] */
		{26, 187, 42, 253},
		UserItem {
			enabled
		},
		/* [39] */
		{26, 282, 42, 345},
		UserItem {
			enabled
		},
		/* [40] */
		{26, 3, 42, 74},
		UserItem {
			enabled
		}
	}
};

resource 'DITL' (319) {
	{	/* array DITLarray: 38 elements */
		/* [1] */
		{138, 344, 158, 402},
		Button {
			enabled,
			"OK"
		},
		/* [2] */
		{166, 344, 186, 402},
		Button {
			enabled,
			"Cancel"
		},
		/* [3] */
		{47, 347, 63, 414},
		RadioButton {
			enabled,
			"48 Khz"
		},
		/* [4] */
		{194, 188, 210, 284},
		StaticText {
			disabled,
			""
		},
		/* [5] */
		{289, 132, 305, 194},
		UserItem {
			disabled
		},
		/* [6] */
		{19, 207, 35, 470},
		StaticText {
			enabled,
			""
		},
		/* [7] */
		{106, 51, 122, 98},
		StaticText {
			disabled,
			"Mode:"
		},
		/* [8] */
		{134, 188, 150, 241},
		StaticText {
			disabled,
			""
		},
		/* [9] */
		{47, 98, 63, 172},
		RadioButton {
			enabled,
			"11 Khz"
		},
		/* [10] */
		{47, 178, 63, 245},
		RadioButton {
			enabled,
			"22 Khz"
		},
		/* [11] */
		{47, 258, 63, 343},
		RadioButton {
			enabled,
			"44 Khz"
		},
		/* [12] */
		{65, 98, 81, 170},
		RadioButton {
			enabled,
			"8 Bits"
		},
		/* [13] */
		{65, 178, 81, 252},
		RadioButton {
			enabled,
			"16 Bits"
		},
		/* [14] */
		{65, 258, 81, 332},
		RadioButton {
			disabled,
			"24 Bits"
		},
		/* [15] */
		{20, 181, 33, 201},
		Picture {
			enabled,
			901
		},
		/* [16] */
		{106, 178, 122, 250},
		RadioButton {
			enabled,
			"Stereo"
		},
		/* [17] */
		{106, 98, 122, 169},
		RadioButton {
			enabled,
			"Mono"
		},
		/* [18] */
		{154, 272, 170, 308},
		StaticText {
			disabled,
			""
		},
		/* [19] */
		{134, 60, 150, 164},
		CheckBox {
			enabled,
			"Stereo Delay:"
		},
		/* [20] */
		{195, 286, 208, 306},
		Picture {
			enabled,
			901
		},
		/* [21] */
		{47, 57, 63, 98},
		StaticText {
			disabled,
			"Rate:"
		},
		/* [22] */
		{65, 62, 81, 98},
		StaticText {
			disabled,
			"Bits:"
		},
		/* [23] */
		{194, 60, 210, 183},
		CheckBox {
			enabled,
			"Over Sampling:"
		},
		/* [24] */
		{136, 244, 149, 264},
		Picture {
			enabled,
			901
		},
		/* [25] */
		{38, -30, 39, 545},
		Picture {
			enabled,
			902
		},
		/* [26] */
		{134, 14, 150, 55},
		StaticText {
			disabled,
			"Misc:"
		},
		/* [27] */
		{174, 60, 190, 171},
		CheckBox {
			enabled,
			"Surround"
		},
		/* [28] */
		{154, 188, 170, 241},
		StaticText {
			disabled,
			""
		},
		/* [29] */
		{154, 60, 170, 135},
		CheckBox {
			enabled,
			"Reverb:"
		},
		/* [30] */
		{156, 244, 169, 264},
		Picture {
			enabled,
			901
		},
		/* [31] */
		{156, 310, 169, 330},
		Picture {
			enabled,
			901
		},
		/* [32] */
		{87, 98, 100, 118},
		Picture {
			enabled,
			901
		},
		/* [33] */
		{85, 3, 101, 98},
		StaticText {
			disabled,
			"Compression:"
		},
		/* [34] */
		{85, 127, 101, 361},
		StaticText {
			enabled,
			""
		},
		/* [35] */
		{129, -30, 130, 545},
		Picture {
			enabled,
			902
		},
		/* [36] */
		{1, 27, 17, 98},
		StaticText {
			disabled,
			"Selection:"
		},
		/* [37] */
		{1, 98, 17, 208},
		RadioButton {
			enabled,
			"Entire Music"
		},
		/* [38] */
		{19, 98, 35, 177},
		RadioButton {
			enabled,
			"Pattern:"
		}
	}
};

resource 'DITL' (2006) {
	{	/* array DITLarray: 32 elements */
		/* [1] */
		{60, 30, 76, 127},
		CheckBox {
			enabled,
			"Instrument"
		},
		/* [2] */
		{80, 30, 96, 87},
		CheckBox {
			enabled,
			"Note"
		},
		/* [3] */
		{60, 150, 76, 215},
		CheckBox {
			enabled,
			"Effect"
		},
		/* [4] */
		{80, 150, 96, 235},
		CheckBox {
			enabled,
			"Argument"
		},
		/* [5] */
		{40, 10, 56, 136},
		StaticText {
			enabled,
			"Cell informations:"
		},
		/* [6] */
		{60, 260, 76, 337},
		CheckBox {
			enabled,
			"Volume"
		},
		/* [7] */
		{100, 0, 101, 352},
		Picture {
			disabled,
			902
		},
		/* [8] */
		{110, 30, 126, 106},
		CheckBox {
			enabled,
			"Markers:"
		},
		/* [9] */
		{110, 120, 126, 166},
		StaticText {
			disabled,
			"Offset:"
		},
		/* [10] */
		{110, 280, 126, 330},
		EditText {
			enabled,
			""
		},
		/* [11] */
		{110, 240, 126, 277},
		StaticText {
			disabled,
			"Loop:"
		},
		/* [12] */
		{110, 170, 126, 220},
		EditText {
			enabled,
			""
		},
		/* [13] */
		{157, 0, 158, 352},
		Picture {
			disabled,
			902
		},
		/* [14] */
		{133, 170, 149, 220},
		UserItem {
			enabled
		},
		/* [15] */
		{164, 10, 180, 226},
		StaticText {
			disabled,
			"Pop-up keys: mouse click +"
		},
		/* [16] */
		{184, 30, 200, 127},
		CheckBox {
			enabled,
			"control"
		},
		/* [17] */
		{204, 30, 220, 87},
		CheckBox {
			enabled,
			"shift"
		},
		/* [18] */
		{184, 150, 200, 235},
		CheckBox {
			enabled,
			"command"
		},
		/* [19] */
		{204, 150, 220, 227},
		CheckBox {
			enabled,
			"option"
		},
		/* [20] */
		{230, 0, 231, 352},
		Picture {
			disabled,
			902
		},
		/* [21] */
		{236, 10, 252, 98},
		StaticText {
			disabled,
			"Lines height:"
		},
		/* [22] */
		{236, 150, 252, 218},
		RadioButton {
			enabled,
			"Normal"
		},
		/* [23] */
		{236, 248, 252, 311},
		RadioButton {
			enabled,
			"Small"
		},
		/* [24] */
		{190, 240, 206, 347},
		UserItem {
			disabled
		},
		/* [25] */
		{134, 121, 150, 164},
		StaticText {
			disabled,
			"Color:"
		},
		/* [26] */
		{296, 0, 297, 352},
		Picture {
			disabled,
			902
		},
		/* [27] */
		{257, 248, 273, 311},
		RadioButton {
			enabled,
			"Off"
		},
		/* [28] */
		{257, 150, 273, 218},
		RadioButton {
			enabled,
			"On"
		},
		/* [29] */
		{257, 10, 273, 98},
		StaticText {
			disabled,
			"Music Trace:"
		},
		/* [30] */
		{278, 150, 294, 218},
		RadioButton {
			enabled,
			"Loop"
		},
		/* [31] */
		{278, 248, 294, 355},
		RadioButton {
			enabled,
			"Partition"
		},
		/* [32] */
		{278, 10, 294, 139},
		StaticText {
			disabled,
			"Pattern Wrapping:"
		}
	}
};

resource 'DITL' (2009) {
	{	/* array DITLarray: 11 elements */
		/* [1] */
		{0, 0, 36, 352},
		UserItem {
			disabled
		},
		/* [2] */
		{69, 139, 89, 217},
		Button {
			enabled,
			"Set Up"
		},
		/* [3] */
		{115, 28, 133, 245},
		CheckBox {
			enabled,
			"MIDI Channel -> Ins & Tracks"
		},
		/* [4] */
		{135, 28, 153, 245},
		CheckBox {
			enabled,
			"MIDI Velocity -> Volume"
		},
		/* [5] */
		{155, 28, 173, 266},
		CheckBox {
			enabled,
			"Use E & D Effect for positionning"
		},
		/* [6] */
		{70, 10, 87, 138},
		CheckBox {
			enabled,
			"Use MIDI Input"
		},
		/* [7] */
		{39, 7, 57, 162},
		StaticText {
			disabled,
			"MIDI Input - (OMS) :"
		},
		/* [8] */
		{93, 28, 111, 142},
		StaticText {
			disabled,
			"Note Conversion:"
		},
		/* [9] */
		{37, 171, 57, 251},
		UserItem {
			disabled
		},
		/* [10] */
		{191, 10, 208, 278},
		CheckBox {
			enabled,
			"Send MIDI Timing Clock (MIDI Output)"
		},
		/* [11] */
		{214, 139, 234, 217},
		Button {
			enabled,
			"Set Up"
		}
	}
};

resource 'DITL' (130) {
	{	/* array DITLarray: 12 elements */
		/* [1] */
		{281, 5, 301, 88},
		Button {
			enabled,
			"Continue"
		},
		/* [2] */
		{90, 3, 138, 262},
		StaticText {
			disabled,
			"^0"
		},
		/* [3] */
		{72, 3, 88, 112},
		StaticText {
			disabled,
			"Description:"
		},
		/* [4] */
		{0, 24, 53, 246},
		StaticText {
			disabled,
			"FATAL ERROR !\nContact the author, please"
			", with the following informations :"
		},
		/* [5] */
		{281, 92, 301, 175},
		Button {
			enabled,
			"Debugger"
		},
		/* [6] */
		{281, 177, 301, 260},
		Button {
			enabled,
			"Exit"
		},
		/* [7] */
		{163, 4, 181, 264},
		StaticText {
			disabled,
			"^1"
		},
		/* [8] */
		{145, 4, 161, 113},
		StaticText {
			disabled,
			"Source Code:"
		},
		/* [9] */
		{209, 4, 227, 264},
		StaticText {
			disabled,
			"^2"
		},
		/* [10] */
		{191, 4, 207, 113},
		StaticText {
			disabled,
			"Line No:"
		},
		/* [11] */
		{256, 4, 274, 264},
		StaticText {
			disabled,
			"^3"
		},
		/* [12] */
		{238, 4, 254, 151},
		StaticText {
			disabled,
			"Compilation date:"
		}
	}
};

resource 'DITL' (2001) {
	{	/* array DITLarray: 28 elements */
		/* [1] */
		{170, 82, 188, 243},
		StaticText {
			disabled,
			"Current Octave offset:"
		},
		/* [2] */
		{75, 88, 93, 248},
		CheckBox {
			enabled,
			"Use Mac Keyboard"
		},
		/* [3] */
		{146, 224, 164, 356},
		CheckBox {
			enabled,
			"Octaves markers"
		},
		/* [4] */
		{119, 273, 137, 329},
		RadioButton {
			enabled,
			"Stop"
		},
		/* [5] */
		{0, 0, 36, 352},
		UserItem {
			disabled
		},
		/* [6] */
		{40, 5, 195, 80},
		UserItem {
			enabled
		},
		/* [7] */
		{119, 183, 137, 257},
		RadioButton {
			enabled,
			"Key OFF"
		},
		/* [8] */
		{40, 82, 72, 311},
		StaticText {
			disabled,
			"Press on your Mac keyboard to change set"
			"tings."
		},
		/* [9] */
		{146, 127, 164, 224},
		CheckBox {
			enabled,
			"Small Piano"
		},
		/* [10] */
		{170, 250, 188, 328},
		StaticText {
			disabled,
			""
		},
		/* [11] */
		{141, 82, 142, 347},
		Picture {
			disabled,
			902
		},
		/* [12] */
		{98, 82, 116, 180},
		StaticText {
			disabled,
			"When key UP:"
		},
		/* [13] */
		{119, 88, 137, 162},
		RadioButton {
			enabled,
			"Nothing"
		},
		/* [14] */
		{258, 87, 276, 219},
		RadioButton {
			enabled,
			"On Current track"
		},
		/* [15] */
		{147, 82, 165, 127},
		StaticText {
			disabled,
			"View:"
		},
		/* [16] */
		{258, 21, 270, 50},
		UserItem {
			disabled
		},
		/* [17] */
		{241, -56, 261, 80},
		UserItem {
			disabled
		},
		/* [18] */
		{254, 30, 270, 76},
		UserItem {
			disabled
		},
		/* [19] */
		{201, 242, 299, 346},
		UserItem {
			enabled
		},
		/* [20] */
		{256, 42, 276, 70},
		UserItem {
			disabled
		},
		/* [21] */
		{198, 5, 234, 85},
		StaticText {
			disabled,
			"Playing & Recording :"
		},
		/* [22] */
		{218, 87, 236, 168},
		RadioButton {
			enabled,
			"On track:"
		},
		/* [23] */
		{221, 188, 234, 208},
		Picture {
			enabled,
			200
		},
		/* [24] */
		{219, 169, 235, 188},
		StaticText {
			disabled,
			"23"
		},
		/* [25] */
		{238, 87, 256, 197},
		RadioButton {
			enabled,
			"On all tracks"
		},
		/* [26] */
		{94, 81, 95, 347},
		Picture {
			disabled,
			902
		},
		/* [27] */
		{195, 80, 196, 347},
		Picture {
			disabled,
			902
		},
		/* [28] */
		{198, 87, 216, 240},
		RadioButton {
			enabled,
			"On following tracks:"
		}
	}
};

resource 'DITL' (145) {
	{	/* array DITLarray: 7 elements */
		/* [1] */
		{0, 26, 100, 2026},
		UserItem {
			enabled
		},
		/* [2] */
		{123, -72, 139, 1428},
		Control {
			enabled,
			136
		},
		/* [3] */
		{60, 25, 92, 2025},
		UserItem {
			enabled
		},
		/* [4] */
		{33, 3, 43, 23},
		UserItem {
			enabled
		},
		/* [5] */
		{1, 3, 21, 23},
		UserItem {
			enabled
		},
		/* [6] */
		{48, 0, 63, 26},
		UserItem {
			disabled
		},
		/* [7] */
		{22, 3, 32, 23},
		UserItem {
			enabled
		}
	}
};

resource 'DITL' (816) {
	{	/* array DITLarray: 12 elements */
		/* [1] */
		{37, 8, 2037, 708},
		UserItem {
			enabled
		},
		/* [2] */
		{24, 8, 36, 36},
		StaticText {
			disabled,
			"Pos"
		},
		/* [3] */
		{8, 216, 28, 248},
		UserItem {
			enabled
		},
		/* [4] */
		{8, 276, 28, 296},
		UserItem {
			enabled
		},
		/* [5] */
		{37, 38, 2037, 50},
		UserItem {
			enabled
		},
		/* [6] */
		{37, -1, 2037, 7},
		UserItem {
			enabled
		},
		/* [7] */
		{2, 56, 22, 76},
		UserItem {
			enabled
		},
		/* [8] */
		{24, 52, 36, 80},
		StaticText {
			disabled,
			"ID"
		},
		/* [9] */
		{24, 80, 35, 172},
		StaticText {
			disabled,
			"Name"
		},
		/* [10] */
		{-7, 79, 25, 111},
		UserItem {
			enabled
		},
		/* [11] */
		{2, 4, 22, 24},
		UserItem {
			enabled
		},
		/* [12] */
		{2, 30, 22, 50},
		UserItem {
			enabled
		}
	}
};

resource 'DITL' (180, purgeable) {
	{	/* array DITLarray: 32 elements */
		/* [1] */
		{71, 37, 86, 62},
		StaticText {
			enabled,
			"Size:"
		},
		/* [2] */
		{33, 218, 65, 250},
		UserItem {
			enabled
		},
		/* [3] */
		{139, 1, 5139, 5001},
		UserItem {
			enabled
		},
		/* [4] */
		{71, 62, 86, 151},
		StaticText {
			enabled,
			""
		},
		/* [5] */
		{56, 18, 71, 62},
		StaticText {
			enabled,
			"Duration:"
		},
		/* [6] */
		{27, 5, 42, 62},
		StaticText {
			enabled,
			"Sound Type:"
		},
		/* [7] */
		{56, 62, 71, 166},
		StaticText {
			enabled,
			""
		},
		/* [8] */
		{27, 62, 42, 214},
		StaticText {
			enabled,
			""
		},
		/* [9] */
		{65, 172, 78, 297},
		UserItem {
			enabled
		},
		/* [10] */
		{42, 2, 57, 62},
		StaticText {
			enabled,
			"Compressor:"
		},
		/* [11] */
		{42, 62, 57, 211},
		StaticText {
			enabled,
			""
		},
		/* [12] */
		{2, 8, 22, 28},
		UserItem {
			enabled
		},
		/* [13] */
		{96, 5, 110, 40},
		StaticText {
			enabled,
			"Levels:"
		},
		/* [14] */
		{105, 131, 120, 166},
		StaticText {
			enabled,
			"Treble:"
		},
		/* [15] */
		{90, 139, 105, 166},
		StaticText {
			enabled,
			"Bass:"
		},
		/* [16] */
		{105, 194, 119, 274},
		UserItem {
			enabled
		},
		/* [17] */
		{90, 194, 104, 274},
		UserItem {
			enabled
		},
		/* [18] */
		{105, 167, 120, 193},
		StaticText {
			enabled,
			""
		},
		/* [19] */
		{90, 167, 105, 193},
		StaticText {
			enabled,
			""
		},
		/* [20] */
		{2, 38, 22, 58},
		UserItem {
			enabled
		},
		/* [21] */
		{121, 276, 136, 296},
		UserItem {
			enabled
		},
		/* [22] */
		{91, 42, 101, 122},
		UserItem {
			enabled
		},
		/* [23] */
		{106, 42, 116, 122},
		UserItem {
			enabled
		},
		/* [24] */
		{105, 276, 120, 296},
		UserItem {
			enabled
		},
		/* [25] */
		{90, 276, 105, 296},
		UserItem {
			enabled
		},
		/* [26] */
		{121, 126, 136, 166},
		StaticText {
			enabled,
			"Balance:"
		},
		/* [27] */
		{121, 194, 137, 274},
		UserItem {
			enabled
		},
		/* [28] */
		{119, 2, 134, 40},
		StaticText {
			enabled,
			"Volume:"
		},
		/* [29] */
		{121, 42, 131, 122},
		UserItem {
			enabled
		},
		/* [30] */
		{121, 167, 136, 193},
		StaticText {
			enabled,
			""
		},
		/* [31] */
		{24, -43, 25, 3957},
		Picture {
			disabled,
			137
		},
		/* [32] */
		{86, -48, 87, 3952},
		Picture {
			disabled,
			137
		}
	}
};

resource 'DITL' (872) {
	{	/* array DITLarray: 2 elements */
		/* [1] */
		{337, 191, 358, 421},
		UserItem {
			enabled
		},
		/* [2] */
		{0, 0, 427, 473},
		Picture {
			disabled,
			132
		}
	}
};

resource 'DITL' (181, purgeable) {
	{	/* array DITLarray: 12 elements */
		/* [1] */
		{11, 340, 31, 408},
		Button {
			enabled,
			"OK"
		},
		/* [2] */
		{45, 342, 65, 406},
		Button {
			enabled,
			"Cancel"
		},
		/* [3] */
		{91, 342, 111, 406},
		Button {
			enabled,
			"Add"
		},
		/* [4] */
		{126, 342, 146, 406},
		Button {
			enabled,
			"Remove"
		},
		/* [5] */
		{223, 69, 236, 327},
		EditText {
			disabled,
			"Description:"
		},
		/* [6] */
		{245, 69, 258, 327},
		EditText {
			disabled,
			"TEXT"
		},
		/* [7] */
		{245, 40, 258, 63},
		StaticText {
			disabled,
			"URL:"
		},
		/* [8] */
		{223, 6, 236, 63},
		StaticText {
			disabled,
			"Description:"
		},
		/* [9] */
		{1, 3, 210, 330},
		UserItem {
			enabled
		},
		/* [10] */
		{229, 340, 249, 404},
		Button {
			enabled,
			"Change"
		},
		/* [11] */
		{268, 6, 341, 407},
		UserItem {
			enabled
		},
		/* [12] */
		{161, 342, 181, 406},
		Button {
			enabled,
			"Reset"
		}
	}
};

resource 'DITL' (2004) {
	{	/* array DITLarray: 19 elements */
		/* [1] */
		{60, 19, 78, 346},
		CheckBox {
			enabled,
			"Automatic MAD1 compression for patterns"
		},
		/* [2] */
		{222, 199, 235, 219},
		Picture {
			enabled,
			200
		},
		/* [3] */
		{40, 8, 56, 262},
		StaticText {
			disabled,
			"Automatic compression when saving:"
		},
		/* [4] */
		{82, 0, 83, 352},
		Picture {
			disabled,
			902
		},
		/* [5] */
		{151, 8, 167, 335},
		StaticText {
			enabled,
			"Oscilloscope:"
		},
		/* [6] */
		{171, 19, 189, 346},
		CheckBox {
			enabled,
			"Draw lines between points"
		},
		/* [7] */
		{148, 0, 149, 352},
		Picture {
			disabled,
			902
		},
		/* [8] */
		{199, 8, 217, 348},
		StaticText {
			enabled,
			"Patterns List Window and Partition Windo"
			"w:"
		},
		/* [9] */
		{159, 293, 183, 345},
		UserItem {
			disabled
		},
		/* [10] */
		{105, 19, 123, 349},
		CheckBox {
			enabled,
			"Add extension to file names (example : \""
			".MOD\")"
		},
		/* [11] */
		{166, 308, 180, 327},
		UserItem {
			disabled
		},
		/* [12] */
		{193, 0, 194, 352},
		Picture {
			disabled,
			902
		},
		/* [13] */
		{221, 19, 237, 196},
		StaticText {
			disabled,
			"Use this editor when open:"
		},
		/* [14] */
		{221, 223, 237, 351},
		StaticText {
			enabled,
			""
		},
		/* [15] */
		{85, 8, 101, 262},
		StaticText {
			disabled,
			"Files:"
		},
		/* [16] */
		{264, 19, 282, 346},
		CheckBox {
			enabled,
			"Play a sound when click in & out"
		},
		/* [17] */
		{244, 8, 260, 335},
		StaticText {
			enabled,
			"Window Buttons:"
		},
		/* [18] */
		{241, 0, 242, 352},
		Picture {
			disabled,
			902
		},
		/* [19] */
		{123, 19, 141, 349},
		CheckBox {
			enabled,
			"Don’t load Mixer settings from music fil"
			"es"
		}
	}
};

resource 'DITL' (182, purgeable) {
	{	/* array DITLarray: 6 elements */
		/* [1] */
		{8, 135, 28, 203},
		Button {
			enabled,
			"OK"
		},
		/* [2] */
		{45, 137, 65, 201},
		Button {
			enabled,
			"Cancel"
		},
		/* [3] */
		{45, 5, 61, 50},
		StaticText {
			disabled,
			"Speed:"
		},
		/* [4] */
		{10, 11, 26, 50},
		StaticText {
			disabled,
			"Pitch:"
		},
		/* [5] */
		{10, 60, 26, 110},
		EditText {
			enabled,
			"16000"
		},
		/* [6] */
		{45, 60, 61, 110},
		EditText {
			enabled,
			"26000"
		}
	}
};

resource 'DITL' (421) {
	{	/* array DITLarray: 6 elements */
		/* [1] */
		{261, 348, 281, 448},
		Button {
			enabled,
			"Continue"
		},
		/* [2] */
		{1, 2, 238, 296},
		UserItem {
			disabled
		},
		/* [3] */
		{1, 313, 237, 483},
		Picture {
			disabled,
			130
		},
		/* [4] */
		{115, 298, 124, 309},
		UserItem {
			enabled
		},
		/* [5] */
		{244, 98, 264, 198},
		Button {
			enabled,
			"How to Order"
		},
		/* [6] */
		{271, 8, 291, 302},
		Button {
			enabled,
			"Go to PlayerPRO Official Web Site"
		}
	}
};

resource 'DITL' (684) {
	{	/* array DITLarray: 32 elements */
		/* [1] */
		{25, 12, 2025, 200},
		UserItem {
			enabled
		},
		/* [2] */
		{27, 207, 40, 262},
		StaticText {
			disabled,
			"Instrument:"
		},
		/* [3] */
		{57, 207, 70, 257},
		StaticText {
			disabled,
			"Size:"
		},
		/* [4] */
		{81, 218, 94, 258},
		StaticText {
			disabled,
			"Start:"
		},
		/* [5] */
		{93, 218, 106, 259},
		StaticText {
			disabled,
			"Size:"
		},
		/* [6] */
		{105, 207, 118, 257},
		StaticText {
			disabled,
			"Volume:"
		},
		/* [7] */
		{117, 207, 130, 273},
		StaticText {
			disabled,
			"Rate:"
		},
		/* [8] */
		{27, 264, 40, 339},
		StaticText {
			disabled,
			""
		},
		/* [9] */
		{57, 264, 70, 339},
		StaticText {
			disabled,
			""
		},
		/* [10] */
		{81, 264, 94, 339},
		StaticText {
			disabled,
			""
		},
		/* [11] */
		{93, 264, 106, 339},
		StaticText {
			disabled,
			""
		},
		/* [12] */
		{105, 264, 118, 339},
		StaticText {
			disabled,
			""
		},
		/* [13] */
		{117, 264, 130, 339},
		StaticText {
			disabled,
			""
		},
		/* [14] */
		{129, 207, 142, 261},
		StaticText {
			disabled,
			"Real Note:"
		},
		/* [15] */
		{129, 264, 142, 339},
		StaticText {
			disabled,
			""
		},
		/* [16] */
		{141, 207, 154, 249},
		StaticText {
			disabled,
			"Bits:"
		},
		/* [17] */
		{141, 264, 154, 339},
		StaticText {
			disabled,
			""
		},
		/* [18] */
		{2, 159, 22, 179},
		UserItem {
			enabled
		},
		/* [19] */
		{2, 69, 22, 89},
		UserItem {
			enabled
		},
		/* [20] */
		{2, 9, 22, 29},
		UserItem {
			enabled
		},
		/* [21] */
		{2, 39, 22, 59},
		UserItem {
			enabled
		},
		/* [22] */
		{2, 99, 22, 119},
		UserItem {
			enabled
		},
		/* [23] */
		{2, 129, 22, 149},
		UserItem {
			enabled
		},
		/* [24] */
		{-7, 185, 25, 217},
		UserItem {
			enabled
		},
		/* [25] */
		{25, 0, 725, 11},
		UserItem {
			enabled
		},
		/* [26] */
		{69, 207, 82, 249},
		StaticText {
			disabled,
			"Loop:"
		},
		/* [27] */
		{54, 201, 55, 553},
		Picture {
			disabled,
			11
		},
		/* [28] */
		{39, 207, 52, 264},
		StaticText {
			disabled,
			"No samples:"
		},
		/* [29] */
		{39, 264, 52, 339},
		StaticText {
			disabled,
			""
		},
		/* [30] */
		{170, 202, 202, 304},
		UserItem {
			disabled
		},
		/* [31] */
		{153, 207, 166, 249},
		StaticText {
			disabled,
			"Mode:"
		},
		/* [32] */
		{153, 264, 166, 339},
		StaticText {
			disabled,
			""
		}
	}
};

resource 'DITL' (202) {
	{	/* array DITLarray: 40 elements */
		/* [1] */
		{56, 0, 2056, 2000},
		UserItem {
			enabled
		},
		/* [2] */
		{14, 95, 26, 127},
		StaticText {
			disabled,
			"Size:"
		},
		/* [3] */
		{14, 128, 26, 198},
		StaticText {
			disabled,
			""
		},
		/* [4] */
		{1, 169, 13, 232},
		StaticText {
			disabled,
			"Selection:"
		},
		/* [5] */
		{1, 233, 13, 283},
		StaticText {
			disabled,
			""
		},
		/* [6] */
		{1, 284, 13, 304},
		StaticText {
			disabled,
			"to:"
		},
		/* [7] */
		{1, 305, 13, 355},
		StaticText {
			disabled,
			""
		},
		/* [8] */
		{14, 199, 26, 232},
		StaticText {
			disabled,
			"Loop:"
		},
		/* [9] */
		{14, 233, 26, 283},
		StaticText {
			disabled,
			""
		},
		/* [10] */
		{14, 284, 26, 304},
		StaticText {
			disabled,
			"to:"
		},
		/* [11] */
		{14, 305, 26, 355},
		StaticText {
			disabled,
			""
		},
		/* [12] */
		{2, 3, 14, 71},
		StaticText {
			enabled,
			"Do Filters:"
		},
		/* [13] */
		{41, 81, 54, 101},
		UserItem {
			enabled
		},
		/* [14] */
		{2, 72, 15, 92},
		Picture {
			enabled,
			900
		},
		/* [15] */
		{27, 101, 39, 126},
		StaticText {
			disabled,
			"Amp:"
		},
		/* [16] */
		{42, 26, 54, 77},
		StaticText {
			disabled,
			"Sample:"
		},
		/* [17] */
		{27, 128, 39, 178},
		StaticText {
			disabled,
			""
		},
		/* [18] */
		{17, 72, 37, 92},
		UserItem {
			enabled
		},
		/* [19] */
		{27, 181, 39, 232},
		StaticText {
			disabled,
			"Display:"
		},
		/* [20] */
		{27, 233, 39, 283},
		StaticText {
			disabled,
			""
		},
		/* [21] */
		{27, 284, 39, 304},
		StaticText {
			disabled,
			"to:"
		},
		/* [22] */
		{27, 305, 39, 355},
		StaticText {
			disabled,
			""
		},
		/* [23] */
		{1, 95, 13, 127},
		StaticText {
			disabled,
			"Zoom:"
		},
		/* [24] */
		{1, 128, 13, 168},
		StaticText {
			disabled,
			""
		},
		/* [25] */
		{17, 3, 37, 23},
		UserItem {
			enabled
		},
		/* [26] */
		{17, 52, 37, 72},
		UserItem {
			enabled
		},
		/* [27] */
		{17, 32, 37, 52},
		UserItem {
			enabled
		},
		/* [28] */
		{41, 3, 54, 23},
		Picture {
			enabled,
			900
		},
		/* [29] */
		{39, -3, 40, 1997},
		Picture {
			disabled,
			11
		},
		/* [30] */
		{42, 174, 55, 8174},
		UserItem {
			disabled
		},
		/* [31] */
		{1, 357, 13, 438},
		StaticText {
			disabled,
			"Rate (c4spd):"
		},
		/* [32] */
		{1, 439, 13, 489},
		StaticText {
			disabled,
			""
		},
		/* [33] */
		{14, 439, 26, 494},
		StaticText {
			disabled,
			""
		},
		/* [34] */
		{14, 375, 26, 438},
		StaticText {
			disabled,
			"Real Note:"
		},
		/* [35] */
		{153, 726, 185, 758},
		UserItem {
			disabled
		},
		/* [36] */
		{41, 104, 54, 124},
		UserItem {
			enabled
		},
		/* [37] */
		{41, 127, 54, 147},
		UserItem {
			enabled
		},
		/* [38] */
		{27, 439, 39, 549},
		StaticText {
			disabled,
			""
		},
		/* [39] */
		{27, 357, 39, 438},
		StaticText {
			disabled,
			"X/Y Position:"
		},
		/* [40] */
		{41, 150, 54, 170},
		UserItem {
			enabled
		}
	}
};

resource 'DITL' (148) {
	{	/* array DITLarray: 22 elements */
		/* [1] */
		{5, 280, 25, 338},
		Button {
			enabled,
			"OK"
		},
		/* [2] */
		{35, 280, 55, 338},
		Button {
			enabled,
			"Cancel"
		},
		/* [3] */
		{20, 90, 33, 269},
		EditText {
			enabled,
			""
		},
		/* [4] */
		{20, 14, 33, 81},
		StaticText {
			disabled,
			"Internal name:"
		},
		/* [5] */
		{40, 13, 53, 81},
		StaticText {
			disabled,
			"Default tempo:"
		},
		/* [6] */
		{60, 15, 73, 81},
		StaticText {
			disabled,
			"Default speed:"
		},
		/* [7] */
		{40, 90, 53, 150},
		EditText {
			enabled,
			""
		},
		/* [8] */
		{60, 90, 73, 150},
		EditText {
			enabled,
			""
		},
		/* [9] */
		{80, 7, 96, 81},
		StaticText {
			disabled,
			"Copyright Text:"
		},
		/* [10] */
		{80, 90, 132, 269},
		EditText {
			enabled,
			""
		},
		/* [11] */
		{40, 160, 56, 235},
		StaticText {
			disabled,
			"BPM"
		},
		/* [12] */
		{60, 160, 76, 270},
		StaticText {
			disabled,
			"Timing pulses per line"
		},
		/* [13] */
		{0, 44, 13, 81},
		StaticText {
			disabled,
			"Tracks:"
		},
		/* [14] */
		{0, 90, 13, 122},
		StaticText {
			disabled,
			""
		},
		/* [15] */
		{139, 90, 152, 334},
		CheckBox {
			enabled,
			"Show Copyright when opening"
		},
		/* [16] */
		{0, 127, 13, 147},
		Picture {
			enabled,
			901
		},
		/* [17] */
		{221, 107, 237, 255},
		StaticText {
			disabled,
			"Max mixing channels:"
		},
		/* [18] */
		{160, 90, 173, 255},
		CheckBox {
			enabled,
			"Old MODs pitch table"
		},
		/* [19] */
		{181, 90, 194, 334},
		CheckBox {
			enabled,
			"Linear Table"
		},
		/* [20] */
		{202, 90, 215, 285},
		CheckBox {
			enabled,
			"Multi-Channel Tracks"
		},
		/* [21] */
		{223, 292, 236, 312},
		Picture {
			enabled,
			901
		},
		/* [22] */
		{223, 255, 236, 287},
		StaticText {
			disabled,
			""
		}
	}
};

resource 'DITL' (183, purgeable) {
	{	/* array DITLarray: 41 elements */
		/* [1] */
		{3, 125, 18, 164},
		UserItem {
			enabled
		},
		/* [2] */
		{28, 48, 44, 178},
		UserItem {
			enabled
		},
		/* [3] */
		{48, 48, 64, 178},
		UserItem {
			enabled
		},
		/* [4] */
		{68, 48, 84, 178},
		UserItem {
			enabled
		},
		/* [5] */
		{88, 48, 104, 178},
		UserItem {
			enabled
		},
		/* [6] */
		{108, 48, 124, 178},
		UserItem {
			enabled
		},
		/* [7] */
		{128, 48, 144, 178},
		UserItem {
			enabled
		},
		/* [8] */
		{148, 48, 164, 178},
		UserItem {
			enabled
		},
		/* [9] */
		{168, 48, 184, 178},
		UserItem {
			enabled
		},
		/* [10] */
		{206, 11, 462, 267},
		UserItem {
			enabled
		},
		/* [11] */
		{363, 350, 379, 480},
		UserItem {
			enabled
		},
		/* [12] */
		{383, 350, 399, 480},
		UserItem {
			enabled
		},
		/* [13] */
		{403, 350, 419, 480},
		UserItem {
			enabled
		},
		/* [14] */
		{2, 37, 22, 57},
		UserItem {
			enabled
		},
		/* [15] */
		{2, 67, 22, 87},
		UserItem {
			enabled
		},
		/* [16] */
		{2, 97, 22, 117},
		UserItem {
			enabled
		},
		/* [17] */
		{2, 7, 22, 27},
		UserItem {
			enabled
		},
		/* [18] */
		{28, 3, 44, 47},
		StaticText {
			disabled,
			"Static Text"
		},
		/* [19] */
		{48, 3, 64, 47},
		StaticText {
			disabled,
			"Static Text"
		},
		/* [20] */
		{68, 3, 84, 47},
		StaticText {
			disabled,
			"Static Text"
		},
		/* [21] */
		{88, 3, 104, 47},
		StaticText {
			disabled,
			"Static Text"
		},
		/* [22] */
		{108, 3, 124, 47},
		StaticText {
			disabled,
			"Static Text"
		},
		/* [23] */
		{128, 3, 144, 47},
		StaticText {
			disabled,
			"Static Text"
		},
		/* [24] */
		{148, 3, 164, 47},
		StaticText {
			disabled,
			"Static Text"
		},
		/* [25] */
		{168, 3, 184, 47},
		StaticText {
			disabled,
			"Static Text"
		},
		/* [26] */
		{343, 305, 359, 349},
		StaticText {
			disabled,
			"Static Text"
		},
		/* [27] */
		{363, 305, 379, 349},
		StaticText {
			disabled,
			"Static Text"
		},
		/* [28] */
		{383, 305, 399, 349},
		StaticText {
			disabled,
			"Static Text"
		},
		/* [29] */
		{403, 305, 419, 349},
		StaticText {
			disabled,
			"Static Text"
		},
		/* [30] */
		{28, 180, 44, 213},
		StaticText {
			disabled,
			"Static Text"
		},
		/* [31] */
		{48, 180, 64, 213},
		StaticText {
			disabled,
			"Static Text"
		},
		/* [32] */
		{68, 180, 84, 213},
		StaticText {
			disabled,
			"Static Text"
		},
		/* [33] */
		{88, 180, 104, 213},
		StaticText {
			disabled,
			"Static Text"
		},
		/* [34] */
		{108, 180, 124, 213},
		StaticText {
			disabled,
			"Static Text"
		},
		/* [35] */
		{128, 180, 144, 213},
		StaticText {
			disabled,
			"Static Text"
		},
		/* [36] */
		{148, 180, 164, 213},
		StaticText {
			disabled,
			"Static Text"
		},
		/* [37] */
		{168, 180, 184, 213},
		StaticText {
			disabled,
			"Static Text"
		},
		/* [38] */
		{343, 482, 359, 515},
		StaticText {
			disabled,
			"Static Text"
		},
		/* [39] */
		{363, 482, 379, 515},
		StaticText {
			disabled,
			"Static Text"
		},
		/* [40] */
		{383, 482, 399, 515},
		StaticText {
			disabled,
			"Static Text"
		},
		/* [41] */
		{403, 482, 419, 515},
		StaticText {
			disabled,
			"Static Text"
		}
	}
};

resource 'DITL' (141, "Adaptator") {
	{	/* array DITLarray: 44 elements */
		/* [1] */
		{113, 484, 125, 538},
		UserItem {
			disabled
		},
		/* [2] */
		{104, 45, 120, 109},
		UserItem {
			enabled
		},
		/* [3] */
		{19, 458, 35, 505},
		UserItem {
			disabled
		},
		/* [4] */
		{102, 414, 118, 454},
		StaticText {
			enabled,
			""
		},
		/* [5] */
		{43, 458, 59, 505},
		UserItem {
			disabled
		},
		/* [6] */
		{119, 334, 132, 379},
		UserItem {
			disabled
		},
		/* [7] */
		{77, 392, 93, 427},
		StaticText {
			disabled,
			"BPM:"
		},
		/* [8] */
		{45, 210, 61, 270},
		UserItem {
			enabled
		},
		/* [9] */
		{91, 458, 107, 505},
		UserItem {
			disabled
		},
		/* [10] */
		{27, 210, 43, 270},
		UserItem {
			enabled
		},
		/* [11] */
		{207, 414, 223, 461},
		UserItem {
			disabled
		},
		/* [12] */
		{27, 26, 43, 156},
		UserItem {
			enabled
		},
		/* [13] */
		{112, 413, 128, 429},
		Picture {
			disabled,
			129
		},
		/* [14] */
		{45, 26, 61, 156},
		UserItem {
			enabled
		},
		/* [15] */
		{171, 414, 187, 461},
		UserItem {
			disabled
		},
		/* [16] */
		{102, 1, 4102, 301},
		UserItem {
			enabled
		},
		/* [17] */
		{2, 37, 22, 57},
		UserItem {
			enabled
		},
		/* [18] */
		{59, 548, 75, 580},
		UserItem {
			enabled
		},
		/* [19] */
		{66, 333, 118, 365},
		UserItem {
			disabled
		},
		/* [20] */
		{44, 193, 60, 209},
		Picture {
			disabled,
			129
		},
		/* [21] */
		{211, 359, 225, 373},
		UserItem {
			disabled
		},
		/* [22] */
		{46, 157, 66, 177},
		UserItem {
			enabled
		},
		/* [23] */
		{28, 157, 48, 177},
		UserItem {
			enabled
		},
		/* [24] */
		{168, 377, 184, 477},
		Control {
			enabled,
			152
		},
		/* [25] */
		{2, 7, 22, 27},
		UserItem {
			enabled
		},
		/* [26] */
		{2, 67, 22, 87},
		UserItem {
			enabled
		},
		/* [27] */
		{14, 372, 26, 416},
		UserItem {
			disabled
		},
		/* [28] */
		{26, 193, 42, 209},
		Picture {
			disabled,
			129
		},
		/* [29] */
		{27, 5, 43, 21},
		Picture {
			disabled,
			138
		},
		/* [30] */
		{43, 5, 59, 21},
		Picture {
			disabled,
			139
		},
		/* [31] */
		{12, 416, 28, 432},
		UserItem {
			disabled
		},
		/* [32] */
		{12, 355, 28, 371},
		UserItem {
			disabled
		},
		/* [33] */
		{41, 553, 53, 597},
		StaticText {
			disabled,
			""
		},
		/* [34] */
		{86, 55, 98, 96},
		StaticText {
			disabled,
			"Volume"
		},
		/* [35] */
		{86, 134, 98, 178},
		StaticText {
			disabled,
			"Panning"
		},
		/* [36] */
		{85, -36, 86, 364},
		Picture {
			disabled,
			137
		},
		/* [37] */
		{28, 168, 39, 192},
		Picture {
			disabled,
			134
		},
		/* [38] */
		{86, 1, 98, 47},
		StaticText {
			disabled,
			"Track ID"
		},
		/* [39] */
		{46, 168, 57, 192},
		Picture {
			disabled,
			153
		},
		/* [40] */
		{4, 206, 20, 258},
		StaticText {
			disabled,
			""
		},
		/* [41] */
		{2, 95, 22, 115},
		UserItem {
			enabled
		},
		/* [42] */
		{65, 2, 75, 23},
		Picture {
			disabled,
			157
		},
		/* [43] */
		{63, 26, 79, 156},
		UserItem {
			enabled
		},
		/* [44] */
		{64, 157, 84, 177},
		UserItem {
			enabled
		}
	}
};

resource 'DITL' (184, purgeable) {
	{	/* array DITLarray: 9 elements */
		/* [1] */
		{7, 6, 27, 74},
		Button {
			enabled,
			"OK"
		},
		/* [2] */
		{42, 8, 62, 72},
		Button {
			enabled,
			"Cancel"
		},
		/* [3] */
		{4, 89, 260, 345},
		UserItem {
			enabled
		},
		/* [4] */
		{129, 8, 149, 72},
		Button {
			enabled,
			"Play"
		},
		/* [5] */
		{85, 8, 105, 72},
		Button {
			enabled,
			"Reset"
		},
		/* [6] */
		{244, 6, 260, 82},
		CheckBox {
			enabled,
			"Preview"
		},
		/* [7] */
		{199, 6, 215, 82},
		RadioButton {
			enabled,
			"Linear"
		},
		/* [8] */
		{215, 6, 231, 82},
		RadioButton {
			enabled,
			"Log"
		},
		/* [9] */
		{183, 6, 198, 52},
		StaticText {
			disabled,
			"Mode:"
		}
	}
};

resource 'DITL' (146) {
	{	/* array DITLarray: 61 elements */
		/* [1] */
		{10, 324, 30, 382},
		Button {
			enabled,
			"OK"
		},
		/* [2] */
		{10, 390, 30, 448},
		Button {
			enabled,
			"Cancel"
		},
		/* [3] */
		{20, 56, 33, 246},
		EditText {
			enabled,
			"ijij\n"
		},
		/* [4] */
		{186, 244, 199, 284},
		EditText {
			enabled,
			""
		},
		/* [5] */
		{187, 354, 200, 394},
		EditText {
			enabled,
			""
		},
		/* [6] */
		{205, 83, 218, 123},
		EditText {
			enabled,
			""
		},
		/* [7] */
		{225, 83, 238, 123},
		EditText {
			enabled,
			""
		},
		/* [8] */
		{20, 19, 33, 53},
		StaticText {
			disabled,
			"Name:"
		},
		/* [9] */
		{186, 184, 199, 241},
		StaticText {
			disabled,
			"Loop Start:"
		},
		/* [10] */
		{187, 299, 200, 351},
		StaticText {
			disabled,
			"Loop Size:"
		},
		/* [11] */
		{205, 13, 218, 80},
		StaticText {
			disabled,
			"Rate (c4spd):"
		},
		/* [12] */
		{225, 14, 238, 80},
		StaticText {
			disabled,
			"Base Volume:"
		},
		/* [13] */
		{144, 6, 157, 452},
		UserItem {
			disabled
		},
		/* [14] */
		{316, 415, 329, 452},
		UserItem {
			disabled
		},
		/* [15] */
		{105, 56, 118, 331},
		RadioButton {
			enabled,
			"Channel Value (See Mixer window)"
		},
		/* [16] */
		{46, 11, 59, 53},
		StaticText {
			enabled,
			"Volume:"
		},
		/* [17] */
		{225, 142, 238, 163},
		UserItem {
			disabled
		},
		/* [18] */
		{225, 244, 238, 270},
		EditText {
			enabled,
			""
		},
		/* [19] */
		{225, 188, 238, 241},
		StaticText {
			disabled,
			"Real Note:"
		},
		/* [20] */
		{185, 25, 198, 80},
		StaticText {
			disabled,
			"Amplitude:"
		},
		/* [21] */
		{185, 83, 198, 103},
		StaticText {
			disabled,
			"16"
		},
		/* [22] */
		{82, 288, 95, 385},
		CheckBox {
			enabled,
			"Fixed Speed"
		},
		/* [23] */
		{248, 0, 249, 600},
		Picture {
			disabled,
			11
		},
		/* [24] */
		{185, 142, 198, 162},
		Picture {
			enabled,
			901
		},
		/* [25] */
		{2, 56, 15, 246},
		UserItem {
			disabled
		},
		/* [26] */
		{317, 3, 330, 348},
		StaticText {
			disabled,
			"Sample settings: click + option key to c"
			"hange assignation of current sample"
		},
		/* [27] */
		{225, 275, 238, 323},
		StaticText {
			disabled,
			""
		},
		/* [28] */
		{185, 108, 198, 131},
		StaticText {
			disabled,
			"bits"
		},
		/* [29] */
		{255, 3, 313, 452},
		Picture {
			enabled,
			225
		},
		/* [30] */
		{351, 259, 364, 276},
		StaticText {
			disabled,
			"99"
		},
		/* [31] */
		{139, -12, 140, 588},
		Picture {
			disabled,
			11
		},
		/* [32] */
		{46, 187, 59, 221},
		EditText {
			enabled,
			""
		},
		/* [33] */
		{82, 56, 95, 142},
		RadioButton {
			enabled,
			"Envelope:"
		},
		/* [34] */
		{205, 128, 218, 141},
		StaticText {
			enabled,
			"Hz"
		},
		/* [35] */
		{64, 56, 77, 347},
		RadioButton {
			enabled,
			"Sample Base Volume (See below)"
		},
		/* [36] */
		{82, 198, 95, 284},
		CheckBox {
			enabled,
			"Sustain pt"
		},
		/* [37] */
		{205, 142, 218, 162},
		Picture {
			enabled,
			901
		},
		/* [38] */
		{161, 406, 177, 432},
		UserItem {
			enabled
		},
		/* [39] */
		{82, 142, 95, 198},
		CheckBox {
			enabled,
			"Loop"
		},
		/* [40] */
		{123, 142, 136, 198},
		CheckBox {
			enabled,
			"Loop"
		},
		/* [41] */
		{124, 288, 137, 387},
		CheckBox {
			enabled,
			"Fixed Speed"
		},
		/* [42] */
		{104, 9, 117, 53},
		StaticText {
			disabled,
			"Panning:"
		},
		/* [43] */
		{123, 198, 136, 288},
		CheckBox {
			enabled,
			"Sustain pt"
		},
		/* [44] */
		{158, 436, 246, 452},
		UserItem {
			enabled
		},
		/* [45] */
		{166, 244, 179, 394},
		EditText {
			enabled,
			""
		},
		/* [46] */
		{166, 207, 179, 241},
		StaticText {
			disabled,
			"Name:"
		},
		/* [47] */
		{46, 57, 59, 184},
		StaticText {
			enabled,
			"Volume FadeOut (Key OFF):"
		},
		/* [48] */
		{123, 56, 136, 141},
		RadioButton {
			enabled,
			"Envelope:"
		},
		/* [49] */
		{40, -20, 41, 580},
		Picture {
			disabled,
			11
		},
		/* [50] */
		{98, -17, 99, 583},
		Picture {
			disabled,
			11
		},
		/* [51] */
		{166, 47, 179, 80},
		StaticText {
			disabled,
			"Mode:"
		},
		/* [52] */
		{166, 83, 179, 126},
		StaticText {
			disabled,
			""
		},
		/* [53] */
		{166, 142, 179, 162},
		Picture {
			enabled,
			901
		},
		/* [54] */
		{205, 186, 218, 241},
		StaticText {
			disabled,
			"Loop Type:"
		},
		/* [55] */
		{205, 244, 218, 293},
		StaticText {
			disabled,
			""
		},
		/* [56] */
		{205, 296, 218, 316},
		Picture {
			enabled,
			901
		},
		/* [57] */
		{387, 152, 400, 172},
		StaticText {
			disabled,
			"bits"
		},
		/* [58] */
		{48, 388, 68, 450},
		Button {
			enabled,
			"Previous"
		},
		/* [59] */
		{74, 388, 94, 450},
		Button {
			enabled,
			"Next"
		},
		/* [60] */
		{80, 10, 96, 50},
		Button {
			enabled,
			"Edit"
		},
		/* [61] */
		{121, 10, 137, 50},
		Button {
			enabled,
			"Edit"
		}
	}
};

resource 'DITL' (143, "Controls") {
	{	/* array DITLarray: 45 elements */
		/* [1] */
		{36, 1, 47, 181},
		UserItem {
			enabled
		},
		/* [2] */
		{1, 7, 25, 31},
		UserItem {
			enabled
		},
		/* [3] */
		{1, 32, 25, 56},
		UserItem {
			enabled
		},
		/* [4] */
		{1, 82, 25, 106},
		UserItem {
			enabled
		},
		/* [5] */
		{1, 107, 25, 131},
		UserItem {
			enabled
		},
		/* [6] */
		{1, 132, 25, 156},
		UserItem {
			enabled
		},
		/* [7] */
		{1, 157, 25, 181},
		UserItem {
			enabled
		},
		/* [8] */
		{23, 2, 35, 35},
		StaticText {
			disabled,
			"00:00"
		},
		/* [9] */
		{23, 147, 35, 181},
		StaticText {
			disabled,
			"00:00"
		},
		/* [10] */
		{22, 36, 36, 145},
		UserItem {
			enabled
		},
		/* [11] */
		{42, 99, 58, 115},
		UserItem {
			enabled
		},
		/* [12] */
		{50, 54, 62, 79},
		StaticText {
			disabled,
			"Pos:"
		},
		/* [13] */
		{50, 1, 62, 26},
		StaticText {
			disabled,
			"Pat:"
		},
		/* [14] */
		{50, 116, 62, 153},
		StaticText {
			disabled,
			"Track:"
		},
		/* [15] */
		{50, 26, 62, 41},
		StaticText {
			disabled,
			"99"
		},
		/* [16] */
		{50, 79, 62, 99},
		StaticText {
			disabled,
			"642"
		},
		/* [17] */
		{50, 153, 62, 168},
		StaticText {
			disabled,
			"99"
		},
		/* [18] */
		{66, 159, 79, 179},
		Picture {
			enabled,
			901
		},
		/* [19] */
		{84, 159, 97, 179},
		Picture {
			enabled,
			901
		},
		/* [20] */
		{121, 70, 137, 125},
		StaticText {
			disabled,
			"Argument:"
		},
		/* [21] */
		{87, 6, 107, 26},
		UserItem {
			enabled
		},
		/* [22] */
		{64, 6, 84, 26},
		UserItem {
			enabled
		},
		/* [23] */
		{52, 168, 60, 179},
		Picture {
			enabled,
			140
		},
		/* [24] */
		{52, 42, 60, 52},
		Picture {
			enabled,
			140
		},
		/* [25] */
		{55, 99, 71, 115},
		UserItem {
			enabled
		},
		/* [26] */
		{120, 159, 133, 179},
		Picture {
			enabled,
			901
		},
		/* [27] */
		{62, -1, 63, 183},
		Picture {
			disabled,
			11
		},
		/* [28] */
		{85, 94, 101, 125},
		StaticText {
			disabled,
			"Note:"
		},
		/* [29] */
		{67, 58, 83, 125},
		StaticText {
			disabled,
			"Instrument:"
		},
		/* [30] */
		{103, 82, 119, 125},
		StaticText {
			disabled,
			"Effect:"
		},
		/* [31] */
		{67, 127, 79, 156},
		UserItem {
			enabled
		},
		/* [32] */
		{85, 127, 97, 156},
		UserItem {
			enabled
		},
		/* [33] */
		{103, 127, 115, 156},
		UserItem {
			enabled
		},
		/* [34] */
		{121, 127, 133, 156},
		UserItem {
			enabled
		},
		/* [35] */
		{103, 70, 116, 80},
		UserItem {
			enabled
		},
		/* [36] */
		{-2, 183, 198, 184},
		Picture {
			disabled,
			11
		},
		/* [37] */
		{1, 186, 135, 450},
		UserItem {
			enabled
		},
		/* [38] */
		{48, -1, 49, 183},
		Picture {
			disabled,
			11
		},
		/* [39] */
		{139, 82, 155, 125},
		StaticText {
			disabled,
			"Volume:"
		},
		/* [40] */
		{110, 6, 130, 26},
		UserItem {
			enabled
		},
		/* [41] */
		{102, 159, 115, 179},
		Picture {
			enabled,
			901
		},
		/* [42] */
		{139, 127, 151, 156},
		UserItem {
			enabled
		},
		/* [43] */
		{138, 159, 151, 179},
		Picture {
			enabled,
			901
		},
		/* [44] */
		{1, 57, 25, 81},
		UserItem {
			enabled
		},
		/* [45] */
		{84, 80, 97, 90},
		UserItem {
			enabled
		}
	}
};

resource 'DITL' (142, "Music list") {
	{	/* array DITLarray: 33 elements */
		/* [1] */
		{49, 623, 65, 698},
		UserItem {
			disabled
		},
		/* [2] */
		{25, 0, 2025, 200},
		UserItem {
			enabled
		},
		/* [3] */
		{21, 591, 41, 649},
		UserItem {
			enabled
		},
		/* [4] */
		{32, 574, 48, 649},
		UserItem {
			disabled
		},
		/* [5] */
		{29, 206, 42, 267},
		StaticText {
			disabled,
			"File Name:"
		},
		/* [6] */
		{44, 206, 57, 275},
		StaticText {
			disabled,
			"Internal Name:"
		},
		/* [7] */
		{104, 206, 117, 266},
		StaticText {
			disabled,
			"Patterns:"
		},
		/* [8] */
		{74, 206, 87, 268},
		StaticText {
			disabled,
			"Signature:"
		},
		/* [9] */
		{119, 205, 132, 264},
		StaticText {
			disabled,
			"Plug Type:"
		},
		/* [10] */
		{89, 206, 102, 269},
		StaticText {
			disabled,
			"Instruments:"
		},
		/* [11] */
		{59, 206, 72, 263},
		StaticText {
			disabled,
			"Size:"
		},
		/* [12] */
		{29, 276, 42, 415},
		StaticText {
			disabled,
			"-"
		},
		/* [13] */
		{44, 276, 57, 415},
		StaticText {
			enabled,
			"-"
		},
		/* [14] */
		{104, 276, 117, 409},
		StaticText {
			disabled,
			"-"
		},
		/* [15] */
		{74, 276, 87, 406},
		StaticText {
			disabled,
			"-"
		},
		/* [16] */
		{119, 276, 132, 410},
		StaticText {
			disabled,
			"-"
		},
		/* [17] */
		{89, 276, 102, 407},
		StaticText {
			disabled,
			"-"
		},
		/* [18] */
		{59, 276, 72, 405},
		StaticText {
			disabled,
			"-"
		},
		/* [19] */
		{2, 157, 22, 177},
		UserItem {
			enabled
		},
		/* [20] */
		{324, 232, 344, 290},
		Control {
			enabled,
			200
		},
		/* [21] */
		{53, 484, 69, 586},
		UserItem {
			disabled
		},
		/* [22] */
		{69, 484, 85, 638},
		UserItem {
			disabled
		},
		/* [23] */
		{26, 201, 303, 477},
		UserItem {
			disabled
		},
		/* [24] */
		{22, 508, 55, 688},
		UserItem {
			disabled
		},
		/* [25] */
		{134, 205, 147, 248},
		StaticText {
			disabled,
			"Where:"
		},
		/* [26] */
		{134, 276, 279, 425},
		StaticText {
			disabled,
			"-"
		},
		/* [27] */
		{2, 97, 22, 117},
		UserItem {
			enabled
		},
		/* [28] */
		{2, 127, 22, 147},
		UserItem {
			enabled
		},
		/* [29] */
		{2, 37, 22, 57},
		UserItem {
			enabled
		},
		/* [30] */
		{-7, 185, 25, 217},
		UserItem {
			enabled
		},
		/* [31] */
		{2, 67, 22, 87},
		UserItem {
			enabled
		},
		/* [32] */
		{2, 7, 22, 27},
		Control {
			enabled,
			200
		},
		/* [33] */
		{159, 225, 191, 257},
		UserItem {
			disabled
		}
	}
};

resource 'ALRT' (131) {
	{40, 40, 138, 278},
	934,
	{	/* array: 4 elements */
		/* [1] */
		OK, visible, silent,
		/* [2] */
		OK, visible, silent,
		/* [3] */
		OK, visible, silent,
		/* [4] */
		OK, visible, silent
	},
	alertPositionMainScreen
};

resource 'ALRT' (128, "General Error") {
	{106, 86, 221, 429},
	268,
	{	/* array: 4 elements */
		/* [1] */
		OK, visible, silent,
		/* [2] */
		OK, visible, silent,
		/* [3] */
		OK, visible, silent,
		/* [4] */
		OK, visible, silent
	},
	centerParentWindowScreen
};

resource 'ALRT' (129, "Info") {
	{106, 86, 217, 429},
	457,
	{	/* array: 4 elements */
		/* [1] */
		OK, visible, silent,
		/* [2] */
		OK, visible, silent,
		/* [3] */
		OK, visible, silent,
		/* [4] */
		OK, visible, silent
	},
	centerParentWindowScreen
};

resource 'ALRT' (300, purgeable) {
	{80, 110, 199, 387},
	300,
	{	/* array: 4 elements */
		/* [1] */
		OK, visible, silent,
		/* [2] */
		OK, visible, silent,
		/* [3] */
		OK, visible, silent,
		/* [4] */
		OK, visible, silent
	},
	centerParentWindowScreen
};

resource 'ALRT' (132, "Look for music", purgeable) {
	{80, 110, 208, 390},
	301,
	{	/* array: 4 elements */
		/* [1] */
		OK, visible, silent,
		/* [2] */
		OK, visible, silent,
		/* [3] */
		OK, visible, silent,
		/* [4] */
		OK, visible, silent
	},
	centerParentWindowScreen
};

resource 'ALRT' (133, "Minimize", purgeable) {
	{80, 110, 210, 453},
	302,
	{	/* array: 4 elements */
		/* [1] */
		OK, visible, silent,
		/* [2] */
		OK, visible, silent,
		/* [3] */
		OK, visible, silent,
		/* [4] */
		OK, visible, silent
	},
	centerParentWindowScreen
};

resource 'ALRT' (130, "FATAL ERROR") {
	{40, 40, 346, 308},
	130,
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
	centerMainScreen
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
	$"6C20 6275 7474 6F6E 0000 0008 0000 0028"            /* l button.......(*/
	$"2054 6F6F 6C73 2057 696E 646F 7720 2D20"            /*  Tools Window -  */
	$"506C 6179 2063 7572 7265 6E74 2063 656C"            /* Play current cel */
	$"6C00 0000 0800 0000 1602 282D 0000 0000"            /* l.........(-.... */
	$"0000 0000 2143 6C61 7373 6963 2045 6469"            /* ....!Classic Edi */
	$"746F 7220 2D20 4C6F 6164 2061 2050 636D"            /* tor - Load a Pcm */
	$"6420 6669 6C65 0000 001B 0000 0008 2743"            /* d file........'C */
	$"6C61 7373 6963 2045 6469 746F 7220 2D20"            /* lassic Editor -  */
	$"5361 7665 2063 7572 7265 6E74 2073 656C"            /* Save current sel */
	$"6563 7469 6F6E 0000 001B 0000 0015 0228"            /* ection.........(*/
	$"2D00 0000 0000 0000 0020 4D75 7369 6320"            /* -........ Music  */
	$"4C69 7374 2057 696E 646F 7720 2D20 4C6F"            /* List Window - Lo */
	$"6164 2061 206D 7573 6963 0000 0007 0000"            /* ad a music...... */
	$"001D"                                               /* .. */
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

resource 'dctb' (500) {
	{	/* array ColorSpec: 5 elements */
		/* [1] */
		wContentColor, 43690, 43690, 43690,
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

resource 'dctb' (141, "Adaptators") {
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

resource 'dctb' (144, "PatList") {
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

resource 'dctb' (129) {
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

resource 'dctb' (137, "Samples") {
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

resource 'dctb' (150, "Digi") {
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

resource 'dctb' (143, "MusicList") {
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

resource 'dctb' (153, "Digi") {
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

resource 'dctb' (154) {
	{	/* array ColorSpec: 5 elements */
		/* [1] */
		wContentColor, 65535, 65535, 65535,
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

resource 'dctb' (133, "Registration") {
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

resource 'dctb' (157) {
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

resource 'dctb' (134, "Registration") {
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

resource 'dctb' (163, "MusicList") {
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

resource 'dctb' (135, "Samples") {
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

resource 'dctb' (147, "Samples") {
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

resource 'dctb' (166, "PatList") {
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

resource 'dctb' (145, "Piano") {
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

resource 'dctb' (140, "Samples") {
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

resource 'dctb' (155, "Color") {
	{	/* array ColorSpec: 5 elements */
		/* [1] */
		wContentColor, 0, 0, 0,
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

resource 'dctb' (171, "StartUp Dialog") {
	{	/* array ColorSpec: 5 elements */
		/* [1] */
		wContentColor, 0, 0, 0,
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

resource 'dctb' (164, "Spectrum") {
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

resource 'dctb' (173) {
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

resource 'dctb' (165, "Microphone") {
	{	/* array ColorSpec: 5 elements */
		/* [1] */
		wContentColor, 61166, 61166, 61166,
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

resource 'dctb' (168, "File informations") {
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

resource 'dctb' (176, "Quicktime 2.0 Instruments") {
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

resource 'dctb' (156, "ProgressWindow") {
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
		wTitleBarColor, 0, 0, 0
	}
};

resource 'dctb' (160, "Export AIFF/SND") {
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

resource 'dctb' (167, "Pattern Info") {
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

resource 'dctb' (161, "SoundFormat Export") {
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

resource 'dctb' (172, "Move") {
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

resource 'dctb' (174, "Print Order Form") {
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

resource 'dctb' (175, "Registration") {
	{	/* array ColorSpec: 5 elements */
		/* [1] */
		wContentColor, 65535, 65535, 65535,
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

resource 'dctb' (2000, "Pref: DRIVER") {
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

resource 'dctb' (3000, "General Error") {
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

resource 'dctb' (3001, "Info") {
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

resource 'dctb' (3002, "Save Or Not?") {
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

resource 'dctb' (3003, "File Not Found") {
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

resource 'dctb' (3004, "Music List Save Or Not?") {
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

resource 'dctb' (149, "Minimize Results") {
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

resource 'dctb' (152, "Find/Replace") {
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

resource 'dctb' (2001) {
	{	/* array ColorSpec: 5 elements */
		/* [1] */
		wContentColor, 65535, 65535, 65535,
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

resource 'dctb' (177) {
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

resource 'dctb' (131, "Intro") {
	{	/* array ColorSpec: 5 elements */
		/* [1] */
		wContentColor, 0, 0, 0,
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

resource 'dctb' (2005) {
	{	/* array ColorSpec: 5 elements */
		/* [1] */
		wContentColor, 65535, 65535, 65535,
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

resource 'dctb' (2004) {
	{	/* array ColorSpec: 5 elements */
		/* [1] */
		wContentColor, 65535, 65535, 65535,
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

resource 'dctb' (178) {
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

resource 'dctb' (139, "Editor") {
	{	/* array ColorSpec: 5 elements */
		/* [1] */
		wContentColor, 65535, 65535, 65535,
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

resource 'dctb' (136, "Windows State") {
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

resource 'dctb' (138, "Reset Window") {
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

resource 'dctb' (2009) {
	{	/* array ColorSpec: 5 elements */
		/* [1] */
		wContentColor, 65535, 65535, 65535,
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

resource 'dctb' (2006) {
	{	/* array ColorSpec: 5 elements */
		/* [1] */
		wContentColor, 65535, 65535, 65535,
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

resource 'dctb' (180) {
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

resource 'dctb' (181) {
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

resource 'dctb' (132) {
	{	/* array ColorSpec: 5 elements */
		/* [1] */
		wContentColor, 65535, 65535, 65535,
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

resource 'dctb' (130) {
	{	/* array ColorSpec: 5 elements */
		/* [1] */
		wContentColor, 65535, 65535, 65535,
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

resource 'dctb' (182) {
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

resource 'dctb' (148, "General Informations") {
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

resource 'dctb' (183) {
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

resource 'dctb' (184) {
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

resource 'dctb' (146, "EditInfo Instrument") {
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

resource 'dctb' (142, "MusicList") {
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

resource 'mctb' (178) {
	{	/* array MCTBArray: 1 elements */
		/* [1] */
		mctbLast, 0,
		{	/* array: 4 elements */
			/* [1] */
			0, 0, 0,
			/* [2] */
			0, 0, 0,
			/* [3] */
			0, 0, 0,
			/* [4] */
			0, 0, 0
		}
	}
};

resource 'mctb' (177) {
	{	/* array MCTBArray: 1 elements */
		/* [1] */
		mctbLast, 0,
		{	/* array: 4 elements */
			/* [1] */
			0, 0, 0,
			/* [2] */
			0, 0, 0,
			/* [3] */
			0, 0, 0,
			/* [4] */
			0, 0, 0
		}
	}
};

resource 'mctb' (135) {
	{	/* array MCTBArray: 1 elements */
		/* [1] */
		mctbLast, 0,
		{	/* array: 4 elements */
			/* [1] */
			0, 0, 0,
			/* [2] */
			0, 0, 0,
			/* [3] */
			0, 0, 0,
			/* [4] */
			0, 0, 0
		}
	}
};

resource 'actb' (130) {
	{	/* array ColorSpec: 5 elements */
		/* [1] */
		wContentColor, 65535, 65535, 65535,
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

resource 'cctb' (200) {
	{	/* array ColorSpec: 4 elements */
		/* [1] */
		cFrameColor, 57079, 57079, 57079,
		/* [2] */
		cBodyColor, 65535, 65535, 65535,
		/* [3] */
		cTextColor, 0, 0, 0,
		/* [4] */
		cElevatorColor, 65535, 65535, 65535
	}
};

resource 'CNTL' (200, purgeable) {
	{115, 515, 135, 535},
	0,
	visible,
	171,
	769,
	33,
	0,
	""
};



resource 'MENU' (131) {
	131,
	textMenuProc,
	0x7FFFFFB5,
	enabled,
	"Views",
	{	/* array: 11 elements */
		/* [1] */
		"Show Online Help", noIcon, noKey, noMark, plain,
		/* [2] */
		"-", noIcon, noKey, noMark, plain,
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

resource 'STR#' (128, "Errors") {
	{	/* array StringArray: 108 elements */
		/* [1] */
		"PlayerPRO can only read 11Khz or 22Khz "
		"sounds.",
		/* [2] */
		"There isn’t enough memory for this sound"
		". Allocate more memory for PlayerPRO us"
		"ing the Finder’s “Get Info.” ",
		/* [3] */
		"This program requires 32-bit addressing."
		" Use the Memory Control Panel to set you"
		"r system to 32-bit mode and restart.",
		/* [4] */
		"No sounds/music could be found in this f"
		"ile. Check this file.",
		/* [5] */
		"A file error has occured. There probably"
		" wasn’t enough room.",
		/* [6] */
		"MacOS 8.0 or higher is required by Playe"
		"r PRO.",
		/* [7] */
		"This sound uses a compression format whi"
		"ch can't be read by PlayerPRO. Please r"
		"efer to the documentation.",
		/* [8] */
		"Your changes will not be in effect until"
		" the next MOD file is opened.",
		/* [9] */
		"Memory is getting critically low.  Alloc"
		"ate more memory for PlayerPRO using the"
		" Finder’s “Get Info.” ",
		/* [10] */
		"This music couldn't be found.",
		/* [11] */
		"This driver is only available on the AV "
		"series Macintoshs.",
		/* [12] */
		"Sorry, but this is the un-registered ver"
		"sion of PlayerPRO! You need to register "
		"PlayerPRO to do this!",
		/* [13] */
		"To do this, you will need to select an i"
		"nstrument in the “Instrument List” windo"
		"w.",
		/* [14] */
		"Do you really want to replace this instr"
		"ument?",
		/* [15] */
		"This MOD file is out of date. Should I u"
		"pdate it?",
		/* [16] */
		"This Macintosh is able to use a higher-p"
		"erformance driver. It's better to use th"
		"e ROSSET Routines MOD 2.",
		/* [17] */
		"Your Macintosh is unable to play sound i"
		"n stereo. Buy another Macintosh!",
		/* [18] */
		"Where is the Apple Sound Chip (ASC)? Wha"
		"t is this computer?",
		/* [19] */
		"Sorry, but you don’t have a microphone.",
		/* [20] */
		"The instrument loop is incorrect. Please"
		" fix it.",
		/* [21] */
		"Are you sure that you want to delete thi"
		"s instrument?",
		/* [22] */
		"Sorry, but you can't use this driver wit"
		"hout an Apple Sound Chip.",
		/* [23] */
		"You must be in 256 color mode and have a"
		" 640x480 monitor to use this function.",
		/* [24] */
		"Sorry, but that sound is too large. Inst"
		"ruments must be smaller than 128K.",
		/* [25] */
		"Please rebuild your Desktop! (Reboot whi"
		"le holding the Command and Option keys.)",
		/* [26] */
		"Sorry, but that response was invalid. Pl"
		"ease enter a number between 0 and the hi"
		"ghest pattern.",
		/* [27] */
		"PlayerPRO requires the Sound Manager 3."
		"0 to run.",
		/* [28] */
		"Sorry, but the length of the partition m"
		"ust be between 1 and 128.",
		/* [29] */
		"Sorry, but this registration code is inc"
		"orrect.",
		/* [30] */
		"Please enter your name.",
		/* [31] */
		"This program has already been registered"
		".",
		/* [32] */
		"The un-registered version of PlayerPRO i"
		"s limited to only 20 minutes! Please sup"
		"port PlayerPRO development!",
		/* [33] */
		"Sorry, but PlayerPRO requires AppleEven"
		"ts.",
		/* [34] */
		"Sorry, but there’s nothing in this windo"
		"w to print.",
		/* [35] */
		"This MIDI file is unrecognizable. It doe"
		"sn’t seem to be a SMF (MThd) file.",
		/* [36] */
		"In your pattern list, the higher pattern"
		" number MUST appear at least one time!",
		/* [37] */
		"Are you sure that you want to replace th"
		"e current partition with this partition "
		"of the MIDI file?",
		/* [38] */
		"Sorry, but nothing is selected.",
		/* [39] */
		"This MIDI file has no samples. Samples a"
		"re required if you want to hear somethin"
		"g!",
		/* [40] */
		"Sorry, but the “Plug-In” modules returne"
		"d an error. Please refer to the document"
		"ation.",
		/* [41] */
		"Sorry, but PlayerPRO is UNREGISTERED, an"
		"d your 15 day trial period is over. You "
		"must either purchase this program or del"
		"ete it!",
		/* [42] */
		"This function doesn't work in this versi"
		"on.",
		/* [43] */
		"It's not a valid value!",
		/* [44] */
		"I cannot open this file.\nFile Type Error"
		".\nPlug Import Error.",
		/* [45] */
		"I don't have Import Method to open this"
		" file.",
		/* [46] */
		"Are you sure that you want to delete not"
		"es of this track and pattern?",
		/* [47] */
		"WARNING - This is a beta version of this"
		" Import Plug!",
		/* [48] */
		"Are you sure that you want to purge this"
		" pattern?",
		/* [49] */
		"I cannot delete this pattern. Your music"
		" file MUST have 1 pattern at least.",
		/* [50] */
		"Are you sure that you want to delete thi"
		"s pattern?",
		/* [51] */
		"Do you want to replace the selected patt"
		"ern or load it as a new pattern?",
		/* [52] */
		"This PlayerPRO version supports pattern "
		"size from 1 to 999. Change it, please!",
		/* [53] */
		"Do you really want to change the amplitu"
		"de of this sound?",
		/* [54] */
		"Printer Driver Error...",
		/* [55] */
		"Use PtMid program to convert Midi files "
		"into PlayerPRO files.",
		/* [56] */
		"I cannot find any help file.",
		/* [57] */
		"MIDI Hardware Error. Check PatchBay Cont"
		"rol Panel and your connections.",
		/* [58] */
		"MIDI Factor value must be between 0% and"
		" 500% of the velocity value.",
		/* [59] */
		"Error in 'argument' value. It's a hexade"
		"cimal from 0 to FF.",
		/* [60] */
		"Correct the file type & creator of this "
		"file before opening?",
		/* [61] */
		"This SoundInPut device doesn't support o"
		"ptions dialog.",
		/* [62] */
		"File not found !\nI cannot find this musi"
		"c on your hard disk.",
		/* [63] */
		"Not enough memory to do this operation. "
		"Allocate more memory to PlayerPRO.",
		/* [64] */
		"The name “^2” is already taken by anothe"
		"r file. Please use a different name.",
		/* [65] */
		"Give a name to this sound, if you want t"
		"o create a file !",
		/* [66] */
		"Sorry, but sample rate (c2spd) must be b"
		"etween 1 Hz and 50 Khz.",
		/* [67] */
		"PLUG ERROR: This function is not support"
		"ed by this plug. (orderNotImplemented)",
		/* [68] */
		"PLUG ERROR: Not enough memory to do this"
		" operation. Allocate more memory to Play"
		"erPRO. (needMoreMemory)",
		/* [69] */
		"PLUG ERROR: This file format isn't suppo"
		"rted by this plug. (fileNotSupportedByTh"
		"isPlug)",
		/* [70] */
		"PLUG ERROR: Unknown Error. (unknowError)",
		/* [71] */
		"PLUG ERROR: PlayerPRO cannot find a plug"
		" to open this file format.\n(cannotFindPl"
		"ug)",
		/* [72] */
		"You don't have 'QuickTime™ Musical Instr"
		"uments' extension. You cannot use this f"
		"unction.",
		/* [73] */
		"Error: PlayerPRO cannot open 'Help PP' r"
		"esources.",
		/* [74] */
		"Error: I cannot find an input device.",
		/* [75] */
		"Writing Error: volume write protected.\n\n"
		"Choose another volume to save this file.",
		/* [76] */
		"Sorry, but RealNote must be between -96 "
		"and 96.",
		/* [77] */
		"Sorry, but Volume must be between 0 and "
		"64.",
		/* [78] */
		"Sorry, but Volume Fade must be between 0"
		" and 32767.",
		/* [79] */
		"Sorry, maximum points for volume envelop"
		"e is 12.",
		/* [80] */
		"Error in 'volume' value. It's a hexadeci"
		"mal from 0 to FF.",
		/* [81] */
		"Sorry, but the unregistered version cann"
		"ot open this kind of files. Only registe"
		"red version can open all sound/music fil"
		"es.",
		/* [82] */
		"You have to turn OFF Recording state. Se"
		"e the Tools Window.",
		/* [83] */
		"Writing Error. I cannot write this file "
		"on this hard disk.",
		/* [84] */
		"Please enter your country.",
		/* [85] */
		"Please select the distributor, by clicki"
		"ng on the pop-up menu.",
		/* [86] */
		"PLUG ERROR: PlayerPRO cannot find the pl"
		"ug to do this operation.\n(cannotFindPlug"
		")",
		/* [87] */
		"Your 'PlayerPRO Pref' file was CORRUPTED"
		". It has been deleted!",
		/* [88] */
		"WARNING - This function doesn't support "
		"UNDO. This function will remove all unus"
		"ed data of this music. Continue?",
		/* [89] */
		"I'm sorry, this function needs Drag Mana"
		"ger Extension. It is available on your "
		"system disks.",
		/* [90] */
		"PlayerPRO has a problem to identify your"
		" name & your registration ID. Check it!",
		/* [91] */
		"Do you really want to change the mode of"
		" this sound?",
		/* [92] */
		"PlayerPRO requires AppearanceManager ext"
		"ension, please update your MacOS to vers"
		"ion 8.0 or higher.",
		/* [93] */
		"This beta version has expired. Please do"
		"wnload final version on www.quadmation.c"
		"om/pphome.htm",
		/* [94] */
		"WARNING !\nYou are entering in a protecte"
		"d area.\nYou should not be here.\nPlease e"
		"xit NOW !",
		/* [95] */
		"PlayerPRO requires Quicktime extension v"
		"ersion 2.5 or higher.",
		/* [96] */
		"PlayerPRO is not able to open this kind "
		"of file.",
		/* [97] */
		"There are NO space between letters in th"
		"e registration code! Remove them please.",
		/* [98] */
		"Not enough memory to load your internet "
		"browser. Closing application can make mo"
		"re memory available.",
		/* [99] */
		"Internet Connection Error.",
		/* [100] */
		"Are you sure that you want to reset the "
		"URLs list to the default one?",
		/* [101] */
		"This version of PlayerPRO requires a Pow"
		"erPC G4 with Altivec support.",
		/* [102] */
		"Are you sure that you want to delete thi"
		"s music file?\nYou cannot undo this opera"
		"tion.",
		/* [103] */
		"You need to open “Digital Editor” window"
		" to use this function.",
		/* [104] */
		"Are you sure that you want to delete thi"
		"s VST settings?",
		/* [105] */
		"Are you sure that you want to replace th"
		"ese settings with current one?",
		/* [106] */
		"This file is compressed in “.zip” format"
		". You need to uncompress it before it can be used.",
		/* [107] */
		"Some VST Plugs are not available: ",
		/* [108] */
		"MPEG4 requires Quicktime 6 or higher. Yo"
		"u can download it on http://www.apple.co"
		"m/quicktime"
	}
};

