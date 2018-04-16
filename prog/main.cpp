//Scott Saunders
//10163541
//Cpsc 453 template
//October 1st, 2016.

#include "main.h"
#include <iostream>
#include <fstream>
#include <algorithm>
#include <string>
#include <vector>
#include <iterator>
#include <sstream>

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "types.h"
#include "gl_helpers.h"
#include "shapes.h"
#include "input.h"
#include "camera.h"

#include <glm/gtc/matrix_transform.hpp>

#include "metaball/metaball.h"
#include "cloud/cloud.h"
#include "lightning_stuff/lightning.h"
#include "lightning_stuff/bspline.h"

#include <chrono>
#include <thread>

//#define WIREFRAME
#define DEBUG

#ifdef DEBUG
	#define DEBUGMSG	printf("\n\n\t\t DEBUG MESSAGE AT LINE:\t%d\t In file:\t%s\n\n",__LINE__,__FILE__);
#else
	#define DEBUGMSG	;
#endif


#define torad(X)	((float)(X*PI/180.f))

Camera activeCamera;

//START: Window Mgmt
mat4 winRatio = mat4(1.f);
mat4 perspectiveMatrix = glm::perspective(glm::radians(45.f), 1.f, 0.1f, 400.f);
int WIDTH = 512;
int HEIGHT = 512;
int MODE = 0;



//Cloud editing?
cloud aCloud;
Tris renderTris;
int havePoints = 0;
std::vector<vec3> positions;
std::vector<float> radiuss;


//Input-passings
float thres=1;
int pass1=1; //MISC
int pass2=0; //MISC


//Lightening/Bspline
BSpline spline;

struct GLSTUFF{
	GLuint prog;
	GLuint vertexShader;
	GLuint fragShader;
	GLuint vertexarray;
	GLuint vertexarray2;
	GLuint vertexbuffer;
	GLuint vertexbuffer2;
	GLuint normalbuffer;
	GLuint uvsbuffer;
	GLuint indiciesbuffer;
	GLuint texture;
  GLuint MB_SSBO;
  GLuint L_SSBO;
};
GLSTUFF glstuffRay;
GLSTUFF glstuffCloud;
GLSTUFF glstuffBspline;

//Minimal modification from main_l
int num_control, num_spline;
void loadSpline()
{
	glUseProgram(glstuffBspline.prog);
	glBindVertexArray(glstuffBspline.vertexarray);

	glBindBuffer(GL_ARRAY_BUFFER, glstuffBspline.vertexbuffer);
	glBufferData(GL_ARRAY_BUFFER, spline.control_vecs.size() * sizeof(vec3), spline.control_vecs.data(), GL_STREAM_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vec3), (void*)0);
	glEnableVertexAttribArray(0);
	glBindVertexArray(0);
	num_control = spline.control_vecs.size();

	glBindVertexArray(glstuffBspline.vertexarray2);

	glBindBuffer(GL_ARRAY_BUFFER, glstuffBspline.vertexbuffer2);
	glBufferData(GL_ARRAY_BUFFER, spline.bspline_vecs.size() * sizeof(vec3), spline.bspline_vecs.data(), GL_STREAM_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vec3), (void*)0);
	glEnableVertexAttribArray(0);
	glBindVertexArray(0);

	num_spline = spline.bspline_vecs.size();
}


