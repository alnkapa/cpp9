#include <filesystem>
#include <unordered_set>
#include "argc/argc.h"
#include "file_finder.h"
#include <regex>

int main(int argc, char *argv[])
{
    namespace fs = std::filesystem;
    // параметры запуска
    arg::Argc arg;
    if (arg.parse(argc, argv) != 0)
    {
        return EXIT_FAILURE;
    }
    auto dir_except = std::make_shared<std::unordered_set<std::string>>();
    std::ranges::for_each(arg.directory_exception,
                          [&dir_except](const std::string &v) noexcept { dir_except->insert(v); });
    dir_except->rehash(dir_except->size());

    proccess p{arg.block_size, arg.hashes};

    std::regex re;
    bool is_re{false};
    if (!arg.wildcards.empty())
    {
        re.assign(arg.wildcards);
        is_re = true;
    }

    std::ranges::for_each(arg.directory,
                          [&dir_except, &p, &arg, &re, is_re](const std::string &v) noexcept
                          {
                              // TODO: thread - may be
                              try
                              {
                                  fs::path dir(v);
                                  for (auto dir_it = fs::recursive_directory_iterator(dir);
                                       dir_it != fs::recursive_directory_iterator();
                                       ++dir_it)
                                  {
                                      // TODO: parse for mask arg.wildcards !
                                      if (dir_it->is_directory())
                                      {
                                          if (dir_it.depth() + 1 > arg.deep_level)
                                          {
                                              dir_it.disable_recursion_pending();
                                          }
                                          else if (dir_except->contains(dir_it->path().filename()))
                                          {
                                              dir_it.disable_recursion_pending();
                                          }
                                      }
                                      else if (dir_it->is_regular_file() && dir_it->file_size() >= arg.file_size)
                                      {
                                          if (is_re)
                                          {
                                              if (!std::regex_match(dir_it->path().filename().string(), re))
                                              {
                                                  continue;
                                              }
                                          }
                                          p.file_cmp(dir_it->path(), dir_it->file_size());
                                      }
                                  }
                              }
                              catch (const std::filesystem::filesystem_error &ex)
                              {
                                  std::cout << "what():  " << ex.what() << '\n';
                              }
                              catch (const std::exception &ex)
                              {
                                  std::cout << "what():  " << ex.what() << '\n';
                              }
                              catch (...)
                              {
                                  std::cout << "Uncaught exception error \n";
                              };
                          });
    p.print_result();
    return EXIT_SUCCESS;
}
