#include "versioningCompiler/CompilerImpl/SystemCompiler.hpp"
#include "versioningCompiler/CompilerImpl/SystemCompilerOptimizer.hpp"
#include "versioningCompiler/Utils.hpp"
#include <map>
#include <sched.h>

#include <dlfcn.h> // needed for loadSymbol
#include <filesystem>
#include <fstream>
#include <iostream>

#include <boost/dll/library_info.hpp>
#include <boost/dll/shared_library.hpp>
#ifndef WORKING_DIR
#define WORKING_DIR "."
#endif
#ifndef PATH_TO_TEST_CPP_DIRECTORY
#define PATH_TO_TEST_CPP_DIRECTORY "./"
#endif
#ifndef TEST_CPP_FILENAME
#define TEST_CPP_FILENAME "test.cpp"
#endif

#ifndef ORIGINAL_SHARED_LIB_PATH
#define ORIGINAL_SHARED_LIB_PATH                                               \
  "./externalLibrary/testlib/target/release/libtestlib.so"
#endif
// Function signatures that we will be using
typedef void (*void_signature_t)();
typedef const int (*string_signature_t)(const std::string &, std::string &);

void run_test(const std::string original_binary);

std::vector<std::string> printAllSymbolsInSO(const std::filesystem::path &bin) {
  void **handler = nullptr;
  std::vector<std::string> symbols = {};
  if (exists(bin)) {
    boost::dll::library_info inf(bin.c_str());
    symbols = inf.symbols();

    // Printing symbols
    for (std::size_t j = 0; j < symbols.size(); ++j) {
      std::cout << "Symbol " << j + 1 << ":\t" << symbols[j] << std::endl;
    }
  } else {
    std::string error_str =
        "cannot load symbol from " + bin.string() + " : file not found";
    std::cerr << error_str;
  }
  return symbols;
}

int main(int argc, char const *argv[]) {

  cpu_set_t mask;
  CPU_ZERO(&mask);
  CPU_SET(0, &mask);
  sched_setaffinity(0, sizeof(mask), &mask);

  vc::vc_utils_init();
  // Print All symbols in the .so
  printAllSymbolsInSO(ORIGINAL_SHARED_LIB_PATH);
  // Run a test
  run_test(ORIGINAL_SHARED_LIB_PATH);
  return 0;
}

void run_test(const std::string rust_shared_library) {
  // --------- Load a .so built from rust
  std::vector<std::string> symbols;
  symbols.push_back("cxxbridge1$hello_world");
  symbols.push_back("cxxbridge1$rust_function");

  // Create a version from an existing .so. The compiler in this case is
  // irrelevant because it won't actually build a version or be versioned
  vc::version_ptr_t versionFromSo = vc::Version::Builder::createFromSO(
      rust_shared_library, symbols, vc::make_compiler<vc::SystemCompiler>(),
      false);
  // Check if the symbol in the shared library has been loaded. If it has not,
  // print some debug info
  if (!versionFromSo->compile()) {
    std::cerr << "version from .so failed to compile" << std::endl;
    if (versionFromSo->hasGeneratedBin()) {
      std::cerr << "version from .so has binary "
                << versionFromSo->getFileName_bin() << std::endl;
    }
    if (versionFromSo->hasLoadedSymbol()) {
      std::cerr << "version from .so has loaded symbol" << std::endl;
    }
    exit(-1);
  }
  // Check that all symbols have been correctly loaded
  if (!versionFromSo->hasLoadedSymbol()) {
    std::cerr << "version from .so hasn't loaded symbol" << std::endl;
    exit(-1);
  }
  // Get the actual function
  void_signature_t hello_wrld = (void_signature_t)versionFromSo->getSymbol(0);
  string_signature_t sha_rust = (string_signature_t)versionFromSo->getSymbol(1);
  // Run the rust function
  if (!hello_wrld) {
    std::cerr << "Rust version's get symbol failed" << std::endl;
    exit(-1);
  }
  hello_wrld();
  std::string hello_wrld_sha = "";
  auto out = sha_rust("hello world", hello_wrld_sha);
  std::cout << "Sha of \"hello world\" from rust: " << hello_wrld_sha
            << " returned " << out << std::endl;

  // --------- Build and import a cpp function

  // Now, prepare a builder for another version
  const std::string patch_dir = PATH_TO_TEST_CPP_DIRECTORY;
  const std::string patch_source = patch_dir + TEST_CPP_FILENAME;
  // Create a compiler wrapper for clang
  vc::compiler_ptr_t clang = vc::make_compiler<vc::SystemCompilerOptimizer>(
      "clang++", std::filesystem::u8path("clang++"),
      std::filesystem::u8path("opt"), std::filesystem::u8path(WORKING_DIR),
      std::filesystem::u8path("./clang++.log"));
  // Create a builder
  vc::Version::Builder builder;
  // Specify the functions to be loaded
  builder._functionName.push_back("printDate");     // Symbol 0
  builder._functionName.push_back("rust_function"); // Symbol 1
  builder._fileName_src.push_back(patch_source); // The file which contains the
                                                 // functions
  builder._compiler = clang;                     // The compiler to be used
  builder._autoremoveFilesEnable =
      true; // Remove the generated intermediate files
  builder._genIROptionList = {
      vc::Option("Set code as position independent", "-fPIC")};
  vc::version_ptr_t v2 = builder.build(); // Prepare the version builder
  /*
  // Prepare the IR
  if (!v2->prepareIR()) {
    if (!clang->hasIRSupport()) {
      std::cerr << "Error: something went wrong with the compiler."
                << std::endl;
    } else if (!v2->hasGeneratedIR()) {
      std::cerr << "Error: generation of IR v2 failed." << std::endl;
    } else {
      std::cerr << "Error: optimization of IR v2 failed." << std::endl;
    }
    std::cerr << "\tPlease check the compiler/optimizer install path in "
                 "main.cpp source file"
              << std::endl;
  }*/
  // Compile the cpp functions
  if (!v2->compile()) {
    if (!v2->hasGeneratedBin()) {
      std::cerr << "Error: compilation 2 failed." << std::endl;
    } else {
      std::cerr << "Error: symbol 2 not loaded" << std::endl;
    }
    exit(-1);
  }
  void_signature_t funv2 = (void_signature_t)v2->getSymbol();
  // Run the cpp version
  if (!funv2) {
    std::cerr << "Cpp version's get symbol failed" << std::endl;
    exit(-1);
  }
  funv2();
  string_signature_t fake_sha_rust = (string_signature_t)v2->getSymbol(1);
  std::string fake_sha = "";
  auto out2 = fake_sha_rust("hello world", fake_sha);
  std::cout << "Overwritten version of sha of \"hello world\" from rust: "
            << fake_sha << " returned " << out2 << std::endl;
  // close
  versionFromSo->fold();
  v2->fold();
  // Try again to load the shared library
  versionFromSo->reload();
  sha_rust = (string_signature_t)versionFromSo->getSymbol(1);
  hello_wrld_sha = "";
  out = sha_rust("hello world2", hello_wrld_sha);
  std::cout << "Sha of \"hello world2\" from rust: " << hello_wrld_sha
            << " returned " << out << std::endl;
  hello_wrld_sha = "";
  out = sha_rust("", hello_wrld_sha);
  std::cout << "Sha of an empty string from rust: " << hello_wrld_sha
            << " returned " << out << std::endl;
  versionFromSo->fold();
}