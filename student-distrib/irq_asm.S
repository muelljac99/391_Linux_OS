# irq_asm.S - holds the assembly linkage for each irq line and the common irq function that links back to the
# 			  do_irq function.


.text

/* define all the function ptrs as global so that they can be accessed by the idt_fill function */
.global irq_00, irq_01, irq_02, irq_03, irq_04, irq_05, irq_06, irq_07, irq_08, irq_09, irq_0A, irq_0B, irq_0C, irq_0D, irq_0E, irq_0F
.global irq_10, irq_11, irq_12, irq_13, irq_14, irq_15, irq_16, irq_17, irq_18, irq_19, irq_1A, irq_1B, irq_1C, irq_1D, irq_1E, irq_1F
.global irq_20, irq_21, irq_22, irq_23, irq_24, irq_25, irq_26, irq_27, irq_28, irq_29, irq_2A, irq_2B, irq_2C, irq_2D, irq_2E, irq_2F
.global irq_30, irq_31, irq_32, irq_33, irq_34, irq_35, irq_36, irq_37, irq_38, irq_39, irq_3A, irq_3B, irq_3C, irq_3D, irq_3E, irq_3F
.global irq_40, irq_41, irq_42, irq_43, irq_44, irq_45, irq_46, irq_47, irq_48, irq_49, irq_4A, irq_4B, irq_4C, irq_4D, irq_4E, irq_4F
.global irq_50, irq_51, irq_52, irq_53, irq_54, irq_55, irq_56, irq_57, irq_58, irq_59, irq_5A, irq_5B, irq_5C, irq_5D, irq_5E, irq_5F
.global irq_60, irq_61, irq_62, irq_63, irq_64, irq_65, irq_66, irq_67, irq_68, irq_69, irq_6A, irq_6B, irq_6C, irq_6D, irq_6E, irq_6F
.global irq_70, irq_71, irq_72, irq_73, irq_74, irq_75, irq_76, irq_77, irq_78, irq_79, irq_7A, irq_7B, irq_7C, irq_7D, irq_7E, irq_7F
.global irq_80, irq_81, irq_82, irq_83, irq_84, irq_85, irq_86, irq_87, irq_88, irq_89, irq_8A, irq_8B, irq_8C, irq_8D, irq_8E, irq_8F
.global irq_90, irq_91, irq_92, irq_93, irq_94, irq_95, irq_96, irq_97, irq_98, irq_99, irq_9A, irq_9B, irq_9C, irq_9D, irq_9E, irq_9F
.global irq_A0, irq_A1, irq_A2, irq_A3, irq_A4, irq_A5, irq_A6, irq_A7, irq_A8, irq_A9, irq_AA, irq_AB, irq_AC, irq_AD, irq_AE, irq_AF
.global irq_B0, irq_B1, irq_B2, irq_B3, irq_B4, irq_B5, irq_B6, irq_B7, irq_B8, irq_B9, irq_BA, irq_BB, irq_BC, irq_BD, irq_BE, irq_BF
.global irq_C0, irq_C1, irq_C2, irq_C3, irq_C4, irq_C5, irq_C6, irq_C7, irq_C8, irq_C9, irq_CA, irq_CB, irq_CC, irq_CD, irq_CE, irq_CF
.global irq_D0, irq_D1, irq_D2, irq_D3, irq_D4, irq_D5, irq_D6, irq_D7, irq_D8, irq_D9, irq_DA, irq_DB, irq_DC, irq_DD, irq_DE, irq_DF
.global irq_E0, irq_E1, irq_E2, irq_E3, irq_E4, irq_E5, irq_E6, irq_E7, irq_E8, irq_E9, irq_EA, irq_EB, irq_EC, irq_ED, irq_EE, irq_EF
.global irq_F0, irq_F1, irq_F2, irq_F3, irq_F4, irq_F5, irq_F6, irq_F7, irq_F8, irq_F9, irq_FA, irq_FB, irq_FC, irq_FD, irq_FE, irq_FF

