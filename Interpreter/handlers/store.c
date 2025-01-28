#include <stdint.h>
#include <assert.h>

#include "../vm.h"
#include "il.h"

void VM_Handler_STORE(struct IL_VirtualMachine* vm, struct IL_Code* code) {
	assert(IL_GetCodeOperandCount(code) == 2);

	struct IL_Operand* op0 = IL_GetCodeOperand(code, 0);
	struct IL_Operand* op1 = IL_GetCodeOperand(code, 1);

	uint64_t address = 0;
	switch (IL_GetOperandType(op0)) {
	case IL_OPERAND_TYPE_REGISTER: {
		struct IL_OperandRegister* reg0 = IL_GetOperandRegister(op0);
		VM_ReadRegisterValue(vm, reg0->id, &address, reg0->size);
		break;
	}
	case IL_OPERAND_TYPE_IMMEDIATE: {
		uint8_t op0_size = IL_GetOperandDataSize(op0);
		IL_ReadOperandData(op0, &address, op0_size);
		break;
	}
	}

	switch (IL_GetOperandType(op1)) {
	case IL_OPERAND_TYPE_REGISTER: {
		uint64_t value = 0;
		struct IL_OperandRegister* reg1 = IL_GetOperandRegister(op1);
		VM_ReadRegisterValue(vm, reg1->id, &value, reg1->size);
		VM_WriteMemoryValue(vm, address, &value, reg1->size);
		break;
	}
	case IL_OPERAND_TYPE_IMMEDIATE: {
		uint64_t value = 0;
		uint8_t op1_size = IL_GetOperandDataSize(op1);
		IL_ReadOperandData(op1, &value, op1_size);
		VM_WriteMemoryValue(vm, address, &value, op1_size);
		break;
	}
	}
}