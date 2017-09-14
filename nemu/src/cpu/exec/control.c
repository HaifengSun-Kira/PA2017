#include "cpu/exec.h"

make_EHelper(jmp) {
  // the target address is calculated at the decode stage
  decoding.is_jmp = 1;

  print_asm("jmp %x", decoding.jmp_eip);
}

make_EHelper(jcc) {
  // the target address is calculated at the decode stage
  uint8_t subcode = decoding.opcode & 0xf;
  rtl_setcc(&t2, subcode);
  decoding.is_jmp = t2;

  print_asm("j%s %x", get_cc_name(subcode), decoding.jmp_eip);
}

make_EHelper(jmp_rm) {
  decoding.jmp_eip = id_dest->val;
  decoding.is_jmp = 1;

  print_asm("jmp *%s", id_dest->str);
}

make_EHelper(call) {
  // the target address is calculated at the decode stage
  if (decoding.is_operand_size_16) {
	rtl_push(&cpu.eip);
	//cpu.eip = (cpu.eip + id_dest -> simm) & 0x0000ffff;
  } else {
	rtl_push(&cpu.eip);
	//cpu.eip += id_dest -> simm;
  }

  decoding.is_jmp = 1;

  print_asm("call %x", decoding.jmp_eip);
}

make_EHelper(ret) {
  if(decoding.is_operand_size_16) {
	rtl_pop(&cpu.eip);
	cpu.eip = cpu.eip & 0x0000ffff;
  } else {
	rtl_pop(&cpu.eip);
  }
  decoding.is_jmp = 1;
  print_asm("ret");
}

make_EHelper(call_rm) {
  TODO();

  print_asm("call *%s", id_dest->str);
}
