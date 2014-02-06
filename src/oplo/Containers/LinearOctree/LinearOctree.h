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

			__inline int8_t getTraverseOrder() const
			{
				return (int8_t)((traverseOrder >> (stackState * 3)) & 0x00000007);
			}


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
		void stackTraversal(Traverser& traverse)
		{
			if (!m_renderStructure)
				return;

			LinearOctreeTypes::ExplicitStack stack[MortonCode::MAX_LEVELS + 1];

			int8_t stackPtr = 0;
			int8_t currentDepth, nextDepth;

			MortonCode code(0);

			//outer radius of node
			double outerRadius = m_ext.getLength() * 0.5;
			Vec3D ext = m_ext * 0.5;
			Vec3D center = m_center;

			double outerRadiusNext;
			Vec3D extNext;
			Vec3D centerNext;

			if (traverse.traverse(center, ext, outerRadius))
			{
				MortonCode nextCode = code;

				stack[0].nodePtr = m_renderStructure;
				stack[0].stackState = 0;
				stack[0].stateOffset = 0;

				traverse.getTraverseOrder(stack[0].traverseOrder, center);

				while (stackPtr >= 0)
				{
					currentDepth = stackPtr;
					nextDepth = stackPtr + 1;

					nextCode = code;
					nextCode.setDepth(nextDepth);

					bool didSomething = false;
					LinearOctreeTypes::ExplicitStack& currentStack = stack[stackPtr];

					for (; currentStack.stackState < 8; ++currentStack.stackState)
					{
						int8_t currentOrder = currentStack.getTraverseOrder();

						if (currentStack.nodePtr->m_children & (1 << currentOrder))
						{
							nextCode.setCode(nextDepth, currentOrder);

							outerRadiusNext = outerRadius * 0.5;
							extNext = ext * 0.5;

							centerNext = center + extNext * MortonCode::getScalar(currentOrder);

							++currentStack.stateOffset;

							if (traverse.traverse(centerNext, extNext, outerRadiusNext))
							{
								++stackPtr;

								code = nextCode;
								center = centerNext;
								outerRadius = outerRadiusNext;
								ext = extNext;

								LinearOctreeTypes::ExplicitStack& nextStack = stack[stackPtr];

								nextStack.stackState = 0;
								nextStack.stateOffset = 0;

								unsigned popCount = getPopCount(currentOrder, currentStack.nodePtr->m_children) - 1;

								nextStack.nodePtr = (currentStack.nodePtr + currentStack.nodePtr->m_nodePointer) + getPopCount(currentOrder, currentStack.nodePtr->m_children) - 1;

								if (nextStack.nodePtr->validData())
								{
									traverse.dataBlock(m_dataStructure + nextStack.nodePtr->m_dataOffset, center, ext, outerRadius);
								}

								traverse.getTraverseOrder(nextStack.traverseOrder, center);

								++currentStack.stackState;

								didSomething = true;
								break;
							}
						}
					}

					if (!didSomething)
					{
						assert(currentDepth == stackPtr);
						decreaseStack(code, center, ext, outerRadius, currentDepth, stackPtr);
					}
				}
			}
		}

		template<typename Traverser>
		void recursiveTraversal(Traverser& traverse)
		{
			if (!m_renderStructure)
				return;

			double outerRadius = m_ext.getLength() * 0.5;

			Vec3D ext = m_ext * 0.5;

			Vec3D center = m_center;

			recursiveInner<Traverser>(traverse, center, ext, outerRadius, m_renderStructure);
		}

	private:

		template<typename T>
		void recursiveInner(T& f, const Vec3D& c, const Vec3D& e, double r, OctNode32* n)
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
				int nextChild = ((traverseOrder >> (i * 3)) & 0x00000007);
				
				if (n->m_children & (1 << nextChild))
				{
					cn = c + en * MortonCode::getScalar(nextChild);

					if (f.traverse(cn, en, rn))
					{
						unsigned popCount = getPopCount(nextChild, n->m_children) - 1;

						recursiveInner<T>(f, cn, en, rn, (n + n->m_nodePointer) + popCount);
					}
				}
			}
		}

		void decreaseStack(MortonCode& code, Vec3D& c, Vec3D& ext, double& outerRadius, unsigned currentDepth, int8_t& stackPtr)
		{
			unsigned bitCode = code.getCode(currentDepth);

			c = c - MortonCode::getScalar(bitCode) * ext;
			ext *= 2;
			outerRadius *= 2;

			code.setCode(currentDepth, 0);
			code.setDepth(currentDepth - 1);

			stackPtr -= 1;
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
