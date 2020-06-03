#ifndef ASTARALGORITHM_H
#define ASTARALGORITHM_H

#include <vector>


class AStarAlgorithm
{
public:
	using Matrix = std::vector<std::vector<bool> >;
	using Point = std::pair<size_t, size_t>;
	enum TypeMetric
	{
		MANHATTAN,
		EUCLIDEAN,
	};

public:
	AStarAlgorithm();

public:
	void setTypeMetric(const TypeMetric& type);

public:
	std::vector<Point> searchPath(const Matrix& grid, const Point& start, const Point& finish) const;

private:
	TypeMetric m_typeMetric = TypeMetric::MANHATTAN;
};

#endif // ASTARALGORITHM_H
