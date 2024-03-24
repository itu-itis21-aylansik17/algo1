/**
 * BLG 336E - Analysis of Algorithms II
 * Assignment 1
 * <Mert Aylanşık>
 * <150170723>
 */
// g++ main.cpp -o main -Wall -Werror
// valgrind --tool=memcheck --leak-check=full --show-leak-kinds=all ./main 1 map_name.txt |& tee -a valgrind_log.txt
// calico test.yaml |& tee calico_log.txt
/* * * * * * * * * * * * * * * * * *
 * DO NOT MODIFY UNLESS REQUESTED  *
 * * * * * * * * * * * * * * * * * */
#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>
#include <stack>
#include <queue>
#include <chrono> // For time measurement

/* INCLUDE ANY OTHER NECESSARY LIBRARIES HERE */
#include <algorithm> // Added library

/* START YOUR CODE HERE */

/* END YOUR CODE HERE */

using namespace std;
using namespace std::chrono;

/* USE HERE IF YOU NEED TO DEFINE EXTRA FUNCTIONS */
// Utility function for DFS traversal
int dfs_util(vector<vector<int>> &map, int row, int col, int resource, vector<vector<bool>> &visited)
{
    int m = map.size();
    int n = map[0].size();

    // Check if out of bounds or already visited or not matching resource
    if (row < 0 || col < 0 || row >= m || col >= n || visited[row][col] || map[row][col] != resource)
    {
        return 0;
    }

    // Mark the current cell as visited
    visited[row][col] = true;
    int size = 1; // Each cell of the same type contributes a size of 1

    // Directions: Up, Down, Left, Right
    vector<pair<int, int>> directions = {{-1, 0}, {1, 0}, {0, -1}, {0, 1}};
    for (auto &d : directions)
    {
        int newRow = (row + d.first + m) % m;  // Wrap around for circular map
        int newCol = (col + d.second + n) % n; // Wrap around for circular map

        // Explore the new cell
        size += dfs_util(map, newRow, newCol, resource, visited);
    }

    return size;
}

/**
 * Perform Depth-First Search (DFS) traversal on a map.
 *
 * @param map The map represented as a 2D vector of integers.
 * @param row The starting row index for the DFS traversal.
 * @param col The starting column index for the DFS traversal.
 * @param resource The resource value to search for during the traversal.
 * @return The size of the colony found during the DFS traversal.
 */

// DFS wrapper function
int dfs(vector<vector<int>> &map, int row, int col, int resource)
{
    vector<vector<bool>> visited(map.size(), vector<bool>(map[0].size(), false));
    return dfs_util(map, row, col, resource, visited);
}
/**
 * Perform Breadth-First Search (BFS) traversal on a map.
 *
 * @param map The map represented as a 2D vector of integers.
 * @param row The starting row index for the BFS traversal.
 * @param col The starting column index for the BFS traversal.
 * @param resource The resource value to search for during the traversal.
 * @return The size of the colony found during the BFS traversal.
 */
// Breadth-First Search to find the size of a colony starting from (row, col).
int bfs(vector<vector<int>> &map, int row, int col, int resource)
{
    int m = map.size(), n = map[0].size();
    vector<vector<bool>> visited(m, vector<bool>(n, false));
    queue<pair<int, int>> q;
    q.push({row, col});
    visited[row][col] = true;
    int size = 0;

    while (!q.empty())
    {
        pair<int, int> front = q.front();
        q.pop();
        int r = front.first;
        int c = front.second;
        size++;

        vector<pair<int, int>> directions = {{-1, 0}, {1, 0}, {0, -1}, {0, 1}};
        for (const auto &dir : directions)
        {
            int nextRow = (r + dir.first + m) % m, nextCol = (c + dir.second + n) % n;
            if (!visited[nextRow][nextCol] && map[nextRow][nextCol] == resource)
            {
                q.push({nextRow, nextCol});
                visited[nextRow][nextCol] = true;
            }
        }
    }

    return size;
}

/**
 * Finds the top-k largest colonies in a given map.
 *
 * @param map The map represented as a 2D vector of integers.
 * @param useDFS A boolean flag indicating whether to use Depth-First Search (DFS) or Breadth-First Search (BFS) algorithm.
 * @param k The number of top colonies to find.
 * @return A vector of pairs representing the size and resource type of the top-k largest colonies.
 */
vector<pair<int, int>> top_k_largest_colonies(vector<vector<int>> &map, bool useDFS, unsigned int k)
{
    auto start = high_resolution_clock::now();

    vector<pair<int, int>> colonySizes;
    for (size_t i = 0; i < map.size(); i++)
    {
        for (size_t j = 0; j < map[0].size(); j++)
        {
            if (map[i][j] == 0)
                continue; // Skip if resource type is 0
            int size = useDFS ? dfs(map, i, j, map[i][j]) : bfs(map, i, j, map[i][j]);
            colonySizes.push_back({size, map[i][j]});
        }
    }

    // Sorting logic remains the same
    sort(colonySizes.rbegin(), colonySizes.rend(), [](const pair<int, int> &a, const pair<int, int> &b)
         { return a.first == b.first ? a.second < b.second : a.first > b.first; });

    if (colonySizes.size() > k)
        colonySizes.resize(k);

    auto stop = high_resolution_clock::now();
    auto duration = duration_cast<nanoseconds>(stop - start);

    return colonySizes;
}

// Main function
int main(int argc, char *argv[])
{

    // Check the command line arguments
    if (argc != 4)
    {

        // It should be: <1 or 0> <k> <filename.txt>
        cerr << "Usage: " << argv[0] << " <1 or 0> <k> <filename.txt>" << endl;
        return 1;
    }

    // Get the arguments
    bool useDFS = (atoi(argv[1]) == 1);
    int k = atoi(argv[2]);

    // Read the map from the text file
    string filename = argv[3];
    ifstream file(filename);
    if (!file.is_open())
    {
        cerr << "Error: Unable to open file." << endl;
        return 1;
    }

    vector<vector<int>> sector_map;
    string line;
    while (getline(file, line))
    {
        istringstream iss(line);
        vector<int> row;
        int num;
        while (iss >> num)
        {
            row.push_back(num);
        }
        sector_map.push_back(row);
    }
    file.close();

    // Find the largest colony
    vector<pair<int, int>> result = top_k_largest_colonies(sector_map, useDFS, k);

    // Print the result
    cout << "Algorithm: " << (useDFS ? "DFS" : "BFS") << endl;
    cout << "Map: " << filename << endl;

    // If no colonies are found
    if (result.empty())
    {
        cout << "No colonies found." << endl;
        return 0;
    }

    // Print the top-k colonies
    for (size_t i = 0; i < result.size(); ++i)
    {
        cout << "Colony " << i + 1 << ": Size = " << result[i].first << ", Resource Type = " << result[i].second << endl;
    }

    return 0;
}