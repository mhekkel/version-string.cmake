VersionString.cmake
===================

About
-----

Wouldn't it be great if all software you use would have version information available built in? So you could ask for the version and know if it is the latest and greatest.

Most software has the command line switch `--version`, so you can do e.g.:

```bash
$ mrc --version
mrc version 1.3.10
```

Sometimes, software is even smarter and can provide more detail by adding the `--verbose` switch, like so:

```bash
$ mrc --version --verbose
mrc version 1.3.10
build: 126 2023-08-15T06:39:10Z
git tag: 963112d*
```

And, perhaps the application is using some very important library that has its own versioning scheme. You could get e.g. this:

```bash
$ tortoize --version --verbose
tortoize version 2.0.11
build: 118 2023-08-16T07:02:09Z
git tag: 07238a1
-
libdssp version 4.4.3
build: 247 2023-08-16T07:02:08Z
git tag: b0af4c9
-
libcifpp version 5.1.2
build: 1027 2023-08-16T07:02:08Z
git tag: 8565e1b
```

As you can see, tortoize uses libdssp and libcifpp.

Creating such version strings from within an application sounds trivial, but it is not. You need to have some global string containing the version info in your code at compile time and keeping that global in sync with the actual version is something that is best left to the tools you use to build the software. And that's precisely what VersionString.cmake offers.

Usage
-----

To use VersionString.cmake, you obviously need to have cmake and you have to write your code in C++. If you have those two covered, things are pretty simple.

Here's a hands-on. Let's write an application called hello-world. Here's a source file called `hello.cpp`.

```c++
#include <iostream>

int main(int argc, char * const argv[])
{
    std::cout << "Hello, world!" << std::endl;
    return 0;
}
```

And we build this with a cmake file:

```cmake
project(hello LANGUAGES CXX)
add_executable(hello ${PROJECT_SOURCE_DIR}/hello.cpp)
```

To support switches, we need to parse the `argv` vector. Let's keep things simple and use [libmcfp](https://github.com/mhekkel/libmcfp.git) for that:

```c++
#include <mcfp/mcfp.hpp>
#include <iostream>

int main(int argc, char * const argv[])
{
    auto &config = mcfp::config::instance();
    config.init("usage: hello",
        mcfp::make_option("version", "Show version information and exit"));
    config.parse(argc, argv);

    if (config.has("version"))
    {
        std::cout << "hello version 1.0" << std::endl;
        return 0;
    }

    std::cout << "Hello, world!" << std::endl;
    return 0;
}
```

And we need to tell cmake to use mcfp of course. BTW, let's add a version number to the cmake project.

```cmake
project(hello 1.0.1 LANGUAGES CXX)
find_package(libmcfp)
add_executable(hello ${PROJECT_SOURCE_DIR}/hello.cpp)
target_link_libraries(hello libmcfp::libmcfp)
```

Cool, we can now run hello and ask for its version:

```bash
$ build/hello --version
hello version 1.0
```

But wait, that's not our version, we entered _1.0.1_ as version number.

Time to start using *VersionString.cmake*.

First, add it to our project. Create a cmake directory and place the VersionString.cmake file there. Then edit the CMakeLists.txt file as follows (note that we now also need to specify the minimum cmake version):

```cmake
cmake_minimum_required(VERSION 3.15)
project(hello VERSION 1.0.1 LANGUAGES CXX)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
include(VersionString)
write_version_header(${PROJECT_SOURCE_DIR})

find_package(libmcfp)
add_executable(hello ${PROJECT_SOURCE_DIR}/hello.cpp)
target_link_libraries(hello libmcfp::libmcfp)
```

A cmake configure step will now generate a file called _revision.hpp_ in the source directory, i.e. the current directory in this case.

We can use this file in our program, which will now look like:

```c++
#include "revision.hpp"

#include <mcfp/mcfp.hpp>
#include <iostream>

int main(int argc, char * const argv[])
{
    auto &config = mcfp::config::instance();
    config.init("usage: hello",
        mcfp::make_option("version", "Show version information and exit"),
        mcfp::make_option("verbose", "Be more verbose"));
    config.parse(argc, argv);

    if (config.has("version"))
    {
        write_version_string(std::cout, config.has("verbose"));
        return 0;
    }

    std::cout << "Hello, world!" << std::endl;
    return 0;
}
```

Now, that was simple, wasn't it?

But if we run `build/hello --version --verbose` the output is the same as for simply `--version`. That's because the revision information from git is missing. Let's add that.

