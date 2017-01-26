	// 使用與 API 相同名稱的變數
	public class var {
	   public static void main(String[] args) {
	      	String a = "22";
        	boolean aa = true;
        	char b = '1';
        	char[] c = {'1', '0'};
        	double d = 1.0;
        	float e = 1.0f;
        	int f = 1;
        	long g = 1;
        	Object h = new Object();
        	String i = "1";
			Object o = new Object();
			
        String ab = "Three are %d dogs and %d cats.\n";
        String bb = "Three are %f dogs and %f cats.\n";
        String cb = "Three are %s dogs and %s cats.\n";			
        String db = "==============================\n";				
        	 
        	System.out.println(aa);
        	System.out.println(b);
        	System.out.println(c);
        	System.out.println(d);
        	System.out.println(e);
        	System.out.println(f);
        	System.out.println(g);
        	System.out.println(h);
        	System.out.println(i);	 
	 
	 
	      System.out.println();

	      System.out.println(o.toString());	      
	      System.out.println();	      
	      System.out.println(a.getClass());	      	      
	      System.out.println();
	  
	      // 變數 String 與 API 中的名稱相同
	      Integer String = 22;
	      System.out.println();	  
	      System.out.println(String.getClass());
	      System.out.println();
	      
        System.out.printf(ab, 33, 22);
        System.out.printf(bb, 11.0, 3.0);
        System.out.printf(cb, "54", "87");     
        System.out.printf(db);
        System.out.printf("---------------------------------\n");         	      
	      
	   }
	}