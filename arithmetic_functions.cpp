#include "arithmetic_functions.h"

std::shared_ptr<Object> AsignValueToArithmeticFunc(std::shared_ptr<Object> ptr,
                                                   std::shared_ptr<IFunctionArithmetic> func_obj) {
    auto cur = As<Cell>(ptr);
    auto cur_args = As<Cell>(cur->GetSecond());

    if (cur_args == nullptr) {
        throw RuntimeError("Not enough args for Arithmetic Func");
    }

    if (cur_args->GetSecond() == nullptr) {  // drops when one arg
        throw RuntimeError("Not enough args for Arithmetic Func");
    }

    auto right_arg_ptr = As<Cell>(cur_args->GetSecond());

    CheckArgsFunc(cur);

    if (cur_args->GetSecond() == nullptr) {  // drops when one arg
        throw RuntimeError("Not enough args for Arithmetic Func");
    }

    int64_t eval_res =  // result of eval
        func_obj->EvalArgs(As<Number>(cur_args->GetFirst()), As<Number>(right_arg_ptr->GetFirst()));

    if (right_arg_ptr->GetSecond() == nullptr) {  // only two args
        return std::make_shared<Number>(eval_res);
    } else {  // two or more
        right_arg_ptr->SetFirst(std::make_shared<Number>(eval_res));
        cur->SetSecond(right_arg_ptr);
        return func_obj->CallInstance(cur);
    }
}

std::shared_ptr<Object> SumFunction::CallInstance(std::shared_ptr<Object> ptr) {
    auto cur = As<Cell>(ptr);

    if (cur->GetSecond() == nullptr) {  // 0 args
        return std::make_shared<Number>(0);
    } else {
        if (Is<Cell>(cur->GetSecond())) {                // arg is ptr
            auto args_ptr = As<Cell>(cur->GetSecond());  // ptr to first arg node

            if (Is<Number>(args_ptr->GetFirst()) && args_ptr->GetSecond() == nullptr) {
                return std::make_shared<Number>(As<Number>(args_ptr->GetFirst())->GetValue());
            } else {
                return AsignValueToArithmeticFunc(ptr, std::make_shared<SumFunction>());
            }
        } else {  // one arg
            if (Is<Number>(cur->GetSecond())) {
                return std::make_shared<Number>(As<Number>(cur->GetSecond())->GetValue());
            } else {
                throw RuntimeError("arg of sum function must be number");
            }
        }
    }
}

int64_t SumFunction::EvalArgs(std::shared_ptr<Number> ptr1, std::shared_ptr<Number> ptr2) {
    return *ptr1 + *ptr2;
}

std::shared_ptr<Object> MinusFunction::CallInstance(std::shared_ptr<Object> ptr) {
    auto cur = As<Cell>(ptr);

    if (cur->GetSecond() == nullptr) {
        throw RuntimeError("Expected at least two arguments at MinusFunction");
    }

    return AsignValueToArithmeticFunc(ptr, std::make_shared<MinusFunction>());
}

int64_t MinusFunction::EvalArgs(std::shared_ptr<Number> ptr1, std::shared_ptr<Number> ptr2) {
    return *ptr1 - *ptr2;
}

std::shared_ptr<Object> MultiplyFunction::CallInstance(std::shared_ptr<Object> ptr) {
    auto cur = As<Cell>(ptr);

    if (cur->GetSecond() == nullptr) {  // 0 args
        return std::make_shared<Number>(1);
    } else {
        auto args_ptr = As<Cell>(cur->GetSecond());  // ptr to first arg node

        return AsignValueToArithmeticFunc(ptr, std::make_shared<MultiplyFunction>());
    }
}

int64_t MultiplyFunction::EvalArgs(std::shared_ptr<Number> ptr1, std::shared_ptr<Number> ptr2) {
    return *ptr1 * *ptr2;
}

std::shared_ptr<Object> DevideFunction::CallInstance(std::shared_ptr<Object> ptr) {
    auto cur = As<Cell>(ptr);

    if (cur->GetSecond() == nullptr) {
        throw RuntimeError("Expected at least two arguments at MinusFunction");
    }

    return AsignValueToArithmeticFunc(ptr, std::make_shared<DevideFunction>());
}

int64_t DevideFunction::EvalArgs(std::shared_ptr<Number> ptr1, std::shared_ptr<Number> ptr2) {
    return *ptr1 / *ptr2;
}

std::shared_ptr<Object> MaxFunction::CallInstance(std::shared_ptr<Object> ptr) {
    auto cur = As<Cell>(ptr);

    if (cur->GetSecond() == nullptr) {  // 0 args
        throw RuntimeError("Max function requires at least one argument");
    } else {
        auto args_ptr = As<Cell>(cur->GetSecond());  // ptr to first arg node

        if (Is<Number>(args_ptr->GetFirst()) && args_ptr->GetSecond() == nullptr) {
            return std::make_shared<Number>(As<Number>(args_ptr->GetFirst())->GetValue());
        } else {
            return AsignValueToArithmeticFunc(ptr, std::make_shared<MaxFunction>());
        }
    }
}

int64_t MaxFunction::EvalArgs(std::shared_ptr<Number> ptr1, std::shared_ptr<Number> ptr2) {
    return std::max(ptr1->GetValue(), ptr2->GetValue());
}

std::shared_ptr<Object> MinFunction::CallInstance(std::shared_ptr<Object> ptr) {
    auto cur = As<Cell>(ptr);

    if (cur->GetSecond() == nullptr) {  // 0 args
        throw RuntimeError("Min function requires at least one argument");
    } else {
        auto args_ptr = As<Cell>(cur->GetSecond());  // ptr to first arg node

        if (Is<Number>(args_ptr->GetFirst()) && args_ptr->GetSecond() == nullptr) {
            return std::make_shared<Number>(As<Number>(args_ptr->GetFirst())->GetValue());
        } else {
            return AsignValueToArithmeticFunc(ptr, std::make_shared<MinFunction>());
        }
    }
}

int64_t MinFunction::EvalArgs(std::shared_ptr<Number> ptr1, std::shared_ptr<Number> ptr2) {
    return std::min(ptr1->GetValue(), ptr2->GetValue());
}

std::shared_ptr<Object> AbsFunction::CallInstance(std::shared_ptr<Object> ptr) {
    auto cur = As<Cell>(ptr);

    if (cur->GetSecond() == nullptr) {  // 0 args
        throw RuntimeError("Abs function requires one argument");
    } else {
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