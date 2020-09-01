if version < 600
  syn clear
elseif exists("b:current_syn")
  finish
endif

hi Normal ctermbg=white

syn region specLiteral start=/'/ end=/'/ contains=specEscape
syn match specEscape "\\[nrtbf\\']" contained
hi def link specLiteral SpecToken
hi def link specEscape SpecToken

highlight SpecToken ctermfg=brown
syn match specToken /[A-Z][a-zA-Z0-9_]*/
hi def link specToken SpecToken

highlight SpecRule ctermfg=blue
syn match specRule /[a-z][a-zA-Z0-9_]*/
hi def link specRule SpecRule

highlight SpecRuleStart ctermfg=126
syn match specRulestart /^[a-z][a-zA-Z0-9_]*/
hi def link specRulestart SpecRuleStart

highlight SpecRuleIndicator ctermfg=126
syn match specri /^    [:|]/
hi def link specri SpecRuleIndicator

highlight SpecOperator ctermfg=red cterm=bold
syn match specKeyword /[|?*()+]/
hi def link specKeyword SpecOperator

syn match specChange /BEGINCHANGED\|ENDCHANGED/
hi def link specChange Normal

let b:current_syn = "gspec"
