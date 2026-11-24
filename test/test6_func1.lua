int a = 13;

fn void t1(int t1a){print("t1a", t1a);}
t1(a);

fn void t2(int t2a, int t2b){
 print("t2a=",t2a," t2b=",t2b);
}
t2(a, 91);

fn void t3(int t3a, int t3b){
 int t3t1 = 100;
 print("t3t1=",t3t1);
 t3t1 = t3a + t3b;
 print("next t3t1=",t3t1);
}
t3(1,1000);
t3(1,3);

int r = 10021;
print("r=",r);

fn void t4(int t4a){
 if(t4a leq 10000){
  print("if t4a=",t4a);
 }
 if(t4a > 10000){
  print("NO PRINT");
 }
}
t4(412);

fn void t5(int t5a){
 print("t5a=",t5a);
 int t5t1 = 10;
 while(t5t1 < 24){
  t5t1 = t5t1 + 5;
  print("while t5t1=",t5t1);
 }
 print("t5 while is OUT");
}
t5(82139);

int end = 1011;
print("end=",end);

t1(78);