[![CMake](https://github.com/EndeyshentLabs/EXSES/actions/workflows/cmake.yml/badge.svg)](https://github.com/EndeyshentLabs/EXSES/actions/workflows/cmake.yml)

# EXSES

**WARNING: THIS PROGRAMMING LANGUAGE IS UNFINISHED! EVERYTHING MAY CHANGE IN THE FUTURE!** 

<img src="./assets/logo1.png" alt="EXSES logp" width="128" />
EXSES - smol stack-oriented concatenative programming language. Development have just started: do not expect something revolutionary new.

## Example

### Hello, World!

NOTE: see [#4](https://github.com/EndeyshentLabs/EXSES/issues/4)

```python
[Hello,]
[World!]
$ ! !
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
500 80 > ( # if 500 is less than 80 do block. NOTE: `(` is if keyword in EXSES
    99 !
) # close the block
```

### More advanced

#### Bindings

```python
# bind 500 to '1100'
1100 500 <-
# bind 80 to '1101'
1101 80 <-

# load value of '1100' (e.g 500)
1100 ^
# load value of '1101' (e.g 80)
1101 ^
- # subtract
! # print (420)
```

#### Procedures

```python
1100 ' # int  # create a procedure named "1100"
    80 - ! # body of the procedure. subtract 80 from the value that was on top of the stack when procedure was invoked and print the result
" # close the procedure

500 1100 : # `:` call a procedure
```

## Build-it

Dependencies:

- CMake >= 3.14
- Any C++ compiler with C++20 (I use GCC 12.2)

```console
$ cmake -G Ninja -B build -DCMAKE_BUILD_TYPE=Release
$ cmake --build build --config Release
$ vim test.xes
... WRITING ...
$ ./build/exsi test.xes
```
