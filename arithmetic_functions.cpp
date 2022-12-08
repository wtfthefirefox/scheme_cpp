#include "arithmetic_functions.h"

int64_t CalcArithmeticArgs(std::shared_ptr<Cell> args_ptr,
                           std::shared_ptr<IFunctionArithmetic> func_obj,
                           std::shared_ptr<Scope> scope) {

    auto cur_args = As<Cell>(args_ptr->GetSecond());
    auto right_arg_ptr = As<Cell>(cur_args->GetSecond());

    auto temp = As<Cell>(args_ptr->GetSecond());
    auto cur_res = std::make_shared<Number>(0);

    CheckArgsFunc(args_ptr, scope);

    int64_t eval_res =  // result of eval
        func_obj->EvalArgs(As<Number>(cur_args->GetFirst()), As<Number>(right_arg_ptr->GetFirst()));

    if (right_arg_ptr->GetSecond() == nullptr) {  // only two args
        return eval_res;
    } else {  // two or more
        right_arg_ptr->SetFirst(std::make_shared<Number>(eval_res));
        args_ptr->SetSecond(right_arg_ptr);
        return CalcArithmeticArgs(args_ptr, func_obj, scope);
    }
}

std::shared_ptr<Number> AsignValueToArithmeticFunc(std::shared_ptr<Object> ptr,
                                                   std::shared_ptr<IFunctionArithmetic> func_obj,
                                                   std::shared_ptr<Scope> scope) {
    auto cur = As<Cell>(ptr);
    auto cur_args = As<Cell>(cur->GetSecond());

    if (cur_args == nullptr) {
        throw RuntimeError("Not enough args for Arithmetic Func");
    }

    if (cur_args->GetSecond() == nullptr) {  // drops when one arg
        throw RuntimeError("Not enough args for Arithmetic Func");
    }

    if (cur_args->GetSecond() == nullptr) {  // drops when one arg
        throw RuntimeError("Not enough args for Arithmetic Func");
    }

    auto calc_obj = std::make_shared<Cell>();
    calc_obj->SetSecond(cur->GetSecond());

    return std::make_shared<Number>(CalcArithmeticArgs(calc_obj, func_obj, scope));
}

std::shared_ptr<Object> SumFunction::CallInstance(std::shared_ptr<Object> ptr,
                                                  std::shared_ptr<Scope> scope) {
    auto cur = As<Cell>(ptr);

    if (cur->GetSecond() == nullptr) {  // 0 args
        return std::make_shared<Number>(0);
    } else {
        if (Is<Cell>(cur->GetSecond())) {                // arg is ptr
            auto args_ptr = As<Cell>(cur->GetSecond());  // ptr to first arg node

            if (Is<Number>(args_ptr->GetFirst()) && args_ptr->GetSecond() == nullptr) {
                return std::make_shared<Number>(As<Number>(args_ptr->GetFirst())->GetValue());
            } else {
                return AsignValueToArithmeticFunc(ptr, std::make_shared<SumFunction>(), scope);
            }
        } else {  // one arg
            throw RuntimeError("arg of sum function must be number");
        }
    }
}

int64_t SumFunction::EvalArgs(std::shared_ptr<Number> ptr1, std::shared_ptr<Number> ptr2) {
    return *ptr1 + *ptr2;
}

std::string SumFunction::ToString() {
    return "+";
}

std::shared_ptr<Object> MinusFunction::CallInstance(std::shared_ptr<Object> ptr,
                                                    std::shared_ptr<Scope> scope) {
    auto cur = As<Cell>(ptr);

    if (cur->GetSecond() == nullptr) {
        throw RuntimeError("Expected at least two arguments at MinusFunction");
    }

    return AsignValueToArithmeticFunc(ptr, std::make_shared<MinusFunction>(), scope);
}

std::string MinusFunction::ToString() {
    return "-";
}

int64_t MinusFunction::EvalArgs(std::shared_ptr<Number> ptr1, std::shared_ptr<Number> ptr2) {
    return *ptr1 - *ptr2;
}

std::shared_ptr<Object> MultiplyFunction::CallInstance(std::shared_ptr<Object> ptr,
                                                       std::shared_ptr<Scope> scope) {
    auto cur = As<Cell>(ptr);

    if (cur->GetSecond() == nullptr) {  // 0 args
        return std::make_shared<Number>(1);
    } else {
        auto args_ptr = As<Cell>(cur->GetSecond());  // ptr to first arg node

        return AsignValueToArithmeticFunc(ptr, std::make_shared<MultiplyFunction>(), scope);
    }
}

