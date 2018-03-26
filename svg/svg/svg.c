#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <conio.h>
#include <graphics\graphics.h>
#include <math.h>

typedef struct point
{
	int xpos;
	int ypos;
} Point;

typedef struct svg
{
	char id;
	int width;
	int height;
	int viewbox[4];
	int tranlate[3];
	int rotate[3];
	int scale[3];
} SVG;

typedef struct line
{
	char id;
	int x1,y1;
	int x2,y2;
	int tranlate[3];
	int rotate[3];
	int scale[3];
} LINE;

typedef struct rect
{
	char id;
	int width;
	int height;
	int x,y;
	int rx,ry;
	int tranlate[3];
	int rotate[3];
	int scale[3];
} RECT;

typedef struct circle
{
	char id;
	int cx,cy;
	int r;
	int tranlate[3];
	int rotate[3];
	int scale[3];
} CIRCLE;

typedef struct ellipse
{
	char id;
	int cx,cy;
	int rx,ry;
	int tranlate[3];
	int rotate[3];
	int scale[3];
} ELLIPSE;

typedef struct path
{
	char id;
	Point M[10];
	Point L[10];
	Point Q[10];
	Point C[10];
	char flow[10];
	int tranlate[3];
	int rotate[3];
	int scale[3];
} PATH;

typedef struct polygon
{
	char id;
	Point pos[20];
	int tranlate[3];
	int rotate[3];
	int scale[3];
} POLYGON;

typedef struct polyline
{
	char id;
	Point pos[20];
	int tranlate[3];
	int rotate[3];
	int scale[3];
} POLYLINE;

typedef struct stackNode
{
	float data[3][3];
	struct stackNode *link;
}stackNode;

stackNode* CTM_Matrixs;

int svg_true = 0;
SVG svg;
float CTM_Matrix[3][3]={0,0,0,0,0,0,0,0,0};
float Temp_Matrix[3][3]={0,0,0,0,0,0,0,0,0};
int matrixInit =-1;
float vertex[3] = {0,0,1};
	/*float tn[3];
	float sn[3];
	float rn[3];*/


void push(float * matrix)
{
	int i,j,k =0;
	stackNode * temp_Matrixs = (stackNode *)malloc(sizeof(stackNode));
	for(i=0; i<3; i++)
	{
		for(j=0; j<3; j++)
		{
			temp_Matrixs->data[i][j] = *(matrix+k);
			k++;
		}
	}
	temp_Matrixs ->link = CTM_Matrixs;
	CTM_Matrixs = temp_Matrixs;
}

void pop()
{
	int i,j,k=0; 
	stackNode* temp_Matrixs;
	if(CTM_Matrixs == NULL)
	{
		printf("\n\n Stack is empty! \n");
	}
	else
	{
		temp_Matrixs = CTM_Matrixs;
		CTM_Matrixs = CTM_Matrixs->link;
		free(temp_Matrixs);
		if(CTM_Matrixs == NULL)
		{
			matrixInit = -1;
			for(i=0; i<3; i++)
			{
				for(j=0; j<3; j++)
				{
					CTM_Matrix[i][j] = 0;
				}
			}
		}
		else
		{
			for(i=0; i<3; i++)
			{
				for(j=0; j<3; j++)
				{
					CTM_Matrix[i][j] = CTM_Matrixs->data[i][j];
				}
			}
		}
	}
	
}

int getTag(FILE * fp, char * cp)
{
	int c;
	int a = 0;
	int b=0;
	
	while((c = getc(fp)) != EOF)
	{
		if( c =='<')
		{
			while((c = getc(fp)) != '>')
			{
				
				* (cp+a) = c;
				a++;
			}
			if( * cp == '/')  //end
			{
				pop();
				return 2;
			}
			else if( * (cp + (a-1)) =='/')	//element
			{
				return 3;
			}
			else	//start
			{
				return 1;
			}
		}
	}
	return 0;
}

