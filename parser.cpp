#include <parser.h>

std::shared_ptr<Object> ReadList(Tokenizer* tokenizer);

std::shared_ptr<Object> Read(Tokenizer* tokenizer,
                             bool is_list) {  // is_list ? it's a new cell : it's a new single child
    auto bracket_open_token = Token{BracketToken::OPEN};
    auto token = tokenizer->GetToken();
    tokenizer->Next();

    if (std::holds_alternative<ConstantToken>(token)) {
        return std::make_shared<Number>(std::get<ConstantToken>(token).value);
    } else if (std::holds_alternative<SymbolToken>(token)) {
        auto name = std::get<SymbolToken>(token).name;

        if (name == "#t") {
            return std::make_shared<Boolean>(true);
        } else if (name == "#f") {
            return std::make_shared<Boolean>(false);
        }

        return std::make_shared<Symbol>(name);
    } else if (std::holds_alternative<QuoteToken>(token)) {
        if (tokenizer->IsEnd()) {  // empty function quote
            throw SyntaxError("Syntax error while reading quote function");
        }

        auto ptr = std::make_shared<QuoteFunction>();

        if (is_list) {  // a new cell
            // it's a function, so it can not be without args
            auto quote_object = std::make_shared<Cell>();
            quote_object->SetFirst(ptr);
            quote_object->SetSecond(Read(tokenizer));

            return quote_object;
        }

        // a new single node
        return ptr;
    } else if (token == bracket_open_token) {
        return ReadList(tokenizer);
    }

    throw SyntaxError("Syntax error while reading input");
}

std::shared_ptr<Object> ReadList(Tokenizer* tokenizer) {
    auto bracket_close_token = Token{BracketToken::CLOSE};
    auto dot_token = Token{DotToken()};

    if (tokenizer->IsEnd()) {
        throw SyntaxError("not enough closed brackets");
    }

    auto head = std::make_shared<Cell>();
    auto tail = head;

    int is_pair = -1;  // -1 - not pair; 1 - second readed; otherwise - error

    while (!tokenizer->IsEnd() && tokenizer->GetToken() != bracket_close_token) {
        if (is_pair == 1) {  // second was readed & can not read anymore
            throw SyntaxError("pair has only 2 parameters");
        }

        tail->SetFirst(Read(tokenizer, true));

        if (tokenizer->IsEnd()) {  // readed all symbols
            throw SyntaxError("Not enough closed brackets");
        }

        if (tokenizer->GetToken() == bracket_close_token) {
            tokenizer->Next();
            return head;
        } else if (tokenizer->GetToken() == dot_token && is_pair == -1) {  // second not readed
            is_pair = 1;
            tokenizer->Next();
            tail->SetSecond(Read(tokenizer, true));
        } else {
            auto temp = std::make_shared<Cell>();  // temp value of new cell
            tail->SetSecond(temp);
            tail = temp;
        }
    }

    if (tokenizer->IsEnd()) {
        throw SyntaxError("not enough closed brackets");
    }

    if (!tokenizer->IsEnd()) {
        tokenizer->Next();
    }

    if (head->GetFirst() == nullptr) {
        return nullptr;
    }

    return head;
}