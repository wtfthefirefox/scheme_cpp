#pragma once

#include <memory>
#include <string>
#include "error.h"
#include <cstdint>

// get type of pointer
#define RETURN_COPY_PTR(ptr)                                            \
    if (ptr == nullptr) {                                               \
        return nullptr;                                                 \
    } else if (Is<Number>(ptr)) {                                       \
        return std::make_shared<Number>(As<Number>(ptr)->GetValue());   \
    } else if (Is<Symbol>(ptr)) {                                       \
        return std::make_shared<Symbol>(As<Symbol>(ptr)->GetName());    \
    } else if (Is<Boolean>(ptr)) {                                      \
        return std::make_shared<Boolean>(As<Boolean>(ptr)->GetValue()); \
    }

#define RUN_FUNC_AS_ARG(name_func_to_rec_run, ptr_to_args, arg)                                \
    std::shared_ptr<Object> func_res = nullptr;                                                \
                                                                                               \
    if (Is<IFunctionIs>(As<Cell>(ptr_to_args->GetFirst())->GetFirst())) {                      \
        auto is_instance = As<IFunctionIs>(As<Cell>(ptr_to_args->GetFirst())->GetFirst())      \
                               ->IsInstanceOf(As<Cell>(ptr_to_args->GetFirst())->GetSecond()); \
        func_res = std::make_shared<Boolean>(is_instance);                                     \
    } else {                                                                                   \
        func_res = As<IFunction>(As<Cell>(ptr_to_args->GetFirst())->GetFirst())                \
                       ->CallInstance(ptr_to_args->GetFirst());                                \
    }                                                                                          \
                                                                                               \
    ptr_to_args->SetFirst(func_res);                                                           \
    name_func_to_rec_run(ptr, arg);

class Object : public std::enable_shared_from_this<Object> {
public:
    virtual ~Object() = default;
};

class Number : public Object {
public:
    Number(int64_t value) : value_(value){};

    int64_t GetValue() const {
        return value_;
    };

    bool operator==(const Number& rhs) const {
        return value_ == rhs.value_;
    }

    bool operator>(const Number& rhs) const {
        return value_ > rhs.value_;
    }

    bool operator>=(const Number& rhs) const {
        return value_ >= rhs.value_;
    }

    bool operator<(const Number& rhs) const {
        return value_ < rhs.value_;
    }

    bool operator<=(const Number& rhs) const {
        return value_ <= rhs.value_;
    }

    int64_t operator+(const Number& rhs) const {
        return value_ + rhs.GetValue();
    }

    int64_t operator-(const Number& rhs) const {
        return value_ - rhs.GetValue();
    }

    int64_t operator*(const Number& rhs) const {
        return value_ * rhs.GetValue();
    }

    int64_t operator/(const Number& rhs) const {
        return value_ / rhs.GetValue();
    }

private:
    int64_t value_ = 0;
};

class Symbol : public Object {
public:
    Symbol(const std::string& name) : name_(name){};

    const std::string& GetName() const {
        return name_;
    };

private:
    std::string name_;
};

class Boolean : public Object {
public:
    Boolean(bool value) : value_(value){};

    bool GetValue() const {
        return value_;
    };

private:
    bool value_ = false;
};

class Cell : public Object {
public:
    void SetFirst(std::shared_ptr<Object> new_value) {
        first_ = new_value;
    }

    void SetSecond(std::shared_ptr<Object> new_value) {
        second_ = new_value;
    }

    std::shared_ptr<Object> GetFirst() const {
        return first_;
    };
    std::shared_ptr<Object> GetSecond() const {
        return second_;
    };

private:
    std::shared_ptr<Object> first_ = nullptr;
    std::shared_ptr<Object> second_ = nullptr;
};

///////////////////////////////////////////////////////////////////////////////

// Runtime type checking and convertion.
// This can be helpful: https://en.cppreference.com/w/cpp/memory/shared_ptr/pointer_cast

template <class T>
std::shared_ptr<T> As(const std::shared_ptr<Object>& obj) {
    return std::dynamic_pointer_cast<T>(obj);
}

template <class T>
bool Is(const std::shared_ptr<Object>& obj) {
    auto ptr_test = dynamic_cast<T*>(obj.get());

    if (ptr_test) {
        return true;
    } else {
        return false;
    }
}

///////////////////////////////////////////////////////////////////////////////

// functions

class IFunction : public Object {  // factory
public:
    virtual std::shared_ptr<Object> CallInstance(std::shared_ptr<Object>) = 0;
};

class IFunctionIs : public IFunction {  // factory
public:
    virtual bool IsInstanceOf(std::shared_ptr<Object>) = 0;
};

inline void CheckArgsFunc(std::shared_ptr<Cell> ptr, bool must_be_number = true) {
    auto cur_args = As<Cell>(ptr->GetSecond());
    auto right_arg_ptr = As<Cell>(cur_args->GetSecond());

    if (Is<Cell>(cur_args->GetFirst()) &&
        Is<IFunction>(As<Cell>(cur_args->GetFirst())->GetFirst())) {  // first arg as function
        RUN_FUNC_AS_ARG(CheckArgsFunc, cur_args, must_be_number)
    }

    if (must_be_number &&
        (!Is<Number>(cur_args->GetFirst()) ||
         (!Is<Cell>(cur_args->GetSecond()) && cur_args->GetSecond() != nullptr))) {
        throw RuntimeError("args of Arithmetic Func must be a number");
    }

    if (right_arg_ptr != nullptr && Is<Cell>(right_arg_ptr->GetFirst()) &&
        Is<IFunction>(As<Cell>(right_arg_ptr->GetFirst())->GetFirst())) {  // second arg as function
        RUN_FUNC_AS_ARG(CheckArgsFunc, right_arg_ptr, must_be_number)
    }

    if (must_be_number && !Is<Number>(right_arg_ptr->GetFirst())) {
        throw RuntimeError("args of Arithmetic Func must be a number or another function");
    }
}

class QuoteFunction : public IFunction {
    // cur -> second != cell_ptr <=> ' 12312313
    // other <=> '(some_eqution)
    // () <=> nullptr
    // (()) <=> cell with first == nullptr && second == nullptr

public:
    QuoteFunction(){};  // fix '(1) => 1 and '() => (())
    std::shared_ptr<Object> CallInstance(
        std::shared_ptr<Object> head) override {  // left - function; right - args
        auto head_cell = As<Cell>(head);
        if (Is<Cell>(head_cell->GetSecond())) {
            auto cur_right = As<Cell>(head_cell->GetSecond());

            // check count of args
            if (cur_right == nullptr) {
                throw RuntimeError("Not enough args for QuoteFunction");
            }

            return cur_right;
        } else {
            return head_cell->GetSecond();
        }
    };
    ~QuoteFunction() = default;
};