int getchild(char * cp)
{
	int i=0;
	char str[10] = {""};
	strncpy(str, cp, sizeof(str));


	if(strncmp(str, "svg" ,3) == 0)
	{
		//getShapeAttr(cp);
		getSvgAttr(cp);
		svg_true = 1;
		return 1;
	}
	else if(strncmp(str, "line" ,4) == 0)
	{
		getLineAttr(cp);
		return 1;
	}
	else if(strncmp(str, "rect" ,4) == 0)
	{
		getRectAttr(cp);
		return 1;
	}
	else if(strncmp(str, "path" ,4) == 0)
	{
		getPathAttr(cp);
		return 1;
	}
	else if(strncmp(str, "circle" ,5) == 0)
	{
		getCircleAttr(cp);
		return 1;
	}
	else if(strncmp(str, "ellipse" ,6) == 0)
	{
		getEllipseAttr(cp);
		return 1;
	}
	else if(strncmp(str, "polygon" ,7) == 0)
	{
		getPolygonAttr(cp);
		return 1;
	}
	else if(strncmp(str, "polyline" ,8) == 0)
	{
		return 1;
	}

	/*while(i<strlen(str))
	{
		if(str[0] =='s' && str[1] =='v' && str[2] =='g')
		{
			getShapeAttr(cp);
			count = 1;
			return 1;
		}
		else if(str[0] =='l' && str[1] =='i' && str[2] == 'n' && str[3] == 'e')
		{
			return 1;
		}
		else if(str[0] =='r' && str[1] =='e' && str[2] == 'c' && str[3] == 't')
		{
			return 1;
		}
		else if(str[0] =='p' && str[1] =='a' && str[2] == 't' && str[3] == 'h')
		{
			return 1;
		}
		else if(str[0] =='c' && str[1] =='i' && str[2] == 'r' && str[3] == 'c' && str[4] == 'l' && str[5] == 'e')
		{
			return 1;

		}
		else if(str[0] =='e' && str[1] =='l' && str[2] == 'l' && str[3] == 'i' && str[4] == 'p' && str[5] == 's' && str[6] == 'e')
		{
			return 1;

		}
		else if(str[0] =='p' && str[1] =='o' && str[2] == 'l' && str[3] == 'y' && str[4] == 'g' && str[5] == 'o' && str[6] == 'n')
		{
			return 1;

		}
		else if(str[0] =='p' && str[1] =='o' && str[2] == 'l' && str[3] == 'y' && str[4] == 'l' && str[5] == 'i' && str[6] == 'n' && str[7] == 'e')
		{
			return 1;

		}
		i++;
	}*/

	return 0;
}

//int getShapeAttr(char * cp)
//{	
//	int i =0, j =0;
//	int k=0;
//	int num = 0;
//	int count =0;
//	char str[50][200] = {""};
//	while(k<strlen(cp))
//	{
//		if(*(cp+k) != ' ')
//		{
//			count =0;
//			str[j][num++] = * (cp+k);
//		}
//		else
//		{
//			if(count == 0)
//			{
//				j++;
//				num=0;
//			}
//			count = 1;
//			
//			
//		}
//		k++;
//	}
//	//strncpy(str, cp, sizeof(str));
//	while(i<50)
//	{
//		if(cp[i] == 'w' && cp[i+1] =='i' && cp[i+2] =='d' && cp[i+3] =='t' && cp[i+4] =='h' && cp[i+5] =='=')
//		{
//			getAttrValue(cp);
//
//		}
//		i++;
//	}
//	return 0;
//}


int getSvgAttr(char * cp)
{
	int i =0, j =0;
	int k=0;
	int num = 0;
	int count =0, cou = 0;
	char str[50][200] = {""};
	char viewbox[4][4] = {""};
	//SVG svg;

	while(k<strlen(cp))
	{
		if(*(cp+k) == ' ' && cou == 2)
		{
			if(count == 0)
			{
				j++;
				num=0;
				cou=0;
			}
			count = 1;
		}
		else
		{
			if(*(cp+k) == '"')
			{
				cou++;
			}
			count =0;
			str[j][num++] = * (cp+k);
	
		}
		k++;
	}

	while(i<=j)
	{
		k=0;
		count = 0;
		num= 0;
		if(strstr(str[i],"width=") != NULL)
		{
			svg.width = getAttrValue(str[i]);
			printf("%d",svg.width);
		}
		else if(strstr(str[i],"height=") != NULL)
		{
			svg.height=  getAttrValue(str[i]);
		}
		else if(strstr(str[i],"id=") != NULL)
		{
			
		}
		else if(strstr(str[i],"viewBox=") != NULL)
		{
			for(j=0; j<strlen(str[i]); j++)
			{

				if(str[i][j]>='0' && str[i][j]<='9')
				{
					viewbox[num][k]= str[i][j];
					k++;
					
				}
				else if(str[i][j] ==' ')
				{
					svg.viewbox[num] = getAttrValue(viewbox[num]);
					num++;
					k=0;
				}
				else
				{
					if(str[i][j] == '"')
					{
						count++;
					}
					if(count == 2)
					{
						svg.viewbox[num]= getAttrValue(viewbox[num]);
					}
				}

			}
			
		}
		else if(strstr(str[i],"transform=") != NULL)
		{
			getTransform(str[i]);
			/*for(k=0; k<3; k++)
			{
				svg.tranlate[k] = tn[k];
				svg.scale[k] = sn[k];
				svg.rotate[k] = rn[k];
			}*/
		}

		i++;
	}

	
}

