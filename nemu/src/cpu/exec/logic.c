#include "cpu/exec.h"

make_EHelper(test) {
  rtl_and(&t2, &id_dest->val, &id_src->val);

  rtl_update_ZFSF(&t2, id_dest->width);

  rtl_set_CF(&tzero);
  rtl_set_OF(&tzero);

  print_asm_template2(test);
}

make_EHelper(and) {
  rtl_and(&t2, &id_dest->val, &id_src->val);
  operand_write(id_dest, &t2);

  rtl_update_ZFSF(&t2, id_dest->width);

  rtl_set_CF(&tzero);
  rtl_set_OF(&tzero);

  print_asm_template2(and);
}

make_EHelper(xor) {
  rtl_xor(&t2, &id_dest->val, &id_src->val);
  operand_write(id_dest, &t2);

  rtl_update_ZFSF(&t2, id_dest->width);

  rtl_set_CF(&tzero);
  rtl_set_OF(&tzero);
  print_asm_template2(xor);
}

make_EHelper(or) {
  rtl_or(&t2, &id_dest->val, &id_src->val);
  operand_write(id_dest, &t2);

  rtl_update_ZFSF(&t2, id_dest->width);

  rtl_set_CF(&tzero);
  rtl_set_OF(&tzero);

  print_asm_template2(or);
}

make_EHelper(sar) {
  if(id_dest->width == 4) {
	rtl_sar(&t2, &id_dest->val, &id_src->val);
  } else if(id_dest->width == 2) {
	t0 = (int16_t) id_dest->val;
	rtl_sar(&t2, &t0, &id_src->val);
	//t2 = ((int16_t)id_dest->val) >> id_src->val;
  } else {
	t0 = (int8_t) id_dest->val;
	rtl_sar(&t2, &t0, &id_src->val);
	//t2 = ((int8_t)id_dest->val) >> id_src->val;
  }

  operand_write(id_dest, &t2);
  // unnecessary to update CF and OF in NEMU

  print_asm_template2(sar);
}

make_EHelper(shl) {
  rtl_shl(&t2, &id_dest->val, &id_src->val);
  operand_write(id_dest, &t2);
  // unnecessary to update CF and OF in NEMU

  print_asm_template2(shl);
}

make_EHelper(shr) {
  rtl_shr(&t2, &id_dest->val, &id_src->val);
  operand_write(id_dest, &t2);
  // unnecessary to update CF and OF in NEMU

  print_asm_template2(shr);
}

make_EHelper(setcc) {
  uint8_t subcode = decoding.opcode & 0xf;
  rtl_setcc(&t2, subcode);
  operand_write(id_dest, &t2);

  print_asm("set%s %s", get_cc_name(subcode), id_dest->str);
}

make_EHelper(not) {
  t2 = id_dest->val;
  rtl_not(&t2);
  operand_write(id_dest, &t2);

  print_asm_template1(not);
}
