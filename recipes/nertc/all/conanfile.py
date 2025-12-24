import os
from conans import ConanFile, tools, CMake


class NetEaseRTCConan(ConanFile):
    name = "nertc"
    description = "NetEase RTC G2"
    license = "GNU Public License or the Artistic License"
    homepage = "https://yunxin.163.com/"
    url = "https://github.com/netease-im/yunxin-center-index"
    topics = ("conan", "rtc", "nertc", "nrtc")
    settings = "os", "arch"
    short_paths = True

    def config_options(self):
        if self.settings.os == "Windows":
            del self.options.fPIC

    def build(self):
        os = str(self.settings.os)
        arch = str(self.settings.arch)
        tools.get(**self.conan_data["sources"][self.version][os][arch], strip_root=True)

    def package(self):
        if (self.settings.os == 'Macos'):
            self.copy("*.framework/*", src="sdk/", dst="", symlinks=True)
        if (self.settings.os == 'Windows'):
            arch = 'x64' if self.settings.arch == 'x86_64' else 'x86'
            self.copy("*.dll", src=os.path.join('dll', arch), dst="bin")
            self.copy("*.lib", src=os.path.join('lib', arch), dst="lib")
            self.copy("*.h", src='api', dst="include")

    def package_info(self):
        if (self.settings.os == 'Macos'):
            self.cpp_info.frameworkdirs.append(self.package_folder)
            # headers
            self.cpp_info.includedirs = [
                "nertc_sdk_Mac.framework/Headers",
                "NERtcAiHowling.framework/Headers",
                "NERtcFaceDetect.framework/Headers",
                "NERtcPersonSegment.framework/Headers",
                "NERtcnn.framework/Headers",
                "NERtcBeauty.framework/Headers",
                "NERtcAiDenoise.framework/Headers",
            ]
            self.cpp_info.includedirs.append("NEFundation_Mac.framework/Headers")
            # link libraries
            self.cpp_info.frameworks.append("nertc_sdk_Mac")
            self.cpp_info.frameworks.append("NERtcAiHowling")
            self.cpp_info.frameworks.append("NERtcFaceDetect")
            self.cpp_info.frameworks.append("NERtcPersonSegment")
            self.cpp_info.frameworks.append("NERtcnn")
            self.cpp_info.frameworks.append("NERtcBeauty")
            self.cpp_info.frameworks.append("NERtcAiDenoise")
            self.cpp_info.frameworks.append("NEFundation_Mac")
        if (self.settings.os == 'Windows'):
            self.cpp_info.libdirs = ["lib"]
            self.cpp_info.includedirs = ["include"]
            self.cpp_info.libs = tools.collect_libs(self)
