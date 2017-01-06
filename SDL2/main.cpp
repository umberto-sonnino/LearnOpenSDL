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

#include "Shader.h"
#include "Camera.h"

using namespace std;

#define HEIGHT 600
#define WIDTH 800

SDL_Window *win;
GLuint vao, vbo, ebo;
GLuint texture1, texture2;
Shader *shader = nullptr;

GLdouble deltaTime = 0.0f;
//std::chrono::high_resolution_clock::time_point lastFrame = std::chrono::high_resolution_clock::now();
Uint64 timeNow = SDL_GetPerformanceCounter();
Uint64 timeLast = timeNow;
Uint64 frequency = SDL_GetPerformanceFrequency();

GLfloat lastX = 400, lastY = 300;
bool firstMouse = true;

Camera camera(glm::vec3(0.0f,0.0f, 3.0f));
bool keys[1024];

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
    deltaTime = GLdouble(timeNow - timeLast)*(10000) / GLdouble(frequency);
    
    return deltaTime;
}

void do_movement()
{
    if(keys[SDLK_w]){
        camera.ProcessKeyboard(FORWARD, deltaTime);
    }
    if(keys[SDLK_s]){
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    }
    if(keys[SDLK_a]){
        camera.ProcessKeyboard(LEFT, deltaTime);
    }
    if(keys[SDLK_d]){
        camera.ProcessKeyboard(RIGHT, deltaTime);
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
            printf("Setting %d DOWN\n", button);
        }
        if(event.type == SDL_KEYUP){
            printf("Setting %d UP\n", button);
            keys[button] = false;
        }
    }
    
    return true;
}

void scroll_callback(double yoffset)
{
    camera.ProcessMouseScroll(yoffset);
}

void mouse_callback(double xpos, double ypos)
{
    if(firstMouse)
    {
        lastX = xpos; lastY = ypos;
        firstMouse = false;
    }
    
    GLfloat xoffset = xpos - lastX;
    GLfloat yoffset = lastY - ypos;
    lastX = xpos;
    lastY = ypos;
    
    camera.ProcessMouseMovement(xoffset, yoffset);
}

void init_data()
{
    glViewport(0, 0, WIDTH, HEIGHT);
    glEnable(GL_DEPTH_TEST);
    
    GLfloat vertices[] = {
        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
        0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
        0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
        0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
        
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
        0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
        0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
        0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
        
        -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
        
        0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
        0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
        0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
        0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
        
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
        0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
        0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        
        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
        0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
        0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
        0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f
    };

    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    
    glBindVertexArray(vao);
    
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    
    //Position Attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GL_FLOAT), (GLvoid*)0);
    glEnableVertexAttribArray(0);
    //Tex Attribute
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GL_FLOAT), (GLvoid*)(3*sizeof(GL_FLOAT)) );
    glEnableVertexAttribArray(2);
    
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    
    int w, h;
    int w2, h2;
    unsigned char* image1 = SOIL_load_image("container.jpg", &w, &h, 0, SOIL_LOAD_RGB);
    unsigned char* image2 = SOIL_load_image("awesomeface.png", &w2, &h2, 0, SOIL_LOAD_RGB);
    
    glGenTextures(1, &texture1);
    glBindTexture(GL_TEXTURE_2D, texture1);
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    
    // This is where we tell OpenGL where to get the resource that we loaded up there
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, w, h, 0, GL_RGB, GL_UNSIGNED_BYTE, image1);
    glGenerateMipmap(GL_TEXTURE_2D);
    SOIL_free_image_data(image1);
    glBindTexture(GL_TEXTURE_2D, 0); // Unbind to prevent messing up
    
    glGenTextures(1, &texture2);
    glBindTexture(GL_TEXTURE_2D, texture2);
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    
    // This is where we tell OpenGL where to get the resource that we loaded up there
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, w2, h2, 0, GL_RGB, GL_UNSIGNED_BYTE, image2);
    glGenerateMipmap(GL_TEXTURE_2D);
    SOIL_free_image_data(image2);
    glBindTexture(GL_TEXTURE_2D, 0); // Unbind again
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
    
    SDL_ShowCursor(SDL_DISABLE);
    // TODO handle mouse
    
    SDL_GLContext context = SDL_GL_CreateContext(win);
    
    int majversion;
    SDL_GL_GetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, &majversion);
    cout << "GL Version " << glGetString(GL_VERSION) << endl;
    
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    
    // Buffer swap is syncronized with the monitor vertical refresh
    SDL_GL_SetSwapInterval(1);
    
    glewExperimental = GL_TRUE;
    glewInit();
    
    shader = new Shader("vertex.glsl", "fragment.glsl");
    init_data();
    
    // World space positions of our cubes
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
    
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    
    SDL_GL_SwapWindow(win);
 
    bool loop = true;
    
    while(loop)
    {
        SDL_Event event;
        
        deltaTime = getDelta();
        cout << deltaTime << endl;
        
        while(SDL_PollEvent(&event))
        {
            if(event.type == SDL_QUIT)
                loop = false;
            else if( event.type == SDL_KEYDOWN || event.type == SDL_KEYUP )
            {
                loop = handle_input(event);
                do_movement();
            }else if(event.type == SDL_MOUSEMOTION) {
                mouse_callback(event.motion.x, event.motion.y);
            }
            
            glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
            glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
            
            // Bind Textures using texture units
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, texture1);
            glUniform1i(glGetUniformLocation(shader->Program, "ourTexture1"), 0);
            glActiveTexture(GL_TEXTURE1);
            glBindTexture(GL_TEXTURE_2D, texture2);
            glUniform1i(glGetUniformLocation(shader->Program, "ourTexture2"), 1);
        
            // Shader is activated
            shader->Use();
            
            glm::mat4 view;
            glm::mat4 projection;
            view = camera.GetViewMatrix();
            projection = glm::perspective(camera.Zoom, (GLfloat)WIDTH/(GLfloat)HEIGHT, 0.1f, 1000.0f);
            
            GLint modelLoc = glGetUniformLocation(shader->Program, "model");
            GLint viewLoc = glGetUniformLocation(shader->Program, "view");
            GLint projLoc = glGetUniformLocation(shader->Program, "projection");
            
            glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
            glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));
            
            glBindVertexArray(vao);
            
            for(GLint i = 0; i < 10; ++i)
            {
                glm::mat4 model;
                model = glm::translate( model, cubePositions[i]);
                GLfloat angle = 20.0f * i;
                model = glm::rotate(model, angle, glm::vec3(1.0f, 0.3f, 0.5f));
                glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
                glDrawArrays(GL_TRIANGLES, 0, 36);
            }
            
            glBindVertexArray(0);
            
            SDL_GL_SwapWindow(win);
            SDL_Delay(1);
        }
        
    }
    
    glDeleteVertexArrays(1, &vao);
    glDeleteBuffers(1, &vbo);
    glDeleteBuffers(1, &ebo);
    
    SDL_GL_DeleteContext(context);
    SDL_DestroyWindow(win);
    SDL_Quit();
    
    
    return 0;
}
