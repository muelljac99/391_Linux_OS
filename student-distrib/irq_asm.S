

common_irq:
	


/* a bunch of short irq specific code segments that will be available for 
 * when a driver initializes an irq line
 */
irq_00:
	pushl $0xFFFFFFFF
	jmp commont_irq
	
irq_01:
	pushl $0xFFFFFFFE
	jmp commont_irq
	
irq_02:
	pushl $0xFFFFFFFD
	jmp commont_irq
	
irq_03:
	pushl $0xFFFFFFFC
	jmp commont_irq

irq_04:
	pushl $0xFFFFFFFB
	jmp commont_irq

irq_05:
	pushl $0xFFFFFFFA
	jmp commont_irq

irq_06:
	pushl $0xFFFFFFF9
	jmp commont_irq
	
irq_07:
	pushl $0xFFFFFFF8
	jmp commont_irq

irq_08:
	pushl $0xFFFFFFF7
	jmp commont_irq
	
irq_09:
	pushl $0xFFFFFFF6
	jmp commont_irq
	
irq_0A:
	pushl $0xFFFFFFF5
	jmp commont_irq
	
irq_0B:
	pushl $0xFFFFFFF4
	jmp commont_irq
	
irq_0C:
	pushl $0xFFFFFFF3
	jmp commont_irq

irq_0D:
	pushl $0xFFFFFFF2
	jmp commont_irq

irq_0E:
	pushl $0xFFFFFFF1
	jmp commont_irq

irq_0F:
	pushl $0xFFFFFFF0
	jmp commont_irq

irq_10:
	pushl $0xFFFFFFEF
	jmp commont_irq
	
irq_11:
	pushl $0xFFFFFFEE
	jmp commont_irq
	
irq_12:
	pushl $0xFFFFFFED
	jmp commont_irq
	
irq_13:
	pushl $0xFFFFFFEC
	jmp commont_irq

irq_14:
	pushl $0xFFFFFFEB
	jmp commont_irq

irq_15:
	pushl $0xFFFFFFEA
	jmp commont_irq

irq_16:
	pushl $0xFFFFFFE9
	jmp commont_irq
	
irq_17:
	pushl $0xFFFFFFE8
	jmp commont_irq

irq_18:
	pushl $0xFFFFFFE7
	jmp commont_irq
	
irq_19:
	pushl $0xFFFFFFE6
	jmp commont_irq
	
irq_1A:
	pushl $0xFFFFFFE5
	jmp commont_irq
	
irq_1B:
	pushl $0xFFFFFFE4
	jmp commont_irq
	
irq_1C:
	pushl $0xFFFFFFE3
	jmp commont_irq

irq_1D:
	pushl $0xFFFFFFE2
	jmp commont_irq

irq_1E:
	pushl $0xFFFFFFE1
	jmp commont_irq

irq_1F:
	pushl $0xFFFFFFE0
	jmp commont_irq
	
irq_20:
	pushl $0xFFFFFFDF
	jmp commont_irq
	
irq_21:
	pushl $0xFFFFFFDE
	jmp commont_irq
	
irq_22:
	pushl $0xFFFFFFDD
	jmp commont_irq
	
irq_23:
	pushl $0xFFFFFFDC
	jmp commont_irq

irq_24:
	pushl $0xFFFFFFDB
	jmp commont_irq

irq_25:
	pushl $0xFFFFFFDA
	jmp commont_irq

irq_26:
	pushl $0xFFFFFFD9
	jmp commont_irq
	
irq_27:
	pushl $0xFFFFFFD8
	jmp commont_irq

irq_28:
	pushl $0xFFFFFFD7
	jmp commont_irq
	
irq_29:
	pushl $0xFFFFFFD6
	jmp commont_irq
	
irq_2A:
	pushl $0xFFFFFFD5
	jmp commont_irq
	
irq_2B:
	pushl $0xFFFFFFD4
	jmp commont_irq
	
irq_2C:
	pushl $0xFFFFFFD3
	jmp commont_irq

irq_2D:
	pushl $0xFFFFFFD2
	jmp commont_irq

irq_2E:
	pushl $0xFFFFFFD1
	jmp commont_irq

irq_2F:
	pushl $0xFFFFFFD0
	jmp commont_irq
	
irq_30:
	pushl $0xFFFFFFCF
	jmp commont_irq
	
