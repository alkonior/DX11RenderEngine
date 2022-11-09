#pragma once
#include <string>
#include <memory>
#include <vector>
#include "TransformUtils.h"
#include "SimpleMath.h"
#include "TextureData.h"



struct Float3ImageUpdate {
	size_t id;
	ImageBox box;
	int32_t width; int32_t height;
	int32_t level;
	float3* data;
};


class Float3Data {
public:
	using Color = float3;
	
	Float3Data(unsigned int width, unsigned int height) noexcept;
	Float3Data(Float3Data&& source) noexcept;
	Float3Data(Float3Data&) = delete;
	Float3Data& operator=(Float3Data&& donor) noexcept;
	Float3Data& operator=(const Float3Data&) = delete;
	~Float3Data();
	void Clear(Color fillValue) noexcept;
	void PutPixel(unsigned int x, unsigned int y, Color c) noexcept;
	Color GetPixel(unsigned int x, unsigned int y) const noexcept;
	unsigned int GetWidth() const noexcept;
	unsigned int GetHeight() const noexcept;
	Color* GetBufferPtr() noexcept;
	const Color* GetBufferPtr() const noexcept;
	const Color* GetBufferPtrConst() const noexcept;
	Color& operator()(uint32_t x, uint32_t y) noexcept;
	void Copy(const Float3Data& src) noexcept;
private:
	Float3Data(unsigned int width, unsigned int height, std::unique_ptr<float3[]> pBufferParam) noexcept;
private:
	std::unique_ptr<float3[]> pBuffer;
	unsigned int width;
	unsigned int height;
};