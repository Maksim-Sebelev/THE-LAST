module;

#include <cstdint>
#include <cstddef>
#include <iostream>
#include <fstream>
#include <ostream>
#include <string>
#include <filesystem>

#define FIELD_BEGIN(NodeT, os, node_tabs, n) write_in_file::field_begin(os, node_tabs, traits::get_node_info<NodeT, traits::FIELD, n>(), n, false)
#define FIELD_BEGIN_SELF_ALIGNMENT(NodeT, os, node_tabs, n) write_in_file::field_begin(os, node_tabs, traits::get_node_info<NodeT, traits::FIELD, n>(), n, true)

#define FIELD(NodeT, os, node_tabs, n, value) do { \
    FIELD_BEGIN(NodeT, os, node_tabs, n);          \
    os << value;                                   \
    write_in_file::field_end(os, node_tabs);       \
} while(false)

#define FIELD_SELF_ALIGNMENT(NodeT, os, node_tabs, n, value) do { \
    FIELD_BEGIN_SELF_ALIGNMENT(NodeT, os, node_tabs, n);          \
    write(value, os, node_tabs + field_enclosure_alignment);      \
    write_in_file::field_end(os, node_tabs, true);                \
} while(false)


export module ast_write;

export import ast;
import last_info;
import node_traits;

namespace last::node
{

export
using writable = void(std::ofstream&, size_t);


constexpr const size_t arg_enclosure_aligment = 1LU;
constexpr const size_t field_enclosure_alignment = 2LU;

void write(BasicNode const & node, std::ofstream& os, size_t enclosure)
{ return visit<void, std::ofstream&, size_t>(node, os, enclosure); }

namespace write_in_file
{

void n_tab(std::ofstream& os, size_t tabs)
{ if (tabs == 0LU) return; os << std::string(tabs, '\t'); }

void open_bracket(std::ofstream& os, size_t tabs = 0LU)
{ n_tab(os, tabs); os << ((tabs == 0LU) ? " {\n" : "{\n"); }

void close_bracket(std::ofstream& os, size_t tabs = 0LU)
{ n_tab(os, tabs); os << "}\n"; }

template <typename NodeT>
void write_begin(std::ofstream& os, size_t tabs = 0LU)
{
    n_tab(os, tabs);
    os << traits::get_node_info<NodeT, traits::NAME>();
    open_bracket(os);
}

void write_end(std::ofstream& os, size_t tabs = 0LU)
{ close_bracket(os, tabs); }

void field_begin(std::ofstream& os, size_t node_tabs, std::string_view field_name, size_t field_num, bool self_alignment)
{ n_tab(os, node_tabs + 1LU); os << "field[" << field_num << "]:" << field_name; open_bracket(os); if (self_alignment) return; n_tab(os, node_tabs + field_enclosure_alignment); }

void field_end(std::ofstream& os, size_t node_tabs, bool self_alignment = false)
{ if (not self_alignment) os << "\n"; close_bracket(os, node_tabs + arg_enclosure_aligment); }

} /* namespace write_in_file */
} /* namespace last::node */

namespace last
{

export
void write(AST const & ast, std::filesystem::path const &file)
{
    auto&& ofs = std::ofstream{file};
    if (ofs.fail()) throw std::runtime_error("failed open '" + file.string() + "' for write ast.");

    ofs << Info::instance().ast_text_representation_signature();

    node::write(ast.root(), ofs, 0LU);
}
} /* namespace last */

