#include "pch.h"
#include "GraphicsException.h"
#include <sstream>


GraphicsException::GraphicsException(int line, const char* file) noexcept
	:
	line(line),
	file(file) {}

const char* GraphicsException::what() const noexcept {
	std::ostringstream oss;
	oss << GetType() << std::endl
		<< GetOriginString();
	whatBuffer = oss.str();
	return whatBuffer.c_str();
}

const char* GraphicsException::GetType() const noexcept {
	return "Graphics Exception";
}

int GraphicsException::GetLine() const noexcept {
	return line;
}

const std::string& GraphicsException::GetFile() const noexcept {
	return file;
}

std::string GraphicsException::GetOriginString() const noexcept {
	std::ostringstream oss;
	oss << "[File] " << file << std::endl
		<< "[Line] " << line;
	return oss.str();
}




// Graphics exception stuff
HrException::HrException(int line, const char* file, HRESULT hr, std::vector<std::string> infoMsgs) noexcept
	:
	Exception(line, file),
	hr(hr) {
	// join all info messages with newlines into single string
	for (const auto& m : infoMsgs) {
		info += m;
		info.push_back('\n');
	}
	// remove final newline if exists
	if (!info.empty()) {
		info.pop_back();
	}
}

const char* HrException::what() const noexcept {
	std::ostringstream oss;
	oss << GetType() << std::endl
		<< "[Error Code] 0x" << std::hex << std::uppercase << GetErrorCode()
		<< std::dec << " (" << (unsigned long)GetErrorCode() << ")" << std::endl
		<< "[Error String] " << GetErrorString() << std::endl
		<< "[Description] " << GetErrorDescription() << std::endl;
	if (!info.empty()) {
		oss << "\n[Error Info]\n" << GetErrorInfo() << std::endl << std::endl;
	}
	oss << GetOriginString();
	whatBuffer = oss.str();
	return whatBuffer.c_str();
}

const char* HrException::GetType() const noexcept {
	return "HrException Exception";
}

HRESULT HrException::GetErrorCode() const noexcept {
	return hr;
}

std::string HrException::GetErrorString() const noexcept {
	return DXGetErrorStringA(hr);
}

std::string HrException::GetErrorDescription() const noexcept {
	char buf[512];
	DXGetErrorDescriptionA(hr, buf, sizeof(buf));
	return buf;
}

std::string HrException::GetErrorInfo() const noexcept {
	return info;
}


const char* DeviceRemovedException::GetType() const noexcept {
	return "Device Removed Exception [Device Removed] (DXGI_ERROR_DEVICE_REMOVED)";
}
InfoException::InfoException(int line, const char* file, std::vector<std::string> infoMsgs) noexcept
	:
	Exception(line, file) {
	// join all info messages with newlines into single string
	for (const auto& m : infoMsgs) {
		info += m;
		info.push_back('\n');
	}
	// remove final newline if exists
	if (!info.empty()) {
		info.pop_back();
	}
}


const char* InfoException::what() const noexcept {
	static int counter = 0;
	counter++;
	std::ostringstream oss;
	oss << GetType() << std::endl
		<< "[" << counter << "]" << std::endl
		<< "\n[Error Info]\n" << GetErrorInfo() << std::endl << std::endl;
	oss << GetOriginString();
	whatBuffer = oss.str();
	return whatBuffer.c_str();
}

const char* InfoException::GetType() const noexcept {
	return "Info Exception";
}

std::string InfoException::GetErrorInfo() const noexcept {
	return info;
}

CompileException::CompileException(int line, const char* file, HRESULT hr, std::vector<std::string> infoMsgs, const char* compileError) noexcept :
	HrException(line, file, hr, infoMsgs) {
	this->compileError = std::string(compileError);
}

const char* CompileException::what() const noexcept {
	std::ostringstream oss;
	oss << GetType() << std::endl
		<< "[Error Code] 0x" << std::hex << std::uppercase << GetErrorCode()
		<< std::dec << " (" << (unsigned long)GetErrorCode() << ")" << std::endl
		<< "[Error String] " << GetErrorString() << std::endl
		<< "[Description] " << GetErrorDescription() << std::endl;
	if (!info.empty()) {
		oss << "\n[Error Info]\n" << GetErrorInfo() << std::endl << std::endl;
	}
	oss << GetOriginString();
	oss << std::endl <<
		"[Compile Error]:" <<
		compileError;
	whatBuffer = oss.str();
	return whatBuffer.c_str();
}

CompileException::~CompileException() {
}
