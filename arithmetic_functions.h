#pragma once

#include <memory>
#include <cmath>
#include <algorithm>
#include <cstdint>

#include "error.h"
#include "object.h"

class IFunctionArithmetic : public IFunction {  // factory
public:
    virtual int64_t EvalArgs(std::shared_ptr<Number> ptr1, std::shared_ptr<Number> ptr2) = 0;
};

std::shared_ptr<Object> AsignValueToArithmeticFunc(std::shared_ptr<Object> ptr,
                                                   std::shared_ptr<IFunctionArithmetic> func_obj);

class SumFunction : public IFunctionArithmetic {
public:
    SumFunction(){};

    std::shared_ptr<Object> CallInstance(std::shared_ptr<Object> ptr) override;

    int64_t EvalArgs(std::shared_ptr<Number> ptr1, std::shared_ptr<Number> ptr2) override;
};

class MinusFunction : public IFunctionArithmetic {
public:
    MinusFunction(){};

    std::shared_ptr<Object> CallInstance(std::shared_ptr<Object> ptr) override;

    int64_t EvalArgs(std::shared_ptr<Number> ptr1, std::shared_ptr<Number> ptr2) override;
};

class MultiplyFunction : public IFunctionArithmetic {
public:
    MultiplyFunction(){};

    std::shared_ptr<Object> CallInstance(std::shared_ptr<Object> ptr) override;

    int64_t EvalArgs(std::shared_ptr<Number> ptr1, std::shared_ptr<Number> ptr2) override;
};

class DevideFunction : public IFunctionArithmetic {
public:
    DevideFunction(){};

    std::shared_ptr<Object> CallInstance(std::shared_ptr<Object> ptr) override;

    int64_t EvalArgs(std::shared_ptr<Number> ptr1, std::shared_ptr<Number> ptr2) override;
};

class MaxFunction : public IFunctionArithmetic {
public:
    MaxFunction(){};

    std::shared_ptr<Object> CallInstance(std::shared_ptr<Object> ptr) override;

    int64_t EvalArgs(std::shared_ptr<Number> ptr1, std::shared_ptr<Number> ptr2) override;
};

class MinFunction : public IFunctionArithmetic {
public:
    MinFunction(){};

    std::shared_ptr<Object> CallInstance(std::shared_ptr<Object> ptr) override;

    int64_t EvalArgs(std::shared_ptr<Number> ptr1, std::shared_ptr<Number> ptr2) override;
};

class AbsFunction : public IFunctionArithmetic {
public:
    AbsFunction(){};

    std::shared_ptr<Object> CallInstance(std::shared_ptr<Object> ptr) override;

    int64_t EvalArgs(std::shared_ptr<Number> ptr1, std::shared_ptr<Number> ptr2) override;
};