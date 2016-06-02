#ifndef V
#error "VirtualKey.inl needs V macro to be defined."
#else
V(None), // 0
V(MouseButtonLeft), // 1
V(MouseButtonRight), // 2
V(ControlBreak), // 3
V(MouseButtonMiddle), // 4
V(MouseButtonX1), // 5
V(MouseButtonX2), // 6
V(Undefined00), // 7
V(Backspace), // 8
V(Tab), // 9
V(Reserved00), // 0xa
V(Reserved01), // 0xb
V(Clear), // 0xc
V(Enter), // 0xd
V(Undefined01), // 0xe
V(Undefined02), // 0xf
V(Shift), // 0x10
V(Control), // 0x11
V(Menu), // 0x12
V(Pause), // 0x13
V(Caps), // 0x14
V(Kana_Hangul), // 0x15
V(Undefined03), // 0x16
V(Junja), // 0x17
V(Final), // 0x18
V(Hanja_Kanji), // 0x19
V(Undefined04), // 0x1a
V(Escape), // 0x1b
V(Convert), // 0x1c
V(NonConvert), // 0x1d
V(Accept), // 0x1e
V(ModeChange), // 0x1f
V(Space), // 0x20
V(PageUp), // 0x21
V(PageDown), // 0x22
V(End), // 0x23
V(Home), // 0x24
V(Left), // 0x25
V(Up), // 0x26
V(Right), // 0x27
V(Down), // 0x28
V(Select), // 0x29
V(Print), // 0x2a
V(Execute), // 0x2b
V(PrintScreen), // 0x2c
V(Insert), // 0x2d
V(Delete), // 0x2e
V(Help), // 0x2f
V(K0), // 0x30
V(K1), // 0x31
V(K2), // 0x32
V(K3), // 0x33
V(K4), // 0x34
V(K5), // 0x35
V(K6), // 0x36
V(K7), // 0x37
V(K8), // 0x38
V(K9), // 0x39
V(Undefined05), // 0x3a
V(Undefined06), // 0x3b
V(Undefined07), // 0x3c
V(Undefined08), // 0x3d
V(Undefined09), // 0x3e
V(Undefined10), // 0x3f
V(Undefined11), // 0x40
V(A), // 0x41
V(B), // 0x42
V(C), // 0x43
V(D), // 0x44
V(E), // 0x45
V(F), // 0x46
V(G), // 0x47
V(H), // 0x48
V(I), // 0x49
V(J), // 0x4a
V(K), // 0x4b
V(L), // 0x4c
V(M), // 0x4d
V(N), // 0x4e
V(O), // 0x4f
V(P), // 0x50
V(Q), // 0x51
V(R), // 0x52
V(S), // 0x53
V(T), // 0x54
V(U), // 0x55
V(V), // 0x56
V(W), // 0x57
V(X), // 0x58
V(Y), // 0x59
V(Z), // 0x5a
V(WindowsKeyLeft), // 0x5b
V(WindowsKeyRight), // 0x5c
V(Apps), // 0x5d
V(Reserved02), // 0x5e
V(Sleep), // 0x5f
V(N0), // 0x60
V(N1), // 0x61
V(N2), // 0x62
V(N3), // 0x63
V(N4), // 0x64
V(N5), // 0x65
V(N6), // 0x66
V(N7), // 0x67
V(N8), // 0x68
V(N9), // 0x69
V(Multiply), // 0x6a
V(Add), // 0x6b
V(Seperator), // 0x6c
V(Subtract), // 0x6d
V(Decimal), // 0x6e
V(Divide), // 0x6f
V(F1), // 0x70
V(F2), // 0x71
V(F3), // 0x72
V(F4), // 0x73
V(F5), // 0x74
V(F6), // 0x75
V(F7), // 0x76
V(F8), // 0x77
V(F9), // 0x78
V(F10), // 0x79
V(F11), // 0x7a
V(F12), // 0x7b
V(F13), // 0x7c
V(F14), // 0x7d
V(F15), // 0x7e
V(F16), // 0x7f
V(F17), // 0x80
V(F18), // 0x81
V(F19), // 0x82
V(F20), // 0x83
V(F21), // 0x84
V(F22), // 0x85
V(F23), // 0x86
V(F24), // 0x87
V(Unassigned00), // 0x88
V(Unassigned01), // 0x89
V(Unassigned02), // 0x8a
V(Unassigned03), // 0x8b
V(Unassigned04), // 0x8c
V(Unassigned05), // 0x8d
V(Unassigned06), // 0x8e
V(Unassigned07), // 0x8f
V(Numlock), // 0x90
V(ScrollLock), // 0x91
V(OEM00), // 0x92
V(OEM01), // 0x93
V(OEM02), // 0x94
V(OEM03), // 0x95
V(OEM04), // 0x96
V(Unassigned08), // 0x97
V(Unassigned09), // 0x98
V(Unassigned10), // 0x99
V(Unassigned11), // 0x9a
V(Unassigned12), // 0x9b
V(Unassigned13), // 0x9c
V(Unassigned14), // 0x9d
V(Unassigned15), // 0x9e
V(Unassigned16), // 0x9f
V(LeftShift), // 0xa0
V(RightShift), // 0xa1
V(LeftControl), // 0xa2
V(RightControl), // 0xa3
V(LeftMenu), // 0xa4
V(RightMenu), // 0xa5
V(BrowserBack), // 0xa6
V(BrowserForward), // 0xa7
V(BrowserRefresh), // 0xa8
V(BrowserStop), // 0xa9
V(BrowserSearch), // 0xaa
V(BrowserFavorites), // 0xab
V(BrowserHome), // 0xac
V(VolumeMute), // 0xad
V(VolumeDown), // 0xae
V(VolumeUp), // 0xaf
V(MediaNextTrack), // 0xb0
V(MediaPrevTrack), // 0xb1
V(MediaStop), // 0xb2
V(MediaPlayPause), // 0xb3
V(LaunchMail), // 0xb4
V(LaunchMediaSelect), // 0xb5
V(Launch01), // 0xb6
V(Launch02), // 0xb7
V(Reserved03), // 0xb8
V(Reserved04), // 0xb9
V(OEM_1_Semicolon), // 0xba
V(OEM_Plus), // 0xbb
V(OEM_Comma), // 0xbc
V(OEM_Minus), // 0xbd
V(OEM_Period), // 0xbe
V(OEM_2_ForwardSlash), // 0xbf
V(OEM_3_Tilde), // 0xc0
V(Reserved05), // 0xc1
V(Reserved06), // 0xc2
V(Reserved07), // 0xc3
V(Reserved08), // 0xc4
V(Reserved09), // 0xc5
V(Reserved10), // 0xc6
V(Reserved11), // 0xc7
V(Reserved12), // 0xc8
V(Reserved13), // 0xc9
V(Reserved14), // 0xca
V(Reserved15), // 0xcb
V(Reserved16), // 0xcc
V(Reserved17), // 0xcd
V(Reserved18), // 0xce
V(Reserved19), // 0xcf
V(Reserved20), // 0xd0
V(Reserved21), // 0xd1
V(Reserved22), // 0xd2
V(Reserved23), // 0xd3
V(Reserved24), // 0xd4
V(Reserved25), // 0xd5
V(Reserved26), // 0xd6
V(Reserved27), // 0xd7
V(Unassigned17), // 0xd8
V(Unassigned18), // 0xd9
V(Unassigned19), // 0xda
V(OEM_4_LeftSquareBracket), // 0xdb
V(OEM_5_BackslashBar), // 0xdc
V(OEM_6_RightSquareBracket), // 0xdd
V(OEM_7_SingleQuoteDoubleQuote), // 0xde
V(OEM_8), // 0xdf
V(Reserved28), // 0xe0
V(OEM_Unknown00), // 0xe1
V(OEM_102), // 0xe2
V(OEM_Unknown01), // 0xe3
V(OEM_Unknown02), // 0xe4
V(Process), // 0xe5
V(OEM_Unknown03), // 0xe6
V(Packet), // 0xe7
V(Unassigned20), // 0xe8
V(OEM_Unknown04), // 0xe9
V(OEM_Unknown05), // 0xea
V(OEM_Unknown06), // 0xeb
V(OEM_Unknown07), // 0xec
V(OEM_Unknown08), // 0xed
V(OEM_Unknown09), // 0xee
V(OEM_Unknown10), // 0xef
V(OEM_Unknown11), // 0xf0
V(OEM_Unknown12), // 0xf1
V(OEM_Unknown13), // 0xf2
V(OEM_Unknown14), // 0xf3
V(OEM_Unknown15), // 0xf4
V(OEM_Unknown16), // 0xf5
V(Attn), // 0xf6
V(CrSel), // 0xf7
V(ExSel), // 0xf8
V(ErEof), // 0xf9
V(Play), // 0xfa
V(Zoom), // 0xfb
V(NoName), // 0xfc
V(Pa1), // 0xfd
V(OEM_Clear), // 0xfe
V(Last), // 0xff
#endif