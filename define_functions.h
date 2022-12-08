#include "object.h"

class DefineFunction : public IFunctionDefine {
public:
    DefineFunction(){};

    std::shared_ptr<Object> CallInstance(std::shared_ptr<Object>, std::shared_ptr<Scope>) override;
};

class SetFunction : public IFunctionDefine {
public:
    SetFunction(){};

    std::shared_ptr<Object> CallInstance(std::shared_ptr<Object>, std::shared_ptr<Scope>) override;
};

// pair representation (car . cdr) car - first elem of pair; cdr - second elem of pair
class SetCarFunction : public IFunctionDefine {
public:
    SetCarFunction(){};

    std::shared_ptr<Object> CallInstance(std::shared_ptr<Object>, std::shared_ptr<Scope>) override;
};

class SetCdrFunction : public IFunctionDefine {
public:
    SetCdrFunction(){};

    std::shared_ptr<Object> CallInstance(std::shared_ptr<Object>, std::shared_ptr<Scope>) override;
};

// lambda (args) body
// (lambda (args) body) run_args