import qbs

CppApplication {
    consoleApplication: true
    Depends
    {
        name: "xcom-epoll"
    }
    files: [
        "../base/base_data_handler.hpp",
        "client.hpp",
        "client.cpp",
        "main.cpp",
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
