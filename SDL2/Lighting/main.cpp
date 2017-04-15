//
//  main.cpp
//  SDL2
//
//  Created by Umberto Sonnino on 3/12/16.
//  Copyright © 2016 Umberto Sonnino. All rights reserved.
//
#include <SDL2/SDL.h>
#include <GL/glew.h>
#include <SOIL/SOIL.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>
#include <chrono>
#include <unistd.h>

#include "Shader.h"
#include "Camera.h"

using namespace std;

#define HEIGHT 600
#define WIDTH 800

SDL_Window *win;
GLuint vao, lightVAO, vbo;
GLuint diffuseMap, specularMap;
Shader *shader = nullptr;
Shader *lightingShader = nullptr;

GLdouble deltaTime = 0.0f;
//std::chrono::high_resolution_clock::time_point lastFrame = std::chrono::high_resolution_clock::now();
Uint64 timeNow = SDL_GetPerformanceCounter();
Uint64 timeLast = timeNow;
Uint64 frequency = SDL_GetPerformanceFrequency();

GLfloat lastX = 0, lastY = 0;
bool firstMouse = true;

Camera camera(glm::vec3(0.0f,0.0f, 3.0f));
bool keys[1024];

glm::vec3 lightPos(1.2f, 1.0f, 2.0f);

GLdouble getDelta()
{
#if 0
    auto timeCurrent = std::chrono::high_resolution_clock::now();
    auto timeDiff = std::chrono::duration_cast<std::chrono::nanoseconds> (timeCurrent - lastFrame);
    deltaTime = timeDiff.count();
    deltaTime /= 100000; // Nanoseconds into seconds
    
    lastFrame = timeCurrent;
#endif
    
    timeLast = timeNow;
    timeNow = SDL_GetPerformanceCounter();
    deltaTime = GLdouble(timeNow - timeLast)*(1000) / GLdouble(frequency);
    
    return deltaTime;
}

void do_movement(GLdouble delta)
{
    if(keys[SDLK_w]){
        camera.ProcessKeyboard(FORWARD, delta);
    }
    if(keys[SDLK_s]){
        camera.ProcessKeyboard(BACKWARD, delta);
    }
    if(keys[SDLK_a]){
        camera.ProcessKeyboard(LEFT, delta);
    }
    if(keys[SDLK_d]){
        camera.ProcessKeyboard(RIGHT, delta);
    }
}

bool handle_input(SDL_Event event)
{
    SDL_Keycode button = event.key.keysym.sym;
    if(event.type == SDL_KEYDOWN && button == SDLK_ESCAPE)
        return false;
    if(button > 0 && button < 1024){
        if(event.type == SDL_KEYDOWN){
            keys[button] = true;
//            printf("Setting %d DOWN\n", button);
        }
        if(event.type == SDL_KEYUP){
//            printf("Setting %d UP\n", button);
            keys[button] = false;
        }
    }
    
    return true;
}

void scroll_callback(const double yoffset)
{
    double invert = -yoffset;
    invert  /= 50;
    
    camera.ProcessMouseScroll(invert);
}

void init_data()
{
    glViewport(0, 0, WIDTH, HEIGHT);
    glEnable(GL_DEPTH_TEST);
    
    GLfloat vertices[] = {
        // Positions          // Normals           // Texture Coords
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,
        0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  0.0f,
        0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
        0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,
        
        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,
        0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  0.0f,
        0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
        0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,
        
        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
        -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
        -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
        
        0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
        0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
        0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
        0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
        0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
        0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
        
        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,
        0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  1.0f,
        0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
        0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,
        
        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f,
        0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  1.0f,
        0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
        0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  0.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f
    };
    
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    
    glBindVertexArray(vao);
    
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    
    // Position Attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GL_FLOAT), (GLvoid*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GL_FLOAT), (GLvoid*)( 3 * sizeof(GLfloat) ));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GL_FLOAT), (GLvoid*)( 6 * sizeof(GLfloat) ));
    glEnableVertexAttribArray(2);
    glBindVertexArray(0);
    
    glGenVertexArrays(1, &lightVAO);
    glBindVertexArray(lightVAO);
    // We only need to bind the vbo, since the container has already been filled
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)0);
    glEnableVertexAttribArray(0);
    glBindVertexArray(0); // Unbind
    
    // Handle Textures
    glGenTextures(1, &diffuseMap);
    glGenTextures(1, &specularMap);
    int width, height;
    unsigned char* image;
    
    image = SOIL_load_image("container2.png", &width, &height, 0, SOIL_LOAD_RGB);
    glBindTexture(GL_TEXTURE_2D, diffuseMap);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
    glGenerateMipmap(GL_TEXTURE_2D);
    SOIL_free_image_data(image);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST_MIPMAP_NEAREST);
    
    // Specular Map
    image = SOIL_load_image("container2_specular.png", &width, &height, 0, SOIL_LOAD_RGB);
    glBindTexture(GL_TEXTURE_2D, specularMap);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
    glGenerateMipmap(GL_TEXTURE_2D);
    SOIL_free_image_data(image);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST_MIPMAP_NEAREST);
    
    glBindTexture(GL_TEXTURE_2D, 0);
    lightingShader->Use();
    glUniform1i(glGetUniformLocation(lightingShader->Program, "material.diffuse"), 0);
    glUniform1i(glGetUniformLocation(lightingShader->Program, "material.specular"), 1);

}

