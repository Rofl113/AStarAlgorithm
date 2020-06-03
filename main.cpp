#include <iostream>
#include <algorithm>
#include <chrono>
#include "AStarAlgorithm.h"



int main()
{
	AStarAlgorithm::Matrix grid
	{{
			{ 1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1 }
			, { 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1 }
			, { 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1 }
			, { 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1 }
			, { 1, 0, 1, 0, 1, 1, 1, 0, 1, 0, 1 }
			, { 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1 }
			, { 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1 }
			, { 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1 }
			, { 1, 0, 1, 1, 1, 0, 1, 1, 1, 0, 1 }
		}};

	AStarAlgorithm path;
	path.setTypeMetric(AStarAlgorithm::TypeMetric::MANHATTAN);

	const auto start = std::chrono::steady_clock::now();
	auto vec = path.searchPath(grid, {8, 0}, {0, 10});
	const auto finish = std::chrono::steady_clock::now();
	const auto timeMsec = std::chrono::duration_cast<std::chrono::microseconds>(finish - start);
	std::cout << "timeMsec: " << timeMsec.count() << " us" << std::endl;

	for (const auto& cit : vec)
	{
		const auto start = std::distance(vec.cbegin(), std::find(vec.cbegin(), vec.cend(), cit)) == 0 ? "" : " -> ";
		std::cout << start << "[" << cit.first << ", " << cit.second << "]";
	}
	std::cout << std::endl;

	for(size_t i = 0; i < grid.size(); ++i)
	{
		const auto& gridRow = grid[i];
		for(size_t j = 0; j < gridRow.size(); ++j)
		{
			const auto cit = std::find(vec.cbegin(), vec.cend(), AStarAlgorithm::Point{i, j});
			std::cout << "|";
			if (cit != vec.cend())
			{
				if (cit == vec.cbegin())
				{
					std::cout << "\033[1;32m*\033[0m";
				}
				else if (cit == --vec.cend())
				{
					std::cout << "\033[1;34m*\033[0m";
				}
				else
				{
					std::cout << "\033[1;31m*\033[0m";
				}
			}
			else
			{
				std::cout << " ";
			}
		}
		std::cout << "|" << std::endl;
	}

	return 0;
}
