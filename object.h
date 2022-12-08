#pragma once

#include <memory>
#include <string>
#include <unordered_map>
#include <cstdint>

#include "error.h"

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

#define RUN_FUNC_AS_ARG(name_func_to_rec_run, ptr_to_args, scope, ...)                            \
    std::shared_ptr<Object> func_res = nullptr;                                                   \
    if (Is<IFunctionIs>(As<Cell>(ptr_to_args->GetFirst())->GetFirst())) {                         \
        auto is_instance =                                                                        \
            As<IFunctionIs>(As<Cell>(ptr_to_args->GetFirst())->GetFirst())                        \
                ->IsInstanceOf(As<Cell>(ptr_to_args->GetFirst())->GetSecond(), scope);            \
        func_res = std::make_shared<Boolean>(is_instance);                                        \
    } else if (Is<LambdaFunction>(As<Cell>(ptr_to_args->GetFirst())->GetFirst())) {               \
        auto func_compiled_args = std::make_shared<Cell>();                                       \
        auto func_compiled_ptr = func_compiled_args;                                              \
        auto func_uncompiled_ptr = As<Cell>(As<Cell>(ptr_to_args->GetFirst())->GetSecond());      \
        auto args_cur_func = As<Cell>(                                                            \
            As<LambdaFunction>(As<Cell>(ptr_to_args->GetFirst())->GetFirst())->GetRequiedArgs()); \
                                                                                                  \
        if (args_cur_func == nullptr) {                                                           \
            func_compiled_args = nullptr;                                                         \
        }                                                                                         \
                                                                                                  \
        while (args_cur_func != nullptr) {                                                        \
            if (Is<Cell>(As<Cell>(func_uncompiled_ptr)->GetFirst()) &&                            \
                Is<IFunction>(As<Cell>(As<Cell>(func_uncompiled_ptr)->GetFirst())->GetFirst())) { \
                std::shared_ptr<Object> func_res = nullptr;                                       \
                                                                                                  \
                if (Is<IFunctionIs>(As<Cell>(func_uncompiled_ptr)->GetFirst())) {                 \
                    auto is_instance =                                                            \
                        As<IFunctionIs>(                                                          \
                            As<Cell>(As<Cell>(func_uncompiled_ptr)->GetFirst())->GetFirst())      \
                            ->IsInstanceOf(                                                       \
                                As<Cell>(As<Cell>(func_uncompiled_ptr)->GetFirst())->GetSecond(), \
                                scope);                                                           \
                    func_res = std::make_shared<Boolean>(is_instance);                            \
                } else {                                                                          \
                    func_res =                                                                    \
                        As<IFunction>(                                                            \
                            As<Cell>(As<Cell>(func_uncompiled_ptr)->GetFirst())->GetFirst())      \
                            ->CallInstance(As<Cell>(func_uncompiled_ptr)->GetFirst(), scope);     \
                }                                                                                 \
                                                                                                  \
                func_compiled_ptr->SetFirst(func_res);                                            \
            } else if (Is<Symbol>(As<Cell>(func_uncompiled_ptr)->GetFirst())) {                   \
                auto founded_ptr =                                                                \
                    scope->Get(As<Symbol>(As<Cell>(func_uncompiled_ptr)->GetFirst())->GetName()); \
                                                                                                  \
                if (founded_ptr == nullptr) {                                                     \
                    throw SyntaxError("can not provide symbol as arg to func");                   \
                }                                                                                 \
                                                                                                  \
                func_compiled_ptr->SetFirst(founded_ptr);                                         \
            } else if (Is<Number>(As<Cell>(func_uncompiled_ptr)->GetFirst())) {                   \
                func_compiled_ptr->SetFirst(As<Cell>(func_uncompiled_ptr)->GetFirst());           \
            }                                                                                     \
                                                                                                  \
            if (args_cur_func->GetSecond() != nullptr) {                                          \
                func_compiled_ptr->SetSecond(std::make_shared<Cell>());                           \
            }                                                                                     \
            func_compiled_ptr = As<Cell>(func_compiled_ptr->GetSecond());                         \
            args_cur_func = As<Cell>(args_cur_func->GetSecond());                                 \
            func_uncompiled_ptr = As<Cell>(func_uncompiled_ptr->GetSecond());                     \
        }                                                                                         \
                                                                                                  \
        auto func_body = std::make_shared<Cell>();                                                \
        func_body->SetFirst(ptr_to_args->GetFirst());                                             \
        func_body->SetSecond(func_compiled_args);                                                 \
                                                                                                  \
        func_res =                                                                                \
            As<LambdaFunction>(As<Cell>(ptr_to_args->GetFirst())->GetFirst())->Call(func_body);   \
    } else {                                                                                      \
        func_res = As<IFunction>(As<Cell>(ptr_to_args->GetFirst())->GetFirst())                   \
                       ->CallInstance(ptr_to_args->GetFirst(), scope);                            \
    }                                                                                             \
                                                                                                  \
    ptr_to_args->SetFirst(func_res);                                                              \
    name_func_to_rec_run(ptr, scope, __VA_ARGS__);

