/* NDgraph v0.31 -- native Ndless/nSDL graph explorer for TI-Nspire CX II CAS.
 * Six slots per plane; full-screen Catalog functions tab; real and complex plots.
 * Never modifies calculator files or settings.
 */
#include <os.h>
#include <libndls.h>
#include <SDL/SDL.h>
#include <ctype.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ndgraph_expr.h"
#include "ndgraph_features.h"
#include "complex_math.h"

#define W 320
#define H 240
#define TOP 26
#define BOT 203
#define PH (BOT-TOP+1)
#define PI 3.14159265358979323846
#define V_GRAPH 0
#define V_FUNCTIONS 1
#define V_HISTORY 2
#define V_SETTINGS 3
#define V_HELP 4

typedef struct {
    NDGraph graph;
    CMResult roots;
    char text[ND_INPUT_MAX+1];
    int used,visible,light,color;
} Slot;
typedef struct {double x,y,scale;} Camera;

static SDL_Surface *screen;
static nSDL_Font *ft_white,*ft_dark,*ft_muted;
static Slot slots[2][ND_SLOTS];
static NDHistory history[2];
static Camera camera[2]={{0,0,4.5},{0,0,4.5}};
static int selected[2]={0,0};
static int mode=1,view=V_GRAPH,previous_view=V_GRAPH,editing=0,trace_on=0,trace_root=0;
static int hist_row=0,setting_row=0,edit_error=0;
static int grid_on=1,axes_on=1,degree_on=0,join_roots=0,show_intersections=0;
static double trace_parameter=0,t_range=2*PI;
static char edittext[ND_INPUT_MAX+1];
static char message[130]="Ready. Open Catalog to enter an equation.";
static const int colors[6][3]={{0,146,137},{242,133,44},{111,82,214},{37,116,216},{207,62,98},{65,145,54}};

