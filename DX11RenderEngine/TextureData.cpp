#include "pch.h"
#define FULL_WINTARD
#include "TextureData.h"
#include "GDIPlusManager.h"
#include <algorithm>
#include <sstream>


GDIPlusManager gdip;


TextureData::TextureData(unsigned int width, unsigned int height) noexcept
	:
	pBuffer(std::make_unique<Color[ ]>(width * height)),
	width(width),
	height(height) {}

TextureData& TextureData::operator=(TextureData&& rhs) noexcept {
	width = rhs.width;
	height = rhs.height;
	pBuffer = std::move(rhs.pBuffer);
	rhs.pBuffer = nullptr;
	return *this;
}

TextureData::TextureData(TextureData&& source) noexcept
	:
	pBuffer(std::move(source.pBuffer)),
	width(source.width),
	height(source.height) {}

TextureData::~TextureData() {}

void TextureData::Clear(Color fillValue) noexcept {
	memset(pBuffer.get(), fillValue.rgba, width * height * sizeof(Color));
}

void TextureData::PutPixel(unsigned int x, unsigned int y, Color c) noexcept(!_DEBUG) {
	assert(x >= 0);
	assert(y >= 0);
	assert(x < width);
	assert(y < height);
	pBuffer[y * width + x] = c;
}

TextureData::Color TextureData::GetPixel(unsigned int x, unsigned int y) const noexcept(!_DEBUG) {
	assert(x >= 0);
	assert(y >= 0);
	assert(x < width);
	assert(y < height);
	return pBuffer[y * width + x];
}

unsigned int TextureData::GetWidth() const noexcept {
	return width;
}

unsigned int TextureData::GetHeight() const noexcept {
	return height;
}

TextureData::Color* TextureData::GetBufferPtr() noexcept {
	return pBuffer.get();
}

const TextureData::Color* TextureData::GetBufferPtr() const noexcept {
	return pBuffer.get();
}

const TextureData::Color* TextureData::GetBufferPtrConst() const noexcept {
	return pBuffer.get();
}

TextureData TextureData::FromFile(const std::string& name) {
	unsigned int width = 0;
	unsigned int height = 0;
	std::unique_ptr<Color[ ]> pBuffer;

	{
		// convert filenam to wide string (for Gdiplus)
		wchar_t wideName[512];
		mbstowcs_s(nullptr, wideName, name.c_str(), _TRUNCATE);
		Gdiplus::Bitmap bitmap(wideName);
		auto status = bitmap.GetLastStatus();
		if (bitmap.GetLastStatus() != Gdiplus::Status::Ok) {
			std::stringstream ss;
			ss << "Loading image [" << name << "]: failed to load.\n";
			ss << "Status: [" << status << "].\n";
			throw Exception(__LINE__, __FILE__, ss.str());
		}

		width = bitmap.GetWidth();
		height = bitmap.GetHeight();
		pBuffer = std::make_unique<Color[ ]>(width * height);

		for (unsigned int y = 0; y < height; y++) {
			for (unsigned int x = 0; x < width; x++) {
				Gdiplus::Color c;
				bitmap.GetPixel(x, y, &c);
				pBuffer[y * width + x] = c.GetValue();
			}
		}
	}

	return TextureData(width, height, std::move(pBuffer));
}

TextureData TextureData::FromFile(LPCWCH name) {
	char output[256];
	const WCHAR* wc = L"Hello World";
	sprintf(output, "%ls", name);
	return TextureData::FromFile(output);
}

void TextureData::Save(const std::string& filename) const {
	auto GetEncoderClsid = [&filename](const WCHAR* format, CLSID* pClsid) -> void
	{
		UINT  num = 0;          // number of image encoders
		UINT  size = 0;         // size of the image encoder array in bytes

		Gdiplus::ImageCodecInfo* pImageCodecInfo = nullptr;

		Gdiplus::GetImageEncodersSize(&num, &size);
		if (size == 0) {
			std::stringstream ss;
			ss << "Saving TextureData to [" << filename << "]: failed to get encoder; size == 0.";
			throw Exception(__LINE__, __FILE__, ss.str());
		}

		pImageCodecInfo = (Gdiplus::ImageCodecInfo*)(malloc(size));
		if (pImageCodecInfo == nullptr) {
			std::stringstream ss;
			ss << "Saving TextureData to [" << filename << "]: failed to get encoder; failed to allocate memory.";
			throw Exception(__LINE__, __FILE__, ss.str());
		}

		GetImageEncoders(num, size, pImageCodecInfo);

		for (UINT j = 0; j < num; ++j) {
			if (wcscmp(pImageCodecInfo[j].MimeType, format) == 0) {
				*pClsid = pImageCodecInfo[j].Clsid;
				free(pImageCodecInfo);
				return;
			}
		}

		free(pImageCodecInfo);
		std::stringstream ss;
		ss << "Saving TextureData to [" << filename <<
			"]: failed to get encoder; failed to find matching encoder.";
		throw Exception(__LINE__, __FILE__, ss.str());
	};

	CLSID bmpID;
	GetEncoderClsid(L"image/bmp", &bmpID);


	// convert filenam to wide string (for Gdiplus)
	wchar_t wideName[512];
	mbstowcs_s(nullptr, wideName, filename.c_str(), _TRUNCATE);

	Gdiplus::Bitmap bitmap(width, height, width * sizeof(Color), PixelFormat32bppARGB, (BYTE*)pBuffer.get());
	if (bitmap.Save(wideName, &bmpID, nullptr) != Gdiplus::Status::Ok) {
		std::stringstream ss;
		ss << "Saving TextureData to [" << filename << "]: failed to save.";
		throw Exception(__LINE__, __FILE__, ss.str());
	}
}

void TextureData::Copy(const TextureData& src) noexcept(!_DEBUG) {
	assert(width == src.width);
	assert(height == src.height);
	memcpy(pBuffer.get(), src.pBuffer.get(), width * height * sizeof(Color));
}

TextureData::TextureData(unsigned int width, unsigned int height, std::unique_ptr<Color[ ]> pBufferParam) noexcept
	:
	width(width),
	height(height),
	pBuffer(std::move(pBufferParam)) {}


// TextureData exception stuff
TextureData::Exception::Exception(int line, const char* file, std::string note) noexcept
	:
	GraphicsException(line, file),
	note(std::move(note)) {}

const char* TextureData::Exception::what() const noexcept {
	std::ostringstream oss;
	oss << GraphicsException::what() << std::endl
		<< "[Note] " << GetNote();
	whatBuffer = oss.str();
	return whatBuffer.c_str();
}

const char* TextureData::Exception::GetType() const noexcept {
	return "Chili Graphics Exception";
}

const std::string& TextureData::Exception::GetNote() const noexcept {
	return note;
}
