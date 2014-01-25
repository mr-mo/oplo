#ifndef HASHMAP_H
#define HASHMAP_H

#include <functional>

template<typename T>
struct QuadraticHashParameters
{
	std::hash<T> hasher;

	const std::size_t hash(T const& key) const
	{
		return hasher(key);
	}

	const std::size_t probe(std::size_t idx, std::size_t iter) const
	{
		return idx + iter * iter;
	}

	bool equals(T const& lhs, T const& rhs) const
	{
		return lhs == rhs;
	}

	bool isValidKey(T const& v) const
	{
		return !equals(v, T(0));
	}

	bool shouldResize(std::size_t residency, std::size_t tableSize) const
	{
		//bin size
		std::size_t delta = tableSize - (tableSize >> 1);

		//quarter bin size
		delta = delta >> 2;

		//have less then a quarter of slots available
		return tableSize - residency < delta;
	}
};


template<
	typename KeyType,
	typename ValueType, 
	typename HashParameters = QuadraticHashParameters<KeyType>
	>
struct DenseHashMap
{
	typedef std::pair<KeyType, ValueType> PairType;
	typedef const KeyType& KeyArgument;
	typedef const ValueType& ValueArgument;
	typedef PairType* Iterator;

	DenseHashMap( std::size_t initialAllocation = 32 ) :
		m_data(0), 
		m_residency(0),
		m_tableSize(initialAllocation)
	{
		if (initialAllocation > 0)
		{
			m_data = new PairType[m_tableSize];
			memset(m_data, 0, sizeof(PairType) * m_tableSize);
		}
	}

	void compress()
	{	
		assert(m_dataMulticand > 1);
		assert(m_data);
		rehash(m_tableSize >> 1);
	}

	const Iterator find(KeyArgument key) const
	{
		const std::size_t ander = (m_tableSize - 1);
		const std::size_t startIdx = m_params.hash(key) & ander;

		for (std::size_t i = 0; i < m_tableSize; ++i)
		{
			const std::size_t idx = m_params.probe(startIdx, i);

			if (m_params.equals(key, m_data[idx % ander].first))
			{
				return &m_data[idx % ander];
			}

			if (!m_params.isValidKey(m_data[idx % ander].first))
			{
				break;
			}
		}

		return end();
	}

	const Iterator end() const
	{
		return m_data + m_tableSize;
	}

	const ValueArgument value( const Iterator itr ) const
	{
		return itr->second;
	}

	bool insert(KeyArgument key, ValueArgument val)
	{
		if (m_params.shouldResize(m_residency + 1, m_tableSize))
		{
			m_residency = 0;
			rehash(m_tableSize << 1);
		}

		const std::size_t ander = (m_tableSize - 1);
		const std::size_t startIdx = m_params.hash(key) & ander;

		for (std::size_t i = 0; i < m_tableSize; ++i)
		{
			const std::size_t idx = m_params.probe(startIdx, i);

			if (!m_params.isValidKey(m_data[idx % ander].first))
			{
				m_data[idx %ander].first = key;
				m_data[idx %ander].second = val;

				++m_residency;

				return true;
			}

			if (m_params.equals(key, m_data[idx % ander].first))
			{
				return false;
			}
		}

	}

private:

	void rehash( std::size_t newTableSize )
	{
		PairType* tmp = m_data;

		std::size_t oldTableSize = m_tableSize;

		m_tableSize = newTableSize;

		m_data = new PairType[newTableSize];

		memset(m_data, 0, sizeof(PairType)* m_tableSize);

		for (int i = 0; i < oldTableSize; ++i)
		{
			if (m_params.isValidKey(tmp[i].first))
			{
				insert(tmp[i].first, tmp[i].second);
			}
		}

		delete[] tmp;
	}

	std::size_t m_residency;

	std::size_t m_tableSize;

	HashParameters m_params;

	PairType* m_data;
};


#endif
