#ifndef OPLO_SUMMED_AREA_TABLE_H
#define OPLO_SUMMED_AREA_TABLE_H

#include <memory>

namespace oplo
{

class Program;
class TextureDescriptor;

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
