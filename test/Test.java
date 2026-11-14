public class Test {
    public static void main(String[] args) {
        // 1. 最简单的打印
        System.out.println("hello world!!");

        // 2. 跑一个 700 万次的纯计算循环
        int c = 0;
        int a = 1;
        long start = System.currentTimeMillis();
        
        while (c < 7000000) {
            c = c + 1;
            a = a + 1;
        }
        
        long end = System.currentTimeMillis();
        System.out.println("Java 700万次循环耗时: " + (end - start) + " 毫秒");
        System.out.println("c=" + c + ", a=" + a);
    }
}