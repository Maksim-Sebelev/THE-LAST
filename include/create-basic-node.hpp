#pragma once

#define SPECIALIZE_CREATE(NodeT, ...)                                                                                             \
template <>                                                                                                                       \
last::node::BasicNode last::node::create(NodeT node)                                                                              \
{                                                                                                                                 \
    return last::node::BasicNode::Actions<__VA_ARGS__>::create(std::move(node));                                                  \
}

#define CREATE_SAME(...)                                                                                                           \
SPECIALIZE_CREATE(Scope           , __VA_ARGS__)                                                                                   \
SPECIALIZE_CREATE(Print           , __VA_ARGS__)                                                                                   \
SPECIALIZE_CREATE(Scan            , __VA_ARGS__)                                                                                   \
SPECIALIZE_CREATE(UnaryOperator   , __VA_ARGS__)                                                                                   \
SPECIALIZE_CREATE(BinaryOperator  , __VA_ARGS__)                                                                                   \
SPECIALIZE_CREATE(If              , __VA_ARGS__)                                                                                   \
SPECIALIZE_CREATE(Else            , __VA_ARGS__)                                                                                   \
SPECIALIZE_CREATE(Condition       , __VA_ARGS__)                                                                                   \
SPECIALIZE_CREATE(While           , __VA_ARGS__)                                                                                   \
SPECIALIZE_CREATE(NumberLiteral   , __VA_ARGS__)                                                                                   \
SPECIALIZE_CREATE(StringLiteral   , __VA_ARGS__)                                                                                   \
SPECIALIZE_CREATE(Variable        , __VA_ARGS__)
