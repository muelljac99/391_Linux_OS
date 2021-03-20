
#ifndef _IRQ_ASM_H
#define _IRQ_ASM_H

#include "x86_desc.h"

/* define a type for the function pointer */
typedef void (*irq_handler_ptr)();

/* define all these function pointers externally in the .S file */
extern irq_handler_ptr irq_00, irq_01, irq_02, irq_03, irq_04, irq_05, irq_06, irq_07, irq_08, irq_09, irq_0A, irq_0B, irq_0C, irq_0D, irq_0E, irq_0F;
extern irq_handler_ptr irq_10, irq_11, irq_12, irq_13, irq_14, irq_15, irq_16, irq_17, irq_18, irq_19, irq_1A, irq_1B, irq_1C, irq_1D, irq_1E, irq_1F;
extern irq_handler_ptr irq_20, irq_21, irq_22, irq_23, irq_24, irq_25, irq_26, irq_27, irq_28, irq_29, irq_2A, irq_2B, irq_2C, irq_2D, irq_2E, irq_2F;
extern irq_handler_ptr irq_30, irq_31, irq_32, irq_33, irq_34, irq_35, irq_36, irq_37, irq_38, irq_39, irq_3A, irq_3B, irq_3C, irq_3D, irq_3E, irq_3F;
extern irq_handler_ptr irq_40, irq_41, irq_42, irq_43, irq_44, irq_45, irq_46, irq_47, irq_48, irq_49, irq_4A, irq_4B, irq_4C, irq_4D, irq_4E, irq_4F;
extern irq_handler_ptr irq_50, irq_51, irq_52, irq_53, irq_54, irq_55, irq_56, irq_57, irq_58, irq_59, irq_5A, irq_5B, irq_5C, irq_5D, irq_5E, irq_5F;
extern irq_handler_ptr irq_60, irq_61, irq_62, irq_63, irq_64, irq_65, irq_66, irq_67, irq_68, irq_69, irq_6A, irq_6B, irq_6C, irq_6D, irq_6E, irq_6F;
extern irq_handler_ptr irq_70, irq_71, irq_72, irq_73, irq_74, irq_75, irq_76, irq_77, irq_78, irq_79, irq_7A, irq_7B, irq_7C, irq_7D, irq_7E, irq_7F;
extern irq_handler_ptr irq_80, irq_81, irq_82, irq_83, irq_84, irq_85, irq_86, irq_87, irq_88, irq_89, irq_8A, irq_8B, irq_8C, irq_8D, irq_8E, irq_8F;
extern irq_handler_ptr irq_90, irq_91, irq_92, irq_93, irq_94, irq_95, irq_96, irq_97, irq_98, irq_99, irq_9A, irq_9B, irq_9C, irq_9D, irq_9E, irq_9F;
extern irq_handler_ptr irq_A0, irq_A1, irq_A2, irq_A3, irq_A4, irq_A5, irq_A6, irq_A7, irq_A8, irq_A9, irq_AA, irq_AB, irq_AC, irq_AD, irq_AE, irq_AF;
extern irq_handler_ptr irq_B0, irq_B1, irq_B2, irq_B3, irq_B4, irq_B5, irq_B6, irq_B7, irq_B8, irq_B9, irq_BA, irq_BB, irq_BC, irq_BD, irq_BE, irq_BF;
extern irq_handler_ptr irq_C0, irq_C1, irq_C2, irq_C3, irq_C4, irq_C5, irq_C6, irq_C7, irq_C8, irq_C9, irq_CA, irq_CB, irq_CC, irq_CD, irq_CE, irq_CF;
extern irq_handler_ptr irq_D0, irq_D1, irq_D2, irq_D3, irq_D4, irq_D5, irq_D6, irq_D7, irq_D8, irq_D9, irq_DA, irq_DB, irq_DC, irq_DD, irq_DE, irq_DF;
extern irq_handler_ptr irq_E0, irq_E1, irq_E2, irq_E3, irq_E4, irq_E5, irq_E6, irq_E7, irq_E8, irq_E9, irq_EA, irq_EB, irq_EC, irq_ED, irq_EE, irq_EF;
extern irq_handler_ptr irq_F0, irq_F1, irq_F2, irq_F3, irq_F4, irq_F5, irq_F6, irq_F7, irq_F8, irq_F9, irq_FA, irq_FB, irq_FC, irq_FD, irq_FE, irq_FF;

/* make an array of the ptrs to be used in the idt initialization */
void (*irq_handle[NUM_VEC])();

#endif /* _IRQ_ASM_H */
