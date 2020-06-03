#include "AStarAlgorithm.h"
#include <functional>
#include <limits>
#include <memory>
#include <map>
#include <queue>
#include <list>
#include <set>
#include <iostream>



namespace
{
	size_t absSizeT(const size_t a, const size_t b)
	{
		if (a > b)
		{
			return (a - b);
		}
		return (b - a);
	}

	constexpr auto MAX_SIZE_T = std::numeric_limits<size_t>::max();

	struct PointData
	{
		AStarAlgorithm::Point parent = { MAX_SIZE_T, MAX_SIZE_T };
		size_t f = MAX_SIZE_T;
		size_t g = MAX_SIZE_T;
		size_t h = MAX_SIZE_T;
	};

	enum class TypeDirection
	{
		NORTH, // {-1, 0}
		SOUTH, // {1, 0}
		EAST, // {0, 1}
		WEST, // {0, -1}
		NORTH_EAST, // {-1, 1}
		NORTH_WEST, // {-1, -1}
		SOUTH_EAST, // {1, 1}
		SOUTH_WEST, // {1, -1}
	};

} // END namespace


AStarAlgorithm::AStarAlgorithm()
{

}

void AStarAlgorithm::setTypeMetric(const AStarAlgorithm::TypeMetric& type)
{
	m_typeMetric = type;
}

std::vector<AStarAlgorithm::Point> AStarAlgorithm::searchPath(const AStarAlgorithm::Matrix& grid, const Point& start, const Point& finish) const
{
	const auto ROWS = grid.size();
	const auto COLS = (grid.empty()) ? 0 : grid[0].size();

	const auto isValid = [ROWS, COLS] (const Point& point)
	{
		return (point.first < ROWS) && (point.second < COLS);
	};

	const auto isDestination = [finish] (const Point& point)
	{
		return (point == finish);
	};

	const auto isCellBlocked = [grid] (const Point& point) -> bool
	{
		return not grid[point.first][point.second];
	};

	std::function<size_t(const Point&, const Point&)> calculate;
	switch (m_typeMetric)
	{
	case TypeMetric::MANHATTAN:
		calculate = [](const Point& current, const Point& dest) -> size_t
		{
			return absSizeT(current.first, dest.first) + absSizeT(current.second, dest.second);
		};
		break;

	case TypeMetric::EUCLIDEAN:
		calculate = [](const Point& current, const Point& dest) -> size_t
			{
				const auto dRow = absSizeT(current.first, dest.first);
				const auto dCol = absSizeT(current.second, dest.second);
				return (dRow * dRow) + (dCol * dCol);
			};
		break;
	}

	auto processPath = [start](const std::vector<std::vector<PointData>>& pointsMatrix, const Point& dest)
	{
		std::vector<Point> ret;
		auto point = dest;
		while (point != start)
		{
			ret.emplace(ret.begin(), point);
			point = pointsMatrix[point.first][point.second].parent;
		}
		ret.emplace(ret.begin(), point); // add start node
		return ret;
	};

	std::map<TypeDirection, size_t> directionOffsets
	{
		{ TypeDirection::NORTH, 1 },	///< NORTH
		{ TypeDirection::SOUTH, 1 },	///< SOUTH
		{ TypeDirection::EAST,  1 },	///< EAST
		{ TypeDirection::WEST,  1 },	///< WEST
	};
	if (m_typeMetric == TypeMetric::EUCLIDEAN)
	{
		directionOffsets[TypeDirection::NORTH_EAST] = 2;	///< NORTH-EAST
		directionOffsets[TypeDirection::NORTH_WEST] = 2;	///< NORTH-WEST
		directionOffsets[TypeDirection::SOUTH_EAST] = 2;	///< SOUTH-EAST
		directionOffsets[TypeDirection::SOUTH_WEST] = 2;	///< SOUTH-WEST
	}

	// check if input cells is invalid, abort if so
	if (not (isValid(start) and isValid(finish)))
	{
		return {};
	}

	// check if input cells is blocked, abort if so
	if (isCellBlocked(start) or isCellBlocked(finish))
	{
		return {};
	}

	// check if input cell is identical, it means we have one-cell path
	if (isDestination(start))
	{
		return { start };
	}

	std::vector<std::vector<PointData>> pointsMatrix;
	{
		pointsMatrix.resize(ROWS);
		for (size_t r = 0; r < ROWS; ++r)
		{
			pointsMatrix[r].resize(COLS);
		}
	}
	std::vector<std::vector<bool>> closedList;
	{
		closedList.resize(ROWS);
		for (size_t r = 0; r < ROWS; ++r)
		{
			closedList[r].resize(COLS);
		}
	}

	using TypeOpenListItem = std::pair<size_t, Point>;
	std::priority_queue<TypeOpenListItem, std::vector<TypeOpenListItem>, std::greater<TypeOpenListItem>> openList;

	// initialization
	auto& pointStart = pointsMatrix[start.first][start.second];
	pointStart.parent = start;
	pointStart.f = 0;
	pointStart.g = 0;
	pointStart.h = 0;

	openList.push({ 0, start });

	const size_t stepMax = ROWS * COLS;
	size_t step = 0;
	while (not openList.empty())
	{
		++step;
		if (stepMax < step)
		{
			break;
		}
		auto current = openList.top();
		openList.pop();
		closedList[current.second.first][current.second.second] = true;

		for (const auto& it : directionOffsets)
		{
			auto pairPos = current.second;
			switch (it.first)
			{
			case TypeDirection::NORTH: // {-1, 0}
				if (pairPos.first == 0)
				{
					continue;
				}
				pairPos.first -= 1;
				break;
			case TypeDirection::SOUTH: // {1, 0}
				pairPos.first += 1;
				break;
			case TypeDirection::EAST: // {0, 1}
				pairPos.second += 1;
				break;
			case TypeDirection::WEST: // {0, -1}
				if (pairPos.second == 0)
				{
					continue;
				}
				pairPos.second -= 1;
				break;
			case TypeDirection::NORTH_EAST: // {-1, 1}
				if (pairPos.first == 0)
				{
					continue;
				}
				pairPos.first -= 1;
				pairPos.second += 1;
				break;
			case TypeDirection::NORTH_WEST: // {-1, -1}
				if (pairPos.first == 0)
				{
					continue;
				}
				if (pairPos.first == 0)
				{
					continue;
				}
				pairPos.first -= 1;
				pairPos.second -= 1;
				break;
			case TypeDirection::SOUTH_EAST: // {1, 1}
				pairPos.first += 1;
				pairPos.second += 1;
				break;
			case TypeDirection::SOUTH_WEST: // {1, -1}
				pairPos.first += 1;
				if (pairPos.second == 0)
				{
					continue;
				}
				pairPos.second -= 1;
				break;
			}
			if (isValid(pairPos))
			{
				const auto row = pairPos.first;
				const auto col = pairPos.second;

				if (isDestination(pairPos))
				{
					pointsMatrix[row][col].parent = current.second;
					return processPath(pointsMatrix, finish);
				}

				if ((not closedList[row][col]) and (not isCellBlocked(pairPos)))
				{
					const size_t l_g = pointsMatrix[current.second.first][current.second.second].g + it.second;
					const size_t l_h = calculate(pairPos, finish);
					const size_t l_f = l_g + l_h;

					if (pointsMatrix[row][col].f > l_f)
					{
						openList.push({ l_f, pairPos });
						auto& pointNext = pointsMatrix[row][col];
						pointNext.g = l_g;
						pointNext.h = l_h;
						pointNext.f = l_f;
						pointNext.parent = current.second;
					}
				}
			}
		}
	}
	return {};
}
