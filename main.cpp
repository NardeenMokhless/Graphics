#if defined(UNICODE) && !defined(_UNICODE)
#define _UNICODE
#elif defined(_UNICODE) && !defined(UNICODE)
#define UNICODE
#endif

#include <tchar.h>
#include <fstream>
#include <stdlib.h>
#include <windows.h>
#include <cmath>
#include <stack>
#include <vector>
#include <iostream>
#include "menu_header.h"
using namespace std;

/*  Declare Windows procedure  */
LRESULT CALLBACK WindowProcedure(HWND, UINT, WPARAM, LPARAM);
TCHAR szClassName[] = _T("CodeBlocksWindowsApp");

int menuItemsId;
int xs, ys, xe, ye, xm, ym, x4, y4, x5, y5,x6,y6,x7,y7;
bool flag1 = false, flag2 = false, flag3 = false, flag4 = false, flag5=false,drawCircleMidPoint = false,drawLineMidPoint = false, drawpologon = false,
    drawCircle = false,drawLine =false,fill_flood=false,cohen = false,drawLineDDA = false, para = false,polarCircle = false,pologon = false,cubic = false,
    save = false, load = false,hermite = false, pologonClipping = false, flag6 = false, flag7 = false;
struct Point
{
    int x,y;
};
struct line
{
    int xs,ys,xe,ye;
};

struct Action
{
    string actionName;
    vector<Point> points;
};
vector<line> lines;
vector<Action> actions;

union Outcode
    {
        unsigned All:4;
        struct{
            unsigned left:1,right:1,bottom:1,top:1;
        };
    };
struct Vertex
{
double x,y;
    Vertex(int x1=0,int y1=0)
    {
    x=x1;
    y=y1;
    }
};
typedef vector<Vertex> VertexList;
typedef bool (*IsInFunc)(Vertex& v,int edge);
typedef Vertex (*IntersectFunc)(Vertex& v1,Vertex& v2,int edge);



int WINAPI WinMain(HINSTANCE hThisInstance,HINSTANCE hPrevInstance,LPSTR lpszArgument,int nCmdShow)
{
	HWND hwnd;               /* This is the handle for our window */
	MSG messages;            /* Here messages to the application are saved */
	WNDCLASSEX wincl;        /* Data structure for the windowclass */

							 /* The Window structure */
	wincl.hInstance = hThisInstance;
	wincl.lpszClassName = szClassName;
	wincl.lpfnWndProc = WindowProcedure;      /* This function is called by windows */
	wincl.style = CS_DBLCLKS;                 /* Catch double-clicks */
	wincl.cbSize = sizeof(WNDCLASSEX);

	/* Use default icon and mouse-pointer */
	wincl.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wincl.hIconSm = LoadIcon(NULL, IDI_APPLICATION);
	wincl.hCursor = LoadCursor(NULL, IDC_ARROW);
	wincl.lpszMenuName = MAKEINTRESOURCE(MY_MENU_ID);    /* There is  a menu */
	wincl.cbClsExtra = 0;                      /* No extra bytes after the window class */
	wincl.cbWndExtra = 0;                      /* structure or the window instance */
											   /* Use Windows's default colour as the background of the window */
	wincl.hbrBackground = (HBRUSH)COLOR_BACKGROUND;

	/* Register the window class, and if it fails quit the program */
	if (!RegisterClassEx(&wincl))
		return 0;

	/* The class is registered, let's create the program*/
	hwnd = CreateWindowEx(
		0,                   /* Extended possibilites for variation */
		szClassName,         /* Classname */
		_T("Paint"),       /* Title Text */
		WS_OVERLAPPEDWINDOW, /* default window */
		CW_USEDEFAULT,       /* Windows decides the position */
		CW_USEDEFAULT,       /* where the window ends up on the screen */
		544,                 /* The programs width */
		375,                 /* and height in pixels */
		HWND_DESKTOP,        /* The window is a child-window to desktop */
		NULL,                /* No menu */
		hThisInstance,       /* Program Instance handler */
		NULL                 /* No Window Creation data */
	);

	/* Make the window visible on the screen */
	ShowWindow(hwnd, nCmdShow);

	/* Run the message loop. It will run until GetMessage() returns 0 */
	while (GetMessage(&messages, NULL, 0, 0))
	{
		/* Translate virtual-key messages into character messages */
		TranslateMessage(&messages);
		/* Send message to WindowProcedure */
		DispatchMessage(&messages);
	}

	/* The program return-value is 0 - The value that PostQuitMessage() gave */
	return messages.wParam;
}

void swap(int& xs, int& ys, int& xe, int& ye)
{
	int temp = xs;
	xs = xe;
	xe = temp;
	temp = ys;
	ys = ye;
	ye = temp;
}
int roundd(double num)
{
	return (int)(num + 0.5);
}