irq_31:
	pushl $0xFFFFFFCE
	jmp commont_irq
	
irq_32:
	pushl $0xFFFFFFCD
	jmp commont_irq
	
irq_33:
	pushl $0xFFFFFFCC
	jmp commont_irq

irq_34:
	pushl $0xFFFFFFCB
	jmp commont_irq

irq_35:
	pushl $0xFFFFFFCA
	jmp commont_irq

irq_36:
	pushl $0xFFFFFFC9
	jmp commont_irq
	
irq_37:
	pushl $0xFFFFFFC8
	jmp commont_irq

irq_38:
	pushl $0xFFFFFFC7
	jmp commont_irq
	
irq_39:
	pushl $0xFFFFFFC6
	jmp commont_irq
	
irq_3A:
	pushl $0xFFFFFFC5
	jmp commont_irq
	
irq_3B:
	pushl $0xFFFFFFC4
	jmp commont_irq
	
irq_3C:
	pushl $0xFFFFFFC3
	jmp commont_irq

irq_3D:
	pushl $0xFFFFFFC2
	jmp commont_irq

irq_3E:
	pushl $0xFFFFFFC1
	jmp commont_irq

irq_3F:
	pushl $0xFFFFFFC0
	jmp commont_irq
	
irq_40:
	pushl $0xFFFFFFBF
	jmp commont_irq
	
irq_41:
	pushl $0xFFFFFFBE
	jmp commont_irq
	
irq_42:
	pushl $0xFFFFFFBD
	jmp commont_irq
	
irq_43:
	pushl $0xFFFFFFBC
	jmp commont_irq

irq_44:
	pushl $0xFFFFFFBB
	jmp commont_irq

irq_45:
	pushl $0xFFFFFFBA
	jmp commont_irq

irq_46:
	pushl $0xFFFFFFB9
	jmp commont_irq
	
irq_47:
	pushl $0xFFFFFFB8
	jmp commont_irq

irq_48:
	pushl $0xFFFFFFB7
	jmp commont_irq
	
irq_49:
	pushl $0xFFFFFFB6
	jmp commont_irq
	
irq_4A:
	pushl $0xFFFFFFB5
	jmp commont_irq
	
irq_4B:
	pushl $0xFFFFFFB4
	jmp commont_irq
	
irq_4C:
	pushl $0xFFFFFFB3
	jmp commont_irq

irq_4D:
	pushl $0xFFFFFFB2
	jmp commont_irq

irq_4E:
	pushl $0xFFFFFFB1
	jmp commont_irq

irq_4F:
	pushl $0xFFFFFFB0
	jmp commont_irq
	
irq_50:
	pushl $0xFFFFFFAF
	jmp commont_irq
	
irq_51:
	pushl $0xFFFFFFAE
	jmp commont_irq
	
irq_52:
	pushl $0xFFFFFFAD
	jmp commont_irq
	
irq_53:
	pushl $0xFFFFFFAC
	jmp commont_irq

irq_54:
	pushl $0xFFFFFFAB
	jmp commont_irq

irq_55:
	pushl $0xFFFFFFAA
	jmp commont_irq

irq_56:
	pushl $0xFFFFFFA9
	jmp commont_irq
	
irq_57:
	pushl $0xFFFFFFA8
	jmp commont_irq

irq_58:
	pushl $0xFFFFFFA7
	jmp commont_irq
	
irq_59:
	pushl $0xFFFFFFA6
	jmp commont_irq
	
irq_5A:
	pushl $0xFFFFFFA5
	jmp commont_irq
	
irq_5B:
	pushl $0xFFFFFFA4
	jmp commont_irq
	
irq_5C:
	pushl $0xFFFFFFA3
	jmp commont_irq

irq_5D:
	pushl $0xFFFFFFA2
	jmp commont_irq

irq_5E:
	pushl $0xFFFFFFA1
	jmp commont_irq

irq_5F:
	pushl $0xFFFFFFA0
	jmp commont_irq
	
irq_60:
	pushl $0xFFFFFF9F
	jmp commont_irq
	
irq_61:
	pushl $0xFFFFFF9E
	jmp commont_irq
	
irq_62:
	pushl $0xFFFFFF9D
	jmp commont_irq
	
irq_63:
	pushl $0xFFFFFF9C
	jmp commont_irq

irq_64:
	pushl $0xFFFFFF9B
	jmp commont_irq