class Object : public std::enable_shared_from_this<Object> {
public:
    virtual ~Object() = default;
};

inline std::shared_ptr<Object> CopyAst(std::shared_ptr<Object> ptr);

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

class Scope {
public:
    Scope(){};
    Scope(std::shared_ptr<Scope> ptr) : upper_scope_(ptr){};

    std::shared_ptr<Object> Get(const std::string& name) {
        if (data_.find(name) != data_.end()) {
            return data_[name];
        }

        if (upper_scope_ != nullptr) {
            auto search_res = upper_scope_->Get(name);

            if (search_res != nullptr) {
                return search_res;
            }
        }

        return nullptr;
    }

    void Set(const std::string& name, std::shared_ptr<Object> ptr) {
        if (data_.find(name) != data_.end()) {
            data_[name] = ptr;
        } else if (upper_scope_ != nullptr) {
            upper_scope_->Set(name, ptr);
        }
    }

    void Define(const std::string& name, std::shared_ptr<Object> ptr) {
        data_[name] = ptr;
    }

private:
    std::unordered_map<std::string, std::shared_ptr<Object>> data_;
    std::shared_ptr<Scope> upper_scope_ = nullptr;
};

// functions

class IFunction : public Object {  // factory
public:
    virtual std::shared_ptr<Object> CallInstance(std::shared_ptr<Object>,
                                                 std::shared_ptr<Scope>) = 0;
    virtual std::string ToString() {
        return "function";
    };
};

class IFunctionDefine : public IFunction {};  // using only for type checking

class IFunctionIs : public IFunction {  // factory
public:
    virtual bool IsInstanceOf(std::shared_ptr<Object>, std::shared_ptr<Scope>) = 0;
};

class LambdaFunction;

inline std::shared_ptr<Object> CompileArgs(std::shared_ptr<Cell> func_uncompiled_ptr,
                                           std::shared_ptr<Cell> args_cur_func,
                                           std::shared_ptr<Scope> scope) {
    auto func_compiled_args = std::make_shared<Cell>();
    auto func_compiled_ptr = func_compiled_args;

    if (args_cur_func == nullptr) {
        func_compiled_args = nullptr;
    }

    while (args_cur_func != nullptr) {
        if (Is<Cell>(As<Cell>(func_uncompiled_ptr)->GetFirst()) &&
            Is<IFunction>(As<Cell>(As<Cell>(func_uncompiled_ptr)->GetFirst())->GetFirst())) {
            std::shared_ptr<Object> func_res;

            if (Is<IFunctionIs>(As<Cell>(func_uncompiled_ptr)->GetFirst())) {
                auto is_instance =
                    As<IFunctionIs>(As<Cell>(As<Cell>(func_uncompiled_ptr)->GetFirst())->GetFirst())
                        ->IsInstanceOf(
                            As<Cell>(As<Cell>(func_uncompiled_ptr)->GetFirst())->GetSecond(),
                            scope);
                func_res = std::make_shared<Boolean>(is_instance);
            } else {
                func_res =
                    As<IFunction>(As<Cell>(As<Cell>(func_uncompiled_ptr)->GetFirst())->GetFirst())
                        ->CallInstance(As<Cell>(func_uncompiled_ptr)->GetFirst(), scope);
            }

            func_compiled_ptr->SetFirst(func_res);
        } else if (Is<Symbol>(As<Cell>(func_uncompiled_ptr)->GetFirst())) {
            auto founded_ptr =
                scope->Get(As<Symbol>(As<Cell>(func_uncompiled_ptr)->GetFirst())->GetName());

            if (founded_ptr == nullptr) {
                throw SyntaxError("can not provide symbol as arg to func");
            }

            func_compiled_ptr->SetFirst(CopyAst(founded_ptr));  // copy func
        } else if (Is<Number>(As<Cell>(func_uncompiled_ptr)->GetFirst())) {
            func_compiled_ptr->SetFirst(As<Cell>(func_uncompiled_ptr)->GetFirst());
        }

        if (args_cur_func->GetSecond() != nullptr) {
            func_compiled_ptr->SetSecond(std::make_shared<Cell>());
        }
        func_compiled_ptr = As<Cell>(func_compiled_ptr->GetSecond());
        args_cur_func = As<Cell>(args_cur_func->GetSecond());
        func_uncompiled_ptr = As<Cell>(func_uncompiled_ptr->GetSecond());
    }

    return func_compiled_args;
}

