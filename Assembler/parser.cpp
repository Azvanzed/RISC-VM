#include <vector>
#include <mutex>
#include <memory>
#include <unordered_map>
#include <cassert>
#include <algorithm>

#include "tokenizer.hpp"
#include "il.h"
#include "parser.hpp"
#include <iostream>

const std::unordered_map<std::string, IL_Mnemonic> MNEMONICS_MAP = {
	{ "SET", IL_MNEMONIC_SET },
	{ "ADD", IL_MNEMONIC_ADD },
	{ "SUB", IL_MNEMONIC_SUB },
	{ "CMP", IL_MNEMONIC_CMP },
	{ "LOAD", IL_MNEMONIC_LOAD },
	{ "STORE", IL_MNEMONIC_STORE },
	{ "BRANCH", IL_MNEMONIC_BRANCH },
	{ "MUL", IL_MNEMONIC_MUL },
	{ "AND", IL_MNEMONIC_AND },
	{ "OR", IL_MNEMONIC_OR },
	{ "XOR", IL_MNEMONIC_XOR },
	{ "NOT", IL_MNEMONIC_NOT },
	{ "SHIFTR", IL_MNEMONIC_SHIFTR },
	{ "SHIFTL", IL_MNEMONIC_SHIFTL },
	{ "PUSH", IL_MNEMONIC_PUSH },
	{ "POP", IL_MNEMONIC_POP },
	{ "CALL", IL_MNEMONIC_CALL },
	{ "RETURN", IL_MNEMONIC_RETURN },
	{ "HALT", IL_MNEMONIC_HALT }
};

const std::unordered_map<std::string, IL_Conditions> CONDITIONS_MAP = {
	{ "EQ", IL_CONDITIONS_EQ },
	{ "NEQ", IL_CONDITIONS_NEQ },
	{ "LT", IL_CONDITIONS_LT },
	{ "GT", IL_CONDITIONS_GT },
	{ "HLT", IL_CONDITIONS_HLT }
};

const std::vector<std::string> REGISTERS_MAP = {
	"R0", "R1", "R2", "R3", "R4", "R5", "R6", "R7",
	"R8", "R9", "R10", "R11", "R12", "SP", "IP", "CD",
};

// add opereator |= for IL_Conditions
IL_Conditions operator|=(IL_Conditions& lhs, IL_Conditions rhs) {
	return lhs = static_cast<IL_Conditions>(static_cast<int>(lhs) | static_cast<int>(rhs));
}

RegisterOperand::RegisterOperand(uint8_t id, uint8_t size)
	: m_id(id), m_size(size) {}

OperandKind RegisterOperand::getKind() {
	return OperandKind::Register;
}

uint8_t RegisterOperand::getId() const {
	return m_id;
}

uint8_t RegisterOperand::getSize() const {
	return m_size;
}

LocationOperand::LocationOperand(const std::string& location)
	: m_location(location) {}

OperandKind LocationOperand::getKind() {
	return OperandKind::Location;
}

const std::string& LocationOperand::getLocation() const {
	return m_location;
}

ImmediateOperand::ImmediateOperand(uint64_t value, uint8_t size)
	: m_value(value), m_size(size) {}

OperandKind ImmediateOperand::getKind() {
	return OperandKind::Immediate;
}

uint64_t ImmediateOperand::getValue() const {
	return m_value;
}

size_t ImmediateOperand::getSize() const {
	return m_size;
}

Instruction::Instruction(const std::string& location, IL_Mnemonic mnemonic, IL_Conditions conditions, const std::vector<std::shared_ptr<Operand>>& operands)
	: m_location(location), m_mnemonic(mnemonic), m_conditions(conditions), m_operands(operands) {}

const std::string& Instruction::getLocation() const {
	return m_location;
}

IL_Mnemonic Instruction::getMnemonic() const {
	return m_mnemonic;
}

IL_Conditions Instruction::getConditions() const {
	return m_conditions;
}

const std::vector<std::shared_ptr<Operand>>& Instruction::getOperands() const {
	return m_operands;
}

void Instruction::addOperand(std::shared_ptr<Operand> operand) {
	m_operands.push_back(operand);
}

uint8_t Parser::getNumberSize(uint64_t num) {
	if (num <= UINT8_MAX) {
		return 1;
	}
	else if (num <= UINT16_MAX) {
		return 2;
	}
	else if (num <= UINT32_MAX) {
		return 4;
	}
	else {
		return 8;
	}
}

