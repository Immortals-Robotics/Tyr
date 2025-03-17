#pragma once

namespace Immortals::Common::Debug
{
using StringMap = std::unordered_map<XXH32_hash_t, std::string>;

struct SourceLocation
{
    std::string_view file;
    std::string_view function;
    int              line;

    SourceLocation() = default;

    explicit SourceLocation(const std::source_location &t_source)
    {
        file     = t_source.file_name();
        function = t_source.function_name();
        line     = t_source.line();
    }

#if FEATURE_LOGGING
    explicit SourceLocation(const spdlog::source_loc &t_source)
    {
        file     = t_source.filename;
        function = t_source.funcname;
        line     = t_source.line;
    }
#endif
    
    SourceLocation(const Protos::Immortals::Debug::SourceLocation &t_source, const StringMap &t_strings)
    {
        file     = t_strings.at(t_source.file());
        function = t_strings.at(t_source.function());
        line     = t_source.line();
    }

    void fillProto(Protos::Immortals::Debug::SourceLocation *t_source, StringMap *t_strings) const
    {
        const XXH32_hash_t file_hash     = XXH32(file.data(), file.size(), 0);
        const XXH32_hash_t function_hash = XXH32(function.data(), function.size(), 0);

        t_source->set_file(file_hash);
        t_source->set_function(function_hash);
        t_source->set_line(line);

        if (!t_strings->contains(file_hash))
            t_strings->try_emplace(file_hash, std::string{file.data(), file.size()});
        if (!t_strings->contains(function_hash))
            t_strings->emplace(function_hash, std::string{function.data(), function.size()});
    }
};
} // namespace Immortals::Common::Debug
