import qbs

CppApplication {
    consoleApplication: true
    Depends
    {
        name: "xcom-epoll"
    }
    files: [
        "main.cpp"
    ]
    cpp.cxxLanguageVersion: "c++20"
    cpp.enableRtti: false
    cpp.includePaths: ["inc"]
}
