struct MyStruct
{
   P * p ;

   void foo()
   {
      if(this->p == NULL)
      {
         this->p = new P() ;
      }

      // Lots of code, some using this->p

      if(this->p != NULL)
      {
         delete this->p ;
         this->p = NULL ;
      }
   }
} ;

truct MyStruct
{
   size_t  c ;
   P *     p ;

   void foo()
   {
      if(c == 0)
      {
         this->p = new P() ;
      }

      ++c ;

      // Lots of code, some using this->p

      --c ;

      if(c == 0)
      {
         delete this->p ;
         this->p = NULL ;
      }
   }
} ;

struct MyStruct
{
   mutex   m ; // recursive mutex
   size_t  c ;
   P *     p ;

   void foo()
   {
      lock(m) ;
      if(c == 0)
      {
         this->p = new P() ;
      }

      ++c ;
      unlock(m) ;

      // Lots of code, some using this->p

      lock(m) ;
      --c ;

      if(c == 0)
      {
         delete this->p ;
         this->p = NULL ;
      }
      unlock(m) ;
   }
} ;