void DrawLine(HDC hdc, int xs, int ys, int xe, int ye, COLORREF color)
{
    //save action
    Action action;
    action.actionName = "drawLine";
    Point p1,p2;
    p1.x = xs;  p1.y = ys;
    p2.x = xe;  p2.y = ye;
    action.points.push_back(p1);
    action.points.push_back(p2);
    actions.push_back(action);

    //save line points
    line l;
    l.xs = xs;
    l.ys = ys;
    l.xe = xe;
    l.ye = ye;
    lines.push_back(l);

    /////
	int dy = ye - ys;
	int dx = xe - xs;
	if (dy == 0 && dx == 0)
	{
		SetPixel(hdc, xs, ys, color);
		return;
	}
	if (dx == 0)
	{
		for (int y = ys; y<ye; y++)
		{
			SetPixel(hdc, xs, y, color);
		}
		return;
	}

	if (dy < dx)
	{
		if (xs>xe)
		{
			swap(xs, ys, xe, ye);
			dy *= -1;
			dx *= -1;
		}
		double m = double(dy) / dx;
		for (int x1 = xs; x1 <= xe; x1++)
		{
			int y = roundd(ys + ((x1 - xs)*m));
			SetPixel(hdc, x1, y, color);
		}
	}
	else {
		if (ys>ye)
		{
			swap(xs, ys, xe, ye);
			dy *= -1;
			dx *= -1;
		}
		double m = double(dx) / dy;
		for (int y1 = ys; y1<ye; y1++)
		{
			int x = roundd(xs + ((y1 - ys)*m));
			SetPixel(hdc, x, y1, color);
		}
	}
}

void DrawLineDDA(HDC hdc, int xs, int ys, int xe, int ye, COLORREF color)
{

    //save action
    Action action;
    action.actionName = "drawLineDDA";
    Point p1,p2;
    p1.x = xs;  p1.y = ys;
    p2.x = xe;  p2.y = ye;
    action.points.push_back(p1);
    action.points.push_back(p2);
    actions.push_back(action);

    //save line points
    line l;
    l.xs = xs;
    l.ys = ys;
    l.xe = xe;
    l.ye = ye;
    lines.push_back(l);

    ////
	int dy = ye - ys;
	int dx = xe - xs;
	int x, y;

	if (dy == 0 && dx == 0)
	{
		SetPixel(hdc, xs, ys, color);
		return;
	}
	if (dx == 0)
	{
		for (int y = ys; y<ye; y++)
		{
			SetPixel(hdc, xs, y, color);
		}
		return;
	}

	if (dy <= dx)
	{
		if (xs > xe)
		{
			swap(xs, ys, xe, ye);
			dy *= -1;
			dx *= -1;
		}
		double m = double(dy) / dx;
		x = xs;
		y = ys;
		SetPixel(hdc, x, y, color);
		while(x < xe)
		{
		    x++;
			y += m;
			SetPixel(hdc, x, round(y), color);
		}
	}
	else
    {
        if (ys > ye)
		{
			swap(xs, ys, xe, ye);
			dy *= -1;
			dx *= -1;
		}
		double m = double(dx) / dy;
		x = xs;
		y = ys;
		SetPixel(hdc, x, y, color);
		while(x < xe)
		{
		    y++;
			x += ((double)1/m);
			SetPixel(hdc, round(x), y, color);
		}

	}
}

void DrawLineMidPoint(HDC hdc, int xs, int ys, int xe, int ye, COLORREF color)
{
    //save action
    Action action;
    action.actionName = "drawLineMidPoint";
    Point p1,p2;
    p1.x = xs;  p1.y = ys;
    p2.x = xe;  p2.y = ye;
    action.points.push_back(p1);
    action.points.push_back(p2);
    actions.push_back(action);

    //save line points
    line l;
    l.xs = xs;
    l.ys = ys;
    l.xe = xe;
    l.ye = ye;
    lines.push_back(l);

    /////
	int dx = xe - xs;
	int dy = ye - ys;
	int d, ch1, ch2;

	if (dy == 0 && dx == 0)
	{
		SetPixel(hdc, xs, ys, color);
		return;
	}

	if( abs(dy) < abs(dx) && dx != 0)
	{
		if (xs>xe)
		{
			swap(xs, ys, xe, ye);
			dy *= -1;
			dx *= -1;
		}
		int x = xs, y = ys;
		SetPixel(hdc, x, y, color);

		if (dy > 0) //f(x+1,y+0.5)
		{
			d = dx - 2 * dy;
			ch1 = -2 * dy;
			ch2 = 2 * (dx - dy);
		}
		else //f(x-1,y+0.5)
		{
			d = dx + 2 * dy;
			ch1 = 2 * dy;
			ch2 = 2 * (dx + dy);
		}

		while (x < xe)
		{
			if (d > 0)
			{
				d = d + ch1;
				x++;
			}
			else
			{
				d = d + ch2;
				x++;

				if (dy > 0) //the line is /.
					y++;
				else    //the line is \.
					y--;
			}
			SetPixel(hdc, x, y, color);
		}
	}
	else
	{
		if (ys>ye)
		{
			swap(xs, ys, xe, ye);
			dy *= -1;
			dx *= -1;
		}
		int x = xs, y = ys;
		SetPixel(hdc, x, y, color);

		if (dx > 0)		//f(x+0.5,y+1)
		{
			d = dy - 2 * dx;
			ch1 = -2 * dx;
			ch2 = 2 * (dy - dx);
		}
		else //f(x-0.5,y+1)
		{
			d = dy + 2 * dx;
			ch1 = 2 * dx;
			ch2 = 2 * (dy + dx);
		}

		while (y < ye)
		{
			if (d > 0)
			{
				d = d + ch1;
				y++;
			}
			else
			{
				d = d + ch2;
				y++;
				if (dx > 0) //the line is /.
					x++;
				else	//the line is \.
					x--;
			}
			SetPixel(hdc, x, y, color);
		}
	}
}

