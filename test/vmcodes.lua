PUSH             90
PUSH             1
ADD              +
STORE            qwertyuiop
TYPE             int
PUSH             79
PUSH             1
SUB              -
STORE            cranepreviewDRC
TYPE             int
VAR              qwertyuiop
PUSH             90
CMP              leq
CONDI            or
JMP_TRUE         38
VAR              cranepreviewDRC
PUSH             79
CMP              geq
JMP_TRUE         38
JMP              46
LOAD             "91!!"
LOAD             " && "
LOAD             "78!!"
CALL             print
LOAD             "complex calculate logic quit."
CALL             print
EXIT