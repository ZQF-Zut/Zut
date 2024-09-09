# Zut
Cross-Platform Modern C++ Utility Library

## Features
- Modern C++ based
- Cross-platform
- UTF-8 support
- Isolation platform related
- Independence between modules

## Modules
- [ZxJson](https://github.com/ZQF-Zut/ZxJson) : `json library`
- [ZxHook](https://github.com/ZQF-Zut/ZxHook) : `x86 hooking wrapper`
- [ZxArg](https://github.com/ZQF-Zut/ZxArg) : `command line parser`
- [ZxFile](https://github.com/ZQF-Zut/ZxFile) : `file stream`
- [ZxFS](https://github.com/ZQF-Zut/ZxFS) : `file system`
- [ZxCvt](https://github.com/ZQF-Zut/ZxCvt) : `text converter`
- [ZxINI](https://github.com/ZQF-Zut/ZxINI) : `ini parser`
- [ZxMem](https://github.com/ZQF-Zut/ZxMem) : `memory stream`
- [Zx32](https://github.com/ZQF-Zut/Zx32) : `win32 api utf8 wrapper`

## Usage

- CMake

  please make sure that the cmake minimum required is greater than 3.20.
  ```cmake
  ...
  include(FetchContent)
  
  FetchContent_Declare(
    ZxFile
    GIT_REPOSITORY https://github.com/ZQF-Zut/ZxFile.git
    GIT_TAG df311de7938fdd9ba7b3db185e2bb2e1c44166ca) # the commit hash
  
  FetchContent_MakeAvailable(ZxFile)
  ...
  target_link_libraries(hello_zut PRIVATE Zut::ZxFile)
  ```
