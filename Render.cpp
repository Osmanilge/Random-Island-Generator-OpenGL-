// Sphere approximation by recursive subdivision of a tetrahedron

#include "Angel.h"
#include <time.h>
int n;
double aci=0,aci2=0;
typedef Angel::vec4  color4;
typedef Angel::vec4  point4;

int viewMode;
#define N  4  // number of subdivisions
#define M 512  // number of resulting points

GLsizei w=512, h=512;
double speed=1.0;
double scale=1/14.0;
//point4 data[M];
point4 data[M+2][M+1];
color4 colors[M+2][M+1];
vec4 normal[M+2][M+1];
int dataT[M+1][M+1];
void fill(int x1,int y1,int x2, int y2);

point4 v[4]= {vec4(0.0, 0.0, 1.0, 1.0), vec4(0.0, 0.942809, -0.333333, 1.0),
                vec4(-0.816497, -0.471405, -0.333333, 1.0),
               vec4(0.816497, -0.471405, -0.333333, 1.0)};

float theta = 0.0;
float phi = 0.0;
float radius = 4.0;
int mode =0;
GLuint program;

GLuint buffers[1];
GLuint loc ,loc2,loc3;
GLint matrix_loc, projection_loc, plane_loc;

static int k =0;

point4 at = vec4(0.0, 1.0, 0.5, 1.0);
point4 eye = vec4(0.0, -2.0, 0.7, 1.0);
vec4 up = vec4(0.0, 0.0, 1.0, 0.0);

GLfloat left= -2.0, right=2.0, top=2.0, bottom= -2.0, zNear= 0.001, zFar=100;

mat4 model_view;
mat4 projection;
mat4 Plane;
double planex=0.0,planey=0.0,planez=0.0;
vec4 fix;


