#include "define_functions.h"

void CheckDefineFunctionsArgs(std::shared_ptr<Object> ptr, std::shared_ptr<Scope> scope,
                              std::shared_ptr<Object> = nullptr) {
    if (As<Cell>(ptr)->GetSecond() == nullptr) {
        throw SyntaxError("define functions requires a argument");
    }

    auto cur = As<Cell>(As<Cell>(ptr)->GetSecond());

    if (Is<Boolean>(cur->GetFirst()) || Is<Number>(cur->GetFirst()) || cur->GetFirst() == nullptr) {
        throw SyntaxError("define functions argument must be a symbol or Cell");
    }
}

void ReplaceSameOperator(std::shared_ptr<Object> head, std::shared_ptr<Scope> scope,
                         const std::string& name) {
    if (Is<Cell>(head)) {
        auto cur = As<Cell>(head);

        if (Is<IFunction>(cur->GetFirst()) && As<IFunction>(cur->GetFirst())->ToString() == name) {
            cur->SetFirst(std::make_shared<Symbol>(name));
        }

        ReplaceSameOperator(cur->GetFirst(), scope, name);
        ReplaceSameOperator(cur->GetSecond(), scope, name);
    }
}

void ReplaceSymbolOperators(std::shared_ptr<Object> head, std::shared_ptr<Scope> scope) {
    if (Is<Cell>(head)) {
        auto cur = As<Cell>(head);

        if (Is<Symbol>(cur->GetFirst())) {
            auto finded_res = scope->Get(As<Symbol>(cur->GetFirst())->GetName());
            if (Is<IFunction>(finded_res)) {
                cur->SetFirst(finded_res);
            }
        }

        ReplaceSymbolOperators(cur->GetFirst(), scope);
        ReplaceSymbolOperators(cur->GetSecond(), scope);
    }
}

std::shared_ptr<Object> DefineFunction::CallInstance(std::shared_ptr<Object> ptr,
                                                     std::shared_ptr<Scope> scope) {
    auto cur = As<Cell>(As<Cell>(ptr)->GetSecond());
    CheckDefineFunctionsArgs(ptr, scope);

    auto value = As<Cell>(cur->GetSecond())->GetFirst();

    // define (func args..) (body)
    if (Is<Cell>(cur->GetFirst())) {
        auto header = As<Cell>(cur->GetFirst());
        if (!Is<Symbol>(header->GetFirst()) && !Is<IFunction>(header->GetFirst())) {
            throw SyntaxError("function in define function must have a name");
        }

        std::string name;

        if (Is<Symbol>(header->GetFirst())) {
            name = As<Symbol>(header->GetFirst())->GetName();
        } else {
            name = As<IFunction>(header->GetFirst())->ToString();
        }

        ReplaceSameOperator(cur->GetSecond(), scope, name);
        ReplaceSymbolOperators(ptr, scope);

        auto func = std::make_shared<LambdaFunction>();
        auto another_values = As<Cell>(cur->GetSecond())->GetSecond();
        func->InitInner(header->GetSecond());
        auto func_head = std::make_shared<Cell>();
        func_head->SetFirst(func);
        func_head->SetSecond(std::make_shared<Cell>());
        As<Cell>(func_head->GetSecond())->SetFirst(header->GetSecond());
        As<Cell>(func_head->GetSecond())->SetSecond(std::make_shared<Cell>());
        As<Cell>(As<Cell>(func_head->GetSecond())->GetSecond())->SetFirst(value);
        As<Cell>(As<Cell>(func_head->GetSecond())->GetSecond())->SetSecond(another_values);

        scope->Define(name, func_head);

        return nullptr;
    }

    std::string var_name;

    if (Is<Symbol>(cur->GetFirst())) {
        var_name = As<Symbol>(cur->GetFirst())->GetName();
    } else {
        var_name = As<IFunction>(cur->GetFirst())->ToString();
    }

    // run lambda with args - define new_func (another_func args...)

    if (Is<Cell>(value) && Is<Symbol>(As<Cell>(value)->GetFirst())) {
        auto search_res = scope->Get(As<Symbol>(As<Cell>(value)->GetFirst())->GetName());

        if (search_res != nullptr && Is<Cell>(search_res) &&
            Is<LambdaFunction>(As<Cell>(search_res)->GetFirst())) {
            As<Cell>(value)->SetFirst(search_res);
        }
    }

    if (Is<Cell>(value) && Is<Cell>(As<Cell>(value)->GetFirst()) &&
        Is<LambdaFunction>(As<Cell>(As<Cell>(value)->GetFirst())->GetFirst())) {
        auto copy_lambda = CopyAst(value);
        auto copy_inner_args =
            As<Cell>(As<Cell>(As<Cell>(copy_lambda)->GetFirst())->GetSecond())->GetFirst();
        auto copy_outer_args = As<Cell>(copy_lambda)->GetSecond();
        As<Cell>(copy_lambda)->SetSecond(nullptr);
        As<Cell>(As<Cell>(As<Cell>(copy_lambda)->GetFirst())->GetSecond())->SetFirst(nullptr);

        auto func = As<LambdaFunction>(As<Cell>(As<Cell>(copy_lambda)->GetFirst())->GetFirst());
        func->SetScope(scope);

        while (copy_inner_args != nullptr) {
            func->DefineNewVarInScope(As<Symbol>(As<Cell>(copy_inner_args)->GetFirst())->GetName(),
                                      As<Cell>(copy_outer_args)->GetFirst());
            copy_inner_args = As<Cell>(copy_inner_args)->GetSecond();
            copy_outer_args = As<Cell>(copy_outer_args)->GetSecond();
        }

        scope->Define(var_name, copy_lambda);

        return nullptr;
    }

    if (cur->GetSecond() == nullptr || As<Cell>(cur->GetSecond())->GetSecond() != nullptr) {
        throw SyntaxError("define functions requires two arguments");
    }

    if (Is<Cell>(value) && Is<LambdaFunction>(As<Cell>(value)->GetFirst())) {
        As<LambdaFunction>(As<Cell>(value)->GetFirst())
            ->InitInner(As<Cell>(As<Cell>(value)->GetSecond())->GetFirst());
        scope->Define(As<Symbol>(cur->GetFirst())->GetName(), value);

        return nullptr;
    }

    CheckArgsFunc(As<Cell>(ptr), scope, false, false, true);  // compile second arg

    value = As<Cell>(cur->GetSecond())->GetFirst();

    if (Is<Symbol>(value) && var_name == As<Symbol>(value)->GetName()) {
        throw NameError("can not define variable the same name as symbol value");
    }

    // name from scope
    if (Is<Symbol>(value)) {
        std::shared_ptr<Object> search_res_name = scope->Get(As<Symbol>(value)->GetName());
        if (search_res_name != nullptr) {
            scope->Define(var_name, search_res_name);

            return nullptr;
        }
    }

    scope->Define(var_name, value);

    return nullptr;
}

