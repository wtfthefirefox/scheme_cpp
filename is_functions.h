#include <memory>

#include "object.h"

class IsNumberFunction : public IFunctionIs {
public:
    IsNumberFunction(){};
    bool IsInstanceOf(std::shared_ptr<Object> ptr, std::shared_ptr<Scope>) override;

    std::shared_ptr<Object> CallInstance(std::shared_ptr<Object>, std::shared_ptr<Scope>) override {
        return nullptr;
    };
};

class IsBooleanFunction : public IFunctionIs {
public:
    IsBooleanFunction(){};
    bool IsInstanceOf(std::shared_ptr<Object> ptr, std::shared_ptr<Scope>) override;

    std::shared_ptr<Object> CallInstance(std::shared_ptr<Object>, std::shared_ptr<Scope>) override {
        return nullptr;
    };
};

class NotFunction : public IFunctionIs {  // revert boolean otherwise false
public:
    NotFunction(){};
    bool IsInstanceOf(std::shared_ptr<Object> ptr, std::shared_ptr<Scope>) override;

    std::shared_ptr<Object> CallInstance(std::shared_ptr<Object>, std::shared_ptr<Scope>) override {
        return nullptr;
    };
};

class IsPairFunction : public IFunctionIs {  // revert boolean otherwise false
public:
    IsPairFunction(){};
    bool IsInstanceOf(std::shared_ptr<Object> ptr, std::shared_ptr<Scope>) override;

    std::shared_ptr<Object> CallInstance(std::shared_ptr<Object>, std::shared_ptr<Scope>) override {
        return nullptr;
    };
};

class IsNullFunction : public IFunctionIs {  // revert boolean otherwise false
public:
    IsNullFunction(){};
    bool IsInstanceOf(std::shared_ptr<Object> ptr, std::shared_ptr<Scope>) override;

    std::shared_ptr<Object> CallInstance(std::shared_ptr<Object>, std::shared_ptr<Scope>) override {
        return nullptr;
    };
};

class IsListFunction : public IFunctionIs {  // revert boolean otherwise false
public:
    IsListFunction(){};
    bool IsInstanceOf(std::shared_ptr<Object> ptr, std::shared_ptr<Scope>) override;

    std::shared_ptr<Object> CallInstance(std::shared_ptr<Object>, std::shared_ptr<Scope>) override {
        return nullptr;
    };
};

class IsSymbolFunction : public IFunctionIs {  // revert boolean otherwise false
public:
    IsSymbolFunction(){};
    bool IsInstanceOf(std::shared_ptr<Object> ptr, std::shared_ptr<Scope>) override;

    std::shared_ptr<Object> CallInstance(std::shared_ptr<Object>, std::shared_ptr<Scope>) override {
        return nullptr;
    };
};