/* OpenGL initialization */
void calcGNormal1(int x,int y);
void calcFNormal1(int x,int y);
void filler();
void init()
{

   //tetrahedron(N);
   	double range=30.0;
	for(int i=0;i<=M;i+=M/2)
		for(int j=0;j<=M;j+=M/2)
			data[i][j]=vec4((j-M/2)*(range/M) ,(i-M/2)*(range/M) ,(rand()%10)/10.0,1.0);
	//data[M/2][M/2].z=6.0;
	for(int i=0;i<M+1;i++)
		for(int j=0;j<=M;j++)dataT[i][j]=1;
	fill(0,0,M/2,M/2);
	fill(M/2,0,M,M/2);
	fill(0,M/2,M/2,M);
	fill(M/2,M/2,M,M);
	for(int i=0;i<=M+1;i++)
		for(int j=0;j<=M;j++)normal[i][j]=vec4(0,0,0,0);
	
	calcFNormal1(0,0);
	filler();
	
	
    // Create a vertex array object

    GLuint vao;
    glGenVertexArrays( 1, &vao );
    glBindVertexArray( vao );

    // Create and initialize a buffer object

/* set up vertex buffer object */

   glGenBuffers(1, buffers);
   glBindBuffer(GL_ARRAY_BUFFER, buffers[0]);
   //glBufferData(GL_ARRAY_BUFFER, sizeof(data), data, GL_STATIC_DRAW);
   glBufferData(GL_ARRAY_BUFFER, sizeof(data)+sizeof(colors)+sizeof(normal), NULL, GL_STATIC_DRAW);

    // Load shaders and use the resulting shader program

    program = InitShader("vshader51.glsl", "fshader51.glsl");
    glUseProgram(program);

   loc = glGetAttribLocation(program, "vPosition");
   
   glEnableVertexAttribArray(loc);
   glVertexAttribPointer(loc, 4, GL_FLOAT, GL_FALSE, 0, 0);
   //newwwww
   loc3 = glGetAttribLocation(program, "vNormal");
   glEnableVertexAttribArray(loc3);
   glVertexAttribPointer(loc3, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(sizeof(data)+ sizeof(colors)));
   
	
   //newwwww
   loc2 = glGetAttribLocation(program, "vColor");
   glEnableVertexAttribArray(loc2);
   glVertexAttribPointer(loc2, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(sizeof(data)));
   data[M+1][0]=vec4(4.0,4.0,6.0,1.0);
   data[M+1][1]=vec4(0.0,2.0,6.0,1.0);
   data[M+1][2]=vec4(2.0,0.0,6.0,1.0);
   fix=vec4(1.0,0.0,2.0,1.0);
   fix.x=(2.0);
   fix.y=(2.0);
   fix.z=6.0;
   
   
   colors[M+1][0]=vec4(0.0,0.0,0.0,1.0);
   colors[M+1][1]=vec4(0.0,0.0,0.0,1.0);
   colors[M+1][2]=vec4(0.0,0.0,0.0,1.0);
   glBufferSubData( GL_ARRAY_BUFFER, 0, sizeof(data), data );
   glBufferSubData( GL_ARRAY_BUFFER, sizeof(data), sizeof(colors), colors );
   glBufferSubData( GL_ARRAY_BUFFER, sizeof(data)+ sizeof(colors),sizeof(normal), normal );
   matrix_loc = glGetUniformLocation(program, "model_view");
   projection_loc = glGetUniformLocation(program, "projection");
   plane_loc =	glGetUniformLocation(program, "plane");
	point4 light_position( 2.0, 3.0, 9.0, 1.0 ); //wrt camera
    color4 light_ambient( 0.2, 0.2, 0.2, 1.0 );
    color4 light_diffuse( 1.0, 1.0, 1.0, 1.0 );
    color4 light_specular( 1.0, 1.0, 1.0, 1.0 );

	//orange
 /*   color4 material_ambient( 1.0, 1.0, 1.0, 1.0 );
    color4 material_diffuse( 1.0, 0.5, 0.0, 1.0 );
    color4 material_specular( 1.0, 1.0, 1.0, 1.0 );*/

    
	//emerald
	color4 material_ambient( 0.633, 0.633, 0.633, 1.0);
    color4 material_diffuse( 0.07568, 0.07568, 0.07568, 1.0 );
    color4 material_specular(0.0215, 0.0215, 0.0215 , 1.0 ); 
    float  material_shininess = 960.8;
//	vec3 Ks = vec3(0.0215, 0.1745, 0.0215); 
//vec3 Kd = vec3(0.07568, 0.61424, 0.07568); 
//vec3 Ka = vec3(0.633, 0.727811, 0.633); 
//float specular_exponent = 76.8; 
   
	
	color4 ambient_product = light_ambient * material_ambient;
    color4 diffuse_product = light_diffuse * material_diffuse;
    color4 specular_product = light_specular * material_specular; 

    glUniform4fv( glGetUniformLocation(program, "AmbientProduct"),  1, ambient_product );
   glUniform4fv( glGetUniformLocation(program, "DiffuseProduct"),  1, diffuse_product );
    glUniform4fv( glGetUniformLocation(program, "SpecularProduct"),  1, specular_product );
	
    glUniform4fv( glGetUniformLocation(program, "LightPosition"), 1, light_position );

    glUniform1f( glGetUniformLocation(program, "Shininess"), material_shininess );
		 
    
    
    glEnable( GL_DEPTH_TEST );
	
   

    

   glClearColor(1.0, 1.0, 1.0, 1.0); /* white background */
}
vec4 triangleNormal(point4 p1,point4 p2,point4 p3){
	vec4 U=p1-p2;
	vec4 V=p3-p1;
	return cross(U,V);
}
void calcGNormal2(int x,int y){	
	if(x>=M-2 || y>=M-2)return;
	if(x==0||y==0){
		normal[x][y]=vec4(0,0.8,0.6,0);
		calcGNormal2(x,y+1);
		return;
	}
	point4 p01=data[x-1][y];
	point4 p0=data[x][y-1];
	point4 p1=data[x][y];
	point4 p2=data[x+1][y];
	point4 p3=data[x][y+1];
	normal[x][y]=vec4(0,0,0,0);
	normal[x][y]+=triangleNormal(p1,p01,p0)/4.0;
	normal[x][y]+=triangleNormal(p1,p01,p3)/4.0;
	normal[x][y]+=triangleNormal(p1,p2,p3)/4.0;
	normal[x][y]+=triangleNormal(p1,p2,p0)/4.0;
	normal[x][y]=normalize(normal[x][y]);
	calcGNormal2(x,y+1);

};
void filler(){
	for(int i=0;i<=M;i++)
		for(int j=0;j<=M;j++){
			//colors[i][j]=vec4(0.5,0.5,0.0,1.0);
			/*if(data[i][j].z>=5.0)colors[i][j]=vec4(0.9,0.9,0.9,1.0);
			else */if(data[i][j].z>=0.7)colors[i][j]=vec4(45.0/255,107/255.0,70/255.0,1.0)/4/*(data[i][j].z)/3.0*/;
			else if(data[i][j].z>=0.6)colors[i][j]=vec4(211/255.0,217/255.0,59/255.0,1.0);
			else  {colors[i][j]=vec4(62/255.0,73/255.0,168/255.0,1.0)/2/*(data[i][j].z)/0.7*/;
				data[i][j].z=0.7;
			}
		
		}
	
		glBufferSubData( GL_ARRAY_BUFFER, sizeof(data), sizeof(colors), colors );

}
void calcGNormal1(int x,int y){	
	if(x>=M-2 || y>=M-2)return;
	if(x==0||y==0){
		normal[x][y]=vec4(0,0.8,0.6,0);
		calcGNormal1(x+1,y);
		calcGNormal2(x,y+1);
		return;
	}
	point4 p01=data[x-1][y];
	point4 p0=data[x][y-1];
	point4 p1=data[x][y];
	point4 p2=data[x+1][y];
	point4 p3=data[x][y+1];
	normal[x][y]=vec4(0,0,0,0);
	normal[x][y]+=triangleNormal(p1,p0,p01)/4.0;
	normal[x][y]+=triangleNormal(p1,p01,p3)/4.0;
	normal[x][y]+=triangleNormal(p1,p3,p2)/4.0;
	normal[x][y]+=triangleNormal(p1,p2,p0)/4.0;
	normal[x][y]=normalize(normal[x][y]);
	
	calcGNormal1(x+1,y);
	calcGNormal2(x,y+1);

};