namespace last::node::visit_specializations
{

template <>
void visit(Print const& node, std::ofstream& os, size_t enclosure)
{
    write_in_file::write_begin<Print>(os, enclosure);

    for (auto&& arg: node)
        write(arg, os, enclosure + arg_enclosure_aligment);

    write_in_file::write_end(os, enclosure);
}

template <>
void visit([[maybe_unused]] Scan const& node, std::ofstream& os, size_t enclosure)
{
    write_in_file::write_begin<Scan>(os, enclosure);
    write_in_file::write_end(os, enclosure);
}

template <>
void visit(Scope const& node, std::ofstream& os, size_t enclosure)
{
    write_in_file::write_begin<Scope>(os, enclosure);

    for (auto&& arg: node)
        write(arg, os, enclosure + arg_enclosure_aligment);

    write_in_file::write_end(os, enclosure);
}

template <>
void visit(Variable const& node, std::ofstream& os, size_t enclosure)
{
    write_in_file::write_begin<Variable>(os, enclosure);
    FIELD(Variable, os, enclosure, 0LU, "\"" << node.name() << "\"");
    write_in_file::write_end(os, enclosure);
}

template <>
void visit(NumberLiteral const& node, std::ofstream& os, size_t enclosure)
{
    write_in_file::write_begin<NumberLiteral>(os, enclosure);
    FIELD(NumberLiteral, os, enclosure, 0LU, node.value());
    write_in_file::write_end(os, enclosure);
}

template <>
void visit(StringLiteral const& node, std::ofstream& os, size_t enclosure)
{
    write_in_file::write_begin<StringLiteral>(os, enclosure);
    FIELD(StringLiteral, os, enclosure, 0LU, "\"" << node.value() << "\"");
    write_in_file::write_end(os, enclosure);
}

template <>
void visit(UnaryOperator const& node, std::ofstream& os, size_t enclosure)
{
    write_in_file::write_begin<UnaryOperator>(os, enclosure);

    FIELD_BEGIN(UnaryOperator, os, enclosure, 0LU);
    switch (node.type())
    {
        case UnaryOperator::MINUS: os << traits::get_node_info<UnaryOperator, traits::OPERATOR_NAME, UnaryOperator::MINUS>(); break;
        case UnaryOperator::PLUS:  os << traits::get_node_info<UnaryOperator, traits::OPERATOR_NAME, UnaryOperator::PLUS>();  break;
        case UnaryOperator::NOT:   os << traits::get_node_info<UnaryOperator, traits::OPERATOR_NAME, UnaryOperator::NOT>();   break;
        default: __builtin_unreachable();
    }
    write_in_file::field_end(os, enclosure);

    FIELD_SELF_ALIGNMENT(UnaryOperator, os, enclosure, 1LU, node.arg());

    write_in_file::write_end(os, enclosure);
}

template <>
void visit(BinaryOperator const& node, std::ofstream& os, size_t enclosure)
{
    write_in_file::write_begin<BinaryOperator>(os, enclosure);

    FIELD_BEGIN(BinaryOperator, os, enclosure, 0LU);
    switch (node.type())
    {
        case BinaryOperator::AND:     os << traits::get_node_info<BinaryOperator, traits::OPERATOR_NAME, BinaryOperator::AND    >(); break;
        case BinaryOperator::OR:      os << traits::get_node_info<BinaryOperator, traits::OPERATOR_NAME, BinaryOperator::OR     >(); break;
        case BinaryOperator::ADD:     os << traits::get_node_info<BinaryOperator, traits::OPERATOR_NAME, BinaryOperator::ADD    >(); break;
        case BinaryOperator::SUB:     os << traits::get_node_info<BinaryOperator, traits::OPERATOR_NAME, BinaryOperator::SUB    >(); break;
        case BinaryOperator::MUL:     os << traits::get_node_info<BinaryOperator, traits::OPERATOR_NAME, BinaryOperator::MUL    >(); break;
        case BinaryOperator::DIV:     os << traits::get_node_info<BinaryOperator, traits::OPERATOR_NAME, BinaryOperator::DIV    >(); break;
        case BinaryOperator::REM:     os << traits::get_node_info<BinaryOperator, traits::OPERATOR_NAME, BinaryOperator::REM    >(); break;
        case BinaryOperator::ISAB:    os << traits::get_node_info<BinaryOperator, traits::OPERATOR_NAME, BinaryOperator::ISAB   >(); break;
        case BinaryOperator::ISABE:   os << traits::get_node_info<BinaryOperator, traits::OPERATOR_NAME, BinaryOperator::ISABE  >(); break;
        case BinaryOperator::ISLS:    os << traits::get_node_info<BinaryOperator, traits::OPERATOR_NAME, BinaryOperator::ISLS   >(); break;
        case BinaryOperator::ISLSE:   os << traits::get_node_info<BinaryOperator, traits::OPERATOR_NAME, BinaryOperator::ISLSE  >(); break;
        case BinaryOperator::ISEQ:    os << traits::get_node_info<BinaryOperator, traits::OPERATOR_NAME, BinaryOperator::ISEQ   >(); break;
        case BinaryOperator::ISNE:    os << traits::get_node_info<BinaryOperator, traits::OPERATOR_NAME, BinaryOperator::ISNE   >(); break;
        case BinaryOperator::ASGN:    os << traits::get_node_info<BinaryOperator, traits::OPERATOR_NAME, BinaryOperator::ASGN   >(); break;
        case BinaryOperator::ADDASGN: os << traits::get_node_info<BinaryOperator, traits::OPERATOR_NAME, BinaryOperator::ADDASGN>(); break;
        case BinaryOperator::SUBASGN: os << traits::get_node_info<BinaryOperator, traits::OPERATOR_NAME, BinaryOperator::SUBASGN>(); break;
        case BinaryOperator::MULASGN: os << traits::get_node_info<BinaryOperator, traits::OPERATOR_NAME, BinaryOperator::MULASGN>(); break;
        case BinaryOperator::DIVASGN: os << traits::get_node_info<BinaryOperator, traits::OPERATOR_NAME, BinaryOperator::DIVASGN>(); break;
        case BinaryOperator::REMASGN: os << traits::get_node_info<BinaryOperator, traits::OPERATOR_NAME, BinaryOperator::REMASGN>(); break;
        default: __builtin_unreachable();
    }
    write_in_file::field_end(os, enclosure);

    FIELD_SELF_ALIGNMENT(BinaryOperator, os, enclosure, 1LU, node.larg());
    FIELD_SELF_ALIGNMENT(BinaryOperator, os, enclosure, 2LU, node.rarg());

    write_in_file::write_end(os, enclosure);
}

template <>
void visit(While const& node, std::ofstream& os, size_t enclosure)
{
    write_in_file::write_begin<While>(os, enclosure);
    FIELD_SELF_ALIGNMENT(While, os, enclosure, 0LU, node.condition());
    FIELD_SELF_ALIGNMENT(While, os, enclosure, 1LU, node.body());
    write_in_file::write_end(os, enclosure);
}

template <>
void visit(If const& node, std::ofstream& os, size_t enclosure)
{
    write_in_file::write_begin<If>(os, enclosure);
    FIELD_SELF_ALIGNMENT(If, os, enclosure, 0LU, node.condition());
    FIELD_SELF_ALIGNMENT(If, os, enclosure, 1LU, node.body());
    write_in_file::write_end(os, enclosure);
}

template <>
void visit(Else const& node, std::ofstream& os, size_t enclosure)
{
    write_in_file::write_begin<Else>(os, enclosure);
    FIELD_SELF_ALIGNMENT(Else, os, enclosure, 0LU, node.body());
    write_in_file::write_end(os, enclosure);
}

template <>
void visit(Condition const& node, std::ofstream& os, size_t enclosure)
{
    write_in_file::write_begin<Condition>(os, enclosure);

    FIELD_BEGIN_SELF_ALIGNMENT(Condition, os, enclosure, 0LU);
    for (auto&& if_node : node.get_ifs())
        write(if_node, os, enclosure + field_enclosure_alignment);
    write_in_file::field_end(os, enclosure, true);

    FIELD_SELF_ALIGNMENT(Condition, os, enclosure, 1LU, node.get_else());

    write_in_file::write_end(os, enclosure);
}

} /* namespace last::node::write_overload_set */
