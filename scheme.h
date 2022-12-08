#pragma once

#include <string>
#include <sstream>

#include "tokenizer.h"
#include "parser.h"

class Interpreter {
public:
    Interpreter() : global_scope_(std::make_shared<Scope>()){};

    std::string Run(const std::string&);

private:
    std::shared_ptr<Scope> global_scope_;
    std::shared_ptr<Object> CalcAllInner(std::shared_ptr<Object>);

    // second arg - is last expr; third - is first;
    std::string TreeToString(std::shared_ptr<Object>, bool = false, bool = false);

    // is in define
    void CreateFunctionsFromSymbols(std::shared_ptr<Object>, bool = false);
    void CreateFunctionsFromVariables(std::shared_ptr<Object>, bool = false);
};
