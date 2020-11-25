// Fizzy: A fast WebAssembly interpreter
// Copyright 2020 The Fizzy Authors.
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "execute.hpp"
#include <test/utils/instantiate_helpers.hpp>
#include <test/utils/typed_value.hpp>
#include <algorithm>
#include <initializer_list>

namespace fizzy::test
{
inline ExecutionResult execute(
    Instance& instance, FuncIdx func_idx, std::initializer_list<TypedValue> typed_args)
{
    const auto& func_type = instance.module->get_function_type(func_idx);
    const auto [typed_arg_it, type_it] = std::mismatch(std::cbegin(typed_args),
        std::cend(typed_args), std::cbegin(func_type.inputs), std::cend(func_type.inputs),
        [](const auto& typed_arg, auto type) { return typed_arg.type == type; });
    if (typed_arg_it != std::cend(typed_args) && type_it != std::cend(func_type.inputs))
        throw std::invalid_argument{"invalid type of the argument " +
                                    std::to_string(typed_arg_it - std::cbegin(typed_args))};
    else if (typed_arg_it != std::cend(typed_args))
        throw std::invalid_argument{"too many arguments"};
    else if (type_it != std::cend(func_type.inputs))
        throw std::invalid_argument{"too few arguments"};

    std::vector<fizzy::Value> args(typed_args.size());
    std::transform(std::cbegin(typed_args), std::cend(typed_args), std::begin(args),
        [](const auto& typed_arg) { return typed_arg.value; });

    return fizzy::execute(instance, func_idx, args.data());
}

inline ExecutionResult execute(const std::unique_ptr<const Module>& module, FuncIdx func_idx,
    std::initializer_list<TypedValue> typed_args)
{
    auto instance = instantiate(*module);
    return test::execute(*instance, func_idx, typed_args);
}
}  // namespace fizzy::test
