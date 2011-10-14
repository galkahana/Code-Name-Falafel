

static  const  char  *  scTest1  =  "testSymbol1"  ;


class  MyClass
{
public  :
MyClass  (  )
{
mA  =  1  ;
mB  =  scTest1  ;
}


private  :
int  mA  ;
char  *  mB  ;

}  ;

int  MyFunc  (  int  inA  ,  int  inB  )
{
printf  (  "testing"  )  ;
return  inA  +  inB  ;
}