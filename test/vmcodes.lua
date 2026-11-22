PUSH             100086
STORE            u
TYPE             int
FNNAME           pint
FNCODE           12
JMP              29
PARAM_LOAD       oer
TYPE             int
LOAD             "en pint"
CALL             print
LOAD             24
LOAD             "__"
LOAD             oer
CALL             print
RET              RET
PUSH             10
STORE            a
TYPE             int
LOAD             "a="
LOAD             a
CALL             print
LOAD             10
STORE_RET        47
CALL             pint
EXIT