irq_65:
	pushl $0xFFFFFF9A
	jmp commont_irq

irq_66:
	pushl $0xFFFFFF99
	jmp commont_irq
	
irq_67:
	pushl $0xFFFFFF98
	jmp commont_irq

irq_68:
	pushl $0xFFFFFF97
	jmp commont_irq
	
irq_69:
	pushl $0xFFFFFF96
	jmp commont_irq
	
irq_6A:
	pushl $0xFFFFFF95
	jmp commont_irq
	
irq_6B:
	pushl $0xFFFFFF94
	jmp commont_irq
	
irq_6C:
	pushl $0xFFFFFF93
	jmp commont_irq

irq_6D:
	pushl $0xFFFFFF92
	jmp commont_irq

irq_6E:
	pushl $0xFFFFFF91
	jmp commont_irq

irq_6F:
	pushl $0xFFFFFF90
	jmp commont_irq
	
irq_70:
	pushl $0xFFFFFF8F
	jmp commont_irq
	
irq_71:
	pushl $0xFFFFFF8E
	jmp commont_irq
	
irq_72:
	pushl $0xFFFFFF8D
	jmp commont_irq
	
irq_73:
	pushl $0xFFFFFF8C
	jmp commont_irq

irq_74:
	pushl $0xFFFFFF8B
	jmp commont_irq

irq_75:
	pushl $0xFFFFFF8A
	jmp commont_irq

irq_76:
	pushl $0xFFFFFF89
	jmp commont_irq
	
irq_77:
	pushl $0xFFFFFF88
	jmp commont_irq

irq_78:
	pushl $0xFFFFFF87
	jmp commont_irq
	
irq_79:
	pushl $0xFFFFFF86
	jmp commont_irq
	
irq_7A:
	pushl $0xFFFFFF85
	jmp commont_irq
	
irq_7B:
	pushl $0xFFFFFF84
	jmp commont_irq
	
irq_7C:
	pushl $0xFFFFFF83
	jmp commont_irq

irq_7D:
	pushl $0xFFFFFF82
	jmp commont_irq

irq_7E:
	pushl $0xFFFFFF81
	jmp commont_irq

irq_7F:
	pushl $0xFFFFFF80
	jmp commont_irq

irq_80:
	pushl $0xFFFFFF7F
	jmp commont_irq
	
irq_81:
	pushl $0xFFFFFF7E
	jmp commont_irq
	
irq_82:
	pushl $0xFFFFFF7D
	jmp commont_irq
	
irq_83:
	pushl $0xFFFFFF7C
	jmp commont_irq

irq_84:
	pushl $0xFFFFFF7B
	jmp commont_irq

irq_85:
	pushl $0xFFFFFF7A
	jmp commont_irq

irq_86:
	pushl $0xFFFFFF79
	jmp commont_irq
	
irq_87:
	pushl $0xFFFFFF78
	jmp commont_irq

irq_88:
	pushl $0xFFFFFF77
	jmp commont_irq
	
irq_89:
	pushl $0xFFFFFF76
	jmp commont_irq
	
irq_8A:
	pushl $0xFFFFFF75
	jmp commont_irq
	
irq_8B:
	pushl $0xFFFFFF74
	jmp commont_irq
	
irq_8C:
	pushl $0xFFFFFF73
	jmp commont_irq

irq_8D:
	pushl $0xFFFFFF72
	jmp commont_irq

irq_8E:
	pushl $0xFFFFFF71
	jmp commont_irq

irq_8F:
	pushl $0xFFFFFF70
	jmp commont_irq
	
irq_90:
	pushl $0xFFFFFF6F
	jmp commont_irq
	
irq_91:
	pushl $0xFFFFFF6E
	jmp commont_irq
	
irq_92:
	pushl $0xFFFFFF6D
	jmp commont_irq
	
irq_93:
	pushl $0xFFFFFF6C
	jmp commont_irq

irq_94:
	pushl $0xFFFFFF6B
	jmp commont_irq

irq_95:
	pushl $0xFFFFFF6A
	jmp commont_irq

irq_96:
	pushl $0xFFFFFF69
	jmp commont_irq
	
irq_97:
	pushl $0xFFFFFF68
	jmp commont_irq

irq_98:
	pushl $0xFFFFFF67
	jmp commont_irq
	
