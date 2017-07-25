class A   
{   
public:   
    A();   
    ~A();   
   
    static void XXX(void *p);   //此函数为线程回调函数 
    void start();   //线程启动函数 
    int a;   //在XXX函数中会进行修改 
 }; 
 
 void start()
 {
     _beginthread(XXX, 0, this);
 }
 
 void XXX(void *p)
 {
     A *handle = (A *)p;
     int b = handle->a;
 }
