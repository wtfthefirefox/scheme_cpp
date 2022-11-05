#pragma once

#include <memory>

#include "error.h"
#include "object.h"

class IFunctionEquality : public IFunction {  // factory
public:
    virtual std::shared_ptr<Object> CheckEquality(std::shared_ptr<Object> ptr1,
                                                  std::shared_ptr<Object> ptr2) = 0;
};

// return_first_false ? first false : first true
std::shared_ptr<Object> AsignValueInEqualityFunc(std::shared_ptr<Object> ptr,
                                                 std::shared_ptr<IFunctionEquality> func_obj,
                                                 bool must_be_number = true,
                                                 bool return_first_false = true);

class EqualFunc : public IFunctionEquality {
public:
    EqualFunc(){};

    std::shared_ptr<Object> CallInstance(std::shared_ptr<Object> ptr) override;

    std::shared_ptr<Object> CheckEquality(std::shared_ptr<Object> ptr1,
                                          std::shared_ptr<Object> ptr2) override;
};

class GreaterFunction : public IFunctionEquality {
public:
    GreaterFunction(){};

    std::shared_ptr<Object> CallInstance(std::shared_ptr<Object> ptr) override;

    std::shared_ptr<Object> CheckEquality(std::shared_ptr<Object> ptr1,
                                          std::shared_ptr<Object> ptr2) override;
};

class GreaterOrEqualFunction : public IFunctionEquality {
public:
    GreaterOrEqualFunction(){};

    std::shared_ptr<Object> CallInstance(std::shared_ptr<Object> ptr) override;

    std::shared_ptr<Object> CheckEquality(std::shared_ptr<Object> ptr1,
                                          std::shared_ptr<Object> ptr2) override;
};

class LowerFunction : public IFunctionEquality {
public:
    LowerFunction(){};

    std::shared_ptr<Object> CallInstance(std::shared_ptr<Object> ptr) override;

    std::shared_ptr<Object> CheckEquality(std::shared_ptr<Object> ptr1,
                                          std::shared_ptr<Object> ptr2) override;
};

class LowerOrEqualFunction : public IFunctionEquality {
public:
    LowerOrEqualFunction(){};

    std::shared_ptr<Object> CallInstance(std::shared_ptr<Object> ptr) override;

    std::shared_ptr<Object> CheckEquality(std::shared_ptr<Object> ptr1,
                                          std::shared_ptr<Object> ptr2) override;
};

class AndFunction : public IFunctionEquality {
public:
    AndFunction(){};

    std::shared_ptr<Object> CallInstance(std::shared_ptr<Object> ptr) override;

    std::shared_ptr<Object> CheckEquality(std::shared_ptr<Object> ptr1,
                                          std::shared_ptr<Object> ptr2) override;
};

class OrFunction : public IFunctionEquality {
public:
    OrFunction(){};

    std::shared_ptr<Object> CallInstance(std::shared_ptr<Object> ptr) override;

    std::shared_ptr<Object> CheckEquality(std::shared_ptr<Object> ptr1,
                                          std::shared_ptr<Object> ptr2) override;
};