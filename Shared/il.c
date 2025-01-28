#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "il.h"

bool IL_HasCondition(enum IL_Conditions conditions, enum IL_Conditions condition) {
	return conditions & condition;
}

void IL_ToggleCondition(enum IL_Conditions* conditions, enum IL_Conditions condition, bool value) {
	if (value) {
		*conditions |= condition;
	}
	else {
		*conditions &= ~condition;
	}
}

bool IL_IsBadMnemonic(enum IL_Mnemonic mnemonic) {
	return mnemonic >= IL_MNEMONIC_COUNT;
}

bool IL_IsBadCode(struct IL_Code* code) {
	return IL_IsBadMnemonic(code->mnemonic);	
}

const char* IL_FormatMnemonic(enum IL_Mnemonic mnemonic) {
	return IL_MNEMONICS_STR[mnemonic];
}

const char* IL_FormatCondition(enum IL_Conditions condition) {
	int index = 0;
	while (condition >>= 1) {
		++index;
	}

	return IL_CONDITIONS_STR[index];
}

const char* IL_FormatRegister(struct IL_OperandRegister reg) {
	const char* reg_str = IL_REGISTERS_STR[reg.id];

	if (reg.size != 8) {
		// 2 for "." and number, last for null terminator
		size_t len = strlen(reg_str) + 3;
		const char* buffer = calloc(len, 1);
		assert(buffer != NULL);

		sprintf_s(buffer, len, "%s.%d", reg_str, reg.size);
		return buffer;
	}
	else {
		return _strdup(reg_str);
	}
}

const char* IL_FormatConditions(enum IL_Conditions conditions) {
	size_t len = 0;

	int condition_count = 0;
	for (int i = 0; i < IL_CONDITIONS_COUNT; ++i) {
		enum IL_Conditions condition = 1 << i;
		if (IL_HasCondition(conditions, condition)) {
			++condition_count;
		}
	}

	for (int i = 0, used_conditions = 0; i < IL_CONDITIONS_COUNT; ++i) {
		enum IL_Conditions condition = 1 << i;
		if (IL_HasCondition(conditions, condition)) {
			const char* condition_str = IL_FormatCondition(condition);
			len += strlen(condition_str);
			// "."
			if (used_conditions != condition_count - 1) {
				len += 1;
			}

			used_conditions += 1;
		}
	}

	size_t buf_size = len + 1;
	size_t buf_used = 0;
	char* buffer = calloc(buf_size, 1);
	assert(buffer != NULL);

	for (int i = 0, used_conditions = 0; i < IL_CONDITIONS_COUNT; ++i) {
		enum IL_Conditions condition = 1 << i;
		if (IL_HasCondition(conditions, condition)) {
			const char* condition_str = IL_FormatCondition(condition);
			strcat_s(buffer, buf_size, condition_str);
			buf_used += strlen(condition_str);

			if (used_conditions != condition_count - 1) {
				strcat_s(buffer, buf_size, ".");
				buf_used += 1;
			}

			used_conditions += 1;
		}
	}

	assert(buf_used == len);
	return buffer;
}

const char* IL_FormatOperand(struct IL_Operand* operand) {
	switch (operand->type) {
	case IL_OPERAND_TYPE_IMMEDIATE: {
		switch (IL_GetOperandDataSize(operand)) {
		case 1: {
			uint8_t value = 0;
			IL_ReadOperandData(operand, &value, sizeof(value));

			char buffer[3];
			sprintf_s(buffer, sizeof(buffer), "%02x", value);
			return _strdup(buffer);
		}
		case 2: {
			uint16_t value = 0;
			IL_ReadOperandData(operand, &value, sizeof(value));

			char buffer[5];
			sprintf_s(buffer, sizeof(buffer), "%04x", value);
			return _strdup(buffer);
		}
		case 4: {
			uint32_t value = 0;
			IL_ReadOperandData(operand, &value, sizeof(value));

			char buffer[9];
			sprintf_s(buffer, sizeof(buffer), "%08x", value);
			return _strdup(buffer);
		}
		case 8: {
			uint64_t value = 0;
			IL_ReadOperandData(operand, &value, sizeof(value));

			char buffer[17];
			sprintf_s(buffer, sizeof(buffer), "%016llx", value);
			return _strdup(buffer);
		}
		}
	}
	case IL_OPERAND_TYPE_REGISTER: {
		struct IL_OperandRegister* reg = IL_GetOperandRegister(operand);
		return IL_FormatRegister(*reg);
	}
	}

	assert(false);
	return NULL;
}

