#ifndef OPLO_UNIFORM_H
#define OPLO_UNIFORM_H

#include <string>
#include <vector>
#include "Rendering/oploGL.h"

namespace oplo
{

	class UniformValue
	{
	public:
		UniformValue();

		UniformValue(const std::string& name);

		UniformValue(const std::string& name, int loc, GLint type);

		UniformValue(const UniformValue& rhs);

		void forceNeedsUpdate();

		bool update(const char* val, unsigned sz);

		void apply() const;

		void setType(GLint type);

		void setLocation(int loc);

		void setName(const char* name);

		bool isValid() const;

		const std::string& getName() const;

		int getLocation() const;

	private:

		std::string m_name;

		int		m_location;

		std::vector< char > m_value;

		mutable bool	m_shouldUpdate;

		GLint m_type;
	};

}

#endif
