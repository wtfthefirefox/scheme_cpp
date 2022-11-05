#pragma once

#include <string>
#include "tokenizer.h"
#include "parser.h"
#include <sstream>

class Interpreter {
public:
    std::string Run(const std::string&);

private:
    std::shared_ptr<Object> CalcAllInner(std::shared_ptr<Object>);

    // second arg - is last expr; third - is first;
    std::string TreeToString(std::shared_ptr<Object>, bool = false, bool = false);

    std::shared_ptr<Object> CopyAst(std::shared_ptr<Object>);

    void CreateFunctionsFromSymbols(std::shared_ptr<Object>);
};
