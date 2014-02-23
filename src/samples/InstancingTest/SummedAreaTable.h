#ifndef OPLO_SUMMED_AREA_TABLE_H
#define OPLO_SUMMED_AREA_TABLE_H

#include <memory>

namespace oplo
{

class Program;
class TextureDescriptor;

class RectangleBoundaries
{
public:
	RectangleBoundaries();

	void initialize(int w, int h);

	void resize(unsigned w, unsigned h);

	void RectangleBoundaries::generate(
		float cocScale, float cocBias,
		const TextureDescriptor& depthTexture,
		const TextureDescriptor& sourceTexture);

	TextureDescriptor& get();

private:

	unsigned m_shaderBuffer;

	std::shared_ptr<Program> m_boundaries;
	std::shared_ptr<Program> m_boundariesHSAT;
	std::shared_ptr<Program> m_boundariesVSAT;
	std::shared_ptr<TextureDescriptor> m_firstOrder;
};

class SummedAreaTable
{
public:

	SummedAreaTable();

	void initialize(int w, int h);

	void resize(unsigned w, unsigned h);

	void generateFor(const TextureDescriptor& sourceTexture);

	TextureDescriptor& get();

private:

	std::shared_ptr<Program> m_satV;
	std::shared_ptr<Program> m_satH;
	std::shared_ptr<TextureDescriptor> m_firstOrder;
};

}

#endif
