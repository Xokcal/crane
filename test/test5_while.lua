int a = 0;
int count = 0;

while(count < 100000 and a < 5000){
    a = a + 3;
    count = count + 1;
}

print("quit while!!");
print("a=", a);
print("count=", count);

while(count > 0){
    a = a - 1;
    count = count - 2;
}

print("quit while!!");
print("a=", a);
print("count=", count);