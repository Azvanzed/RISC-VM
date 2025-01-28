#pragma once

#include <string>
#include <vector>
#include <mutex>
#include <memory>

enum class TokenKind {
	Unknown = 0,
	Location,
	Mnemonic,
	Condition,
	Operand
};

class Token {
private:
	size_t m_line;
	TokenKind kind;
	std::string value;

public:
	Token(size_t line, TokenKind kind, const std::string& value);

	bool Is(TokenKind kind) const;

	bool operator!=(TokenKind kind) const;
	bool operator==(TokenKind kind) const;

	size_t getLine() const;
	TokenKind getKind() const;
	std::string getValue() const;
};

class Tokenizer {
private:
	std::mutex m_mtx;
	std::vector<std::shared_ptr<Token>> m_tokens;

	static std::string extractLocation(const std::string& line);
	static std::string extractMnemonic(const std::string& line);
	static std::vector<std::string> extractConditions(const std::string& line);
	static std::vector<std::string> extractOperands(const std::string& line);
	static std::string processNumericToken(const std::string& token);
	static std::string sanitizeToken(const std::string& token);

public:
	const std::vector<std::shared_ptr<Token>>& getTokens();

	Tokenizer(const std::vector<std::string>& lines);
};