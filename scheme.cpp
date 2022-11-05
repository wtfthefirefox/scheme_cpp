#include "scheme.h"

std::string Interpreter::Run(const std::string& str) {
    if (str.empty()) {
        throw SyntaxError("Empty string");
    }

    std::stringstream ss{str};
    Tokenizer tokenizer(&ss);
    std::shared_ptr<Object> tree;

    if (str[0] == '(') {  // it's a (something)
        tree = Read(&tokenizer);
    } else {  // it's function args...
        tree = Read(&tokenizer, true);
    }

    if (!tokenizer.IsEnd()) {
        throw SyntaxError("Not enough closed brackets");
    }

    if (tree == nullptr) {
        throw RuntimeError("Expression can not be empty");
    }

    if (Is<Cell>(tree) && As<Cell>(tree)->GetFirst() == nullptr) {
        throw RuntimeError("Expression in () can only number or function");
    }

    // check before parse functions
    if (!Is<Number>(tree) && !Is<Symbol>(tree) && !Is<Boolean>(tree)) {
        if (!Is<IFunction>(As<Cell>(tree)->GetFirst()) && !Is<Symbol>(As<Cell>(tree)->GetFirst()) &&
            !Is<Cell>(As<Cell>(tree)->GetFirst())) {
            throw RuntimeError("Expression can be only number or function");
        }
    }

    auto tree_copy = CopyAst(tree);
    CreateFunctionsFromSymbols(tree_copy);

    // check after parse functions
    if (!Is<Number>(tree_copy) && !Is<Boolean>(tree_copy) && !Is<Symbol>(tree_copy)) {
        if (!Is<IFunction>(As<Cell>(tree_copy)->GetFirst()) &&
            !Is<Cell>(As<Cell>(tree_copy)->GetFirst())) {  // first arg not func or cell
            throw RuntimeError("Expression can be only number or function");
        }

        if (Is<Cell>(As<Cell>(tree_copy)->GetFirst()) &&
            !Is<IFunction>(As<Cell>(As<Cell>(tree_copy)->GetFirst())->GetFirst())) {
            throw RuntimeError("Expression can be (func...) or func");
        }
    }

    auto calc_res = CalcAllInner(tree_copy);

    return TreeToString(calc_res, false, true);
}

void Interpreter::CreateFunctionsFromSymbols(std::shared_ptr<Object> head) {
    if (Is<Cell>(head)) {
        auto cur = As<Cell>(head);

        if (Is<Symbol>(As<Cell>(head)->GetFirst())) {
            auto name = As<Symbol>(cur->GetFirst())->GetName();

            // fix empty operators must throw error
            if (name == "+") {
                cur->SetFirst(std::make_shared<SumFunction>());
            } else if (name == "-") {
                cur->SetFirst(std::make_shared<MinusFunction>());
            } else if (name == "*") {
                cur->SetFirst(std::make_shared<MultiplyFunction>());
            } else if (name == "/") {
                cur->SetFirst(std::make_shared<DevideFunction>());
            } else if (name == "and") {
                cur->SetFirst(std::make_shared<AndFunction>());
            } else if (name == "=") {
                cur->SetFirst(std::make_shared<EqualFunc>());
            } else if (name == "number?") {
                cur->SetFirst(std::make_shared<IsNumberFunction>());
            } else if (name == "boolean?") {
                cur->SetFirst(std::make_shared<IsBooleanFunction>());
            } else if (name == "pair?") {
                cur->SetFirst(std::make_shared<IsPairFunction>());
            } else if (name == "null?") {
                cur->SetFirst(std::make_shared<IsNullFunction>());
            } else if (name == "list?") {
                cur->SetFirst(std::make_shared<IsListFunction>());
            } else if (name == ">") {
                cur->SetFirst(std::make_shared<GreaterFunction>());
            } else if (name == ">=") {
                cur->SetFirst(std::make_shared<GreaterOrEqualFunction>());
            } else if (name == "<") {
                cur->SetFirst(std::make_shared<LowerFunction>());
            } else if (name == "<=") {
                cur->SetFirst(std::make_shared<LowerOrEqualFunction>());
            } else if (name == "max") {
                cur->SetFirst(std::make_shared<MaxFunction>());
            } else if (name == "min") {
                cur->SetFirst(std::make_shared<MinFunction>());
            } else if (name == "abs") {
                cur->SetFirst(std::make_shared<AbsFunction>());
            } else if (name == "not") {
                cur->SetFirst(std::make_shared<NotFunction>());
            } else if (name == "or") {
                cur->SetFirst(std::make_shared<OrFunction>());
            } else if (name == "cons") {
                cur->SetFirst(std::make_shared<ConsFunction>());
            } else if (name == "car") {
                cur->SetFirst(std::make_shared<CarFunction>());
            } else if (name == "cdr") {
                cur->SetFirst(std::make_shared<CdrFunction>());
            } else if (name == "list") {
                cur->SetFirst(std::make_shared<ListFuntion>());
            } else if (name == "list-ref") {
                cur->SetFirst(std::make_shared<ListRefFuntion>());
            } else if (name == "list-tail") {
                cur->SetFirst(std::make_shared<ListTailFunction>());
            }
        }

        CreateFunctionsFromSymbols(cur->GetSecond());
        CreateFunctionsFromSymbols(cur->GetFirst());
    }
}

