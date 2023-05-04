#include <graphics.h>
#include <conio.h>

double powi(double v, int k){
	double temp = 1.0;
	if(k == 0 && v == 0){
		return 1;
	}else{
		for(int i = 1;i <= k;i++){
			temp = temp * v;
		}
	}
	return temp;
}

long fac(int m){
	int i;
	long temp = 1;
	if(m == 0) return 1;
	else{
		for(i = 2;i <= m;i++){
			temp = temp * i;
		}
	}
	return temp;
}

void Bezier(POINT *p,int n){
	int x,y,i,j,k = 100;
	double t,t1,u,v;
	double temp,temp1,temp2,bi;
	t = 1.0/k;
	moveto(p[0].x,p[0].y);
	for(j = 1;j < k;j++){
		t1 = j * t;
		u = t1;
		v = 1 - u;
		x = 0;y = 0;
		for(i = 0;i <= n;i++){
			temp = (double)fac(n)/fac(i)/fac(n - 1);
			temp1 = powi(u,i);
			temp2 = powi(v,n - i);
			bi = temp * temp1 * temp2;
			x = (int)(x + bi * p[i].x);
			y = (int)(y + bi * p[i].y);
		}
		lineto(x,y);
	}
	lineto(p[n].x,p[n].y);
}

int main(){
	initgraph(640,480);
	circle(200,200,100);
	Sleep(3000);
	setlinecolor(RED);
	line(1,1,100,100);
	setlinecolor(BLUE);
	line(100,100,200,200);
	
	//POINT[] p = new POINT();
	//p[0].x = 100;
	//p[0].y = 100;
	//p[1].x = 200;
	//p[1].y = 200;
	//p[2].x = 100;
	//p[2].y = 200;
	//Bezier(&p,5);
	getch();
	closegraph();
	return 0;
}