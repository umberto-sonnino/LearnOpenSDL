#pragma once

#ifndef SHADER_H
#define	SHADER_H

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

using namespace std;

// Needed for the opengl headers
#include <GL/glew.h>

class Shader
{
public:
	GLuint Program;

    Shader();
	Shader(const GLchar* vertexShaderSourcePath, const GLchar* fragmentShaderSourcePath);

	void Use();
};

Shader::Shader(const GLchar* vertexPath, const GLchar* fragmentPath)
{
	string vertexCode, fragmentCode;
	ifstream vShaderFile, fShaderFile;

	// Ensures ifstream objects can throw exceptions
	vShaderFile.exceptions(ifstream::failbit | ifstream::badbit);
	fShaderFile.exceptions(ifstream::failbit | ifstream::badbit);

	try
	{
		vShaderFile.open(vertexPath);
		fShaderFile.open(fragmentPath);

		stringstream vShaderStream, fShaderStream;

		vShaderStream << vShaderFile.rdbuf();
		fShaderStream << fShaderFile.rdbuf();

		vShaderFile.close();
		fShaderFile.close();

		vertexCode = vShaderStream.str();
		fragmentCode = fShaderStream.str();
	}
	catch (ifstream::failure e)
	{
		cout << "ERROR::SHADER_FILE::READ_PROBLEM" << endl;
	}

	const GLchar* vShaderCode = vertexCode.c_str();
	const GLchar* fShaderCode = fragmentCode.c_str();

	GLuint vertex, fragment;
	GLint success;
	GLchar infoLog[512];

	vertex = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertex, 1, &vShaderCode, NULL);
	glCompileShader(vertex);

	glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);

	if (!success)
	{
		glGetShaderInfoLog(vertex, 512, NULL, infoLog);
		cout << "ERROR::VERTEX_SHADER::COMPILATION_FAILED\n" << infoLog << endl;
	};

	fragment = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragment, 1, &fShaderCode, NULL);
	glCompileShader(fragment);

	glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetProgramInfoLog(fragment, 512, NULL, infoLog);
		cout << "ERROR::FRAGMENT_SHADER::COMPILATION_FAILED\n" << infoLog << endl;
	};

	this->Program = glCreateProgram();
	glAttachShader(this->Program, vertex);
	glAttachShader(this->Program, fragment);

	glLinkProgram(this->Program);

	glGetProgramiv(this->Program, GL_LINK_STATUS, &success);
	if(!success)
	{
		glGetProgramInfoLog(this->Program, 512, NULL, infoLog);
		cout << "ERROR::PROGRAM_LINKING\n" << infoLog << endl;
	}

	glDeleteShader(vertex);
	glDeleteShader(fragment);
}

void Shader::Use()
{
	glUseProgram(this->Program);
}

#endif

