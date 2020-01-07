#include <IO/OpCorporaIO.h>
#include <graphem/Tokenizer.h>
#include <morph/Processor.h>
#include <phem/Phemmer.h>
#include <disamb/SingleWordDisambiguate.h>
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/program_options.hpp>
#include <chrono>
#include <iostream>
#include <iterator>
#include <memory>
#include <string>
#include <Resource.h>
#include <sstream>

using namespace base;
using namespace disamb;
using namespace tokenize;
using namespace analyze;
using namespace std;
using namespace utils;
using namespace ml;

std::string executable_path_fallback(const char* argv0) {
    if (argv0 == nullptr || argv0[0] == 0) {
        return "";
    }
    boost::system::error_code ec;
    boost::filesystem::path p(boost::filesystem::canonical(
        argv0, boost::filesystem::current_path(), ec));
    return p.make_preferred().string();
}

#if (BOOST_OS_CYGWIN || BOOST_OS_WINDOWS) // {

#include <Windows.h>

std::string executable_path(const char* argv0) {
    char buf[1024] = {0};
    DWORD ret = GetModuleFileNameA(NULL, buf, sizeof(buf));
    if (ret == 0 || ret == sizeof(buf)) {
        return executable_path_fallback(argv0);
    }
    return buf;
}

#elif (BOOST_OS_MACOS) // } {

#include <mach-o/dyld.h>

std::string executable_path(const char* argv0) {
    char buf[1024] = {0};
    uint32_t size = sizeof(buf);
    int ret = _NSGetExecutablePath(buf, &size);
    if (0 != ret) {
        return executable_path_fallback(argv0);
    }
    boost::system::error_code ec;
    boost::filesystem::path p(
        boost::filesystem::canonical(buf, boost::filesystem::current_path(), ec));
    return p.make_preferred().string();
}

#elif (BOOST_OS_SOLARIS) // } {

#include <stdlib.h>

std::string executable_path(const char* argv0) {
    std::string ret = getexecname();
    if (ret.empty()) {
        return executable_path_fallback(argv0);
    }
    boost::filesystem::path p(ret);
    if (!p.has_root_directory()) {
        boost::system::error_code ec;
        p = boost::filesystem::canonical(p, boost::filesystem::current_path(), ec);
        ret = p.make_preferred().string();
    }
    return ret;
}

#elif (BOOST_OS_BSD) // } {

#include <sys/sysctl.h>

std::string executable_path(const char* argv0) {
    int mib[4] = {0};
    mib[0] = CTL_KERN;
    mib[1] = KERN_PROC;
    mib[2] = KERN_PROC_PATHNAME;
    mib[3] = -1;
    char buf[1024] = {0};
    size_t size = sizeof(buf);
    sysctl(mib, 4, buf, &size, NULL, 0);
    if (size == 0 || size == sizeof(buf)) {
        return executable_path_fallback(argv0);
    }
    std::string path(buf, size);
    boost::system::error_code ec;
    boost::filesystem::path p(boost::filesystem::canonical(
        path, boost::filesystem::current_path(), ec));
    return p.make_preferred().string();
}

#elif (BOOST_OS_LINUX) // } {

#include <unistd.h>

std::string executable_path(const char* argv0) {
    char buf[1024] = {0};
    ssize_t size = readlink("/proc/self/exe", buf, sizeof(buf));
    if (size == 0 || size == sizeof(buf)) {
        return executable_path_fallback(argv0);
    }
    std::string path(buf, size);
    boost::system::error_code ec;
    boost::filesystem::path p(boost::filesystem::canonical(
        path, boost::filesystem::current_path(), ec));
    return p.make_preferred().string();
}

#else // } {

std::string executable_path(const char* argv0) {
    return executable_path_fallback(argv0);
}

#endif // }

std::string gulp(std::istream* in) {
    std::string ret;
    if (in == &std::cin) {
        ret = std::string((std::istreambuf_iterator<char>(*in)),
                          std::istreambuf_iterator<char>());
    } else {
        char buffer[4096];
        while (in->read(buffer, sizeof(buffer)))
            ret.append(buffer, sizeof(buffer));
        ret.append(buffer, in->gcount());
    }
    return ret;
}

struct Options
{
    std::string inputFile;
    std::string outputFile;
    bool disambiguate = false;
    bool json = false;
};

namespace po = boost::program_options;
namespace fs = boost::filesystem;
bool processCommandLineOptions(int argc, char ** argv, Options & opts)
{
    try
    {
        po::options_description desc(
            "XMorphy morphological analyzer for Russian language.");
        desc.add_options()
            ("input,i", po::value<string>(&opts.inputFile), "set input file")
            ("output,o", po::value<string>(&opts.outputFile), "set output file")
            ("disamb,d", "disambiguate")
            ("json,j", "json");

        po::variables_map vm;
        po::store(po::parse_command_line(argc, argv, desc), vm);

        if (vm.count("help"))
        {
            std::cout << desc << "\n";
            return false;
        }

        po::notify(vm);
        if (vm.count("disamb"))
            opts.disambiguate = true;
        if (vm.count("json"))
            opts.json = true;
    }
    catch (const std::exception & ex)
    {
        std::cerr << "Error: " << ex.what() << "\n";
        return false;
    }
    catch (...)
    {
        std::cerr << "Unknown error!" << "\n";
        return false;
    }
    return true;
}


int main(int argc, char** argv) {
    boost::locale::generator gen;
    std::locale loc = gen("ru_RU.UTF8");
    std::locale::global(loc);

    fs::path exePath = executable_path(argv[0]);
    fs::path prefix = exePath.parent_path().parent_path();

    Options opts;

    if (!processCommandLineOptions(argc, argv, opts))
        return 1;


    std::istream* is = &cin;
    std::ostream* os = &cout;
    if (!opts.inputFile.empty()) {
        is = new ifstream(opts.inputFile);
    }
    if (!opts.outputFile.empty()) {
        os = new ofstream(opts.outputFile);
    }
    io::OpCorporaIO opprinter;
    Tokenizer tok;

    const auto & factory = CppResource::ResourceFactory::instance();

    std::istringstream mainIs(factory.getAsString("maindict"));
    std::istringstream affixIs(factory.getAsString("affixdict"));
    std::istringstream prefixDict(factory.getAsString("prefixdict"));
    std::istringstream suffixDict(factory.getAsString("suffixdict"));
    std::istringstream hyphDict(factory.getAsString("hyphdict"));
    Processor anal(mainIs, affixIs, prefixDict, suffixDict, hyphDict);


    std::istringstream disambdict(factory.getAsString("disambdict"));

    SingleWordDisambiguate disamb(disambdict);

    while (is->good() || is == &std::cin) {
        std::string inpfile = gulp(is);

        std::vector<TokenPtr> tokens = tok.analyze(UniString(inpfile));
        std::vector<WordFormPtr> forms = anal.analyze(tokens);
        if (opts.disambiguate)
            disamb.disambiguate(forms);
        for (auto& ptr : forms) {
            (*os) << opprinter.write(ptr) << "\n";
        }
        os->flush();
    }
    return 0;
}
