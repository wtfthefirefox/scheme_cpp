#include <tokenizer.h>

bool SymbolToken::operator==(const SymbolToken &other) const {
    return name == other.name;
}

bool QuoteToken::operator==(const QuoteToken &) const {
    return true;
}

bool DotToken::operator==(const DotToken &) const {
    return true;
}

bool ConstantToken::operator==(const ConstantToken &other) const {
    return value == other.value;
}

bool Tokenizer::IsEnd() {
    size_t temp_size = 0;

    while (stream_->peek() != EOF && (stream_->peek() == ' ' || stream_->peek() == '\n')) {
        stream_->get();
        ++temp_size;
    }

    auto cur_symbol = stream_->peek();  // symbol after skipping spaces

    while (temp_size > 0) {
        --temp_size;
        stream_->unget();
    }

    return cur_symbol == EOF;
}

void Tokenizer::Next() {
    if (token_size_ == 0) {
        GetToken();
    }

    while (stream_->peek() != EOF && token_size_ > 0) {
        --token_size_;
        stream_->get();
    }

    ClearInputFromSpaces();
}

Token Tokenizer::GetToken() {
    ClearInputFromSpaces();

    int readed_num = 0;
    bool is_readed_num = false;
    bool is_readed_num_neg = false;
    token_size_ = 0;

    if (stream_->peek() == '-') {  // may be only '-' or -number
        is_readed_num_neg = true;
        stream_->get();
        ++token_size_;

        if (!std::isdigit(stream_->peek())) {
            stream_->unget();
            return SymbolToken{"-"};
        }
    }

    if (stream_->peek() == '+') {  // may be only '+' or +number
        stream_->get();
        ++token_size_;

        if (!std::isdigit(stream_->peek())) {
            stream_->unget();
            return SymbolToken{"+"};
        }
    }

    while (std::isdigit(stream_->peek())) {  // count number
        is_readed_num = true;
        ++token_size_;

        readed_num = 10 * readed_num + (stream_->peek() - '0');
        stream_->get();
    }

    IsSymbolCorrect();

    if (is_readed_num) {  // number readed
        if (readed_num == 0) {
            stream_->unget();

            return ConstantToken{0};
        }

        int temp = readed_num;

        while (temp > 0) {
            stream_->unget();
            temp /= 10;
        }

        if (is_readed_num_neg) {
            stream_->unget();
            return ConstantToken{-readed_num};
        }

        return ConstantToken{readed_num};
    }

    ++token_size_;  // readed one symbol

    if (stream_->peek() == ')') {
        return BracketToken::CLOSE;
    }

    if (stream_->peek() == '(') {
        return BracketToken::OPEN;
    }

    if (stream_->peek() == '+') {
        return SymbolToken{"+"};
    }

    if (stream_->peek() == '.') {
        return DotToken{};
    }

    if (stream_->peek() == '>') {
        stream_->get();

        if (stream_->peek() == '=') {
            stream_->unget();
            ++token_size_;
            return SymbolToken{">="};
        }
        stream_->unget();

        return SymbolToken{">"};
    }

    if (stream_->peek() == '<') {
        stream_->get();

        if (stream_->peek() == '=') {
            stream_->unget();
            ++token_size_;
            return SymbolToken{"<="};
        }
        stream_->unget();

        return SymbolToken{"<"};
    }

    if (stream_->peek() == '=') {
        return SymbolToken{"="};
    }

    std::string cur_symbol;

    if (stream_->peek() == '*') {
        stream_->get();

        if (stream_->peek() == ' ' || stream_->peek() == '\n') {
            stream_->unget();
            return SymbolToken{"*"};
        }

        cur_symbol += "*";
    }

    if (stream_->peek() == '/') {
        stream_->get();

        if (stream_->peek() == ' ' || stream_->peek() == '\n') {
            stream_->unget();
            return SymbolToken{"/"};
        }

        cur_symbol += "/";
    }

    if (stream_->peek() == static_cast<char>(39)) {
        return QuoteToken{};
    }

    if (std::isalpha(stream_->peek()) || stream_->peek() == '#') {
        cur_symbol += stream_->peek();
        stream_->get();
    }

    if (!cur_symbol.empty()) {  // readed at least one char for Symbol Token
        while (stream_->peek() != EOF &&
               (std::isdigit(stream_->peek()) || std::isalpha(stream_->peek()) ||
                stream_->peek() == '?' || stream_->peek() == '*' || stream_->peek() == '#' ||
                stream_->peek() == '/' || stream_->peek() == '!' || stream_->peek() == '-' ||
                stream_->peek() == '<' || stream_->peek() == '>' || stream_->peek() == '=')) {
            ++token_size_;
            cur_symbol += stream_->peek();
            stream_->get();
        }

        if (stream_->peek() != EOF) {
            IsSymbolCorrect();
        }
    }

    if (!cur_symbol.empty()) {  // clear stream after operations
        int temp = token_size_;

        while (temp > 0) {
            stream_->unget();
            --temp;
        }

        if (cur_symbol == "quote") {
            return QuoteToken();
        }

        return SymbolToken{cur_symbol};
    }

    throw SyntaxError("Undefined symbol in list");
}

void Tokenizer::ClearInputFromSpaces() {
    while (stream_->peek() != EOF && (stream_->peek() == ' ' || stream_->peek() == '\n')) {
        stream_->get();
    }
}

void Tokenizer::IsSymbolCorrect() {
    if (std::isdigit(stream_->peek()) || std::isalpha(stream_->peek()) || stream_->peek() == '?' ||
        stream_->peek() == '*' || stream_->peek() == '#' || stream_->peek() == '/' ||
        stream_->peek() == '!' || stream_->peek() == '-' || stream_->peek() == EOF ||
        stream_->peek() == ' ' || stream_->peek() == '\n' || stream_->peek() == '#' ||
        stream_->peek() == '+' || stream_->peek() == '(' || stream_->peek() == ')' ||
        stream_->peek() == '<' || stream_->peek() == '>' || stream_->peek() == '=' ||
        stream_->peek() == static_cast<char>(39) || stream_->peek() == '.') {
        return;
    }

    throw SyntaxError("Undefined symbol");
}