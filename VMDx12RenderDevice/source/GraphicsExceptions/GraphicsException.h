#pragma once

#include "winHandler.h"
#include <exception>
#include <string>


namespace GVM {


class GraphicsException : public std::exception {
public:
	GraphicsException(int line, const char* file) noexcept;
	const char* what() const noexcept override;
	virtual const char* GetType() const noexcept;
	int GetLine() const noexcept;
	const std::string& GetFile() const noexcept;
	std::string GetOriginString() const noexcept;
private:
	int line;
	std::string file;
protected:
	mutable std::string whatBuffer;
};

class Exception : public GraphicsException {
	using GraphicsException::GraphicsException;
};
class HrException : public Exception {
public:
	HrException(int line, const char* file, HRESULT hr, std::vector<std::string> infoMsgs = {}) noexcept;
	const char* what() const noexcept override;
	const char* GetType() const noexcept override;
	HRESULT GetErrorCode() const noexcept;
	std::string GetErrorString() const noexcept;
	std::string GetErrorDescription() const noexcept;
	std::string GetErrorInfo() const noexcept;
private:
	HRESULT hr;
	std::string info;
};

class InfoException : public Exception {
public:
	InfoException(int line, const char* file, std::vector<std::string> infoMsgs) noexcept;
	const char* what() const noexcept override;
	const char* GetType() const noexcept override;
	std::string GetErrorInfo() const noexcept;
private:
	std::string info;
};

class CompileException : public HrException {
	std::string compileError;
	std::string shaderType;
	std::string defines;
	std::string shaderName;
public:
	CompileException(int line, const char* file, HRESULT hr, std::vector<std::string> infoMsgs, const char* compileError, const char* shaderName) noexcept;
	CompileException(int line, const char* file, HRESULT hr,
		std::vector<std::string> infoMsgs, const char* compileError, const char* shaderName,
		std::string typeShader,
		const std::vector<_D3D_SHADER_MACRO>& d3ddefines);
	const char* what() const noexcept override;
	//const char* GetType() const noexcept override;
	//std::string GetErrorInfo() const noexcept;
	~CompileException();
private:
	std::string info;
};

class DeviceRemovedException : public HrException {
	using HrException::HrException;
public:
	const char* GetType() const noexcept override;
private:
	std::string reason;
};

}
