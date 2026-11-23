int e = 9;
int pi = 3 * e * 3;

fn void calc(int p, int r){
 print("p=",p);
 print("r=",r);
 int circle = p * r;
 circle = circle + 16;
 print("circle = ",circle);
}

calc(pi, 10);
calc(e, 23);