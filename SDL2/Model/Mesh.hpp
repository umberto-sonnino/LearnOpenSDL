//
//  Mesh.h
//  SDL2
//
//  Created by Umberto Sonnino on 24/4/17.
//  Copyright © 2017 Umberto Sonnino. All rights reserved.
//

#ifndef Mesh_h
#define Mesh_h
#pragma once

#include "Shader.h"
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>

using namespace std;

struct Vertex {
    glm::vec3 Position;
    glm::vec3 Normal;
    glm::vec2 TexCoords;
};

struct Texture {
    GLuint id;
    string type;
    aiString path;
};

class Mesh {
public:
    /* Mesh Data */
    vector<Vertex> vertices;
    vector<GLuint> indices;
    vector<Texture> textures;
    /* Functions */
    Mesh(vector<Vertex> vertices, vector<GLuint> indices, vector<Texture> textures)
    {
        this->vertices = vertices;
        this->indices = indices;
        this->textures = textures;
        
        this->setupMesh();
    }
    
    void Draw(Shader &shader)
    {
        GLuint diffuseNr = 1;
        GLuint specularNr = 1;
        
        for(GLuint i = 0; i < this->textures.size(); i++)
        {
            glActiveTexture(GL_TEXTURE0 + i);
            
            stringstream ss;
            string number;
            string name = this->textures[i].type;
            if ( name == "texture_diffuse" )
            {
                ss << diffuseNr++;
            }
            else if ( name == "texture_specular" )
            {
                ss << specularNr++;
            }
            number = ss.str();
            
            glUniform1f(glGetUniformLocation(shader.Program, ( "material." + name + number ).c_str()), i);
            glBindTexture(GL_TEXTURE_2D, this->textures[i].id);
        }
        
        glActiveTexture(GL_TEXTURE0);
        
        glBindVertexArray(this->VAO);
        glDrawElements(GL_TRIANGLES, this->indices.size(), GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
    }
    
private:
    GLuint VAO, VBO, EBO;
    void setupMesh()
    {
        glGenVertexArrays(1, &this->VAO);
        glGenBuffers(1, &this->VBO);
        glGenBuffers(1, &this->EBO);
        
        glBindVertexArray(this->VAO);
        glBindBuffer(GL_ARRAY_BUFFER, this->VAO);
        
        glBufferData(GL_ARRAY_BUFFER, this->vertices.size() * sizeof(Vertex), &this->vertices[0], GL_STATIC_DRAW);
        
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, this->indices.size() * sizeof(GLuint), &this->indices[0], GL_STATIC_DRAW);
        
        // Vertex Positions
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0,                // Index
                              3,                // Num Components per Vertex Attribute
                              GL_FLOAT,         // Type of Components
                              GL_FALSE,         // Normalized
                              sizeof(Vertex),   // Stride
                              (GLvoid*)0        // Offset of the first component
                              );
        
        // Normals
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, Normal));
        
        // Texture Coords
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, TexCoords));
        
        glBindVertexArray(0);
    }
};

#endif /* Mesh_h */
