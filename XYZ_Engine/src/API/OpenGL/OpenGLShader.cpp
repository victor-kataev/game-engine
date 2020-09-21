#include "stdafx.h"
#include "OpenGLShader.h"

#include <glm/gtc/type_ptr.hpp>
#include <fstream>
#include <array>
#include <GL/glew.h>


namespace XYZ {

	static std::string CutArrayIndexing(const std::string& name)
	{
		std::string str;
		int count = 0;
		while (name[count] != '[')
			count++;

		str = name.substr(0, count);
		return str;
	}

	static GLenum ShaderTypeFromString(const std::string& type)
	{
		if (type == "vertex")
			return GL_VERTEX_SHADER;
		if (type == "fragment" || type == "pixel")
			return GL_FRAGMENT_SHADER;
		if (type == "geometry")
			return GL_GEOMETRY_SHADER;
		if (type == "compute")
			return GL_COMPUTE_SHADER;

		XYZ_ASSERT(false, "Unknown shader type!");
		return 0;
	}
	OpenGLShader::OpenGLShader(const std::string& path)
		: m_NumTakenTexSlots(0), m_UniformsSize(0), m_Textures(0), m_Path(path)
	{
		std::string source = readFile(path);
		parsePredefVariables("Assets/Shaders/Variables/PredefinedVariables.glsl", source);
		auto shaderSources = preProcess(source);
		compile(shaderSources);
		parseUniforms();
		parseSubRoutines();
	}
	OpenGLShader::OpenGLShader(const std::string& name, const std::string& path)
		: m_Name(name), m_NumTakenTexSlots(0), m_UniformsSize(0), m_Textures(0), m_Path(path)
	{
		std::string source = readFile(m_Path);
		parsePredefVariables("Assets/Shaders/Variables/PredefinedVariables.glsl", source);
		auto shaderSources = preProcess(source);
		compile(shaderSources);
		parseUniforms();
		parseSubRoutines();
	}
	OpenGLShader::~OpenGLShader()
	{
		glDeleteProgram(m_RendererID);
	}
	void OpenGLShader::Bind() const
	{
		glUseProgram(m_RendererID);
	}
	void OpenGLShader::Compute(unsigned int groupX, unsigned int groupY, unsigned int groupZ) const
	{
		XYZ_ASSERT(m_Type == ShaderProgramType::COMPUTE, "Calling compute on non compute shader");
		glDispatchCompute(groupX, groupY, groupZ);
		glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
	}
	void OpenGLShader::Unbind() const
	{
		glUseProgram(0);
	}

	const Uniform* OpenGLShader::FindUniform(const std::string& name)
	{
		for (size_t i = 0; i < m_Uniforms.size(); ++i)
		{
			if (m_Uniforms[i].name == name)
				return &m_Uniforms[i];
		}
		return nullptr;
	}


	const TextureUniform* OpenGLShader::FindTexture(const std::string& name)
	{
		for (size_t i = 0; i < m_Textures.size(); ++i)
		{
			if (m_Textures[i].name == name)
				return &m_Textures[i];
		}
		return nullptr;
	}

	void OpenGLShader::addUniform(UniformDataType type, unsigned int size, unsigned int offset, const std::string& name, unsigned int count)
	{
		int32_t id = glGetUniformLocation(m_RendererID, name.c_str());
		if (id != -1)
		{
			if (type == UniformDataType::SAMPLER2D)
			{
				TextureUniform texture;
				texture.count = count;
				texture.location = id;
				texture.name = name;
				texture.slot = m_NumTakenTexSlots;

				XYZ_ASSERT(texture.slot + (count - 1) < sc_MaxTextureSlots, "It is possible to have only ", sc_MaxTextureSlots, " textures per shader");
				glUseProgram(m_RendererID);
				if (count > 1)
				{
					texture.name = CutArrayIndexing(texture.name);
					int32_t slots[32];
					for (int32_t i = (int32_t)texture.slot; i < (int32_t)count + (int32_t)texture.slot; ++i)
						slots[i] = i;
					glUniform1iv(texture.location, texture.count, &slots[texture.slot]);
				}
				else
					glUniform1i(id, texture.slot);


				m_NumTakenTexSlots += count;
				m_Textures.push_back(texture);
			}
			else
			{
				Uniform uniform;
				uniform.size = size;
				uniform.offset = offset;
				uniform.type = type;
				uniform.location = id;
				uniform.name = name;
				uniform.count = count;
				if (uniform.count > 0)
					uniform.isArray = true;
				m_Uniforms.push_back(uniform);
			}
		}
	}

