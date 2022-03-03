#pragma once


struct ShaderHolder {
	ShaderHolder(void* shaderData, size_t dataLength) :
		shaderData(shaderData), dataLength(dataLength) {};

	void* shaderData;
	size_t dataLength;
};