void initalize_GL(){
    //Intalizes... all the GLStuff.. sorry.
    
		glEnable(GL_DEPTH_TEST); 		//Turn on depth testing
		glDepthFunc(GL_LEQUAL); 			//Configure depth testing

		//OpenGL programs
		glstuffRay.prog = glCreateProgram();
		glstuffCloud.prog = glCreateProgram();
		glstuffBspline.prog = glCreateProgram();

    //Setup loading things
    std::vector<std::string> fs;
    
    fs.push_back(FRAGMENTPATH);
    fs.push_back(FRAGMENTPATH1);
    fs.push_back(FRAGMENTPATH2);
    fs.push_back(FRAGMENTPATH3);


    //Ray-vertex-shaders
		glstuffRay.vertexShader = CompileShader(GL_VERTEX_SHADER,LoadSource(VERTEXPATHRAY));
		glstuffBspline.vertexShader = CompileShader(GL_VERTEX_SHADER,LoadSource(VERTEXPATHRAY));
    
    //Normal-vertex-shaders
		glstuffCloud.vertexShader = CompileShader(GL_VERTEX_SHADER,LoadSource(VERTEXPATH));


    //non-bspline fragment
    glstuffRay.fragShader = CompileShader(GL_FRAGMENT_SHADER,LoadSourceM(fs));
    glstuffCloud.fragShader = CompileShader(GL_FRAGMENT_SHADER,LoadSourceM(fs));
    
    //bspline-fragment-shaders 
    glstuffBspline.fragShader = CompileShader(GL_FRAGMENT_SHADER,LoadSource(FRAGMENTBSPLINE));


    //Attaching to prog
		glAttachShader(glstuffRay.prog, glstuffRay.vertexShader);
		glAttachShader(glstuffRay.prog, glstuffRay.fragShader);
    //Attaching to prog
		glAttachShader(glstuffCloud.prog, glstuffCloud.vertexShader);
		glAttachShader(glstuffCloud.prog, glstuffCloud.fragShader);
    //Attaching to prog
		glAttachShader(glstuffBspline.prog, glstuffBspline.vertexShader);
		glAttachShader(glstuffBspline.prog, glstuffBspline.fragShader);

			//Attrib things here 

      //Linking/compiling
		glLinkProgram(glstuffRay.prog);
		check_gllink(glstuffRay.prog);
		glLinkProgram(glstuffBspline.prog);
		check_gllink(glstuffBspline.prog);
		glLinkProgram(glstuffCloud.prog);
		check_gllink(glstuffCloud.prog);

		//Vertex stuffs

      //ray
		glUseProgram(glstuffRay.prog);
		glGenVertexArrays(1, &glstuffRay.vertexarray);
		glGenBuffers(1, &glstuffRay.vertexbuffer);

		glBindBuffer(GL_ARRAY_BUFFER,glstuffRay.vertexbuffer);
//		glVertexAttribPointer(, 3, GL_FLOAT, GL_FALSE, sizeof(vec3), 0);	//Points -UNHAPPY

      //bspline
		glUseProgram(glstuffBspline.prog);
		glGenVertexArrays(1, &glstuffBspline.vertexarray);
		glGenBuffers(1, &glstuffBspline.vertexbuffer);

		glBindBuffer(GL_ARRAY_BUFFER,glstuffBspline.vertexbuffer);
//		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vec3), 0);	//Points -UNHAPPY
		
    glGenVertexArrays(1, &glstuffBspline.vertexarray2);
		glGenBuffers(1, &glstuffBspline.vertexbuffer2);
		glBindBuffer(GL_ARRAY_BUFFER,glstuffBspline.vertexbuffer2);


    //CLOUDS
		glUseProgram(glstuffCloud.prog);
		glGenVertexArrays(1, &glstuffCloud.vertexarray);
		glGenBuffers(1, &glstuffCloud.vertexbuffer);

		glGenBuffers(1, &glstuffCloud.normalbuffer);
		glGenBuffers(1, &glstuffCloud.indiciesbuffer);

		glBindVertexArray(glstuffCloud.vertexarray);
		glBindBuffer(GL_ARRAY_BUFFER,glstuffCloud.vertexbuffer);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vec3), 0);	//Points
		glEnableVertexAttribArray(0);

		glBindVertexArray(glstuffCloud.vertexarray);
		glBindBuffer(GL_ARRAY_BUFFER,glstuffCloud.normalbuffer);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(vec3), 0); //Normals
		glEnableVertexAttribArray(1);
		
    glBindVertexArray(glstuffCloud.vertexarray);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,glstuffCloud.indiciesbuffer);	//Indices

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);


    //MB -ssbo
    glBindVertexArray(glstuffRay.vertexarray);
    glGenBuffers(1, &glstuffRay.MB_SSBO);
  	glBindBufferBase(GL_SHADER_STORAGE_BUFFER,1,glstuffRay.MB_SSBO);

    //Lightning -ssbo
    glBindVertexArray(glstuffRay.vertexarray);
    glGenBuffers(1, &glstuffRay.L_SSBO);
  	glBindBufferBase(GL_SHADER_STORAGE_BUFFER,0,glstuffRay.L_SSBO);

    
}

