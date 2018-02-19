#include "OBJObject.h"

OBJObject::OBJObject(const char *filepath) 
{
	toWorld = glm::mat4(1.0f);
	parse(filepath);
    this->angle = 0.0f;
    rmx = glm::mat4(1.0f);
    tmx = glm::mat4(1.0f);
    position = glm::vec3(0.0f);
    size = glm::vec3(1.0f);
    
}

void OBJObject::parse(const char *filepath) 
{
	//TODO parse the OBJ file
	// Populate the face indices, vertices, and normals vectors with the OBJ Object data
    FILE* fp;
    float x, y, z;
    float r, g, b;
    float v1, v2, v3, n1, n2, n3;
    int c1, c2;
    
    fp = fopen(filepath, "r");

    while((c1 = fgetc(fp)) != EOF) {
        if(c1 == 'v') {
            c2 = fgetc(fp);
            if(c2 == ' ') {
                fscanf(fp, "%f %f %f %f %f %f", &x, &y, &z, &r, &g, &b);
                vertices.push_back(glm::vec3(x, y, z));
            }
            else if(c2 == 'n') {
                fscanf(fp, "%f %f %f", &x, &y, &z);
                normals.push_back(glm::vec3(x, y, z));
            }
        }
        else if(c1 == 'f') {
            c2 = fgetc(fp);
            if(c2 == ' ') {
                fscanf(fp, "%f//%f %f//%f %f//%f", &v1, &n1, &v2, &n2, &v3, &n3);
                indices.push_back(glm::vec3(v1, v2, v3));
            }
         }
        
        if(feof(fp)) break;
    }
    fclose(fp);
}
    
void OBJObject::draw() 
{
	
    glMatrixMode(GL_MODELVIEW);
	// Push a save state onto the matrix stack, and multiply in the toWorld matrix
	glPushMatrix();
	glMultMatrixf(&(toWorld[0][0]));
    glBegin(GL_TRIANGLES);
	// Loop through all the vertices of this OBJ Object and render them
    for (unsigned int i = 0; i < indices.size(); i++)
    {
        int index1 = indices[i].x - 1;
        int index2 = indices[i].y - 1;
        int index3 = indices[i].z - 1;
        
        glm::vec3 normalized = glm::normalize(normals[index1]) * 0.5f + 0.5f;
        glColor3f(normalized.x, normalized.y, normalized.z);
        glVertex3f(vertices[index1].x, vertices[index1].y, vertices[index1].z);
        
        normalized = glm::normalize(normals[index2]) * 0.5f + 0.5f;
        glColor3f(normalized.x, normalized.y, normalized.z);
        glVertex3f(vertices[index2].x, vertices[index2].y, vertices[index2].z);
        
        normalized = glm::normalize(normals[index3]) * 0.5f + 0.5f;
        glColor3f(normalized.x, normalized.y, normalized.z);
        glVertex3f(vertices[index3].x, vertices[index3].y, vertices[index3].z);
        
    }
        
	glEnd();
    
	// Pop the save state off the matrix stack
	// This will undo the multiply we did earlier
	glPopMatrix();
    toWorld = glm::mat4(1.0f);
}
void OBJObject::update(glm::vec3 vec)
{
    spin(1.0f, vec);
    this->toWorld = tmx * rmx * smx * toWorld;
}

void OBJObject::spin(float deg, glm::vec3 vec)
{
    this->angle += deg;
    if (this->angle > 360.0f || this->angle <-360.0f) this->angle = 0.0f;
    this->rmx = glm::rotate(glm::mat4(1.0f), this->angle / 180.0f * glm::pi<float>(), vec);
}

void OBJObject::translate(glm::vec3 vec) {
    this->position += vec;
    this->tmx = glm::translate(glm::mat4(1.0f), this->position);
}

void OBJObject::scale(glm::vec3 vec) {
    this->size *= vec;
    this->smx = glm::scale(glm::mat4(1.0f), this->size);
}

void OBJObject::resetPos(glm::vec3 vec) {
    this->position = vec;
    translate(glm::vec3(0.0f));
}

void OBJObject::resetOriScale() {
    this->size = glm::vec3(1.0f);
    scale(glm::vec3(2.0f));
    this->angle = 0.0f;
}

glm::mat4 OBJObject::getWorld() {
    return this->toWorld;
}

std::vector<glm::vec3> OBJObject::getVertices() {
    return this->vertices;
}

std::vector<glm::vec3> OBJObject::getNormals() {
    return this->normals;
}

void OBJObject::resetToWorld() {
    this->toWorld = glm::mat4(1.0f);
}

void OBJObject::setCenter() {
    float max_x = 0, max_y = 0, max_z = 0, min_x = 0, min_y = 0, min_z = 0;
    for(auto it = vertices.begin(); it != vertices.end(); ++it) {
        glm::vec3 v = scale_mx * glm::vec4(*it, 1.0f);
        if(v.x > max_x) {
            max_x = v.x;
        }
        else if(v.x < min_x) {
            min_x = v.x;
        }
        
        if(v.y > max_y) {
            max_y = v.y;
        }
        else if(v.y < min_y) {
            min_y = v.y;
        }
        
        if(v.z > max_z) {
            max_z = v.z;
        }
        else if(v.z < min_z) {
            min_z = v.z;
        }
    }
    
    // Center the object
    glm::vec3 origin((max_x + min_x)/2.0f, (max_y + min_y)/2.0f, (max_z + min_z)/2.0f);
    this->origin_mx = glm::translate(glm::mat4(1.0f), origin * -1.0f);
    translate(origin * -1.0f);
}

void OBJObject::setScale() {
    float scale_factor = 0;
    
    for(unsigned int i = 0; i < vertices.size(); i++) {
        if(abs(vertices[i]).x > scale_factor) scale_factor = abs(vertices[i]).x;
        if(abs(vertices[i]).y > scale_factor) scale_factor = abs(vertices[i]).y;
        if(abs(vertices[i]).z > scale_factor) scale_factor = abs(vertices[i]).z;
    }
    
    scale_factor = 2.0f/scale_factor;
    this->scale_mx = glm::scale(glm::mat4(1.0f), glm::vec3(scale_factor));
    scale(glm::vec3(scale_factor));
}

void OBJObject::setBuffers() {
    vector<float> verts;
    for(auto it = vertices.begin(); it != vertices.end(); ++it) {
        verts.push_back(it->x);
        verts.push_back(it->y);
        verts.push_back(it->z);
    }
    
    unsigned int VAO, VBO, EBO;
    
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
    
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(verts), &verts[0], GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), &indices[0], GL_STATIC_DRAW);
    
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*) 0);
    glEnableVertexAttribArray(0);
    
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

