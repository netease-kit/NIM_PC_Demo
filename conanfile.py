from conans import ConanFile, tools
import platform


class ModuleConan(ConanFile):
    settings = "os", "compiler", "build_type", "arch"
    build_requires = (
        # "sqlcipher/4.4.2",
        "openssl/1.1.1k",
        "tinyxml/2.6.2",
        "jsoncpp/1.9.4",
        "libuv/1.41.0",
        "libyuv/cci.20201106",
        "sqlite3/3.35.3",
        "cef_2623_binaries/1.0.0-3-ge03c0ce@yunxin/testing",
        "nls_play/1.0.0-1-g3f1fcc3@yunxin/testing",
        "nim_p2p_sdk/1.0.0-3-g6a6cedb@yunxin/testing",
        "nim_cpp_wrapper/8.5.0@yunxin/testing",
        "nertc/4.1.1-2-g0dc7953@yunxin/testing"
    )
    generators = "cmake"
    build_policy = "missing"

    def imports(self):
        self.copy("*.dll", "bin", "bin", keep_path=True)
        self.copy("*.pak", "bin", "bin", keep_path=True)
        self.copy("*.dat", "bin", "bin", keep_path=True)
        self.copy("cef*.pak", src="bin", dst="bin")
        self.copy("*.txt", "bin", "bin", keep_path=True)
        self.copy("*.bin", "bin", "bin", keep_path=True)
        self.copy("wow_helper.exe", "bin", "bin", keep_path=True)
        self.copy("*.html", "bin", "bin", keep_path=True)
        self.copy("*.dylib", "lib", "lib")
        self.copy("*.lib", "lib", "lib")
        self.copy("*.h", "include", "include", keep_path=True)