void Update_Perspective(){

  //'CLOUD' PERSPECTIVE:
  if(MODE == MODE_CLOUD){
  glUniformMatrix4fv(glGetUniformLocation(glstuffCloud.prog, "perspectiveMatrix"),
            1,
            false,
            &perspectiveMatrix[0][0]);
  
  glm::mat4 camMatrix = activeCamera.view();
  glUniformMatrix4fv(glGetUniformLocation(glstuffCloud.prog, "cameraMatrix"),
            1,
            false,
            &camMatrix[0][0]);

  glUniformMatrix4fv(glGetUniformLocation(glstuffCloud.prog, "windowRatio"),
            1,
            false,
            &winRatio[0][0]);
  }

  //Todo: Get raytracing perspective.  
  if(MODE == MODE_RAY){

  glm::mat4 camMatrix = activeCamera.view();
  glUniformMatrix4fv(glGetUniformLocation(glstuffRay.prog, "cameraMatrix"),
            1,
            false,
            &camMatrix[0][0]);
  }



  //BSpline has no perspective.
}


void Update_GPU_data(){

  std::vector<vec3> storage;

	storage.push_back(vec3(-1.0, 1.0, 0.0));
	storage.push_back(vec3(1.0, 1.0, 0.0));
	storage.push_back(vec3(1.0, -1.0, 0.0));

	storage.push_back(vec3(1.0, -1.0, 0.0));
	storage.push_back(vec3(-1.0, -1.0, 0.0));
	storage.push_back(vec3(-1.0, 1.0, 0.0));

  glUseProgram(glstuffRay.prog);
	glBindVertexArray(glstuffRay.vertexarray);

  //Get MBs
  MBS d = cloud::getAllMBs();
  int i1 = d.size() + 1;

  std::cout << "Rendering " << i1 - 1 << " metaballs" << std::endl; 
  vec4 inf = vec4(i1,thres,0.0,0.0);

  //UNHAPPY

  //allocate space
  glBindBuffer(GL_SHADER_STORAGE_BUFFER, glstuffRay.MB_SSBO);
  glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(vec4)*i1, NULL, GL_DYNAMIC_COPY);
  //Send Most data
  glBufferSubData(GL_SHADER_STORAGE_BUFFER, sizeof(vec4), sizeof(vec4)*d.size(), d.data());
  glBufferSubData(GL_SHADER_STORAGE_BUFFER,0,sizeof(vec4),&inf); //Send info data..

  
	glBindBuffer(GL_ARRAY_BUFFER, glstuffRay.vertexbuffer);
	glBufferData(GL_ARRAY_BUFFER, storage.size() * sizeof(vec3), storage.data(), GL_STREAM_DRAW);

  //END UNHAPPY

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vec3), (void*)0);
	glEnableVertexAttribArray(0);
}


void Update_GPU_data(Tris t){
  //Calc verts/etc
	glBindVertexArray(glstuffCloud.vertexarray);

  glBindBuffer(GL_ARRAY_BUFFER,glstuffCloud.vertexbuffer);
  glBufferData(GL_ARRAY_BUFFER,sizeof(vec3)*t.verts->size(),t.verts->data(),GL_DYNAMIC_DRAW);

  glBindBuffer(GL_ARRAY_BUFFER,glstuffCloud.normalbuffer);
  glBufferData(GL_ARRAY_BUFFER,sizeof(vec3)*t.norms->size(),t.norms->data(),GL_DYNAMIC_DRAW);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,glstuffCloud.indiciesbuffer);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER,sizeof(GLuint)*t.idx->size(),t.idx->data(),GL_DYNAMIC_DRAW);

}



