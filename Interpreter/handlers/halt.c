#include <stdint.h>

#include "../vm.h"
#include "il.h"

void VM_Handler_HALT(struct IL_VirtualMachine* vm, struct IL_Code* code) {
	VM_ToggleCondition(vm, IL_CONDITIONS_HLT, true);
}