void DrawParabolaCurve(HDC hdc, int xs, int ys, int xm, int ym, int xe, int ye, COLORREF color)
{
    //save action
    Action action;
    action.actionName = "drawParabolaCurve";
    Point p1,p2,p3;
    p1.x = xs;  p1.y = ys;
    p2.x = xm;  p2.y = ym;
    p3.x = xe;  p3.y = ye;
    action.points.push_back(p1);
    action.points.push_back(p2);
    action.points.push_back(p3);
    actions.push_back(action);

	for (double t = 0; t < 1; t += 0.001)
	{
		double x = xs + (t*double(4 * xm - 3 * xs - xe)) + (t*t*double(2 * xe + 2 * xs - 4 * xm));
		double y = ys + (t*double(4 * ym - 3 * ys - ye)) + (t*t*double(2 * ye + 2 * ys - 4 * ym));

		SetPixel(hdc, roundd(x), roundd(y), color);
	}

}
void DrawCubicCurve(HDC hdc, int x1, int y1, int x2, int y2,int x3,int y3, int x4, int y4, COLORREF color)
{
    //save action
    Action action;
    action.actionName = "drawCubicCurve";
    Point p1,p2,p3,p4;
    p1.x = x1;  p1.y = y1;
    p2.x = x2;  p2.y = y2;
    p3.x = x3;  p3.y = y3;
    p4.x = x4;  p4.y = y4;
    action.points.push_back(p1);
    action.points.push_back(p2);
    action.points.push_back(p3);
    action.points.push_back(p4);
    actions.push_back(action);

	for (double t = 0; t < 1; t += 0.001)
	{
		double x = ((1-t)*(1-t)*(1-t)*x1) + ((1-t)*(1-t)*3*t*x2)+ ((1-t)*t*t*3*x3)+ (t*t*t*x4);
		double y = ((1-t)*(1-t)*(1-t)*y1) + ((1-t)*(1-t)*3*t*y2)+ ((1-t)*t*t*3*y3)+ (t*t*t*y4);

		SetPixel(hdc, round(x), round(y), color);
	}

}
void Hermite(HDC hdc, int x1, int y1, int x2, int y2, int x3, int y3, int x4, int y4, COLORREF color)
{
     //save action
    Action action;
    action.actionName = "hermite";
    Point p1,p2,p3,p4;
    p1.x = x1;  p1.y = y1;
    p2.x = x2;  p2.y = y2;
    p3.x = x3;  p3.y = y3;
    p4.x = x4;  p4.y = y4;
    action.points.push_back(p1);
    action.points.push_back(p2);
    action.points.push_back(p3);
    action.points.push_back(p4);
    actions.push_back(action);

	int a3, a2, a1, a0, b3, b2, b1, b0;
	a0 = x1;
	a1 = x2;
	a2 = -3 * x1 - 2 * x2 + 3 * x3 - x4;
	a3 = 2 * x1 + x2 - 2 * x3 + x4;

	b0 = y1;
	b1 = y2;
	b2 = -3 * y1 - 2 * y2 + 3 * y3 - y4;
	b3 = 2 * y1 + y2 - 2 * y3 + y4;

	double x, y;
	for (double t = 0; t <= 1; t += 0.0001)
	{
		x = a3*(t*t*t) + a2*(t*t) + a1*(t) + a0;
		y = b3*(t*t*t) + b2*(t*t) + b1*(t) + b0;
		SetPixel(hdc, round(x), round(y), color);
	}
}
void Draw8points(HDC hdc, int xc, int yc, int x, int y, COLORREF color)
{
	SetPixel(hdc, xc + x, yc + y, color);
	SetPixel(hdc, xc + x, yc - y, color);
	SetPixel(hdc, xc - x, yc + y, color);
	SetPixel(hdc, xc - x, yc - y, color);
	SetPixel(hdc, xc + y, yc + x, color);
	SetPixel(hdc, xc + y, yc - x, color);
	SetPixel(hdc, xc - y, yc + x, color);
	SetPixel(hdc, xc - y, yc - x, color);
}
void DrawCircle(HDC hdc, int xc, int yc, int xe, int ye, COLORREF color)
{
     //save action
    Action action;
    action.actionName = "drawCircle";
    Point p1,p2;
    p1.x = xc;  p1.y = yc;
    p2.x = xe;  p2.y = ye;
    action.points.push_back(p1);
    action.points.push_back(p2);
    actions.push_back(action);

	double r = sqrt(((xe - xc)*(xe - xc)) + ((ye - yc)*(ye - yc)));

	int x = 0;
	int y = r;
	Draw8points(hdc, xc, yc, x, y, color);
	while (x < y)
	{
		x++;
		y = roundd(sqrt(r*r - x*x));
		Draw8points(hdc, xc, yc, x, y, color);
	}

}
void DrawCirclePolar(HDC hdc, int xc, int yc, int xe, int ye, COLORREF color)
{
     //save action
    Action action;
    action.actionName = "drawCirclePolar";
    Point p1,p2;
    p1.x = xc;  p1.y = yc;
    p2.x = xe;  p2.y = ye;
    action.points.push_back(p1);
    action.points.push_back(p2);
    actions.push_back(action);

	double r = sqrt(((xe - xc)*(xe - xc)) + ((ye - yc)*(ye - yc)));
	double x = r, y = 0;
	Draw8points(hdc, r, 0, xc, yc, color);
	double c = cos(1.0/r), s = sin(1.0/r);

 	while(x > y)
	{
	    double x1 = x*c - y*s;
	    y = x*s + y*c;
	    x = x1;
	    Draw8points(hdc, xc, yc,round(x), round(y),color);
	}

}

