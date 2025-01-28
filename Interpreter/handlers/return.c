#include <stdint.h>
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>

#include "../vm.h"
#include "il.h"

void VM_Handler_RETURN(struct IL_VirtualMachine* vm, struct IL_Code* code) {
	// Read the return address that was pushed on the stack by the expected CALL instruction
	uint64_t ip = 0;
	VM_ReadMemoryValue(vm, vm->sp, &ip, sizeof(ip));
	vm->sp += sizeof(ip);

	vm->ip = ip;

	// Disable Next Instruction flag so IP is not incremented
	VM_ToggleCondition(vm, IL_CONDITIONS_NI, false);
}