int getLineAttr(char * cp)
{
	int i =0, j =0;
	int k=0;
	int num = 0;
	int count =0, cou = 0;
	char str[50][200] = {""};
	LINE li;
	//int color;

	while(k<strlen(cp))
	{
		if((*(cp+k) == ' ' && cou == 2) || cou == 2)
		{
			if(count == 0)
			{
				j++;
				num=0;
				cou=0;
			}
			count = 1;
		}
		else
		{
			if(*(cp+k) == '"')
			{
				cou++;
			}
			count =0;
			str[j][num++] = * (cp+k);
	
		}
		k++;
	}

	while(i<=j)
	{
		k=0;
		count = 0;
		num= 0;
		if(strstr(str[i],"x1=") != NULL)
		{
			li.x1 = getAttrValue(str[i]);
		}
		else if(strstr(str[i],"y1=") != NULL)
		{
			li.y1 = getAttrValue(str[i]);
		}
		else if(strstr(str[i],"x2=") != NULL)
		{
			li.x2 = getAttrValue(str[i]);
		}
		else if(strstr(str[i],"y2=") != NULL)
		{
			li.y2 = getAttrValue(str[i]);
		}

		i++;
	}

	//color = BLACK;
	//setcolor(color);

	line(li.x1,li.y1, li.x2, li.y2); // 직선 

	/*

   * void far setfillstyle(int pattern,int color)

   * pattern : [1 이면 빈틈없이 채움]

   * color   : 칼러 

  */
	//setfillstyle(1, BLACK);
	
	return 0;
}

int getRectAttr(char * cp)
{
	int i =0, j =0;
	int k=0;
	int num = 0;
	int count =0, cou = 0;
	char str[50][200] = {""};
	RECT re;
	float x,y,x1,y1,x2,y2,x3,y3;
	
	/*float tx,ty,tx1,ty1,tx2,ty2,tx3,ty3;
	float cx,cy,cx1,cy1,cx2,cy2,cx3,cy3;
	float sx,sy,sx1,sy1,sx2,sy2,sx3,sy3;
	float t;*/
	

	while(k<strlen(cp))
	{
		if((*(cp+k) == ' ' && cou == 2) || cou == 2)
		{
			if(count == 0)
			{
				j++;
				num=0;
				cou=0;
			}
			count = 1;
		}
		else
		{
			if(*(cp+k) == '"')
			{
				cou++;
			}
			count =0;
			str[j][num++] = * (cp+k);
	
		}
		k++;
	}

	while(i<=j)
	{
		k=0;
		count = 0;
		num= 0;
		if(strstr(str[i],"x=") != NULL)
		{
			re.x = getAttrValue(str[i]);
		}
		else if(strstr(str[i],"y=") != NULL)
		{
			re.y = getAttrValue(str[i]);
		}
		else if(strstr(str[i],"width=") != NULL)
		{
			re.width = getAttrValue(str[i]);
		}
		else if(strstr(str[i],"height=") != NULL)
		{
			re.height = getAttrValue(str[i]);
		}
		else if(strstr(str[i],"rx=") != NULL)
		{
			re.rx = getAttrValue(str[i]);
		}
		else if(strstr(str[i],"ry=") != NULL)
		{
			re.ry = getAttrValue(str[i]);
		}
		else if(strstr(str[i],"transform=") != NULL)
		{
			getTransform(str[i]);
			/*for(k=0; k<3; k++)
			{
				re.tranlate[k] = tn[k];
				if(sn[k] == 0 )
					re.scale[k] = 1;
				else
					re.scale[k] = sn[k];
				re.rotate[k] = rn[k];
			}*/
		}
	
		i++; 
	}

	//color = RED;
	//setcolor(color);
	printf("\n%d",svg.height);

	line(re.x,re.y,re.x+re.width,re.y);
	line(re.x,re.y,re.x,re.y+re.height);
	line(re.x+re.width,re.y,re.x+re.width,re.y+re.height);
	line(re.x,re.y+re.height,re.x+re.width,re.y+re.height);


	x= re.x;
	y= re.y;
	x1 = re.x+re.width;
	y1 = re.y;
	x2 = re.x;
	y2 = re.y+re.height;
	x3 = re.x+re.width;
	y3 = re.y+re.height;

	tranformMt(&x,&y);
	x = vertex[0]; 
	y = vertex[1];
	tranformMt(&x1,&y1);
	x1 = vertex[0]; 
	y1 = vertex[1];
	tranformMt(&x2,&y2);
	x2 = vertex[0]; 
	y2= vertex[1];
	tranformMt(&x3,&y3);
	x3 = vertex[0]; 
	y3 = vertex[1];

	line(x,y,x1,y1);
	line(x,y,x2,y2);
	line(x1,y1,x3,y3);
	line(x2,y2,x3,y3);

	
	
	//x= re.x+re.tranlate[0];
	//y= re.y+re.tranlate[1];
	//x1 = (re.x+re.width) + re.tranlate[0];
	//y1 = (re.y) + re.tranlate[1];
	//x2 = (re.x) + re.tranlate[0];
	//y2 = (re.y+re.height) + re.tranlate[1];
	//x3 = (re.x+re.width) + re.tranlate[0];
	//y3 = (re.y+re.height) + re.tranlate[1];

	//sx= x + re.scale[0];
	//sy= y + re.scale[1];
	//sx1 = x1 + re.scale[0];
	//sy1 = y1 + re.scale[1];
	//sx2 = x2 + re.scale[0];
	//sy2 = y2 + re.scale[1];
	//sx3 = x3 + re.scale[0];
	//sy3 = y3 + re.scale[1];

	//t= 3.14*re.rotate[0]/180;
	//cx = cos(t) * (sx - re.rotate[1]) - sin(t) * (sy - re.rotate[2]);
	//cy = sin(t) * (sx - re.rotate[1]) + cos(t) * (sy - re.rotate[2]);
	//cx+=re.rotate[1];
	//cy+=re.rotate[2];
	//cx1 = cos(t) * (sx1 - re.rotate[1]) - sin(t) * (sy1 - re.rotate[2]);
	//cy1 = sin(t) * (sx1 - re.rotate[1]) + cos(t) * (sy1 - re.rotate[2]);
	//cx1+=re.rotate[1];
	//cy1+=re.rotate[2];
	//cx2 = cos(t) * (sx2 - re.rotate[1]) - sin(t) * (sy2 - re.rotate[2]);
	//cy2 = sin(t) * (sx2 - re.rotate[1]) + cos(t) * (sy2 - re.rotate[2]);
	//cx2+=re.rotate[1];
	//cy2+=re.rotate[2];
	//cx3 = cos(t) * (sx3 - re.rotate[1]) - sin(t) * (sy3 - re.rotate[2]);
	//cy3 = sin(t) * (sx3 - re.rotate[1]) + cos(t) * (sy3 - re.rotate[2]);
	//cx3+=re.rotate[1];
	//cy3+=re.rotate[2];

	////sx = cx * re.scale[0];
	////sy = cy * re.scale[1];
	////sx1 = cx1 * re.scale[0];
	////sy1 = cy1 * re.scale[1];
	////sx2 = cx2 * re.scale[0];
	////sy2 = cy2 * re.scale[1];
	////sx3 = cx3 * re.scale[0];
	////sy3 = cy3 * re.scale[1];

	//tx= cx + re.tranlate[0];
	//ty= cy + re.tranlate[1];
	//tx1 = cx1 + re.tranlate[0];
	//ty1 = cy1 + re.tranlate[1];
	//tx2 = cx2 + re.tranlate[0];
	//ty2 = cy2 + re.tranlate[1];
	//tx3 = cx3 + re.tranlate[0];
	//ty3 = cy3 + re.tranlate[1];

	///*line(sx,sy,sx1,sy1);
	//line(sx,sy,sx2,sy2);
	//line(sx1,sy1,sx3,sy3);
	//line(sx2,sy2,sx3,sy3);*/
	//line(tx,ty,tx1,ty1);
	//line(tx,ty,tx2,ty2);
	//line(tx1,ty1,tx3,ty3);
	//line(tx2,ty2,tx3,ty3);
	//rectangle(x,y,x+re.width,y+re.height);
	/*setfillstyle( SOLID_FILL, RED );
	floodfill(x+1,y+1 ,WHITE); */

	/*

   * void far setfillstyle(int pattern,int color)

   * pattern : [1 이면 빈틈없이 채움]

   * color   : 칼러 

  */
	//setfillstyle(1, RED);
	pop();
	return 0;
}

