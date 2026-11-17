#include "ndgraph_features.h"
#include <math.h>
#include <stdio.h>
#include <string.h>
static int n=0,fail=0;
static void ck(int v,const char *s){printf("%s %s\n",v?"PASS":"FAIL",s);n++;if(!v)fail++;}
static int c(const char *src,int mode,NDGraph *g){char err[128];int ok=nd_compile(src,mode,g,err,sizeof(err));if(!ok)printf("compile %s: %s\n",src,err);return ok;}
int main(void){
    NDHistory h={0};NDGraph a,b,g;NDPoint p[ND_INTERSECTIONS];int k;double x,y;
    ck(nd_history_get(&h,0)==0,"empty history");
    nd_history_push(&h,"y=x");nd_history_push(&h,"y=x^2");nd_history_push(&h,"y=x");
    ck(h.count==2&&!strcmp(nd_history_get(&h,0),"y=x")&&!strcmp(nd_history_get(&h,1),"y=x^2"),"history dedup move front");
    for(k=0;k<20;k++){char s[20];sprintf(s,"y=x+%d",k);nd_history_push(&h,s);}
    ck(h.count==ND_HISTORY&&!strcmp(nd_history_get(&h,0),"y=x+19")&&!strcmp(nd_history_get(&h,11),"y=x+8"),"history rolling limit");
    ck(c("y=x",0,&a)&&c("y=-x",0,&b),"compile line pair");
    k=nd_find_intersections(&a,&b,-4,4,p,ND_INTERSECTIONS);
    ck(k==1&&fabs(p[0].x)<1e-5&&fabs(p[0].y)<1e-5,"line intersection origin");
    ck(c("y=x^2",0,&a)&&c("y=1",0,&b),"compile parabola pair");
    k=nd_find_intersections(&a,&b,-3,3,p,ND_INTERSECTIONS);
    ck(k==2&&fabs(p[0].x+1)<1e-5&&fabs(p[1].x-1)<1e-5,"two parabola intersections");
    ck(c("y=x^2+2",0,&a)&&c("y=1",0,&b),"compile nonintersecting pair");
    ck(nd_find_intersections(&a,&b,-5,5,p,ND_INTERSECTIONS)==0,"no false intersections");
    ck(c("z=t*(1+i)",1,&g)&&nd_curve_sample(&g,2,&x,&y)&&fabs(x-2)<1e-6&&fabs(y-2)<1e-6,"complex trace sample");
    ck(c("x=cos(t);y=sin(t)",0,&g)&&nd_curve_sample(&g,0,&x,&y)&&fabs(x-1)<1e-6&&fabs(y)<1e-6,"real param trace sample");
    ck(c("z^4=1",1,&g)&&!nd_curve_sample(&g,0,&x,&y),"root set not continuous curve");
    ck(c("x=2",0,&g)&&!nd_curve_sample(&g,0,&x,&y),"implicit not continuous param trace");
    printf("%d checks, %d failures\n",n,fail);return fail?1:0;
}
