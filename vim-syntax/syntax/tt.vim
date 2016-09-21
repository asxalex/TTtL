if exists("b:current_syntax")
    finish
endif

syntax keyword ttKeyword define if while else
highlight link ttKeyword Keyword

syntax keyword ttFunction printf
highlight link ttFunction Function

syntax match ttComment "\v#.*$"
highlight link ttComment Comment

syntax region ttString start=/\v"/ skip=/\v\\./ end=/\v"/
syntax region ttString start=/\v'/ skip=/\v\\./ end=/\v'/
highlight link ttString String

syntax match ttConstant "[0-9]\+"
highlight link ttConstant Constant

let b:current_syntax = "tt"

