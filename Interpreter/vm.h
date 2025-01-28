#pragma once

#include <stdint.h>

#include "il.h"

struct IL_VirtualMachine {
	union {
		uint64_t regs[16];
		struct {
			uint64_t r0;
			uint64_t r1;
			uint64_t r2;
			uint64_t r3;
			uint64_t r4;
			uint64_t r5;
			uint64_t r6;
			uint64_t r7;
			uint64_t r8;
			uint64_t r9;
			uint64_t r10;
			uint64_t r11;
			uint64_t r12;
			uint64_t sp;
			uint64_t ip;
			enum IL_Conditions conditions;
		};
	};
};

bool VM_HasCodeConditions(struct IL_VirtualMachine* vm, struct IL_Code* code);
bool VM_HasConditions(struct IL_VirtualMachine* vm, enum IL_Conditions conditions);
bool VM_ToggleCondition(struct IL_VirtualMachine* vm, enum IL_Conditions condition, bool value);

void VM_Run(struct IL_VirtualMachine* vm);
void VM_Init(struct IL_VirtualMachine* vm);
void VM_PrintContext(struct IL_VirtualMachine* vm);

void VM_WriteRegisterValue(struct IL_VirtualMachine* vm, uint8_t reg_id, void* data, size_t size);
void VM_ReadRegisterValue(struct IL_VirtualMachine* vm, uint8_t reg_id, void* data, size_t size);

void VM_WriteOperandValue(struct IL_VirtualMachine* vm, struct IL_Operand* op, void* data, size_t size);
void VM_ReadOperandValue(struct IL_VirtualMachine* vm, struct IL_Operand* op, void* data, size_t size);

void VM_WriteMemoryValue(struct IL_VirtualMachine* vm, uint64_t address, void* data, size_t size);
void VM_ReadMemoryValue(struct IL_VirtualMachine* vm, uint64_t address, void* data, size_t size);

