#ifndef LINEAR_OCTREE_CONSTRUCTOR_H
#define LINEAR_OCTREE_CONSTRUCTOR_H

#include "Containers/Utility/MortonCode.h"
#include "Math\Vector.h"

#include <queue>
#include <unordered_set>

namespace oplo
{
	template<typename DataType>
	class MinimalOctree
	{
	public:

		struct Node
		{
			Node() : m_parent(0)
			{
				m_children[0] = 0;
				m_children[1] = 0;
				m_children[2] = 0;
				m_children[3] = 0;
				m_children[4] = 0;
				m_children[5] = 0;
				m_children[6] = 0;
				m_children[7] = 0;
				m_code.setDepth(0);
			}

			void clean()
			{
				m_parent = 0;

				for (int i = 0; i < 8; ++i)
				{
					m_children[i] = 0;
				}
			}

			MortonCode m_code;

			Node* m_parent;
			Node* m_children[8];

			DataType m_data;
		};

		struct PrintBFS
		{
			bool operator()(Node* n)
			{
				for (unsigned i = 0; i < n->m_code.getDepth(); ++i)
					printf("  ");

				n->m_code.print();
				
				return true;
			}
		};

		MinimalOctree() : m_nodeCount(1)
		{}

		~MinimalOctree()
		{
			destroy();
		}

		void destroy()
		{
			m_root.clean();

			for (int i = 0; i < m_memory.size(); ++i)
			{
				delete m_memory[i];
			}

			m_memory.clear();
		}

		unsigned getNodeCount() const
		{
			return m_nodeCount;
		}

		Node* getRoot()
		{
			return &m_root;
		}

		Node* createNewNode()
		{
			++m_nodeCount;

			Node* n = new Node;
			m_memory.push_back(n);
			return n;
		}

		//MortonNode* find(const mortonCode& c)
		template<typename SearchFunctor>
		void BFS(SearchFunctor& f)
		{
			std::queue<Node*> nodeQueue;

			Node* rootPtr = &m_root;
			nodeQueue.push(rootPtr);
			f(rootPtr);

			while (!nodeQueue.empty())
			{
				Node* current = nodeQueue.front(); 
				nodeQueue.pop();

				for (int i = 0; i < 8; ++i)
				{
					if (current->m_children[i] && f(current->m_children[i]))
					{
						nodeQueue.push(current->m_children[i]);
					}
				}
			}
		}

		template<typename SearchFunctor>
		void DFS(SearchFunctor& f)
		{
			std::stack<Node*> nodeQueue;

			nodeQueue.push(&m_root);
			f(&m_root);

			while (!nodeQueue.empty())
			{
				Node* current = nodeQueue.top(); nodeQueue.pop();

				for (int i = 0; i < 8; ++i)
				{
					if (current->m_children[i] && f(current->m_children[i]))
					{
						nodeQueue.push(current->m_children[i]);
					}
				}
			}
		}

	private:

		unsigned m_nodeCount;

		std::vector<Node*> m_memory;

		Node m_root;
	};

}

#endif