void calcFNormal2(int x,int y){	
	if(x>=M-2 || y>=M-2)return;
	

	point4 p1=data[x][y];
	point4 p2=data[x+1][y];
	point4 p3=data[x][y+1];
	normal[x][y]=triangleNormal(p1,p2,p3);
	
	normal[x][y]=normalize(normal[x][y]);
	calcFNormal2(x,y+1);

};

void calcFNormal1(int x,int y){	
	if(x>=M-2 || y>=M-2)return;
	point4 p1=data[x][y];
	point4 p2=data[x+1][y];
	point4 p3=data[x][y+1];
	normal[x][y]=triangleNormal(p1,p2,p3);
	
	normal[x][y]=normalize(normal[x][y]);
	calcFNormal1(x+1,y);
	calcFNormal2(x,y+1);

};

double pertubution(){
	return  (fmod(rand(),100)/50.0 + (-1))*6;
}
void drawP2(int x,int y){
	if(x>=M-1 || y>=M-1)return;
	int c[3]={x*(M+1)+y,(x+1)*(M+1)+y,x*(M+1)+y+1};
	if(0){
	vec4 nnn=triangleNormal(data[x][y],data[x][y+1],data[x+1][y]);
	
	glBufferSubData( GL_ARRAY_BUFFER, sizeof(data)+ sizeof(colors)+x*(M+1)+y,sizeof(normal[1][1]), nnn );
	glBufferSubData( GL_ARRAY_BUFFER, sizeof(data)+ sizeof(colors)+(x+1)*(M+1)+y,sizeof(normal[1][1]), nnn );
	glBufferSubData( GL_ARRAY_BUFFER, sizeof(data)+ sizeof(colors)+x*(M+1)+y+1,sizeof(normal[1][1]), nnn );
	}
	
	glDrawElements(GL_TRIANGLES, 3,GL_UNSIGNED_INT,c); 
	c[0]+=(M+1)*1+1;
	if(0){
	vec4 nnn=triangleNormal(data[x+1][y+1],data[x][y+1],data[x+1][y]);
	
	glBufferSubData( GL_ARRAY_BUFFER, sizeof(data)+ sizeof(colors)+c[0],sizeof(normal[1][1]), nnn );
	glBufferSubData( GL_ARRAY_BUFFER, sizeof(data)+ sizeof(colors)+(x+1)*(M+1)+y,sizeof(normal[1][1]), nnn );
	glBufferSubData( GL_ARRAY_BUFFER, sizeof(data)+ sizeof(colors)+x*(M+1)+y+1,sizeof(normal[1][1]), nnn );
	}
	glDrawElements(GL_TRIANGLES, 3,GL_UNSIGNED_INT,c); 
	
	drawP2(x,y+1);
	
}
void drawP1(int x,int y){
	if(x>=M-1 || y>=M-1)return;
	int c[3]={x*(M+1)+y,(x+1)*(M+1)+y,x*(M+1)+y+1};
	if(0){
	vec4 nnn=triangleNormal(data[x][y],data[x][y+1],data[x+1][y]);
	
	glBufferSubData( GL_ARRAY_BUFFER, sizeof(data)+ sizeof(colors)+x*(M+1)+y,sizeof(normal[1][1]), nnn );
	glBufferSubData( GL_ARRAY_BUFFER, sizeof(data)+ sizeof(colors)+(x+1)*(M+1)+y,sizeof(normal[1][1]), nnn );
	glBufferSubData( GL_ARRAY_BUFFER, sizeof(data)+ sizeof(colors)+x*(M+1)+y+1,sizeof(normal[1][1]), nnn );
	}
	glDrawElements(GL_TRIANGLES, 3,GL_UNSIGNED_INT,c); 
	c[0]+=(M+1)*1+1;
	if(0){
	vec4 nnn=triangleNormal(data[x+1][y+1],data[x][y+1],data[x+1][y]);
	
	glBufferSubData( GL_ARRAY_BUFFER, sizeof(data)+ sizeof(colors)+c[0],sizeof(normal[1][1]), nnn );
	glBufferSubData( GL_ARRAY_BUFFER, sizeof(data)+ sizeof(colors)+(x+1)*(M+1)+y,sizeof(normal[1][1]), nnn );
	glBufferSubData( GL_ARRAY_BUFFER, sizeof(data)+ sizeof(colors)+x*(M+1)+y+1,sizeof(normal[1][1]), nnn );
	}
	glDrawElements(GL_TRIANGLES, 3,GL_UNSIGNED_INT,c); 
	
	drawP1((x+1),y);
	drawP2(x,y+1);
	
	
}

