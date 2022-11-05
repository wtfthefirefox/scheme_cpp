#include "list_functions.h"

std::shared_ptr<Object> ConsFunction::CallInstance(std::shared_ptr<Object> head) {
    CheckArgsFunc(As<Cell>(head), false);

    std::shared_ptr<Object> last_head = nullptr;
    std::shared_ptr<Object> last_tail = head;

    while (last_tail != nullptr && As<Cell>(last_tail)->GetSecond() != nullptr) {
        last_head = last_tail;
        last_tail = As<Cell>(last_tail)->GetSecond();
    }

    As<Cell>(last_head)->SetSecond(As<Cell>(last_tail)->GetFirst());

    return As<Cell>(head)->GetSecond();
}

std::shared_ptr<Object> CarFunction::CallInstance(std::shared_ptr<Object> head) {
    CheckArgsFunc(As<Cell>(head), false);

    auto cur_args = As<Cell>(head)->GetSecond();

    if (As<Cell>(cur_args)->GetFirst() == nullptr) {
        throw RuntimeError("car function expect at least one arg");
    }

    return As<Cell>(As<Cell>(cur_args)->GetFirst())->GetFirst();
}

std::shared_ptr<Object> CdrFunction::CallInstance(std::shared_ptr<Object> head) {
    CheckArgsFunc(As<Cell>(head), false);

    auto cur_args = As<Cell>(head)->GetSecond();

    if (As<Cell>(cur_args)->GetFirst() == nullptr) {
        throw RuntimeError("car function expect at least one arg");
    }

    return As<Cell>(As<Cell>(cur_args)->GetFirst())->GetSecond();
}

std::shared_ptr<Object> ListFuntion::CallInstance(std::shared_ptr<Object> head) {
    if (As<Cell>(head)->GetSecond() == nullptr) {
        return nullptr;
    }

    CheckArgsFunc(As<Cell>(head), false);

    return As<Cell>(head)->GetSecond();
}

std::shared_ptr<Object> ListRefFuntion::CallInstance(std::shared_ptr<Object> head) {
    if (As<Cell>(head)->GetSecond() == nullptr) {
        throw RuntimeError("empty equation");
    }

    CheckArgsFunc(As<Cell>(head), false);
    auto cur_args = As<Cell>(head)->GetSecond();

    if (As<Cell>(cur_args)->GetSecond() == nullptr) {
        throw RuntimeError("not enough second arg in list-ref function");
    }

    if (!Is<Number>(As<Cell>(As<Cell>(cur_args)->GetSecond())->GetFirst())) {
        throw RuntimeError("second arg in list-ref function must be a number");
    }

    int64_t idx = As<Number>(As<Cell>(As<Cell>(cur_args)->GetSecond())->GetFirst())->GetValue();
    std::shared_ptr<Object> temp = As<Cell>(cur_args)->GetFirst();

    while (temp != nullptr && idx > 0) {
        temp = As<Cell>(temp)->GetSecond();
        --idx;
    }

    if (temp != nullptr) {
        return As<Cell>(temp)->GetFirst();
    }

    throw RuntimeError("idx should be less than len of sequence");
}

std::shared_ptr<Object> ListTailFunction::CallInstance(std::shared_ptr<Object> head) {
    if (As<Cell>(head)->GetSecond() == nullptr) {
        throw RuntimeError("empty equation");
    }

    CheckArgsFunc(As<Cell>(head), false);
    auto cur_args = As<Cell>(head)->GetSecond();

    if (As<Cell>(cur_args)->GetSecond() == nullptr) {
        throw RuntimeError("not enough second arg in list-ref function");
    }

    if (!Is<Number>(As<Cell>(As<Cell>(cur_args)->GetSecond())->GetFirst())) {
        throw RuntimeError("second arg in list-ref function must be a number");
    }

    int64_t idx = As<Number>(As<Cell>(As<Cell>(cur_args)->GetSecond())->GetFirst())->GetValue();

    std::shared_ptr<Object> last_head = nullptr;
    std::shared_ptr<Object> last_tail = As<Cell>(cur_args)->GetFirst();

    while (last_tail != nullptr && As<Cell>(last_tail)->GetSecond() != nullptr && idx > 0) {
        last_head = last_tail;
        last_tail = As<Cell>(last_tail)->GetSecond();
        --idx;
    }

    if (last_tail != nullptr && idx <= 1) {
        if (As<Cell>(last_tail)->GetSecond() == nullptr && idx == 1) {
            return As<Cell>(last_tail)->GetSecond();
        }

        return last_tail;
    }

    throw RuntimeError("idx should be less than len of sequence");
}