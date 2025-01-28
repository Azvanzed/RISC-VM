#include <stdint.h>
#include <assert.h>
#include <stdlib.h>

#include "../vm.h"
#include "il.h"

void VM_Handler_RETURN(struct IL_VirtualMachine* vm, struct IL_Code* code) {
	uint64_t ip = 0;
	VM_ReadMemoryValue(vm, vm->sp, &ip, sizeof(ip));
	vm->sp += sizeof(ip);

	// Substract current code size to get the offset
	vm->ip = ip - IL_GetCodeSize(code);
}