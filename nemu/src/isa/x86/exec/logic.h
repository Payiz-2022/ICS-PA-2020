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
  rtl_xor(s, ddest, ddest, dsrc1);

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
  if (*dsrc1 != 0 && *dsrc1 < id_src2->width * 8 && *dsrc1 <= id_dest->width * 8)
    *ddest = (*ddest << *dsrc1) | (*dsrc2 >> (id_src2->width * 8 - *dsrc1));
  operand_write(s, id_dest, ddest);
  rtl_update_ZFSF(s, ddest, id_dest->width);
  // unnecessary to update CF and OF in NEMU

  print_asm_template3(shld);
}

static inline def_EHelper(shrd) {
  if (*dsrc1 != 0 && *dsrc1 < id_src2->width * 8 && *dsrc1 <= id_dest->width * 8)
    *ddest = (*ddest >> *dsrc1) | (*dsrc2 << (id_src2->width * 8 - *dsrc1));
  operand_write(s, id_dest, ddest);
  rtl_update_ZFSF(s, ddest, id_dest->width);
  // unnecessary to update CF and OF in NEMU

  print_asm_template3(shrd);
}

static inline def_EHelper(rol) {
  // printf("rol ddest 0x%x(width %d); dsrc1 0x%x ", *ddest, id_dest->width, *dsrc1, id_src2->width);
  if (*dsrc1 != 0 && *dsrc1 < id_dest->width * 8 && *dsrc1 <= id_dest->width * 8)
    *ddest = (*ddest << *dsrc1) | (*ddest >> (id_dest->width * 8 - *dsrc1));
  // printf("result 0x%x\n", *ddest);
  operand_write(s, id_dest, ddest);
  rtl_update_ZFSF(s, ddest, id_dest->width);

  print_asm_template2(rol);
}

static inline def_EHelper(ror) {
  if (*dsrc1 != 0 && *dsrc1 < id_dest->width * 8 && *dsrc1 <= id_dest->width * 8)
    *ddest = (*ddest >> *dsrc1) | (*ddest << (id_dest->width * 8 - *dsrc1));
  operand_write(s, id_dest, ddest);
  rtl_update_ZFSF(s, ddest, id_dest->width);

  print_asm_template2(ror);
}

static inline def_EHelper(bsf) {
  uint32_t mask = 1;
  int cnt = 0;
  while (*dsrc1) {
    if (mask & *dsrc1) {
      *ddest = cnt;
      rtl_addi(s, s0, rz, 1);
      rtl_update_ZF(s, s0, id_dest->width);
      break;
    }
    cnt++;
    if (cnt >= id_src1->width * 8) {
      rtl_update_ZF(s, rz, id_dest->width);
    }
    mask <<= 1;
  }
  operand_write(s, id_dest, ddest);

  print_asm_template2(bsf);
}

static inline def_EHelper(bsr) {
  uint32_t mask = 1 << (id_src1->width * 8 - 1);
  int cnt = id_src1->width * 8 - 1;
  while (*dsrc1) {
    if (mask & *dsrc1) {
      *ddest = cnt;
      rtl_addi(s, s0, rz, 1);
      rtl_update_ZF(s, s0, id_dest->width);
      break;
    }
    cnt--;
    if (cnt < 0) {
      rtl_update_ZF(s, rz, id_dest->width);
    }
    mask >>= 1;
  }
  operand_write(s, id_dest, ddest);

  print_asm_template2(bsr);
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