void fill(int x1,int y1,int x2, int y2){
	vec4 temp=vec4(0,0,1,0);
	if(x2-x1==1||y2-y1==1)return;
	if(dataT[(x1+x2)/2][y1]){
	data[(x1+x2)/2][y1]=(data[x1][y1]+data[x2][y1])/2.0 + temp*pertubution()*(x1-x2)/(double)(M);
		dataT[(x1+x2)/2][y1]=0;
	}
	if(dataT[(x1+x2)/2][y2]){
	data[(x1+x2)/2][y2]=(data[x1][y2]+data[x2][y2])/2.0 +temp*pertubution()*(x1-x2)/(double)(M);
		dataT[(x1+x2)/2][y2]=0;
	}
	if(dataT[x1][(y1+y2)/2]){
	data[x1][(y1+y2)/2]=(data[x1][y1]+data[x1][y2])/2.0 +temp*pertubution()*(x1-x2)/(double)(M);
		dataT[x1][(y1+y2)/2]=0;
	}
	if(dataT[x2][(y1+y2)/2]){
	data[x2][(y1+y2)/2]=(data[x2][y1]+data[x2][y2])/2.0 +temp*pertubution()*(x1-x2)/(double)(M);
		dataT[x2][(y1+y2)/2]=0;
	}
	data[(x1+x2)/2][(y1+y2)/2]=(data[x1][y1]+data[x2][y1]+data[x1][y2]+data[x2][y2])/4.0 +temp*pertubution()*(x1-x2)/(double)(M);
	fill(x1,y1,(x1+x2)/2,(y1+y2)/2);
	fill((x1+x2)/2,y1,x2,(y1+y2)/2);
	fill(x1,(y1+y2)/2,(x1+x2)/2,y2);
	fill((x1+x2)/2,(y1+y2)/2,x2,y2);
}
void fillk(int x1,int y1,int x2, int y2){
	vec4 temp=vec4(0,0,1,0);
	if(x2-x1==1||y2-y1==1)return;
	if(data[(x1+x2)/2][y1]==0)
	data[(x1+x2)/2][y1]=(data[x1][y1]+data[x2][y1])/2.0 + temp*pertubution()*(x1-x2)/(double)(M);
	if(data[(x1+x2)/2][y2]==0)
	data[(x1+x2)/2][y2]=(data[x1][y2]+data[x2][y2])/2.0 +temp*pertubution()*(x1-x2)/(double)(M);
	if(data[x1][(y1+y2)/2]==0)
	data[x1][(y1+y2)/2]=(data[x1][y1]+data[x1][y2])/2.0 +temp*pertubution()*(x1-x2)/(double)(M);
	if(data[x2][(y1+y2)/2]==0)
	data[x2][(y1+y2)/2]=(data[x2][y1]+data[x2][y2])/2.0 +temp*pertubution()*(x1-x2)/(double)(M);
	data[(x1+x2)/2][(y1+y2)/2]=(data[x1][y1]+data[x2][y1]+data[x1][y2]+data[x2][y2])/4.0 +temp*pertubution()*(x1-x2)/(double)(M);
	fill(x1,y1,(x1+x2)/2,(y1+y2)/2);
	fill((x1+x2)/2,y1,x2,(y1+y2)/2);
	fill(x1,(y1+y2)/2,(x1+x2)/2,y2);
	fill((x1+x2)/2,(y1+y2)/2,x2,y2);
}


