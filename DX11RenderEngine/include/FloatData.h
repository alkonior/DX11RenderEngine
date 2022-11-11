#pragma once
#include <string>
#include <memory>
#include <vector>
#include "TransformUtils.h"
#include "SimpleMath.h"
#include "TextureData.h"



struct FloatImageUpdate {
	size_t id;
	ImageBox box;
	int32_t width; int32_t height;
	int32_t level;
	float* data;
};


class FloatData {
public:
	using Color = float;
	
	FloatData(unsigned int width, unsigned int height) noexcept;
	FloatData(FloatData&& source) noexcept;
	FloatData(FloatData&) = delete;
	FloatData& operator=(FloatData&& donor) noexcept;
	FloatData& operator=(const FloatData&) = delete;
	~FloatData();
	void Clear(Color fillValue) noexcept;
	void PutPixel(unsigned int x, unsigned int y, Color c) noexcept;
	Color GetPixel(unsigned int x, unsigned int y) const noexcept;
	unsigned int GetWidth() const noexcept;
	unsigned int GetHeight() const noexcept;
	Color* GetBufferPtr() noexcept;
	const Color* GetBufferPtr() const noexcept;
	const Color* GetBufferPtrConst() const noexcept;
	Color& operator()(uint32_t x, uint32_t y) noexcept;
	void Copy(const FloatData& src) noexcept;
private:
	FloatData(unsigned int width, unsigned int height, std::unique_ptr<float[]> pBufferParam) noexcept;
private:
	std::unique_ptr<float[]> pBuffer;
	unsigned int width;
	unsigned int height;
};