.global irq_handle

.align 32

irq_handle:
	.long irq_00
	.long irq_01
	.long irq_02
	.long irq_03
	.long irq_04
	.long irq_05
	.long irq_06
	.long irq_07
	.long irq_08
	.long irq_09
	.long irq_0A
	.long irq_0B
	.long irq_0C
	.long irq_0D
	.long irq_0E
	.long irq_0F
	.long irq_10
	.long irq_11
	.long irq_12
	.long irq_13
	.long irq_14
	.long irq_15
	.long irq_16
	.long irq_17
	.long irq_18
	.long irq_19
	.long irq_1A
	.long irq_1B
	.long irq_1C
	.long irq_1D
	.long irq_1E
	.long irq_1F
	.long irq_20
	.long irq_21
	.long irq_22
	.long irq_23
	.long irq_24
	.long irq_25
	.long irq_26
	.long irq_27
	.long irq_28
	.long irq_29
	.long irq_2A
	.long irq_2B
	.long irq_2C
	.long irq_2D
	.long irq_2E
	.long irq_2F
	.long irq_30
	.long irq_31
	.long irq_32
	.long irq_33
	.long irq_34
	.long irq_35
	.long irq_36
	.long irq_37
	.long irq_38
	.long irq_39
	.long irq_3A
	.long irq_3B
	.long irq_3C
	.long irq_3D
	.long irq_3E
	.long irq_3F
	.long irq_40
	.long irq_41
	.long irq_42
	.long irq_43
	.long irq_44
	.long irq_45
	.long irq_46
	.long irq_47
	.long irq_48
	.long irq_49
	.long irq_4A
	.long irq_4B
	.long irq_4C
	.long irq_4D
	.long irq_4E
	.long irq_4F
	.long irq_50
	.long irq_51
	.long irq_52
	.long irq_53
	.long irq_54
	.long irq_55
	.long irq_56
	.long irq_57
	.long irq_58
	.long irq_59
	.long irq_5A
	.long irq_5B
	.long irq_5C
	.long irq_5D
	.long irq_5E
	.long irq_5F
	.long irq_60
	.long irq_61
	.long irq_62
	.long irq_63
	.long irq_64
	.long irq_65
	.long irq_66
	.long irq_67
	.long irq_68
	.long irq_69
	.long irq_6A
	.long irq_6B
	.long irq_6C
	.long irq_6D
	.long irq_6E
	.long irq_6F
	.long irq_70
	.long irq_71
	.long irq_72
	.long irq_73
	.long irq_74
	.long irq_75
	.long irq_76
	.long irq_77
	.long irq_78
	.long irq_79
	.long irq_7A
	.long irq_7B
	.long irq_7C
	.long irq_7D
	.long irq_7E
	.long irq_7F
	.long irq_80
	.long irq_81
	.long irq_82
	.long irq_83
	.long irq_84
	.long irq_85
	.long irq_86
	.long irq_87
	.long irq_88
	.long irq_89
	.long irq_8A
	.long irq_8B
	.long irq_8C
	.long irq_8D
	.long irq_8E
	.long irq_8F
	.long irq_90
	.long irq_91
	.long irq_92
	.long irq_93
	.long irq_94
	.long irq_95
	.long irq_96
	.long irq_97
	.long irq_98
	.long irq_99
	.long irq_9A
	.long irq_9B
	.long irq_9C
	.long irq_9D
	.long irq_9E
	.long irq_9F
	.long irq_A0
	.long irq_A1
	.long irq_A2
	.long irq_A3
	.long irq_A4
	.long irq_A5
	.long irq_A6
	.long irq_A7
	.long irq_A8
	.long irq_A9
	.long irq_AA
	.long irq_AB
	.long irq_AC
	.long irq_AD
	.long irq_AE
	.long irq_AF
	.long irq_B0
	.long irq_B1
	.long irq_B2
	.long irq_B3
	.long irq_B4
	.long irq_B5
	.long irq_B6
	.long irq_B7
	.long irq_B8
	.long irq_B9
	.long irq_BA
	.long irq_BB
	.long irq_BC
	.long irq_BD
	.long irq_BE
	.long irq_BF
	.long irq_C0
	.long irq_C1
	.long irq_C2
	.long irq_C3
	.long irq_C4
	.long irq_C5
	.long irq_C6
	.long irq_C7
	.long irq_C8
	.long irq_C9
	.long irq_CA
	.long irq_CB
	.long irq_CC
	.long irq_CD
	.long irq_CE
	.long irq_CF
	.long irq_D0
	.long irq_D1
	.long irq_D2
	.long irq_D3
	.long irq_D4
	.long irq_D5
	.long irq_D6
	.long irq_D7
	.long irq_D8
	.long irq_D9
	.long irq_DA
	.long irq_DB
	.long irq_DC
	.long irq_DD
	.long irq_DE
	.long irq_DF
	.long irq_E0
	.long irq_E1
	.long irq_E2
	.long irq_E3
	.long irq_E4
	.long irq_E5
	.long irq_E6
	.long irq_E7
	.long irq_E8
	.long irq_E9
	.long irq_EA
	.long irq_EB
	.long irq_EC
	.long irq_ED
	.long irq_EE
	.long irq_EF
	.long irq_F0
	.long irq_F1
	.long irq_F2
	.long irq_F3
	.long irq_F4
	.long irq_F5
	.long irq_F6
	.long irq_F7
	.long irq_F8
	.long irq_F9
	.long irq_FA
	.long irq_FB
	.long irq_FC
	.long irq_FD
	.long irq_FE
	.long irq_FF