int getCircleAttr(char * cp)
{
	
	int i =0, j =0;
	int k=0;
	int num = 0;
	int count =0, cou = 0;
	char str[50][200] = {""};
	CIRCLE ci;

	while(k<strlen(cp))
	{
		if(*(cp+k) == ' ' && cou == 2)
		{
			if(count == 0)
			{
				j++;
				num=0;
				cou=0;
			}
			count = 1;
		}
		else
		{
			if(*(cp+k) == '"')
			{
				cou++;
			}
			count =0;
			str[j][num++] = * (cp+k);
	
		}
		k++;
	}

	while(i<=j)
	{
		k=0;
		count = 0;
		num= 0;
		if(strstr(str[i],"cx=") != NULL)
		{
			ci.cx = getAttrValue(str[i]);
		}
		else if(strstr(str[i],"cy=") != NULL)
		{
			ci.cy = getAttrValue(str[i]);
		}
		else if(strstr(str[i],"r=") != NULL)
		{
			ci.r = getAttrValue(str[i]);
		}
		else if(strstr(str[i],"transform=") != NULL)
		{
			getTransform(str[i]);
			/*for(k=0; k<3; k++)
			{
				ci.tranlate[k] = tn[k];
				ci.scale[k] = sn[k];
				ci.rotate[k] = rn[k];
			}*/
		}

		i++;
	}

	circle(ci.cx,ci.cy,ci.r);
	//setfillstyle( SOLID_FILL, BLUE );
	//floodfill(ci.cx+1,ci.cy+1 ,WHITE); 
	return 0;
}