const char* IL_FormatOperands(struct IL_Code* code) {
	size_t len = 0;	

	uint8_t op_count = IL_GetCodeOperandCount(code);

	for (uint8_t i = 0; i < op_count; ++i) {
		struct IL_Operand* op = IL_GetCodeOperand(code, i);
		const char* op_str = IL_FormatOperand(op);
		len += strlen(op_str);

		if (i != op_count - 1) {
			len += 2; // ", "
		}

		free((void*)op_str);
	}

	size_t buf_size = len + 1;
	size_t buf_used = 0;
	char* buffer = calloc(buf_size, 1);
	assert(buffer != NULL);

	for (uint8_t i = 0; i < op_count; ++i) {
		struct IL_Operand* op = IL_GetCodeOperand(code, i);
		const char* op_str = IL_FormatOperand(op);
		strcat_s(buffer, buf_size, op_str);
		buf_used += strlen(op_str);

		if (i != op_count - 1) {
			strcat_s(buffer, buf_size, ", ");
			buf_used += 2;
		}

		free((void*)op_str);
	}

	assert(buf_used == len);
	return buffer;

}

size_t IL_GetOperandSize(const struct IL_Operand* operand) {
	return sizeof(struct IL_Operand) + IL_GetOperandDataSize(operand);
}

struct IL_Operand* IL_GetNextOperand(struct IL_Operand* operand) {
	size_t op_size = IL_GetOperandSize(operand);
	return (struct IL_Operand*)((uint64_t)operand + op_size);
}

struct IL_Operand* IL_GetCodeOperand(struct IL_Code* code, uint8_t index) {
	assert(index < code->operand_count);

	struct IL_Operand* op = (struct IL_Operand*)(code + 1);
	for (uint8_t i = 0; i < index; ++i) {
		op = IL_GetNextOperand(op);
	}

	return op;
}

const char* IL_FormatCode(struct IL_Code* code) {
	size_t len = 0;
	
	enum IL_Mnemonic mnemonic = IL_GetCodeMnemonic(code);
	const char* mnemonic_str = IL_FormatMnemonic(mnemonic);
	len += strlen(mnemonic_str);

	const char* conditions_str = NULL;
	if (IL_HasCodeConditions(code)) {
		enum IL_Conditions conditions = IL_GetCodeConditions(code);
		conditions_str = IL_FormatConditions(conditions);
		len += strlen(conditions_str);
		len += 2; // "()"
	}

	const char* operands_str = NULL;
	if (IL_HasCodeOperands(code)) {
		len += 1;
		operands_str = IL_FormatOperands(code);
		len += strlen(operands_str);
	}

	size_t buf_size = len + 1;
	size_t buf_used = 0;

	char* buffer = calloc(buf_size, 1);
	assert(buffer != NULL);

	strcat_s(buffer, buf_size, mnemonic_str);
	buf_used += strlen(mnemonic_str);

	if (conditions_str) {
		strcat_s(buffer, buf_size, "(");
		buf_used += 1;

		strcat_s(buffer, buf_size, conditions_str);
		buf_used += strlen(conditions_str);

		strcat_s(buffer, buf_size, ")");
		buf_used += 1;

		free((void*)conditions_str);
	}

	if (operands_str) {
		strcat_s(buffer, buf_size, " ");
		buf_used += 1;

		strcat_s(buffer, buf_size, operands_str);
		buf_used += strlen(operands_str);
		free((void*)operands_str);
	}

	assert(buf_used == len);
	return buffer;
}

enum IL_OperandType IL_GetOperandType(struct IL_Operand* operand) {
	return operand->type;
}

void IL_SetOperandType(struct IL_Operand* operand, enum IL_OperandType type) {
	operand->type = type;
}

