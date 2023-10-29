if exists('b:current_syntax')
  finish
endif

syntax keyword xesTodo contained TODO FIXME XXX NOTE

syntax region xesCommentLine start="#" end="$" display contains=xesTodo

syntax match xesSpecial contained '\\[nrtv\]]'
syntax region xesString start='\[' end=']' display contains=xesSpecial

syntax match xesOperators '+'
syntax match xesOperators '-'
syntax match xesOperators '\*'
syntax match xesOperators '/'
syntax match xesOperators '&'
syntax match xesOperators '\$&'
syntax match xesOperators '\$'
syntax match xesOperators '_'
syntax match xesOperators '!'
syntax match xesOperators '<-'
syntax match xesOperators '\^'
syntax match xesOperators '\^8'
syntax match xesOperators '\^16'
syntax match xesOperators '\^32'
syntax match xesOperators '\^64'
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
syntax match xesOperators 's!'
syntax match xesOperators 's+'
syntax match xesOperators '@'

syntax match xesSystem '>rax'
syntax match xesSystem '>rbx'
syntax match xesSystem '>rcx'
syntax match xesSystem '>rdx'
syntax match xesSystem '>rsi'
syntax match xesSystem '>rdi'
syntax match xesSystem '>rbp'
syntax match xesSystem '>r8'
syntax match xesSystem '>r9'
syntax match xesSystem '>r10'
syntax match xesSystem '\\syscall'

syntax match xesBindingSize 'byte'
syntax match xesBindingSize 'word'
syntax match xesBindingSize 'dword'
syntax match xesBindingSize 'qword'

syntax match xesNumber "\v<\d+>"
syntax match xesNumber "\v-<\d+>"

syntax keyword xesBooleans true false

syntax match xesConditional '\.?'
syntax match xesConditional '('
syntax match xesConditional ')'

syntax match xesLoop '{'
syntax match xesLoop '=>'
syntax match xesLoop '}'

highlight default link xesTodo           Todo
highlight default link xesNumber         Number
highlight default link xesCommentLine    Comment
highlight default link xesOperators      Keyword
highlight default link xesBooleans       Boolean
highlight default link xesConditional    Conditional
highlight default link xesLoop           Repeat
highlight default link xesString         String
highlight default link xesSpecial        Special
highlight default link xesSystem         Special
highlight default link xesBindingSize    Type

let b:current_syntax = 'exses'