std::string MultiplyFunction::ToString() {
    return "*";
}

int64_t MultiplyFunction::EvalArgs(std::shared_ptr<Number> ptr1, std::shared_ptr<Number> ptr2) {
    return *ptr1 * *ptr2;
}

std::shared_ptr<Object> DevideFunction::CallInstance(std::shared_ptr<Object> ptr,
                                                     std::shared_ptr<Scope> scope) {
    auto cur = As<Cell>(ptr);

    if (cur->GetSecond() == nullptr) {
        throw RuntimeError("Expected at least two arguments at MinusFunction");
    }

    return AsignValueToArithmeticFunc(ptr, std::make_shared<DevideFunction>(), scope);
}

int64_t DevideFunction::EvalArgs(std::shared_ptr<Number> ptr1, std::shared_ptr<Number> ptr2) {
    return *ptr1 / *ptr2;
}

std::string DevideFunction::ToString() {
    return "/";
}

std::shared_ptr<Object> MaxFunction::CallInstance(std::shared_ptr<Object> ptr,
                                                  std::shared_ptr<Scope> scope) {
    auto cur = As<Cell>(ptr);

    if (cur->GetSecond() == nullptr) {  // 0 args
        throw RuntimeError("Max function requires at least one argument");
    } else {
        auto args_ptr = As<Cell>(cur->GetSecond());  // ptr to first arg node

        if (Is<Number>(args_ptr->GetFirst()) && args_ptr->GetSecond() == nullptr) {
            return std::make_shared<Number>(As<Number>(args_ptr->GetFirst())->GetValue());
        } else {
            return AsignValueToArithmeticFunc(ptr, std::make_shared<MaxFunction>(), scope);
        }
    }
}

int64_t MaxFunction::EvalArgs(std::shared_ptr<Number> ptr1, std::shared_ptr<Number> ptr2) {
    return std::max(ptr1->GetValue(), ptr2->GetValue());
}

std::string MaxFunction::ToString() {
    return "max";
}

std::shared_ptr<Object> MinFunction::CallInstance(std::shared_ptr<Object> ptr,
                                                  std::shared_ptr<Scope> scope) {
    auto cur = As<Cell>(ptr);

    if (cur->GetSecond() == nullptr) {  // 0 args
        throw RuntimeError("Min function requires at least one argument");
    } else {
        auto args_ptr = As<Cell>(cur->GetSecond());  // ptr to first arg node

        if (Is<Number>(args_ptr->GetFirst()) && args_ptr->GetSecond() == nullptr) {
            return std::make_shared<Number>(As<Number>(args_ptr->GetFirst())->GetValue());
        } else {
            return AsignValueToArithmeticFunc(ptr, std::make_shared<MinFunction>(), scope);
        }
    }
}

int64_t MinFunction::EvalArgs(std::shared_ptr<Number> ptr1, std::shared_ptr<Number> ptr2) {
    return std::min(ptr1->GetValue(), ptr2->GetValue());
}

std::string MinFunction::ToString() {
    return "min";
}

std::shared_ptr<Object> AbsFunction::CallInstance(std::shared_ptr<Object> ptr,
                                                  std::shared_ptr<Scope>) {
    auto cur = As<Cell>(ptr);

    if (cur->GetSecond() == nullptr) {  // 0 args
        throw RuntimeError("Abs function requires one argument");
    } else {
        if (!Is<Cell>(cur->GetSecond())) {
            throw SyntaxError("Abs function requires one argument");
        }
        auto args_ptr = As<Cell>(cur->GetSecond());  // ptr to first arg node

        if (Is<Number>(args_ptr->GetFirst()) && args_ptr->GetSecond() == nullptr) {
            return std::make_shared<Number>(
                EvalArgs(As<Number>(args_ptr->GetFirst()), std::make_shared<Number>(0)));
        } else {
            throw RuntimeError("Abs function requires one Number argument");
        }
    }
}

int64_t AbsFunction::EvalArgs(std::shared_ptr<Number> ptr1, std::shared_ptr<Number>) {
    return std::abs(ptr1->GetValue());
}

std::string AbsFunction::ToString() {
    return "abs";
}