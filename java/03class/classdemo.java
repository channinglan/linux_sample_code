	public class classdemo {
	   int a;
	     
	   // �o�̩w�q doSomething() ���Ĥ@�Ӫ���
	   public int doSomething(int p1) {
	      return a + p1;
	   }
	
	   // �o�̩w�q doSomething() ���ĤG�Ӫ���
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