void Render(){

  if(MODE == MODE_RAY){
    Update_GPU_data();
    Update_Perspective();	//updates perspective uniform, as it's never changed.

  	glUseProgram(glstuffRay.prog);
   	glBindVertexArray(glstuffRay.vertexarray);
    glDrawArrays(GL_TRIANGLES, 0, 6);

     //Tell shaders to enable raytrace.
    glUniform1i(glGetUniformLocation(glstuffRay.prog, "RayTrace"),true);

  }
  else if (MODE == MODE_CLOUD){
  	glClearColor(40/255.0,56/255.0,81/255.0,0);

    Tris t = cloud::getAllTris();
    Update_Perspective();	//updates perspective uniform, as it's never changed.
    if(t.verts != NULL && t.norms != NULL && t.idx != NULL){
      Update_GPU_data(t);
    }else{
      std::cout << "Not updating data, some of it is null." << std::endl;
      return;
    }
  
  	glClear(GL_COLOR_BUFFER_BIT);
  	glClear(GL_DEPTH_BUFFER_BIT);
  	glUseProgram(glstuffCloud.prog);
   	glBindVertexArray(glstuffCloud.vertexarray);
   	glUseProgram(glstuffCloud.prog);

  	glDrawElements(
    	GL_TRIANGLES,   //What shape we're drawing  - GL_TRIANGLES, GL_LINES, GL_POINTS, GL_QUADS, GL_TRIANGLE_STRIP
  		t.idx->size(),    //How many indices
  		GL_UNSIGNED_INT,  //Type
  		0
  	);
  }else if (MODE == MODE_BSPLINE){
  	glClearColor(0,0,0,0);
  	glClear(GL_COLOR_BUFFER_BIT);
  	glClear(GL_DEPTH_BUFFER_BIT);
    //BSPLINE STUFF TODO.
    //

    spline.loadBSpline();
    loadSpline();

		glUseProgram(glstuffBspline.prog);
	
		glBindVertexArray(glstuffBspline.vertexarray);
		glDrawArrays(GL_LINES, 0, num_control);

	
		glBindVertexArray(glstuffBspline.vertexarray2);
		glDrawArrays(GL_LINE_STRIP, 0, num_spline);
  }


}


int main(int argc, char * argv[]){

	GLFWwindow * window = glfw_init(WIDTH,HEIGHT,"CPSC-589 - Cloud Modeling");	//Init window.

	glfwMakeContextCurrent(window); //Sets up a OpenGL context

  input::setup(window);

	initalize_GL();


  //Bspline
  spline = BSpline();
  spline.addPoint(vec3(-1.0, 0.0, 0.0));
  spline.addPoint(vec3(0.0, 1.0, 0.0));
  spline.addPoint(vec3(1.0, 0.0, 0.0));

	spline.loadControlPoints();
	spline.loadBSpline();
	loadSpline();


  for(float i=-4; i <= 4; i+=.5){
    float r = WyvillMetaBall(vec3(0,0,0), vec3(0,i,0), 1);
    std::cout << i << ", " << r << std::endl;
  }
 
  for(int i=0; i < 1; i++){
    vec3 t = vec3(0,0,-2);
    new cloud(NULL,&t,1,1,1.0f,1,3);
    new cloud(NULL,&t,1,1,1.0f,1,3);
//cloud::cloud(float(*f)(vec3, vec3, float) , vec3 * pos, int initBalls, int rounds, int rad, float gap, int type)
//    new cloud(NULL,&t,6,10,1.0f,8.0f,0); //wings
//    new cloud(NULL,&t,5,10,1.0f,15.0f,0); //frog
//    new cloud(NULL,&t,5,10,1.0f,40.0f,0); //scattered
//    new cloud(NULL,&t,16,10,1.0f,16.0f,1); //circular
  }

  positions.push_back(vec3(0,0,0));
  radiuss.push_back(1.0f);
  aCloud = *(new cloud(positions,radiuss));
  //aCloud.process_placed_metaballs();


	while(!glfwWindowShouldClose(window))
	{ //Main loop.

    glfwGetFramebufferSize(window, &WIDTH, &HEIGHT);
		Render();
    glfwSwapBuffers(window);
    
    //std::this_thread::sleep_for(std::chrono::milliseconds(100));
    if(MODE == MODE_BSPLINE){
      glfwPollEvents();
    }else{
  		glfwWaitEvents();
    }

	}
	glfwTerminate();	//Kill the glfw interface
}