int main(int argc, const char * argv[]) {
    
    if(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER))
    {
        cout << "SDL failed to initialize: " << SDL_GetError() << endl;
        return -1;
    }
    
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
    
    win = SDL_CreateWindow("SDL Window", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WIDTH, HEIGHT, SDL_WINDOW_OPENGL);
    
    if(win == nullptr)
    {
        cout << "SDL_CreateWindow error: " << SDL_GetError() << endl;
        SDL_Quit();
        return -1;
    }
    
    SDL_GLContext context = SDL_GL_CreateContext(win);
    
    int majversion;
    SDL_GL_GetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, &majversion);
    cout << "GL Version " << glGetString(GL_VERSION) << endl;
    
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    
    // Buffer swap is syncronized with the monitor vertical refresh
    SDL_GL_SetSwapInterval(1);
    
    glewExperimental = GL_TRUE;
    glewInit();

    // shader is associated with the 'lamp' object
    shader = new Shader("vertex.glsl", "fragment.glsl");
    // lightingShader is the shader for the 'lighted' objects
    lightingShader = new Shader("lightingVs.glsl", "lightingFs.glsl");
    init_data();
    
    SDL_GL_SwapWindow(win);
    // Trap mouse inside the window
    SDL_SetRelativeMouseMode(SDL_TRUE);
    
    glm::vec3 cubePositions[] = {
        glm::vec3( 0.0f,  0.0f,  0.0f),
        glm::vec3( 2.0f,  5.0f, -15.0f),
        glm::vec3(-1.5f, -2.2f, -2.5f),
        glm::vec3(-3.8f, -2.0f, -12.3f),
        glm::vec3( 2.4f, -0.4f, -3.5f),
        glm::vec3(-1.7f,  3.0f, -7.5f),
        glm::vec3( 1.3f, -2.0f, -2.5f),
        glm::vec3( 1.5f,  2.0f, -2.5f),
        glm::vec3( 1.5f,  0.2f, -1.5f),
        glm::vec3(-1.3f,  1.0f, -1.5f)
    };
    
    
    bool loop = true;
    
    while(loop)
    {
        SDL_Event event;
        
        timeLast = timeNow;
        timeNow = SDL_GetPerformanceCounter();
        deltaTime = GLdouble(timeNow - timeLast)*(1000) / GLdouble(SDL_GetPerformanceFrequency());
        
        while(SDL_PollEvent(&event))
        {
            switch (event.type) {
                case SDL_QUIT:
                    loop = false;
                    break;
                case SDL_KEYDOWN:
                case SDL_KEYUP:
                    loop = handle_input(event);
                    do_movement(deltaTime / 150);
                    break;
                case SDL_MOUSEMOTION:
                    camera.ProcessMouseMovement(event.motion.xrel, -event.motion.yrel);
                    break;
                case SDL_MOUSEWHEEL:
                    scroll_callback(event.wheel.y);
                    break;
                default:
                    break;
            }
        }
        
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
        
        // First "Bigger-Cube" Shader is activated
        lightingShader->Use();
        
        GLint lightDirLoc = glGetUniformLocation(lightingShader->Program, "light.direction");
        GLint viewPosLoc = glGetUniformLocation(lightingShader->Program, "viewPos");
        glUniform3f(lightDirLoc, -1.f, -1.0f, -0.3f);
        glUniform3f(viewPosLoc, camera.Position.x, camera.Position.y, camera.Position.z);
        
        glUniform3f(glGetUniformLocation(lightingShader->Program, "light.ambient"), 0.2f, 0.2f, 0.2f);
        glUniform3f(glGetUniformLocation(lightingShader->Program, "light.diffuse"), 0.5f, 0.5f, 0.5f);
        glUniform3f(glGetUniformLocation(lightingShader->Program, "light.specular"), 1.0f, 1.0f, 1.0f);
        
        glUniform1f(glGetUniformLocation(lightingShader->Program, "material.shininess"), 64.0f);
        
        glm::mat4 view;
        view = camera.GetViewMatrix();
        glm::mat4 projection = glm::perspective(camera.Zoom, (GLfloat)WIDTH/(GLfloat)HEIGHT, 0.1f, 100.0f);
        
        GLint modelLoc = glGetUniformLocation(lightingShader->Program, "model");
        GLint viewLoc = glGetUniformLocation(lightingShader->Program, "view");
        GLint projLoc = glGetUniformLocation(lightingShader->Program, "projection");
        
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));
        
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, diffuseMap);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, specularMap);
        
        glBindVertexArray(vao);
        glm::mat4 model;
        
        for(GLint i = 0; i < 10; ++i)
        {
            model = glm::mat4();
            model = glm::translate(model, cubePositions[i]);
            GLfloat angle = 20.0f * i;
            model = glm::rotate(model, angle, glm::vec3(1.0f, 0.3f, 0.5f));
            glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }
        
        glBindVertexArray(0);
        
        // Lamp Object Shader is activated
//        shader->Use();
//        modelLoc = glGetUniformLocation(shader->Program, "model");
//        viewLoc = glGetUniformLocation(shader->Program, "view");
//        projLoc = glGetUniformLocation(shader->Program, "projection");
//        
//        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
//        glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));
//        
//        model = glm::mat4();
//        model = glm::translate(model, lightPos);
//        model = glm::scale(model, glm::vec3(0.2f)); // Smaller cube
//        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
//        
//        glBindVertexArray(lightVAO);
//        glDrawArrays(GL_TRIANGLES, 0, 36);
//        glBindVertexArray(0);
        
        SDL_GL_SwapWindow(win);
        SDL_Delay(1000 / 60);
        
    }
    
    glDeleteVertexArrays(1, &vao);
    glDeleteBuffers(1, &vbo);
    
    SDL_GL_DeleteContext(context);
    SDL_DestroyWindow(win);
    SDL_Quit();
    
    
    return 0;
}