class LambdaFunction : public IFunction {
public:
    LambdaFunction(){};

    std::shared_ptr<Object> CallInstance(std::shared_ptr<Object>, std::shared_ptr<Scope>) {
        return nullptr;
    };

    // instant call
    // if not enough args - it drops inner content
    std::shared_ptr<Object> Call(std::shared_ptr<Object> ptr, bool is_called_from_lambda = false) {
        // check count of inner and outer args
        if (required_args_ == nullptr) {
            InitInnerFromLambda(ptr);
        }

        auto cur_args = As<Cell>(As<Cell>(As<Cell>(ptr)->GetFirst())->GetSecond());
        auto left_tree = required_args_;
        auto right_tree = As<Cell>(ptr)->GetSecond();

        while (left_tree != nullptr && right_tree != nullptr) {
            if (!Is<Symbol>(As<Cell>(left_tree)->GetFirst())) {
                throw SyntaxError("arg of function must be a symbol");
            }
            left_tree = As<Cell>(left_tree)->GetSecond();
            right_tree = As<Cell>(right_tree)->GetSecond();
        }

        if (left_tree != nullptr || right_tree != nullptr) {
            if (!is_called_from_lambda) {
                throw RuntimeError("Not enough args to lambda function");
            }

            return As<Cell>(ptr)->GetFirst();
        }

        left_tree = required_args_;
        right_tree = As<Cell>(ptr)->GetSecond();

        // set variables to local scope from args
        while (right_tree != nullptr) {
            inner_scope_->Define(As<Symbol>(As<Cell>(left_tree)->GetFirst())->GetName(),
                                 As<Cell>(right_tree)->GetFirst());
            left_tree = As<Cell>(left_tree)->GetSecond();
            right_tree = As<Cell>(right_tree)->GetSecond();
        }

        // compile body & return res
        std::shared_ptr<Object> compile_body_res = nullptr;
        auto body_ptr = As<Cell>(As<Cell>(cur_args)->GetSecond());

        while (body_ptr != nullptr) {
            if (Is<Symbol>(body_ptr->GetFirst())) {
                auto name = As<Symbol>(body_ptr->GetFirst())->GetName();
                auto search_res = inner_scope_->Get(name);

                if (search_res != nullptr) {
                    compile_body_res = search_res;

                    if (Is<Cell>(compile_body_res) &&
                        Is<LambdaFunction>(As<Cell>(compile_body_res)->GetFirst())) {
                        As<LambdaFunction>(As<Cell>(compile_body_res)->GetFirst())
                            ->SetScope(inner_scope_);
                    }
                }
            } else if (Is<Number>(body_ptr->GetFirst())) {
                compile_body_res = body_ptr->GetFirst();
            } else {
                auto function_head_ptr = As<Cell>(body_ptr->GetFirst());
                if (Is<IFunctionIs>(function_head_ptr->GetFirst())) {
                    auto func_res =
                        As<IFunctionIs>(function_head_ptr->GetFirst())
                            ->IsInstanceOf(function_head_ptr->GetSecond(), inner_scope_);
                    compile_body_res = std::make_shared<Boolean>(func_res);
                } else if (Is<LambdaFunction>(function_head_ptr->GetFirst())) {
                    auto func = As<LambdaFunction>(function_head_ptr->GetFirst());
                    func->SetScope(inner_scope_);

                    compile_body_res = func->Call(body_ptr, true);
                } else {
                    // try to avoid copy
                    auto copy_func = As<Cell>(CopyAst(function_head_ptr));
                    auto name = As<IFunction>(copy_func->GetFirst())->ToString();
                    auto search_res = inner_scope_->Get(name);

                    if (search_res != nullptr) {
                        copy_func->SetFirst(search_res);
                        auto func_compiled_args = CompileArgs(
                            As<Cell>(As<Cell>(copy_func)->GetSecond()),
                            As<Cell>(As<LambdaFunction>(As<Cell>(search_res)->GetFirst())
                                         ->GetRequiedArgs()),
                            inner_scope_);

                        auto func_body = std::make_shared<Cell>();
                        func_body->SetFirst(search_res);
                        func_body->SetSecond(func_compiled_args);

                        As<LambdaFunction>(As<Cell>(search_res)->GetFirst())
                            ->SetScope(inner_scope_);
                        compile_body_res = As<LambdaFunction>(As<Cell>(search_res)->GetFirst())
                                               ->Call(func_body, true);
                    } else {
                        compile_body_res = As<IFunction>(copy_func->GetFirst())
                                               ->CallInstance(copy_func, inner_scope_);
                    }
                }
            }

            body_ptr = As<Cell>(body_ptr->GetSecond());
        }

        return compile_body_res;
    };

