#include <IO/OpCorporaIO.h>
#include <IO/UniDepIO.h>
#include <disamb/ContextDisambiguate.h>
#include <disamb/SingleWordDisambiguate.h>
#include <graphem/Tokenizer.h>
#include <morph/Processor.h>
#include <phem/Phemmer.h>
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/program_options.hpp>
#include <chrono>
#include <iostream>
#include <iterator>
#include <memory>
#include <string>

using namespace base;
using namespace tokenize;
using namespace analyze;
using namespace std;
using namespace utils;
using namespace disamb;
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

namespace po = boost::program_options;
namespace fs = boost::filesystem;
int main(int argc, char** argv) {
    boost::locale::generator gen;
    std::locale loc = gen("ru_RU.UTF8");
    std::locale::global(loc);

    fs::path exePath = executable_path(argv[0]);
    fs::path prefix = exePath.parent_path().parent_path();

    std::string defDPath = (prefix / "share/xmorphy/dicts").c_str();
    std::string defMPath = (prefix / "share/xmorphy/models").c_str();
    std::string defLibraryPath = "/usr/lib/libcatboostmodel.so";

    po::options_description desc(
        "XMorphy morphological analyzer for russian language.");
    desc.add_options()
        ("input,i", po::value<string>(), "set input file")
        ("output,o", po::value<string>(), "set output file")
        ("newline,n", "new line mode")
        ("disamb,d", "disambiguate")
        ("phem,p", "phemise")
        ("json,j", "json")
        ("mpath", po::value<string>()->default_value(defMPath), "models folder path")
        ("dpath", po::value<string>()->default_value(defDPath), "dicts folder path");
    po::variables_map vm;
    po::store(po::parse_command_line(argc, argv, desc), vm);
    po::notify(vm);
    std::istream* is = &cin;
    std::ostream* os = &cout;
    if (vm.count("input")) {
        is = new ifstream(vm["input"].as<string>());
    }
    if (vm.count("output")) {
        os = new ofstream(vm["output"].as<string>());
    }
    bool newline = false;
    if (vm.count("newline")) {
        newline = true;
    }
    io::OpCorporaIO opprinter;
    Tokenizer tok;
    std::string dpath = vm["dpath"].as<std::string>();
    std::string mpath = vm["mpath"].as<std::string>();
    if (!fs::exists(dpath)) {
        dpath = "./dicts";
    }

    if (!fs::exists(mpath)) {
        mpath = "./models";
    }

    std::string libPath = defLibraryPath;
    if(!fs::exists(libPath)) {
        libPath = "/usr/local/lib/libcatboostmodel.so";
    }
    if(!fs::exists(libPath)) {
        libPath = "./libcatboostmodel.so";
    }

    Processor anal(dpath + "/realdict_x", dpath + "/real_affixes_x.txt",
                   dpath + "/prefix_dict", dpath + "/realsuffixdict_x",
                   dpath + "/hyph_dict.txt");

    SingleWordDisambiguate swd(dpath + "/realdisambdict_x");

    disamb::ContextDisambiguator cdm(
        mpath + "/sp_model_clean", mpath + "/gender_model_clean",
        mpath + "/number_model_clean", mpath + "/case_model_clean");

    phem::Phemmer phemmer(dpath + "/phemdict", dpath + "/prefix_dict", libPath, mpath + "/catboostmodel");
    while (is->good() || is == &std::cin) {
        std::string inpfile = gulp(is);

        std::vector<TokenPtr> tokens = tok.analyze(UniString(inpfile));
        std::vector<WordFormPtr> forms = anal.analyze(tokens);
        if (vm.count("disamb")) {
            cdm.disambiguate(forms);
            swd.disambiguate(forms);
        }
        if (vm.count("phem")) {
            phemmer.phemise(forms);
        }
        if (vm.count("json")) {
            namespace pt = boost::property_tree;
            pt::ptree root;
            size_t i = 0;
            for (auto& ptr : forms) {
                auto sub = opprinter.writeToJSON(ptr);
                root.add_child(std::to_string(i++) + "_" + ptr->getWordForm().getRawString(), sub);
            }
            if (i > 0)
                pt::write_json(*os, root);
        } else {
            for (auto& ptr : forms) {
                (*os) << opprinter.write(ptr) << "\n";
            }
        }
        os->flush();
    }
    return 0;
}
