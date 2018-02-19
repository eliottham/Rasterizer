#include "Window.h"
#include <cfloat>

const char* window_title = "GLFW Starter Project";

int Window::width;
int Window::height;

static int window_width = 512, window_height = 512;
static float* pixels = new float[window_width * window_height * 3];
static float* zbuffer = new float[window_width * window_height];
std::vector<glm::vec3> vertices;
std::vector<glm::vec3> normals;
int point_size = 1;
bool flag = false;

OBJObject bunny("./bunny.obj");
OBJObject dragon("./dragon.obj");

void loadData() {
    vertices = dragon.getVertices();
    normals = dragon.getNormals();
}

struct Color {
    float r, g, b;
};

void clearBuffer() {
    Color clearColor = {0.0, 0.0, 0.0};
    for(int i = 0; i < window_width * window_height; ++i) {
        pixels[i * 3] = clearColor.r;
        pixels[i * 3 + 1] = clearColor.g;
        pixels[i * 3 + 2] = clearColor.b;
        
        zbuffer[i] = FLT_MAX;
    }
}

void drawPoint(int x, int y, float z, float r, float g, float b) {
    int offset = y * window_width * 3 + x * 3;
    int zoffset = y * window_width + x;

    pixels[offset] = r;
    pixels[offset + 1] = g;
    pixels[offset + 2] = b;
    
    zbuffer[zoffset] = z;
}

