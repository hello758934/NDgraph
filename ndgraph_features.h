#ifndef NDGRAPH_FEATURES_H
#define NDGRAPH_FEATURES_H
#include "ndgraph_expr.h"
#define ND_SLOTS 6
#define ND_HISTORY 12
#define ND_INTERSECTIONS 16

typedef struct {char lines[ND_HISTORY][ND_INPUT_MAX+1];int count;} NDHistory;
typedef struct {double x,y;} NDPoint;
void nd_history_push(NDHistory *hist,const char *input);
const char *nd_history_get(const NDHistory *hist,int index);
int nd_curve_sample(const NDGraph *g,double param,double *x,double *y);
int nd_find_intersections(const NDGraph *a,const NDGraph *b,double left,double right,NDPoint *out,int cap);
#endif
