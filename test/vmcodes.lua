PUSH             1
STORE            a
TYPE             int
PUSH             0
STORE            c
TYPE             int
SADD             a
LOAD             "self add a="
LOAD             a
CALL             print
VAR              c
PUSH             29
CMP              leq
JMP_TRUE         30
JMP              41
VAR              a
VAR              a
ADD              +
REASSIGN         a
SADD             c
JMP              20
LOAD             c
CALL             print
LOAD             "a="
LOAD             a
CALL             print
EXIT