irq_99:
	pushl $0xFFFFFF66
	jmp commont_irq
	
irq_9A:
	pushl $0xFFFFFF65
	jmp commont_irq
	
irq_9B:
	pushl $0xFFFFFF64
	jmp commont_irq
	
irq_9C:
	pushl $0xFFFFFF63
	jmp commont_irq

irq_9D:
	pushl $0xFFFFFF62
	jmp commont_irq

irq_9E:
	pushl $0xFFFFFF61
	jmp commont_irq

irq_9F:
	pushl $0xFFFFFF60
	jmp commont_irq
	
irq_A0:
	pushl $0xFFFFFF5F
	jmp commont_irq
	
irq_A1:
	pushl $0xFFFFFF5E
	jmp commont_irq
	
irq_A2:
	pushl $0xFFFFFF5D
	jmp commont_irq
	
irq_A3:
	pushl $0xFFFFFF5C
	jmp commont_irq

irq_A4:
	pushl $0xFFFFFF5B
	jmp commont_irq

irq_A5:
	pushl $0xFFFFFF5A
	jmp commont_irq

irq_A6:
	pushl $0xFFFFFF59
	jmp commont_irq
	
irq_A7:
	pushl $0xFFFFFF58
	jmp commont_irq

irq_A8:
	pushl $0xFFFFFF57
	jmp commont_irq
	
irq_A9:
	pushl $0xFFFFFF56
	jmp commont_irq
	
irq_AA:
	pushl $0xFFFFFF55
	jmp commont_irq
	
irq_AB:
	pushl $0xFFFFFF54
	jmp commont_irq
	
irq_AC:
	pushl $0xFFFFFF53
	jmp commont_irq

irq_AD:
	pushl $0xFFFFFF52
	jmp commont_irq

irq_AE:
	pushl $0xFFFFFF51
	jmp commont_irq

irq_AF:
	pushl $0xFFFFFF50
	jmp commont_irq
	
irq_B0:
	pushl $0xFFFFFF4F
	jmp commont_irq
	
irq_B1:
	pushl $0xFFFFFF4E
	jmp commont_irq
	
irq_B2:
	pushl $0xFFFFFF4D
	jmp commont_irq
	
irq_B3:
	pushl $0xFFFFFF4C
	jmp commont_irq

irq_B4:
	pushl $0xFFFFFF4B
	jmp commont_irq

irq_B5:
	pushl $0xFFFFFF4A
	jmp commont_irq

irq_B6:
	pushl $0xFFFFFF49
	jmp commont_irq
	
irq_B7:
	pushl $0xFFFFFF48
	jmp commont_irq

irq_B8:
	pushl $0xFFFFFF47
	jmp commont_irq
	
irq_B9:
	pushl $0xFFFFFF46
	jmp commont_irq
	
irq_BA:
	pushl $0xFFFFFF45
	jmp commont_irq
	
irq_BB:
	pushl $0xFFFFFF44
	jmp commont_irq
	
irq_BC:
	pushl $0xFFFFFF43
	jmp commont_irq

irq_BD:
	pushl $0xFFFFFF42
	jmp commont_irq

irq_BE:
	pushl $0xFFFFFF41
	jmp commont_irq

irq_BF:
	pushl $0xFFFFFF40
	jmp commont_irq
	
irq_C0:
	pushl $0xFFFFFF3F
	jmp commont_irq
	
irq_C1:
	pushl $0xFFFFFF3E
	jmp commont_irq
	
irq_C2:
	pushl $0xFFFFFF3D
	jmp commont_irq
	
irq_C3:
	pushl $0xFFFFFF3C
	jmp commont_irq

irq_C4:
	pushl $0xFFFFFF3B
	jmp commont_irq

irq_C5:
	pushl $0xFFFFFF3A
	jmp commont_irq

irq_C6:
	pushl $0xFFFFFF39
	jmp commont_irq
	
irq_C7:
	pushl $0xFFFFFF38
	jmp commont_irq

irq_C8:
	pushl $0xFFFFFF37
	jmp commont_irq
	
irq_C9:
	pushl $0xFFFFFF36
	jmp commont_irq
	
irq_CA:
	pushl $0xFFFFFF35
	jmp commont_irq
	
irq_CB:
	pushl $0xFFFFFF34
	jmp commont_irq
	
irq_CC:
	pushl $0xFFFFFF33
	jmp commont_irq

