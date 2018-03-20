//Scott Saunders
//10163541
//Cpsc 453 template 
//October 1st, 2016.


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

//#include <Eigen/Dense>


#include "types.h"
#include "gl_helpers.h"
#include "shapes.h"
#include "input.h"



//#define WIREFRAME
#define DEBUG

#ifdef DEBUG
	#define DEBUGMSG	printf("\n\n\t\t DEBUG MESSAGE AT LINE:\t%d\t In file:\t%s\n\n",__LINE__,__FILE__);
#else 
	#define DEBUGMSG	;
#endif


#define torad(X)	((float)(X*PI/180.f))

using namespace std;

float speed = 1;

struct GLSTUFF{
	GLuint prog;
	GLuint vertexShader;
	GLuint fragShader;
	GLuint vertexarray;
	GLuint vertexbuffer;
	GLuint normalbuffer;
	GLuint uvsbuffer;
	GLuint indiciesbuffer;
	GLuint texture;
};
GLSTUFF glstuff;

int WIDTH = 512;
int HEIGHT = 512;

void initalize_GL(){	
		glEnable(GL_DEPTH_TEST); 		//Turn on depth testing
		glDepthFunc(GL_LEQUAL); 			//Configure depth testing

		#ifdef WIREFRAME
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		#endif
	
		//OpenGL programs
		glstuff.prog = glCreateProgram();
		glstuff.vertexShader = CompileShader(GL_VERTEX_SHADER,LoadSource("vertex.glsl"));
		glstuff.fragShader = CompileShader(GL_FRAGMENT_SHADER,LoadSource("fragment.glsl"));

    //Attaching to prog
		glAttachShader(glstuff.prog, glstuff.vertexShader);
		glAttachShader(glstuff.prog, glstuff.fragShader);

			//Attrib things here

      //Linking/compiling
		glLinkProgram(glstuff.prog);	//Link to full program.
		check_gllink(glstuff.prog);

			
		//Vertex stuffs

		glUseProgram(glstuff.prog);
		glGenVertexArrays(1, &glstuff.vertexarray);
		glGenBuffers(1, &glstuff.vertexbuffer);

//		glGenBuffers(1, &glstuff.normalbuffer);
//		glGenBuffers(1, &glstuff.uvsbuffer);
		glGenBuffers(1, &glstuff.indiciesbuffer);
	
		glBindVertexArray(glstuff.vertexarray);
		glBindBuffer(GL_ARRAY_BUFFER,glstuff.vertexbuffer);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vec3), 0);	//Points
		glEnableVertexAttribArray(0);

//		glBindVertexArray(glstuff.vertexarray);
//		glBindBuffer(GL_ARRAY_BUFFER,glstuff.normalbuffer);
//		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(vec3), 0); //Normals
//		glEnableVertexAttribArray(1);
//
//		glBindVertexArray(glstuff.vertexarray);
//		glBindBuffer(GL_ARRAY_BUFFER,glstuff.uvsbuffer);
//		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(vec2), 0); //UVS
//		glEnableVertexAttribArray(2);
//		
		glBindVertexArray(glstuff.vertexarray);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,glstuff.indiciesbuffer);	//Indices

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);

		//Texture stuff
/*
		glGenTextures(1,&glstuff.texture);
		glBindTexture(GL_TEXTURE_2D, glstuff.texture);
		
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT ); //GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT ); //GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);	//Sets paramiters of the texture.
//  */

}

void Update_Perspective(){
/*	glm::mat4 perspectiveMatrix = glm::perspective(torad(80.f), 1.f, 0.1f, 20.f);
  glUniformMatrix4fv(glGetUniformLocation(glstuff.prog, "perspectiveMatrix"),
            1,
            false,
            &perspectiveMatrix[0][0]);
*/
}

void Update_GPU_data(){
//		glBindBuffer(GL_ARRAY_BUFFER,glstuff.vertexbuffer);	//Setup data-copy (points)
//		glBufferData(GL_ARRAY_BUFFER,sizeof(vec3)*s->positions.size(),s->positions.data(),GL_DYNAMIC_DRAW);
/*
	glm::mat4 camMatrix = cam.getMatrix();
  glUniformMatrix4fv(glGetUniformLocation(glstuff.prog, "cameraMatrix"),
            1,
            false,
            &camMatrix[0][0]);
*/
}

Object sphere;

	
void Render(){
	glClearColor(0.5,0,0,0);
	glClear(GL_COLOR_BUFFER_BIT);
	glClear(GL_DEPTH_BUFFER_BIT);
	glUseProgram(glstuff.prog);
 	glBindVertexArray(glstuff.vertexarray);
 	glUseProgram(glstuff.prog);

//	  Update_Perspective();	//updates perspective uniform, as it's never changed.
//    Update_GPU_data();

  glBindBuffer(GL_ARRAY_BUFFER,glstuff.vertexbuffer);
  glBufferData(GL_ARRAY_BUFFER,sizeof(vec3)*sphere.positions.size(),sphere.positions.data(),GL_DYNAMIC_DRAW); 
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,glstuff.indiciesbuffer);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER,sizeof(GLuint)*sphere.indices.size(),sphere.indices.data(),GL_DYNAMIC_DRAW);

//  glDrawArrays(GL_TRIANGLES,0,3);

	glDrawElements(
  	GL_TRIANGLES,   //What shape we're drawing  - GL_TRIANGLES, GL_LINES, GL_POINTS, GL_QUADS, GL_TRIANGLE_STRIP
		sphere.indices.size(),    //How many indices
		GL_UNSIGNED_INT,  //Type
		0
	);


	return;
}
int main(int argc, char * argv[]){

	GLFWwindow * window = glfw_init(WIDTH,HEIGHT,"Scott Saunders - Template");	//Init window.

	glfwMakeContextCurrent(window); //Sets up a OpenGL context

  input::setup(window);

	initalize_GL();


	speed =0.01;

  generateSphere(&sphere,0.1,10,10);

	while(!glfwWindowShouldClose(window))
	{ //Main loop.
    
    glfwGetFramebufferSize(window, &WIDTH, &HEIGHT);

		Render();
    glfwSwapBuffers(window);
		glfwPollEvents();

	}
	glfwTerminate();	//Kill the glfw interface
}