common_irq:
	// save all the registers (use push a instead
	pushw %fs
	pushw $0
	pushw %es
	pushw $0
	pushw %ds
	pushw $0
	pushl %eax
	pushl %ebp
	pushl %edi
	pushl %esi
	pushl %edx
	pushl %ecx
	pushl %ebx
	
	// starts C style call to do_irq
	pushl %esp 			// pushing instead of using fastcall
	call do_irq
	addl $4, %esp 		// remove the argument from the stack
	jmp ret_from_irq
	
ret_from_irq:
	// restore all the registers
	popl %ebx
	popl %ecx
	popl %edx
	popl %esi
	popl %edi
	popl %ebp
	popl %eax
	addl $2, %esp
	popw %ds
	addl $2, %esp
	popw %es
	addl $2, %esp
	popw %fs
	
	// pop the irq value
	addl $4, %esp
	
	//return from the interrupt call
	//leave
	iret

/* a bunch of short irq specific code segments that will be available for 
 * when a driver initializes an irq line
 */
irq_00:
	pushl $0xFFFFFFFF
	jmp common_irq
	
irq_01:
	pushl $0xFFFFFFFE
	jmp common_irq
	
irq_02:
	pushl $0xFFFFFFFD
	jmp common_irq
	
irq_03:
	pushl $0xFFFFFFFC
	jmp common_irq

irq_04:
	pushl $0xFFFFFFFB
	jmp common_irq

irq_05:
	pushl $0xFFFFFFFA
	jmp common_irq

irq_06:
	pushl $0xFFFFFFF9
	jmp common_irq
	
irq_07:
	pushl $0xFFFFFFF8
	jmp common_irq

irq_08:
	pushl $0xFFFFFFF7
	jmp common_irq
	
irq_09:
	pushl $0xFFFFFFF6
	jmp common_irq
	
irq_0A:
	pushl $0xFFFFFFF5
	jmp common_irq
	
irq_0B:
	pushl $0xFFFFFFF4
	jmp common_irq
	
irq_0C:
	pushl $0xFFFFFFF3
	jmp common_irq

irq_0D:
	pushl $0xFFFFFFF2
	jmp common_irq

irq_0E:
	pushl $0xFFFFFFF1
	jmp common_irq

