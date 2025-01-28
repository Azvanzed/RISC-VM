#include <string>
#include <vector>
#include <iostream>
#include <sstream>
#include <cctype>
#include <algorithm>
#include <mutex>
#include <memory>

#include "tokenizer.hpp"

// General sanitization function to clean all tokens
std::string Tokenizer::sanitizeToken(const std::string& token) {
    std::string sanitized = token;

    // Remove specific problematic control characters like '\0', '\r', '\n'
    sanitized.erase(std::remove_if(sanitized.begin(), sanitized.end(), [](unsigned char c) {
        return c == '\0' || c == '\r' || c == '\n'; // Only remove specific control characters
        }), sanitized.end());

    // Preserve internal spaces but trim leading and trailing whitespace
    sanitized.erase(0, sanitized.find_first_not_of(" \t"));
    sanitized.erase(sanitized.find_last_not_of(" \t") + 1);

    return sanitized;
}

Token::Token(size_t line, TokenKind kind, const std::string& value)
    : m_line(line), kind(kind), value(value) {}

bool Token::Is(TokenKind kind) const {
    return this->kind == kind;
}

bool Token::operator==(TokenKind kind) const {
    return Is(kind);
}

bool Token::operator!=(TokenKind kind) const {
    return !(*this == kind);
}

size_t Token::getLine() const {
    return m_line;
}

TokenKind Token::getKind() const {
    return kind;
}

std::string Token::getValue() const {
    return value;
}

const std::vector<std::shared_ptr<Token>>& Tokenizer::getTokens() {
    std::lock_guard<std::mutex> lock(m_mtx);
    return m_tokens;
}

std::string Tokenizer::extractLocation(const std::string& line) {
    std::string location = line.substr(1);
    return sanitizeToken(location);
}

std::string Tokenizer::extractMnemonic(const std::string& line) {
    size_t mnemonic_end = line.find('('); // Check for conditions first
    if (mnemonic_end == std::string::npos) {
        mnemonic_end = line.find(' '); // Check for operands
    }

    if (mnemonic_end == std::string::npos) {
        return sanitizeToken(line); // No conditions or operands, the whole line is the mnemonic
    }

    return sanitizeToken(line.substr(0, mnemonic_end));
}

std::vector<std::string> Tokenizer::extractConditions(const std::string& line) {
    std::vector<std::string> conditions;
    size_t start = line.find('(');
    size_t end = line.find(')');

    if (start != std::string::npos && end != std::string::npos && start < end) {
        std::string cond_str = line.substr(start + 1, end - start - 1);

        size_t pos = 0;
        std::string token;
        while ((pos = cond_str.find('.')) != std::string::npos) {
            token = cond_str.substr(0, pos);
            conditions.push_back(sanitizeToken(token));
            cond_str.erase(0, pos + 1);
        }

        if (!cond_str.empty()) {
            conditions.push_back(sanitizeToken(cond_str)); // Add the last condition
        }
    }

    return conditions;
}

std::vector<std::string> Tokenizer::extractOperands(const std::string& line) {
    std::vector<std::string> operands;

    size_t space_pos = line.find(' ');
    if (space_pos == std::string::npos) {
        return operands; // No operands
    }

    std::string op_str = line.substr(space_pos + 1);

    size_t pos = 0;
    std::string token;
    while ((pos = op_str.find(',')) != std::string::npos) {
        token = op_str.substr(0, pos);
        token = sanitizeToken(token); // Sanitize token
        operands.push_back(token);
        op_str.erase(0, pos + 1);
    }

    if (!op_str.empty()) {
        op_str = sanitizeToken(op_str); // Sanitize the last operand
        operands.push_back(op_str);
    }

    return operands;
}

std::string Tokenizer::processNumericToken(const std::string& token) {
    if (token.empty()) {
        return token;
    }

    std::string sanitized = sanitizeToken(token);

    // Check if the token is a hex number
    if (sanitized.size() > 2 && sanitized[0] == '0' && (sanitized[1] == 'x' || sanitized[1] == 'X')) {
        return sanitized; // Hex format is already correct
    }

    // Check if the token is a binary number
    if (sanitized.size() > 2 && sanitized[0] == '0' && (sanitized[1] == 'b' || sanitized[1] == 'B')) {
        return sanitized; // Binary format is already correct
    }

    // Check if the token is a decimal number
    bool is_decimal = true;
    for (char ch : sanitized) {
        if (!isdigit(ch)) {
            is_decimal = false;
            break;
        }
    }

    if (is_decimal) {
        return sanitized; // Decimal format is already correct
    }

    return sanitized; // If not a number, return sanitized version
}

Tokenizer::Tokenizer(const std::vector<std::string>& lines) {
    std::vector<std::shared_ptr<Token>> tokens;
    for (size_t i = 0; i < lines.size(); ++i) {
        const std::string& line = sanitizeToken(lines[i]);

        if (line.starts_with("@")) {
            std::string location = extractLocation(line);
            tokens.push_back(std::make_shared<Token>(Token(i, TokenKind::Location, location)));
        }
        else {
            std::string mnemonic = extractMnemonic(line);
            tokens.push_back(std::make_shared<Token>(Token(i, TokenKind::Mnemonic, mnemonic)));

            std::vector<std::string> conditions = extractConditions(line);
            for (const std::string& condition : conditions) {
                tokens.push_back(std::make_shared<Token>(Token(i, TokenKind::Condition, condition)));
            }

            std::vector<std::string> operands = extractOperands(line);
            for (const std::string& operand : operands) {
                tokens.push_back(std::make_shared<Token>(Token(i, TokenKind::Operand, processNumericToken(operand))));
            }
        }
    }

    std::lock_guard<std::mutex> lock(m_mtx);
    m_tokens.insert(m_tokens.end(), tokens.begin(), tokens.end());
}