void display()
{
     vec4 t;
     glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);  /*clear the window */
     //point4 eye(radius*sin(theta)*cos(phi), radius*sin(theta)*sin(phi), radius*cos(theta), 1.0);
     double s=8;
     if(mode)calcFNormal1(0,0);
     else calcGNormal1(0,0);
     
    
      
      
     if(viewMode==0)model_view =  LookAt(eye, at, up)*Scale(scale);
     else if(viewMode==1){
     	eye=vec4(0.0,0.0,10.0,1.0);
     	at=vec4(0.1,0.1,8.0,1.0);
     	model_view =  LookAt(eye, at, up)*Scale(scale*6);
     }
     else if(viewMode==2) {
     	//fix=model_view*fix;
     	eye=vec4(scale*s*planex,scale*s*planey,fix.z+0.04,1.0);
     	at=vec4(scale*s*planex+0.020*cos(aci),scale*s*planey+0.020*sin(aci),fix.z,1.0);
     	//up = vec4(0.020*sin(-aci), 0.020*cos(-aci), 0.0, 0.0);
     	model_view =  LookAt(eye, at, up)*Scale(scale*8);
     	
     	
     	}
      else if(viewMode==3) {
     	//fix=model_view*fix;
     	eye=vec4(scale*s*(planex+0.08*cos(aci)),scale*s*(planey+0.08*sin(aci)),fix.z+0.08,1.0);
     	at=vec4(scale*s*(planex+0.08*cos(aci))+0.120*cos(aci),scale*s*(planey+0.08*sin(aci))+0.120*sin(aci),fix.z,1.0);
     	//up = vec4(0.020*sin(-aci), 0.020*cos(-aci), 0.0, 0.0);
     	model_view =  LookAt(eye, at, up)*Scale(scale*8);
     	
     	
     	}
     projection = Ortho(left, right, bottom, top, zNear, zFar);
     projection = Perspective( 80, 1.0, zNear, zFar);
     //Plane=Translate(planex,planey,planez);
     //filler();
     glUniformMatrix4fv(matrix_loc, 1, GL_TRUE, model_view);
     glUniformMatrix4fv(projection_loc, 1, GL_TRUE, projection);
     //for(int i = 0; i<M; i+=3) 
       
       drawP1(0,0);
       //fix=model_view*fix;
       model_view=model_view*Translate(planex,planey,planez)*RotateZ(aci2-45)*Translate(-fix.x,-fix.y,0.0);
       glUniformMatrix4fv(matrix_loc, 1, GL_TRUE, model_view);
       glDrawArrays(GL_TRIANGLES, (M+1)*(M+1), 3); 
     glutSwapBuffers();
}