    // slow call
    void InitInnerFromLambda(std::shared_ptr<Object> ptr) {
        auto cur_args = As<Cell>(As<Cell>(As<Cell>(ptr)->GetFirst())->GetSecond());
        if (cur_args == nullptr) {
            throw SyntaxError("Lambda function can not be without args");
        }
        required_args_ = cur_args->GetFirst();
    };

    void InitInner(std::shared_ptr<Object> args) {
        required_args_ = args;
    };

    bool IsScopeSetted() const {
        return inner_scope_ != nullptr;
    }

    void SetScope(std::shared_ptr<Scope> scope) {
        inner_scope_ = std::make_shared<Scope>(scope);
    }

    void DefineNewVarInScope(const std::string& name, std::shared_ptr<Object> ptr) {
        inner_scope_->Define(name, ptr);
    }

    std::shared_ptr<Object> GetRequiedArgs() const {
        return required_args_;
    }

private:
    std::shared_ptr<Scope> inner_scope_ = nullptr;
    std::shared_ptr<Object> required_args_ = nullptr;
};

inline std::shared_ptr<Object> CompileArgAsFunc(std::shared_ptr<Object> ptr,
                                                std::shared_ptr<Scope> scope) {
    if (ptr == nullptr) {
        return nullptr;
    } else if (Is<Number>(ptr) || Is<Boolean>(ptr)) {
        return ptr;
    } else if (Is<Symbol>(ptr)) {
        auto search_res = scope->Get(As<Symbol>(ptr)->GetName());

        if (search_res != nullptr) {
            return search_res;
        } else {
            throw RuntimeError("Undefined variable");
        }
    }

    // Cell

    auto cur = As<Cell>(ptr);
    auto cur_args = As<Cell>(cur->GetSecond());
    std::shared_ptr<Object> search_res_arg;

    if (!Is<IFunctionDefine>(cur->GetFirst()) && cur_args != nullptr &&
        Is<Symbol>(cur_args->GetFirst())) {
        search_res_arg = scope->Get(As<Symbol>(cur_args->GetFirst())->GetName());
    }

    if (!Is<IFunctionDefine>(cur->GetFirst()) && cur_args != nullptr &&
        Is<Symbol>(cur_args->GetFirst()) && search_res_arg != nullptr) {
        return search_res_arg;
    }

    // catch compiled lambda...
    if (Is<Cell>(cur_args->GetFirst()) &&
        (Is<IFunction>(As<Cell>(cur_args->GetFirst())->GetFirst()) ||
         Is<Symbol>(As<Cell>(cur_args->GetFirst())->GetFirst()))) {
        if (Is<Symbol>(As<Cell>(cur_args->GetFirst())->GetFirst())) {
            auto search_res =
                scope->Get(As<Symbol>(As<Cell>(cur_args->GetFirst())->GetFirst())->GetName());

            if (search_res != nullptr) {
                auto func_compiled_args = CompileArgs(
                    As<Cell>(As<Cell>(cur_args->GetFirst())->GetSecond()),
                    As<Cell>(
                        As<LambdaFunction>(As<Cell>(search_res)->GetFirst())->GetRequiedArgs()),
                    scope);

                auto func_body = std::make_shared<Cell>();
                func_body->SetFirst(search_res);
                func_body->SetSecond(func_compiled_args);

                As<LambdaFunction>(As<Cell>(search_res)->GetFirst())->SetScope(scope);
                auto lambda_res =
                    As<LambdaFunction>(As<Cell>(search_res)->GetFirst())->Call(func_body, true);

                return lambda_res;
            }
        } else if (Is<IFunction>(As<Cell>(cur_args->GetFirst())->GetFirst())) {
            std::shared_ptr<Object> func_res;
            if (Is<IFunctionIs>(As<Cell>(cur_args->GetFirst())->GetFirst())) {
                auto is_instance =
                    As<IFunctionIs>(As<Cell>(cur_args->GetFirst())->GetFirst())
                        ->IsInstanceOf(As<Cell>(cur_args->GetFirst())->GetSecond(), scope);
                func_res = std::make_shared<Boolean>(is_instance);
            } else {
                func_res = As<IFunction>(As<Cell>(cur_args->GetFirst())->GetFirst())
                               ->CallInstance(cur_args->GetFirst(), scope);
            }

            cur_args->SetFirst(func_res);
            return CompileArgAsFunc(ptr, scope);
        }
    } else if (cur_args->GetFirst() == nullptr) {
        return nullptr;
    } else if (Is<Number>(cur_args->GetFirst()) || Is<Boolean>(cur_args->GetFirst())) {
        return cur_args->GetFirst();
    } else if (Is<Symbol>(cur_args->GetFirst())) {
        auto search_res = scope->Get(As<Symbol>(cur_args->GetFirst())->GetName());

        if (search_res != nullptr) {
            return search_res;
        } else {
            throw RuntimeError("Undefined variable");
        }
    }
}