int getEllipseAttr(char * cp)
{
	
	int i =0, j =0;
	int k=0;
	int num = 0;
	int count =0, cou = 0;
	char str[50][200] = {""};
	ELLIPSE el;
	
	while(k<strlen(cp))
	{
		if(*(cp+k) == ' ' && cou == 2)
		{
			if(count == 0)
			{
				j++;
				num=0;
				cou=0;
			}
			count = 1;
		}
		else
		{
			if(*(cp+k) == '"')
			{
				cou++;
			}
			count =0;
			str[j][num++] = * (cp+k);
	
		}
		k++;
	}

	while(i<=j)
	{
		k=0;
		count = 0;
		num= 0;
		if(strstr(str[i],"cx=") != NULL)
		{
			el.cx = getAttrValue(str[i]);
		}
		else if(strstr(str[i],"cy=") != NULL)
		{
			el.cy = getAttrValue(str[i]);
		}
		else if(strstr(str[i],"rx=") != NULL )
		{
			el.rx = getAttrValue(str[i]);
		}
		else if(strstr(str[i],"ry=") != NULL)
		{
			el.ry =getAttrValue(str[i]);
		}

		i++;
	}

	ellipse(el.cx,el.cy,0,360,el.rx,el.ry);
	//setfillstyle( SOLID_FILL, GREEN );
	//floodfill(el.cx+1,el.cy+1 ,WHITE); 
	return 0;
}

int getPathAttr(char * cp)
{
	
	int i =0, j =0;
	int k=0, l=0;
	int num = 0;
	int count =0, cou = 0;
	char str[50][200] = {""};
	char str2[50][10] = {""};
	char str3[50][10] = {""};
	char * tok;
	char * tok1;
	PATH pa;

	while(k<strlen(cp))
	{
		if(*(cp+k) == ' ' && cou == 2)
		{
			if(count == 0)
			{
				j++;
				num=0;
				cou=0;
			}
			count = 1;
		}
		else
		{
			if(*(cp+k) == '"')
			{
				cou++;
			}
			count =0;
			str[j][num++] = * (cp+k);
	
		}
		k++;
	}

	while(i<=j)
	{
		k=0;
		count = 0;
		cou=0;
		if(strstr(str[i],"d=") != NULL)
		{
			tok = strtok(str[i]," \"");
			printf("%s",tok);
			while(tok != NULL)
			{
				tok = strtok(NULL," \"");
				if(tok == NULL)
				{
					break;
				}
				strcpy(str2[k],tok);
				if(strstr(str2[k],"M") != NULL || strstr(str2[k],"m") != NULL)
				{
					pa.flow[cou] = str2[k][0];
					strcpy(str3[l],str2[k]);
					tok1 = strtok(str3[l],",");
					printf("%s",tok1);
					strcpy(str3[l],tok1);
				
					if(strspn(str3[l],"0123456789. \r\t\n")>0)
					{
							if(l%2 ==0)
								pa.M[count].xpos =  getAttrValue(str3[l]);
							else
							{
								pa.M[count].ypos =  getAttrValue(str3[l]);
								num = count++;
							}
				
					}
					l++;
					while(tok1 != NULL)
					{
						tok1 = strtok(NULL,",");
						printf("%s",tok1);
						if(tok1 == NULL)
						{
							break;
						}
						strcpy(str3[l],tok1);
				
						num = strspn(str3[l],"0123456789. \r\t\n");
						if(strspn(str3[l],"0123456789. \r\t\n")>0)
						{
							if(l%2 ==0)
								pa.M[count].xpos =  getAttrValue(str3[l]);
							else
							{
								pa.M[count].ypos =  getAttrValue(str3[l]);
								num = count++;
							}
				
						}
						l++;
					}
					cou++;
				}
				
				k++;
			}
		}

		i++;
		
	}
	return 0;
}

