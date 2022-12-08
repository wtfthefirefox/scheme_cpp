#include "scheme.h"

// i'm super fast, so i can copy every tree that you want(time to run < 1ms)
inline std::shared_ptr<Object> CopyAst(std::shared_ptr<Object> ptr) {
    RETURN_COPY_PTR(ptr)

    if (Is<QuoteFunction>(ptr)) {
        return std::make_shared<QuoteFunction>();
    } else if (Is<LambdaFunction>(ptr)) {
        return std::make_shared<LambdaFunction>();
    } else if (Is<SumFunction>(ptr)) {
        return std::make_shared<SumFunction>();
    } else if (Is<IfFunction>(ptr)) {
        return std::make_shared<IfFunction>();
    } else if (Is<MinusFunction>(ptr)) {
        return std::make_shared<MinusFunction>();
    } else if (Is<SetFunction>(ptr)) {
        return std::make_shared<SetFunction>();
    } else if (Is<EqualFunc>(ptr)) {
        return std::make_shared<EqualFunc>();
    } else if (Is<LowerFunction>(ptr)) {
        return std::make_shared<LowerFunction>();
    } else if (Is<MultiplyFunction>(ptr)) {
        return std::make_shared<MultiplyFunction>();
    } else if (Is<DevideFunction>(ptr)) {
        return std::make_shared<DevideFunction>();
    } else if (Is<MinFunction>(ptr)) {
        return std::make_shared<MinFunction>();
    } else if (Is<MaxFunction>(ptr)) {
        return std::make_shared<MaxFunction>();
    } else if (Is<AbsFunction>(ptr)) {
        return std::make_shared<AbsFunction>();
    } else if (Is<GreaterFunction>(ptr)) {
        return std::make_shared<GreaterFunction>();
    } else if (Is<ConsFunction>(ptr)) {
        return std::make_shared<ConsFunction>();
    } else if (Is<DefineFunction>(ptr)) {
        return std::make_shared<DefineFunction>();
    }

    std::shared_ptr<Cell> temp_cell = std::make_shared<Cell>();

    temp_cell->SetFirst(CopyAst(As<Cell>(ptr)->GetFirst()));
    temp_cell->SetSecond(CopyAst(As<Cell>(ptr)->GetSecond()));

    return temp_cell;
}

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
            throw RuntimeError("Expression can be only number or (function)");
        }
    }

    if (Is<Symbol>(tree)) {
        auto search_res = global_scope_->Get(As<Symbol>(tree)->GetName());
        if (search_res != nullptr) {
            return TreeToString(search_res);
        }
    }

    // ('some) fix
    if (Is<Cell>(tree) && As<Cell>(tree)->GetSecond() == nullptr &&
        Is<Cell>(As<Cell>(tree)->GetFirst()) &&
        Is<QuoteFunction>(As<Cell>(As<Cell>(tree)->GetFirst())->GetFirst())) {
        tree = As<Cell>(tree)->GetFirst();
    }

    auto tree_copy = CopyAst(tree);
    CreateFunctionsFromVariables(tree_copy);
    CreateFunctionsFromSymbols(tree_copy);

    // check after parse functions
    if (Is<Cell>(tree_copy) && Is<LambdaFunction>(As<Cell>(tree_copy)->GetFirst())) {
        throw SyntaxError("can not define lambda function like (lambda ...)");
    }

    if (!Is<Number>(tree_copy) && !Is<Boolean>(tree_copy) && !Is<Symbol>(tree_copy)) {
        if (!Is<IFunction>(As<Cell>(tree_copy)->GetFirst()) &&
            !Is<Cell>(As<Cell>(tree_copy)->GetFirst())) {  // first arg not func or cell
            throw RuntimeError("Expression can be only number or (function)");
        }

        //        if (Is<Cell>(As<Cell>(tree_copy)->GetFirst()) &&
        //            !Is<IFunction>(As<Cell>(As<Cell>(tree_copy)->GetFirst())->GetFirst()) ||) {
        //            throw RuntimeError("Expression can be (func...) or func or ((func) args)");
        //        }
    }

    if (Is<Symbol>(tree_copy)) {
        throw NameError("undefined functions or variable");
    }

    auto calc_res = CalcAllInner(tree_copy);

    // uncompiled (lambda...) fix
    if (Is<Cell>(calc_res) && Is<Cell>(As<Cell>(calc_res)->GetFirst()) &&
        Is<LambdaFunction>(As<Cell>(As<Cell>(calc_res)->GetFirst())->GetFirst())) {
        calc_res = CalcAllInner(calc_res);
    }

    // another-lambda
    if (Is<Cell>(calc_res) && Is<LambdaFunction>(As<Cell>(calc_res)->GetFirst())) {
        calc_res = CalcAllInner(calc_res);
    }

    return TreeToString(calc_res, false, true);
}

