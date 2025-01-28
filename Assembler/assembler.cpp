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
	std::unordered_map<size_t, size_t> delayed_loc_map;

	for (size_t insn_idx = 0; insn_idx < instructions.size(); ++insn_idx) {
		const std::shared_ptr<Instruction>& instruction = instructions[insn_idx];
		size_t curr_offset = opcodes.size();
		instr_map[insn_idx] = curr_offset;

		std::vector<const IL_Operand*> il_operands;
		const std::vector<std::shared_ptr<Operand>>& operands = instruction->getOperands();
		for (size_t op_idx = 0; op_idx < operands.size(); ++op_idx) {
			const std::shared_ptr<Operand>& operand = operands[op_idx];
			
			switch (operand->getKind()) {
			case OperandKind::Register: {
				const std::shared_ptr<RegisterOperand>& reg = std::static_pointer_cast<RegisterOperand>(operand);
				uint8_t id = reg->getId();
				uint8_t size = reg->getSize();

				il_operands.push_back(IL_CreateOperandRegister(id, size));
				break;
			}
			case OperandKind::Location: {
				// Find the instruction with that location
				const std::shared_ptr<LocationOperand>& loc = std::static_pointer_cast<LocationOperand>(operand);

				// Skip its assignement, will be assigned once every instruction is set
				// Its detremental to give it always 8 bytes mainly, but later we will fix it
				size_t value = 0;
				il_operands.push_back(IL_CreateOperandImmediate(&value, sizeof(value)));
				delayed_loc_map[insn_idx] = op_idx;
				break;
			}
			case OperandKind::Immediate: {
				const std::shared_ptr<ImmediateOperand>& imm = std::static_pointer_cast<ImmediateOperand>(operand);

				uint64_t value = imm->getValue();
				uint8_t size = imm->getSize();
				il_operands.push_back(IL_CreateOperandImmediate(&value, size));
				break;
			}
			}
		}

		size_t predicted_size = sizeof(IL_Code);
		for (const IL_Operand* il_operand : il_operands) {
			predicted_size += IL_GetOperandSize(il_operand);
		}

		std::vector<uint8_t> opcode(predicted_size);

		IL_Code* il_code = reinterpret_cast<IL_Code*>(opcode.data());
		IL_SetCodeMnemonic(il_code, instruction->getMnemonic());
		IL_SetCodeConditions(il_code, instruction->getConditions());
		IL_SetCodeOperandCount(il_code, 0); // Appending operands will increment

		for (const IL_Operand* il_operand : il_operands) {
			IL_AppendCodeOperand(il_code, il_operand);
		}

		opcodes.insert(opcodes.end(), opcode.begin(), opcode.end());
	}

	// Fix delayed locs
	for (auto [insn_idx, op_idx] : delayed_loc_map) {
		printf("insn_idx: %i | op_idx: %i\n", insn_idx, op_idx);

		const std::shared_ptr<Instruction>& loc_instruction = instructions[insn_idx];
		const std::shared_ptr<LocationOperand>& loc = std::static_pointer_cast<LocationOperand>(loc_instruction->getOperands()[op_idx]);


		// Find targeted instruction location
		bool corrected = false;

		for (size_t target_idx = 0; target_idx < instructions.size(); ++target_idx) {
			const std::shared_ptr<Instruction>& target_instruction = instructions[target_idx];

			if (target_instruction->getLocation() == loc->getLocation()) {
				size_t loc_offset = instr_map[insn_idx];
				size_t target_offset = instr_map[target_idx];
				size_t offset = target_offset - loc_offset;

				// Reassign the location operand
				IL_Code* il_code = reinterpret_cast<IL_Code*>(&opcodes[loc_offset]);
				IL_Operand* il_operand = IL_GetCodeOperand(il_code, op_idx);
				IL_WriteOperandData(il_operand, &offset, sizeof(offset));

				corrected = true;
				break;
			}
		}

		assert(corrected);
	}

	std::scoped_lock lock(m_mtx);
	m_opcodes.insert(m_opcodes.end(), opcodes.begin(), opcodes.end());
}
