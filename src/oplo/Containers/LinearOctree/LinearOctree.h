#ifndef OPLO_LINEAR_OCTREE_H
#define OPLO_LINEAR_OCTREE_H

#include <memory>
#include <cassert>
#include <cstdint>

#include "Containers/Utility/MortonCode.h"
#include "Math/Vector.h"

namespace oplo
{
	struct OctNode32
	{
		OctNode32() : m_node(0), m_dataOffset(0)
		{}

		bool validData() const
		{
			return m_dataOffset != -1;
		}

		union
		{
			struct
			{
				uint32_t m_children : 8;
				uint32_t m_nodePointer : 24;
				uint32_t m_dataPointer : 32;
			};

			struct
			{
				uint32_t m_node;
				uint32_t m_dataOffset;
			};
		};
	};

	struct LinearOctreeTypes
	{
		struct ExplicitStack
		{
			ExplicitStack() :
				nodePtr(0),
				stackState(0),
				stateOffset(0),
				//111110101100011010001000
				traverseOrder(16434824)
			{}



			int8_t					stackState;
			int8_t					stateOffset;
			uint32_t				traverseOrder;
			OctNode32*  nodePtr;
		};
	};

	template<typename DataType>
	class LinearOctree32
	{
	public:

		LinearOctree32() :
			m_renderStructure(0),
			m_total(0),
			m_dataStructure(0),
			m_totalData(0)
		{}

		~LinearOctree32()
		{
			if (m_renderStructure)
				delete[] m_renderStructure;

			m_renderStructure = 0;
		}

		void allocate(uint32_t num)
		{
			m_renderStructure = new OctNode32[num];
			m_total = num;
		}

		void setExtents(Vec3D const& min, Vec3D const& max)
		{
			m_min = min;
			m_max = max;
			m_ext = (m_max - m_min);
			m_center = m_min + m_ext * 0.5;

			m_maximumAxis = 0;

			for (int i = 0; i < 3; ++i)
			{
				const double axis = m_max[i] - m_min[i];

				if (axis > m_maximumAxis)
				{
					m_maximumAxis = axis;
				}
			}
		}

		const Vec3D& getMin() const
		{
			return m_min;
		}

		const Vec3D& getMax() const
		{
			return m_max;
		}

		const Vec3D& getCenter() const
		{
			return m_center;
		}

		double getMaximumAxis() const
		{
			return m_maximumAxis;
		}

		OctNode32* root()
		{
			return m_renderStructure;
		}
		
		void setDataPointer(DataType* data)
		{
			m_dataStructure = data;
		}

		template<typename Traverser>
		void traverse(Traverser& traverse)
		{
			if (!m_renderStructure)
				return;

			double outerRadius = m_ext.getLength() * 0.5;

			traverseInner<Traverser>(traverse, m_center, m_ext * 0.5, outerRadius, m_renderStructure);
		}

	private:

		template<typename T>
		void traverseInner(T& f, const Vec3D& c, const Vec3D& e, double r, OctNode32* n)
		{
			if (n->validData())
			{
				f.dataBlock(m_dataStructure + n->m_dataOffset, c, e, r);
			}

			if (n->m_children == 0)
			{
				return;
			}

			uint32_t traverseOrder = 0;

			f.getTraverseOrder(traverseOrder, c);

			double rn = r * 0.5;
			Vec3D en = e * 0.5;
			Vec3D cn;

			for (int i = 0; i < 8; ++i)
			{
				int nextChild = getTraverseOrder(traverseOrder, i);
				
				if (n->m_children & (1 << nextChild))
				{
					cn = c + en * MortonCode::getScalar(nextChild);

					if (f.traverse(cn, en, rn))
					{
						unsigned popCount = getPopCount(nextChild, n->m_children) - 1;

						traverseInner<T>(f, cn, en, rn, (n + n->m_nodePointer) + popCount);
					}
				}
			}
		}

		int getTraverseOrder(int traverseOrder, int iter) const
		{
			return (int)((traverseOrder >> (iter * 3)) & 0x00000007);
		}

		int8_t getPopCount(int8_t numBitsOff, int8_t number)
		{
			unsigned mask = 0x000000ff >> (7 - numBitsOff);
			unsigned v = number & mask;
			v = v - ((v >> 1) & 0x55555555);                    // reuse input as temporary
			v = (v & 0x33333333) + ((v >> 2) & 0x33333333);     // temp		
			return ((v + (v >> 4) & 0xF0F0F0F) * 0x1010101) >> 24; // count
		}

	private:

		OctNode32* m_renderStructure;
		DataType* m_dataStructure;

		Vec3D m_min;
		Vec3D m_max;
		Vec3D m_center;
		Vec3D m_ext;
		double m_maximumAxis;

		uint32_t m_total;
		uint32_t m_totalData;
	};

}

#endif
