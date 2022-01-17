#pragma once
#include "pch.h"
#include "GraphicsException.h"



class TextureData {
public:
	class Color {
	public:
		uint32_t rgba;
	public:
		constexpr Color() noexcept : rgba() {}
		constexpr Color(const Color& col) noexcept : rgba(col.rgba) {}
		constexpr Color(uint32_t dw) noexcept : rgba(dw) {}
		constexpr Color(unsigned char x, unsigned char r, unsigned char g, unsigned char b) noexcept
		:rgba((x << 24u) | (r << 16u) | (g << 8u) | b) {}
		constexpr Color(unsigned char r, unsigned char g, unsigned char b) noexcept : rgba((r << 16u) | (g << 8u) | b) {}
		constexpr Color(Color col, unsigned char x) noexcept : Color((x << 24u) | col.rgba) {}
		Color& operator =(Color color) noexcept {
			rgba = color.rgba;
			return *this;
		}
		constexpr unsigned char GetX() const noexcept {
			return rgba >> 24u;
		}
		constexpr unsigned char GetA() const noexcept {
			return GetX();
		}
		constexpr unsigned char GetR() const noexcept {
			return (rgba >> 16u) & 0xFFu;
		}
		constexpr unsigned char GetG() const noexcept {
			return (rgba >> 8u) & 0xFFu;
		}
		constexpr unsigned char GetB() const noexcept {
			return rgba & 0xFFu;
		}
		void SetX(unsigned char x) noexcept {
			rgba = (rgba & 0xFFFFFFu) | (x << 24u);
		}
		void SetA(unsigned char a) noexcept {
			SetX(a);
		}
		void SetR(unsigned char r) noexcept {
			rgba = (rgba & 0xFF00FFFFu) | (r << 16u);
		}
		void SetG(unsigned char g) noexcept {
			rgba = (rgba & 0xFFFF00FFu) | (g << 8u);
		}
		void SetB(unsigned char b) noexcept {
			rgba = (rgba & 0xFFFFFF00u) | b;
		}
	};
public:
	class Exception : public GraphicsException {
	public:
		Exception(int line, const char* file, std::string note) noexcept;
		const char* what() const noexcept override;
		const char* GetType() const noexcept override;
		const std::string& GetNote() const noexcept;
	private:
		std::string note;
	};
public:
	TextureData(unsigned int width, unsigned int height) noexcept;
	TextureData(TextureData&& source) noexcept;
	TextureData(TextureData&) = delete;
	TextureData& operator=(TextureData&& donor) noexcept;
	TextureData& operator=(const TextureData&) = delete;
	~TextureData();
	void Clear(Color fillValue) noexcept;
	void PutPixel(unsigned int x, unsigned int y, Color c) noexcept(!_DEBUG);
	Color GetPixel(unsigned int x, unsigned int y) const noexcept(!_DEBUG);
	unsigned int GetWidth() const noexcept;
	unsigned int GetHeight() const noexcept;
	Color* GetBufferPtr() noexcept;
	const Color* GetBufferPtr() const noexcept;
	const Color* GetBufferPtrConst() const noexcept;
	static TextureData FromFile(const std::string& name);
#ifdef WIN32
	static TextureData FromFile(LPCWCH name);
#endif
	void Save(const std::string& filename) const;
	void Copy(const TextureData& src) noexcept(!_DEBUG);
private:
	TextureData(unsigned int width, unsigned int height, std::unique_ptr<Color[ ]> pBufferParam) noexcept;
private:
	std::unique_ptr<Color[ ]> pBuffer;
	unsigned int width;
	unsigned int height;
};