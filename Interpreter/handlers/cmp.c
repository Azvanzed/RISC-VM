#include <stdint.h>
#include <assert.h>

#include "../vm.h"
#include "il.h"

void VM_Handler_CMP(struct IL_VirtualMachine* vm, struct IL_Code* code) {
	assert(IL_GetCodeOperandCount(code) == 2);

	struct IL_Operand* op0 = IL_GetCodeOperand(code, 0);
	struct IL_Operand* op1 = IL_GetCodeOperand(code, 1);

	uint64_t a = 0;
	switch (IL_GetOperandType(op0)) {
	case IL_OPERAND_TYPE_REGISTER: {
		struct IL_OperandRegister* reg0 = IL_GetOperandRegister(op0);
		VM_ReadRegisterValue(vm, reg0->id, &a, reg0->size);
		break;
	}
	case IL_OPERAND_TYPE_IMMEDIATE: {
		uint8_t op0_size = IL_GetOperandDataSize(op0);
		IL_ReadOperandData(op0, &a, op0_size);
		break;
	}
	}

	uint64_t b = 0;
	switch (IL_GetOperandType(op1)) {
	case IL_OPERAND_TYPE_REGISTER: {
		struct IL_OperandRegister* reg1 = IL_GetOperandRegister(op1);
		VM_ReadRegisterValue(vm, reg1->id, &b, reg1->size);
		break;
	}
	case IL_OPERAND_TYPE_IMMEDIATE: {
		uint8_t op1_size = IL_GetOperandDataSize(op1);
		IL_ReadOperandData(op1, &b, op1_size);
		break;
	}
	}

	VM_ToggleCondition(vm, IL_CONDITIONS_EQ, a == b);
	VM_ToggleCondition(vm, IL_CONDITIONS_NEQ, a != b);
	VM_ToggleCondition(vm, IL_CONDITIONS_LT, a < b);
	VM_ToggleCondition(vm, IL_CONDITIONS_GT, a > b);
}