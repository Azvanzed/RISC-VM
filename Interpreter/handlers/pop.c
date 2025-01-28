#include <stdint.h>
#include <assert.h>
#include <stdlib.h>

#include "../vm.h"
#include "il.h"

void VM_Handler_POP(struct IL_VirtualMachine* vm, struct IL_Code* code) {
	assert(IL_GetCodeOperandCount(code) == 1);

	struct IL_Operand* op = IL_GetCodeOperand(code, 0);
	assert(IL_GetOperandType(op) == IL_OPERAND_TYPE_REGISTER);

	struct IL_OperandRegister* reg = IL_GetOperandRegister(op);
	uint8_t size = reg->size;

	uint64_t value = 0;
	VM_ReadMemoryValue(vm, vm->sp, &value, size);
	vm->sp += size;

	VM_WriteRegisterValue(vm, reg->id, &value, size);
}