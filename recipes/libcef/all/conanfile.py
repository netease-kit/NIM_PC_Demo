from conan import ConanFile
from conan.errors import ConanInvalidConfiguration
from conan.tools.cmake import CMake, CMakeToolchain, cmake_layout
from conan.tools.files import get, copy, collect_libs
from conan.tools.build import check_min_cppstd
import os


class CEFConan(ConanFile):
    name = "libcef"
    description = "The Chromium Embedded Framework (CEF) is a simple framework for embedding Chromium-based browsers in other applications."
    license = "GNU Public License or the Artistic License"
    homepage = "https://github.com/chromiumembedded/cef"
    url = "https://github.com/conan-io/conan-center-index"
    topics = ("cef", "chromium")
    settings = "os", "arch", "compiler", "build_type"
    options = {
        "shared": [True, False],
        "fPIC": [True, False]
    }
    default_options = {
        "shared": False,
        "fPIC": True
    }
    short_paths = True

    def validate(self):
        if self.settings.compiler.get_safe("cppstd"):
            check_min_cppstd(self, 11)
        if self.settings.os not in ["Windows", "Macos"]:
            raise ConanInvalidConfiguration(
                f"{self.ref} unsupported platform."
            )

    def config_options(self):
        if self.settings.os == "Windows":
            del self.options.fPIC

    def configure(self):
        if self.options.shared:
            self.options.rm_safe("fPIC")

    def layout(self):
        cmake_layout(self, src_folder="src")

    def generate(self):
        tc = CMakeToolchain(self)
        tc.variables["CMAKE_BUILD_TYPE"] = "Release" if self.settings.build_type == "Release" else "Debug"
        tc.variables["BUILD_SHARED_LIBS"] = "ON" if self.options.shared else "OFF"
        tc.variables["BUILD_TESTING"] = "OFF"
        tc.generate()

    def build(self):
        get(self, **self.conan_data["sources"][self.version]
            [str(self.settings.os)][str(self.settings.arch)])
        cmake = CMake(self)
        cmake.configure(build_script_folder=self.build_folder)
        cmake.build(target="install")

    def package(self):
        copy(self, "*.h", src=os.path.join(self.build_folder, "include"), dst=os.path.join(self.package_folder, "include/include"))
        copy(self, "*.h", src=os.path.join(self.build_folder, "libcef_dll"), dst=os.path.join(self.package_folder, "libcef_dll"))
        copy(self, "*.dll", src=os.path.join(self.build_folder, "Release"), dst=os.path.join(self.package_folder, "bin"))
        copy(self, "*.exe", src=os.path.join(self.build_folder, "Release"), dst=os.path.join(self.package_folder, "bin"))
        copy(self, "*.bin", src=os.path.join(self.build_folder, "Release"), dst=os.path.join(self.package_folder, "bin"))
        copy(self, "*.lib", src=os.path.join(self.build_folder, "Release"), dst=os.path.join(self.package_folder, "lib"))
        copy(self, "*.*", src=os.path.join(self.build_folder, "Resources"), dst=os.path.join(self.package_folder, "bin"))

    def package_info(self):
        self.cpp_info.libdirs = ["lib"]
        self.cpp_info.includedirs = ["include", "libcef_dll"]
        self.cpp_info.libs = collect_libs(self)
