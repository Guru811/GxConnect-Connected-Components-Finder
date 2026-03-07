/*
  GxConnect — Connected Components Finder
  Feature 1: Core Graph Structure
*/

#include <iostream>
#include <vector>
#include <algorithm>

using namespace std;

//  Utility
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

//  Graph — Adjacency list, undirected
class Graph {
public:
    int V;
    vector<vector<int>> adj;
    vector<pair<int,int>> edges;

    Graph(int v) : V(v), adj(v) {}

    // Adds an undirected edge; ignores duplicates and self-loops
    void addEdge(int u, int v) {
        if (u >= 0 && u < V && v >= 0 && v < V && u != v)
            if (find(adj[u].begin(), adj[u].end(), v) == adj[u].end()) {
                adj[u].push_back(v);
                adj[v].push_back(u);
                edges.push_back({u, v});
            }
    }

    int edgeCount() const { return (int)edges.size(); }

    void printInfo() const {
        blank();
        cout << "  Graph Info\n";
        cout << "  Vertices : " << V << "\n";
        cout << "  Edges    : " << edgeCount() << "\n";
        blank();
        for (int i = 0; i < V; i++) {
            cout << "  Node " << i << " : ";
            if (adj[i].empty()) cout << "(isolated)";
            for (int j = 0; j < (int)adj[i].size(); j++) {
                cout << adj[i][j];
                if (j < (int)adj[i].size() - 1) cout << " ";
            }
            cout << "\n";
        }
        blank();
    }
};

//  Main
int main() {
    heading("GxConnect  —  Feature 1: Core Graph Structure");

    Graph g(6);
    g.addEdge(0, 1);
    g.addEdge(1, 2);
    g.addEdge(3, 4);

    g.printInfo();

    cout << "  Core graph structure working correctly.\n";
    blank();
    return 0;
}