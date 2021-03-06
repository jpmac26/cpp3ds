////////////////////////////////////////////////////////////
//
// SFML - Simple and Fast Multimedia Library
// Copyright (C) 2007-2014 Laurent Gomila (laurent.gom@gmail.com)
//
// This software is provided 'as-is', without any express or implied warranty.
// In no event will the authors be held liable for any damages arising from the use of this software.
//
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it freely,
// subject to the following restrictions:
//
// 1. The origin of this software must not be misrepresented;
//    you must not claim that you wrote the original software.
//    If you use this software in a product, an acknowledgment
//    in the product documentation would be appreciated but is not required.
//
// 2. Altered source versions must be plainly marked as such,
//    and must not be misrepresented as being the original software.
//
// 3. This notice may not be removed or altered from any source distribution.
//
////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////
#include <cpp3ds/Graphics/Shader.hpp>
#include <cpp3ds/Graphics/Texture.hpp>
#include <cpp3ds/Resources.hpp>
#include <cpp3ds/OpenGL.hpp>
#include <cpp3ds/OpenGL/GLExtensions.hpp>
#include <cpp3ds/System/InputStream.hpp>
#include <cpp3ds/System/Err.hpp>
#ifndef EMULATION
#include <3ds.h>
#endif
#include <fstream>
#include <vector>


namespace
{
	// Read the contents of a file into an array of char
	bool getFileContents(const std::string& filename, std::vector<char>& buffer)
	{
		std::ifstream file(filename.c_str(), std::ios_base::binary);
		if (file)
		{
			file.seekg(0, std::ios_base::end);
			std::streamsize size = file.tellg();
			if (size > 0)
			{
				file.seekg(0, std::ios_base::beg);
				buffer.resize(static_cast<std::size_t>(size));
				file.read(&buffer[0], size);
			}
			buffer.push_back('\0');
			return true;
		}
		else
		{
			return false;
		}
	}
}


