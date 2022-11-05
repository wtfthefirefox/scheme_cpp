#include <memory>

#include "object.h"

class ConsFunction : public IFunction {
public:
    ConsFunction(){};

    std::shared_ptr<Object> CallInstance(std::shared_ptr<Object> head) override;
};

class CarFunction : public IFunction {
public:
    CarFunction(){};

    std::shared_ptr<Object> CallInstance(std::shared_ptr<Object> head) override;
};

class CdrFunction : public IFunction {
public:
    CdrFunction(){};

    std::shared_ptr<Object> CallInstance(std::shared_ptr<Object> head) override;
};

class ListFuntion : public IFunction {
public:
    ListFuntion(){};

    std::shared_ptr<Object> CallInstance(std::shared_ptr<Object> head) override;
};

class ListRefFuntion : public IFunction {
public:
    ListRefFuntion(){};

    std::shared_ptr<Object> CallInstance(std::shared_ptr<Object> head) override;
};

class ListTailFunction : public IFunction {
public:
    ListTailFunction(){};

    std::shared_ptr<Object> CallInstance(std::shared_ptr<Object> head) override;
};