void DrawCircleMidPoint(HDC hdc, int xc, int yc, int xe, int ye, COLORREF color)
{
    //save action
    Action action;
    action.actionName = "drawCircleMidPoint";
    Point p1,p2;
    p1.x = xc;  p1.y = yc;
    p2.x = xe;  p2.y = ye;
    action.points.push_back(p1);
    action.points.push_back(p2);
    actions.push_back(action);

	double r = sqrt(((xe - xc)*(xe - xc)) + ((ye - yc)*(ye - yc)));
	int x = 0;
	double y = r;
	Draw8points(hdc, xc, yc, x, roundd(y), color);

	double d = 1 - r;
	while (x < y)
	{
		if (d < 0)
			d += (2 * x + 3);
		else
		{
			d += (2 * (x - y) + 5);
			y--;
		}
		x++;
		Draw8points(hdc, xc, yc, x, roundd(y), color);

	}


}

void MyFloodFill(HDC hdc,int x,int y, COLORREF borderColor,COLORREF fillColor)
{
     //save action
    Action action;
    action.actionName = "myFloodFill";
    Point p1;
    p1.x = x;  p1.y = y;
    action.points.push_back(p1);
    actions.push_back(action);

    stack<int> xp;
    stack<int> yp;
    xp.push(x);
    yp.push(y);

    while(!xp.empty())
    {
        int xtop = xp.top();
        xp.pop();
        int ytop = yp.top();
        yp.pop();
        COLORREF color = GetPixel(hdc,xtop,ytop);
        if(color != borderColor && color != fillColor)
        {
            SetPixel(hdc,xtop,ytop,fillColor);
            xp.push(xtop+1);
            yp.push(ytop);

            xp.push(xtop);
            yp.push(ytop+1);

            xp.push(xtop-1);
            yp.push(ytop);

            xp.push(xtop);
            yp.push(ytop-1);
        }
    }

}

