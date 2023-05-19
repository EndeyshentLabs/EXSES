# EXSES

## Example

```forth
20 10 + ! # Will output the '30'
# Raise 5 to the power of 2
5 & # `&` will duplicate top value e.g '5'
* # Multiply
! # Output
```

## Build-it

Dependencies:

- CMake >= 3.14
- Any C++ compiler with C++20 (I use GCC 12.2)

```console
$ cmake -G Ninja -B build
$ cmake --build build
$ vim test.xes
... WRITING ...
$ ./build/exsi test.xes
```
