#ifndef OBJOBJECT_H
#define OBJOBJECT_H

#ifdef __APPLE__
#include <OpenGL/gl3.h>
#include <OpenGL/glext.h>
#include <OpenGL/gl.h> // Remove this line in future projects
#else
#include <GL/glew.h>
#endif

#include <GLFW/glfw3.h>
#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
using namespace std;

class OBJObject
{
private:
std::vector<glm::vec3> indices;
std::vector<glm::vec3> vertices;
std::vector<glm::vec3> normals;
glm::mat4 toWorld;
glm::mat4 rmx, tmx, smx;
glm::vec3 size, position;
glm::mat4 origin_mx, scale_mx;


public:
	OBJObject(const char* filepath);
	void parse(const char* filepath);
	void draw();
    void update(glm::vec3 vec);
    void spin(float deg, glm::vec3 vec);
    void translate(glm::vec3 vec);
    void scale(glm::vec3 vec);
    float angle;
    void resetPos(glm::vec3 vec);
    void resetOriScale();
    glm::mat4 getWorld();
    std::vector<glm::vec3> getVertices();
    std::vector<glm::vec3> getNormals();
    void resetToWorld();
    void setBuffers();
    void setCenter();
    void setScale();
};

#endif
