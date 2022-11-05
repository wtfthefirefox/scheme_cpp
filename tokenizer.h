#pragma once

#include <variant>
#include <optional>
#include <istream>
#include <stddef.h>

#include "error.h"

struct SymbolToken {
    std::string name;

    bool operator==(const SymbolToken& other) const;
};

struct QuoteToken {
    bool operator==(const QuoteToken&) const;
};

struct DotToken {
    bool operator==(const DotToken&) const;
};

enum class BracketToken { OPEN, CLOSE };

struct ConstantToken {
    int64_t value;

    bool operator==(const ConstantToken& other) const;
};

using Token = std::variant<ConstantToken, BracketToken, SymbolToken, QuoteToken, DotToken>;

class Tokenizer {
public:
    Tokenizer(std::istream* in) : stream_(in){};

    bool IsEnd();

    void Next();

    Token GetToken();

private:
    std::istream* stream_;
    size_t token_size_ = 0;

    void ClearInputFromSpaces();

    void IsSymbolCorrect();
};