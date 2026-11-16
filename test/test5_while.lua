int a = 1;
int c = 0;
int p = 1000000000;
print("p=",p);
a++;
print("self add a=",a);
while(c < 200000010){
    c++;
}
print(c);
print("a=", a);
while(c > 199999997){
    print(c);
    c = c - 1;
}

print("endc = ",c);