irq_0F:
	pushl $0xFFFFFFF0
	jmp common_irq

irq_10:
	pushl $0xFFFFFFEF
	jmp common_irq
	
irq_11:
	pushl $0xFFFFFFEE
	jmp common_irq
	
irq_12:
	pushl $0xFFFFFFED
	jmp common_irq
	
irq_13:
	pushl $0xFFFFFFEC
	jmp common_irq

irq_14:
	pushl $0xFFFFFFEB
	jmp common_irq

irq_15:
	pushl $0xFFFFFFEA
	jmp common_irq

irq_16:
	pushl $0xFFFFFFE9
	jmp common_irq
	
irq_17:
	pushl $0xFFFFFFE8
	jmp common_irq

irq_18:
	pushl $0xFFFFFFE7
	jmp common_irq
	
irq_19:
	pushl $0xFFFFFFE6
	jmp common_irq
	
irq_1A:
	pushl $0xFFFFFFE5
	jmp common_irq
	
irq_1B:
	pushl $0xFFFFFFE4
	jmp common_irq
	
irq_1C:
	pushl $0xFFFFFFE3
	jmp common_irq

irq_1D:
	pushl $0xFFFFFFE2
	jmp common_irq

irq_1E:
	pushl $0xFFFFFFE1
	jmp common_irq

irq_1F:
	pushl $0xFFFFFFE0
	jmp common_irq
	
irq_20:
	pushl $0xFFFFFFDF
	jmp common_irq
	
irq_21:
	pushl $0xFFFFFFDE
	jmp common_irq
	
irq_22:
	pushl $0xFFFFFFDD
	jmp common_irq
	
irq_23:
	pushl $0xFFFFFFDC
	jmp common_irq

irq_24:
	pushl $0xFFFFFFDB
	jmp common_irq

irq_25:
	pushl $0xFFFFFFDA
	jmp common_irq

irq_26:
	pushl $0xFFFFFFD9
	jmp common_irq
	
irq_27:
	pushl $0xFFFFFFD8
	jmp common_irq

irq_28:
	pushl $0xFFFFFFD7
	jmp common_irq
	
irq_29:
	pushl $0xFFFFFFD6
	jmp common_irq
	
irq_2A:
	pushl $0xFFFFFFD5
	jmp common_irq
	
irq_2B:
	pushl $0xFFFFFFD4
	jmp common_irq
	
irq_2C:
	pushl $0xFFFFFFD3
	jmp common_irq

irq_2D:
	pushl $0xFFFFFFD2
	jmp common_irq

irq_2E:
	pushl $0xFFFFFFD1
	jmp common_irq

irq_2F:
	pushl $0xFFFFFFD0
	jmp common_irq
	
irq_30:
	pushl $0xFFFFFFCF
	jmp common_irq
	
irq_31:
	pushl $0xFFFFFFCE
	jmp common_irq
	
irq_32:
	pushl $0xFFFFFFCD
	jmp common_irq
	
irq_33:
	pushl $0xFFFFFFCC
	jmp common_irq

irq_34:
	pushl $0xFFFFFFCB
	jmp common_irq

irq_35:
	pushl $0xFFFFFFCA
	jmp common_irq

irq_36:
	pushl $0xFFFFFFC9
	jmp common_irq
	
irq_37:
	pushl $0xFFFFFFC8
	jmp common_irq

irq_38:
	pushl $0xFFFFFFC7
	jmp common_irq
	
irq_39:
	pushl $0xFFFFFFC6
	jmp common_irq
	
irq_3A:
	pushl $0xFFFFFFC5
	jmp common_irq
	
irq_3B:
	pushl $0xFFFFFFC4
	jmp common_irq
	
irq_3C:
	pushl $0xFFFFFFC3
	jmp common_irq

irq_3D:
	pushl $0xFFFFFFC2
	jmp common_irq

irq_3E:
	pushl $0xFFFFFFC1
	jmp common_irq

