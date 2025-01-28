#pragma once

#include <vector>
#include <mutex>
#include <memory>

#include "tokenizer.hpp"
#include "il.h"

enum class OperandKind {
	Register,
	Location,
	Immediate
};

class Operand {
public:
	virtual OperandKind getKind() = 0;
};

class RegisterOperand : public Operand {
private:
	uint8_t m_id;
	uint8_t m_size;

public:
	RegisterOperand(uint8_t id, uint8_t size);

	OperandKind getKind() override;

	uint8_t getId() const;
	uint8_t getSize() const;
};

class LocationOperand : public Operand {
private:
	std::string m_location;

public:
	LocationOperand(const std::string& location);


	OperandKind getKind() override;
	const std::string& getLocation() const;
};

class ImmediateOperand : public Operand {
private:
	uint64_t m_value;
	uint8_t m_size;

public:
	ImmediateOperand(uint64_t value, uint8_t size);

	OperandKind getKind() override;

	uint64_t getValue() const;
	size_t getSize() const;
};

class Instruction {
private:
	std::string m_location;
	IL_Mnemonic m_mnemonic;
	IL_Conditions m_conditions;
	std::vector<std::shared_ptr<Operand>> m_operands;

public:
	Instruction(const std::string& location, IL_Mnemonic mnemonic, IL_Conditions conditions, const std::vector<std::shared_ptr<Operand>>& operands);

	const std::string& getLocation() const;
	IL_Mnemonic getMnemonic() const;
	IL_Conditions getConditions() const;
	const std::vector<std::shared_ptr<Operand>>& getOperands() const;

	void addOperand(std::shared_ptr<Operand> operand);
};

class Parser {
private:
	std::mutex m_mtx;
	std::vector<std::shared_ptr<Instruction>> m_instructions;

	static bool isRegister(const std::shared_ptr<Token>& token);
	static bool isLocation(const std::shared_ptr<Token>& token);

	static IL_Mnemonic parseMnemonic(const std::shared_ptr<Token>& token);
	static IL_Conditions parseCondition(const std::shared_ptr<Token>& token);
	static std::shared_ptr<Operand> parseOperand(const std::shared_ptr<Token>& token);
public:
	static uint8_t getNumberSize(uint64_t num);

	const std::vector<std::shared_ptr<Instruction>>& getInstructions();
	Parser(const std::vector<std::shared_ptr<Token>>& tokens);
};