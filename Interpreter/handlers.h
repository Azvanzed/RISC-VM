#pragma once

#include "il.h"
#include "vm.h"

typedef void (*VM_HandlerFn_t)(struct IL_VirtualMachine* vm, struct IL_Code* code);

void VM_Handler_SET(struct IL_VirtualMachine* vm, struct IL_Code* code);
void VM_Handler_ADD(struct IL_VirtualMachine* vm, struct IL_Code* code);
void VM_Handler_SUB(struct IL_VirtualMachine* vm, struct IL_Code* code);
void VM_Handler_CMP(struct IL_VirtualMachine* vm, struct IL_Code* code);
void VM_Handler_LOAD(struct IL_VirtualMachine* vm, struct IL_Code* code);
void VM_Handler_STORE(struct IL_VirtualMachine* vm, struct IL_Code* code);
void VM_Handler_BRANCH(struct IL_VirtualMachine* vm, struct IL_Code* code);
void VM_Handler_MUL(struct IL_VirtualMachine* vm, struct IL_Code* code);
void VM_Handler_AND(struct IL_VirtualMachine* vm, struct IL_Code* code);
void VM_Handler_OR(struct IL_VirtualMachine* vm, struct IL_Code* code);
void VM_Handler_XOR(struct IL_VirtualMachine* vm, struct IL_Code* code);
void VM_Handler_NOT(struct IL_VirtualMachine* vm, struct IL_Code* code);
void VM_Handler_SHIFTR(struct IL_VirtualMachine* vm, struct IL_Code* code);
void VM_Handler_SHIFTL(struct IL_VirtualMachine* vm, struct IL_Code* code);
void VM_Handler_PUSH(struct IL_VirtualMachine* vm, struct IL_Code* code);
void VM_Handler_POP(struct IL_VirtualMachine* vm, struct IL_Code* code);
void VM_Handler_CALL(struct IL_VirtualMachine* vm, struct IL_Code* code);
void VM_Handler_RETURN(struct IL_VirtualMachine* vm, struct IL_Code* code);
void VM_Handler_HALT(struct IL_VirtualMachine* vm, struct IL_Code* code);

const VM_HandlerFn_t VM_HANDLERS[] = {
	[IL_MNEMONIC_SET] = VM_Handler_SET,
	[IL_MNEMONIC_ADD] = VM_Handler_ADD,
	[IL_MNEMONIC_SUB] = VM_Handler_SUB,
	[IL_MNEMONIC_CMP] = VM_Handler_CMP,
	[IL_MNEMONIC_LOAD] = VM_Handler_LOAD,
	[IL_MNEMONIC_STORE] = VM_Handler_STORE,
	[IL_MNEMONIC_BRANCH] = VM_Handler_BRANCH,
	[IL_MNEMONIC_MUL] = VM_Handler_MUL,
	[IL_MNEMONIC_AND] = VM_Handler_AND,
	[IL_MNEMONIC_OR] = VM_Handler_OR,
	[IL_MNEMONIC_XOR] = VM_Handler_XOR,
	[IL_MNEMONIC_NOT] = VM_Handler_NOT,
	[IL_MNEMONIC_SHIFTR] = VM_Handler_SHIFTR,
	[IL_MNEMONIC_SHIFTL] = VM_Handler_SHIFTL,
	[IL_MNEMONIC_PUSH] = VM_Handler_PUSH,
	[IL_MNEMONIC_POP] = VM_Handler_POP,
	[IL_MNEMONIC_CALL] = VM_Handler_CALL,
	[IL_MNEMONIC_RETURN] = VM_Handler_RETURN,
	[IL_MNEMONIC_HALT] = VM_Handler_HALT,
};