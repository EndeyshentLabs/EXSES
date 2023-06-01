if exists('b:current_syntax')
  finish
endif

syntax keyword xesTodo contained TODO FIXME XXX NOTE

syntax region xesCommentLine start="#" end="$" display contains=xesTodo

syntax region xesString start='\[' end=']' display

syntax match xesOperators '+'
syntax match xesOperators '-'
syntax match xesOperators '\*'
syntax match xesOperators '/'
syntax match xesOperators '&'
syntax match xesOperators '\$&'
syntax match xesOperators '\$'
syntax match xesOperators '!'
syntax match xesOperators '<-'
syntax match xesOperators '\^'
syntax match xesOperators '\''
syntax match xesOperators '\"'
syntax match xesOperators ':'
syntax match xesOperators '='
syntax match xesOperators '<>'
syntax match xesOperators '<'
syntax match xesOperators '<='
syntax match xesOperators '>'
syntax match xesOperators '>='
syntax match xesOperators '||'
syntax match xesOperators '&&'
syntax match xesOperators '!!'

syntax match xesStdlibMath 'Math(sqrt)'
syntax match xesStdlibMath 'Math(sin)'
syntax match xesStdlibMath 'Math(cos)'

syntax match xesNumber "\v<\d+>"
syntax match xesNumber "\v-<\d+>"

syntax keyword xesBooleans true false

syntax match xesConditional '\.?'
syntax match xesConditional '('
syntax match xesConditional ')'

highlight default link xesTodo           Todo
highlight default link xesNumber         Number
highlight default link xesCommentLine    Comment
highlight default link xesOperators      Keyword
highlight default link xesBooleans       Boolean
highlight default link xesConditional    Conditional
highlight default link xesStdlibMath     Function
highlight default link xesString         String

let b:current_syntax = 'exses'