Start by initialising a git repository:

```bash
git init
```

Then add everything and check it in:

```bash
git add CMakeLists.txt hello.cpp cmake/VersionString.cmake
git commit -m'my first revision'
```

Then, the important part, tag this first release with a tag called _build_. That way, git can tell us how many check-in's there are between our current HEAD and the first check in. Note that this needs to be an annotated or signed tag.

```bash
git tag -a build
```

Now, we need to check in one more time to make things work. Let's take that opportunity by adding a .gitignore file (we don't want to check in the revision.hpp files)

```bash
echo '**/revision.hpp' > .gitignore
git add .gitignore
git commit -m'added .gitignore'
```

If you now build hello again, a new revision file is written containing information about the git version:

```bash
hello version 1.0.1
build: 1 2023-08-16T08:10:45Z
git tag: 54020dd
```

To show one more feature of VersionString.cmake, edit hello.cpp and add a new emtpy line or some other trivial change. Save the file but do not check in the change. Rebuild hello, and you'll notice a new output:

```bash
hello version 1.0.1
build: 1 2023-08-16T08:10:45Z
git tag: 54020dd*
```

You see, there's an asterisk following the git tag. That indicates that the application was built using code that was not checked in, the git status is *dirty*.

Libraries
---------

Now if we want to use a library and see it's version as well? Simple, let's go through the steps in a fast pace.

First create a library in our project, start with a new directory called mylib and place the following file called mylib.cpp (note we already include revision.hpp):

```c++
#include "mylib.h"
#include "revision.hpp"

int foo()
{
    return 42;
}
```

And the accompanying header file called foo.h:

```c++
int foo();
```

Add a CMakeLists.txt file here, not very trivial, but hey.

```cmake
cmake_minimum_required(VERSION 3.15)

# set the project name
project(mylib VERSION 0.1.0 LANGUAGES CXX)

include(VersionString)
write_version_header("${PROJECT_SOURCE_DIR}" LIB_NAME "mylib")

add_library(mylib ${PROJECT_SOURCE_DIR}/mylib.cpp)
add_library(mylib::mylib ALIAS mylib)

target_include_directories(mylib
	PUBLIC
	"$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}>"
	"$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>"
)
```

Did you notice the *LIB_NAME* parameter to `write_version_header`? That way the variables and structs inside revision.hpp get a new name so they won't conflict with the ones in your main application.

Update our hello project:

```cmake
cmake_minimum_required(VERSION 3.15)
project(hello VERSION 1.0.1 LANGUAGES CXX)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
include(VersionString)
write_version_header(${PROJECT_SOURCE_DIR})

find_package(libmcfp)

add_subdirectory(mylib)

add_executable(hello ${PROJECT_SOURCE_DIR}/hello.cpp)
target_link_libraries(hello libmcfp::libmcfp mylib::mylib)
```

And update hello.cpp to use this library:

```c++
#include "revision.hpp"

#include <mylib.h>

#include <mcfp/mcfp.hpp>
#include <iostream>

int main(int argc, char * const argv[])
{
    auto &config = mcfp::config::instance();
    config.init("usage: hello",
        mcfp::make_option("version", "Show version information and exit"),
        mcfp::make_option("verbose", "Be more verbose"));
    config.parse(argc, argv);

    if (config.has("version"))
    {
        write_version_string(std::cout, config.has("verbose"));
        return 0;
    }

    std::cout << "Hello, world!" << std::endl;

    (void)foo();

    return 0;
}
```

Now, the output of the verbose version information should look like:

```bash
hello version 1.0.1
build: 2 2023-08-16T08:16:18Z
git tag: 54020dd*
-
mylib version 0.1.0
build: 2 2023-08-16T08:16:18Z
git tag: 54020dd*
```

Ah, we should update the version number of our hello app, edit the CMakeLists.txt file of the main project and change the version to 1.1.0. And besides, let's add our lib to the git repository and check everything in.

Did that work? yes:

```bash
hello version 1.1.0
build: 3 2023-08-16T08:18:57Z
git tag: 4ce930f
-
mylib version 0.1.0
build: 3 2023-08-16T08:18:57Z
git tag: 4ce930f
```

Troubleshooting
---------------

Sometimes, library information does not show up. Modern linkers try to be smart and leave out all code they think is not used. So if you have your revision.hpp file included in a source file in your library, make sure this source file contains something that is referenced in your main application. One way to do this is by placing a global variable in the source file and assign a value to it in your main. A simple `int` is enough to do the trick.