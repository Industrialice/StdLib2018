#pragma once

#include <StdCoreLib.hpp>
#include <ListenerHandle.hpp>

namespace StdLib
{
	template <typename T, typename E, E T::*field, typename IT> [[nodiscard]] E FindSmallestID(IT start, IT end)
	{
		static_assert(std::is_unsigned_v<E>);

		if (start == end)
		{
			return 0;
		}

		std::vector<E> temp;
		for (; start != end; ++start)
		{
			const auto &value = *start;
			temp.push_back(value.*field);
		}

		std::sort(temp.begin(), temp.end());

		if (temp.front() != 0)
		{
			return 0;
		}

		for (uiw index = 0, size = temp.size() - 1; index != size; ++index)
		{
			E nextId = temp[index] + 1;
			if (nextId != temp[index + 1])
			{
				return nextId;
			}
		}

		return temp.back() + 1;
	}

	template <typename T, typename E, E T::*field, typename IT> [[nodiscard]] NOINLINE E AssignId(E &currentId, IT start, IT end)
	{
		if (currentId != std::numeric_limits<E>::max())
		{
			E id = currentId++;
			return id;
		}
		// this should never happen unless you have bogus code that calls AssignId repeatedly
		// you'd need 50k AssignId calls per second to exhaust ui32 within 24 hours
		SOFTBREAK;
		return FindSmallestID<T, E, field>(start, end);
	}
}