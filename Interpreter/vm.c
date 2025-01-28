#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

#include "il.h"
#include "handlers.h"

bool VM_HasConditions(struct IL_VirtualMachine* vm, struct IL_Code* code) {
	if (!IL_HasCodeConditions(code)) {
		return true;
	}

	for (int condition = 0; condition < IL_CONDITIONS_COUNT; ++condition) {
		enum IL_Conditions cond = 1 << condition;
		if (IL_HasCondition(code->conditions, cond) && !IL_HasCondition(vm->conditions, cond)) {
			return false;
		}
	}

	return true;
}

void VM_Run(struct IL_VirtualMachine* vm) {
	while (!IL_HasCondition(vm->conditions, IL_CONDITIONS_HLT)) {
		struct IL_Code* code = (struct IL_Code*)vm->ip;
		if (IL_IsBadCode(code)) {
			printf("Bad code at %llx\n", vm->ip);
			break;
		}

		const char* formated = IL_FormatCode(code);
		printf("%p: %s:", code, formated);
		free((void*)formated);

		if (VM_HasConditions(vm, code)) {
			VM_HANDLERS[code->mnemonic](vm, code);
			// VM_PrintContext(vm);
			printf("\n");
		}
		else {
			printf("(Skipped)\n");
		}

		vm->ip += IL_GetCodeSize(code);
	}
}

void VM_Init(struct IL_VirtualMachine* vm) {
	memset(vm->regs, 0, sizeof(vm->regs));

	vm->ip = 0;
	vm->conditions = IL_CONDITIONS_NONE;
}

void VM_PrintContext(struct IL_VirtualMachine* vm) {
	printf("============== VM CONTEXT ==============\n");

	for (uint8_t i = 0; i < 16; ++i) {
		struct IL_OperandRegister reg;
		reg.id = i;
		reg.size = 8;

		printf("%s: %llx (%ju)", IL_FormatRegister(reg), vm->regs[i], vm->regs[i]);
		if (i == IL_CD_REG) {
			const char* conditions = IL_FormatConditions(vm->conditions);
			printf(" (%s)", conditions);
			free((void*)conditions);
		}

		printf("\n");
	}

	printf("=======================================\n");
}

void VM_WriteRegisterValue(struct IL_VirtualMachine* vm, uint8_t reg_id, void* data, size_t size) {
	assert(size <= sizeof(uint64_t));
	memcpy(&vm->regs[reg_id], data, size);
}

void VM_ReadRegisterValue(struct IL_VirtualMachine* vm, uint8_t reg_id, void* data, size_t size) {
	assert(size <= sizeof(uint64_t));
	memcpy(data, &vm->regs[reg_id], size);
}

void VM_ReadOperandValue(struct IL_VirtualMachine* vm, struct IL_Operand* op, void* data, size_t size) {
	assert(data != NULL);

	void* value = NULL;
	size_t value_size = 0;
	switch (op->type) {
	case IL_OPERAND_TYPE_IMMEDIATE: {
		value_size = IL_GetOperandDataSize(op);
		assert(value_size == 1 || value_size == 2 || value_size == 4 || value_size == 8);

		value = malloc(value_size);
		assert(value != NULL);

		IL_ReadOperandData(op, value, (uint8_t)value_size);
		break;
	}
	case IL_OPERAND_TYPE_REGISTER: {
		struct IL_OperandRegister* reg = IL_GetOperandRegister(op);
		value_size = reg->size;
		value = malloc(value_size);
		assert(value != NULL);

		VM_ReadRegisterValue(vm, reg->id, value, value_size);
		break;
	}
	}

	memcpy(data, value, value_size);
	free((void*)value);
}

void VM_WriteOperandValue(struct IL_VirtualMachine* vm, struct IL_Operand* op, void* data, size_t size) {
	switch (op->type) {
	case IL_OPERAND_TYPE_IMMEDIATE: {
		assert(false);
	}
	case IL_OPERAND_TYPE_REGISTER: {
		struct IL_OperandRegister* reg = IL_GetOperandRegister(op);
		VM_WriteRegisterValue(vm, reg->id, data, reg->size);
		break;
	}
	}
}

void VM_WriteMemoryValue(struct IL_VirtualMachine* vm, uint64_t address, void* data, size_t size) {
	memcpy((void*)address, data, size);
}

void VM_ReadMemoryValue(struct IL_VirtualMachine* vm, uint64_t address, void* data, size_t size) {
	memcpy(data, (void*)address, size);
}