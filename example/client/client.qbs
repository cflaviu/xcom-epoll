import qbs

CppApplication {
    consoleApplication: true
    Depends
    {
        name: "xcom-epoll"
    }
    files: [
        "client.hpp",
        "client.cpp",
        "main.cpp",
    ]
    cpp.cxxLanguageVersion: "c++20"
    cpp.enableRtti: false
    cpp.includePaths: ["inc", "../../source/inc"]
    cpp.staticLibraries: ["pthread"]
}
