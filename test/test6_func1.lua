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

fn void t5(int t5a, int t5b){
 print("en t5_____t5a=",t5a);
 int t5t1 = 10;
 while(t5t1 < t5a){
  t5t1 = t5t1 + 1;
  if(t5t1 eq 50 or t5t1 eq 51){
   int if1 = 10;
   print("en if");
   while(t5t1 < t5b){
    t5t1 = t5t1 + if1;
    print("if t5t1=",t5t1);
   }
  }
  print("while t5t1=",t5t1);
 }
 print("t5 while is OUT");
}
t5(100, 80);

int end = 1011;
print("end=",end);
t1(78);
t5(200, 180);
