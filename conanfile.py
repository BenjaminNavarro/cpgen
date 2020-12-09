from conans import ConanFile, CMake, tools


class CPGenConan(ConanFile):
    name = "cpgen"
    version = "0.1"
    license = "BSD"
    author = "Benjamin Navarro <navarro.benjamin13@gmail.com>"
    url = "https://github.com/BenjaminNavarro/cpgen"
    description = "A C++ project generator based on CMake and Conan"
    topics = "C++", "Conan", "CMake"
    settings = "os", "compiler", "build_type", "arch"
    options = {"build_tests": [True, False]}
    default_options = {"build_tests": False}
    generators = "cmake"
    requires = "fmt/7.1.2", "cli11/1.9.1", "libcurl/7.73.0", "libarchive/3.4.3"
    exports_sources = "!.clangd*", "!.ccls-cache*", "!compile_commands.json", "*"

    def configure(self):
        self.options["libarchive"].with_acl = False

    def requirements(self):
        if self.options.build_tests:
            self.requires("cppcheck_installer/2.0@bincrafters/stable")
            self.requires("catch2/2.13.0")

    def build(self):
        cmake = CMake(self)
        if self.options.build_tests:
            cmake.definitions["ENABLE_TESTING"] = True
        cmake.configure()
        cmake.build()
        if self.options.build_tests:
            cmake.test()
        cmake.install()

    def package(self):
        self.copy("bin/*", dst="bin", keep_path=False)
