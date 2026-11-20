-- Outer variable defination.
int a = 10;
int b = 20;

-- Use function lexctop.

fn int lexctop(int a, int b){
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

string s1 = "CRANE NICE!!";
s1.len();
s1.split();
s1.map();
Map<long> m = crte_struct(Map, long);
m.psh("p1", 10293012312);
m.psh("p2", 0x378297382ff);
print("map p1=", m.po("p1"));
Link<int> l = crte_struct(Link, int);
l.lnk(34);
l.lnk(907);
print("link l=", l.fnd(34));