irq_CD:
	pushl $0xFFFFFF32
	jmp commont_irq

irq_CE:
	pushl $0xFFFFFF31
	jmp commont_irq

irq_CF:
	pushl $0xFFFFFF30
	jmp commont_irq
	
irq_D0:
	pushl $0xFFFFFF2F
	jmp commont_irq
	
irq_D1:
	pushl $0xFFFFFF2E
	jmp commont_irq
	
irq_D2:
	pushl $0xFFFFFF2D
	jmp commont_irq
	
irq_D3:
	pushl $0xFFFFFF2C
	jmp commont_irq

irq_D4:
	pushl $0xFFFFFF2B
	jmp commont_irq

irq_D5:
	pushl $0xFFFFFF2A
	jmp commont_irq

irq_D6:
	pushl $0xFFFFFF29
	jmp commont_irq
	
irq_D7:
	pushl $0xFFFFFF28
	jmp commont_irq

irq_D8:
	pushl $0xFFFFFF27
	jmp commont_irq
	
irq_D9:
	pushl $0xFFFFFF26
	jmp commont_irq
	
irq_DA:
	pushl $0xFFFFFF25
	jmp commont_irq
	
irq_DB:
	pushl $0xFFFFFF24
	jmp commont_irq
	
irq_DC:
	pushl $0xFFFFFF23
	jmp commont_irq

irq_DD:
	pushl $0xFFFFFF22
	jmp commont_irq

irq_DE:
	pushl $0xFFFFFF21
	jmp commont_irq

irq_DF:
	pushl $0xFFFFFF20
	jmp commont_irq
	
irq_E0:
	pushl $0xFFFFFF1F
	jmp commont_irq
	
irq_E1:
	pushl $0xFFFFFF1E
	jmp commont_irq
	
irq_E2:
	pushl $0xFFFFFF1D
	jmp commont_irq
	
irq_E3:
	pushl $0xFFFFFF1C
	jmp commont_irq

irq_E4:
	pushl $0xFFFFFF1B
	jmp commont_irq

irq_E5:
	pushl $0xFFFFFF1A
	jmp commont_irq

irq_E6:
	pushl $0xFFFFFF19
	jmp commont_irq
	
irq_E7:
	pushl $0xFFFFFF18
	jmp commont_irq

irq_E8:
	pushl $0xFFFFFF17
	jmp commont_irq
	
irq_E9:
	pushl $0xFFFFFF16
	jmp commont_irq
	
irq_EA:
	pushl $0xFFFFFF15
	jmp commont_irq
	
irq_EB:
	pushl $0xFFFFFF14
	jmp commont_irq
	
irq_EC:
	pushl $0xFFFFFF13
	jmp commont_irq

irq_ED:
	pushl $0xFFFFFF12
	jmp commont_irq

irq_EE:
	pushl $0xFFFFFF11
	jmp commont_irq

irq_EF:
	pushl $0xFFFFFF10
	jmp commont_irq
	
irq_F0:
	pushl $0xFFFFFFEF
	jmp commont_irq
	
irq_F1:
	pushl $0xFFFFFFEE
	jmp commont_irq
	
irq_F2:
	pushl $0xFFFFFFED
	jmp commont_irq
	
irq_F3:
	pushl $0xFFFFFFEC
	jmp commont_irq

irq_F4:
	pushl $0xFFFFFFEB
	jmp commont_irq

irq_F5:
	pushl $0xFFFFFFEA
	jmp commont_irq

irq_F6:
	pushl $0xFFFFFFE9
	jmp commont_irq
	
irq_F7:
	pushl $0xFFFFFFE8
	jmp commont_irq

irq_F8:
	pushl $0xFFFFFFE7
	jmp commont_irq
	
irq_F9:
	pushl $0xFFFFFFE6
	jmp commont_irq
	
irq_FA:
	pushl $0xFFFFFFE5
	jmp commont_irq
	
irq_FB:
	pushl $0xFFFFFFE4
	jmp commont_irq
	
irq_FC:
	pushl $0xFFFFFFE3
	jmp commont_irq

irq_FD:
	pushl $0xFFFFFFE2
	jmp commont_irq

irq_FE:
	pushl $0xFFFFFFE1
	jmp commont_irq

irq_FF:
	pushl $0xFFFFFFE0
	jmp commont_irq
