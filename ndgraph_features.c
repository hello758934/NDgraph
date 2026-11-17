#include "ndgraph_features.h"
#include <math.h>
#include <string.h>

void nd_history_push(NDHistory *hist,const char *input){
    int i,prev=-1;
    if(!input||!*input)return;
    for(i=0;i<hist->count;i++)if(strcmp(hist->lines[i],input)==0){prev=i;break;}
    if(prev>=0){for(i=prev;i>0;i--)strcpy(hist->lines[i],hist->lines[i-1]);}
    else{
        int end=hist->count<ND_HISTORY?hist->count++:ND_HISTORY-1;
        for(i=end;i>0;i--)strcpy(hist->lines[i],hist->lines[i-1]);
    }
    strncpy(hist->lines[0],input,ND_INPUT_MAX);
    hist->lines[0][ND_INPUT_MAX]=0;
}
const char *nd_history_get(const NDHistory *hist,int index){
    if(index<0||index>=hist->count)return 0;
    return hist->lines[index];
}

static int real(const NDExpr *expr,NDVars *vars,double *result){
    int ok=0;NDComplex z=nd_eval(expr,vars,&ok);
    if(!ok||fabs(z.im)>1e-7||!isfinite(z.re)||fabs(z.re)>1e9)return 0;
    *result=z.re;return 1;
}
int nd_curve_sample(const NDGraph *g,double param,double *x,double *y){
    NDVars v;int ok=0;NDComplex z;
    memset(&v,0,sizeof(v));v.t=param;
    if(g->kind==ND_REAL_EXPLICIT){
        v.x=param;v.t=param;
        *x=param;return real(&g->rhs,&v,y);
    }
    if(g->kind==ND_REAL_PARAM){
        return real(&g->xexpr,&v,x)&&real(&g->yexpr,&v,y);
    }
    if(g->kind==ND_IMAG_PARAM){
        z=nd_eval(&g->rhs,&v,&ok);
        if(!ok||!isfinite(z.re)||!isfinite(z.im)||fabs(z.re)>1e9||fabs(z.im)>1e9)return 0;
        *x=z.re;*y=z.im;return 1;
    }
    return 0;
}
static int difference(const NDGraph *a,const NDGraph *b,double x,double *d,double *y){
    double ax,ay,bx,by;
    if(!nd_curve_sample(a,x,&ax,&ay)||!nd_curve_sample(b,x,&bx,&by))return 0;
    *d=ay-by;*y=(ay+by)*0.5;
    return isfinite(*d);
}
int nd_find_intersections(const NDGraph *a,const NDGraph *b,double left,double right,NDPoint *out,int cap){
    int i,n=0,oldok=0;double oldx=0,oldd=0,oldy=0;
    if(!a||!b||!out||cap<=0||!isfinite(left)||!isfinite(right)||left>=right)return 0;
    if(a->kind!=ND_REAL_EXPLICIT||b->kind!=ND_REAL_EXPLICIT)return 0;
    for(i=0;i<=400;i++){
        double x=left+(right-left)*i/400.0,d=0,y=0;
        int ok=difference(a,b,x,&d,&y);
        if(ok&&oldok&&((d>0&&oldd<0)||(d<0&&oldd>0)||d==0||oldd==0)){
            double lo=oldx,hi=x,ld=oldd,m,yval=y;
            int k,valid=1;
            for(k=0;k<35;k++){
                double mid=(lo+hi)*0.5,md;
                if(!difference(a,b,mid,&md,&yval)){valid=0;break;}
                if((ld<=0&&md<=0)||(ld>=0&&md>=0)){lo=mid;ld=md;}
                else hi=mid;
            }
            m=(lo+hi)*0.5;
            if(valid&&difference(a,b,m,&d,&yval)&&fabs(d)<1e-5*(1+fabs(yval))&&
               (n==0||fabs(m-out[n-1].x)>(right-left)/2000.0)){
                out[n].x=m;out[n].y=yval;
                if(++n>=cap)break;
            }
        }
        oldok=ok;oldx=x;oldd=d;oldy=y;
    }
    (void)oldy;
    return n;
}
