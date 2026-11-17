#ifndef NDGRAPH_EXPR_H
#define NDGRAPH_EXPR_H
#include <stddef.h>

#define ND_INPUT_MAX 180
#define ND_NODE_MAX 140

typedef struct {double re,im;} NDComplex;
typedef struct {int type, op, a, b; NDComplex value;} NDNode;
typedef struct {
  NDNode nodes[ND_NODE_MAX];
  int count, root;
  unsigned vars; /* bit 0 x, 1 y, 2 z, 3 t */
} NDExpr;
typedef struct {double x,y,t; NDComplex z;} NDVars;
enum {ND_REAL_EXPLICIT=1, ND_REAL_IMPLICIT, ND_REAL_PARAM,
      ND_IMAG_ROOTS, ND_IMAG_PARAM, ND_IMAG_LOCUS, ND_IMAG_POINT};
typedef struct {
  int kind, function_light;
  NDExpr lhs, rhs, xexpr, yexpr;
  char raw[ND_INPUT_MAX+1];
} NDGraph;

void nd_set_angle_degrees(int degrees); /* real trigonometry only; complex values use radians */
int nd_parse_expr(const char *source, NDExpr *out, char *err, size_t err_size);
NDComplex nd_eval(const NDExpr *e, const NDVars *v, int *ok);
int nd_compile(const char *raw, int imaginary_mode, NDGraph *out, char *err, size_t err_size);
int nd_real_implicit_is_function(const NDGraph *g, double xmin,double xmax,double ymin,double ymax);

#endif
