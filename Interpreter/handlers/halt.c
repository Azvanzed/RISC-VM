#include <stdint.h>

#include "../vm.h"
#include "il.h"

void VM_Handler_HALT(struct IL_VirtualMachine* vm, struct IL_Code* code) {
	IL_ToggleCondition(&vm->conditions, IL_CONDITIONS_HLT, true);
}