std::shared_ptr<Object> Interpreter::CopyAst(std::shared_ptr<Object> ptr) {
    RETURN_COPY_PTR(ptr);

    if (Is<QuoteFunction>(ptr)) {
        return std::make_shared<QuoteFunction>();
    }

    std::shared_ptr<Cell> temp_cell = std::make_shared<Cell>();

    temp_cell->SetFirst(CopyAst(As<Cell>(ptr)->GetFirst()));
    temp_cell->SetSecond(CopyAst(As<Cell>(ptr)->GetSecond()));

    return temp_cell;
}

// must return value
std::shared_ptr<Object> Interpreter::CalcAllInner(
    std::shared_ptr<Object> head) {  // called_from_func ? can be empty equations : can not be empty

    RETURN_COPY_PTR(head);

    auto cur = As<Cell>(head);

    if (cur->GetFirst() == nullptr && cur->GetSecond() == nullptr) {
        throw RuntimeError("Empty equation");
    }

    if (Is<IFunction>(cur->GetFirst())) {        // call function
        if (Is<IFunctionIs>(cur->GetFirst())) {  // is IFunctionIs function
            auto res = As<IFunctionIs>(cur->GetFirst())->IsInstanceOf(cur->GetSecond());
            return std::make_shared<Boolean>(res);
        } else if (Is<QuoteFunction>(cur->GetFirst())) {  // quote function
            return As<IFunction>(cur->GetFirst())->CallInstance(cur);
        } else {
            auto res = As<IFunction>(cur->GetFirst())->CallInstance(cur);
            return res;
        }
    } else if (Is<Cell>(cur->GetFirst())) {  // call all function in left part
        auto calc_res = std::make_shared<Cell>();
        calc_res->SetFirst(CalcAllInner(cur->GetFirst()));
        calc_res->SetSecond(CalcAllInner(cur->GetSecond()));

        return calc_res;
    }

    throw SyntaxError("Can't call that function with thouse args");
}

std::string Interpreter::TreeToString(std::shared_ptr<Object> ptr, bool is_last, bool is_first) {
    if (ptr == nullptr) {
        return "()";
    }

    if (Is<Number>(ptr)) {
        return std::to_string(As<Number>(ptr)->GetValue());
    } else if (Is<Symbol>(ptr)) {
        return As<Symbol>(ptr)->GetName();
    } else if (Is<Boolean>(ptr)) {
        return As<Boolean>(ptr)->GetValue() ? "#t" : "#f";
    } else if (Is<IFunction>(ptr)) {
        return "function";  // bottleneck for functions
    } else {
        auto cur = As<Cell>(ptr);

        if (cur->GetSecond() == nullptr && cur->GetFirst() == nullptr) {
            return "(())";
        }

        if (cur->GetSecond() == nullptr) {
            if (is_first) {
                return "(" + TreeToString(cur->GetFirst()) + ")";
            }
            return TreeToString(cur->GetFirst());
        }

        if (cur->GetFirst() == nullptr) {
            return TreeToString(cur->GetSecond());
        }

        if (!Is<Cell>(cur->GetSecond())) {
            if (is_last) {
                return TreeToString(cur->GetFirst()) + " . " + TreeToString(cur->GetSecond());
            }
            return "(" + TreeToString(cur->GetFirst()) + " . " + TreeToString(cur->GetSecond()) +
                   ")";
        }

        if (is_first) {
            return "(" + TreeToString(cur->GetFirst(), true) + " " +
                   TreeToString(cur->GetSecond(), true) + ")";
        }

        return TreeToString(cur->GetFirst()) + " " + TreeToString(cur->GetSecond(), true);
    }
}