static Uint16 rgb(int r,int g,int b){return (Uint16)SDL_MapRGB(screen->format,(Uint8)r,(Uint8)g,(Uint8)b);}
static Uint16 ink(int index){const int *p=colors[index%6];return rgb(p[0],p[1],p[2]);}
static void rect(int x,int y,int w,int h,Uint16 color){
    SDL_Rect r;if(w<1||h<1)return;
    r.x=(Sint16)x;r.y=(Sint16)y;r.w=(Uint16)w;r.h=(Uint16)h;
    SDL_FillRect(screen,&r,color);
}
static void str(int x,int y,nSDL_Font *font,const char *s){
    /* Tinytype is approximately 6px wide; clip for the 320px display. */
    char shown[64];size_t n=strlen(s),cap=x<W?(size_t)((W-x)/6):0;
    if(cap>63)cap=63;
    if(n>cap)n=cap;
    memcpy(shown,s,n);shown[n]=0;
    nSDL_DrawString(screen,font,x,y,"%s",shown);
}
static void px(int x,int y,Uint16 color){
    if(x>=0&&x<W&&y>=TOP&&y<=BOT)
        *((Uint16*)((Uint8*)screen->pixels+y*screen->pitch)+x)=color;
}
static int clip(double p,double q,double *u0,double *u1){
    double a;if(fabs(p)<1e-14)return q>=0;
    a=q/p;if(p<0){if(a>*u1)return 0;if(a>*u0)*u0=a;}
    else{if(a<*u0)return 0;if(a<*u1)*u1=a;}return 1;
}
static void segment(double ax,double ay,double bx,double by,Uint16 color,int dash){
    double dx=bx-ax,dy=by-ay,u0=0,u1=1;int x0,y0,x1,y1,ex,ey,sx,sy,er,j=0;
    if(!isfinite(ax)||!isfinite(ay)||!isfinite(bx)||!isfinite(by))return;
    if(!clip(-dx,ax,&u0,&u1)||!clip(dx,W-1-ax,&u0,&u1)||
       !clip(-dy,ay-TOP,&u0,&u1)||!clip(dy,BOT-ay,&u0,&u1))return;
    x0=(int)floor(ax+u0*dx+0.5);y0=(int)floor(ay+u0*dy+0.5);
    x1=(int)floor(ax+u1*dx+0.5);y1=(int)floor(ay+u1*dy+0.5);
    ex=abs(x1-x0);ey=abs(y1-y0);sx=x0<x1?1:-1;sy=y0<y1?1:-1;er=ex-ey;
    for(;;){
        if(!dash||j%9<5)px(x0,y0,color);
        if(x0==x1&&y0==y1)break;
        {int e2=er*2;if(e2>-ey){er-=ey;x0+=sx;}if(e2<ex){er+=ex;y0+=sy;}}
        ++j;
    }
}
static double xpixel(double x){return W/2.0+(x-camera[mode].x)*(PH/2.0)/camera[mode].scale;}
static double ypixel(double y){return (TOP+BOT)/2.0-(y-camera[mode].y)*(PH/2.0)/camera[mode].scale;}
static double xworld(double x){return camera[mode].x+(x-W/2.0)*camera[mode].scale/(PH/2.0);}
static double yworld(double y){return camera[mode].y+((TOP+BOT)/2.0-y)*camera[mode].scale/(PH/2.0);}
static void circle_dot(double x,double y,Uint16 color,int radius){
    double a=xpixel(x),b=ypixel(y);int i,j,cx,cy;
    if(!isfinite(a)||!isfinite(b)||a<radius||a>=W-radius||b<TOP+radius||b>BOT-radius)return;
    cx=(int)floor(a+0.5);cy=(int)floor(b+0.5);
    for(j=-radius;j<=radius;j++)for(i=-radius;i<=radius;i++)
        if(i*i+j*j<=radius*radius)px(cx+i,cy+j,color);
}
static void grid(void){
    int k;double base=camera[mode].scale/2.5,power=pow(10.0,floor(log10(base))),step=power;
    Uint16 faint=rgb(225,234,243),axis=rgb(111,127,148);
    if(base/power>5)step*=10;else if(base/power>2)step*=5;else if(base/power>1)step*=2;
    if(grid_on){
        for(k=-70;k<=70;k++){
            double a=(floor(camera[mode].x/step)+k)*step;
            double b=(floor(camera[mode].y/step)+k)*step;
            if(xpixel(a)>=0&&xpixel(a)<W)segment(xpixel(a),TOP,xpixel(a),BOT,faint,0);
            if(ypixel(b)>=TOP&&ypixel(b)<=BOT)segment(0,ypixel(b),W-1,ypixel(b),faint,0);
        }
    }
    if(axes_on){segment(xpixel(0),TOP,xpixel(0),BOT,axis,0);
        segment(0,ypixel(0),W-1,ypixel(0),axis,0);}
}
static NDVars env(double x,double y){NDVars v;memset(&v,0,sizeof(v));v.x=x;v.y=y;v.t=x;v.z.re=x;v.z.im=y;return v;}
static int implicit_value(const Slot *slot,double x,double y,double *result){
    NDVars v=env(x,y);int oka=0,okb=0;
    NDComplex a=nd_eval(&slot->graph.lhs,&v,&oka);
    NDComplex b=nd_eval(&slot->graph.rhs,&v,&okb);
    if(!oka||!okb||fabs(a.im-b.im)>1e-6)return 0;
    *result=a.re-b.re;return isfinite(*result)&&fabs(*result)<1e11;
}
static void plot_implicit(const Slot *s,Uint16 color){
    enum {STEP=5,COLS=(W+STEP-1)/STEP,ROWS=(PH+STEP-1)/STEP};
    double prior[COLS+1],next[COLS+1];
    unsigned char a[COLS+1],b[COLS+1];int col,row,i;
    for(col=0;col<=COLS;col++)a[col]=(unsigned char)implicit_value(s,xworld(col*STEP),yworld(TOP),&prior[col]);
    for(row=1;row<=ROWS;row++){
        int y0=TOP+(row-1)*STEP,y1=TOP+row*STEP;if(y1>BOT)y1=BOT;
        for(col=0;col<=COLS;col++)b[col]=(unsigned char)implicit_value(s,xworld(col*STEP),yworld(y1),&next[col]);
        for(col=0;col<COLS;col++){
            double vals[4]={prior[col],prior[col+1],next[col+1],next[col]};
            double xy[4][2]={{col*STEP,y0},{(col+1)*STEP,y0},{(col+1)*STEP,y1},{col*STEP,y1}};
            double contacts[4][2];int found=0;
            if(!(a[col]&&a[col+1]&&b[col]&&b[col+1]))continue;
            for(i=0;i<4;i++){
                int k=(i+1)%4;
                if((vals[i]<0&&vals[k]>=0)||(vals[k]<0&&vals[i]>=0)){
                    double den=vals[i]-vals[k];double t=fabs(den)>1e-30?vals[i]/den:0.5;
                    if(t<0)t=0;
                    if(t>1)t=1;
                    contacts[found][0]=xy[i][0]+t*(xy[k][0]-xy[i][0]);
                    contacts[found][1]=xy[i][1]+t*(xy[k][1]-xy[i][1]);found++;
                }
            }
            if(found==2)segment(contacts[0][0],contacts[0][1],contacts[1][0],contacts[1][1],color,0);
            else if(found==4){
                segment(contacts[0][0],contacts[0][1],contacts[1][0],contacts[1][1],color,0);
                segment(contacts[2][0],contacts[2][1],contacts[3][0],contacts[3][1],color,0);
            }
        }
        memcpy(prior,next,sizeof(prior));memcpy(a,b,sizeof(a));
    }
}
static void plot_curve(const Slot *s,Uint16 color){
    int i,prev=0,n=400;double ox=0,oy=0,x,y,t;
    for(i=0;i<=n;i++){
        t=s->graph.kind==ND_REAL_EXPLICIT?xworld((double)i*W/n):-t_range+2*t_range*i/n;
        if(nd_curve_sample(&s->graph,t,&x,&y)){
            double pxv=xpixel(x),pyv=ypixel(y);
            if(prev&&fabs(pyv-oy)<PH*1.4&&fabs(pxv-ox)<W*1.4)
                segment(ox,oy,pxv,pyv,color,0);
            ox=pxv;oy=pyv;prev=1;
        }else prev=0;
    }
}
static void plot_roots(const Slot *s,Uint16 color){
    int n=s->roots.count,i,j,order[16];
    if(s->graph.kind==ND_IMAG_POINT){
        int ok=0;NDVars v=env(0,0);NDComplex z=nd_eval(&s->graph.rhs,&v,&ok);
        if(ok)circle_dot(z.re,z.im,color,3);
        return;
    }
    if(n>16)n=16;
    for(i=0;i<n;i++)order[i]=i;
    for(i=0;i<n;i++)for(j=i+1;j<n;j++){
        CMComplex a=s->roots.roots[order[i]],b=s->roots.roots[order[j]];
        if(atan2(a.im,a.re)>atan2(b.im,b.re)){int tmp=order[i];order[i]=order[j];order[j]=tmp;}
    }
    if(join_roots&&n>=3)for(i=0;i<n;i++){
        int a=order[i],b=order[(i+1)%n];
        segment(xpixel(s->roots.roots[a].re),ypixel(s->roots.roots[a].im),
                xpixel(s->roots.roots[b].re),ypixel(s->roots.roots[b].im),rgb(158,181,190),1);
    }
    for(i=0;i<n;i++)circle_dot(s->roots.roots[i].re,s->roots.roots[i].im,color,3);
}
static void plot_slot(const Slot *s){
    if(!s->used||!s->visible)return;
    if(s->graph.kind==ND_REAL_EXPLICIT||s->graph.kind==ND_REAL_PARAM||s->graph.kind==ND_IMAG_PARAM)
        plot_curve(s,ink(s->color));
    else if(s->graph.kind==ND_REAL_IMPLICIT||s->graph.kind==ND_IMAG_LOCUS)
        plot_implicit(s,ink(s->color));
    else plot_roots(s,ink(s->color));
}
static void indicator(int x,int y,int yes){
    rect(x-2,y-2,16,16,rgb(117,132,151));
    rect(x,y,12,12,yes?rgb(32,211,105):rgb(50,62,82));
}
static void title(const char *name){
    rect(0,0,W,27,rgb(20,40,67));str(6,7,ft_white,name);
    str(233,7,ft_white,mode?"IMAG":"REAL");
}
/* The bottom strip is an event log, not a hotkey reference bar.
 * H is the ONLY shortcut permanently displayed outside Help. */
