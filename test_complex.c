#include "complex_math.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int contains(const CMResult *r,double x,double y,double tol){
    int j;for(j=0;j<r->count;j++)if(hypot(r->roots[j].re-x,r->roots[j].im-y)<tol)return 1;
    return 0;
}
static int test(const char *expr,int expected,const double pts[][2],int npts){
    CMResult r;char msg[128];int i,ok=cm_solve(expr,&r,msg,sizeof(msg));
    printf("%-27s -> %s",expr,msg);
    if(!ok||r.count!=expected){printf(" [FAIL count=%d]\n",r.count);return 0;}
    for(i=0;i<npts;i++)if(!contains(&r,pts[i][0],pts[i][1],0.005)){
        printf(" [FAIL missing (%g,%g)]\n",pts[i][0],pts[i][1]);return 0;
    }
    printf(" [PASS]\n");return 1;
}
int main(void){
    int pass=1;CMResult r;char msg[128];
    const double four[][2]={{1,0},{0,1},{-1,0},{0,-1}};
    const double two[][2]={{0,1},{0,-1}};
    const double three[][2]={{1,0},{-0.5,0.86602540378},{-0.5,-0.86602540378}};
    const double shifted[][2]={{1,0},{0,1}};
    const double repeated[][2]={{-1,0}};
    const double half_four[][2]={{0.840896415,0},{-0.840896415,0},{0,0.840896415},{0,-0.840896415}};
    const double origin[][2]={{0,0}};
    pass &=test("z^4=1",4,four,4);
    pass &=test("z^2+1=0",2,two,2);
    pass &=test("z^3=1",3,three,3);
    pass &=test("(z-1)*(z-i)=0",2,shifted,2);
    pass &=test("z^2+2z+1=0",1,repeated,1);
    pass &=test("z^2=-1",2,two,2);
    pass &=test("2z^2+2=0",2,two,2);
    pass &=test("z^4-1=0",4,four,4);
    pass &=test("z^4=1/2",4,half_four,4);
    pass &=test("z=0",1,origin,1);
    pass &=test("z^0=1",0,NULL,0);
    pass &=test("z^10=1",10,NULL,0);
    pass &=test("z^5=1",5,NULL,0);
    pass &=test("(z-i)^3=0",1,(const double[][2]){{0,1}},1);
    pass &=test("(z+1)(z-1)=0",2,(const double[][2]){{1,0},{-1,0}},2);
    if(cm_solve("z^11=1",&r,msg,sizeof(msg))){puts("FAIL invalid degree accepted");pass=0;}
    if(cm_solve("z/(z+1)=0",&r,msg,sizeof(msg))){puts("FAIL variable division accepted");pass=0;}
    if(cm_solve("z^2=banana",&r,msg,sizeof(msg))){puts("FAIL invalid name accepted");pass=0;}
    return pass?0:1;
}
