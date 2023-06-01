# EXSES Language Reference

| Operator Name | Operator symbol | Signature | Description |
| ------------- | --------------- | --------- | ----------- |
| DUP | `&` | `a -- a a` | Duplicates the value on top of the stack |
| OVER | `$&` | `a b -- a b a` | Duplicates the value over the top of the stack |
| DROP | `_` | `a --` | Drops(pops) value on top of the stack |
| SWAP | `$` | `a b -- b a` | Swaps 2 value on top of the stack |
| STRING_PLUS | `[+]` | `str1 str2 -- str1+str2` | Concatinates 2 strings on top of the stack |
| PLUS | `+` | `a b -- a+b` | Sums up 2 numbers on top of the stack |
| MINUS | `-` | `a b -- a-b` | Subtracts value on the top of the stack from value over the top of the stack |
| MULT | `*` | `a b -- a*b` | Multiplies up 2 numbers on top of the stack |
| DIV | `/` | `a b -- a/b` | Divides value on the top of the stack from value over the top of the stack |
| DUMP | `!` | `a --` | Prints value on top of the stack and drop it from the stack |
| BIND | `<-` | `name value --` | Binds `value` to `name` |
| SAVE | `<!` | `name value --` | BIND but updating the binding |
| LOAD | `^` | `name -- value` | Loads `value` of bind with the name `name` |
| TERNARY | `.?` | `main alt cond -- if cond: main else alt` | Ternary operator |
| MAKEPROC | `'` | `name --` | Starts procedure code block |
| ENDPROC | `"` | `--` | Ends procedure code block |
| INVOKEPROC | `:` | `name --` | Invokes procedure `name` |
| IF | `(` | `cond --` | Starts IF code block if `cond` is true |
| ENDIF | `)` | `--` | Ends IF code block |
| EQUAL | `=` | `a b -- a==b` | Equality comparison |
| NOTEQUAL | `<>` | `a b -- a!=b` | Not-quality comparison |
| LESS | `<` | `` | Less comparison |
| LESSEQUAL | `<=` | `a b -- a<=b` | Less-or-equal comparison |
| GREATER | `>` | `a b -- a > b` | Greater comparison |
| GREATEREQUAL | `` | `a b -- a >= b` | Greater-or-equal comparison |
| LOR | `\|\|` | `a b -- a\|\|b` | Logical OR |
| LAND | `&&` | `a b -- a&&b` | Logical AND |
| LNOT | `!!` | `a -- !a` | Logical NOT |
| TRUE | `true` | `1` | Logical TRUE |
| FALSE | `false` | `0` | Logical FALSE |
| STDLIB_MATH_SQRT | `Math(sqrt)` | `a -- sqrt(a)` | Square root of number `a` (NOTE: result is casted to int!) |
| STDLIB_MATH_COS | `Math(cos)` | `a -- cos(a)` | Cosine of number `a` (NOTE: result is casted to int!) |
| STDLIB_MATH_SIN | `Math(sin)` | `a -- sin(a)` | Sine of number `a` (NOTE: result is casted to int!) |
