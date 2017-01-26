	public class classdemo {
	   int a;
	     
	   // 這裡定義 doSomething() 的第一個版本
	   public int doSomething(int p1) {
	      return a + p1;
	   }
	
	   // 這裡定義 doSomething() 的第二個版本
	   public double doSomething(double p1) {
	      return a + p1 + a;
	   }
	      
	   public static void main(String[] args) { 
	      classdemo d = new classdemo();
	      d.a = 11;
	
	      System.out.println();
	      System.out.println(d.doSomething(5));
	      System.out.println(d.doSomething(7.2));
	      System.out.println();
	   }
}	  