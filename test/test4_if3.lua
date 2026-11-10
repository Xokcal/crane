int a = 10;
int b = 20;
int c = 30;

if (a + b > c or b * 2 < a * 5 or c / 3 eq a) {
    print("Math logic passed!");
    
    int d = a * 2 + b - c / 3;
    if (d eq c) {
        print("Nested math passed! d=", d);
    }
}

if (a < b) {
    int y = 9012 * 21/3;
    if(b < c){
        print("a < b < c is true", y);
    }
}