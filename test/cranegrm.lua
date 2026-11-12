-- Outer variable defination.
int a = 10;
int b = 20;

-- Use function lexctop.

fn lexctop(int a, int b){
 $int lexin = 10;
 $string strarr[10] = {"you", "are", "man", "bo"};
 print("lexin=", lexin);
 strarr[0].sadd("r");
 return a * b - lexin, strarr;
}
-- Get function return result.

int cto;
string strcto[];
cto, strcto = lexctop(a, b);

-- Print return datas.
print("cto s=", strcto.sarray());
print("cto %d=", cto);