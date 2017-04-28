//
//  Model.h
//  SDL2
//
//  Created by Umberto Sonnino on 28/4/17.
//  Copyright © 2017 Umberto Sonnino. All rights reserved.
//

#ifndef Model_h
#define Model_h

#pragma once
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <map>
#include <vector>

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <SOIL/SOIL.h>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "Mesh.hpp"
#include "Shader.h"

using namespace std;

GLint TextureFromFile (const char* path, string directory);

class Model
{
public:
    Model (GLchar* path)
    {
        this->loadModel (path);
    }
    
    void Draw (Shader shader)
    {
        for ( GLuint i = 0; i < this->meshes.size(); i++ ) {
            this->meshes[i].Draw (shader);
        }
    }
private:
    vector<Mesh> meshes;
    string directory;
    
    void loadModel (string path)
    {
        Assimp::Importer import;
        
        const aiScene* scene = import.ReadFile (path, aiProcess_Triangulate | aiProcess_FlipUVs);
        
        if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
        {
            cout << "ERROR:ASSIMP::" << import.GetErrorString() << endl;
            
            return;
        }
        
        this->directory = path.substr (0, path.find_last_of('/'));
        
        this->processNode (scene->mRootNode, scene);
    }
    
    void processNode (aiNode* node, const aiScene* scene)
    {
        for (GLuint i = 0; i < node->mNumMeshes; i++)
        {
            aiMesh* mesh = scene->mMeshes [node->mMeshes[i]];
            this->meshes.push_back (this->processMesh(mesh,scene));
        }
        
        for (GLuint i = 0; i < node->mNumChildren; i++)
        {
            this->processNode (node->mChildren[i], scene);
        }
    }
    
    Mesh processMesh (aiMesh* mesh, const aiScene* scene)
    {
        vector<Vertex> vertices;
        vector<GLuint> indices;
        vector<Texture> textures;
        
        for (GLuint i = 0; i < mesh->mNumVertices; i++) {
            Vertex vertex;
            
            // Process vertex positions, normals and coords
            glm::vec3 vector;
            vector.x = mesh->mVertices[i].x;
            vector.y = mesh->mVertices[i].y;
            vector.z = mesh->mVertices[i].z;
            vertex.Position = vector;
            
            vector.x = mesh->mNormals[i].x;
            vector.y = mesh->mNormals[i].y;
            vector.z = mesh->mNormals[i].z;
            vertex.Normal = vector;
            
            if (mesh->mTextureCoords[0])
            {
                glm::vec2 vec;
                vec.x = mesh->mTextureCoords[0][i].x;
                vec.y = mesh->mTextureCoords[0][i].y;
                vertex.TexCoords = vec;
            }
            else
            {
                vertex.TexCoords = glm::vec2(0.0f, 0.0f);
            }
            
            vertices.push_back (vertex);
        }
        
        // Process indices
        for (GLuint i = 0; i < mesh->mNumFaces; i++)
        {
            aiFace face = mesh->mFaces[i];
            
            for(GLuint j = 0; j < face.mNumIndices; j++)
            {
                indices.push_back (face.mIndices[j]);
            }
        }
        
        // Process Material
        if(mesh->mMaterialIndex >= 0)
        {
            aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
            vector<Texture> diffuseMaps = this->loadMaterialTexture (material, aiTextureType_DIFFUSE, "texture_diffuse");
            textures.insert (textures.end(), diffuseMaps.begin(), diffuseMaps.end());
            
            vector<Texture> specularMaps = this->loadMaterialTexture (material, aiTextureType_SPECULAR, "texture_specular");
            textures.insert (textures.end(), specularMaps.begin(), specularMaps.end());
        }
        
        return Mesh (vertices, indices, textures);
        
    }
    
    vector<Texture> loadMaterialTexture (aiMaterial* mat, aiTextureType type, string typeName)
    {
        vector<Texture> textures;
        for (GLuint i = 0; i < mat->GetTextureCount(type); i++)
        {
            aiString str;
            mat->GetTexture (type, i, &str);
            Texture texture;
            texture.id = TextureFromFile (str.C_Str(), this->directory);
            texture.type = typeName;
            texture.path = str;
            textures.push_back (texture);
        }
    }
};

GLint TextureFromFile (const char* path, string directory)
{
    string filename = string (path);
    filename = directory + '/' + filename;
    
    GLuint textureID;
    glGenTextures (1, &textureID);
    int width, height;
    
    unsigned char* image = SOIL_load_image (filename.c_str(), &width, &height, 0, SOIL_LOAD_RGB);
    
    glBindTexture (GL_TEXTURE_2D, textureID);
    glTexImage2D (GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
    glGenerateMipmap (GL_TEXTURE_2D);
    
    glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    
    glBindTexture (GL_TEXTURE_2D, 0);
    SOIL_free_image_data (image);
    
    return textureID;
    
}


#endif /* Model_h */
