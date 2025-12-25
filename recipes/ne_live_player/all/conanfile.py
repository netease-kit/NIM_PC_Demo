from conan import ConanFile
from conan.errors import ConanInvalidConfiguration
from conan.tools.cmake import CMake, CMakeToolchain, cmake_layout
from conan.tools.files import get, copy, collect_libs
from conan.tools.build import check_min_cppstd
import os


class CEFConan(ConanFile):
    name = "ne_live_player"
    description = "NetEase Yunxin live player."
    license = "GNU Public License or the Artistic License"
    homepage = "https://yunxin.163.com"
    url = "https://github.com/conan-io/conan-center-index"
    topics = ("player", "yunxin")
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

    def build(self):
        get(self, **self.conan_data["sources"][self.version]
            [str(self.settings.os)], strip_root=True)

    def package(self):
        release_folder = os.path.join(
            self.build_folder, "release/x64") if self.settings.arch == "x86_64" else os.path.join(self.build_folder, "release/x86")
        print(os.path.join(release_folder, "include"))
        copy(self, "*.h", src=os.path.join(release_folder, "include"),
             dst=os.path.join(self.package_folder, "include"))
        copy(self, "*.dll", src=os.path.join(release_folder, "lib"),
             dst=os.path.join(self.package_folder, "bin"))
        copy(self, "*.lib", src=os.path.join(release_folder, "lib"),
             dst=os.path.join(self.package_folder, "lib"))

    def package_info(self):
        self.cpp_info.libdirs = ["lib"]
        self.cpp_info.includedirs = ["include"]
        self.cpp_info.libs = collect_libs(self)
