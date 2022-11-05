#include "is_functions.h"

void CheckIsFunctionArgs(std::shared_ptr<Object> ptr, bool can_be_null = false) {
    if (!can_be_null && ptr == nullptr) {
        throw RuntimeError("Is functions requires one argument");
    }

    if (Is<Cell>(ptr)) {
        auto cur = As<Cell>(ptr);

        if (cur->GetFirst() != nullptr && cur->GetSecond() != nullptr) {
            throw RuntimeError("Is functions requires one argument");
        }

        auto cur_args = As<Cell>(ptr);

        if (Is<Cell>(cur_args->GetFirst()) &&
            Is<IFunction>(As<Cell>(cur_args->GetFirst())->GetFirst())) {  // first arg as function
            RUN_FUNC_AS_ARG(CheckIsFunctionArgs, cur_args, can_be_null)
        }
    }
}

bool IsNumberFunction::IsInstanceOf(std::shared_ptr<Object> ptr) {
    CheckIsFunctionArgs(ptr);

    if (Is<Number>(ptr)) {
        return true;
    }

    if (Is<Cell>(ptr)) {  // read like cell with first arg as number
        auto cur = As<Cell>(ptr);

        if (Is<Number>(cur->GetFirst()) && cur->GetSecond() == nullptr) {
            return true;
        }
    }

    return false;
}

bool IsBooleanFunction::IsInstanceOf(std::shared_ptr<Object> ptr) {
    CheckIsFunctionArgs(ptr);

    if (Is<Boolean>(ptr)) {
        return true;
    }

    if (Is<Cell>(ptr)) {  // read like cell with first arg as boolean
        auto cur = As<Cell>(ptr);

        if (Is<Boolean>(cur->GetFirst()) && cur->GetSecond() == nullptr) {
            return true;
        }
    }

    return false;
}

bool NotFunction::IsInstanceOf(std::shared_ptr<Object> ptr) {
    CheckIsFunctionArgs(ptr);

    if (Is<Boolean>(ptr)) {
        if (As<Boolean>(ptr)->GetValue()) {
            return false;
        }

        return true;
    }

    auto cur = As<Cell>(ptr);

    if (Is<Boolean>(cur->GetFirst()) && cur->GetSecond() == nullptr) {
        if (As<Boolean>(cur->GetFirst())->GetValue()) {
            return false;
        }
        return true;
    }

    return false;
}

bool IsPairFunction::IsInstanceOf(std::shared_ptr<Object> ptr) {
    CheckIsFunctionArgs(ptr, true);

    if (As<Cell>(ptr)->GetFirst() == nullptr || Is<Symbol>(As<Cell>(ptr)->GetFirst()) ||
        Is<Number>(As<Cell>(ptr)->GetFirst())) {
        return false;
    }

    return true;
}

bool IsNullFunction::IsInstanceOf(std::shared_ptr<Object> ptr) {
    CheckIsFunctionArgs(ptr, true);

    if (As<Cell>(ptr)->GetFirst() == nullptr) {
        return true;
    }

    return false;
}

bool CheckArgsInList(std::shared_ptr<Object> ptr) {
    if (ptr == nullptr) {
        return true;
    } else if (Is<Symbol>(ptr) || Is<Number>(ptr)) {  // can only in left part
        return true;
    } else if (Is<Symbol>(As<Cell>(ptr)->GetSecond()) || Is<Number>(As<Cell>(ptr)->GetSecond())) {
        return false;
    }

    auto left = CheckArgsInList(As<Cell>(ptr)->GetFirst());
    auto right = CheckArgsInList(As<Cell>(ptr)->GetSecond());

    if (left && right) {
        return true;
    } else {
        return false;
    }
}

bool IsListFunction::IsInstanceOf(std::shared_ptr<Object> ptr) {
    CheckIsFunctionArgs(ptr, true);

    if (Is<Symbol>(As<Cell>(ptr)->GetFirst()) || Is<Number>(As<Cell>(ptr)->GetFirst())) {
        return false;
    }

    return CheckArgsInList(ptr);
}