	void OpenGLShader::setUniform(Uniform* uniform, unsigned char* data)
	{
		switch (uniform->type)
		{
		case UniformDataType::FLOAT:
			uploadFloat(uniform->location, *(float*)& data[uniform->offset]);
			break;
		case UniformDataType::FLOAT_VEC2:
			uploadFloat2(uniform->location, *(glm::vec2*) & data[uniform->offset]);
			break;
		case UniformDataType::FLOAT_VEC3:
			uploadFloat3(uniform->location, *(glm::vec3*) & data[uniform->offset]);
			break;
		case UniformDataType::FLOAT_VEC4:
			uploadFloat4(uniform->location, *(glm::vec4*) & data[uniform->offset]);
			break;
		case UniformDataType::INT:
			uploadInt(uniform->location, *(int*)& data[uniform->offset]);
			break;
		case UniformDataType::FLOAT_MAT4:
			uploadMat4(uniform->location, *(glm::mat4*) & data[uniform->offset]);
			break;
		};
	}

	void OpenGLShader::setUniformArr(Uniform* uniform, unsigned char* data)
	{
		switch (uniform->type)
		{
		case UniformDataType::FLOAT:
			uploadFloatArr(uniform->location, (float*)& data[uniform->offset], uniform->count);
			break;
		case UniformDataType::FLOAT_VEC2:
			uploadFloat2Arr(uniform->location, *(glm::vec2*) & data[uniform->offset], uniform->count);
			break;
		case UniformDataType::FLOAT_VEC3:
			uploadFloat3Arr(uniform->location, *(glm::vec3*) & data[uniform->offset], uniform->count);
			break;
		case UniformDataType::FLOAT_VEC4:
			uploadFloat4Arr(uniform->location, *(glm::vec4*) & data[uniform->offset], uniform->count);
			break;
		case UniformDataType::INT:
			uploadIntArr(uniform->location, (int*)& data[uniform->offset], uniform->count);
			break;
		case UniformDataType::FLOAT_MAT4:
			uploadMat4Arr(uniform->location, *(glm::mat4*) & data[uniform->offset], uniform->count);
			break;
		};
	}

	void OpenGLShader::SetUniforms(unsigned char* data)
	{
		for (auto uniform : m_Uniforms)
		{
			if (uniform.isArray)
				setUniformArr(&uniform, data);
			else
				setUniform(&uniform, data);
		}
	}

	void OpenGLShader::SetSubRoutine(const std::string& name)
	{
		for (auto& routine : m_Routines)
		{
			if (routine.activeSubRoutine.name == name)
				return;
			for (auto& sub : routine.subRoutines)
			{
				if (sub.name == name)
				{
					routine.activeSubRoutine = sub;
					return;
				}
			}
		}
		XYZ_ASSERT(false, "Routine does not exist ", name.c_str());
	}

	void OpenGLShader::UploadRoutines()
	{
		for (size_t i = 0; i < m_Routines.size(); ++i)
		{
			SubRoutine routine = m_Routines[i].activeSubRoutine;
			glUniformSubroutinesuiv(routine.shaderType, 1, &routine.index);
		}
	}


	void OpenGLShader::Reload()
	{
		XYZ_LOG_WARN("Reloading shader ", m_Name.c_str());
		m_Uniforms.clear();
		m_Textures.clear();
		m_Routines.clear();
		m_UniformsSize = 0;
		m_NumTakenTexSlots = 0;

		std::string source = readFile(m_Path);
		parsePredefVariables("Assets/Shaders/Variables/PredefinedVariables.glsl", source);
		auto shaderSources = preProcess(source);
		compile(shaderSources);
		parseUniforms();
		parseSubRoutines();

		for (size_t i = 0; i < m_ShaderReloadCallbacks.size(); ++i)
			m_ShaderReloadCallbacks[i]();
	}

	void OpenGLShader::AddReloadCallback(std::function<void()> callback)
	{
		m_ShaderReloadCallbacks.push_back(callback);
	}

	void OpenGLShader::uploadInt(uint32_t loc, int value)
	{
		glUniform1i(loc, value);
	}

