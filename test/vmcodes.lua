VAR              a
PUSH             2
ADD              +
PUSH             2
SUB              -
PUSH             10
CMP              <
CONDI            or
JMP_TRUE         49
VAR              a2
PUSH             1
CMP              >
CONDI            and
JMP_FALSE        39
VAR              a3
PUSH             9
ADD              +
PUSH             19
CMP              eq
CONDI            or
JMP_TRUE         49
PUSH             4
PUSH             9
CMP              eq
JMP_TRUE         49
JMP              61
LOAD             "hello"
LOAD             "crane if!!"
CALL             print
LOAD             "call 2 if inner"
LOAD             1432
CALL             print
PUSH             3
PUSH             8
CMP              >
JMP_TRUE         71
JMP              75
LOAD             "en second if"
CALL             print
LOAD             "out"
CALL             print
EXIT