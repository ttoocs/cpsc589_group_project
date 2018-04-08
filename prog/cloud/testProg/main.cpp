//Scott Saunders
//10163541
//Cpsc 453 template
//October 1st, 2016.

#include "../../main.h"
#include "../../metaball/metaball.h"
#include "../cloud.h"

Camera activeCamera;

float speed = 1;

  std::vector<vec3> verts;
  std::vector<vec3> norms;
  std::vector<GLuint> idx;
Object sphere;

//START: Metaball vars for testing
std::vector<MetaBall*> metaballs;
std::vector<float> vertices;
int num_points;

mat4 perspectiveMatrix = glm::perspective(glm::radians(45.f), 1.f, 0.1f, 400.f);
int nextRound = 0;
//END: Metaball vars for testing

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
		glstuff.vertexShader = CompileShader(GL_VERTEX_SHADER,LoadSource(VERTEXPATH));
		glstuff.fragShader = CompileShader(GL_FRAGMENT_SHADER,LoadSource(FRAGMENTPATH));

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

		glGenBuffers(1, &glstuff.normalbuffer);
//		glGenBuffers(1, &glstuff.uvsbuffer);
		glGenBuffers(1, &glstuff.indiciesbuffer);

		glBindVertexArray(glstuff.vertexarray);
		glBindBuffer(GL_ARRAY_BUFFER,glstuff.vertexbuffer);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vec3), 0);	//Points
		glEnableVertexAttribArray(0);

		glBindVertexArray(glstuff.vertexarray);
		glBindBuffer(GL_ARRAY_BUFFER,glstuff.normalbuffer);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(vec3), 0); //Normals
		glEnableVertexAttribArray(1);
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
  glUniformMatrix4fv(glGetUniformLocation(glstuff.prog, "perspectiveMatrix"),
            1,
            false,
            &perspectiveMatrix[0][0]);

}


void Update_GPU_data(){
//		glBindBuffer(GL_ARRAY_BUFFER,glstuff.vertexbuffer);	//Setup data-copy (points)
//		glBufferData(GL_ARRAY_BUFFER,sizeof(vec3)*s->positions.size(),s->positions.data(),GL_DYNAMIC_DRAW);

  //Calc verts/etc
  
  glBindBuffer(GL_ARRAY_BUFFER,glstuff.vertexbuffer);
  glBufferData(GL_ARRAY_BUFFER,sizeof(vec3)*verts.size(),verts.data(),GL_DYNAMIC_DRAW);

  glBindBuffer(GL_ARRAY_BUFFER,glstuff.normalbuffer);
  glBufferData(GL_ARRAY_BUFFER,sizeof(vec3)*norms.size(),norms.data(),GL_DYNAMIC_DRAW);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,glstuff.indiciesbuffer);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER,sizeof(GLuint)*idx.size(),idx.data(),GL_DYNAMIC_DRAW);

  glm::mat4 camMatrix = activeCamera.getMatrix();
  glUniformMatrix4fv(glGetUniformLocation(glstuff.prog, "cameraMatrix"),
            1,
            false,
            &camMatrix[0][0]);

}


void Render(){
	glClearColor(0.5,0,0,0);
  
  
  glm::mat4 camMatrix = activeCamera.getMatrix();
  glUniformMatrix4fv(glGetUniformLocation(glstuff.prog, "cameraMatrix"),
            1,
            false,
            &camMatrix[0][0]);

  
  
	glClear(GL_COLOR_BUFFER_BIT);
	glClear(GL_DEPTH_BUFFER_BIT);
	glUseProgram(glstuff.prog);
 	glBindVertexArray(glstuff.vertexarray);
 	glUseProgram(glstuff.prog);



//  glDrawArrays(GL_TRIANGLES,0,3);

	glDrawElements(
  	GL_TRIANGLES,   //What shape we're drawing  - GL_TRIANGLES, GL_LINES, GL_POINTS, GL_QUADS, GL_TRIANGLE_STRIP
		idx.size(),    //How many indices
		GL_UNSIGNED_INT,  //Type
		0
	);


	return;
}

