 #include <iostream>
#include <vector>
using namespace std;


bool dfs(int node, vector<vector<int>>& graph, vector<int>& state) {
    state[node] = 1; // Visiting


    for (int neighbor : graph[node]) {
        if (state[neighbor] == 1) {
            return true; // Found a cycle
        }
        if (state[neighbor] == 0 && dfs(neighbor, graph, state)) {
            return true;
        }
    }


    state[node] = 2; // Visited
    return false;
}


bool hasCircularDependency(int n, vector<vector<int>>& edges) {
    vector<vector<int>> graph(n);
    for (auto& edge : edges) {
        graph[edge[0]].push_back(edge[1]);
    }


    vector<int> state(n, 0); // 0 = unvisited, 1 = visiting, 2 = visited


    for (int i = 0; i < n; ++i) {
        if (state[i] == 0 && dfs(i, graph, state)) {
            return true;
        }
    }


    return false;
}


int main() {
    // Example 1: No cycle
    int n1 = 4;
    vector<vector<int>> edges1 = {{0, 1}, {1, 2}, {2, 3}};
    cout << "Example 1: " << (hasCircularDependency(n1, edges1) ? "true" : "false") << endl;


    // Example 2: Has cycle
    int n2 = 4;
    vector<vector<int>> edges2 = {{0, 1}, {1, 2}, {2, 0}};
    cout << "Example 2: " << (hasCircularDependency(n2, edges2) ? "true" : "false") << endl;


    // Example 3: Self-loop
    int n3 = 2;
    vector<vector<int>> edges3 = {{1, 1}};
    cout << "Example 3: " << (hasCircularDependency(n3, edges3) ? "true" : "false") << endl;


    return 0;
}