irq_3F:
	pushl $0xFFFFFFC0
	jmp common_irq
	
irq_40:
	pushl $0xFFFFFFBF
	jmp common_irq
	
irq_41:
	pushl $0xFFFFFFBE
	jmp common_irq
	
irq_42:
	pushl $0xFFFFFFBD
	jmp common_irq
	
irq_43:
	pushl $0xFFFFFFBC
	jmp common_irq

irq_44:
	pushl $0xFFFFFFBB
	jmp common_irq

irq_45:
	pushl $0xFFFFFFBA
	jmp common_irq

irq_46:
	pushl $0xFFFFFFB9
	jmp common_irq
	
irq_47:
	pushl $0xFFFFFFB8
	jmp common_irq

irq_48:
	pushl $0xFFFFFFB7
	jmp common_irq
	
irq_49:
	pushl $0xFFFFFFB6
	jmp common_irq
	
irq_4A:
	pushl $0xFFFFFFB5
	jmp common_irq
	
irq_4B:
	pushl $0xFFFFFFB4
	jmp common_irq
	
irq_4C:
	pushl $0xFFFFFFB3
	jmp common_irq

irq_4D:
	pushl $0xFFFFFFB2
	jmp common_irq

irq_4E:
	pushl $0xFFFFFFB1
	jmp common_irq

irq_4F:
	pushl $0xFFFFFFB0
	jmp common_irq
	
irq_50:
	pushl $0xFFFFFFAF
	jmp common_irq
	
irq_51:
	pushl $0xFFFFFFAE
	jmp common_irq
	
irq_52:
	pushl $0xFFFFFFAD
	jmp common_irq
	
irq_53:
	pushl $0xFFFFFFAC
	jmp common_irq

irq_54:
	pushl $0xFFFFFFAB
	jmp common_irq

irq_55:
	pushl $0xFFFFFFAA
	jmp common_irq

irq_56:
	pushl $0xFFFFFFA9
	jmp common_irq
	
irq_57:
	pushl $0xFFFFFFA8
	jmp common_irq

irq_58:
	pushl $0xFFFFFFA7
	jmp common_irq
	
irq_59:
	pushl $0xFFFFFFA6
	jmp common_irq
	
irq_5A:
	pushl $0xFFFFFFA5
	jmp common_irq
	
irq_5B:
	pushl $0xFFFFFFA4
	jmp common_irq
	
irq_5C:
	pushl $0xFFFFFFA3
	jmp common_irq

irq_5D:
	pushl $0xFFFFFFA2
	jmp common_irq

irq_5E:
	pushl $0xFFFFFFA1
	jmp common_irq

irq_5F:
	pushl $0xFFFFFFA0
	jmp common_irq
	
irq_60:
	pushl $0xFFFFFF9F
	jmp common_irq
	
irq_61:
	pushl $0xFFFFFF9E
	jmp common_irq
	
irq_62:
	pushl $0xFFFFFF9D
	jmp common_irq
	
irq_63:
	pushl $0xFFFFFF9C
	jmp common_irq

irq_64:
	pushl $0xFFFFFF9B
	jmp common_irq

irq_65:
	pushl $0xFFFFFF9A
	jmp common_irq

irq_66:
	pushl $0xFFFFFF99
	jmp common_irq
	
irq_67:
	pushl $0xFFFFFF98
	jmp common_irq

irq_68:
	pushl $0xFFFFFF97
	jmp common_irq
	
irq_69:
	pushl $0xFFFFFF96
	jmp common_irq
	
irq_6A:
	pushl $0xFFFFFF95
	jmp common_irq
	
irq_6B:
	pushl $0xFFFFFF94
	jmp common_irq
	
irq_6C:
	pushl $0xFFFFFF93
	jmp common_irq

irq_6D:
	pushl $0xFFFFFF92
	jmp common_irq

irq_6E:
	pushl $0xFFFFFF91
	jmp common_irq