Outcode GetOutcode (float x,float y,int xleft,int xright,int ybottom,int ytop)
{
    Outcode r;          r.All = 0;
    if(x < xleft)       r.left = 1;
    else if(x > xright) r.right = 1;
    if(y < ybottom)     r.bottom = 1;
    else if(y > ytop)   r.top = 1;
    return r;
}
void VIntersect(int xedge,double xs,double ys,double xe,double ye,double &xin,double &yin)
{
    xin = xedge;
    yin = ys + (xedge - xs)*(ye - ys)/(xe - xs);
}
void HIntersect(int yedge,double xs,double ys,double xe,double ye,double &xin,double &yin)
{
    yin = yedge;
    xin = xs + (yedge - ys)*(xe - xs)/(ye - ys);
}
void CohenSuth(double xs,double ys,double xe,double ye,int xleft,int xright,int ybottom,int ytop,HDC hdc,COLORREF color)
{
    Outcode out1 = GetOutcode(xs,ys,xleft,xright,ybottom,ytop);
    Outcode out2 = GetOutcode(xe,ye,xleft,xright,ybottom,ytop);

    while((out1.All || out2.All) && !(out1.All & out2.All))
    {
        if(out1.All)
        {
            if(out1.left)
            {
                double xin,yin;
                VIntersect(xleft,xs,ys,xe,ye,xin,yin);
                xs = xin;
                ys = yin;
            }
            else if(out1.right)
            {
                double xin,yin;
                VIntersect(xright,xs,ys,xe,ye,xin,yin);
                xs = xin;
                ys = yin;
            }
            else if(out1.bottom)
            {
                double xin,yin;
                HIntersect(ybottom,xs,ys,xe,ye,xin,yin);
                xs = xin;
                ys = yin;
            }
            else
            {
                double xin,yin;
                HIntersect(ytop,xs,ys,xe,ye,xin,yin);
                xs = xin;
                ys = yin;
            }
            out1 = GetOutcode(xs,ys,xleft,xright,ybottom,ytop);
        }
        else
        {
             if(out2.left)
            {
                double xin,yin;
                VIntersect(xleft,xs,ys,xe,ye,xin,yin);
                xe = xin;
                ye = yin;
            }
            else if(out2.right)
            {
                double xin,yin;
                VIntersect(xright,xs,ys,xe,ye,xin,yin);
                xe = xin;
                ye = yin;
            }
            else if(out2.bottom)
            {
                double xin,yin;
                HIntersect(ybottom,xs,ys,xe,ye,xin,yin);
                xe = xin;
                ye = yin;
            }
            else
            {
                double xin,yin;
                HIntersect(ytop,xs,ys,xe,ye,xin,yin);
                xe = xin;
                ye = yin;
            }
            out2 = GetOutcode(xe,ye,xleft,xright,ybottom,ytop);
        }

    }
    if(out1.All == 0 && out2.All == 0)
        DrawLine(hdc,xs,ys,xe,ye,color);
}

void Cohen(int xleft,int xright,int ybottom,int ytop,HDC hdc,COLORREF color)
{
    //save action
    Action action;
    action.actionName = "Cohen";
    Point p1,p2;
    p1.x = xleft;  p1.y = ybottom;
    p2.x = xright;  p2.y = ytop;
    action.points.push_back(p1);
    action.points.push_back(p2);
    actions.push_back(action);

    color = RGB(150, 55, 25);
    int sz = lines.size();
    for(int i = 0;i<sz;i++)
    {
        CohenSuth(lines[i].xs,lines[i].ys,lines[i].xe,lines[i].ye,xleft,xright,ybottom,ytop,hdc,color);
    }
}

void DrawPolygon(HDC hdc,int x1,int y1,int x2,int y2,int x3,int y3,int x4,int y4,int x5,int y5,COLORREF color)
{
     DrawLineMidPoint(hdc,x1,y1,x2,y2,color);
     DrawLineMidPoint(hdc,x2,y2,x3,y3,color);
     DrawLineMidPoint(hdc,x3,y3,x4,y4,color);
     DrawLineMidPoint(hdc,x4,y4,x5,y5,color);
     DrawLineMidPoint(hdc,x5,y5,x1,y1,color);
}
void Polygon(HDC hdc,int x1,int y1,int x2,int y2,int x3,int y3,int x4,int y4,int x5,int y5,COLORREF color)
{
     POINT points[5];
     points[0] = { x1, y1 };
     points[1] = { x2, y2 };
     points[2] = { x3, y3 };
     points[3] = { x4, y4 };
     points[4] = { x5, y5 };
     Polygon(hdc, points, 5);

}

