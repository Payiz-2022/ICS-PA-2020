static inline def_EHelper(mov) {
  operand_write(s, id_dest, dsrc1);
  print_asm_template2(mov);
}

static inline def_EHelper(push) {
  rtl_push(s, ddest);

  operand_write(s, id_dest, ddest);
  print_asm_template1(push);
}

static inline def_EHelper(pop) {
  rtl_pop(s, ddest);

  operand_write(s, id_dest, ddest);
  print_asm_template1(pop);
}

static inline def_EHelper(pusha) {
  TODO();

  print_asm("pusha");
}

static inline def_EHelper(popa) {
  TODO();

  print_asm("popa");
}

static inline def_EHelper(leave) {
  rtl_mv(s, &cpu.esp, &cpu.ebp);
  rtl_pop(s, &cpu.ebp);

  print_asm("leave");
}

static inline def_EHelper(cltd) {
  if (s->isa.is_operand_size_16) {
    if (cpu.eax & 0x8000) cpu.edx |= 0xffff;
    else cpu.edx &= 0xffff0000;
  } else {
    if (cpu.eax & 0x80000000) cpu.edx = 0xffffffff;
    else cpu.edx = 0;
  }

  print_asm(s->isa.is_operand_size_16 ? "cwtd" : "cltd");
}

static inline def_EHelper(cwtl) {
  if (s->isa.is_operand_size_16) {
    rtl_lr(s, s0, R_AL, 1);
    rtl_sext(s, s0, s0, 1);
    rtl_sr(s, R_AX, s0, 2);
  }
  else {
    rtl_lr(s, s0, R_AX, 2);
    rtl_sext(s, s0, s0, 2);
    rtl_sr(s, R_EAX, s0, 4);
  }

  print_asm(s->isa.is_operand_size_16 ? "cbtw" : "cwtl");
}

static inline def_EHelper(movsx) {
  id_dest->width = s->isa.is_operand_size_16 ? 2 : 4;
  rtl_sext(s, ddest, dsrc1, id_src1->width);

  operand_write(s, id_dest, ddest);
  print_asm_template2(movsx);
}

static inline def_EHelper(movzx) {
  id_dest->width = s->isa.is_operand_size_16 ? 2 : 4;

  operand_write(s, id_dest, dsrc1);
  print_asm_template2(movzx);
}

static inline def_EHelper(lea) {
  rtl_addi(s, ddest, s->isa.mbase, s->isa.moff);
  
  operand_write(s, id_dest, ddest);
  print_asm_template2(lea);
}

