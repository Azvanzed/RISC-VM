#include <stdint.h>
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>

#include "../vm.h"
#include "il.h"

void VM_Handler_CALL(struct IL_VirtualMachine* vm, struct IL_Code* code) {
	assert(IL_GetCodeOperandCount(code) == 1);

	size_t code_size = IL_GetCodeSize(code);

	uint64_t next_ip = vm->ip + code_size;
	vm->sp -= sizeof(next_ip);
	VM_WriteMemoryValue(vm, vm->sp, &next_ip, sizeof(next_ip));

	struct IL_Operand* op = IL_GetCodeOperand(code, 0);

	uint64_t offset = 0;
	switch (IL_GetOperandType(op)) {
	case IL_OPERAND_TYPE_REGISTER: {
		struct IL_OperandRegister* reg = IL_GetOperandRegister(op);
		VM_ReadRegisterValue(vm, reg->id, &offset, reg->size);
		break;
	}
	case IL_OPERAND_TYPE_IMMEDIATE: {
		uint8_t op_size = IL_GetOperandDataSize(op);
		IL_ReadOperandData(op, &offset, op_size);
		break;
	}
	}

	// Offset - current addr since we increment the ip after the switch
	uint64_t branched_ip = offset - code_size;
	vm->ip += branched_ip;
}