int getPolygonAttr(char * cp)
{
	
	int i =0, j =0;
	int k=0;
	int num = 0;
	int count =0, cou = 0;
	char str[50][200] = {""};
	char str2[50][10] = {""};
	int polypoint[20];
	char * tok;
	POLYGON pg;

	while(k<strlen(cp))
	{
		if(*(cp+k) == ' ' && cou == 2)
		{
			if(count == 0)
			{
				j++;
				num=0;
				cou=0;
			}
			count = 1;
		}
		else
		{
			if(*(cp+k) == '"')
			{
				cou++;
			}
			count =0;
			str[j][num++] = * (cp+k);
	
		}
		k++;
	}

	num = 0;
	while(i<=j)
	{
		k=0;
		count =0;
		
		if(strstr(str[i],"points=") != NULL)
		{
			tok = strtok(str[i]," ,\"");
			printf("%s",tok);
			while(tok != NULL)
			{
				tok = strtok(NULL," ,\"");
				printf("%s",tok);
				if(strstr(tok,"/") != NULL)
				{
					break;
				}
				strcpy(str2[k],tok);
				
				num = strspn(str2[k],"0123456789. \r\t\n");
				if(strspn(str2[k],"0123456789. \r\t\n")>0)
				{
					if(k%2 ==0)
						pg.pos[count].xpos = getAttrValue(str2[k]);
					else
					{
						pg.pos[count].ypos = getAttrValue(str2[k]);
						num = count++;
						
					}
				}
				k++;
			}
		}
		i++;
	}

	count=0;
	//for(j=0; j<=num; j++)
	//{
	//	/*if(j >= num)
	//	{
	//		line(pg.pos[j].xpos,pg.pos[j].ypos,pg.pos[0].xpos,pg.pos[0].ypos);
	//		break;
	//	}
	//	else
	//	{
	//		line(pg.pos[j].xpos,pg.pos[j].ypos,pg.pos[j+1].xpos,pg.pos[j+1].ypos);
	//	}*/
	//	
	//	
	//}
	for(j=0; j<= num+1; j++)
	{
		if(j < num+1)
		{
			polypoint[count++] = pg.pos[j].xpos;
			polypoint[count++] = pg.pos[j].ypos;
		}
		else
		{
			polypoint[count++] = polypoint[0];
			polypoint[count] = polypoint[1];
		}
	}
	drawpoly(count/2+1,polypoint);
	//setfillstyle( SOLID_FILL, GREEN );
	//fillpoly(count/2+1,polypoint);
	
	return 0;
} 

int getPolylineAttr(char * cp)
{
	
	int i =0, j =0;
	int k=0;
	int num = 0;
	int count =0, cou = 0;
	char str[50][200] = {""};
	char str2[50][10] = {""};
	int polypoint[20];
	char * tok;
	POLYLINE pl;

	while(k<strlen(cp))
	{
		if(*(cp+k) == ' ' && cou == 2)
		{
			if(count == 0)
			{
				j++;
				num=0;
				cou=0;
			}
			count = 1;
		}
		else
		{
			if(*(cp+k) == '"')
			{
				cou++;
			}
			count =0;
			str[j][num++] = * (cp+k);
	
		}
		k++;
	}

	while(i<=j)
	{
		k=0;
		count = 0;
		num= 0;
		if(strstr(str[i],"points=") != NULL)
		{
			tok = strtok(str[i]," ,\"");
			printf("%s",tok);
			while(tok != NULL)
			{
				tok = strtok(NULL," ,\"");
				printf("%s",tok);
				if(strstr(tok,"/") != NULL)
				{
					break;
				}
				strcpy(str2[k],tok);
				
				num = strspn(str2[k],"0123456789. \r\t\n");
				if(strspn(str2[k],"0123456789. \r\t\n")>0)
				{
					if(k%2 ==0)
						pl.pos[count].xpos = getAttrValue(str2[k]);
					else
					{
						pl.pos[count].ypos = getAttrValue(str2[k]);
						num = count++;
						
					}
				}
				k++;
			}
		}
		i++;
	}

	count=0;
	//for(j=0; j<num; j++)
	//{

	//	line(pg.pos[j].xpos,pg.pos[j].ypos,pg.pos[j+1].xpos,pg.pos[j+1].ypos);

	//}
	for(j=0; j<= num+1; j++)
	{
		if(j < num+1)
		{
			polypoint[count++] = pl.pos[j].xpos;
			polypoint[count++] = pl.pos[j].ypos;
		}
		
	}
	drawpoly(count/2,polypoint);
	setfillstyle( SOLID_FILL, GREEN );
	fillpoly(count/2,polypoint);
} 


int getAttrValue(char * cp)	
{
	int j = 0;
	int k = 0;
	int cou = 0;
	int num =0;
	char str[10] = {""};

	if(strstr(cp,"\"") != NULL)
	{
		num =1;
	}
	else 
	{
		num =0;
	}

	while(j<strlen(cp))
	{

		//if(cp[j] != '"' && cp[j] != '=')  
		//{
		//			
		//	str[k]= cp[j];
		//	if(str[k]>='0' && str[k]<='9')
		//	{
		//		num = num*10+str[k]-'0'; //문자열 숫자를 int형 숫자로 변환
		//	}
		//	k++;
		//}
		//else
		//{
		//	if(cp[j] == '"')
		//	{
		//		cou++;
		//		if(cou == 2)
		//		{
		//			break;
		//		}
		//		
		//	
		//	}
		//	
		//} 
		if(num ==1)
			if(cp[j]>='0' && cp[j]<='9' && cou >0)
			{
				str[k++] = cp[j];
			}
			else
			{
				if(cp[j] == '"')
				{
					cou++;
				}
				if(cp[j] == '.')
				{
					break;
				}
			}
		
		else
		{
			
			if(cp[j]>='0' && cp[j]<='9')
			{
				str[k++] = cp[j];
			}
			else
			{
				if(cp[j] == '"')
				{
					cou++;
				}
				if(cp[j] == '.')
				{
					break;
				}
			}
		}
		j++;		
	}

	num = atoi(str);
	return num;
}


