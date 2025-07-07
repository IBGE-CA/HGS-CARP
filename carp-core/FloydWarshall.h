#pragma once

#include <vector>
#include <list>
#include <limits>

namespace carp_core
{
	template<class T>
	class FloydWarshall
	{
	public:
		FloydWarshall(std::vector<std::vector<T>>& adj_matrix) : dist(adj_matrix)
		{
			pred.resize(dist.size());
			path.resize(dist.size());
			for (int i = 0; i < (int)dist.size(); i++)
			{
				pred[i].resize(dist.size(), i);
				path[i].resize(dist.size());
			}
		}

		void run()
		{
			for (int k = 0; k < (int)dist.size(); k++)
				for (int i = 0; i < (int)dist.size(); i++)
					for (int j = 0; j < (int)dist.size(); j++)
						if (dist[i][j] > dist[i][k] + dist[k][j])
						{
							dist[i][j] = dist[i][k] + dist[k][j];
							pred[i][j] = k;
						}
		}

		const std::vector<std::vector<T>>& getDist() const { return dist; }

		const std::vector<int>& getPath(int from, int to)
		{
			if (path[from][to].empty())
				if (from != to)
				{
					std::list<int> pre_path;
					if (dist[from][to] != infinity())
					{
						pre_path.push_back(from);
						pre_path.push_back(to);

						auto it = pre_path.begin();
						doPath(pre_path, from, to, ++it);
					}
					path[from][to].assign(pre_path.begin(), pre_path.end());
				}
				else
					path[from][to].push_back(from);

			return path[from][to];
		}

		static T infinity()
		{
			return std::numeric_limits<T>::has_infinity ?
				std::numeric_limits<T>::infinity() :
				std::numeric_limits<T>::max() / 2
			;
		}

	private:
		std::vector<std::vector<T>>& dist;
		std::vector<std::vector<int>> pred;
		std::vector<std::vector<std::vector<int>>> path;

		void doPath(std::list<int>& pre_path, int from, int to, std::list<int>::iterator it)
		{
			if (from == pred[from][to]) return;
			doPath(pre_path, from, pred[from][to], pre_path.insert(it, pred[from][to]));
			doPath(pre_path, pred[from][to], to, it);
		}
	};
}
