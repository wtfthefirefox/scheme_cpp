#include "eqaulity_functions.h"

std::shared_ptr<Object> AsignValueInEqualityFunc(std::shared_ptr<Object> ptr,
                                                 std::shared_ptr<IFunctionEquality> func_obj,
                                                 bool must_be_number, bool return_first_false) {
    auto cur = As<Cell>(ptr);

    if (cur->GetSecond() == nullptr) {
        if (return_first_false) {
            return std::make_shared<Boolean>(true);
        } else {
            return std::make_shared<Boolean>(false);
        }
    } else {
        auto cur_args = As<Cell>(cur->GetSecond());
        auto right_arg_ptr = As<Cell>(cur_args->GetSecond());
        CheckArgsFunc(cur, must_be_number);

        if (cur_args->GetSecond() == nullptr) {
            throw RuntimeError("Not enough args for Arithmetic Func");
        }

        std::shared_ptr<Object> comp_res;  // result of equality
        if (must_be_number) {
            comp_res = func_obj->CheckEquality(As<Number>(cur_args->GetFirst()),
                                               As<Number>(right_arg_ptr->GetFirst()));
        } else {
            comp_res = func_obj->CheckEquality(cur_args->GetFirst(), right_arg_ptr->GetFirst());
        }

        if (must_be_number) {                             // between numbers
            if (right_arg_ptr->GetSecond() == nullptr) {  // only two args
                return comp_res;
            } else {
                if (As<Boolean>(comp_res)->GetValue()) {  // <=> two numbers
                    cur->SetSecond(right_arg_ptr);
                    return func_obj->CallInstance(cur);
                } else {
                    return comp_res;
                }
            }
        } else {
            if (right_arg_ptr->GetSecond() == nullptr) {                  // only two args
                if (Is<Boolean>(cur_args->GetFirst())) {                  // check first arg
                    if (As<Boolean>(cur_args->GetFirst())->GetValue()) {  // first arg true
                        if (return_first_false) {
                            if (comp_res == cur_args->GetFirst()) {
                                return right_arg_ptr->GetFirst();
                            } else {
                                return comp_res;
                            }
                        } else {
                            return cur_args->GetFirst();
                        }
                    } else {
                        if (return_first_false) {
                            return cur_args->GetFirst();
                        } else {
                            if (comp_res == cur_args->GetFirst()) {  // second arg is true
                                return right_arg_ptr->GetFirst();
                            } else {
                                return comp_res;
                            }
                        }
                    }
                } else {  // first arg is true
                    if (comp_res == cur_args->GetFirst()) {
                        return right_arg_ptr->GetFirst();
                    } else {
                        return comp_res;
                    }
                }
            } else {  // two or more
                // check if arg return prev value ? return bad value : return new value
                if (comp_res == cur_args->GetFirst()) {
                    return right_arg_ptr->GetFirst();
                } else {  // need to continue
                    cur->SetSecond(right_arg_ptr);
                    return func_obj->CallInstance(cur);
                }
            }
        }
    }
}

std::shared_ptr<Object> EqualFunc::CallInstance(std::shared_ptr<Object> ptr) {

    return AsignValueInEqualityFunc(ptr, std::make_shared<EqualFunc>());
}

std::shared_ptr<Object> EqualFunc::CheckEquality(std::shared_ptr<Object> ptr1,
                                                 std::shared_ptr<Object> ptr2) {
    return std::make_shared<Boolean>(*As<Number>(ptr1) == *As<Number>(ptr2));
}

std::shared_ptr<Object> GreaterFunction::CallInstance(std::shared_ptr<Object> ptr) {
    return AsignValueInEqualityFunc(ptr, std::make_shared<GreaterFunction>());
}

std::shared_ptr<Object> GreaterFunction::CheckEquality(std::shared_ptr<Object> ptr1,
                                                       std::shared_ptr<Object> ptr2) {
    return std::make_shared<Boolean>(*As<Number>(ptr1) > *As<Number>(ptr2));
}

std::shared_ptr<Object> GreaterOrEqualFunction::CallInstance(std::shared_ptr<Object> ptr) {
    return AsignValueInEqualityFunc(ptr, std::make_shared<GreaterOrEqualFunction>());
}

std::shared_ptr<Object> GreaterOrEqualFunction::CheckEquality(std::shared_ptr<Object> ptr1,
                                                              std::shared_ptr<Object> ptr2) {
    return std::make_shared<Boolean>(*As<Number>(ptr1) >= *As<Number>(ptr2));
}

std::shared_ptr<Object> LowerFunction::CallInstance(std::shared_ptr<Object> ptr) {
    return AsignValueInEqualityFunc(ptr, std::make_shared<LowerFunction>());
}

std::shared_ptr<Object> LowerFunction::CheckEquality(std::shared_ptr<Object> ptr1,
                                                     std::shared_ptr<Object> ptr2) {
    return std::make_shared<Boolean>(*As<Number>(ptr1) < *As<Number>(ptr2));
}

std::shared_ptr<Object> LowerOrEqualFunction::CallInstance(std::shared_ptr<Object> ptr) {
    return AsignValueInEqualityFunc(ptr, std::make_shared<LowerOrEqualFunction>());
}

std::shared_ptr<Object> LowerOrEqualFunction::CheckEquality(std::shared_ptr<Object> ptr1,
                                                            std::shared_ptr<Object> ptr2) {
    return std::make_shared<Boolean>(*As<Number>(ptr1) <= *As<Number>(ptr2));
}

std::shared_ptr<Object> AndFunction::CallInstance(std::shared_ptr<Object> ptr) {
    return AsignValueInEqualityFunc(ptr, std::make_shared<AndFunction>(), false);
}

std::shared_ptr<Object> AndFunction::CheckEquality(std::shared_ptr<Object> ptr1,
                                                   std::shared_ptr<Object> ptr2) {
    // return last true
    if (Is<Boolean>(ptr2)) {                  // if bool
        if (As<Boolean>(ptr2)->GetValue()) {  // if true - return ptr2
            return ptr2;
        } else {  // return ptr2
            return ptr1;
        }
    }

    return ptr2;
}

std::shared_ptr<Object> OrFunction::CallInstance(std::shared_ptr<Object> ptr) {
    return AsignValueInEqualityFunc(ptr, std::make_shared<OrFunction>(), false, false);
}

std::shared_ptr<Object> OrFunction::CheckEquality(std::shared_ptr<Object> ptr1,
                                                  std::shared_ptr<Object> ptr2) {
    // return first true
    if (Is<Boolean>(ptr2)) {                  // if bool
        if (As<Boolean>(ptr2)->GetValue()) {  // if true - return ptr1
            return ptr1;
        } else {  // return ptr2
            return ptr2;
        }
    }

    return ptr1;
}