std::shared_ptr<Object> SetFunction::CallInstance(std::shared_ptr<Object> ptr,
                                                  std::shared_ptr<Scope> scope) {
    auto cur = As<Cell>(As<Cell>(ptr)->GetSecond());
    CheckDefineFunctionsArgs(ptr, scope);

    if (cur->GetSecond() == nullptr || As<Cell>(cur->GetSecond())->GetSecond() != nullptr) {
        throw SyntaxError("define functions requires two arguments");
    }

    CheckArgsFunc(As<Cell>(ptr), scope, false, false, true);  // compile second arg

    auto var_name = As<Symbol>(cur->GetFirst())->GetName();
    auto value = As<Cell>(cur->GetSecond())->GetFirst();

    if (scope->Get(var_name) == nullptr) {
        throw NameError("Undefined variable in set function");
    }

    scope->Set(var_name, value);

    return nullptr;
}

// !!may be need to create helper for cdr and car functions
std::shared_ptr<Object> SetCarFunction::CallInstance(std::shared_ptr<Object> ptr,
                                                     std::shared_ptr<Scope> scope) {
    auto cur = As<Cell>(As<Cell>(ptr)->GetSecond());
    CheckDefineFunctionsArgs(ptr, scope);
    CheckArgsFunc(As<Cell>(ptr), scope, false, false, true);

    if (!Is<Symbol>(cur->GetFirst())) {  // compile first arg
        RUN_FUNC_AS_ARG(CheckDefineFunctionsArgs, cur, scope, nullptr)

        As<Cell>(cur->GetFirst())->SetFirst(As<Cell>(cur->GetSecond())->GetFirst());

        return nullptr;
    }

    auto var_name = As<Symbol>(cur->GetFirst())->GetName();
    auto value = As<Cell>(cur->GetSecond())->GetFirst();

    auto variable_ptr = scope->Get(var_name);

    if (variable_ptr == nullptr) {
        throw NameError("Undefined variable in set-car! function");
    }

    if (!Is<Cell>(variable_ptr)) {
        throw NameError("variable provided to ser-car! function must be a list");
    }

    As<Cell>(variable_ptr)->SetFirst(value);

    return nullptr;
}

std::shared_ptr<Object> SetCdrFunction::CallInstance(std::shared_ptr<Object> ptr,
                                                     std::shared_ptr<Scope> scope) {
    auto cur = As<Cell>(As<Cell>(ptr)->GetSecond());
    CheckDefineFunctionsArgs(ptr, scope);
    CheckArgsFunc(As<Cell>(ptr), scope, false, false, true);  // compile second arg

    if (!Is<Symbol>(cur->GetFirst())) {  // compile first arg
        RUN_FUNC_AS_ARG(CheckDefineFunctionsArgs, cur, scope, nullptr)

        As<Cell>(cur->GetFirst())->SetSecond(As<Cell>(cur->GetSecond())->GetFirst());

        return nullptr;
    }

    auto var_name = As<Symbol>(cur->GetFirst())->GetName();
    auto value = As<Cell>(cur->GetSecond())->GetFirst();

    auto variable_ptr = scope->Get(var_name);

    if (variable_ptr == nullptr) {
        throw NameError("Undefined variable in set-cdr! function");
    }

    if (!Is<Cell>(variable_ptr)) {
        throw NameError("variable provided to ser-cdr! function must be a list");
    }

    As<Cell>(variable_ptr)->SetSecond(value);

    return nullptr;
}