	void OpenGLShader::uploadFloat(uint32_t loc, float value)
	{
		glUniform1f(loc, value);
	}
	void OpenGLShader::uploadFloat2(uint32_t loc, const glm::vec2& value)
	{
		glUniform2f(loc, value.x, value.y);
	}
	void OpenGLShader::uploadFloat3(uint32_t loc, const glm::vec3& value)
	{
		glUniform3f(loc, value.x, value.y, value.z);
	}
	void OpenGLShader::uploadFloat4(uint32_t loc, const glm::vec4& value)
	{
		glUniform4f(loc, value.x, value.y, value.z, value.w);
	}
	void OpenGLShader::uploadMat3(uint32_t loc, const glm::mat3& matrix)
	{
		glUniformMatrix3fv(loc, 1, GL_FALSE, glm::value_ptr(matrix));
	}
	void OpenGLShader::uploadMat4(uint32_t loc, const glm::mat4& matrix)
	{
		glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(matrix));
	}
	void OpenGLShader::uploadIntArr(uint32_t loc, int* values, uint32_t count)
	{
		glUniform1iv(loc, count, values);
	}
	void OpenGLShader::uploadFloatArr(uint32_t loc, float* values, uint32_t count)
	{
		glUniform1fv(loc, count, values);
	}
	void OpenGLShader::uploadFloat2Arr(uint32_t loc, const glm::vec2& value, uint32_t count)
	{
		glUniform2fv(loc, count, (float*)& value);
	}
	void OpenGLShader::uploadFloat3Arr(uint32_t loc, const glm::vec3& value, uint32_t count)
	{
		glUniform3fv(loc, count, (float*)& value);
	}
	void OpenGLShader::uploadFloat4Arr(uint32_t loc, const glm::vec4& value, uint32_t count)
	{
		glUniform4fv(loc, count, (float*)& value);
	}
	void OpenGLShader::uploadMat3Arr(uint32_t loc, const glm::mat3& matrix, uint32_t count)
	{
		glUniformMatrix3fv(loc, count, GL_FALSE, glm::value_ptr(matrix));
	}
	void OpenGLShader::uploadMat4Arr(uint32_t loc, const glm::mat4& matrix, uint32_t count)
	{
		glUniformMatrix4fv(loc, count, GL_FALSE, glm::value_ptr(matrix));
	}
	std::string OpenGLShader::readFile(const std::string& filepath)
	{
		std::string result;
		std::ifstream in(filepath, std::ios::in | std::ios::binary);
		if (in)
		{
			in.seekg(0, std::ios::end);
			result.resize(in.tellg());
			in.seekg(0, std::ios::beg);
			in.read(&result[0], result.size());
			in.close();
		}
		else
			XYZ_LOG_ERR("Could not open file ", filepath.c_str());


		return result;
	}
	std::unordered_map<unsigned int, std::string> OpenGLShader::preProcess(const std::string& source)
	{
		std::unordered_map<GLenum, std::string> shaderSources;

		const char* typeToken = "#type";
		size_t typeTokenLength = strlen(typeToken);
		size_t pos = source.find(typeToken, 0);
		while (pos != std::string::npos)
		{
			size_t eol = source.find_first_of("\r\n", pos);
			XYZ_ASSERT(eol != std::string::npos, "Syntax error");
			size_t begin = pos + typeTokenLength + 1;
			std::string type = source.substr(begin, eol - begin);
			XYZ_ASSERT(ShaderTypeFromString(type), "Invalid shader type specified");

			size_t nextLinePos = source.find_first_not_of("\r\n", eol);
			pos = source.find(typeToken, nextLinePos);
			shaderSources[ShaderTypeFromString(type)] = source.substr(nextLinePos, pos - (nextLinePos == std::string::npos ? source.size() - 1 : nextLinePos));
			if (type == "compute")
				m_Type = ShaderProgramType::COMPUTE;
			else
				m_Type = ShaderProgramType::RENDER;
		}
		return shaderSources;
	}
	void OpenGLShader::compile(const std::unordered_map<unsigned int, std::string>& shaderSources)
	{
		GLuint program = glCreateProgram();

		XYZ_ASSERT(shaderSources.size() <= 3, "We only support 3 shaders for now");
		std::array<GLenum, 3> glShaderIDs;

		int glShaderIDIndex = 0;
		for (auto& kv : shaderSources)
		{
			GLenum type = kv.first;
			const std::string& source = kv.second;

			GLuint shader = glCreateShader(type);

			const GLchar* sourceCStr = source.c_str();
			glShaderSource(shader, 1, &sourceCStr, 0);

			glCompileShader(shader);

			GLint isCompiled = 0;
			glGetShaderiv(shader, GL_COMPILE_STATUS, &isCompiled);

			if (isCompiled == GL_FALSE)
			{
				GLint maxLength = 0;
				glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &maxLength);

				std::vector<GLchar> infoLog(maxLength);
				glGetShaderInfoLog(shader, maxLength, &maxLength, &infoLog[0]);

				glDeleteShader(shader);

				XYZ_LOG_ERR(infoLog.data());
				XYZ_ASSERT(false, "Shader compilation failure!");
				break;
			}

			glAttachShader(program, shader);
			glShaderIDs[glShaderIDIndex++] = shader;
		}
		if (m_RendererID)
			glDeleteProgram(m_RendererID);

		// Link our program
		m_RendererID = program;
		glLinkProgram(m_RendererID);

		GLint isLinked = 0;
		glGetProgramiv(m_RendererID, GL_LINK_STATUS, (int*)& isLinked);

		if (isLinked == GL_FALSE)
		{
			GLint maxLength = 0;
			glGetProgramiv(m_RendererID, GL_INFO_LOG_LENGTH, &maxLength);

			// The maxLength includes the NULL character
			std::vector<GLchar> infoLog(maxLength);
			glGetProgramInfoLog(m_RendererID, maxLength, &maxLength, &infoLog[0]);

			// We don't need the program anymore.
			glDeleteProgram(m_RendererID);

			for (size_t i = 0; i < glShaderIDIndex; ++i)
				glDeleteShader(glShaderIDs[i]);


			XYZ_LOG_ERR(infoLog.data());
			XYZ_ASSERT(false, "Shader link failure!");
			return;
		}

		for (size_t i = 0; i < glShaderIDIndex; ++i)
		{
			glDetachShader(m_RendererID, glShaderIDs[i]);
			glDeleteShader(glShaderIDs[i]);
		}
	}
	void OpenGLShader::parseUniforms()
	{
		GLint count;
		GLint size;
		GLenum type;
		const GLsizei bufSize = 50;
		GLchar name[bufSize];
		GLsizei length;

		// Parse uniforms
		glGetProgramiv(m_RendererID, GL_ACTIVE_UNIFORMS, &count);

		int offset = 0;
		for (int i = 0; i < count; i++)
		{
			int sizeUni = 0;
			glGetActiveUniform(m_RendererID, (GLuint)i, bufSize, &length, &size, &type, name);
			UniformDataType uniType = UniformDataType::NONE;
			switch (type)
			{
			case GL_FLOAT:	    uniType = UniformDataType::FLOAT;
				sizeUni = sizeof(float);
				break;
			case GL_FLOAT_VEC2: uniType = UniformDataType::FLOAT_VEC2;
				sizeUni = 2 * sizeof(float);
				break;
			case GL_FLOAT_VEC3: uniType = UniformDataType::FLOAT_VEC3;
				sizeUni = 3 * sizeof(float);
				break;
			case GL_FLOAT_VEC4: uniType = UniformDataType::FLOAT_VEC4;
				sizeUni = 4 * sizeof(float);
				break;
			case GL_INT:	    uniType = UniformDataType::INT;
				sizeUni = sizeof(int);
				break;
			case GL_INT_VEC2:	uniType = UniformDataType::INT_VEC2;
				sizeUni = 2 * sizeof(int);
				break;
			case GL_INT_VEC3:	uniType = UniformDataType::INT_VEC3;
				sizeUni = 3 * sizeof(int);
				break;
			case GL_INT_VEC4:	uniType = UniformDataType::INT_VEC4;
				sizeUni = 4 * sizeof(int);
				break;
			case GL_FLOAT_MAT4:	uniType = UniformDataType::FLOAT_MAT4;
				sizeUni = sizeof(glm::mat4);
				break;
			case GL_SAMPLER_2D: uniType = UniformDataType::SAMPLER2D;
				sizeUni = 0;
				break;
			case GL_SAMPLER_CUBE: uniType = UniformDataType::SAMPLERCUBE;
				sizeUni = 0;
				break;
			case GL_BOOL:	uniType = UniformDataType::BOOL;
				sizeUni = sizeof(bool);
				break;
			}

			if (size > 1)
				sizeUni *= size;

			
			addUniform(uniType, sizeUni, offset, name, size);
			offset += sizeUni;
			m_UniformsSize += sizeUni;
		}

	}

	void OpenGLShader::parseSubRoutines()
	{
		int countActiveSU;
		char name[256];
		int len;
		int numCompS;

		unsigned int types[4] = { GL_VERTEX_SHADER,GL_FRAGMENT_SHADER,GL_GEOMETRY_SHADER,GL_COMPUTE_SHADER };
		for (auto type : types)
		{
			glGetProgramStageiv(m_RendererID, type, GL_ACTIVE_SUBROUTINE_UNIFORMS, &countActiveSU);
			for (int i = 0; i < countActiveSU; ++i)
			{
				glGetActiveSubroutineUniformiv(m_RendererID, type, i, GL_NUM_COMPATIBLE_SUBROUTINES, &numCompS);
				Routine routine;
				for (int j = 0; j < numCompS; ++j)
				{
					glGetActiveSubroutineName(m_RendererID, type, j, 256, &len, name);
					GLuint id = glGetSubroutineIndex(m_RendererID, type, name);
					routine.subRoutines.push_back(SubRoutine{ type,id,std::string(name) });
				}
				m_Routines.push_back(routine);
			}
		}
	}

	void OpenGLShader::parsePredefVariables(const std::string& filepath, std::string& source)
	{
		std::string preDefSource = readFile(filepath);

		std::vector<std::string> vertexVariables;
		std::vector<std::string> fragmentVariables;
		std::vector<std::string> computeVariables;
		std::vector<std::string> geometryVariables;


		const char* typeToken = "#type";
		size_t typeTokenLength = strlen(typeToken);
		size_t pos = preDefSource.find(typeToken, 0);

		while (pos != std::string::npos)
		{
			size_t eol = preDefSource.find_first_of("\r\n", pos);
			XYZ_ASSERT(eol != std::string::npos, "Syntax error");
			size_t begin = pos + typeTokenLength + 1;
			std::string type = preDefSource.substr(begin, eol - begin);
			XYZ_ASSERT(ShaderTypeFromString(type), "Invalid shader type specified");

			size_t nextLinePos = preDefSource.find_first_not_of("\r\n", eol);
			pos = preDefSource.find(typeToken, nextLinePos);
			std::string var = preDefSource.substr(nextLinePos, pos - (nextLinePos == std::string::npos ? preDefSource.size() - 1 : nextLinePos));

			if (type == "vertex")
				vertexVariables.push_back(var);
			if (type == "fragment" || type == "pixel")
				fragmentVariables.push_back(var);
			if (type == "geometry")
				geometryVariables.push_back(var);
			if (type == "compute")
				computeVariables.push_back(var);
		}


		const char* versionToken = "#version";
		size_t versionTokenLength = strlen(versionToken);

		size_t verPos = source.find(versionToken, 0);
		pos = source.find(typeToken, 0);
		while (pos != std::string::npos && verPos != std::string::npos)
		{
			size_t eol = source.find_first_of("\r\n", pos);
			XYZ_ASSERT(eol != std::string::npos, "Syntax error");
			size_t begin = pos + typeTokenLength + 1;
			std::string type = source.substr(begin, eol - begin);
			XYZ_ASSERT(ShaderTypeFromString(type), "Invalid shader type specified");

			size_t eolVer = source.find_first_of("\r\n", verPos);
			XYZ_ASSERT(eolVer != std::string::npos, "Syntax error");
			size_t beginVer = verPos + versionTokenLength + 1;
			std::string version = source.substr(beginVer, eolVer - beginVer);

			if (type == "vertex")
			{
				for (auto var : vertexVariables)
				{
					source.insert(eolVer, "\n" + var + "\n");
					eolVer += var.size() + 2;
				}
			}
			else if (type == "fragment")
			{
				for (auto var : fragmentVariables)
				{
					source.insert(eolVer, "\n" + var + "\n");
					eolVer += var.size() + 2;
				}
			}
			else if (type == "geometry")
			{
				for (auto var : geometryVariables)
				{
					source.insert(eolVer, "\n" + var + "\n");
					eolVer += var.size() + 2;
				}
			}
			else if (type == "compute")
			{
				for (auto var : computeVariables)
				{
					source.insert(eolVer, "\n" + var + "\n");
					eolVer += var.size() + 2;
				}
			}
			size_t nextLinePos = source.find_first_not_of("\r\n", eol);
			pos = source.find(typeToken, nextLinePos);

			size_t nextLineVer = source.find_first_not_of("\r\n", eolVer);
			verPos = source.find(versionToken, nextLineVer);
		}
	}

}