void IL_SetOperandDataSize(struct IL_Operand* operand, uint8_t size) {
	operand->size = size;
}

uint8_t IL_GetOperandDataSize(struct IL_Operand* operand) {
	return operand->size;
}

void IL_ReadOperandData(struct IL_Operand* operand, void* data, uint8_t size) {
	assert(size == operand->size);

	void* data_start = (void*)((uint64_t)operand + sizeof(struct IL_Operand));
	memcpy(data, data_start, size);
}

void IL_WriteOperandData(struct IL_Operand* operand, void* data, uint8_t size) {
	assert(size == operand->size);

	void* data_start = (void*)((uint64_t)operand + sizeof(struct IL_Operand));
	memcpy(data_start, data, size);
}

struct IL_OperandRegister* IL_GetOperandRegister(struct IL_Operand* operand) {
	assert(operand->type == IL_OPERAND_TYPE_REGISTER);
	return (struct IL_OperandRegister*)((uint64_t)operand + sizeof(struct IL_Operand));
}

struct IL_Operand* IL_CreateOperandRegister(uint8_t reg_id, uint8_t reg_size) { 
	struct IL_OperandRegister reg = { reg_id, reg_size };

	struct IL_Operand* operand = malloc(sizeof(struct IL_Operand) + sizeof(reg));
	assert(operand != NULL);

	IL_SetOperandType(operand, IL_OPERAND_TYPE_REGISTER);
	IL_SetOperandDataSize(operand, sizeof(reg));
	IL_WriteOperandData(operand, &reg, sizeof(reg));
	return operand;
}

struct IL_Operand* IL_CreateOperandImmediate(void* data, uint8_t size) {
	struct IL_Operand* operand = malloc(sizeof(struct IL_Operand) + size);
	assert(operand != NULL);

	IL_SetOperandType(operand, IL_OPERAND_TYPE_IMMEDIATE);
	IL_SetOperandDataSize(operand, size);
	IL_WriteOperandData(operand, data, size);
	return operand;
}

void IL_SetCodeMnemonic(struct IL_Code* code, enum IL_Mnemonic mnemonic) {
	code->mnemonic = mnemonic;
}

void IL_SetCodeConditions(struct IL_Code* code, enum IL_Conditions conditions) {
	code->conditions = conditions;
}

enum IL_Mnemonic IL_GetCodeMnemonic(struct IL_Code* code) {
	return code->mnemonic;
}

enum IL_Conditions IL_GetCodeConditions(struct IL_Code* code) {
	return code->conditions;
}

bool IL_HasCodeConditions(struct IL_Code* code) {
	return code->conditions != IL_CONDITIONS_NONE;
}

bool IL_HasCodeOperands(struct IL_Code* code) {
	return code->operand_count > 0;
}

uint8_t IL_GetCodeOperandCount(struct IL_Code* code) {
	return (uint8_t)code->operand_count;
}

void IL_SetCodeOperandCount(struct IL_Code* code, uint8_t count) {
	code->operand_count = count;	
}

size_t IL_GetCodeSize(struct IL_Code* code) {
	size_t code_size = 0;
	if (IL_HasCodeOperands(code)) {
		struct IL_Operand* last_op = IL_GetCodeOperand(code, code->operand_count - 1);
		size_t last_op_size = IL_GetOperandSize(last_op);

		code_size = (uint64_t)last_op + last_op_size - (uint64_t)code;
	}
	else {
		code_size += sizeof(struct IL_Code);
	}

	return code_size;
}

struct IL_Code* IL_GetNextCode(struct IL_Code* code) {
	size_t code_size = IL_GetCodeSize(code);
	return (struct IL_Code*)((uint64_t)code + code_size);
}

void IL_AppendCodeOperand(struct IL_Code* code, const struct IL_Operand* operand) {
	uint8_t op_count = IL_GetCodeOperandCount(code);
	op_count += 1;
	IL_SetCodeOperandCount(code, op_count);

	struct IL_Operand* new_op = IL_GetCodeOperand(code, op_count - 1);

	size_t op_size = IL_GetOperandSize(operand);
	memcpy(new_op, operand, op_size);
}