static void footer(void){
    rect(0,205,W,35,rgb(20,40,67));
    str(5,209,ft_white,"LOG");
    str(276,209,ft_white,"H Help");
    str(5,224,ft_white,message);
}
static void calculate_intersections(NDPoint *pts,int *n){
    const Slot *a=0,*b=0;int i;
    *n=0;if(mode||!show_intersections)return;
    for(i=0;i<ND_SLOTS;i++){
        const Slot *s=&slots[mode][i];
        if(s->used&&s->visible&&s->graph.kind==ND_REAL_EXPLICIT){if(!a)a=s;else{b=s;break;}}
    }
    if(a&&b)*n=nd_find_intersections(&a->graph,&b->graph,xworld(0),xworld(W),pts,ND_INTERSECTIONS);
}
static void graph_view(void){
    int i,n;char detail[130]="";NDPoint points[ND_INTERSECTIONS];
    Slot *selected_slot=&slots[mode][selected[mode]];
    title("NDgraph v0.31 GRAPH");
    indicator(298,8,selected_slot->used&&selected_slot->light);
    rect(0,TOP,W,PH,rgb(255,255,255));
    if(SDL_MUSTLOCK(screen))SDL_LockSurface(screen);
    grid();
    for(i=0;i<ND_SLOTS;i++)plot_slot(&slots[mode][i]);
    calculate_intersections(points,&n);
    for(i=0;i<n;i++)circle_dot(points[i].x,points[i].y,rgb(204,26,42),3);
    if(trace_on&&selected_slot->used){
        double x=0,y=0;
        if(selected_slot->graph.kind==ND_IMAG_ROOTS&&selected_slot->roots.count>0){
            int ix=(trace_root%selected_slot->roots.count+selected_slot->roots.count)%selected_slot->roots.count;
            CMComplex z=selected_slot->roots.roots[ix];x=z.re;y=z.im;
        }else if(selected_slot->graph.kind==ND_IMAG_POINT){
            NDVars v=env(0,0);int ok;NDComplex z=nd_eval(&selected_slot->graph.rhs,&v,&ok);
            if(ok){x=z.re;y=z.im;}
        }else if(!nd_curve_sample(&selected_slot->graph,trace_parameter,&x,&y)){
            x=NAN;y=NAN;
        }
        if(isfinite(x)&&isfinite(y)){
            circle_dot(x,y,rgb(18,38,53),5);
            if(mode)snprintf(detail,sizeof(detail),"z=%.5g%+.5gi |z|=%.5g arg=%.3g",x,y,hypot(x,y),atan2(y,x));
            else snprintf(detail,sizeof(detail),"TRACE %d: x=%.7g y=%.7g",selected[mode]+1,x,y);
        }else snprintf(detail,sizeof(detail),"Trace unavailable for this relation");
    }
    if(SDL_MUSTLOCK(screen))SDL_UnlockSurface(screen);
    /* Live numerical readouts are overlays. They never replace the event log. */
    if(!detail[0]&&show_intersections&&!mode&&n>0)
        snprintf(detail,sizeof(detail),"Intersections: %d first (%.5g,%.5g)",n,points[0].x,points[0].y);
    if(detail[0]){
        rect(4,29,W-8,14,rgb(225,248,239));
        str(7,31,ft_dark,detail);
    }
    footer();
}
static void functions_view(void){
    int i;char line[90];Slot *slot;
    title("NDgraph v0.31 FUNCTIONS");
    indicator(298,8,slots[mode][selected[mode]].used&&slots[mode][selected[mode]].light);
    for(i=0;i<ND_SLOTS;i++){
        int y=31+i*25;slot=&slots[mode][i];
        rect(3,y,W-6,23,i==selected[mode]?rgb(215,236,252):rgb(241,247,253));
        rect(6,y+5,7,13,ink(slot->color));
        snprintf(line,sizeof(line),"%d %c %c %.49s",i+1,slot->visible?'V':'-',slot->used?(slot->light?'F':'R'):' ',slot->text);
        str(20,y+6,ft_dark,line);
        if(i==selected[mode]&&editing){
            rect(0,y,3,23,rgb(11,138,144));
        }
    }
    if(editing){
        size_t len=strlen(edittext);const char *tail=edittext;
        if(len>46)tail+=len-46;
        rect(3,183,314,20,rgb(222,250,242));
        str(7,188,ft_dark,tail);
        /* Keep all error/success information in the log. */
    }
    footer();
}
static void history_view(void){
    int i;char line[ND_INPUT_MAX+10];const NDHistory *h=&history[mode];
    title("NDgraph v0.31 HISTORY");
    str(7,31,ft_muted,"Session history: newest first");
    for(i=0;i<h->count;i++){
        int y=48+i*12;const char *s=h->lines[i];
        if(i==hist_row)rect(4,y-2,312,13,rgb(215,236,252));
        snprintf(line,sizeof(line),"%2d  %.45s",i+1,s);
        str(7,y,ft_dark,line);
    }
    footer();
}
static void settings_view(void){
    const char *names[]={"Grid","Axes","Angle unit","Connect roots","Parameter range","Mode"};
    char val[60],line[110];int i;
    title("NDgraph v0.31 SETTINGS");
    for(i=0;i<6;i++){
        int y=40+i*25;
        if(i==setting_row)rect(4,y-4,312,23,rgb(215,236,252));
        switch(i){
            case 0:strcpy(val,grid_on?"ON":"OFF");break;
            case 1:strcpy(val,axes_on?"ON":"OFF");break;
            case 2:strcpy(val,degree_on?"DEGREES":"RADIANS");break;
            case 3:strcpy(val,join_roots?"ON":"OFF");break;
            case 4:snprintf(val,sizeof(val),"-%.3g..%.3g",t_range,t_range);break;
            default:strcpy(val,mode?"IMAGINARY":"REAL");break;
        }
        snprintf(line,sizeof(line),"%s: %s",names[i],val);
        str(10,y,ft_dark,line);
    }
    footer();
}
/* Full-screen Help: H toggles it from any non-editing screen. */
static void help_view(void){
    title("NDgraph v0.31 HELP");
    str(7,31,ft_muted,"GENERAL");
    str(7,43,ft_dark,"H        Show / hide this Help screen");
    str(7,55,ft_dark,"Catalog  Toggle Functions / Graph");
    str(7,67,ft_dark,"Tab      Real / Imaginary mode");
    str(7,79,ft_dark,"R        Equation history");
    str(7,91,ft_dark,"S        Settings");
    str(7,106,ft_muted,"GRAPH");
    str(7,118,ft_dark,"T Trace  F Auto-fit  I Intersections");
    str(7,130,ft_dark,"Arrows: pan/zoom, or move trace");
    str(7,142,ft_dark,"PgUp/PgDn: pan Y; Home: recenter");
    str(7,154,ft_dark,"Esc: exit app (from Graph)");
    str(7,168,ft_muted,"FUNCTIONS / OTHER SCREENS");
    str(7,180,ft_dark,"Up/Down: select  Enter: edit/save");
    str(7,192,ft_dark,"V: visibility  C: color  Del: remove");
    str(7,204,ft_dark,"History: Enter load   Settings: arrows");
    rect(0,219,W,21,rgb(20,40,67));
    str(98,226,ft_white,"made by carlos 2026");
}
static void render(void){
    rect(0,0,W,H,rgb(247,250,254));
    switch(view){
        case V_FUNCTIONS:functions_view();break;
        case V_HISTORY:history_view();break;
        case V_SETTINGS:settings_view();break;
        case V_HELP:help_view();break;
        default:graph_view();break;
    }
    SDL_Flip(screen);
}
static void switch_mode(void){
    mode=!mode;editing=0;trace_on=0;show_intersections=0;trace_parameter=0;
    snprintf(message,sizeof(message),"Switched to %s mode",mode?"imaginary":"real");
}
static void commit_edit(void){
    char error[140];static NDGraph prepared;CMResult result;
    Slot *s=&slots[mode][selected[mode]];int k;
    if(edittext[0]==0){
        s->text[0]=0;s->used=0;s->light=0;editing=0;
        snprintf(message,sizeof(message),"Slot %d cleared",selected[mode]+1);return;
    }
    if(!nd_compile(edittext,mode,&prepared,error,sizeof(error))){
        snprintf(message,sizeof(message),"%.120s",error);edit_error=1;return;
    }
    if(prepared.kind==ND_IMAG_ROOTS){
        if(!cm_solve(prepared.raw,&result,error,sizeof(error))){
            snprintf(message,sizeof(message),"%.120s",error);edit_error=1;return;
        }
    }
    s->graph=prepared;
    if(prepared.kind==ND_IMAG_ROOTS)s->roots=result;
    strcpy(s->text,edittext);s->used=1;s->visible=1;s->light=prepared.function_light;
    if(prepared.kind==ND_REAL_IMPLICIT||prepared.kind==ND_IMAG_LOCUS){
        s->light=nd_real_implicit_is_function(&s->graph,xworld(0),xworld(W),yworld(BOT),yworld(TOP));
    }else if(prepared.kind==ND_REAL_PARAM){
        double last=0;int direction=0,valid=1;
        for(k=0;k<=120;k++){
            double x,y,t=-t_range+2*t_range*k/120.0;
            if(!nd_curve_sample(&s->graph,t,&x,&y)){valid=0;break;}
            if(k){double d=x-last;int current=d>1e-8?1:d<(-1e-8)?-1:0;
                if(!current||(direction&&current!=direction)){valid=0;break;}
                direction=current;
            }last=x;
        }
        s->light=valid&&direction!=0;
    }
    nd_history_push(&history[mode],edittext);
    editing=0;trace_on=0;
    snprintf(message,sizeof(message),"Slot %d graphed  |  %s",selected[mode]+1,s->light?"function (green)":"relation/uncertain (dark)");
}
static void fit_add(double x,double y,double *xmin,double *xmax,double *ymin,double *ymax,int *count){
    if(!isfinite(x)||!isfinite(y)||fabs(x)>1e7||fabs(y)>1e7)return;
    if(*count==0){*xmin=*xmax=x;*ymin=*ymax=y;}
    else{if(x<*xmin)*xmin=x;if(x>*xmax)*xmax=x;if(y<*ymin)*ymin=y;if(y>*ymax)*ymax=y;}
    (*count)++;
}
static void auto_fit(void){
    int i,j,n=0;double xmin=0,xmax=0,ymin=0,ymax=0;
    for(i=0;i<ND_SLOTS;i++){
        const Slot *s=&slots[mode][i];
        if(!s->used||!s->visible)continue;
        if(s->graph.kind==ND_IMAG_ROOTS){
            for(j=0;j<s->roots.count;j++)fit_add(s->roots.roots[j].re,s->roots.roots[j].im,&xmin,&xmax,&ymin,&ymax,&n);
        }else if(s->graph.kind==ND_IMAG_POINT){
            NDVars v=env(0,0);int ok;NDComplex z=nd_eval(&s->graph.rhs,&v,&ok);
            if(ok)fit_add(z.re,z.im,&xmin,&xmax,&ymin,&ymax,&n);
        }else if(s->graph.kind==ND_REAL_EXPLICIT||s->graph.kind==ND_REAL_PARAM||s->graph.kind==ND_IMAG_PARAM){
            for(j=0;j<=180;j++){
                double t=(s->graph.kind==ND_REAL_EXPLICIT)?-5+10.0*j/180.0:-t_range+2*t_range*j/180.0;
                double x,y;
                if(nd_curve_sample(&s->graph,t,&x,&y))fit_add(x,y,&xmin,&xmax,&ymin,&ymax,&n);
            }
        }else{
            /* Implicit candidates: scan a fixed domain for sign-changing cells. */
            for(j=0;j<34;j++){
                int k;double xx=-9+18.0*j/33.0;
                for(k=0;k<34;k++){
                    double yy=-9+18.0*k/33.0,a,b,c;
                    if(implicit_value(s,xx,yy,&a)&&
                       implicit_value(s,xx+18.0/33.0,yy,&b)&&
                       implicit_value(s,xx,yy+18.0/33.0,&c)){
                        if((a<0&&b>=0)||(a>0&&b<=0)||(a<0&&c>=0)||(a>0&&c<=0))
                            fit_add(xx,yy,&xmin,&xmax,&ymin,&ymax,&n);
                    }
                }
            }
        }
    }
    if(!n){snprintf(message,sizeof(message),"Auto-fit: no visible graph samples");return;}
    camera[mode].x=(xmin+xmax)*0.5;camera[mode].y=(ymin+ymax)*0.5;
    camera[mode].scale=fmax(0.8,fmax((ymax-ymin)*0.65,(xmax-xmin)*PH/(double)W*0.65));
    if(camera[mode].scale>1e7)camera[mode].scale=1e7;
    snprintf(message,sizeof(message),"Auto-fit: %d samples",n);
}
static void begin_edit(void){
    strcpy(edittext,slots[mode][selected[mode]].text);editing=1;edit_error=0;
}
static void remove_slot(void){
    Slot *s=&slots[mode][selected[mode]];s->text[0]=0;s->used=0;s->light=0;
    snprintf(message,sizeof(message),"Deleted slot %d",selected[mode]+1);
}
static void toggle_intersections(void){
    int count=0,i;show_intersections=!show_intersections;
    if(!mode&&show_intersections){
        for(i=0;i<ND_SLOTS;i++)if(slots[mode][i].used&&slots[mode][i].visible&&
            slots[mode][i].graph.kind==ND_REAL_EXPLICIT)count++;
        if(count<2)snprintf(message,sizeof(message),"Intersections need 2 visible real f(x) graphs");
        else snprintf(message,sizeof(message),"Intersections shown as red dots (numerical)");
    }else snprintf(message,sizeof(message),"Intersections %s",show_intersections?"on":"off");
}
static void settings_change(int direction){
    switch(setting_row){
        case 0:grid_on=!grid_on;snprintf(message,sizeof(message),"Grid %s",grid_on?"enabled":"disabled");break;
        case 1:axes_on=!axes_on;snprintf(message,sizeof(message),"Axes %s",axes_on?"enabled":"disabled");break;
        case 2:degree_on=!degree_on;nd_set_angle_degrees(degree_on);
            if(degree_on&&fabs(t_range-2*PI)<1e-7)t_range=360;
            else if(!degree_on&&fabs(t_range-360)<1e-7)t_range=2*PI;
            snprintf(message,sizeof(message),"Angles: %s",degree_on?"degrees":"radians");break;
        case 3:join_roots=!join_roots;snprintf(message,sizeof(message),"Root guides %s",join_roots?"enabled":"disabled");break;
        case 4:t_range=fmax(0.05,fmin(10000.0,t_range*(direction>0?1.5:1.0/1.5)));
            snprintf(message,sizeof(message),"Parameter range: +/-%.3g",t_range);break;
        case 5:switch_mode();break;
    }
}
static char character(const SDL_KeyboardEvent *ev){
    SDLKey k=ev->keysym.sym;Uint16 u=ev->keysym.unicode;
    if(k>=SDLK_a&&k<=SDLK_z)return (char)('a'+k-SDLK_a);
    if(k>=SDLK_0&&k<=SDLK_9)return (char)('0'+k-SDLK_0);
    switch(k){
        case SDLK_PLUS:return '+';case SDLK_MINUS:return '-';case SDLK_ASTERISK:return '*';
        case SDLK_SLASH:return '/';case SDLK_CARET:return '^';case SDLK_EQUALS:return '=';
        case SDLK_PERIOD:return '.';case SDLK_COMMA:return ',';
        case SDLK_LEFTPAREN:return '(';case SDLK_RIGHTPAREN:return ')';case SDLK_SEMICOLON:return ';';
        default:break;
    }
    if(u>=32&&u<127&&strchr("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789.+-*/^=();",(char)u))
        return (char)tolower((unsigned char)u);
    return 0;
}
static void key_down(const SDL_KeyboardEvent *event,int *running){
    SDLKey k=event->keysym.sym;char ch;size_t len;
    if(view==V_HELP){
        if(k==SDLK_h||k==SDLK_ESCAPE)view=previous_view;
        return;
    }
    /* While editing, H remains a letter (e.g. for sinh/tanh). */
    if(k==SDLK_h&&!(view==V_FUNCTIONS&&editing)){
        previous_view=view;view=V_HELP;return;
    }
    if(view==V_FUNCTIONS){
        if(editing){
            len=strlen(edittext);
            if(k==SDLK_ESCAPE){editing=0;return;}
            if(k==SDLK_RETURN||k==SDLK_KP_ENTER){commit_edit();return;}
            if(k==SDLK_BACKSPACE){if(len)edittext[len-1]=0;return;}
            if(k==SDLK_DELETE){edittext[0]=0;return;}
            ch=character(event);
            if(ch&&len<ND_INPUT_MAX){edittext[len]=ch;edittext[len+1]=0;}
            return;
        }
        if(k==SDLK_ESCAPE){view=V_GRAPH;return;}
        if(k==SDLK_TAB){switch_mode();return;}
        if(k==SDLK_UP){selected[mode]=(selected[mode]+ND_SLOTS-1)%ND_SLOTS;return;}
        if(k==SDLK_DOWN){selected[mode]=(selected[mode]+1)%ND_SLOTS;return;}
        if(k==SDLK_RETURN||k==SDLK_KP_ENTER){begin_edit();return;}
        if(k==SDLK_DELETE){remove_slot();return;}
        if(k==SDLK_r){hist_row=0;view=V_HISTORY;snprintf(message,sizeof(message),"Equation history opened");return;}
        if(k==SDLK_s){setting_row=0;view=V_SETTINGS;snprintf(message,sizeof(message),"Settings opened");return;}
        if(k==SDLK_f){auto_fit();return;}
        if(k==SDLK_v){Slot *s=&slots[mode][selected[mode]];s->visible=!s->visible;
            snprintf(message,sizeof(message),"Slot %d %s",selected[mode]+1,s->visible?"visible":"hidden");return;}
        if(k==SDLK_c){Slot *s=&slots[mode][selected[mode]];s->color=(s->color+1)%6;
            snprintf(message,sizeof(message),"Slot %d color changed",selected[mode]+1);return;}
        if(k==SDLK_t){trace_on=1;trace_parameter=0;view=V_GRAPH;
            snprintf(message,sizeof(message),"Trace enabled");return;}
        /* Typing into a selected slot opens the editor, even if it was empty. */
        ch=character(event);
        if(ch){begin_edit();len=strlen(edittext);if(len<ND_INPUT_MAX){edittext[len]=ch;edittext[len+1]=0;}}
        return;
    }
    if(view==V_HISTORY){
        if(k==SDLK_ESCAPE){view=V_FUNCTIONS;return;}
        if(k==SDLK_UP&&hist_row>0){hist_row--;return;}
        if(k==SDLK_DOWN&&hist_row<history[mode].count-1){hist_row++;return;}
        if((k==SDLK_RETURN||k==SDLK_KP_ENTER)&&history[mode].count){
            begin_edit();strcpy(edittext,history[mode].lines[hist_row]);view=V_FUNCTIONS;return;
        }
        return;
    }
    if(view==V_SETTINGS){
        if(k==SDLK_ESCAPE){view=V_FUNCTIONS;return;}
        if(k==SDLK_UP){setting_row=(setting_row+5)%6;return;}
        if(k==SDLK_DOWN){setting_row=(setting_row+1)%6;return;}
        if(k==SDLK_LEFT){settings_change(-1);return;}
        if(k==SDLK_RIGHT||k==SDLK_RETURN||k==SDLK_KP_ENTER){settings_change(1);return;}
        return;
    }
    /* Graph screen. */
    if(k==SDLK_ESCAPE){*running=0;return;}
    if(k==SDLK_TAB){switch_mode();return;}
    if(k==SDLK_r){view=V_HISTORY;hist_row=0;snprintf(message,sizeof(message),"Equation history opened");return;}
    if(k==SDLK_s){view=V_SETTINGS;setting_row=0;snprintf(message,sizeof(message),"Settings opened");return;}
    if(k==SDLK_f){auto_fit();return;}
    if(k==SDLK_i){toggle_intersections();return;}
    if(k==SDLK_t){trace_on=!trace_on;trace_parameter=0;trace_root=0;
        snprintf(message,sizeof(message),"Trace %s: arrows select graph/position",trace_on?"on":"off");return;}
    if(trace_on){
        Slot *s=&slots[mode][selected[mode]];
        if(k==SDLK_UP){selected[mode]=(selected[mode]+ND_SLOTS-1)%ND_SLOTS;return;}
        if(k==SDLK_DOWN){selected[mode]=(selected[mode]+1)%ND_SLOTS;return;}
        if(k==SDLK_LEFT||k==SDLK_RIGHT){
            int sign=k==SDLK_RIGHT?1:-1;
            if(s->graph.kind==ND_IMAG_ROOTS)trace_root+=sign;
            else trace_parameter+=sign*(s->graph.kind==ND_REAL_EXPLICIT?camera[mode].scale/18.0:t_range/35.0);
            return;
        }
    }
    if(k==SDLK_UP){camera[mode].scale=fmax(1e-4,camera[mode].scale*.80);return;}
    if(k==SDLK_DOWN){camera[mode].scale=fmin(1e7,camera[mode].scale*1.25);return;}
    if(k==SDLK_LEFT){camera[mode].x-=camera[mode].scale*.22;return;}
    if(k==SDLK_RIGHT){camera[mode].x+=camera[mode].scale*.22;return;}
    if(k==SDLK_PAGEUP){camera[mode].y+=camera[mode].scale*.22;return;}
    if(k==SDLK_PAGEDOWN){camera[mode].y-=camera[mode].scale*.22;return;}
    if(k==SDLK_HOME){camera[mode].x=camera[mode].y=0;return;}
}
static void toggle_catalog(void){
    if(view==V_HELP)return; /* H closes Help, leaving the prior view intact. */
    if(view==V_GRAPH){view=V_FUNCTIONS;editing=0;
        snprintf(message,sizeof(message),"Functions tab opened");}
    else{view=V_GRAPH;editing=0;
        snprintf(message,sizeof(message),"Returned to graph");}
}
int main(void){
    SDL_Event event;int running=1,cat_prev=0,dirty=1,im,i;
    for(im=0;im<2;im++)for(i=0;i<ND_SLOTS;i++){
        slots[im][i].visible=1;slots[im][i].color=i;
    }
    if(SDL_Init(SDL_INIT_VIDEO)!=0)return 1;
    screen=SDL_SetVideoMode(W,H,16,SDL_SWSURFACE);
    if(!screen){SDL_Quit();return 1;}
    ft_white=nSDL_LoadFont(NSDL_FONT_TINYTYPE,243,250,255);
    ft_dark=nSDL_LoadFont(NSDL_FONT_TINYTYPE,22,43,69);
    ft_muted=nSDL_LoadFont(NSDL_FONT_TINYTYPE,102,121,143);
    SDL_ShowCursor(SDL_DISABLE);
    SDL_EnableUNICODE(1);
    SDL_EnableKeyRepeat(SDL_DEFAULT_REPEAT_DELAY,SDL_DEFAULT_REPEAT_INTERVAL);
    nd_set_angle_degrees(0);
    while(running){
        /* Poll physical Catalog instead of assuming a particular SDL keycode.
         * Edge detection prevents a long press from rapidly toggling views. */
        int cat=isKeyPressed(KEY_NSPIRE_CAT);
        if(cat&&!cat_prev){toggle_catalog();dirty=1;}
        cat_prev=cat;
        while(SDL_PollEvent(&event)){
            if(event.type==SDL_QUIT){running=0;break;}
            if(event.type==SDL_KEYDOWN){key_down(&event.key,&running);dirty=1;}
        }
        if(dirty&&running){render();dirty=0;}
        SDL_Delay(25);
    }
    nSDL_FreeFont(ft_white);nSDL_FreeFont(ft_dark);nSDL_FreeFont(ft_muted);
    SDL_Quit();return 0;
}
