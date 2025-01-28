#include <stdint.h>
#include <assert.h>
#include <stdlib.h>

#include "../vm.h"
#include "il.h"

void VM_Handler_NOT(struct IL_VirtualMachine* vm, struct IL_Code* code) {
	assert(IL_GetCodeOperandCount(code) == 1);

	struct IL_Operand* op0 = IL_GetCodeOperand(code, 0);
	assert(IL_GetOperandType(op0) == IL_OPERAND_TYPE_REGISTER);

	struct IL_OperandRegister* reg0 = IL_GetOperandRegister(op0);
	uint8_t reg0_size = reg0->size;

	uint64_t value = 0;
	VM_ReadOperandValue(vm, op0, &value, reg0_size);
	value = ~value;
	VM_WriteOperandValue(vm, op0, &value, reg0_size);
}