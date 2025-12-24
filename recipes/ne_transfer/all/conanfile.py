from conan import ConanFile
from conan.errors import ConanInvalidConfiguration
from conan.tools.cmake import CMake, CMakeToolchain, cmake_layout
from conan.tools.files import get, copy, collect_libs
from conan.tools.build import check_min_cppstd
import os


class CEFConan(ConanFile):
    name = "ne_transfer"
    description = "NetEase Yunxin P2P file transfer."
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
            [str(self.settings.os)][str(self.settings.arch)])

    def package(self):
        print('package')
        copy(self, "*.h", src=os.path.join(self.build_folder, "include"), dst=os.path.join(self.package_folder, "include"))
        if (self.settings.build_type == 'Debug'):
            copy(self, "nim_p2p_inlan_d.dll", src=os.path.join(self.build_folder, "bin"), dst=os.path.join(self.package_folder, "bin"))
            copy(self, "nim_p2p_sdk_d.dll", src=os.path.join(self.build_folder, "bin"), dst=os.path.join(self.package_folder, "bin"))
            copy(self, "Qt5Cored.dll", src=os.path.join(self.build_folder, "bin"), dst=os.path.join(self.package_folder, "bin"))
            copy(self, "Qt5Networkd.dll", src=os.path.join(self.build_folder, "bin"), dst=os.path.join(self.package_folder, "bin"))
        else:
            copy(self, "nim_p2p_inlan.dll", src=os.path.join(self.build_folder, "bin"), dst=os.path.join(self.package_folder, "bin"))
            copy(self, "nim_p2p_sdk.dll", src=os.path.join(self.build_folder, "bin"), dst=os.path.join(self.package_folder, "bin"))
            copy(self, "Qt5Core.dll", src=os.path.join(self.build_folder, "bin"), dst=os.path.join(self.package_folder, "bin"))
            copy(self, "Qt5Network.dll", src=os.path.join(self.build_folder, "bin"), dst=os.path.join(self.package_folder, "bin"))
        

    def package_info(self):
        self.cpp_info.libdirs = ["lib"]
        self.cpp_info.includedirs = ["include"]
        self.cpp_info.libs = collect_libs(self)