bool Parser::isRegister(const std::shared_ptr<Token>& token) {
	std::string upper_token = token->getValue();
	std::transform(upper_token.begin(), upper_token.end(), upper_token.begin(), toupper);

	for (const std::string& reg : REGISTERS_MAP) {
		if (upper_token.starts_with(reg)) {
			return true;
		}
	}

	return false;
}

bool Parser::isLocation(const std::shared_ptr<Token>& token) {
	return token->getValue().starts_with("@");
}

IL_Mnemonic Parser::parseMnemonic(const std::shared_ptr<Token>& token) {
	auto it = MNEMONICS_MAP.find(token->getValue());
	assert(it != MNEMONICS_MAP.end());

	return it->second;
}

IL_Conditions Parser::parseCondition(const std::shared_ptr<Token>& token) {
	auto it = CONDITIONS_MAP.find(token->getValue());
	assert(it != CONDITIONS_MAP.end());

	return it->second;
}

uint64_t Parser::parseImmediate(const std::shared_ptr<Token>& token) {
	std::string value = token->getValue();

	if (value.size() > 2 && value[0] == '0' && (value[1] == 'x' || value[1] == 'X')) {
		std::string hex = value.substr(2);
		return std::stoull(hex, nullptr, 16);
	}

	if (value.size() > 2 && value[0] == '0' && (value[1] == 'b' || value[1] == 'B')) {
		std::string binary = value.substr(2);
		return std::stoull(binary, nullptr, 2);
	}

	return std::stoull(value);
}

std::shared_ptr<Operand> Parser::parseOperand(const std::shared_ptr<Token>& token) {
	if (isRegister(token)) {
		std::string upper_token = token->getValue();
		std::transform(upper_token.begin(), upper_token.end(), upper_token.begin(), toupper);

		uint8_t id = 0;
		for (const std::string& reg : REGISTERS_MAP) {
			if (upper_token.starts_with(reg)) {
				// if after the register name there's ".", then get the size otherwise the size is 8 by default
				uint8_t size = 8;
				if (upper_token.size() > reg.size() && upper_token[reg.size()] == '.') {
					size = upper_token[reg.size() + 1] - '0';
					assert((size & (size - 1)) == 0); // Power of two only
				}

				return std::make_shared<RegisterOperand>(id, size);
			}

			++id;
		}
	}
	else if (isLocation(token)) {
		std::string location = token->getValue().substr(1);
		return std::make_shared<LocationOperand>(location);
	}
	else {
		std::string value = token->getValue();
		uint64_t num = parseImmediate(token);
		uint8_t size = getNumberSize(num);

		return std::make_shared<ImmediateOperand>(num, size);
	}
}

const std::vector<std::shared_ptr<Instruction>>& Parser::getInstructions() {
	std::lock_guard<std::mutex> lock(m_mtx);
	return m_instructions;
}

Parser::Parser(const std::vector<std::shared_ptr<Token>>& tokens) {
	std::vector<std::shared_ptr<Instruction>> instructions;

	std::string next_location = "";
	size_t token_count = tokens.size();
	for (size_t i = 0; i < token_count; ++i) {
		switch (tokens[i]->getKind()) {
		case TokenKind::Location: {
			next_location = tokens[i]->getValue();
			break;
		}
		case TokenKind::Mnemonic: {
			size_t line = tokens[i]->getLine();
			IL_Mnemonic mnemonic = parseMnemonic(tokens[i]);

			IL_Conditions conditions = IL_CONDITIONS_NONE;
			while (++i < token_count) {
				if (tokens[i]->getKind() != TokenKind::Condition) {
					--i;
					break;
				}

				IL_Conditions condition = parseCondition(tokens[i]);
				conditions |= condition;
			}

			std::vector<std::shared_ptr<Operand>> operands;
			while (++i < token_count) {
				if (tokens[i]->getKind() != TokenKind::Operand) {
					--i;
					break;
				}

				std::shared_ptr<Operand> operand = parseOperand(tokens[i]);
				operands.push_back(operand);
			}

			std::string location;
			if (!next_location.empty()) {
				location = next_location;
				next_location.clear();
			}
			else {
				location = std::to_string(line);
			}

			instructions.push_back(std::make_unique<Instruction>(location, mnemonic, conditions, operands));
			break;
		}
		default: {
			assert(false);
			break;
		}
		}
	}

	std::lock_guard<std::mutex> lock(m_mtx);
	m_instructions.insert(m_instructions.end(), instructions.begin(), instructions.end());
}