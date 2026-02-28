module;

#include <ostream>
#include <fstream>
#include <filesystem>
#include <utility>

export module ast_read;

export import ast;

namespace last
{

export
template <typename... Signatures>
AST read_ast_txt(std::filesystem::path const & ast_txt)
{
    /* must call create */
    static_assert(false, "not ready now");
}

} /* namespace last */
