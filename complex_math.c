#include "complex_math.h"
#include <ctype.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define CM_PI 3.14159265358979323846
#define CM_TINY 1e-20

typedef struct { CMComplex a[CM_MAX_DEGREE+1]; int deg; } Poly;
typedef struct { const char *s; int pos; char err[96]; } Parser;

static CMComplex c_make(double r,double i){CMComplex a={r,i};return a;}
static CMComplex c_add(CMComplex a,CMComplex b){return c_make(a.re+b.re,a.im+b.im);}
static CMComplex c_sub(CMComplex a,CMComplex b){return c_make(a.re-b.re,a.im-b.im);}
static CMComplex c_mul(CMComplex a,CMComplex b){return c_make(a.re*b.re-a.im*b.im,a.re*b.im+a.im*b.re);}
static double c_abs(CMComplex a){return hypot(a.re,a.im);}
static CMComplex c_div(CMComplex a,CMComplex b){
    /* Scale before dividing to avoid squaring huge components. */
    double s=fmax(fabs(b.re),fabs(b.im));
    if(s<CM_TINY)return c_make(NAN,NAN);
    double x=b.re/s,y=b.im/s,d=s*(x*x+y*y);
    return c_make((a.re*x+a.im*y)/d,(a.im*x-a.re*y)/d);
}
static Poly p_zero(void){Poly p;memset(&p,0,sizeof(p));return p;}
static Poly p_constant(CMComplex c){Poly p=p_zero();p.a[0]=c;return p;}
static Poly p_z(void){Poly p=p_zero();p.deg=1;p.a[1]=c_make(1,0);return p;}
static void fail(Parser *p,const char *s){if(!p->err[0])snprintf(p->err,sizeof(p->err),"%s at %d",s,p->pos+1);}
static char peek(Parser *p){return p->s[p->pos];}
static int eat(Parser *p,char ch){if(peek(p)==ch){p->pos++;return 1;}return 0;}
static void trim(Poly *p){while(p->deg>0&&c_abs(p->a[p->deg])<1e-11)p->deg--;}
static Poly p_add(Poly a,Poly b,int sign){
    Poly r=p_zero();int i;r.deg=a.deg>b.deg?a.deg:b.deg;
    for(i=0;i<=r.deg;i++)r.a[i]=sign>0?c_add(a.a[i],b.a[i]):c_sub(a.a[i],b.a[i]);
    trim(&r);return r;
}
static Poly p_mul(Parser *ps,Poly a,Poly b){
    Poly r=p_zero();int i,j;
    if(a.deg+b.deg>CM_MAX_DEGREE){fail(ps,"Degree exceeds 10");return r;}
    r.deg=a.deg+b.deg;
    for(i=0;i<=a.deg;i++)for(j=0;j<=b.deg;j++)r.a[i+j]=c_add(r.a[i+j],c_mul(a.a[i],b.a[j]));
    trim(&r);return r;
}
static Poly p_div(Parser *ps,Poly a,Poly b){
    int i;
    if(b.deg!=0){fail(ps,"Division must be by constant");return p_zero();}
    if(c_abs(b.a[0])<CM_TINY){fail(ps,"Division by zero");return p_zero();}
    for(i=0;i<=a.deg;i++)a.a[i]=c_div(a.a[i],b.a[0]);
    return a;
}
static Poly p_pow(Parser *ps,Poly a,int n){
    Poly r=p_constant(c_make(1,0));int i;
    if(n<0||n>CM_MAX_DEGREE){fail(ps,"Power must be 0..10");return p_zero();}
    for(i=0;i<n&&!ps->err[0];i++)r=p_mul(ps,r,a);
    return r;
}
static Poly parse_expr(Parser *ps);
static Poly parse_atom(Parser *ps){
    char ch=peek(ps);
    if(eat(ps,'(')){
        Poly a=parse_expr(ps);
        if(!eat(ps,')'))fail(ps,"Missing )");
        return a;
    }
    if(eat(ps,'z'))return p_z();
    if(eat(ps,'i'))return p_constant(c_make(0,1));
    if(isdigit((unsigned char)ch)||ch=='.'){
        char *end;
        double v=strtod(ps->s+ps->pos,&end);
        if(end==ps->s+ps->pos){fail(ps,"Expected number");return p_zero();}
        ps->pos=(int)(end-ps->s);
        if(!isfinite(v)||fabs(v)>1e8){fail(ps,"Number out of range");return p_zero();}
        return p_constant(c_make(v,0));
    }
    fail(ps,"Expected number, z, i or (");return p_zero();
}
static Poly parse_power(Parser *ps){
    Poly a=parse_atom(ps);
    if(eat(ps,'^')){
        int n=0;
        if(!isdigit((unsigned char)peek(ps))){fail(ps,"Use integer powers 0..10");return p_zero();}
        while(isdigit((unsigned char)peek(ps))){n=n*10+peek(ps)-'0';ps->pos++;if(n>10){fail(ps,"Power exceeds 10");return p_zero();}}
        a=p_pow(ps,a,n);
    }
    return a;
}
static Poly parse_unary(Parser *ps){
    if(eat(ps,'+'))return parse_unary(ps);
    if(eat(ps,'-')){
        Poly a=parse_unary(ps);int i;
        for(i=0;i<=a.deg;i++){a.a[i].re=-a.a[i].re;a.a[i].im=-a.a[i].im;}
        return a;
    }
    return parse_power(ps);
}
static Poly parse_term(Parser *ps){
    Poly a=parse_unary(ps);
    while(!ps->err[0]){
        char ch=peek(ps);
        if(ch=='*'){ps->pos++;a=p_mul(ps,a,parse_unary(ps));}
        else if(ch=='/'){ps->pos++;a=p_div(ps,a,parse_unary(ps));}
        else if(ch=='z'||ch=='i'||ch=='('){a=p_mul(ps,a,parse_unary(ps));}
        else break;
    }
    return a;
}
static Poly parse_expr(Parser *ps){
    Poly a=parse_term(ps);
    while(!ps->err[0]){
        if(eat(ps,'+'))a=p_add(a,parse_term(ps),1);
        else if(eat(ps,'-'))a=p_add(a,parse_term(ps),-1);
        else break;
    }
    return a;
}
static CMComplex p_eval(const Poly *p,CMComplex z){
    CMComplex v=c_make(0,0);int j;
    for(j=p->deg;j>=0;j--)v=c_add(c_mul(v,z),p->a[j]);
    return v;
}
static CMComplex p_deriv_eval(const Poly *p,CMComplex z){
    CMComplex v=c_make(0,0);int j;
    for(j=p->deg;j>=1;j--)v=c_add(c_mul(v,z),c_mul(p->a[j],c_make(j,0)));
    return v;
}
static int sort_cmp(const void *aa,const void *bb){
    const CMComplex *a=(const CMComplex*)aa,*b=(const CMComplex*)bb;
    double pa=atan2(a->im,a->re),pb=atan2(b->im,b->re);
    return pa<pb?-1:pa>pb?1:0;
}
static int solve_poly(const Poly *poly,CMResult *res,char *msg,size_t mlen){
    CMComplex z[CM_MAX_DEGREE],lead=poly->a[poly->deg];
    Poly p=*poly;
    double radius=1,worst=0;
    int n=p.deg,i,j,k,iter,converged=0;
    res->degree=n;res->count=0;res->infinite_solutions=0;
    if(n==0){
        if(c_abs(p.a[0])<1e-10){res->infinite_solutions=1;snprintf(msg,mlen,"All complex numbers solve this");}
        else snprintf(msg,mlen,"No solutions");
        return 1;
    }
    for(i=0;i<=n;i++)p.a[i]=c_div(p.a[i],lead);
    for(i=0;i<n;i++)if(c_abs(p.a[i])>radius)radius=c_abs(p.a[i]);
    radius+=1;
    if(!isfinite(radius)||radius>1e9){snprintf(msg,mlen,"Coefficients too large");return 0;}
    for(i=0;i<n;i++){
        double angle=2*CM_PI*i/n+0.23;
        z[i]=c_make(radius*cos(angle),radius*sin(angle));
    }
    for(iter=0;iter<450;iter++){
        worst=0;
        for(i=0;i<n;i++){
            CMComplex den=c_make(1,0),delta;
            for(j=0;j<n;j++)if(i!=j)den=c_mul(den,c_sub(z[i],z[j]));
            if(c_abs(den)<1e-17)den=c_add(den,c_make(1e-9,1e-9));
            delta=c_div(p_eval(&p,z[i]),den);
            z[i]=c_sub(z[i],delta);
            if(c_abs(delta)>worst)worst=c_abs(delta);
            if(!isfinite(worst)||worst>1e12){snprintf(msg,mlen,"Solver overflow");return 0;}
        }
        if(worst<1e-11){converged=1;break;}
    }
    for(i=0;i<n;i++){
        for(iter=0;iter<35;iter++){
            CMComplex val=p_eval(&p,z[i]),der=p_deriv_eval(&p,z[i]),step;
            if(c_abs(der)<1e-14)break;
            step=c_div(val,der);z[i]=c_sub(z[i],step);
            if(c_abs(step)<1e-12)break;
        }
        if(!isfinite(z[i].re)||!isfinite(z[i].im)||c_abs(p_eval(&p,z[i]))>1e-5){
            snprintf(msg,mlen,"Numerical solver did not converge");return 0;
        }
        if(fabs(z[i].re)<1e-7)z[i].re=0;
        if(fabs(z[i].im)<1e-7)z[i].im=0;
        if(fabs(z[i].re-nearbyint(z[i].re))<1e-7)z[i].re=nearbyint(z[i].re);
        if(fabs(z[i].im-nearbyint(z[i].im))<1e-7)z[i].im=nearbyint(z[i].im);
    }
    qsort(z,n,sizeof(z[0]),sort_cmp);
    for(i=0;i<n;i++){
        int unique=1;
        for(k=0;k<res->count;k++)if(c_abs(c_sub(z[i],res->roots[k]))<1e-3){unique=0;break;}
        if(unique)res->roots[res->count++]=z[i];
    }
    if(res->count<n)snprintf(msg,mlen,"%d distinct roots (%d with multiplicity)%s",res->count,n,converged?"":" ~");
    else snprintf(msg,mlen,"%d roots%s",res->count,converged?"":" (approx)");
    return 1;
}
int cm_solve(const char *equation,CMResult *result,char *message,size_t message_size){
    char cleaned[CM_MAX_INPUT+1];size_t i,j=0;
    Parser parser;Poly p;
    if(!equation||!result||!message||message_size==0)return 0;
    memset(result,0,sizeof(*result));message[0]=0;
    for(i=0;equation[i];i++){
        unsigned char c=(unsigned char)equation[i];
        if(isspace(c))continue;
        if(j>=CM_MAX_INPUT){snprintf(message,message_size,"Equation too long");return 0;}
        cleaned[j++]=(char)tolower(c);
    }
    cleaned[j]=0;
    if(j==0){snprintf(message,message_size,"Enter an equation");return 0;}
    memset(&parser,0,sizeof(parser));parser.s=cleaned;
    p=parse_expr(&parser);
    if(eat(&parser,'='))p=p_add(p,parse_expr(&parser),-1);
    if(!parser.err[0]&&peek(&parser))fail(&parser,"Unexpected character");
    if(parser.err[0]){snprintf(message,message_size,"%s",parser.err);return 0;}
    trim(&p);
    for(i=0;i<=(size_t)p.deg;i++){
        if(!isfinite(p.a[i].re)||!isfinite(p.a[i].im)||c_abs(p.a[i])>1e9){
            snprintf(message,message_size,"Coefficient out of range");return 0;
        }
    }
    return solve_poly(&p,result,message,message_size);
}