void printVec(vec3 v)
{
  std::cout <<"aVec: (x,y,z) = (" << v.x << ", " << v.y << ", " << v.z << ")\n";
}

int main(int argc, char * argv[]){

	GLFWwindow * window = glfw_init(WIDTH,HEIGHT,"Scott Saunders - Template");	//Init window.

	glfwMakeContextCurrent(window); //Sets up a OpenGL context

  input::setup(window);

	initalize_GL();


	speed =0.01;

  //metaballs.push_back(new MetaBall(vec3(0,0,-5), 1, fanceyMB));
 // metaballs.push_back(new MetaBall(vec3(-1,0,-5), 1, fanceyMB));
 // metaballs.push_back(new MetaBall(vec3(1,0,-5), 1, fanceyMB));
/*  metaballs.push_back(new MetaBall(vec3(-3,0,-5), 1, fanceyMB));
  metaballs.push_back(new MetaBall(vec3(3,0,-5), 1, fanceyMB));
  metaballs.push_back(new MetaBall(vec3(0,3,-5), 1, fanceyMB));
  metaballs.push_back(new MetaBall(vec3(0,-3,-5), 1, fanceyMB));*/

  int size = 10;
  cloud aCloud;
  cloud clouds[size];
  aCloud.balls = metaballs;
  
std::vector<vec3> verts_s;
std::vector<GLuint> idx_s;
std::vector<vec3> norms_s;
  for(int i = 0; i < size;i++)
  {
    float x = -1+3*i;
    float y = 10 - 20*(((float)rand())/((float)INT_MAX));
    float z = 10 - 20*(((float)rand())/((float)INT_MAX));
    clouds[i].balls.push_back(new MetaBall(vec3(x,y,z), 1, fanceyMB));
    x = x+2 - 4*(((float)rand())/((float)INT_MAX));
    y = y+2 - 4*(((float)rand())/((float)INT_MAX));
    z = z+2 - 4*(((float)rand())/((float)INT_MAX));
    clouds[i].balls.push_back(new MetaBall(vec3(x,y,z), 1, fanceyMB));
    x = x+2 - 4*(((float)rand())/((float)INT_MAX));
    y = y+2 - 4*(((float)rand())/((float)INT_MAX));
    z = z+2 - 4*(((float)rand())/((float)INT_MAX));
    clouds[i].balls.push_back(new MetaBall(vec3(x,y,z), 1, fanceyMB));
    x = x+2 - 4*(((float)rand())/((float)INT_MAX));
    y = y+2 - 4*(((float)rand())/((float)INT_MAX));
    z = z+2 - 4*(((float)rand())/((float)INT_MAX));
    clouds[i].balls.push_back(new MetaBall(vec3(x,y,z), 1, fanceyMB));
  }

  verts.clear();
  idx.clear();
  norms.clear();
  
  //aCloud.create_cloud_paper(&verts, &idx,&norms, 3);
  for(int i = 0; i < size;i++)
  {
    verts_s.clear();
    norms_s.clear();
    idx_s.clear();
    clouds[i].create_cloud_paper(&verts_s, &idx_s,&norms_s, 6);
    for(int j = 0; j < idx_s.size();j++)
    {
      verts.push_back(verts_s[j]);
      norms.push_back(norms_s[j]);
      idx.push_back(idx.size());
    }
  }
  
  Update_Perspective();	//updates perspective uniform, as it's never changed.
  Update_GPU_data();
  
  printVec(activeCamera.pos);
  printVec(activeCamera.dir);
  printVec(activeCamera.up);
  printVec(activeCamera.right);

	while(!glfwWindowShouldClose(window))
	{ //Main loop.

    glfwGetFramebufferSize(window, &WIDTH, &HEIGHT);

    if(nextRound == 1)
    {
     // aCloud.create_cloud_naive(&verts, &idx,&norms, 1);
      nextRound = 0;
    }
		Render();
    glfwSwapBuffers(window);
		glfwPollEvents();

	}
	glfwTerminate();	//Kill the glfw interface
}
