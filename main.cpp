/*
  GxConnect - Connected Components Finder
  Feature 5: Step-by-Step Visualizers (BFS + DFS)
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

// Undirected graph represented as an adjacency list
class Graph {
public:
    int V;
    vector<vector<int>> adj;
    vector<pair<int,int>> edges;

    Graph(int v) : V(v), adj(v) {}

    // Adds an undirected edge between u and v, ignoring duplicates and self-loops
    void addEdge(int u, int v) {
        if (u >= 0 && u < V && v >= 0 && v < V && u != v)
            if (find(adj[u].begin(), adj[u].end(), v) == adj[u].end()) {
                adj[u].push_back(v);
                adj[v].push_back(u);
                edges.push_back({u, v});
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

// Finds all connected components using Breadth-First Search (O(V + E))
AlgoResult runBFS(const Graph& g) {
    AlgoResult res; res.name="BFS"; res.complexity="O(V+E)";
    int ops=0; vector<bool> visited(g.V,false);
    auto t0=high_resolution_clock::now();
    for(int i=0;i<g.V;i++){
        if(!visited[i]){
            vector<int> comp; queue<int> q;
            q.push(i); visited[i]=true; ops++;
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
    AlgoResult res; res.name="DFS"; res.complexity="O(V+E)";
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

// Prints a step-by-step trace of BFS traversal, showing queue state at each step
void bfsStepByStep(const Graph& g) {
    heading("BFS  Step-by-Step Visualizer");
    cout << "  How BFS works:\n";
    cout << "  Start at a node, visit all neighbors before going deeper.\n";
    cout << "  Uses a QUEUE  ( first in, first out )\n";
    blank();

    vector<bool> visited(g.V, false);
    int compNum = 0;

    for (int start = 0; start < g.V; start++) {
        if (visited[start]) continue;
        blank();
        cout << "  Starting from Node " << start
             << "  =>  Component #" << compNum << "\n";
        cout << "  ----------------------------------------\n";

        queue<int> q;
        q.push(start); visited[start] = true;
        int step = 1;

        while (!q.empty()) {
            cout << "\n  Step " << step++ << "\n";
            cout << "  Queue  :  [ ";
            queue<int> qcopy = q;
            while (!qcopy.empty()) {
                cout << qcopy.front(); qcopy.pop();
                if (!qcopy.empty()) cout << "  ";
            }
            cout << " ]\n";

            int node = q.front(); q.pop();
            cout << "  Process:  Node " << node << "\n";
            cout << "  Neighbors:\n";

            for (int nb : g.adj[node]) {
                if (!visited[nb]) {
                    cout << "            Node " << nb << "  -> added to queue\n";
                    visited[nb] = true; q.push(nb);
                } else {
                    cout << "            Node " << nb << "  -> already visited\n";
                }
            }
            if (g.adj[node].empty()) cout << "            (no neighbors)\n";
        }

        blank();
        cout << "  Component #" << compNum++ << " done.\n";
    }

    blank();
    cout << "  BFS complete.  Total components: " << compNum << "\n";
    blank();
}

// Prints a step-by-step trace of DFS traversal, showing stack state at each step
void dfsStepByStep(const Graph& g) {
    heading("DFS  Step-by-Step Visualizer");
    cout << "  How DFS works:\n";
    cout << "  Start at a node, go as deep as possible before backtracking.\n";
    cout << "  Uses a STACK  ( last in, first out )\n";
    blank();

    vector<bool> visited(g.V, false);
    int compNum = 0;

    for (int start = 0; start < g.V; start++) {
        if (visited[start]) continue;
        blank();
        cout << "  Starting from Node " << start
             << "  =>  Component #" << compNum << "\n";
        cout << "  ----------------------------------------\n";

        stack<int> st;
        st.push(start); visited[start] = true;
        int step = 1;

        while (!st.empty()) {
            cout << "\n  Step " << step++ << "\n";
            stack<int> scopy = st;
            vector<int> sv;
            while (!scopy.empty()) { sv.push_back(scopy.top()); scopy.pop(); }
            cout << "  Stack  :  top -> [ ";
            for (int i = 0; i < (int)sv.size(); i++) {
                cout << sv[i];
                if (i < (int)sv.size()-1) cout << "  ";
            }
            cout << " ] <- bottom\n";

            int node = st.top(); st.pop();
            cout << "  Process:  Node " << node << "\n";
            cout << "  Neighbors:\n";

            for (int nb : g.adj[node]) {
                if (!visited[nb]) {
                    cout << "            Node " << nb << "  -> pushed to stack\n";
                    visited[nb] = true; st.push(nb);
                } else {
                    cout << "            Node " << nb << "  -> already visited\n";
                }
            }
            if (g.adj[node].empty()) cout << "            (no neighbors)\n";
        }

        blank();
        cout << "  Component #" << compNum++ << " done.\n";
    }

    blank();
    cout << "  DFS complete.  Total components: " << compNum << "\n";
    blank();
}

int main() {
    Graph g(9);
    g.addEdge(0,1); g.addEdge(1,2); g.addEdge(2,3); g.addEdge(0,3);
    g.addEdge(4,5); g.addEdge(5,6);
    g.addEdge(7,8);

    bfsStepByStep(g);
    dfsStepByStep(g);

    return 0;
}