inline void CheckArgsFunc(std::shared_ptr<Cell> ptr, std::shared_ptr<Scope> scope,
                          bool must_be_number = true, bool can_run_first_arg = true,
                          bool can_run_second_arg = true);

inline void RunFunctionsInArg(std::shared_ptr<Cell> ptr, std::shared_ptr<Cell> ptr_to_args,
                              std::shared_ptr<Scope> scope, bool must_be_number,
                              bool can_run_first_arg, bool can_run_second_arg) {
    if (Is<Symbol>(As<Cell>(ptr_to_args->GetFirst())->GetFirst())) {
        auto search_res =
            scope->Get(As<Symbol>(As<Cell>(ptr_to_args->GetFirst())->GetFirst())->GetName());
        if (search_res != nullptr) {
            auto func_compiled_args = CompileArgs(
                As<Cell>(As<Cell>(ptr_to_args->GetFirst())->GetSecond()),
                As<Cell>(As<LambdaFunction>(As<Cell>(search_res)->GetFirst())->GetRequiedArgs()),
                scope);

            auto func_body = std::make_shared<Cell>();
            func_body->SetFirst(search_res);
            func_body->SetSecond(func_compiled_args);

            As<LambdaFunction>(As<Cell>(search_res)->GetFirst())->SetScope(scope);
            auto lambda_res =
                As<LambdaFunction>(As<Cell>(search_res)->GetFirst())->Call(func_body, true);
            ptr_to_args->SetFirst(lambda_res);
        }
    } else if (Is<IFunction>(As<Cell>(ptr_to_args->GetFirst())->GetFirst())) {
        RUN_FUNC_AS_ARG(CheckArgsFunc, ptr_to_args, scope, must_be_number, can_run_first_arg,
                        can_run_second_arg)
    }
}