namespace cpp3ds
{
////////////////////////////////////////////////////////////
Shader::CurrentTextureType Shader::CurrentTexture;
Shader Shader::Default;


////////////////////////////////////////////////////////////
Shader::Shader() :
m_shaderProgram (0),
m_currentTexture(-1),
m_textures      (),
m_params        ()
{
}


////////////////////////////////////////////////////////////
Shader::~Shader()
{
    if (m_shaderProgram)
        glDeleteProgram(m_shaderProgram);
}


////////////////////////////////////////////////////////////
bool Shader::loadFromFile(const std::string& filename, Type type)
{
	std::string shaderfile;
#ifdef EMULATION
	// TODO: check for sdmc:/ filenames
	shaderfile = "../res/glsl/" + filename + ".glsl";
#else
	shaderfile = filename + ".shbin";
#endif

	// Read the file
	if (!getFileContents(shaderfile, m_shaderData))
	{
		err() << "Failed to open shader file \"" << shaderfile << "\"" << std::endl;
		return false;
	}

#ifdef EMULATION
	// Compile the shader program
    if (type == Vertex)
        return compile(&m_shaderData[0], NULL);
    else
        return compile(NULL, &m_shaderData[0]);
#else
	return loadBinary(reinterpret_cast<Uint8*>(&m_shaderData[0]), m_shaderData.size(), type);
#endif
}


////////////////////////////////////////////////////////////
bool Shader::loadFromFile(const std::string& vertexShaderFilename, const std::string& fragmentShaderFilename)
{
    return false;
}


////////////////////////////////////////////////////////////
bool Shader::loadFromMemory(const std::string& shader, Type type)
{
    return false;
}


////////////////////////////////////////////////////////////
bool Shader::loadFromMemory(const std::string& vertexShader, const std::string& fragmentShader)
{
    return false;
}


////////////////////////////////////////////////////////////
bool Shader::loadFromStream(InputStream& stream, Type type)
{
    return false;
}


////////////////////////////////////////////////////////////
bool Shader::loadFromStream(InputStream& vertexShaderStream, InputStream& fragmentShaderStream)
{
    return false;
}


////////////////////////////////////////////////////////////
void Shader::setParameter(const std::string& name, float x)
{
    if (m_shaderProgram)
    {
        // Enable program
        GLint program;
        glCheck(glGetIntegerv(GL_CURRENT_PROGRAM, &program));
        glCheck(glUseProgram(m_shaderProgram));

        // Get parameter location and assign it new values
        GLint location = getParamLocation(name);
        if (location != -1)
        {
            glCheck(glUniform1f(location, x));
        }

        // Disable program
        glCheck(glUseProgram(program));
    }
}


////////////////////////////////////////////////////////////
void Shader::setParameter(const std::string& name, float x, float y)
{
    if (m_shaderProgram)
    {
        // Enable program
        GLint program;
        glCheck(glGetIntegerv(GL_CURRENT_PROGRAM, &program));
        glCheck(glUseProgram(m_shaderProgram));

        // Get parameter location and assign it new values
        GLint location = getParamLocation(name);
        if (location != -1)
        {
            glCheck(glUniform2f(location, x, y));
        }

        // Disable program
        glCheck(glUseProgram(program));
    }
}


////////////////////////////////////////////////////////////
void Shader::setParameter(const std::string& name, float x, float y, float z)
{
    if (m_shaderProgram)
    {
        // Enable program
        GLint program;
        glCheck(glGetIntegerv(GL_CURRENT_PROGRAM, &program));
        glCheck(glUseProgram(m_shaderProgram));

        // Get parameter location and assign it new values
        GLint location = getParamLocation(name);
        if (location != -1)
        {
            glCheck(glUniform3f(location, x, y, z));
        }

        // Disable program
        glCheck(glUseProgram(program));
    }
}


////////////////////////////////////////////////////////////
void Shader::setParameter(const std::string& name, float x, float y, float z, float w)
{
    if (m_shaderProgram)
    {
        // Enable program
        GLint program;
        glCheck(glGetIntegerv(GL_CURRENT_PROGRAM, &program));
        glCheck(glUseProgram(m_shaderProgram));

        // Get parameter location and assign it new values
        GLint location = getParamLocation(name);
        if (location != -1)
        {
            glCheck(glUniform4f(location, x, y, z, w));
        }

        // Disable program
        glCheck(glUseProgram(program));
    }
}


////////////////////////////////////////////////////////////
void Shader::setParameter(const std::string& name, const Vector2f& v)
{
    setParameter(name, v.x, v.y);
}


////////////////////////////////////////////////////////////
void Shader::setParameter(const std::string& name, const Vector3f& v)
{
    setParameter(name, v.x, v.y, v.z);
}


////////////////////////////////////////////////////////////
void Shader::setParameter(const std::string& name, const Color& color)
{
    setParameter(name, color.r / 255.f, color.g / 255.f, color.b / 255.f, color.a / 255.f);
}


////////////////////////////////////////////////////////////
void Shader::setParameter(const std::string& name, const cpp3ds::Transform& transform)
{
    if (m_shaderProgram)
    {
        // Enable program
        GLint program;
        glCheck(glGetIntegerv(GL_CURRENT_PROGRAM, &program));
        glCheck(glUseProgram(m_shaderProgram));

        // Get parameter location and assign it new values
        GLint location = getParamLocation(name);
        if (location != -1)
        {
            glCheck(glUniformMatrix4fv(location, 1, GL_FALSE, transform.getMatrix()));
        }

        // Disable program
        glCheck(glUseProgram(program));
    }
}


////////////////////////////////////////////////////////////
void Shader::setParameter(const std::string& name, const Texture& texture)
{
    if (m_shaderProgram)
    {
        // Find the location of the variable in the shader
        int location = getParamLocation(name);
        if (location != -1)
        {
            // Store the location -> texture mapping
            TextureTable::iterator it = m_textures.find(location);
            if (it == m_textures.end())
            {
                // New entry, make sure there are enough texture units
                GLint maxUnits = 0;
                glCheck(glGetIntegerv(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, &maxUnits));

                if (m_textures.size() + 1 >= static_cast<std::size_t>(maxUnits))
                {
                    err() << "Impossible to use texture \"" << name << "\" for shader: all available texture units are used" << std::endl;
                    return;
                }

                m_textures[location] = &texture;
            }
            else
            {
                // Location already used, just replace the texture
                it->second = &texture;
            }
        }
    }
}


////////////////////////////////////////////////////////////
void Shader::setParameter(const std::string& name, CurrentTextureType)
{
    if (m_shaderProgram)
    {
        // Find the location of the variable in the shader
        m_currentTexture = getParamLocation(name);
    }
}



////////////////////////////////////////////////////////////
unsigned int Shader::getNativeHandle() const
{
    return m_shaderProgram;
}


////////////////////////////////////////////////////////////
void Shader::bind(const Shader* shader)
{
    // Make sure that we can use shaders
    if (!isAvailable())
    {
        err() << "Failed to bind or unbind shader: your system doesn't support shaders "
        << "(you should test Shader::isAvailable() before trying to use the Shader class)" << std::endl;
        return;
    }

    if (shader && shader->m_shaderProgram)
    {
        // Enable the program
//        glCheck(GLEXT_glUseProgramObject(castToGlHandle(shader->m_shaderProgram)));
        glCheck(glUseProgram(shader->m_shaderProgram));

        // Bind the textures
        shader->bindTextures();

        // Bind the current texture
//        if (shader->m_currentTexture != -1)
//            glCheck(GLEXT_glUniform1i(shader->m_currentTexture, 0));
    }
    else
    {
#ifdef EMULATION
		//Bind no shader
		glCheck(glUseProgram(0));
#else
        // Bind default shader
        glCheck(glUseProgram(Default.m_shaderProgram));

		// Bind the textures
		shader->bindTextures();
#endif
    }
}


////////////////////////////////////////////////////////////
bool Shader::isAvailable()
{
    return true;
}


////////////////////////////////////////////////////////////
bool Shader::compile(const char* vertexShaderCode, const char* fragmentShaderCode)
{
	// First make sure that we can use shaders
	if (!isAvailable())
	{
		err() << "Failed to create a shader: your system doesn't support shaders "
		<< "(you should test Shader::isAvailable() before trying to use the Shader class)" << std::endl;
		return false;
	}
#ifdef EMULATION
	// Destroy the shader if it was already created
	if (m_shaderProgram)
	{
		glCheck(glDeleteObjectARB(m_shaderProgram));
		m_shaderProgram = 0;
	}

	// Reset the internal state
	m_currentTexture = -1;
	m_textures.clear();
	m_params.clear();

	// Create the program
	GLhandleARB shaderProgram;
	glCheck(shaderProgram = glCreateProgramObjectARB());

	// Create the vertex shader if needed
	if (vertexShaderCode)
	{
		// Create and compile the shader
		GLhandleARB vertexShader;
		glCheck(vertexShader = glCreateShaderObjectARB(GL_VERTEX_SHADER));
		glCheck(glShaderSource(vertexShader, 1, &vertexShaderCode, NULL));
		glCheck(glCompileShader(vertexShader));

		// Check the compile log
		GLint success;
		glCheck(glGetObjectParameterivARB(vertexShader, GL_OBJECT_COMPILE_STATUS_ARB, &success));
		if (success == GL_FALSE)
		{
			char log[1024];
			glCheck(glGetInfoLogARB(vertexShader, sizeof(log), 0, log));
			err() << "Failed to compile vertex shader:" << std::endl
			<< log << std::endl;
			glCheck(glDeleteObjectARB(vertexShader));
			glCheck(glDeleteObjectARB(shaderProgram));
			return false;
		}

		// Attach the shader to the program, and delete it (not needed anymore)
		glCheck(glAttachObjectARB(shaderProgram, vertexShader));
		glCheck(glDeleteObjectARB(vertexShader));
	}

	// Link the program
	glCheck(glLinkProgram(shaderProgram));

	// Check the link log
	GLint success;
	glCheck(glGetObjectParameterivARB(shaderProgram, GL_OBJECT_LINK_STATUS_ARB, &success));
	if (success == GL_FALSE)
	{
		char log[1024];
		glCheck(glGetInfoLogARB(shaderProgram, sizeof(log), 0, log));
		err() << "Failed to link shader:" << std::endl
		<< log << std::endl;
		glCheck(glDeleteObjectARB(shaderProgram));
		return false;
	}

	m_shaderProgram = shaderProgram;

	// Force an OpenGL flush, so that the shader will appear updated
	// in all contexts immediately (solves problems in multi-threaded apps)
	glCheck(glFlush());

	return true;
#endif
	return false;
}


////////////////////////////////////////////////////////////
bool Shader::loadBinary(const Uint8* data, const Uint32 size, Type type)
{
#ifndef EMULATION
	if (!m_shaderProgram)
		m_shaderProgram = glCreateProgram();

    // Reset the internal state
    m_currentTexture = -1;
    m_textures.clear();
    m_params.clear();

	if (type == Vertex)
    	glProgramBinary(m_shaderProgram, GL_VERTEX_SHADER_BINARY, data, (GLsizei)size);
	else if (type == Geometry)
		glProgramBinary(m_shaderProgram, GL_GEOMETRY_SHADER_BINARY, data, (GLsizei)size);
#endif

    return true;
}


////////////////////////////////////////////////////////////
void Shader::bindTextures() const
{
}

////////////////////////////////////////////////////////////
int Shader::getParamLocation(const std::string& name)
{
    // Check the cache
    ParamTable::const_iterator it = m_params.find(name);
    if (it != m_params.end()) {
        // Already in cache, return it
        return it->second;
    }
    else {
        // Not in cache, request the location from OpenGL
        int location = glGetUniformLocation(m_shaderProgram, name.c_str());
        m_params.insert(std::make_pair(name, location));

        if (location == -1)
            err() << "Parameter \"" << name << "\" not found in shader" << std::endl;

        return location;
    }
}

} // namespace cpp3ds

