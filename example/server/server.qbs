import qbs

CppApplication {
    consoleApplication: true
    Depends
    {
        name: "xcom-epoll"
    }
    files: [
        "../base/base_data_handler.hpp",
        "server.hpp",
        "main.cpp",
        "server.cpp",
    ]
    cpp.cxxLanguageVersion: "c++20"
    cpp.enableRtti: false
    cpp.includePaths: [
        "inc",
        "../../source/inc",
        "../base",
    ]
    cpp.staticLibraries: ["pthread"]
}
