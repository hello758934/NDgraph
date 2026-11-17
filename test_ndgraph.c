#include "ndgraph_expr.h"
#include "complex_math.h"
#include <math.h>
#include <stdio.h>
#include <string.h>

static int pass=0,fail=0;
static void check(int ok,const char *name){
    printf("%s %s\n",ok?"PASS":"FAIL",name);
    if(ok)pass++;else fail++;
}
static void expression(const char *s,double x,double y,double t,double zr,double zi,double re,double im){
    NDExpr e;NDVars v;char err[140];NDComplex q;int ok;
    int parsed=nd_parse_expr(s,&e,err,sizeof(err));
    if(!parsed){printf(" PARSE ERROR %s: %s\n",s,err);check(0,s);return;}
    memset(&v,0,sizeof(v));v.x=x;v.y=y;v.t=t;v.z.re=zr;v.z.im=zi;
    q=nd_eval(&e,&v,&ok);
    check(ok&&fabs(q.re-re)<1e-6&&fabs(q.im-im)<1e-6,s);
    if(ok&&!(fabs(q.re-re)<1e-6&&fabs(q.im-im)<1e-6))
        printf(" got %g+%gi expected %g+%gi\n",q.re,q.im,re,im);
}
static void compile(const char *s,int imag,int type,int flag){
    char err[140];NDGraph g;int ok=nd_compile(s,imag,&g,err,sizeof(err));
    if(!ok)printf(" COMPILE ERROR %s: %s\n",s,err);
    check(ok&&g.kind==type&&g.function_light==flag,s);
}
static void implicit(const char *s,int expected){
    char err[140];NDGraph g;int ok=nd_compile(s,0,&g,err,sizeof(err));
    if(!ok){printf(" COMPILE ERROR %s: %s\n",s,err);check(0,s);return;}
    check(nd_real_implicit_is_function(&g,-4,4,-4,4)==expected,s);
}
int main(void){
    char err[140];NDGraph g;
    expression("sin(pi/2)",0,0,0,0,0,1,0);
    expression("cos(0)",0,0,0,0,0,1,0);
    expression("tan(pi/4)",0,0,0,0,0,1,0);
    expression("abs(3+4i)",0,0,0,0,0,5,0);
    expression("log(e)",0,0,0,0,0,1,0);
    expression("ln(e^2)",0,0,0,0,0,2,0);
    expression("exp(i*pi)",0,0,0,0,0,-1,0);
    expression("sqrt(-1)",0,0,0,0,0,0,1);
    expression("re(z)",0,0,0,3,4,3,0);
    expression("im(z)",0,0,0,3,4,4,0);
    expression("conj(z)",0,0,0,3,4,3,-4);
    expression("arg(i)",0,0,0,0,0,1.5707963267949,0);
    expression("-x^2",3,0,0,0,0,-9,0);
    expression("(-x)^2",3,0,0,0,0,9,0);
    expression("2^3^2",0,0,0,0,0,512,0);
    expression("2z",0,0,0,3,4,6,8);
    expression("3sin(pi/2)",0,0,0,0,0,3,0);
    expression("t*(1+i)",0,0,2,0,0,2,2);
    expression("z^4",0,0,0,0,1,1,0);
    expression("sin(i)",0,0,0,0,0,0,1.17520119364);
    compile("z^4=1",1,ND_IMAG_ROOTS,0);
    compile("z=z^2",1,ND_IMAG_ROOTS,0);
    compile("z=1+i",1,ND_IMAG_ROOTS,0);
    compile("z=sin(i)",1,ND_IMAG_POINT,0);
    compile("z=t*(1+i)",1,ND_IMAG_PARAM,1);
    compile("z=cos(t)+i*sin(t)",1,ND_IMAG_PARAM,1);
    compile("re(z)=im(z)",1,ND_IMAG_LOCUS,0);
    compile("abs(z)=1",1,ND_IMAG_LOCUS,0);
    compile("y=x",0,ND_REAL_EXPLICIT,1);
    compile("f(x)=sin(x)",0,ND_REAL_EXPLICIT,1);
    compile("x=2",0,ND_REAL_IMPLICIT,0);
    compile("x=cos(t);y=sin(t)",0,ND_REAL_PARAM,0);
    compile("y=t^2",0,ND_REAL_EXPLICIT,1);
    compile("x(t)=t;y(t)=t^2",0,ND_REAL_PARAM,0);
    {
      NDGraph locus;int ok=nd_compile("re(z)=im(z)",1,&locus,err,sizeof(err));
      check(ok&&nd_real_implicit_is_function(&locus,-4,4,-4,4)==1,"imag locus line light green");
      ok=nd_compile("abs(z)=1",1,&locus,err,sizeof(err));
      check(ok&&nd_real_implicit_is_function(&locus,-4,4,-4,4)==0,"imag locus circle light off");
    }
    implicit("x=y",1);
    implicit("x=2",0);
    implicit("x=y^2",0);
    implicit("x^2+y^2=4",0);
    check(!nd_compile("",1,&g,err,sizeof(err)),"blank input stays blank");
    check(!nd_compile("z=sin(z)",1,&g,err,sizeof(err)),"unsupported complex relation rejected");
    check(!nd_compile("y=z",0,&g,err,sizeof(err)),"real mode rejects z");
    check(!nd_compile("z=t*z",1,&g,err,sizeof(err)),"param requires t only");
    check(!nd_compile("x=cos(t);y=sin(z)",0,&g,err,sizeof(err)),"real param rejects z");
    printf("%d passed, %d failed\n",pass,fail);
    return fail?1:0;
}
