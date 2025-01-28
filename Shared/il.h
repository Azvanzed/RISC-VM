#pragma once

#include <stdbool.h>
#include <stdint.h>

#define IL_SP_REG	13
#define IL_IP_REG	14
#define IL_CD_REG	15

enum IL_Mnemonic {
	IL_MNEMONIC_SET = 0,
	IL_MNEMONIC_ADD,
	IL_MNEMONIC_SUB,
	IL_MNEMONIC_CMP,
	IL_MNEMONIC_LOAD,
	IL_MNEMONIC_STORE,
	IL_MNEMONIC_BRANCH,
	IL_MNEMONIC_MUL,
	IL_MNEMONIC_AND, 
	IL_MNEMONIC_OR,
	IL_MNEMONIC_XOR,
	IL_MNEMONIC_NOT, 
	IL_MNEMONIC_SHIFTR,
	IL_MNEMONIC_SHIFTL,
	IL_MNEMONIC_PUSH,
	IL_MNEMONIC_POP,
	IL_MNEMONIC_CALL,
	IL_MNEMONIC_RETURN,
	IL_MNEMONIC_HALT,
};

#define IL_MNEMONIC_COUNT (IL_MNEMONIC_HALT + 1)

static const char* IL_MNEMONICS_STR[] = {
	"SET",
	"ADD",
	"SUB",
	"CMP",
	"LOAD",
	"STORE",
	"BRANCH",
	"MUL",
	"AND",
	"OR",
	"XOR",
	"NOT",
	"SHIFTR",
	"SHIFTL",
	"PUSH",
	"POP",
	"CALL",
	"RETURN",
	"HALT",
};

enum IL_Conditions {
	IL_CONDITIONS_NONE = 0,
	IL_CONDITIONS_EQ = 1 << 0,
	IL_CONDITIONS_NEQ = 1 << 1,
	IL_CONDITIONS_LT = 1 << 2,
	IL_CONDITIONS_GT = 1 << 3,
	IL_CONDITIONS_HLT = 1 << 4,
};

#define IL_CONDITIONS_COUNT 5

static const char* IL_CONDITIONS_STR[] = {
	"EQ",
	"NEQ",
	"LT",
	"GT",
	"HLT",
};

static const char* IL_REGISTERS_STR[] = {
	"R0", "R1", "R2", "R3", "R4", "R5", "R6", "R7",
	"R8", "R9", "R10", "R11", "R12", "SP", "IP", "CD",
};

#define IL_REGISTERS_COUNT 16

enum IL_OperandType {
	IL_OPERAND_TYPE_IMMEDIATE,
	IL_OPERAND_TYPE_REGISTER,
};

struct IL_OperandRegister {
	uint8_t id : 4;
	uint8_t size : 4;
};

struct IL_Operand {
	uint8_t type : 2;
	uint8_t size : 6;
};

struct IL_Code {
	uint16_t mnemonic : 8;
	uint16_t conditions : 6;
	uint16_t operand_count : 2;
};

#ifdef __cplusplus
extern "C" {
#endif

bool IL_HasCondition(enum IL_Conditions conditions, enum IL_Conditions flag);
void IL_ToggleCondition(enum IL_Conditions* conditions, enum IL_Conditions condition, bool value);
bool IL_IsBadMnemonic(enum IL_Mnemonic mnemonic);
bool IL_IsBadCode(struct IL_Code* code);

const char* IL_FormatMnemonic(enum IL_Mnemonic mnemonic);
const char* IL_FormatCondition(enum IL_Conditions condition);
const char* IL_FormatRegister(struct IL_OperandRegister reg);

const char* IL_FormatConditions(enum IL_Conditions conditions);
const char* IL_FormatOperand(struct IL_Operand* operand);
const char* IL_FormatOperands(struct IL_Code* code);
const char* IL_FormatCode(struct IL_Code* code);

size_t IL_GetOperandSize(const struct IL_Operand* operand);

struct IL_Operand* IL_GetNextOperand(struct IL_Operand* operand);
struct IL_Operand* IL_GetCodeOperand(struct IL_Code* code, uint8_t index);

enum IL_OperandType IL_GetOperandType(struct IL_Operand* operand);
void IL_SetOperandType(struct IL_Operand* operand, enum IL_OperandType type);

void IL_SetOperandDataSize(struct IL_Operand* operand, uint8_t size);
uint8_t IL_GetOperandDataSize(struct IL_Operand* operand);

void IL_ReadOperandData(struct IL_Operand* operand, void* data, uint8_t size);
void IL_WriteOperandData(struct IL_Operand* operand, void* data, uint8_t size);

struct IL_OperandRegister* IL_GetOperandRegister(struct IL_Operand* operand);

struct IL_Operand* IL_CreateOperandRegister(uint8_t reg_id, uint8_t reg_size);
struct IL_Operand* IL_CreateOperandImmediate(void* data, uint8_t size);

void IL_SetCodeMnemonic(struct IL_Code* code, enum IL_Mnemonic mnemonic);
enum IL_Mnemonic IL_GetCodeMnemonic(struct IL_Code* code);

void IL_SetCodeConditions(struct IL_Code* code, enum IL_Conditions conditions);
enum IL_Conditions IL_GetCodeConditions(struct IL_Code* code);

void IL_SetCodeOperandCount(struct IL_Code* code, uint8_t count);
uint8_t IL_GetCodeOperandCount(struct IL_Code* code);

void IL_AppendCodeOperand(struct IL_Code* code, const struct IL_Operand* operand);

bool IL_HasCodeConditions(struct IL_Code* code);
bool IL_HasCodeOperands(struct IL_Code* code);

size_t IL_GetCodeSize(struct IL_Code* code);
struct IL_Code* IL_GetNextCode(struct IL_Code* code);

#ifdef __cplusplus
}
#endif
