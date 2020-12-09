# CPGen - A C++ project generator

## What is CPGen

CPGen is a CLI tool to create CMake and Conan based C++ projects.

Once a project is created with CPGen, you can reuse the tool to add components to it:
* libraries: static, shared, module or header only
* applications
* tests

After a project is setup, CPGen is no longer required. It is not a new tool you have to impose on your users.

## Goals & Non Goals

Goals:
* Very fast project creation: you shouldn't spend more than a minute to create the structure of a project with multiple libraries, applications and tests
* Use of standard tools: CMake as the build system and Conan as the package manager. Other auxiliary tools might also be supported (e.g. clang-format)
* Straightforward build: just call CMake to configure and build the project. Others tools, including Conan, are handled automatically
* Non intrusive: the only trace of CPGen in your project is an empty `.cpgen` file at the root. It allows CPGen to identify a compatible project where new components can be added
* Standard project structure: all CPGen generated projects are organized the same way so it's easier to find your way around
* Push best practices: modern CMake, increased warning level by default, etc

Non goals:
* Manage every possible tool: I prefer to handle one tool well than ten badly
* Deal with every use case: the provided template is simple yet flexible and should work for the vast majority of cases. No usability sacrifice will be made to cater corner cases
* Update dependencies after project or component creation: it would requires CPGen to parse CMake and Python files to gather the existing dependencies, which can be very complicated. New dependencies have to be added manually

## How to get CPGen

CPGen relies on CMake and Conan so make sure you have both installed first

### From source
```bash
cd cpgen/build
cmake ..
cmake --build . --parallel
export PATH=$PATH:`pwd`/bin # or copy bin/cpgen somewhere in your path
```

### Binaries

Check the GitHub release page to find prebuilt binaries

## How to use CPGen

First download the latest project template
```bash
cpgen update
```
Then create a project
```bash
cpgen new-project --name my_cool_project                \
                  --root ~/dev                          \
                  --version 1.0.0                       \
                  --description "A bunch of cool stuff" \
                  --conan-pkgs catch2/2.13.0 fmt/7.1.2  \
                  --cmake-pkgs Threads                   
```
And finally add a bunch of components to it
```bash
cd ~/dev/my_cool_project
cpgen add-library    --name greeter                           \
                     --type static                            \
                     --std 11                                 \
                     --dependencies CONAN_PKG::fmt            \
      add-test       --name say-hello                         \
                     --std 20                                 \
                     --dependencies CONAN_PKG::catch2 greeter \
      add-executable --name welcome                           \
                     --std 17                                 \
                     --dependencies greeter Threads::Threads   
```
All these steps can also be combined into a single one
```bash
cpgen update new-project ... add-library ...
```

After that it is business as usual:
```bash
cd ~/dev/my_cool_project/build
cmake -DENABLE_TESTING=ON ..
cmake --build . --parallel
ctest
```

## TODO

- [ ] Get feedback
- [ ] CI for binary generation
- [ ] Unit testing
- [ ] Windows support
- [ ] macOS support