void rasterize() {
    // model matrix
    glm::mat4 world = dragon.getWorld();
    
    // view matrix
    glm::mat4 view = glm::lookAt(glm::vec3(0.0f, 0.0f, 20.0f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    
    // projection matrix
    glm::mat4 proj = glm::perspective(glm::radians(60.0f), (float)(window_width)/(float)(window_height), 1.0f, 1000.0f);
    
    // viewport matrix
    glm::mat4 vp(window_width/2.0f, 0.0f, 0.0f, 0.0f,
                 0.0f, window_height/2.0f, 0.0f, 0.0f,
                 0.0f, 0.0f, 0.5f, 0.0f,
                 window_width/2.0f, window_height/2.0f, 0.5f, 1.0f);
    
    // draw each point
    for(int i = 0; i < vertices.size(); i++) {
        glm::vec3 normalized = glm::normalize(normals[i]);
        normalized = normalized * 0.5f + 0.5f;  // normal coloring
        
        glm::vec4 p = vp * proj * view * world * glm::vec4(vertices[i], 1.0f);
        
        int x_onscreen = p.x/p.w;
        int y_onscreen = p.y/p.w;
        float z = p.z/p.w;

        // accounts for increased pixel size
        int ps_x, ps_y;
        for(int j = 0; j < point_size; j++) {
            ps_x = x_onscreen + j;
            for(int k = 0; k < point_size; k++) {
                ps_y = y_onscreen + k;
                
                if((ps_x >= 0) && (ps_x <= window_width) && (ps_y >= 0) && (ps_y <= window_height)) {
                    if((z >= -1.0f) && (z <= 1.0f) && (z < zbuffer[ps_y * window_width + ps_x])) {
                        drawPoint(ps_x, ps_y, z, normalized.x, normalized.y, normalized.z);
                    }
                }
            }
        }
        
    }
    dragon.resetToWorld();
}

void Window::initialize_objects()
{

    bunny.translate(glm::vec3(2.0f, 0.0f, 0.0f));
    dragon.translate(glm::vec3(-2.0f, -2.0f, 0.0f));
    
    bunny.scale(glm::vec3(2.0f));
    dragon.scale(glm::vec3(2.0f));
    
    loadData();
}

void Window::clean_up()
{
    delete[] pixels;
    delete[] zbuffer;
}

GLFWwindow* Window::create_window(int width, int height)
{
	// Initialize GLFW.
	if (!glfwInit())
	{
		fprintf(stderr, "Failed to initialize GLFW\n");
		return NULL;
	}

	// 4x antialiasing
	glfwWindowHint(GLFW_SAMPLES, 4);

	// Create the GLFW window
	GLFWwindow* window = glfwCreateWindow(width, height, window_title, NULL, NULL);

	// Check if the window could not be created
	if (!window)
	{
		fprintf(stderr, "Failed to open GLFW window.\n");
		glfwTerminate();
		return NULL;
	}

	// Make the context of the window
	glfwMakeContextCurrent(window);

	// Set swap interval to 1
	glfwSwapInterval(1);

	// Call the resize callback to make sure things get drawn immediately
	Window::resize_callback(window, width, height);

	return window;
}

void Window::resize_callback(GLFWwindow* window, int width, int height)
{
#ifdef __APPLE__
	glfwGetFramebufferSize(window, &width, &height); // In case your Mac has a retina display
#endif
	Window::width = width;
	Window::height = height;
    
    window_width = width;
    window_height = height;
    delete[] pixels;
    pixels = new float[window_width * window_height * 3];
    delete[] zbuffer;
    zbuffer = new float[window_width * window_height];
    
	// Set the viewport size
    glViewport(0, 0, width, height);
	// Set the matrix mode to GL_PROJECTION to determine the proper camera properties
	glMatrixMode(GL_PROJECTION);
	// Load the identity matrix
	glLoadIdentity();
	// Set the perspective of the projection viewing frustum
	gluPerspective(60.0, double(width) / (double)height, 1.0, 1000.0);
	// Move camera back 20 units so that it looks at the origin (or else it's in the origin)
	glTranslatef(0, 0, -20);
     
}

void Window::idle_callback()
{
	// Perform any updates as necessary. Here, we will spin the cube slightly.
    bunny.update(glm::vec3(0.0f, 1.0f, 0.0f));
    dragon.update(glm::vec3(0.0f, 1.0f, 0.0f));
     
}

void Window::display_callback(GLFWwindow* window)
{
    // Clear the color and depth buffers
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    // Set the matrix mode to GL_MODELVIEW
    glMatrixMode(GL_MODELVIEW);
    // Load the identity matrix
    glLoadIdentity();
    
	// Render objects
    if(flag) {
        clearBuffer();
        rasterize();
        glDrawPixels(window_width, window_height, GL_RGB, GL_FLOAT, pixels);
    }
    else {
            bunny.draw();
            dragon.draw();
    }
	// Gets events, including input such as keyboard and mouse or window resizing
	glfwPollEvents();
	// Swap buffers
	glfwSwapBuffers(window);
}

void Window::key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	// Check for a key press
	if (action == GLFW_PRESS)
	{
		// Check if escape was pressed
		if (key == GLFW_KEY_ESCAPE)
		{
			// Close the window. This causes the program to also terminate.
			glfwSetWindowShouldClose(window, GL_TRUE);
		}
        // Increase the size of points with 'P', decrease with 'p'
        if(key == GLFW_KEY_P) {
            GLfloat size;
            glGetFloatv(GL_POINT_SIZE, &size);
            
            if(mods == GLFW_MOD_SHIFT) {
                glPointSize(size + 1.0);
                point_size++;
            }
            else {
                glPointSize(size - 1.0);
                point_size--;
                if(point_size < 1) {
                    point_size = 1;
                }
            }
        }
        // Move left along the x-axis
        if(key == GLFW_KEY_A) {
            bunny.translate(glm::vec3(-1.0f, 0.0f, 0.0f));
            dragon.translate(glm::vec3(-1.0f, 0.0f, 0.0f));
        }
        // Move right along the x-axis
        if(key == GLFW_KEY_D) {
            bunny.translate(glm::vec3(1.0f, 0.0f, 0.0f));
            dragon.translate(glm::vec3(1.0f, 0.0f, 0.0f));
        }
        // Move up along the y-axis
        if(key == GLFW_KEY_W) {
            bunny.translate(glm::vec3(0.0f, 1.0f, 0.0f));
            dragon.translate(glm::vec3(0.0f, 1.0f, 0.0f));
        }
        // Move down along the y-axis
        if(key == GLFW_KEY_S) {
            bunny.translate(glm::vec3(0.0f, -1.0f, 0.0f));
            dragon.translate(glm::vec3(0.0f, -1.0f, 0.0f));
        }
        // Move into the z-axis with 'Z', out with 'z'
        if(key == GLFW_KEY_Z) {
            if(mods == GLFW_MOD_SHIFT) {
                bunny.translate(glm::vec3(0.0f, 0.0f, -1.0f));
                dragon.translate(glm::vec3(0.0f, 0.0f, -1.0f));
            }
            else {
                bunny.translate(glm::vec3(0.0f, 0.0f, 1.0f));
                dragon.translate(glm::vec3(0.0f, 0.0f, 1.0f));
            }
        }
        // Scale up with 'C', scale down with 'c'
        if(key == GLFW_KEY_C) {
            if(mods == GLFW_MOD_SHIFT) {
                bunny.scale(glm::vec3(1.2f));
                dragon.scale(glm::vec3(1.2f));
            }
            else {
                bunny.scale(glm::vec3(0.8f));
                dragon.scale(glm::vec3(0.8f));
            }
        }
        // Reset the position with 'r', reset orientation/scale with 'R'
        if(key == GLFW_KEY_R) {
            if(mods == GLFW_MOD_SHIFT) {
                bunny.resetOriScale();
                dragon.resetOriScale();
            }
            else {
                bunny.resetPos(glm::vec3(2.0f, 0.0f, 0.0f));
                dragon.resetPos(glm::vec3(-2.0f, -2.0f, 0.0f));
            }
        }
        // Toggle rasterizer
        if(key == GLFW_KEY_M) {
            flag ? flag = false : flag = true;
        }
	}
}
