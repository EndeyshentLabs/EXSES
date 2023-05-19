if exists('b:current_syntax')
  finish
endif

syntax keyword xesTodo contained TODO FIXME XXX NOTE

syntax region xesCommentLine start="#" end="$" display contains=xesTodo

syntax match xesOperators '+'
syntax match xesOperators '-'
syntax match xesOperators '\*'
syntax match xesOperators '/'
syntax match xesOperators '&'
syntax match xesOperators '\$'
syntax match xesOperators '!'
syntax match xesOperators '<-'
syntax match xesOperators '\^'

syntax match xesNumber "\v<\d+>"
syntax match xesNumber "\v-<\d+>"

highlight default link xesNumber         Number
highlight default link xesCommentLine    Comment
highlight default link xesOperators      Keyword

let b:current_syntax = 'exses'
