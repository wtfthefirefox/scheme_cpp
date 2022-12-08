#include <memory>

#include "object.h"

class ConsFunction : public IFunction {
public:
    ConsFunction(){};

    std::shared_ptr<Object> CallInstance(std::shared_ptr<Object> head,
                                         std::shared_ptr<Scope>) override;
};

// pair representation (car . cdr) car - first elem of pair; cdr - second elem of pair
class CarFunction : public IFunction {
public:
    CarFunction(){};

    std::shared_ptr<Object> CallInstance(std::shared_ptr<Object> head,
                                         std::shared_ptr<Scope>) override;
};

class CdrFunction : public IFunction {
public:
    CdrFunction(){};

    std::shared_ptr<Object> CallInstance(std::shared_ptr<Object> head,
                                         std::shared_ptr<Scope>) override;
};

class ListFuntion : public IFunction {
public:
    ListFuntion(){};

    std::shared_ptr<Object> CallInstance(std::shared_ptr<Object> head,
                                         std::shared_ptr<Scope>) override;
};

class ListRefFuntion : public IFunction {
public:
    ListRefFuntion(){};

    std::shared_ptr<Object> CallInstance(std::shared_ptr<Object> head,
                                         std::shared_ptr<Scope>) override;
};

class ListTailFunction : public IFunction {
public:
    ListTailFunction(){};

    std::shared_ptr<Object> CallInstance(std::shared_ptr<Object> head,
                                         std::shared_ptr<Scope>) override;
};