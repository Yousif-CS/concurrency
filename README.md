# concurrency

A concurrency utility library. Documentation can be found in the `docs`
directory.

## How to build

This project utilises boost, so make sure you have that
installed. After that, in a terminal, run the following:

```bash
    mkdir build && cd build
    cmake ..
    make 
    # To install
    make install
```

## How to run tests

After building, and starting from `build`, run the following

```bash
    cd tests
    ctest
```

## How to integrate

This is a header only library, once installed, you can integrate it
in a cmake project:

```cmake
find_package(Concurrency REQUIRED)

add_executable(example)

target_link_libraries(example concurrency)
```