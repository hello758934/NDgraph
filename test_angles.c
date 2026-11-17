#include "ndgraph_expr.h"
#include <stdio.h>
#include <math.h>
#include <string.h>
static int result(const char *e,double expect){NDExpr g;NDVars v;NDComplex z;char err[100];int ok;
  memset(&v,0,sizeof(v));if(!nd_parse_expr(e,&g,err,sizeof(err)))return 0;
  z=nd_eval(&g,&v,&ok);return ok&&fabs(z.re-expect)<1e-6&&fabs(z.im)<1e-6;
}
int main(void){int n=0;
  nd_set_angle_degrees(0);n+=result("sin(pi/2)",1);n+=result("asin(1)",1.5707963267948966);
  nd_set_angle_degrees(1);n+=result("sin(90)",1);n+=result("cos(180)",-1);n+=result("tan(45)",1);n+=result("asin(1)",90);
  nd_set_angle_degrees(0);n+=result("sin(90)",sin(90));
  printf("Angle tests: %d/7 passed\n",n);return n==7?0:1;
}