irq_6F:
	pushl $0xFFFFFF90
	jmp common_irq
	
irq_70:
	pushl $0xFFFFFF8F
	jmp common_irq
	
irq_71:
	pushl $0xFFFFFF8E
	jmp common_irq
	
irq_72:
	pushl $0xFFFFFF8D
	jmp common_irq
	
irq_73:
	pushl $0xFFFFFF8C
	jmp common_irq

irq_74:
	pushl $0xFFFFFF8B
	jmp common_irq

irq_75:
	pushl $0xFFFFFF8A
	jmp common_irq

irq_76:
	pushl $0xFFFFFF89
	jmp common_irq
	
irq_77:
	pushl $0xFFFFFF88
	jmp common_irq

irq_78:
	pushl $0xFFFFFF87
	jmp common_irq
	
irq_79:
	pushl $0xFFFFFF86
	jmp common_irq
	
irq_7A:
	pushl $0xFFFFFF85
	jmp common_irq
	
irq_7B:
	pushl $0xFFFFFF84
	jmp common_irq
	
irq_7C:
	pushl $0xFFFFFF83
	jmp common_irq

irq_7D:
	pushl $0xFFFFFF82
	jmp common_irq

irq_7E:
	pushl $0xFFFFFF81
	jmp common_irq

irq_7F:
	pushl $0xFFFFFF80
	jmp common_irq

irq_80:
	pushl $0xFFFFFF7F
	jmp common_irq
	
irq_81:
	pushl $0xFFFFFF7E
	jmp common_irq
	
irq_82:
	pushl $0xFFFFFF7D
	jmp common_irq
	
irq_83:
	pushl $0xFFFFFF7C
	jmp common_irq

irq_84:
	pushl $0xFFFFFF7B
	jmp common_irq

irq_85:
	pushl $0xFFFFFF7A
	jmp common_irq

irq_86:
	pushl $0xFFFFFF79
	jmp common_irq
	
irq_87:
	pushl $0xFFFFFF78
	jmp common_irq

irq_88:
	pushl $0xFFFFFF77
	jmp common_irq
	
irq_89:
	pushl $0xFFFFFF76
	jmp common_irq
	
irq_8A:
	pushl $0xFFFFFF75
	jmp common_irq
	
irq_8B:
	pushl $0xFFFFFF74
	jmp common_irq
	
irq_8C:
	pushl $0xFFFFFF73
	jmp common_irq

irq_8D:
	pushl $0xFFFFFF72
	jmp common_irq

irq_8E:
	pushl $0xFFFFFF71
	jmp common_irq

irq_8F:
	pushl $0xFFFFFF70
	jmp common_irq
	
irq_90:
	pushl $0xFFFFFF6F
	jmp common_irq
	
irq_91:
	pushl $0xFFFFFF6E
	jmp common_irq
	
irq_92:
	pushl $0xFFFFFF6D
	jmp common_irq
	
irq_93:
	pushl $0xFFFFFF6C
	jmp common_irq

irq_94:
	pushl $0xFFFFFF6B
	jmp common_irq

irq_95:
	pushl $0xFFFFFF6A
	jmp common_irq

irq_96:
	pushl $0xFFFFFF69
	jmp common_irq
	
irq_97:
	pushl $0xFFFFFF68
	jmp common_irq

irq_98:
	pushl $0xFFFFFF67
	jmp common_irq
	
irq_99:
	pushl $0xFFFFFF66
	jmp common_irq
	
irq_9A:
	pushl $0xFFFFFF65
	jmp common_irq
	
irq_9B:
	pushl $0xFFFFFF64
	jmp common_irq
	
irq_9C:
	pushl $0xFFFFFF63
	jmp common_irq

irq_9D:
	pushl $0xFFFFFF62
	jmp common_irq

irq_9E:
	pushl $0xFFFFFF61
	jmp common_irq

irq_9F:
	pushl $0xFFFFFF60
	jmp common_irq
	
