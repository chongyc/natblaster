syntax match MACRO_USE "[a-z0-9]\@![A-Z_0-9]\+[ (),*;\]\n\t|]\@="
hi link MACRO_USE MACRO

syntax match ERRORTYPE "ERROR[A-Z_1-9]\+"
hi ERRORTYPE ctermfg=6 

syntax match FUNCTION "[a-zA-Z0-9_]\+(\@="
hi FUNCTION ctermfg=7

syntax keyword OTHER_ERR SUCCESS NOT_OK
hi link OTHER_ERR ERRORTYPE

syntax keyword mytypedefs errorcode
hi link mytypedefs Type

syntax match MYTYPE "[a-z_]*_t[ (),*;|]\@="
hi link MYTYPE Type





