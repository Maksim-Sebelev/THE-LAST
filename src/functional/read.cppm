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
AST read(std::filesystem::path const & ast_txt)
{
    /* must call create */
    // static_assert(false, "not ready now");
}

} /* namespace last */
