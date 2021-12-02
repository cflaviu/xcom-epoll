import qbs

StaticLibrary {
    name: "xcom-epoll"
    Depends {
        name: "cpp"
    }
    files: [
        "inc/xcom/epoll/base.hpp",
        "inc/xcom/epoll/util.hpp",
        "inc/xcom/type.hpp",
        "inc/xcom/epoll/client.hpp",
        "inc/xcom/epoll/server.hpp",
        "src/xcom/epoll/util.cpp",
        "src/xcom/type.cpp",
    ]
    cpp.cxxLanguageVersion: "c++20"
    cpp.enableRtti: false
    cpp.includePaths: ["inc"]
    cpp.staticLibraries: ["pthread"]
}
