#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>

#include <GL/glew.h>
#include <GL/glfw.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "utils.h"
#include "data.h"

GLuint vao, vbo[1];

GLchar *vertexsource, *fragmentsource;
GLuint vertexshader, fragmentshader;
GLuint shader;

void check(char *where) { // Function to check OpenGL error status
	char * what;
	int err = glGetError(); //0 means no error
	if(!err)
		return;
	if(err == GL_INVALID_ENUM)
		what = (char *)"GL_INVALID_ENUM";
	else if(err == GL_INVALID_VALUE)
		what = (char *)"GL_INVALID_VALUE";
	else if(err == GL_INVALID_OPERATION)
		what = (char *)"GL_INVALID_OPERATION";
	else if(err == GL_INVALID_FRAMEBUFFER_OPERATION)
		what = (char *)"GL_INVALID_FRAMEBUFFER_OPERATION";
	else if(err == GL_OUT_OF_MEMORY)
		what = (char *)"GL_OUT_OF_MEMORY";
	else
		what = (char *)"Unknown Error";
	fprintf(stderr, "Error (%d) %s  at %s\n", err, what, where);
	exit(1);
}

/*
 This function is a bit more complex than the one provided in Tutorial 3 so that we
 can get more details error logs. If there are syntax errors in the shaders this will
 report the line number and some information about what is wrong with it.
 */
void SetupShaders() {
	char error[1000];
	GLint Result = GL_FALSE;
    int InfoLogLength;
	
	shader = glCreateProgram();
	vertexsource = fileToBuffer((char *)"VertexShader.glsl");
	fragmentsource = fileToBuffer((char *)"FragmentShader.glsl");
	
	vertexshader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexshader, 1, (const char **)&vertexsource, NULL);
	glCompileShader(vertexshader);
	
	// Check Vertex Shader
    glGetShaderiv(vertexshader, GL_COMPILE_STATUS, &Result);
    glGetShaderiv(vertexshader, GL_INFO_LOG_LENGTH, &InfoLogLength);
    glGetShaderInfoLog(vertexshader, InfoLogLength, NULL, error);
	if (InfoLogLength > 0) {
		fprintf(stdout, "%s\n", error);
	}
	
	fragmentshader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentshader, 1, (const char **)&fragmentsource , NULL);
    glCompileShader(fragmentshader);
	
    // Check Fragment Shader
    glGetShaderiv(fragmentshader, GL_COMPILE_STATUS, &Result);
    glGetShaderiv(fragmentshader, GL_INFO_LOG_LENGTH, &InfoLogLength);
    glGetShaderInfoLog(fragmentshader, InfoLogLength, NULL, error);
    if (InfoLogLength > 0) {
		fprintf(stdout, "%s\n", error);
	}
	
	glAttachShader(shader, vertexshader);
	glAttachShader(shader, fragmentshader);
	glLinkProgram(shader);
	
	glGetProgramiv(shader, GL_LINK_STATUS, &Result);
    glGetProgramiv(shader, GL_INFO_LOG_LENGTH, &InfoLogLength);
    glGetProgramInfoLog(shader, InfoLogLength, NULL, error);
	if (InfoLogLength > 0) {
		fprintf(stdout, "%s\n", error);
	}
	
	check((char *)"SetupShaders");
}

void SetupGeometry() {
	
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
	
	glGenBuffers(1, vbo);

	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
	glBufferData(GL_ARRAY_BUFFER, 12 * sizeof(struct Vertex), tetrahedron, GL_STATIC_DRAW);
	
	glVertexAttribPointer((GLuint)0, 3, GL_DOUBLE, GL_FALSE, sizeof(struct Vertex), (const GLvoid*)offsetof(struct Vertex,position));
	glEnableVertexAttribArray(0);
		
	glVertexAttribPointer((GLuint)1, 3, GL_FLOAT, GL_FALSE, sizeof(struct Vertex), (const GLvoid*)offsetof(struct Vertex, color));
	glEnableVertexAttribArray(1);
	
	glBindAttribLocation(shader, 0, "in_Position");
	glBindAttribLocation(shader, 1, "in_Color");
	
	/*
	 In GLSL 1.50 on OpenGL 3.2, gl_FragColor has been removed. The way to get colour
	 data out of the shader now is to specify the output variable that will be used with
	 this function. In this case, it's called "FragColor" in the shader.
	 */
	glBindFragDataLocation(shader, 0, "FragColor");
	
	glUseProgram(shader);
	check((char *)"SetupGeometry");
}

void Render(int i) {
	glClearColor(0.0, 0.0, 0.0, 1.0);
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
	
	GLfloat angle;
	glm::mat4 Projection = glm::perspective(45.0f, 1.0f, 0.1f, 100.0f);
	angle = (GLfloat) (i % 360);
	glm::mat4 View = glm::mat4(1.);
	View = glm::translate(View, glm::vec3(0.f, 0.f, -5.0f));
	View = glm::rotate(View, angle * -1.0f, glm::vec3(1.f, 0.f, 0.f));
	View = glm::rotate(View, angle * 0.5f, glm::vec3(0.f, 1.f, 0.f));
	View = glm::rotate(View, angle * 0.5f, glm::vec3(0.f, 0.f, 1.f));
	glm::mat4 Model = glm::mat4(1.0);
	glm::mat4 MVP = Projection * View * Model;
	glUniformMatrix4fv(glGetUniformLocation(shader, "mvpmatrix"), 1, GL_FALSE, glm::value_ptr(MVP));
	
	check((char *)"Rendering");
	glDrawArrays(GL_TRIANGLES, 0, 12);
	glFlush();
}

int main() {
	int running = GL_TRUE;
	if (!glfwInit()) {
		exit(EXIT_FAILURE);
	}
	
	/*
	 We must ask for version 3.2 on Mac OS. That is the highest supported, but does not
	 appear to be the default. If we don't do this, the GLSL 1.50 features will not work.
	 */
	glfwOpenWindowHint(GLFW_FSAA_SAMPLES, 4);
	glfwOpenWindowHint(GLFW_OPENGL_VERSION_MAJOR, 3);
	glfwOpenWindowHint(GLFW_OPENGL_VERSION_MINOR, 2);
	glfwOpenWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	
	if (!glfwOpenWindow(1136, 640, 0, 0, 0, 0, 0, 0, GLFW_WINDOW)) {
		glfwTerminate();
		exit(EXIT_FAILURE);
	}
	glewExperimental = GL_TRUE;
	glewInit();
	glGetError();
	SetupShaders();
	SetupGeometry();
	glEnable(GL_DEPTH_TEST);
	int i = 0;
	while (running) {
		Render(i);
		i++;
		glfwSwapBuffers();
		running = !glfwGetKey(GLFW_KEY_ESC) && glfwGetWindowParam(GLFW_OPENED);
	}
	glfwTerminate();
	exit(EXIT_SUCCESS);
}
