[![CMake](https://github.com/EndeyshentLabs/EXSES/actions/workflows/cmake.yml/badge.svg)](https://github.com/EndeyshentLabs/EXSES/actions/workflows/cmake.yml)

# EXSES

**WARNING: THIS PROGRAMMING LANGUAGE IS UNFINISHED! EVERYTHING MAY CHANGE IN THE FUTURE!** 

<img src="./assets/logo1.png" alt="EXSES logp" width="128" />
EXSES - smol stack-oriented concatenative programming language. Development have just started: do not expect something revolutionary new.

## [Language Reference](./REFERENCE.md)

## Example

### Hello, World!

```python
[Hello, World!\n] s!
```

### Basic

```python
20 10 + ! # Will output the '30'
# Raise 5 to the power of 2
5 & # `&` will duplicate top value e.g '5'
* # Multiply
! # Output
```

### Control flow

```python
500 80 > ( # if 500 is less than 80 do following code block
    99 !
) # close the block
```

### More advanced

#### Bindings

```python
# bind 500 to 'FiveOO'
FiveOO <- |~ 8 ~| # allocate 8 bytes (64 bits)
500 FiveOO <!
# bind 80 to 'EightO'
EightO <- |~ 8 ~|
80 EightO <!

# load value of 'FiveOO' (e.g 500)
FiveOO ^ ^64 # ^64 - dereferencing
# load value of 'EightO' (e.g 80)
1101 ^ ^64
- # subtract
! # print (420)
```

#### Procedures

```python
minus80 ' # int  # create a procedure named "minus80".
    80 - ! # body of the procedure. subtract 80 from the value that was on top of the stack when procedure was invoked and print the result
" # close the procedure

500 minus80 : # `:` call a procedure
```

## Build-it

Dependencies:

- CMake >= 3.14
- Any C++ compiler with C++20 (I use GCC 13.2)
- [fmtlib](https://github.com/fmtlib/fmt). You can probably do something like `sudo apt install libfmt-dev`

```console
$ cmake -G Ninja -B build -DCMAKE_BUILD_TYPE=Release
$ cmake --build build --config Release
$ vim test.xes
... WRITING ...
$ ./build/exsi nasm-linux-x86_64 test.xes # only for linux for now
```