void Interpreter::CreateFunctionsFromSymbols(std::shared_ptr<Object> head, bool is_in_define) {
    if (Is<Cell>(head)) {
        bool is_lambda = false;
        auto cur = As<Cell>(head);

        if (Is<Symbol>(cur->GetFirst())) {
            auto name = As<Symbol>(cur->GetFirst())->GetName();
            auto search_res_name = global_scope_->Get(name);

            if (name == "+" && search_res_name == nullptr) {
                cur->SetFirst(std::make_shared<SumFunction>());
            } else if (name == "-" && search_res_name == nullptr) {
                cur->SetFirst(std::make_shared<MinusFunction>());
            } else if (name == "*" && search_res_name == nullptr) {
                cur->SetFirst(std::make_shared<MultiplyFunction>());
            } else if (name == "/" && search_res_name == nullptr) {
                cur->SetFirst(std::make_shared<DevideFunction>());
            } else if (name == "and" && search_res_name == nullptr) {
                cur->SetFirst(std::make_shared<AndFunction>());
            } else if (name == "if" && search_res_name == nullptr) {
                cur->SetFirst(std::make_shared<IfFunction>());
            } else if (name == "=" && search_res_name == nullptr) {
                cur->SetFirst(std::make_shared<EqualFunc>());
            } else if (name == "number?" && search_res_name == nullptr) {
                cur->SetFirst(std::make_shared<IsNumberFunction>());
            } else if (name == "boolean?" && search_res_name == nullptr) {
                cur->SetFirst(std::make_shared<IsBooleanFunction>());
            } else if (name == "pair?" && search_res_name == nullptr) {
                cur->SetFirst(std::make_shared<IsPairFunction>());
            } else if (name == "null?" && search_res_name == nullptr) {
                cur->SetFirst(std::make_shared<IsNullFunction>());
            } else if (name == "list?" && search_res_name == nullptr) {
                cur->SetFirst(std::make_shared<IsListFunction>());
            } else if (name == ">" && search_res_name == nullptr) {
                cur->SetFirst(std::make_shared<GreaterFunction>());
            } else if (name == ">=" && search_res_name == nullptr) {
                cur->SetFirst(std::make_shared<GreaterOrEqualFunction>());
            } else if (name == "<" && search_res_name == nullptr) {
                cur->SetFirst(std::make_shared<LowerFunction>());
            } else if (name == "<=" && search_res_name == nullptr) {
                cur->SetFirst(std::make_shared<LowerOrEqualFunction>());
            } else if (name == "max" && search_res_name == nullptr) {
                cur->SetFirst(std::make_shared<MaxFunction>());
            } else if (name == "min" && search_res_name == nullptr) {
                cur->SetFirst(std::make_shared<MinFunction>());
            } else if (name == "abs" && search_res_name == nullptr) {
                cur->SetFirst(std::make_shared<AbsFunction>());
            } else if (name == "not" && search_res_name == nullptr) {
                cur->SetFirst(std::make_shared<NotFunction>());
            } else if (name == "or" && search_res_name == nullptr) {
                cur->SetFirst(std::make_shared<OrFunction>());
            } else if (name == "cons" && search_res_name == nullptr) {
                cur->SetFirst(std::make_shared<ConsFunction>());
            } else if (name == "car" && search_res_name == nullptr) {
                cur->SetFirst(std::make_shared<CarFunction>());
            } else if (name == "set-car!" && search_res_name == nullptr) {
                cur->SetFirst(std::make_shared<SetCarFunction>());
            } else if (name == "cdr" && search_res_name == nullptr) {
                cur->SetFirst(std::make_shared<CdrFunction>());
            } else if (name == "set-cdr!" && search_res_name == nullptr) {
                cur->SetFirst(std::make_shared<SetCdrFunction>());
            } else if (name == "list" && search_res_name == nullptr) {
                cur->SetFirst(std::make_shared<ListFuntion>());
            } else if (name == "list-ref" && search_res_name == nullptr) {
                cur->SetFirst(std::make_shared<ListRefFuntion>());
            } else if (name == "list-tail" && search_res_name == nullptr) {
                cur->SetFirst(std::make_shared<ListTailFunction>());
            } else if (name == "define" && search_res_name == nullptr) {
                cur->SetFirst(std::make_shared<DefineFunction>());
                is_in_define = true;
            } else if (name == "set!" && search_res_name == nullptr) {
                cur->SetFirst(std::make_shared<SetFunction>());
            } else if (name == "symbol?" && search_res_name == nullptr) {
                cur->SetFirst(std::make_shared<IsSymbolFunction>());
            } else if (name == "lambda" && search_res_name == nullptr) {
                cur->SetFirst(std::make_shared<LambdaFunction>());
            } else if (search_res_name != nullptr) {  // catch lambda
                auto search_res = global_scope_->Get(name);

                if (!is_in_define) {
                    if (Is<Cell>(search_res)) {
                        if (Is<LambdaFunction>(As<Cell>(search_res)->GetFirst())) {
                            cur->SetFirst(search_res);
                            is_lambda = true;
                        } else if (Is<Cell>(As<Cell>(search_res)->GetFirst()) &&
                                   Is<LambdaFunction>(
                                       As<Cell>(As<Cell>(search_res)->GetFirst())->GetFirst())) {
                            cur->SetFirst(As<Cell>(search_res)->GetFirst());
                            cur->SetSecond(As<Cell>(search_res)->GetSecond());
                            is_lambda = true;
                        }
                    }
                }
            }
        }

        if (!is_lambda) {
            CreateFunctionsFromSymbols(cur->GetFirst(), is_in_define);
            CreateFunctionsFromSymbols(cur->GetSecond(), is_in_define);
        }
    }
}

