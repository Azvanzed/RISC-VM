	#include <stdint.h>
#include <Windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "vm.h"
#include "il.h"

uint8_t* LoadFile(const char* path) {
	FILE* file = fopen(path, "rb");
	if (!file) {
		return NULL;
	}	

	fseek(file, 0, SEEK_END);	
	size_t size = ftell(file);
	fseek(file, 0, SEEK_SET);

	uint8_t* buffer = (uint8_t*)malloc(size);
	if (!buffer) {
		fclose(file);
		return NULL;
	}

	fread(buffer, 1, size, file);
	fclose(file);

	return buffer;
}

int main(int argc, char* argv[]) {
	if (argc != 2) {
		printf("Usage: %s <input file>\n", argv[0]);
		return EXIT_FAILURE;
	}

	const char* path = argv[1];
	uint8_t* alloc = LoadFile(path);

	void* stack = malloc(4096);

	printf("Alloc: %p\n", alloc);
	printf("Stack: %p\n", stack);

	struct IL_VirtualMachine vm;
	VM_Init(&vm);
	
	vm.ip = (uint64_t)alloc;
	vm.sp = (uint64_t)stack + 4096;

	VM_Run(&vm);
	VM_PrintContext(&vm);

	free(alloc);
	free(stack);
	return EXIT_SUCCESS;
}