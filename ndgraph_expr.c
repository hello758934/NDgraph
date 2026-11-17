/* NDgraph expression engine -- no dynamic allocations, portable C99.
 * Complex math evaluates expressions; real and complex graph types share syntax.
 */
#include "ndgraph_expr.h"
#include "complex_math.h"
#include <ctype.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define ND_PI 3.1415926535897932384626433832795
#define ND_E 2.7182818284590452353602874713527
#define VAR_X 1u
#define VAR_Y 2u
#define VAR_Z 4u
#define VAR_T 8u
#define NODE_NUM 1
#define NODE_VAR 2
#define NODE_UN 3
#define NODE_BIN 4
#define NODE_FUNC 5

/* Function numbers are intentionally stable within the binary. */
enum {FN_SIN=1,FN_COS,FN_TAN,FN_SINH,FN_COSH,FN_TANH,FN_ABS,
      FN_LOG,FN_EXP,FN_SQRT,FN_RE,FN_IM,FN_CONJ,FN_ARG,
      FN_ASIN,FN_ACOS,FN_ATAN,FN_FLOOR,FN_CEIL};

typedef struct {const char *s; size_t pos; NDExpr *e; char *err; size_t errlen;} Parser;
static NDComplex nv(double r,double i){NDComplex z;z.re=r;z.im=i;return z;}
static NDComplex add(NDComplex a,NDComplex b){return nv(a.re+b.re,a.im+b.im);}
static NDComplex sub(NDComplex a,NDComplex b){return nv(a.re-b.re,a.im-b.im);}
static NDComplex mul(NDComplex a,NDComplex b){return nv(a.re*b.re-a.im*b.im,a.re*b.im+a.im*b.re);}
static double mag(NDComplex a){return hypot(a.re,a.im);}
static NDComplex divide(NDComplex a,NDComplex b){
    double m=fmax(fabs(b.re),fabs(b.im)),x,y,d;
    if(m<1e-290)return nv(NAN,NAN);
    x=b.re/m;y=b.im/m;d=m*(x*x+y*y);
    return nv((a.re*x+a.im*y)/d,(a.im*x-a.re*y)/d);
}
static NDComplex elog(NDComplex a){return mag(a)>0?nv(log(mag(a)),atan2(a.im==0.0?0.0:a.im,a.re)):nv(NAN,NAN);}
static NDComplex eexp(NDComplex a){double v=exp(a.re);return nv(v*cos(a.im),v*sin(a.im));}
static NDComplex epow(NDComplex a,NDComplex b){
    if(mag(a)==0){if(b.im==0&&b.re>0)return nv(0,0);if(b.im==0&&b.re==0)return nv(1,0);return nv(NAN,NAN);}
    if(b.im==0&&b.re==floor(b.re)&&fabs(b.re)<=32){
        int n=(int)fabs(b.re),i;NDComplex r=nv(1,0);
        for(i=0;i<n;i++)r=mul(r,a);
        return b.re<0?divide(nv(1,0),r):r;
    }
    return eexp(mul(b,elog(a)));
}
static NDComplex esin(NDComplex a){return nv(sin(a.re)*cosh(a.im),cos(a.re)*sinh(a.im));}
static NDComplex ecos(NDComplex a){return nv(cos(a.re)*cosh(a.im),-sin(a.re)*sinh(a.im));}
static int isfinite_c(NDComplex z){return isfinite(z.re)&&isfinite(z.im);}
static int degrees_mode=0;
void nd_set_angle_degrees(int degrees){degrees_mode=degrees!=0;}
static NDComplex apply_func(int fn, NDComplex a){
    if(degrees_mode&&fabs(a.im)<1e-12&&(fn==FN_SIN||fn==FN_COS||fn==FN_TAN))
        a.re*=ND_PI/180.0;
    switch(fn){
        case FN_SIN:return esin(a);
        case FN_COS:return ecos(a);
        case FN_TAN:return divide(esin(a),ecos(a));
        case FN_SINH:return nv(sinh(a.re)*cos(a.im),cosh(a.re)*sin(a.im));
        case FN_COSH:return nv(cosh(a.re)*cos(a.im),sinh(a.re)*sin(a.im));
        case FN_TANH:{NDComplex e2=eexp(mul(nv(2,0),a));return divide(sub(e2,nv(1,0)),add(e2,nv(1,0)));}
        case FN_ABS:return nv(mag(a),0);
        case FN_LOG:return elog(a);
        case FN_EXP:return eexp(a);
        case FN_SQRT:return epow(a,nv(0.5,0));
        case FN_RE:return nv(a.re,0);
        case FN_IM:return nv(a.im,0);
        case FN_CONJ:return nv(a.re,-a.im);
        case FN_ARG:return nv(atan2(a.im,a.re),0);
        case FN_ASIN:return fabs(a.im)<1e-12&&fabs(a.re)<=1?nv(asin(a.re)*(degrees_mode?180.0/ND_PI:1),0):nv(NAN,NAN);
        case FN_ACOS:return fabs(a.im)<1e-12&&fabs(a.re)<=1?nv(acos(a.re)*(degrees_mode?180.0/ND_PI:1),0):nv(NAN,NAN);
        case FN_ATAN:return fabs(a.im)<1e-12?nv(atan(a.re)*(degrees_mode?180.0/ND_PI:1),0):nv(NAN,NAN);
        case FN_FLOOR:return fabs(a.im)<1e-12?nv(floor(a.re),0):nv(NAN,NAN);
        case FN_CEIL:return fabs(a.im)<1e-12?nv(ceil(a.re),0):nv(NAN,NAN);
    }
    return nv(NAN,NAN);
}
static void seterr(Parser *p,const char *msg){
    if(p->err[0]==0)snprintf(p->err,p->errlen,"%s at %u",msg,(unsigned)(p->pos+1));
}
static char chr(Parser *p){return p->s[p->pos];}
static int take(Parser *p,char c){if(chr(p)==c){p->pos++;return 1;}return 0;}
static void ws(Parser *p){while(isspace((unsigned char)chr(p)))p->pos++;}
static int node(Parser *p,int type,int op,int a,int b,NDComplex val){
    int i=p->e->count;
    if(i>=ND_NODE_MAX){seterr(p,"Expression too large");return 0;}
    p->e->nodes[i].type=type;p->e->nodes[i].op=op;
    p->e->nodes[i].a=a;p->e->nodes[i].b=b;p->e->nodes[i].value=val;
    p->e->count++;
    return i;
}
static int expr(Parser *p);
static int atom(Parser *p){
    char name[32];int j=0,func=0,sub;char *end;double d;
    ws(p);
    if(take(p,'(')){
        sub=expr(p);ws(p);if(!take(p,')'))seterr(p,"Expected )");return sub;
    }
    if(isdigit((unsigned char)chr(p))||chr(p)=='.'){
        d=strtod(p->s+p->pos,&end);
        if(end==p->s+p->pos){seterr(p,"Expected number");return 0;}
        p->pos=(size_t)(end-p->s);
        if(!isfinite(d)){seterr(p,"Number out of range");return 0;}
        return node(p,NODE_NUM,0,0,0,nv(d,0));
    }
    if(isalpha((unsigned char)chr(p))||chr(p)=='_'){
        while((isalpha((unsigned char)chr(p))||chr(p)=='_')&&j<31)name[j++]=(char)tolower((unsigned char)p->s[p->pos++]);
        name[j]=0;
        if(strcmp(name,"x")==0){p->e->vars|=VAR_X;return node(p,NODE_VAR,'x',0,0,nv(0,0));}
        if(strcmp(name,"y")==0){p->e->vars|=VAR_Y;return node(p,NODE_VAR,'y',0,0,nv(0,0));}
        if(strcmp(name,"z")==0){p->e->vars|=VAR_Z;return node(p,NODE_VAR,'z',0,0,nv(0,0));}
        if(strcmp(name,"t")==0){p->e->vars|=VAR_T;return node(p,NODE_VAR,'t',0,0,nv(0,0));}
        if(strcmp(name,"i")==0)return node(p,NODE_NUM,0,0,0,nv(0,1));
        if(strcmp(name,"pi")==0)return node(p,NODE_NUM,0,0,0,nv(ND_PI,0));
        if(strcmp(name,"e")==0)return node(p,NODE_NUM,0,0,0,nv(ND_E,0));
        if(strcmp(name,"sin")==0)func=FN_SIN;
        else if(strcmp(name,"cos")==0)func=FN_COS;
        else if(strcmp(name,"tan")==0)func=FN_TAN;
        else if(strcmp(name,"sinh")==0)func=FN_SINH;
        else if(strcmp(name,"cosh")==0)func=FN_COSH;
        else if(strcmp(name,"tanh")==0)func=FN_TANH;
        else if(strcmp(name,"abs")==0)func=FN_ABS;
        else if(strcmp(name,"log")==0||strcmp(name,"ln")==0)func=FN_LOG;
        else if(strcmp(name,"exp")==0)func=FN_EXP;
        else if(strcmp(name,"sqrt")==0)func=FN_SQRT;
        else if(strcmp(name,"re")==0)func=FN_RE;
        else if(strcmp(name,"im")==0)func=FN_IM;
        else if(strcmp(name,"conj")==0)func=FN_CONJ;
        else if(strcmp(name,"arg")==0)func=FN_ARG;
        else if(strcmp(name,"asin")==0)func=FN_ASIN;
        else if(strcmp(name,"acos")==0)func=FN_ACOS;
        else if(strcmp(name,"atan")==0)func=FN_ATAN;
        else if(strcmp(name,"floor")==0)func=FN_FLOOR;
        else if(strcmp(name,"ceil")==0)func=FN_CEIL;
        if(!func){seterr(p,"Unknown name");return 0;}
        ws(p);
        if(!take(p,'(')){seterr(p,"Function needs (");return 0;}
        sub=expr(p);ws(p);
        if(!take(p,')'))seterr(p,"Expected )");
        return node(p,NODE_FUNC,func,sub,0,nv(0,0));
    }
    seterr(p,"Expected number or name");return 0;
}
static int unary(Parser *p);
/* Right associative exponentiation; -x^2 correctly means -(x^2). */
static int power(Parser *p){
    int a=atom(p);ws(p);
    if(take(p,'^')){
        int b=unary(p);
        a=node(p,NODE_BIN,'^',a,b,nv(0,0));
    }
    return a;
}
static int unary(Parser *p){
    int sub;ws(p);
    if(take(p,'+'))return unary(p);
    if(take(p,'-')){sub=unary(p);return node(p,NODE_UN,'-',sub,0,nv(0,0));}
    return power(p);
}
static int term(Parser *p){
    int a=unary(p);ws(p);
    while(!p->err[0]){
        int op=0,b;char c=chr(p);
        if(c=='*'||c=='/'){op=c;p->pos++;}
        else if(c=='('||isdigit((unsigned char)c)||isalpha((unsigned char)c))op='*'; /* implicit multiplication */
        if(!op)break;
        b=unary(p);a=node(p,NODE_BIN,op,a,b,nv(0,0));ws(p);
    }
    return a;
}
static int expr(Parser *p){
    int a=term(p);ws(p);
    while(!p->err[0]&&(chr(p)=='+'||chr(p)=='-')){
        int op=chr(p);int b;p->pos++;b=term(p);
        a=node(p,NODE_BIN,op,a,b,nv(0,0));ws(p);
    }
    return a;
}
int nd_parse_expr(const char *src,NDExpr *out,char *err,size_t len){
    Parser p;memset(out,0,sizeof(*out));
    if(!src||!err||len==0)return 0;
    err[0]=0;
    memset(&p,0,sizeof(p));p.s=src;p.e=out;p.err=err;p.errlen=len;
    ws(&p);out->root=expr(&p);ws(&p);
    if(!err[0]&&chr(&p)!=0)seterr(&p,"Unexpected character");
    return err[0]==0;
}
NDComplex nd_eval(const NDExpr *e,const NDVars *v,int *ok){
    NDComplex vals[ND_NODE_MAX];int i;NDComplex r=nv(NAN,NAN);
    *ok=1;
    for(i=0;i<e->count;i++){
        NDNode n=e->nodes[i];NDComplex a,b;
        switch(n.type){
            case NODE_NUM:r=n.value;break;
            case NODE_VAR:
                if(n.op=='x')r=nv(v->x,0);
                else if(n.op=='y')r=nv(v->y,0);
                else if(n.op=='t')r=nv(v->t,0);
                else r=v->z;
                break;
            case NODE_UN:a=vals[n.a];r=nv(-a.re,-a.im);break;
            case NODE_BIN:
                a=vals[n.a];b=vals[n.b];
                if(n.op=='+')r=add(a,b);
                else if(n.op=='-')r=sub(a,b);
                else if(n.op=='*')r=mul(a,b);
                else if(n.op=='/')r=divide(a,b);
                else r=epow(a,b);
                break;
            case NODE_FUNC:r=apply_func(n.op,vals[n.a]);break;
            default:r=nv(NAN,NAN);break;
        }
        if(!isfinite_c(r)||mag(r)>1e14){*ok=0;return nv(NAN,NAN);}
        vals[i]=r;
    }
    return e->count?vals[e->root]:nv(NAN,NAN);
}
static void compact(const char *src,char *dst,size_t max){
    size_t j=0;while(*src&&j+1<max){unsigned char c=(unsigned char)*src++;
        if(!isspace(c))dst[j++]=(char)tolower(c);
    }dst[j]=0;
}
static int eq_count(const char *s){int count=0;while(*s)if(*s++=='=')count++;return count;}
static int parse_assignment(const char *line,char *lhs,size_t lcap,char *rhs,size_t rcap){
    const char *sep=strchr(line,'=');size_t n;
    if(!sep||strchr(sep+1,'='))return 0;
    n=(size_t)(sep-line);if(n>=lcap||strlen(sep+1)>=rcap)return 0;
    memcpy(lhs,line,n);lhs[n]=0;strcpy(rhs,sep+1);return 1;
}
static int compile_implicit(const char *l,const char *r,NDGraph *g,char *err,size_t cap){
    if(!nd_parse_expr(l,&g->lhs,err,cap))return 0;
    if(!nd_parse_expr(r,&g->rhs,err,cap))return 0;
    return 1;
}
int nd_compile(const char *source,int imaginary,NDGraph *out,char *err,size_t cap){
    char cleaned[ND_INPUT_MAX+1],lhs[ND_INPUT_MAX+1],rhs[ND_INPUT_MAX+1],*semi;
    static NDGraph g;unsigned vars;int has_equal;
    if(!source||!out||!err||!cap)return 0;
    memset(&g,0,sizeof(g));err[0]=0;compact(source,cleaned,sizeof(cleaned));
    if(strlen(source)>ND_INPUT_MAX){snprintf(err,cap,"Input too long");return 0;}
    if(!cleaned[0]){snprintf(err,cap,"Type an equation first");return 0;}
    strcpy(g.raw,cleaned);semi=strchr(cleaned,';');has_equal=eq_count(cleaned);
    if(imaginary){
        if(semi){snprintf(err,cap,"Imag mode: use z=...t...");return 0;}
        if(!has_equal){snprintf(err,cap,"Use z=..., re(z)=... or abs(z)=...");return 0;}
        if(!parse_assignment(cleaned,lhs,sizeof(lhs),rhs,sizeof(rhs))){snprintf(err,cap,"Use one = sign");return 0;}
        {
            CMResult res;char msg[128];
            /* Existing polynomial solver has priority, including z=z^2. */
            if(cm_solve(cleaned,&res,msg,sizeof(msg))){
                g.kind=ND_IMAG_ROOTS;g.function_light=0;
            }else if(strcmp(lhs,"z")==0){
                NDExpr candidate;
                if(!nd_parse_expr(rhs,&candidate,err,cap))return 0;
                if(candidate.vars&VAR_T){
                    if(candidate.vars&(VAR_X|VAR_Y|VAR_Z)){
                        snprintf(err,cap,"z(t) can only use t and constants");return 0;
                    }
                    g.rhs=candidate;g.kind=ND_IMAG_PARAM;g.function_light=1;
                }else{
                    if(candidate.vars&(VAR_X|VAR_Y|VAR_Z)){
                        snprintf(err,cap,"Right side of z= uses t or constants");return 0;
                    }
                    g.rhs=candidate;g.kind=ND_IMAG_POINT;g.function_light=0;
                }
            }else{
                if(!compile_implicit(lhs,rhs,&g,err,cap))return 0;
                vars=g.lhs.vars|g.rhs.vars;
                if(vars&(VAR_T|VAR_X|VAR_Y)) {
                    snprintf(err,cap,"Imag locus uses z, re(z), im(z)");return 0;
                }
                /* A complex equality is two constraints; only take a contour
                   when its imaginary component vanishes at generic samples. */
                {
                    NDVars v;int i,ok1,ok2;NDComplex a,b;
                    const double samples[][2]={{0.22,0.71},{1.13,-0.37},{-0.41,1.31}};
                    memset(&v,0,sizeof(v));
                    for(i=0;i<3;i++){
                        v.z=nv(samples[i][0],samples[i][1]);
                        a=nd_eval(&g.lhs,&v,&ok1);b=nd_eval(&g.rhs,&v,&ok2);
                        if(!ok1||!ok2||fabs(a.im-b.im)>1e-7){
                            snprintf(err,cap,"Complex equation needs polynomial roots");return 0;
                        }
                    }
                }
                g.kind=ND_IMAG_LOCUS;g.function_light=0;
            }
        }
    }else{
        if(semi){
            char first[ND_INPUT_MAX+1],lhs1[ND_INPUT_MAX+1],rhs1[ND_INPUT_MAX+1];
            char lhs2[ND_INPUT_MAX+1],rhs2[ND_INPUT_MAX+1];
            NDExpr x,y;unsigned used;
            if(strchr(semi+1,';')){snprintf(err,cap,"Only two parametric equations");return 0;}
            memcpy(first,cleaned,(size_t)(semi-cleaned));first[semi-cleaned]=0;
            if(!parse_assignment(first,lhs1,sizeof(lhs1),rhs1,sizeof(rhs1))||
               !parse_assignment(semi+1,lhs2,sizeof(lhs2),rhs2,sizeof(rhs2))){snprintf(err,cap,"Use x=...;y=...");return 0;}
            if((strcmp(lhs1,"x")==0||strcmp(lhs1,"x(t)")==0)&&
               (strcmp(lhs2,"y")==0||strcmp(lhs2,"y(t)")==0)){
                if(!nd_parse_expr(rhs1,&x,err,cap)||!nd_parse_expr(rhs2,&y,err,cap))return 0;
            }else if((strcmp(lhs2,"x")==0||strcmp(lhs2,"x(t)")==0)&&
                     (strcmp(lhs1,"y")==0||strcmp(lhs1,"y(t)")==0)){
                if(!nd_parse_expr(rhs2,&x,err,cap)||!nd_parse_expr(rhs1,&y,err,cap))return 0;
            }else {snprintf(err,cap,"Parametric needs x=...;y=...");return 0;}
            used=x.vars|y.vars;
            if(used&(VAR_X|VAR_Y|VAR_Z)){snprintf(err,cap,"Parametric expressions use t only");return 0;}
            g.xexpr=x;g.yexpr=y;g.kind=ND_REAL_PARAM;g.function_light=0;
        }else{
            if(has_equal){
                if(!parse_assignment(cleaned,lhs,sizeof(lhs),rhs,sizeof(rhs))){snprintf(err,cap,"Use one = sign");return 0;}
            }else{strcpy(lhs,"y");strcpy(rhs,cleaned);}
            if(strcmp(lhs,"f(x)")==0)strcpy(lhs,"y");
            if(!compile_implicit(lhs,rhs,&g,err,cap))return 0;
            vars=g.rhs.vars;
            if(strcmp(lhs,"y")==0&&!(vars&(VAR_Y|VAR_Z))){
                g.kind=ND_REAL_EXPLICIT;g.function_light=1;
            }else{
                if((g.lhs.vars|vars)&VAR_Z){snprintf(err,cap,"Real mode can't use z");return 0;}
                g.kind=ND_REAL_IMPLICIT;g.function_light=0; /* verified numerically after graphing */
            }
        }
    }
    *out=g;return 1;
}
static double implicit_value(const NDGraph *g,double x,double y,int *ok){
    NDVars v;NDComplex a,b;int oka,okb;
    memset(&v,0,sizeof(v));v.x=x;v.y=y;v.t=x;v.z=nv(x,y);
    a=nd_eval(&g->lhs,&v,&oka);b=nd_eval(&g->rhs,&v,&okb);
    *ok=oka&&okb&&fabs(a.im)<1e-7&&fabs(b.im)<1e-7;
    return *ok?a.re-b.re:NAN;
}
/* Conservative numerical vertical-line scan. Green only if sample scan sees
   exactly one crossing on multiple x columns, and never sees two crossings.
   This is NOT symbolic proof; on-screen indicator is intentionally provisional. */
int nd_real_implicit_is_function(const NDGraph *g,double xmin,double xmax,double ymin,double ymax){
    int xi,yi,columns_with_one=0;
    if(g->kind!=ND_REAL_IMPLICIT&&g->kind!=ND_IMAG_LOCUS)return 0;
    for(xi=0;xi<21;xi++){
        double x=xmin+(xmax-xmin)*(xi+0.37)/21.0;
        double prev=0;int prev_ok=0,crossings=0;
        for(yi=0;yi<=140;yi++){
            double y=ymin+(ymax-ymin)*yi/140.0;
            int ok;double val=implicit_value(g,x,y,&ok);
            if(ok&&prev_ok){
                if((val>0&&prev<0)||(val<0&&prev>0))crossings++;
                else if(fabs(val)<1e-9&&fabs(prev)>1e-9)crossings++;
            }
            if(crossings>1)return 0;
            prev=val;prev_ok=ok;
        }
        if(crossings==1)columns_with_one++;
    }
    return columns_with_one>=4;
}