int getTransform(char * cp)
{
	int i = 0;
	int j = 0;
	int k = 0;
	int cou = 0;
	int num =0;
	char * tok = NULL;
	char * tok2 = NULL;
	char str[5][50] = {""};
	char tm[3][10] = {""};
	float tn[3];
	float sn[3];
	float rn[3];
	float tx,ty=0;
	int r =0;
	float rx,ry =0;
	float sx,sy =0;

	for(i=0; i<3; i++)
	{
		tn[i]=0;
		sn[i]=0;
		rn[i]=0;
	}
	
	tok = strtok(cp,"\"");
	while(tok != NULL)
	{
		tok = strtok(NULL," \"");
		if(tok == NULL)
			break;
		//printf("%s",tok);
		strcpy(str[k],tok);		
		
		k++;
	}

	num=0;
	while(num<k)
	{
		if(strstr(str[num],"translate") != NULL)
		{
				j=0;
				cou =0;
				tok2 = strtok(str[num],"(,)");
				while(tok2 != NULL)
				{
					tok2 = strtok(NULL,"(,)");
					if(tok2 == NULL)
						break;
					//printf("%s",tok2);
					strcpy(tm[j],tok2);
					j++;
				}
				for(i=0; i<j; i++)
				{
					if(getAttrValue(tm[i])!= NULL )
					{
						tn[cou]= getAttrValue(tm[i]);
						cou++;
					}
				}

				tx= tn[0];
				ty= tn[1];
				translationMt(&tx,&ty);
			}
			else if(strstr(str[num],"rotate") != NULL)
			{
				j=0;
				cou =0;
				tok2 = strtok(str[num],"(,)");
				while(tok2 != NULL)
				{
					tok2 = strtok(NULL,"(,)");
					if(tok2 == NULL)
						break;
					//printf("%s",tok2);
					strcpy(tm[j],tok2);
					j++;
				}
				for(i=0; i<j; i++)
				{
					if(getAttrValue(tm[i])!= NULL )
					{
						rn[cou]= getAttrValue(tm[i]);
						cou++;
					}
				}
				r = rn[0];
				rx = rn[1];
				ry = rn[2];
				if(rx == 0 && ry==0)
				{
					roatateMt(&r);
				}
				else
				{
					translationMt(&rx,&ry);
					roatateMt(&r);
					rx = -rx;
					ry= -ry;
					translationMt(&rx,&ry);
				}

			}
			else if(strstr(str[num],"scale") != NULL)
			{
				j=0;
				cou =0;
				tok2 = strtok(str[num],"(,)");
				while(tok2 != NULL)
				{
					tok2 = strtok(NULL,"(,)");
					if(tok2 == NULL)
						break;
					//printf("%s",tok2);
					strcpy(tm[j],tok2);
					j++;
				}
				for(i=0; i<j; i++)
				{
					if(getAttrValue(tm[i])!= NULL )
					{
						sn[cou]= getAttrValue(tm[i]);
						cou++;
					}
				}
				sx = sn[0];
				sy = sn[1];
				if(sy ==0 && cou <2 )
				{
					sy = sn[0];
				}
				scaleMt(&sx,&sy);
			}
		num++;
	}

	push(CTM_Matrix[0]);

	return 0;
}

int translationMt(float *tx, float *ty)
{
	int i,j,k;
	float x=0,y=0;
	float TmMatrix[3][3] ={1,0,1,0,1,1,0,0,1};
	float sum = 0;
	x = *tx;
	y = *ty;
	
	TmMatrix[0][2] = x;
	TmMatrix[1][2]= y;
	//float temp =0;

	if(matrixInit == -1)
	{
		matrixInit = 0;
		for(i=0; i<3; i++)
		{
			for(j=0; j<3; j++)
			{
				Temp_Matrix[i][j] = CTM_Matrix[i][j];
				CTM_Matrix[i][j] = TmMatrix[i][j];
			}

		}
		
	}
	else
	{
		for(i=0; i<3; i++)
		{
			for(j=0; j<3; j++)
			{
				Temp_Matrix[i][j] = CTM_Matrix[i][j];
			}

		}
		for(i=0; i<3; i++)
		{
			for(j=0; j<3; j++)
			{
				sum =0;
				for(k=0; k<3; k++)
				{
					sum += Temp_Matrix[i][k] * TmMatrix[k][j];
				}
				CTM_Matrix[i][j] = sum;
				printf("\n%f\t", CTM_Matrix[i][j]);
			}
		}
		
	}
	

	return 0;
}

