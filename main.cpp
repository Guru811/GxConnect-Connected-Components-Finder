/*
  GxConnect - Connected Components Finder
  Feature 6: Algorithm Comparison Table
*/

#include <iostream>
#include <vector>
#include <queue>
#include <stack>
#include <unordered_map>
#include <algorithm>
#include <chrono>
#include <numeric>
#include <functional>
#include <iomanip>

using namespace std;
using namespace chrono;

// Utility functions for formatted console output
void blank() { cout << "\n"; }

void heading(const string& title) {
    blank();
    cout << "  ==========================================================\n";
    cout << "  " << title << "\n";
    cout << "  ==========================================================\n";
    blank();
}

void subheading(const string& title) {
    blank();
    cout << "  " << title << "\n";
    cout << "  ----------------------------------------------------------\n";
}

// Undirected graph represented as an adjacency list and edge list
class Graph {
public:
    int V;
    vector<vector<int>> adj;
    vector<pair<int,int>> edges;

    Graph(int v) : V(v), adj(v) {}

    // Adds an undirected edge between u and v, ignoring duplicates and self-loops
    void addEdge(int u, int v) {
        if (u>=0&&u<V&&v>=0&&v<V&&u!=v)
            if (find(adj[u].begin(),adj[u].end(),v)==adj[u].end()) {
                adj[u].push_back(v); adj[v].push_back(u); edges.push_back({u,v});
            }
    }

    int edgeCount() const { return (int)edges.size(); }
};

// Stores the output of a single algorithm run
struct AlgoResult {
    string name, complexity;
    vector<vector<int>> components;
    long long microseconds;
    int operations, componentCount;
};

// Disjoint set data structure with path compression and union by rank
class UnionFind {
public:
    vector<int> parent, rank_;
    int components, operations;

    UnionFind(int n): parent(n), rank_(n,0), components(n), operations(0) {
        iota(parent.begin(),parent.end(),0);
    }

    // Returns the root of x's set, applying path compression recursively
    int find(int x) { operations++; if(parent[x]!=x) parent[x]=find(parent[x]); return parent[x]; }

    // Merges the sets containing x and y; returns false if already in the same set
    bool unite(int x, int y) {
        int px=find(x),py=find(y); if(px==py) return false;
        if(rank_[px]<rank_[py]) swap(px,py);
        parent[py]=px; if(rank_[px]==rank_[py]) rank_[px]++;
        components--; operations++; return true;
    }

    // Groups all nodes by their root to produce the final list of components
    vector<vector<int>> getComponents(int V) {
        unordered_map<int,vector<int>> comp;
        for(int i=0;i<V;i++) comp[find(i)].push_back(i);
        vector<vector<int>> result;
        for(auto&[k,v]:comp) result.push_back(v);
        return result;
    }
};

// Finds all connected components using Breadth-First Search (O(V + E))
AlgoResult runBFS(const Graph& g) {
    AlgoResult res; res.name="BFS"; res.complexity="O(V + E)";
    int ops=0; vector<bool> visited(g.V,false);
    auto t0=high_resolution_clock::now();
    for(int i=0;i<g.V;i++){
        if(!visited[i]){
            vector<int> comp; queue<int> q; q.push(i); visited[i]=true; ops++;
            while(!q.empty()){
                int node=q.front(); q.pop(); comp.push_back(node); ops++;
                for(int nb:g.adj[node]){ops++; if(!visited[nb]){visited[nb]=true;q.push(nb);}}
            }
            res.components.push_back(comp);
        }
    }
    auto t1=high_resolution_clock::now();
    res.microseconds=duration_cast<microseconds>(t1-t0).count();
    res.operations=ops; res.componentCount=(int)res.components.size();
    return res;
}

// Finds all connected components using Depth-First Search (O(V + E))
AlgoResult runDFS(const Graph& g) {
    AlgoResult res; res.name="DFS"; res.complexity="O(V + E)";
    int ops=0; vector<bool> visited(g.V,false);
    auto t0=high_resolution_clock::now();
    // Recursive lambda that visits all nodes in the current component
    function<void(int,vector<int>&)> dfs=[&](int u,vector<int>& comp){
        visited[u]=true; comp.push_back(u); ops++;
        for(int nb:g.adj[u]){ops++; if(!visited[nb]) dfs(nb,comp);}
    };
    for(int i=0;i<g.V;i++){
        if(!visited[i]){vector<int> comp; dfs(i,comp); res.components.push_back(comp);}
    }
    auto t1=high_resolution_clock::now();
    res.microseconds=duration_cast<microseconds>(t1-t0).count();
    res.operations=ops; res.componentCount=(int)res.components.size();
    return res;
}

