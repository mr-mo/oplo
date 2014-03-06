#ifndef OPLO_OBJ_READER_H
#define OPLO_OBJ_READER_H

#include <fstream>

#include <Containers/Utility/TokenizeString.h>
#include <unordered_map>

namespace oplo
{
	class ObjReader
	{
	public:

		struct DefaultProcessor
		{
			DefaultProcessor() : m_hash(60101, 
				[](const std::tuple<int, int, int>& in)
				{
					std::hash<int> tmp;
					std::size_t h0 = tmp(std::get<0>(in));
					std::size_t h1 = hashCombine( tmp(std::get<1>(in)), h0);
					std::size_t h2 = hashCombine( tmp(std::get<2>(in)), h1 );
					return h2;
				}
			),
			m_interleave(-1)
			{}
			
			void addVertex(oplo::Vec3F const& v)
			{
				m_vertices.push_back(v);
			}

			void addTex(oplo::Vec2F const& t)
			{
				m_tex.push_back(t);
			}

			void addNormal(oplo::Vec3F const& n)
			{
				m_normals.push_back(n);
			}

			void startFace()
			{
				m_debug = 0;
			}

			void addIndex( int v, int t, int n )
			{
				int interleave = 3 + (t >= 0 ? 2 : 0) + (n >= 0 ? 3 : 0);
				
				if (m_interleave < 0)
				{
					m_interleave = interleave;
				}
	
				assert(interleave == m_interleave);

				ReorderHash::iterator itr = m_hash.find(std::make_tuple(v, t, n));

				if (itr == m_hash.end())
				{
					std::size_t idx = m_remappedBuffer.size() / m_interleave;
					m_indices.push_back(static_cast<unsigned>(idx));

					m_remappedBuffer.push_back(m_vertices[v][0]);
					m_remappedBuffer.push_back(m_vertices[v][1]);
					m_remappedBuffer.push_back(m_vertices[v][2]);

					if (t >= 0)
					{
						m_remappedBuffer.push_back(m_vertices[t][0]);
						m_remappedBuffer.push_back(m_vertices[t][1]);
					}

					if (n >= 0)
					{
						m_remappedBuffer.push_back(m_normals[n][0]);
						m_remappedBuffer.push_back(m_normals[n][1]);
						m_remappedBuffer.push_back(m_normals[n][2]);
					}

					m_hash.insert(std::make_pair(std::make_tuple(v, t, n), idx));
				}
				else
				{
					m_indices.push_back(itr->second);
				}

				++m_debug;
			}

			void endFace()
			{
				assert(m_debug == 3);
			}

			std::size_t numVertices() const
			{
				return m_vertices.size();
			}

			std::size_t numNormals() const
			{
				return m_normals.size();
			}

			std::size_t numTexCoords() const
			{
				return m_tex.size();
			}

			int m_interleave;
			int m_debug;
			std::vector<oplo::Vec3F> m_vertices;
			std::vector<oplo::Vec3F> m_normals;
			std::vector<oplo::Vec2F> m_tex;

			typedef std::unordered_map< 
						std::tuple<int, int, int>, 
						std::size_t, 
						std::function<std::size_t(const std::tuple<int,int,int>&)> > ReorderHash;
			ReorderHash m_hash;

			std::vector<unsigned int> m_indices;
			std::vector<float> m_remappedBuffer;
		};

		enum
		{
			UNKNOWN = -1,
			VERTEX = 0,
			NORMAL = 1,
			TEXCOORD = 2,
			FACE = 3
		};

		ObjReader()
		{}

		template<typename Functor>
		static void Process(const char* fn, Functor& func)
		{
			oplo::Delimiters delim(" /", 2);

			std::ifstream s(fn);

			if (s.is_open())
			{
				std::string in;
	
				while (!s.eof())
				{
					s >> in;

					if (in == "#")
					{
						continue;
					}

					if (in == "v")
					{
						float x; s >> x;
						float y; s >> y;
						float z; s >> z;
						func.addVertex(oplo::Vec3F(x, y, z));
					}

					if (in == "vn")
					{
						float x; s >> x;
						float y; s >> y;
						float z; s >> z;
						func.addNormal(oplo::Vec3F(x, y, z));
					}

					if (in == "vt")
					{
						float x; s >> x;
						float y; s >> y;
						func.addTex(oplo::Vec2F(x, y));
					}

					if (in == "f")
					{
						std::getline(s, in);
						func.startFace();

						std::string idx[3];
						int iCur = 0;
						for (int i = 1; i < in.size(); ++i)
						{
							if (in[i] == '/')
							{
								++iCur;
							}
							else if (in[i] == ' ')
							{
								addIndex(idx, func);
								iCur = 0;
							}
							else
							{
								idx[iCur].push_back(in[i]);
							}
						}

						addIndex(idx, func);
						func.endFace();
					}
				}
			}
		}

		template<typename F>
		static void addIndex(std::string* strIn, F& f)
		{
			int numericalIndex[3] = { -1, -1, -1 };

			bool hasSomething = false;

			for (int i = 0; i < 3; ++i)
			{
				if (!strIn[i].empty())
				{
					sscanf_s(strIn[i].c_str(), "%i", &numericalIndex[i]);

					if (numericalIndex[i] > 0)
					{
						numericalIndex[i] -= 1; //make it 0 based
					}
					else
					{
						switch (i)
						{
						case 0:
							numericalIndex[i] = f.numVertices() + numericalIndex[i];
							break;
						case 1:
							numericalIndex[i] = f.numTexCoords() + numericalIndex[i];
							break;
						case 2:
							numericalIndex[i] = f.numNormals() + numericalIndex[i];
							break;
						}
					}

					hasSomething = true;
				}

				strIn[i].clear();
			}

			if (hasSomething)
			{
				f.addIndex(numericalIndex[0], numericalIndex[1], numericalIndex[2]);
			}
		}

	private:
	};
}

#endif