void Interpreter::CreateFunctionsFromVariables(std::shared_ptr<Object> head, bool is_in_define) {
    if (Is<Cell>(head)) {
        auto cur = As<Cell>(head);

        if (Is<Symbol>(cur->GetFirst())) {
            auto name = As<Symbol>(cur->GetFirst())->GetName();

            if (name == "define") {
                is_in_define = true;
            }

            auto search_res = global_scope_->Get(name);

            if (search_res != nullptr && Is<IFunction>(search_res) && !is_in_define) {
                cur->SetFirst(search_res);
            }
        }

        CreateFunctionsFromVariables(cur->GetFirst(), is_in_define);
        CreateFunctionsFromVariables(cur->GetSecond(), is_in_define);
    }
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
            auto res =
                As<IFunctionIs>(cur->GetFirst())->IsInstanceOf(cur->GetSecond(), global_scope_);
            return std::make_shared<Boolean>(res);
        } else if (Is<QuoteFunction>(cur->GetFirst())) {  // quote function
            return As<IFunction>(cur->GetFirst())->CallInstance(cur, global_scope_);
        } else if (Is<LambdaFunction>(cur->GetFirst())) {  // lambda function
            auto temp = std::make_shared<Cell>();
            temp->SetFirst(cur);
            return As<LambdaFunction>(cur->GetFirst())->Call(temp);
        } else {
            auto res = As<IFunction>(cur->GetFirst())->CallInstance(cur, global_scope_);
            return res;
        }
    } else if (Is<Cell>(cur->GetFirst())) {  // call all function in left & right part
        // tryina catch lamda
        if (Is<LambdaFunction>(As<Cell>(cur->GetFirst())->GetFirst())) {
            auto func = As<LambdaFunction>(As<Cell>(cur->GetFirst())->GetFirst());
            if (!func->IsScopeSetted()) {
                func->SetScope(global_scope_);
            }

            return func->Call(cur);
        }

        auto calc_res = std::make_shared<Cell>();
        calc_res->SetFirst(CalcAllInner(cur->GetFirst()));

        // func return lambda
        if (Is<Cell>(calc_res->GetFirst()) &&
            Is<LambdaFunction>(As<Cell>(calc_res->GetFirst())->GetFirst())) {
            calc_res->SetSecond(cur->GetSecond());

            return As<LambdaFunction>(As<Cell>(As<Cell>(calc_res)->GetFirst())->GetFirst())
                ->Call(calc_res);
        }

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