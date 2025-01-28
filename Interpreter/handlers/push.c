#include <stdint.h>
#include <assert.h>
#include <stdlib.h>

#include "../vm.h"
#include "il.h"

void VM_Handler_PUSH(struct IL_VirtualMachine* vm, struct IL_Code* code) {
	assert(IL_GetCodeOperandCount(code) == 1);

	struct IL_Operand* op = IL_GetCodeOperand(code, 0);

	uint64_t value = 0;
	uint8_t size = 0;
	switch (IL_GetOperandType(op)) {
	case IL_OPERAND_TYPE_REGISTER: {
		struct IL_OperandRegister* reg = IL_GetOperandRegister(op);
		size = reg->size;
		VM_ReadRegisterValue(vm, reg->id, &value, size);
		break;
	}
	case IL_OPERAND_TYPE_IMMEDIATE: {
		size = IL_GetOperandDataSize(op);
		IL_ReadOperandData(op, &value, size);
		break;
	}
	default:
		assert(false);
	}

	vm->sp -= size;
	VM_WriteMemoryValue(vm, vm->sp, &value, size);
}