// Finds all connected components using Union-Find (O(E * a(V)))
AlgoResult runUnionFind(const Graph& g) {
    AlgoResult res; res.name="Union-Find"; res.complexity="O(E * a(V))";
    auto t0=high_resolution_clock::now();
    UnionFind uf(g.V);
    for(auto&[u,v]:g.edges) uf.unite(u,v);
    res.components=uf.getComponents(g.V);
    auto t1=high_resolution_clock::now();
    res.microseconds=duration_cast<microseconds>(t1-t0).count();
    res.operations=uf.operations; res.componentCount=(int)res.components.size();
    return res;
}

// Prints a formatted comparison table of runtime, operation count, and component count for all algorithms
void printComparisonTable(const vector<AlgoResult>& results, int V, int E) {
    heading("Algorithm Comparison");

    cout << "  Graph  :  Vertices = " << V << "   Edges = " << E << "\n";
    blank();

    cout << "  "
         << left << setw(14) << "Algorithm"
         << setw(14) << "Complexity"
         << setw(14) << "Time (us)"
         << setw(12) << "Operations"
         << setw(12) << "Components"
         << "\n";
    cout << "  ----------------------------------------------------------\n";

    for (const auto& r : results) {
        cout << "  "
             << left << setw(14) << r.name
             << setw(14) << r.complexity
             << setw(14) << r.microseconds
             << setw(12) << r.operations
             << setw(12) << r.componentCount
             << "\n";
    }
    cout << "  ----------------------------------------------------------\n";
    blank();

    // Identifies and displays the fastest algorithm by elapsed time
    auto fastest = min_element(results.begin(), results.end(),
        [](const AlgoResult& a, const AlgoResult& b){ return a.microseconds < b.microseconds; });
    cout << "  Fastest  :  " << fastest->name
         << "  ( " << fastest->microseconds << " microseconds )\n";
    blank();

    // Renders a proportional bar chart where shorter bars indicate faster runtimes
    cout << "  Speed comparison  ( shorter bar = faster ):\n";
    blank();

    long long maxT = max(1LL, max_element(results.begin(), results.end(),
        [](const AlgoResult& a, const AlgoResult& b){ return a.microseconds < b.microseconds; })->microseconds);

    for (const auto& r : results) {
        int bar = (int)(30.0 * r.microseconds / maxT) + 1;
        cout << "  " << left << setw(12) << r.name << "  |";
        for (int x = 0; x < bar; x++) cout << "=";
        cout << "  " << r.microseconds << " us\n";
    }
    blank();
}

// Prints the components found by the first algorithm in the results vector
void printComponents(const vector<AlgoResult>& results) {
    if (results.empty()) return;
    const auto& r = results[0];
    subheading("Components Found");
    for (int i = 0; i < (int)r.components.size(); i++) {
        cout << "  Component " << i << "  ( " << r.components[i].size() << " nodes )  :  ";
        for (int k = 0; k < (int)r.components[i].size(); k++) {
            cout << r.components[i][k];
            if (k < (int)r.components[i].size()-1) cout << " - ";
        }
        cout << "\n";
    }
    blank();
    cout << "  Total components  :  " << r.componentCount << "\n";
    blank();
}

int main() {
    heading("GxConnect  -  Feature 6: Algorithm Comparison");

    Graph g(9);
    g.addEdge(0,1); g.addEdge(1,2); g.addEdge(2,3); g.addEdge(0,3);
    g.addEdge(4,5); g.addEdge(5,6);
    g.addEdge(7,8);

    vector<AlgoResult> results;
    results.push_back(runBFS(g));
    results.push_back(runDFS(g));
    results.push_back(runUnionFind(g));

    printComparisonTable(results, g.V, g.edgeCount());
    printComponents(results);

    return 0;
}