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
PUSH             9999999
CMP              <
JMP_TRUE         30
JMP              34
SADD             c
JMP              20
LOAD             c
CALL             print
LOAD             "a="
LOAD             a
CALL             print
EXIT