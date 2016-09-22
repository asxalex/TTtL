if exists("b:current_syntax")
    finish
endif


syntax keyword ttFunction printf
highlight link ttFunction Function

syntax match ttComment "\v#.*$"
highlight link ttComment Comment

syntax match potionOperator "\v\*"
syntax match potionOperator "\v/"
syntax match potionOperator "\v\+"
syntax match potionOperator "\v-"
syntax match potionOperator "\v\?"
syntax match potionOperator "\v\*\="
syntax match potionOperator "\v/\="
syntax match potionOperator "\v\+\="
syntax match potionOperator "\v-\="
syntax match potionOperator "\v\=\="
syntax match potionOperator "\v!\="
highlight link potionOperator Operator

syntax match ttDefine "\w\+\s*("hs=s,he=e-1,ms=s,me=e-1
highlight link ttDefine Function
" he stands for highlight end
" me stands for match end
" FYI http://vimdoc.sourceforge.net/htmldoc/syntax.html#:syn-pattern-offset

syntax keyword ttKeyword define if while else require
highlight link ttKeyword Keyword

syntax region ttString start=/\v"/ skip=/\v\\./ end=/\v"/
syntax region ttString start=/\v'/ skip=/\v\\./ end=/\v'/
highlight link ttString String

syntax match ttConstant "[^a-zA-Z_]\([0-9]\+\)\v"hs=s+1
highlight link ttConstant Constant

let b:current_syntax = "tt"