void CheckArgsFunc(std::shared_ptr<Cell> ptr, std::shared_ptr<Scope> scope, bool must_be_number,
                   bool can_run_first_arg, bool can_run_second_arg) {
    auto cur_args = As<Cell>(ptr->GetSecond());
    auto right_arg_ptr = As<Cell>(cur_args->GetSecond());
    std::shared_ptr<Object> search_res_first_args;
    std::shared_ptr<Object> search_res_second_args;

    if (!Is<IFunctionDefine>(ptr->GetFirst()) && cur_args != nullptr &&
        Is<Symbol>(cur_args->GetFirst())) {
        search_res_first_args = scope->Get(As<Symbol>(cur_args->GetFirst())->GetName());
    }

    if (right_arg_ptr != nullptr && Is<Symbol>(right_arg_ptr->GetFirst())) {
        search_res_second_args = scope->Get(As<Symbol>(right_arg_ptr->GetFirst())->GetName());
    }

    if (!Is<IFunctionDefine>(ptr->GetFirst()) && cur_args != nullptr &&
        Is<Symbol>(cur_args->GetFirst()) && search_res_first_args != nullptr) {
        auto search_res = search_res_first_args;

        cur_args->SetFirst(search_res);
    }

    // first arg as function
    if (can_run_first_arg && cur_args != nullptr && Is<Cell>(cur_args->GetFirst()) &&
        (Is<IFunction>(As<Cell>(cur_args->GetFirst())->GetFirst()) ||
         Is<Symbol>(As<Cell>(cur_args->GetFirst())->GetFirst()))) {
        RunFunctionsInArg(ptr, cur_args, scope, must_be_number, can_run_first_arg,
                          can_run_second_arg);
    }

    if (must_be_number &&
        ((!Is<Number>(cur_args->GetFirst()) ||
          (!Is<Cell>(cur_args->GetSecond()) && cur_args->GetSecond() != nullptr)))) {
        throw RuntimeError("args of Arithmetic Func must be a number");
    }

    if (right_arg_ptr != nullptr && Is<Symbol>(right_arg_ptr->GetFirst()) &&
        search_res_second_args != nullptr) {
        right_arg_ptr->SetFirst(search_res_second_args);
    }

    // second arg as function
    if (can_run_second_arg && right_arg_ptr != nullptr && Is<Cell>(right_arg_ptr->GetFirst()) &&
        (Is<IFunction>(As<Cell>(right_arg_ptr->GetFirst())->GetFirst()) ||
         Is<Symbol>(As<Cell>(right_arg_ptr->GetFirst())->GetFirst()))) {
        RunFunctionsInArg(ptr, right_arg_ptr, scope, must_be_number, can_run_first_arg,
                          can_run_second_arg);
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
    QuoteFunction(){};

    std::shared_ptr<Object> CallInstance(std::shared_ptr<Object> head, std::shared_ptr<Scope> scope)
        override {  // left - function; right - args
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

class IfFunction : public IFunction {
public:
    IfFunction(){};

    std::shared_ptr<Object> CallInstance(std::shared_ptr<Object> ptr,
                                         std::shared_ptr<Scope> scope) override {
        if (As<Cell>(ptr)->GetSecond() == nullptr) {
            throw SyntaxError("if function requires a condition");
        }

        int count_of_args = 1;
        std::shared_ptr<Object> temp = As<Cell>(ptr)->GetSecond();

        while (As<Cell>(temp)->GetSecond() != nullptr) {
            ++count_of_args;
            temp = As<Cell>(temp)->GetSecond();
        }

        if (count_of_args > 3 || count_of_args < 2) {
            throw SyntaxError("if function requires from 2 to 3 args");
        }

        auto cur = As<Cell>(ptr);
        bool condition_res = As<Boolean>(CompileArgAsFunc(cur, scope))->GetValue();

        if (condition_res) {  // true
            auto temp_cell = std::make_shared<Cell>();
            temp_cell->SetSecond(std::make_shared<Cell>());
            As<Cell>(temp_cell->GetSecond())
                ->SetFirst(CopyAst(As<Cell>(As<Cell>(cur->GetSecond())->GetSecond())->GetFirst()));
            return CompileArgAsFunc(temp_cell, scope);
        } else {
            if (count_of_args == 2) {  // empty false instructions
                return nullptr;
            }

            auto temp_cell = std::make_shared<Cell>();
            temp_cell->SetSecond(std::make_shared<Cell>());
            As<Cell>(temp_cell->GetSecond())
                ->SetFirst(
                    CopyAst(As<Cell>(As<Cell>(As<Cell>(cur->GetSecond())->GetSecond())->GetSecond())
                                ->GetFirst()));

            return CompileArgAsFunc(temp_cell, scope);
        }
    };
};