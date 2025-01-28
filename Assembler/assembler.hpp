#pragma once

#include <vector>
#include <mutex>

#include "parser.hpp"

class Assembler {
private:
	std::mutex m_mtx;
	std::vector<uint8_t> m_opcodes;

public:
	const std::vector<uint8_t>& getOpcodes();

	Assembler(const std::vector<std::shared_ptr<Instruction>>& instructions);
};