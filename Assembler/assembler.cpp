#include <vector>
#include <mutex>
#include <memory>
#include <unordered_map>
#include <cassert>

#include "parser.hpp"
#include "assembler.hpp"
#include "il.h"
#include <iostream>


const std::vector<uint8_t>& Assembler::getOpcodes() {
	std::scoped_lock lock(m_mtx);
	return m_opcodes;
}

Assembler::Assembler(const std::vector<std::shared_ptr<Instruction>>& instructions) {
	std::vector<uint8_t> opcodes;
	std::unordered_map<size_t, size_t> instr_map;

	for (size_t i = 0; i < instructions.size(); ++i) {
		const std::shared_ptr<Instruction>& instruction = instructions[i];
		size_t curr_offset = opcodes.size();
		instr_map[i] = curr_offset;

		std::vector<const IL_Operand*> operands;
		for (const std::shared_ptr<Operand>& operand : instruction->getOperands()) {
			switch (operand->getKind()) {
			case OperandKind::Register: {
				const std::shared_ptr<RegisterOperand>& reg = std::static_pointer_cast<RegisterOperand>(operand);
				uint8_t id = reg->getId();
				uint8_t size = reg->getSize();

				operands.push_back(IL_CreateOperandRegister(id, size));
				break;
			}
			case OperandKind::Location: {
				// Find the instruction with that location
				const std::shared_ptr<LocationOperand>& loc = std::static_pointer_cast<LocationOperand>(operand);

				bool found_loc = false;
				size_t loc_offset = 0;
				for (size_t j = 0; j < std::min(i + 1, instr_map.size()); ++j) {
					const std::shared_ptr<Instruction>& prev_instruction = instructions[j];
					if (prev_instruction->getLocation() == loc->getLocation()) {
						found_loc = true;
						loc_offset = instr_map[j];
						break;
					}
				}

				assert(found_loc);

				int64_t value = loc_offset - curr_offset;
				uint8_t size = Parser::getNumberSize(value);
				operands.push_back(IL_CreateOperandImmediate(&value, size));
				break;
			}
			case OperandKind::Immediate: {
				const std::shared_ptr<ImmediateOperand>& imm = std::static_pointer_cast<ImmediateOperand>(operand);

				uint64_t value = imm->getValue();
				uint8_t size = imm->getSize();
				operands.push_back(IL_CreateOperandImmediate(&value, size));
				break;
			}
			}
		}

		size_t predicted_size = sizeof(IL_Code);
		for (const IL_Operand* operand : operands) {
			predicted_size += IL_GetOperandSize(operand);
		}

		std::vector<uint8_t> opcode(predicted_size);

		IL_Code* code = reinterpret_cast<IL_Code*>(opcode.data());
		code->mnemonic = instruction->getMnemonic();
		code->conditions = instruction->getConditions();
		code->operand_count = 0;

		for (const IL_Operand* operand : operands) {
			IL_AppendCodeOperand(code, operand);
		}

		opcodes.insert(opcodes.end(), opcode.begin(), opcode.end());
	}

	std::scoped_lock lock(m_mtx);
	m_opcodes.insert(m_opcodes.end(), opcodes.begin(), opcodes.end());
}
