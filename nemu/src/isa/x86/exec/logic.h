#include "cc.h"


static inline def_EHelper(test) {
  rtl_and(s, t0, ddest, dsrc1);
  rtl_update_ZFSF(s, t0, id_dest->width);
  rtl_set_CF(s, rz);
  rtl_set_OF(s, rz);

  print_asm_template2(test);
}

static inline def_EHelper(and) {
  rtl_and(s, ddest, ddest, dsrc1);

  rtl_update_ZFSF(s, ddest, id_dest->width);
  rtl_set_OF(s, rz);
  rtl_set_CF(s, rz);
  operand_write(s, id_dest, ddest);

  print_asm_template2(and);
}

static inline def_EHelper(xor) {
  if (cpu.pc == 0x10d2f3)
    printf("xor 0x%x 0x%x ", *ddest, *dsrc1);
  rtl_xor(s, ddest, ddest, dsrc1);
  if (cpu.pc == 0x10d2f3)
    printf("result 0x%x\n", *ddest);

  rtl_update_ZFSF(s, ddest, id_dest->width);
  rtl_set_CF(s, rz);
  rtl_set_OF(s, rz);
  operand_write(s, id_dest, ddest);

  print_asm_template2(xor);
}

static inline def_EHelper(or) {
  rtl_or(s, ddest, ddest, dsrc1);
  
  rtl_update_ZFSF(s, ddest, id_dest->width);
  rtl_set_OF(s, rz);
  rtl_set_CF(s, rz);
  operand_write(s, id_dest, ddest);
  
  print_asm_template2(or);
}

static inline def_EHelper(sar) {
  *ddest = (signed)*ddest >> *dsrc1;
  operand_write(s, id_dest, ddest);
  rtl_update_ZFSF(s, ddest, id_dest->width);
  // unnecessary to update CF and OF in NEMU

  print_asm_template2(sar);
}

static inline def_EHelper(shl) {
  *ddest = *ddest << *dsrc1;
  operand_write(s, id_dest, ddest);
  rtl_update_ZFSF(s, ddest, id_dest->width);
  // unnecessary to update CF and OF in NEMU

  print_asm_template2(shl);
}

static inline def_EHelper(shr) {
  *ddest = *ddest >> *dsrc1;
  operand_write(s, id_dest, ddest);
  rtl_update_ZFSF(s, ddest, id_dest->width);
  // unnecessary to update CF and OF in NEMU

  print_asm_template2(shr);
}

static inline def_EHelper(shld) {
  *ddest = (*ddest << *dsrc1) + (*dsrc2 >> (id_src1->width * 8 - *dsrc1));
  operand_write(s, id_dest, ddest);
  rtl_update_ZFSF(s, ddest, id_dest->width);
  // unnecessary to update CF and OF in NEMU

  print_asm_template2(shl);
}

static inline def_EHelper(shrd) {
  *ddest = (*ddest >> *dsrc1) + (*dsrc2 << (id_src1->width * 8 - *dsrc1));
  operand_write(s, id_dest, ddest);
  rtl_update_ZFSF(s, ddest, id_dest->width);
  // unnecessary to update CF and OF in NEMU

  print_asm_template2(shr);
}

static inline def_EHelper(rol) {
  *ddest = (*ddest << *dsrc1) + (*ddest >> (id_dest->width * 8 - *dsrc1));
  operand_write(s, id_dest, ddest);
  rtl_update_ZFSF(s, ddest, id_dest->width);

  print_asm_template2(rol);
}

static inline def_EHelper(ror) {
  *ddest = (*ddest >> *dsrc1) + (*ddest << (id_dest->width * 8 - *dsrc1));
  operand_write(s, id_dest, ddest);
  rtl_update_ZFSF(s, ddest, id_dest->width);

  print_asm_template2(ror);
}


static inline def_EHelper(setcc) {
  uint32_t cc = s->opcode & 0xf;
  rtl_setcc(s, ddest, cc);
  operand_write(s, id_dest, ddest);

  print_asm("set%s %s", get_cc_name(cc), id_dest->str);
}

static inline def_EHelper(not) {
  rtl_not(s, ddest, ddest);
  operand_write(s, id_dest, ddest);

  print_asm_template1(not);
}