irq_A0:
	pushl $0xFFFFFF5F
	jmp common_irq
	
irq_A1:
	pushl $0xFFFFFF5E
	jmp common_irq
	
irq_A2:
	pushl $0xFFFFFF5D
	jmp common_irq
	
irq_A3:
	pushl $0xFFFFFF5C
	jmp common_irq

irq_A4:
	pushl $0xFFFFFF5B
	jmp common_irq

irq_A5:
	pushl $0xFFFFFF5A
	jmp common_irq

irq_A6:
	pushl $0xFFFFFF59
	jmp common_irq
	
irq_A7:
	pushl $0xFFFFFF58
	jmp common_irq

irq_A8:
	pushl $0xFFFFFF57
	jmp common_irq
	
irq_A9:
	pushl $0xFFFFFF56
	jmp common_irq
	
irq_AA:
	pushl $0xFFFFFF55
	jmp common_irq
	
irq_AB:
	pushl $0xFFFFFF54
	jmp common_irq
	
irq_AC:
	pushl $0xFFFFFF53
	jmp common_irq

irq_AD:
	pushl $0xFFFFFF52
	jmp common_irq

irq_AE:
	pushl $0xFFFFFF51
	jmp common_irq

irq_AF:
	pushl $0xFFFFFF50
	jmp common_irq
	
irq_B0:
	pushl $0xFFFFFF4F
	jmp common_irq
	
irq_B1:
	pushl $0xFFFFFF4E
	jmp common_irq
	
irq_B2:
	pushl $0xFFFFFF4D
	jmp common_irq
	
irq_B3:
	pushl $0xFFFFFF4C
	jmp common_irq

irq_B4:
	pushl $0xFFFFFF4B
	jmp common_irq

irq_B5:
	pushl $0xFFFFFF4A
	jmp common_irq

irq_B6:
	pushl $0xFFFFFF49
	jmp common_irq
	
irq_B7:
	pushl $0xFFFFFF48
	jmp common_irq

irq_B8:
	pushl $0xFFFFFF47
	jmp common_irq
	
irq_B9:
	pushl $0xFFFFFF46
	jmp common_irq
	
irq_BA:
	pushl $0xFFFFFF45
	jmp common_irq
	
irq_BB:
	pushl $0xFFFFFF44
	jmp common_irq
	
irq_BC:
	pushl $0xFFFFFF43
	jmp common_irq

irq_BD:
	pushl $0xFFFFFF42
	jmp common_irq

irq_BE:
	pushl $0xFFFFFF41
	jmp common_irq

irq_BF:
	pushl $0xFFFFFF40
	jmp common_irq
	
irq_C0:
	pushl $0xFFFFFF3F
	jmp common_irq
	
irq_C1:
	pushl $0xFFFFFF3E
	jmp common_irq
	
irq_C2:
	pushl $0xFFFFFF3D
	jmp common_irq
	
irq_C3:
	pushl $0xFFFFFF3C
	jmp common_irq

irq_C4:
	pushl $0xFFFFFF3B
	jmp common_irq

irq_C5:
	pushl $0xFFFFFF3A
	jmp common_irq

irq_C6:
	pushl $0xFFFFFF39
	jmp common_irq
	
irq_C7:
	pushl $0xFFFFFF38
	jmp common_irq

irq_C8:
	pushl $0xFFFFFF37
	jmp common_irq
	
irq_C9:
	pushl $0xFFFFFF36
	jmp common_irq
	
irq_CA:
	pushl $0xFFFFFF35
	jmp common_irq
	
irq_CB:
	pushl $0xFFFFFF34
	jmp common_irq
	
irq_CC:
	pushl $0xFFFFFF33
	jmp common_irq

irq_CD:
	pushl $0xFFFFFF32
	jmp common_irq

irq_CE:
	pushl $0xFFFFFF31
	jmp common_irq

irq_CF:
	pushl $0xFFFFFF30
	jmp common_irq
	
