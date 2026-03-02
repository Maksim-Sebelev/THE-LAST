#pragma once

#warning "Stupid read realisation. Use newer versions."


#if not defined(THELAST_READ_AST_NO_INCLUDES)
#include <ostream>
#include <fstream>
#include <filesystem>
#include <utility>
#include <stdexcept>
#include <memory>
#include <string>
#include <sstream>
#include <stack>
#include <vector>
#endif /* not defined(THELAST_READ_AST_NO_INCLUDES) */

namespace last::node
{

template <typename unusedT = void>
BasicNode read(std::ifstream& ifs);

void skip_whitespace(std::ifstream& ifs)
{
    while (ifs.peek() == ' ' || ifs.peek() == '\n' || ifs.peek() == '\t' || ifs.peek() == '\r')
        ifs.get();
}

bool check_open_bracket(std::ifstream& ifs)
{
    skip_whitespace(ifs);
    auto&& potential_bracket = char{};
    ifs >> potential_bracket;
    return (potential_bracket == '{');
}

bool check_close_bracket(std::ifstream& ifs)
{
    skip_whitespace(ifs);
    auto&& potential_bracket = char{};
    ifs >> potential_bracket;
    return (potential_bracket == '}');
}

void check_expected_token(std::ifstream& ifs, const std::string& expected)
{
    skip_whitespace(ifs);
    auto&& token = std::string{};
    ifs >> token;
    if (token != expected)
        throw std::runtime_error("expected '" + expected + "' but got '" + token + "'");
}

std::string read_quoted_string(std::ifstream& ifs)
{
    skip_whitespace(ifs);
    auto&& quote = char{};
    ifs.get(quote);
    if (quote != '"')
        throw std::runtime_error("expected '\"' at start of string");
    
    std::string result;
    char ch;
    while (ifs.get(ch) && ch != '"')
        result += ch;
    
    return result;
}

template <typename unusedT = void>
BasicNode read_scope(std::ifstream& ifs)
{
    if (not check_open_bracket(ifs))
        throw std::runtime_error("expect '{' after 'Scope'");

    auto&& node = Scope{};

    try {
        while (true)
            node.push_back(read(ifs));
    } catch(...) {
    }

    return last::node::template create(std::move(node));
}

template <typename unusedT = void>
BasicNode read_print(std::ifstream& ifs)
{
    if (not check_open_bracket(ifs))
        throw std::runtime_error("expect '{' after 'Print'");

    auto&& node = Print{};

    try {
        while (true)
            node.push_back(read(ifs));
    } catch(...) {
    }

    
    return last::node::template create(std::move(node));
}

template <typename unusedT = void>
BasicNode read_scan(std::ifstream& ifs)
{
    if (not check_open_bracket(ifs))
        throw std::runtime_error("expect '{' after 'Scan'");

    if (not check_close_bracket(ifs))
        throw std::runtime_error("expect '}' after 'Scan'");
    
    auto&& node = Scan{};
    return last::node::template create(std::move(node));
}

template <typename unusedT = void>
BasicNode read_number_literal(std::ifstream& ifs)
{
    if (not check_open_bracket(ifs))
        throw std::runtime_error("expect '{' after 'NumberLiteral'");

    check_expected_token(ifs, "value");

    if (not check_open_bracket(ifs))
        throw std::runtime_error("expect '{' after 'value'");

    auto&& value = int{};
    ifs >> value;

    if (not check_close_bracket(ifs))
        throw std::runtime_error("expect '}' after value");

    if (not check_close_bracket(ifs))
        throw std::runtime_error("expect '}' after NumberLiteral");

    auto&& node = NumberLiteral{value};
    return last::node::template create(std::move(node));
}

template <typename unusedT = void>
BasicNode read_string_literal(std::ifstream& ifs)
{
    if (not check_open_bracket(ifs))
        throw std::runtime_error("expect '{' after 'StringLiteral'");

    check_expected_token(ifs, "value");

    if (not check_open_bracket(ifs))
        throw std::runtime_error("expect '{' after 'value'");

    auto&& value = read_quoted_string(ifs);

    if (not check_close_bracket(ifs))
        throw std::runtime_error("expect '}' after value");

    if (not check_close_bracket(ifs))
        throw std::runtime_error("expect '}' after StringLiteral");

    auto&& node = StringLiteral{std::move(value)};
    return last::node::template create(std::move(node));
}

template <typename unusedT = void>
BasicNode read_variable(std::ifstream& ifs)
{
    if (not check_open_bracket(ifs))
        throw std::runtime_error("expect '{' after 'Variable'");

    check_expected_token(ifs, "name");

    if (not check_open_bracket(ifs))
        throw std::runtime_error("expect '{' after 'name'");

    auto&& name = read_quoted_string(ifs);

    if (not check_close_bracket(ifs))
        throw std::runtime_error("expect '}' after name");

    if (not check_close_bracket(ifs))
        throw std::runtime_error("expect '}' after Variable");

    auto&& node = Variable{std::move(name)};
    return last::node::template create(std::move(node));
}

template <typename unusedT = void>
BasicNode read_unary_operator(std::ifstream& ifs)
{
    if (not check_open_bracket(ifs))
        throw std::runtime_error("expect '{' after 'UnaryOperator'");

    check_expected_token(ifs, "type");

    if (not check_open_bracket(ifs))
        throw std::runtime_error("expect '{' after 'type'");

    auto&& op_type = std::string{};
    ifs >> op_type;

    if (not check_close_bracket(ifs))
        throw std::runtime_error("expect '}' after type");

    check_expected_token(ifs, "operand");

    if (not check_open_bracket(ifs))
        throw std::runtime_error("expect '{' after 'operand'");

    auto&& operand = read(ifs);

    if (not check_close_bracket(ifs))
        throw std::runtime_error("expect '}' after operand");

    if (not check_close_bracket(ifs))
        throw std::runtime_error("expect '}' after UnaryOperator");

    auto&& type = UnaryOperator::UnaryOperatorT{};

    if (op_type == "+") type = UnaryOperator::PLUS;
    else if (op_type == "-") type = UnaryOperator::MINUS;
    else if (op_type == "!") type = UnaryOperator::NOT;
    else throw std::runtime_error("unknown unary operator type: " + op_type);

    auto&& node = UnaryOperator{type, std::move(operand)};
    return last::node::template create(std::move(node));
}

template <typename unusedT = void>
BasicNode read_binary_operator(std::ifstream& ifs)
{
    if (not check_open_bracket(ifs))
        throw std::runtime_error("expect '{' after 'BinaryOperator'");

    check_expected_token(ifs, "type");

    if (not check_open_bracket(ifs))
        throw std::runtime_error("expect '{' after 'type'");

    auto&& op_type = std::string{};
    ifs >> op_type;

    if (not check_close_bracket(ifs))
        throw std::runtime_error("expect '}' after type");

    check_expected_token(ifs, "left");

    if (not check_open_bracket(ifs))
        throw std::runtime_error("expect '{' after 'left'");

    auto&& left = read(ifs);

    if (not check_close_bracket(ifs))
        throw std::runtime_error("expect '}' after left");

    check_expected_token(ifs, "right");

    if (not check_open_bracket(ifs))
        throw std::runtime_error("expect '{' after 'right'");

    auto&& right = read(ifs);

    if (not check_close_bracket(ifs))
        throw std::runtime_error("expect '}' after right");

    if (not check_close_bracket(ifs))
        throw std::runtime_error("expect '}' after BinaryOperator");

    auto&& type = BinaryOperator::BinaryOperatorT{};

    if (op_type == "&&") type = BinaryOperator::AND;
    else if (op_type == "||") type = BinaryOperator::OR;
    else if (op_type == "!") type = BinaryOperator::NOT;
    else if (op_type == "+") type = BinaryOperator::ADD;
    else if (op_type == "-") type = BinaryOperator::SUB;
    else if (op_type == "*") type = BinaryOperator::MUL;
    else if (op_type == "/") type = BinaryOperator::DIV;
    else if (op_type == "%") type = BinaryOperator::REM;
    else if (op_type == ">") type = BinaryOperator::ISAB;
    else if (op_type == ">=") type = BinaryOperator::ISABE;
    else if (op_type == "<") type = BinaryOperator::ISLS;
    else if (op_type == "<=") type = BinaryOperator::ISLSE;
    else if (op_type == "==") type = BinaryOperator::ISEQ;
    else if (op_type == "!=") type = BinaryOperator::ISNE;
    else if (op_type == "=") type = BinaryOperator::ASGN;
    else if (op_type == "+=") type = BinaryOperator::ADDASGN;
    else if (op_type == "-=") type = BinaryOperator::SUBASGN;
    else if (op_type == "*=") type = BinaryOperator::MULASGN;
    else if (op_type == "/=") type = BinaryOperator::DIVASGN;
    else if (op_type == "%=") type = BinaryOperator::REMASGN;
    else throw std::runtime_error("unknown binary operator type: " + op_type);

    auto&& node = BinaryOperator{type, std::move(left), std::move(right)};
    return last::node::template create(std::move(node));
}

template <typename unusedT = void>
BasicNode read_if(std::ifstream& ifs)
{
    if (not check_open_bracket(ifs))
        throw std::runtime_error("expect '{' after 'If'");

    check_expected_token(ifs, "condition");

    if (not check_open_bracket(ifs))
        throw std::runtime_error("expect '{' after 'condition'");

    auto&& condition = read(ifs);

    if (not check_close_bracket(ifs))
        throw std::runtime_error("expect '}' after condition");

    check_expected_token(ifs, "body");

    if (not check_open_bracket(ifs))
        throw std::runtime_error("expect '{' after 'body'");

    auto&& body = read(ifs);

    if (not check_close_bracket(ifs))
        throw std::runtime_error("expect '}' after body");

    if (not check_close_bracket(ifs))
        throw std::runtime_error("expect '}' after If");

    auto&& node = If{std::move(condition), std::move(body)};
    return last::node::template create(std::move(node));
}

template <typename unusedT = void>
BasicNode read_else(std::ifstream& ifs)
{
    if (not check_open_bracket(ifs))
        throw std::runtime_error("expect '{' after 'Else'");

    check_expected_token(ifs, "body");

    if (not check_open_bracket(ifs))
        throw std::runtime_error("expect '{' after 'body'");

    auto&& body = read(ifs);

    if (not check_close_bracket(ifs))
        throw std::runtime_error("expect '}' after body");

    if (not check_close_bracket(ifs))
        throw std::runtime_error("expect '}' after Else");

    auto&& node = Else{std::move(body)};
    return last::node::template create(std::move(node));
}

template <typename unusedT = void>
BasicNode read_condition(std::ifstream& ifs)
{
    if (not check_open_bracket(ifs))
        throw std::runtime_error("expect '{' after 'Condition'");

    std::vector<BasicNode> ifs_nodes;

    check_expected_token(ifs, "if");

    if (not check_open_bracket(ifs))
        throw std::runtime_error("expect '{' after 'if'");

    auto&& if_node = read(ifs);

    if (not check_close_bracket(ifs))
        throw std::runtime_error("expect '}' after if");

    ifs_nodes.push_back(std::move(if_node));

    while (true)
    {
        skip_whitespace(ifs);
        if (ifs.peek() == 'i')
        {
            auto&& token = std::string{};
            ifs >> token;
            if (token == "if")
            {
                if (not check_open_bracket(ifs))
                    throw std::runtime_error("expect '{' after 'if'");

                auto&& next_if = read(ifs);

                if (not check_close_bracket(ifs))
                    throw std::runtime_error("expect '}' after if");

                ifs_nodes.push_back(std::move(next_if));
                continue;
            }
            else
            {
                ifs.seekg(-static_cast<int>(token.length()), std::ios::cur);
                break;
            }
        }
        else break;
    }

    check_expected_token(ifs, "else");

    if (not check_open_bracket(ifs))
        throw std::runtime_error("expect '{' after 'else'");

    auto&& else_node = read(ifs);

    if (not check_close_bracket(ifs))
        throw std::runtime_error("expect '}' after else");

    if (not check_close_bracket(ifs))
        throw std::runtime_error("expect '}' after Condition");

    auto&& node = Condition{std::move(ifs_nodes), std::move(else_node)};
    return last::node::template create(std::move(node));
}

template <typename unusedT = void>
BasicNode read_while(std::ifstream& ifs)
{
    if (not check_open_bracket(ifs))
        throw std::runtime_error("expect '{' after 'While'");

    check_expected_token(ifs, "condition");

    if (not check_open_bracket(ifs))
        throw std::runtime_error("expect '{' after 'condition'");

    auto&& condition = read(ifs);

    if (not check_close_bracket(ifs))
        throw std::runtime_error("expect '}' after condition");

    check_expected_token(ifs, "body");

    if (not check_open_bracket(ifs))
        throw std::runtime_error("expect '{' after 'body'");

    auto&& body = read(ifs);

    if (not check_close_bracket(ifs))
        throw std::runtime_error("expect '}' after body");

    if (not check_close_bracket(ifs))
        throw std::runtime_error("expect '}' after While");

    auto&& node = While{std::move(condition), std::move(body)};
    return last::node::template create(std::move(node));
}

template <typename unusedT>
BasicNode read(std::ifstream& ifs)
{
    skip_whitespace(ifs);
    auto&& node_type = std::string{};
    ifs >> node_type;
    if (node_type == traits::get_node_info<Scope, traits::NAME>())
        return read_scope(ifs);
    if (node_type == traits::get_node_info<Print, traits::NAME>())
        return read_print(ifs);
    if (node_type == traits::get_node_info<Scan, traits::NAME>())
        return read_scan(ifs);
    if (node_type == traits::get_node_info<UnaryOperator, traits::NAME>())
        return read_unary_operator(ifs);
    if (node_type == traits::get_node_info<BinaryOperator, traits::NAME>())
        return read_binary_operator(ifs);
    if (node_type == traits::get_node_info<Variable, traits::NAME>())
        return read_variable(ifs);
    if (node_type == traits::get_node_info<NumberLiteral, traits::NAME>())
        return read_number_literal(ifs);
    if (node_type == traits::get_node_info<StringLiteral, traits::NAME>())
        return read_string_literal(ifs);
    if (node_type == traits::get_node_info<If, traits::NAME>())
        return read_if(ifs);
    if (node_type == traits::get_node_info<Else, traits::NAME>())
        return read_else(ifs);
    if (node_type == traits::get_node_info<Condition, traits::NAME>())
        return read_condition(ifs);
    if (node_type == traits::get_node_info<While, traits::NAME>())
        return read_while(ifs);

    throw std::runtime_error("Failed read ast: undefined node type " + node_type);
}

} /* namespace last::node */

namespace last
{

template <typename unusedT = void>
AST read(std::filesystem::path const & ast_txt)
{
    auto&& ifs = std::ifstream{ast_txt};

    if (ifs.fail())
        throw std::runtime_error("Failed read ast. No such file: " + ast_txt.string());

    auto&& signature = std::string{};
    std::getline(ifs, signature);

    if (signature != Info::get().ast_text_representation_signature())
        throw std::runtime_error("Bad ast format signature. Unsupported file format: '" + ast_txt.string() + "'");

    return AST{node::read(ifs)};
}

} /* namespace last */