bool InLeft(Vertex& v,int edge)
{
return v.x>=edge;
}
bool InRight(Vertex& v,int edge)
{
    return v.x<=edge;
}
bool InTop(Vertex& v,int edge)
{
    return v.y>=edge;
}
bool InBottom(Vertex& v,int edge)
{
    return v.y<=edge;
}
Vertex VIntersect(Vertex& v1,Vertex& v2,int xedge)
{
    Vertex res;
    res.x=xedge;
    res.y=v1.y+(xedge-v1.x)*(v2.y-v1.y)/(v2.x-v1.x);
    return res;
}
Vertex HIntersect(Vertex& v1,Vertex& v2,int yedge)
{
    Vertex res;
    res.y=yedge;
    res.x=v1.x+(yedge-v1.y)*(v2.x-v1.x)/(v2.y-v1.y);
    return res;
}
VertexList ClipWithEdge(VertexList p,int edge,IsInFunc In,IntersectFunc Intersect)
{
    VertexList OutList;
    Vertex v1 = p[p.size()-1];
    bool v1_in =In(v1,edge);
    for(int i=0;i<(int)p.size();i++)
    {
        Vertex v2 = p[i];
        bool v2_in = In(v2,edge);
        if(!v1_in && v2_in)
        {
            OutList.push_back(Intersect(v1,v2,edge));
            OutList.push_back(v2);
        }
        else if(v1_in && v2_in) OutList.push_back(v2);
        else if(v1_in) OutList.push_back(Intersect(v1,v2,edge));
        v1=v2;
        v1_in=v2_in;
    }
    return OutList;
}
void PolygonClip(HDC hdc,COLORREF color,int xleft,int ytop,int xright,int ybottom)
{
    VertexList vlist;

    vlist.push_back(Vertex(lines[lines.size()-5].xs,lines[lines.size()-5].ys));
    vlist.push_back(Vertex(lines[lines.size()-4].xs,lines[lines.size()-4].ys));
    vlist.push_back(Vertex(lines[lines.size()-3].xs,lines[lines.size()-3].ys));
    vlist.push_back(Vertex(lines[lines.size()-2].xs,lines[lines.size()-2].ys));
    vlist.push_back(Vertex(lines[lines.size()-1].xs,lines[lines.size()-1].ys));

    vlist=ClipWithEdge(vlist,xleft,InLeft,VIntersect);
    vlist=ClipWithEdge(vlist,ytop,InTop,HIntersect);
    vlist=ClipWithEdge(vlist,xright,InRight,VIntersect);
    vlist=ClipWithEdge(vlist,ybottom,InBottom,HIntersect);

    Vertex v1=vlist[vlist.size()-1];
    for(int i=0;i<(int)vlist.size();i++)
    {
        Vertex v2=vlist[i];
        DrawLineMidPoint(hdc,round(v1.x),round(v1.y),round(v2.x),round(v2.y),color);
        v1=v2;
    }
}
void SaveActions()
{
    ofstream myfile;
    myfile.open ("Paint.txt");
    int sz = actions.size();
    cout<<"size = "<<sz<<endl;
    for(int i = 0; i < sz;i++)
    {
        cout<<"Action name: "<<actions[i].actionName<<endl;
        myfile << actions[i].actionName;
        myfile << " ";
        myfile << actions[i].points.size();
        myfile << " ";
        for(int j=0;j< actions[i].points.size();j++)
        {
             cout<<"Point "<<j+1<<": ("<<actions[i].points[j].x<<","<<actions[i].points[j].y<<") "<<endl;
             myfile << actions[i].points[j].x;
             myfile << " ";
             myfile << actions[i].points[j].y;
             myfile << " ";
        }
    }
    myfile.close();
}
void  MakeAction(Action action,HDC hdc,COLORREF color)
{
    cout<<"Action >>"<<action.actionName<<endl;
    if( action.actionName == "drawLine")
        DrawLine(hdc,action.points[0].x, action.points[0].y, action.points[1].x, action.points[1].y, color);
    else if( action.actionName == "drawLineDDA")
        DrawLineDDA(hdc,action.points[0].x, action.points[0].y, action.points[1].x, action.points[1].y, color);
    else if( action.actionName == "drawLineMidPoint")
        DrawLineMidPoint(hdc,action.points[0].x, action.points[0].y, action.points[1].x, action.points[1].y, color);
    else if( action.actionName == "drawParabolaCurve")
        DrawParabolaCurve(hdc,action.points[0].x, action.points[0].y, action.points[1].x, action.points[1].y, action.points[2].x,action.points[2].y, color);
    else if( action.actionName == "drawCubicCurve")
        DrawCubicCurve(hdc,action.points[0].x, action.points[0].y, action.points[1].x, action.points[1].y, action.points[2].x, action.points[2].y,action.points[3].x, action.points[3].y, color);
    else if( action.actionName == "drawCircle")
        DrawCircle(hdc,action.points[0].x, action.points[0].y, action.points[1].x, action.points[1].y, color);
     else if( action.actionName == "drawCirclePolar")
        DrawCirclePolar(hdc,action.points[0].x, action.points[0].y, action.points[1].x, action.points[1].y, color);
     else if( action.actionName == "drawCircleMidPoint")
        DrawCircleMidPoint(hdc,action.points[0].x, action.points[0].y, action.points[1].x, action.points[1].y, color);
    else if( action.actionName == "myFloodFill")
        MyFloodFill(hdc,action.points[0].x, action.points[0].y, color, RGB(0, 255, 0));
    else if( action.actionName == "Cohen")
        Cohen(action.points[0].x,action.points[1].x,action.points[0].y,action.points[1].y,hdc,color);
    else if( action.actionName == "hermite")
        Hermite(hdc,action.points[0].x, action.points[0].y, action.points[1].x, action.points[1].y, action.points[2].x, action.points[2].y,action.points[3].x, action.points[3].y, color);

}
void LoadActions(HDC hdc,COLORREF color)
{
    ifstream infile("Paint.txt");
    vector<Action> actionsToBeDone;
	if ( !infile.is_open()) {
		cout << "Error"<<endl;
	}
	else
	{
	    while(!infile.eof())
		{
		    Action action;  int sz;
			infile >> action.actionName;
			if(infile.eof())
                break;
			infile >> sz;
			for(int i=0;i<sz;i++)
            {
                Point point;
                infile >> point.x;
                infile >> point.y;
                action.points.push_back(point);
            }
            actionsToBeDone.push_back(action);
		}
		infile.close();

		for(int i=0;i<actionsToBeDone.size();i++)
            MakeAction(actionsToBeDone[i],hdc,color);
	}
}

