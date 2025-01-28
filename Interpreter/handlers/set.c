#include <stdint.h>
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>

#include "../vm.h"
#include "il.h"

void VM_Handler_SET(struct IL_VirtualMachine* vm, struct IL_Code* code) {
	assert(IL_GetCodeOperandCount(code) == 2);

	struct IL_Operand* op0 = IL_GetCodeOperand(code, 0);
	assert(IL_GetOperandType(op0) == IL_OPERAND_TYPE_REGISTER);

	struct IL_Operand* op1 = IL_GetCodeOperand(code, 1);
	
	struct IL_OperandRegister* reg0 = IL_GetOperandRegister(op0);
	uint8_t reg0_size = reg0->size;

	uint64_t value = 0;
	switch (IL_GetOperandType(op1)) {
	case IL_OPERAND_TYPE_REGISTER: {
		struct IL_OperandRegister* reg1 = IL_GetOperandRegister(op1);
		uint8_t data_size = min(reg1->size, reg0_size);
		VM_ReadRegisterValue(vm, reg1->id, &value, data_size);
		break;
	}
	case IL_OPERAND_TYPE_IMMEDIATE: {
		uint8_t op1_size = IL_GetOperandDataSize(op1);
		uint8_t data_size = min(op1_size, reg0_size);
		IL_ReadOperandData(op1, &value, data_size);
		break;
	}
	default:
		assert(false);
	}

	VM_WriteOperandValue(vm, op0, &value, reg0_size);
}