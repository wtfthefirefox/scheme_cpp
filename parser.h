#pragma once

#include <memory>

#include "object.h"
#include "eqaulity_functions.h"
#include "arithmetic_functions.h"
#include "is_functions.h"
#include "list_functions.h"
#include "tokenizer.h"

std::shared_ptr<Object> Read(Tokenizer* tokenizer, bool is_list = false);

std::shared_ptr<Object> ReadList(Tokenizer* tokenizer);