LRESULT CALLBACK WindowProcedure(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	PAINTSTRUCT p;
	COLORREF color = RGB(0, 0, 255);
	HDC hdc;

	switch (message)                  /* handle the messages */
	{
	case WM_DESTROY:
		PostQuitMessage(0);       /* send a WM_QUIT to the message queue */
		break;
	case WM_COMMAND:
		menuItemsId = LOWORD(wParam);
		switch (menuItemsId)
		{
		case MY_FILE_EXIT_ID:
			PostQuitMessage(0);
			break;
        case Save_ID:
			save = true;
			break;
        case Load_ID:
            load = true;
            break;
        case FILL_FLOOD_ID:
            fill_flood = true;
		case MY_CIRCLE_MIDPOINT_ID:
			drawCircleMidPoint = true;
			break;
        case MY_CIRCLE_CARTESIAN_ID:
			drawCircle = true;
			break;
        case MY_CIRCLE_POLAR_ID:
            polarCircle=true;
            break;
        case MY_LINE_MIDPOINT_ID:
			drawLineMidPoint = true;
			break;
        case MY_LINE_CARTESIAN_ID:
			drawLine = true;
			break;
        case MY_LINE_DDA_ID:
			drawLineDDA = true;
			break;
        case Parabola_ID:
			para = true;
			break;
        case Cubic_ID:
			cubic = true;
			break;
        case PolygonClipping_ID:
            pologonClipping = true;
            break;
        case Hermite_ID:
            hermite = true;
            break;
        case COHEN_ID:
			cohen = true;
			break;
        case POLYGON_ID:
			drawpologon = true;
			break;
        case POLYGON2_ID:
            pologon = true;
            break;
		default:
			return DefWindowProc(hwnd, message, wParam, lParam);
		}
		break;
	case WM_PAINT:
		if (flag1 && flag2 && drawCircleMidPoint)
		{
			hdc = BeginPaint(hwnd, &p);
			DrawCircleMidPoint(hdc, xs, ys, xe, ye, color);
			EndPaint(hwnd, &p);
			flag1 = false;
			flag2 = false;
			drawCircleMidPoint = false;
			InvalidateRect(hwnd, NULL, FALSE);  //to paint more points and remain the prev points
		}
		if (flag1 && flag2 && drawCircle)
		{
			hdc = BeginPaint(hwnd, &p);
			DrawCircle(hdc, xs, ys, xe, ye, color);
			EndPaint(hwnd, &p);
			flag1 = false;
			flag2 = false;
			drawCircle = false;
			InvalidateRect(hwnd, NULL, FALSE);  //to paint more points and remain the prev points
		}
		if (flag1 && flag2 && polarCircle)
		{
			hdc = BeginPaint(hwnd, &p);
			DrawCirclePolar(hdc, xs, ys, xe, ye, color);
			EndPaint(hwnd, &p);
			flag1 = false;
			flag2 = false;
			polarCircle = false;
			InvalidateRect(hwnd, NULL, FALSE);  //to paint more points and remain the prev points
		}
		if (flag1 && flag2 && drawLineMidPoint)
		{
			hdc = BeginPaint(hwnd, &p);
			DrawLineMidPoint(hdc, xs, ys, xe, ye, color);
			EndPaint(hwnd, &p);
			flag1 = false;
			flag2 = false;
			drawLineMidPoint = false;
			InvalidateRect(hwnd, NULL, FALSE);  //to paint more points and remain the prev points
		}
		if (flag1 && flag2 && drawLine)
		{
			hdc = BeginPaint(hwnd, &p);
			DrawLine(hdc, xs, ys, xe, ye, color);
			EndPaint(hwnd, &p);
			flag1 = false;
			flag2 = false;
			drawLine = false;
			InvalidateRect(hwnd, NULL, FALSE);  //to paint more points and remain the prev points
		}
		if (flag1 && flag2 && drawLineDDA)
		{
			hdc = BeginPaint(hwnd, &p);
			DrawLineDDA(hdc, xs, ys, xe, ye, color);
			EndPaint(hwnd, &p);
			flag1 = false;
			flag2 = false;
			drawLineDDA= false;
			InvalidateRect(hwnd, NULL, FALSE);  //to paint more points and remain the prev points
		}
		if(flag1 && fill_flood)
        {
            hdc = BeginPaint(hwnd, &p);
			MyFloodFill(hdc, xs, ys, color, RGB(0, 255, 0) );
			EndPaint(hwnd, &p);
			flag1 = false;
			fill_flood = false;
			InvalidateRect(hwnd, NULL, FALSE);  //to paint more points and remain the prev points

        }
        if(flag1 && flag2 && cohen)
        {
            actions.clear();
            InvalidateRect(hwnd, NULL, TRUE);
            hdc = BeginPaint(hwnd, &p);
			Cohen(xs,xe,ys,ye,hdc,color);
			EndPaint(hwnd, &p);
			flag1 = false;
			flag2 = false;
			cohen = false;
			InvalidateRect(hwnd, NULL, FALSE);  //to paint more points and remain the prev points
        }
        if(flag1 && flag2 && flag3 && para)
        {
            hdc = BeginPaint(hwnd, &p);
			DrawParabolaCurve(hdc,xs,ys, xe, ye, xm, ym,color);
			EndPaint(hwnd, &p);
			flag1 = false;
			flag2 = false;
			flag3 = false;
			para = false;
			InvalidateRect(hwnd, NULL, FALSE);  //to paint more points and remain the prev points

        }
        if(flag1 && flag2 && flag3 && flag4 && cubic)
        {
            hdc = BeginPaint(hwnd, &p);
			DrawCubicCurve(hdc,xs,ys, xe, ye, xm, ym,x4,y4,color);
			EndPaint(hwnd, &p);
			flag1 = false;
			flag2 = false;
			flag3 = false;
			flag4 = false;
			cubic = false;
			InvalidateRect(hwnd, NULL, FALSE);  //to paint more points and remain the prev points
        }
        if(flag1 && flag2 && flag3 && flag4 && hermite)
        {
            hdc = BeginPaint(hwnd, &p);
			Hermite(hdc,xs,ys, xe, ye, xm, ym,x4,y4,color);
			EndPaint(hwnd, &p);
			flag1 = false;
			flag2 = false;
			flag3 = false;
			flag4 = false;
			hermite = false;
			InvalidateRect(hwnd, NULL, FALSE);  //to paint more points and remain the prev points
        }
        if(drawpologon && flag1 && flag2 && flag3 && flag4 && flag5)
        {
            hdc = BeginPaint(hwnd, &p);
            DrawPolygon(hdc,xs,ys,xe,ye,xm,ym,x4,y4,x5,y5,color);
			EndPaint(hwnd, &p);
			drawpologon = false;
			flag1 = false;
			flag2 = false;
			flag3 = false;
			flag4 = false;
			flag5 = false;
			InvalidateRect(hwnd, NULL, FALSE);
        }
        if(pologonClipping && flag1 && flag2 )
        {
            actions.clear();
            InvalidateRect(hwnd, NULL, TRUE);
            hdc = BeginPaint(hwnd, &p);
			PolygonClip(hdc,color,xs,ys,xe,ye);
			EndPaint(hwnd, &p);
			flag1 = false;
			flag2 = false;
			pologonClipping = false;
			InvalidateRect(hwnd, NULL, FALSE);
        }
        if(pologon && flag1 && flag2 && flag3 && flag4 && flag5)
        {
            hdc = BeginPaint(hwnd, &p);
			Polygon(hdc,xs,ys,xe,ye,xm,ym,x4,y4,x5,y5,color);
			EndPaint(hwnd, &p);
			pologon = false;
			flag1 = false;
			flag2 = false;
			flag3 = false;
			flag4 = false;
			flag5 = false;
			InvalidateRect(hwnd, NULL, FALSE);
        }
        if(save)
        {
            hdc = BeginPaint(hwnd, &p);
			SaveActions();
			EndPaint(hwnd, &p);
			save = false;
			InvalidateRect(hwnd, NULL, FALSE);
        }
        if(load)
        {
            hdc = BeginPaint(hwnd, &p);
			LoadActions(hdc,color);
			EndPaint(hwnd, &p);
			load = false;
			InvalidateRect(hwnd, NULL, FALSE);
        }
		break;
	case WM_LBUTTONDOWN:
		if (!flag1)
		{
			xs = LOWORD(lParam);
			ys = HIWORD(lParam);
			flag1 = true;
			break;
		}
		else if (!flag2)
		{
			xe = LOWORD(lParam);
			ye = HIWORD(lParam);
			flag2 = true;
            break;
		}
		else if (!flag3)
		{
			xm = LOWORD(lParam);
			ym = HIWORD(lParam);
			flag3 = true;
            break;
		}
		else if (!flag4)
		{
			x4 = LOWORD(lParam);
			y4 = HIWORD(lParam);
			flag4 = true;
            break;
		}
		else if (!flag5)
		{
			x5 = LOWORD(lParam);
			y5 = HIWORD(lParam);
			flag5 = true;
		}
		else if (!flag6)
		{
			x6 = LOWORD(lParam);
			y6 = HIWORD(lParam);
			flag6 = true;
		}
		else if (!flag7)
		{
			x7 = LOWORD(lParam);
			y7 = HIWORD(lParam);
			flag7 = true;
		}
		break;

	default:
		return DefWindowProc(hwnd, message, wParam, lParam);
	}
	return 0;
}
