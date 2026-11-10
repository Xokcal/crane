int base = 100;
int level = 4;
int score = base * 2 + level * 5 - 10 / 2;
print("Init score:", score);

if (score > 200) {
    print("Level 1: High Score!");
    int bonus = score * 2; 
    print("Bonus in block:", bonus);
    if(score < 201){
        print("score > 201");
    }
    print("if1 end");
}
print("out if1");

if (score > 200) {
    print("Level 1: High Score!");
    int bonus = score * 2; 
    print("Bonus in block:", bonus);
    if(score > 201){
        print("score > 201");
        if(score geq 205){
            print("score geq 205");
            print("score=", score);
            if(score eq 215){
                print("score eq 215");
            }
        }
    }
    print("if1 end");
}
print("out if1");