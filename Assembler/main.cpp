#include <iostream>
#include <vector>
#include <fstream>
#include <string>
#include <stdexcept>

#include "tokenizer.hpp"
#include "parser.hpp"
#include "assembler.hpp"

void SaveFile(const std::string& filename, const std::vector<uint8_t>& data) {
    std::ofstream file(filename, std::ios::binary);
    if (!file.is_open()) {
        throw std::runtime_error("Failed to open file for writing: " + filename);
    }

    file.write(reinterpret_cast<const char*>(data.data()), data.size());
}

void LoadFile(const std::string& filename, std::vector<uint8_t>& data) {
    std::ifstream file(filename, std::ios::binary);
    if (!file.is_open()) {
        throw std::runtime_error("Failed to open file for reading: " + filename);
    }

    file.seekg(0, std::ios::end);
    size_t size = file.tellg();
    file.seekg(0, std::ios::beg);

    data.resize(size);
    file.read(reinterpret_cast<char*>(data.data()), size);
}

std::vector<std::string> ReadSource(const std::string& filename) {
    std::vector<uint8_t> data;
    LoadFile(filename, data);

    std::vector<std::string> lines;
    std::string line;
    for (size_t i = 0; i < data.size(); i++) {
        if (data[i] == '\n') {
            lines.push_back(line);
            line.clear();
        }
        else {
            line.push_back(data[i]);
        }
    }

    if (!line.empty()) {
        lines.push_back(line);
    }

    return lines;
}

int main(int argc, char* argv[]) {
	if (argc != 3) {
		std::cerr << "Usage: " << argv[0] << " <input file> <output file>" << std::endl;
		return EXIT_FAILURE;
	}

	std::string input_file = argv[1];
	std::string output_file = argv[2];

	std::cout << "Reading source file: " << input_file << std::endl;
	std::vector<std::string> source = ReadSource(input_file);

	std::cout << "Tokenizing source..." << std::endl;
    Tokenizer tokenizer(source);

	std::cout << "Parsing tokens..." << std::endl;
    Parser parser(tokenizer.getTokens());

	std::cout << "Generating opcodes..." << std::endl;
    Assembler assembler(parser.getInstructions());

	std::cout << "Saving opcodes to file: " << output_file << std::endl;
	SaveFile(output_file, assembler.getOpcodes());

    return EXIT_SUCCESS;
}