int roatateMt(int *a)
{
	int i,j,k;
	int theta=0;
	float deg =0;
	float RmMatrix[3][3] ={1,1,0,1,1,0,0,0,1};
	float sum = 0;
	theta = *a;
	deg = theta*3.1428/180;
	RmMatrix[0][0] = cos(deg);
	RmMatrix[0][1]= -sin(deg);
	RmMatrix[1][0] = sin(deg);
	RmMatrix[1][1] = cos(deg);
	//float temp =0;

	if(matrixInit == -1)
	{
		matrixInit = 0;
		for(i=0; i<3; i++)
		{
			for(j=0; j<3; j++)
			{
				Temp_Matrix[i][j] = CTM_Matrix[i][j];
				CTM_Matrix[i][j] = RmMatrix[i][j];
			}

		}
	
	}
	else
	{
		for(i=0; i<3; i++)
		{
			for(j=0; j<3; j++)
			{
				Temp_Matrix[i][j] = CTM_Matrix[i][j];
			}

		}
		for(i=0; i<3; i++)
		{
			for(j=0; j<3; j++)
			{
				
				sum =0;
				for(k=0; k<3; k++)
				{
					sum += Temp_Matrix[i][k] * RmMatrix[k][j];
					
				}
				CTM_Matrix[i][j] = sum;
				printf("\n%f\t", CTM_Matrix[i][j]);
			}
		}
		
	}
	return 0;
}

int scaleMt(float *sx, float *sy)
{
	int i,j,k;
	float x=0,y=0;
	float SmMatrix[3][3] ={1,0,0,0,1,0,0,0,1};
	float sum = 0;
	x = *sx;
	y = *sy;
	
	SmMatrix[0][0] = x;
	SmMatrix[1][1]= y;
	//float temp =0;

	if(matrixInit == -1)
	{
		matrixInit = 0;
		for(i=0; i<3; i++)
		{
			for(j=0; j<3; j++)
			{
				Temp_Matrix[i][j] = CTM_Matrix[i][j];
				CTM_Matrix[i][j] = SmMatrix[i][j];
			}

		}
	}
	else
	{
		for(i=0; i<3; i++)
		{
			for(j=0; j<3; j++)
			{
				Temp_Matrix[i][j] = CTM_Matrix[i][j];
			}

		}
		for(i=0; i<3; i++)
		{
			for(j=0; j<3; j++)
			{
				sum =0;
				for(k=0; k<3; k++)
				{
					sum += Temp_Matrix[i][k] * SmMatrix[k][j];
				}
				CTM_Matrix[i][j] = sum;
				printf("\n%f\t", CTM_Matrix[i][j]);
			}
		}
	}
	

	return 0;
}

int tranformMt(float *vx, float *vy)
{
	int i,j=0;
	float x,y=0;
	float VertexMt[3] = {0,0,1};
	float sum = 0;
	x= *vx;
	y= *vy;
	VertexMt[0] = x;
	VertexMt[1] = y;
	for(i=0; i<3; i++)
	{
		sum=0;
		for(j=0; j<3; j++)
		{
			sum+=CTM_Matrix[i][j] * VertexMt[j];
		}
		vertex[i] = sum;
	}
	return 0;
}

int main(void)
{

	
	int i =0, j=0, k=0;
	int c;
	int temp =0, num =0 ;
	char str[10000] ={""};
	//char start[3] = {""};
	//FILE * fp = fopen("D:\\test\\car.svg","rt");
	FILE * fp = fopen("D:\\test\\path.svg","rt");
	//FILE * wp = fopen("D:\\test\\car.txt","wt");
	FILE * wp = fopen("D:\\test\\path.txt","wt");

	int gdr = DETECT, gmode;

	initgraph(&gdr,&gmode,""); // 다만 *gdriver, *gmode 선언하면 winbgi 에서는 안됨.
	cleardevice();
	setbkcolor(WHITE);
	if(fp == NULL)
	{
		puts("파일 오픈 실패!");
		return -1;
	}

	CTM_Matrixs = NULL;

	while( (c= getTag(fp, str)) != 0)
	{
		k=0;
		i++;

		getchild(str);
		
	
		if(svg_true)
		{
			switch(c)
			{
				case 1:

					if(temp == 2 || temp ==3)
					{
						temp =1;
					}
					else
					{
						j++;
						temp =1;
					}
					break;
				case 2:
					j--;
					temp = 2;
					break;
				case 3:
					if(c != temp)
					{
						if(temp ==2)
						{
							temp=3;
							break;
						}
						temp = 3;
						j++;
						//printf("%d\n",line.x1);
					}
					break;
			}
		}
		fprintf(wp,"%4d %4d %4d: %s \n",i,j,c,str);
		for(num = 0; num<10000; num++)
		{
			str[num] = '\0';
		}
		
	}

	
	
	fclose(fp);
	fclose(wp);
	

	getch();
    closegraph(); 
	return 0;
}