void mykey(unsigned char key, int mousex, int mousey)
{
   float dr = 3.14159/180.0*10.0;
   if(key=='q'||key=='Q') exit(0);
   /*if(key == 'x') {left *= 1.1; right *= 1.1; }
   if(key == 'X') {left *= 0.9; right *= 0.9; }*/
   if(key == 'y') {bottom *= 1.1; top *= 1.1; }
   if(key == 'Y') {bottom *= 0.9; top *= 0.9; }
   if(key == 'z') {zNear  *= 1.1; zFar *= 1.1;}
   if(key == 'Z') {zNear *= 0.9; zFar *= 0.9;}
   if(key == 'r') radius *= 2.0;
   if(key == 'R') radius *= 0.5;
   if(key == 'o') theta += dr;
   if(key == 'O') theta -= dr;
   if(key == 'p') phi += dr;
   if(key == 'P') phi -= dr;
   if(key == 's') scale *= 101/100.0;
   if(key == 'S') scale *= 100.0/111;
   if(key == 'a') speed *= 100.0/111;
   if(key == 'd') speed *= 111/100.0;
   if(key == 'w') viewMode = 1;
   if(key == 't') viewMode = 2;
   if(key == 'c') viewMode = 3;
   if(key == 'x') viewMode = 0;
   if(key == 'f'){ if(mode)mode=0;
 			else mode=1;  	
 			 glBufferSubData( GL_ARRAY_BUFFER, sizeof(data)+ sizeof(colors),sizeof(normal), normal );
 			}		
   glutPostRedisplay();
}

void reshape(GLsizei ww, GLsizei hh)
{
   glViewport(0,0, ww, hh);
   w = ww;
   h = hh;
}

void specialKeyboard( int key, int x, int y ) 
{ 
	float dr = 3.14159/180.0;
    switch (key)
    {
    case GLUT_KEY_RIGHT:
               aci-=dr;
               aci2-=1;
        
        break;
    case GLUT_KEY_LEFT:
         	aci+=dr;
        	aci2+=1;
        
        break;
        case GLUT_KEY_DOWN:
        planey -= 0.020*sin(aci); 
        planex -= 0.020*cos(aci); 
        break;
    case GLUT_KEY_UP:
        planey += 0.020*sin(aci);
        planex += 0.020*cos(aci); 
        
        break;
    }
    printf("\n%f\n\n",aci);
    glutPostRedisplay();
}

void timer (int value){

	planey += 0.020*sin(aci)*speed;
        planex += 0.020*cos(aci)*speed; 

	glutPostRedisplay();
	glutTimerFunc(10, timer, 0);
}

int main(int argc, char** argv)
{
    glutInit(&argc, argv);

#ifdef __APPLE__
    glutInitDisplayMode( GLUT_3_2_CORE_PROFILE | GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH );
#else
    glutInitDisplayMode( GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH );
#endif

    glutInitWindowSize(1024, 1024);
	srand (time ( NULL));
//    glutInitContextVersion( 3, 2 );
//	glutInitContextProfile( GLUT_CORE_PROFILE );
    glutCreateWindow("Color Cube");
	glewExperimental = GL_TRUE;
	glewInit();
    init();

    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(mykey);
    glutSpecialFunc(specialKeyboard);
    glutTimerFunc(40, timer, 0);
    glEnable(GL_DEPTH_TEST);
    glutMainLoop();
    return 0;
}