irq_D0:
	pushl $0xFFFFFF2F
	jmp common_irq
	
irq_D1:
	pushl $0xFFFFFF2E
	jmp common_irq
	
irq_D2:
	pushl $0xFFFFFF2D
	jmp common_irq
	
irq_D3:
	pushl $0xFFFFFF2C
	jmp common_irq

irq_D4:
	pushl $0xFFFFFF2B
	jmp common_irq

irq_D5:
	pushl $0xFFFFFF2A
	jmp common_irq

irq_D6:
	pushl $0xFFFFFF29
	jmp common_irq
	
irq_D7:
	pushl $0xFFFFFF28
	jmp common_irq

irq_D8:
	pushl $0xFFFFFF27
	jmp common_irq
	
irq_D9:
	pushl $0xFFFFFF26
	jmp common_irq
	
irq_DA:
	pushl $0xFFFFFF25
	jmp common_irq
	
irq_DB:
	pushl $0xFFFFFF24
	jmp common_irq
	
irq_DC:
	pushl $0xFFFFFF23
	jmp common_irq

irq_DD:
	pushl $0xFFFFFF22
	jmp common_irq

irq_DE:
	pushl $0xFFFFFF21
	jmp common_irq

irq_DF:
	pushl $0xFFFFFF20
	jmp common_irq
	
irq_E0:
	pushl $0xFFFFFF1F
	jmp common_irq
	
irq_E1:
	pushl $0xFFFFFF1E
	jmp common_irq
	
irq_E2:
	pushl $0xFFFFFF1D
	jmp common_irq
	
irq_E3:
	pushl $0xFFFFFF1C
	jmp common_irq

irq_E4:
	pushl $0xFFFFFF1B
	jmp common_irq

irq_E5:
	pushl $0xFFFFFF1A
	jmp common_irq

irq_E6:
	pushl $0xFFFFFF19
	jmp common_irq
	
irq_E7:
	pushl $0xFFFFFF18
	jmp common_irq

irq_E8:
	pushl $0xFFFFFF17
	jmp common_irq
	
irq_E9:
	pushl $0xFFFFFF16
	jmp common_irq
	
irq_EA:
	pushl $0xFFFFFF15
	jmp common_irq
	
irq_EB:
	pushl $0xFFFFFF14
	jmp common_irq
	
irq_EC:
	pushl $0xFFFFFF13
	jmp common_irq

irq_ED:
	pushl $0xFFFFFF12
	jmp common_irq

irq_EE:
	pushl $0xFFFFFF11
	jmp common_irq

irq_EF:
	pushl $0xFFFFFF10
	jmp common_irq
	
irq_F0:
	pushl $0xFFFFFFEF
	jmp common_irq
	
irq_F1:
	pushl $0xFFFFFFEE
	jmp common_irq
	
irq_F2:
	pushl $0xFFFFFFED
	jmp common_irq
	
irq_F3:
	pushl $0xFFFFFFEC
	jmp common_irq

irq_F4:
	pushl $0xFFFFFFEB
	jmp common_irq

irq_F5:
	pushl $0xFFFFFFEA
	jmp common_irq

irq_F6:
	pushl $0xFFFFFFE9
	jmp common_irq
	
irq_F7:
	pushl $0xFFFFFFE8
	jmp common_irq

irq_F8:
	pushl $0xFFFFFFE7
	jmp common_irq
	
irq_F9:
	pushl $0xFFFFFFE6
	jmp common_irq
	
irq_FA:
	pushl $0xFFFFFFE5
	jmp common_irq
	
irq_FB:
	pushl $0xFFFFFFE4
	jmp common_irq
	
irq_FC:
	pushl $0xFFFFFFE3
	jmp common_irq

irq_FD:
	pushl $0xFFFFFFE2
	jmp common_irq

irq_FE:
	pushl $0xFFFFFFE1
	jmp common_irq

irq_FF:
	pushl $0xFFFFFFE0
	jmp common_irq
