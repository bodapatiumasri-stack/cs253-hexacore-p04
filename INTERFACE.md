# Common Graph Interface

## 1. Graph Representation
We use an undirected graph.
- Vertices are numbered from `1` to `n`.
- Every edge has a unique ID from `0` to `m-1`.
- The graph may be disconnected.
- Each undirected edge is stored in both endpoints' adjacency lists.
- Edge IDs are used to distinguish edges, including parallel edges.

### Common C++ data structures
```cpp
struct Edge {
    int to;
    int id;
};
using Graph = vector<vector<Edge>>;
```
The graph is represented using an adjacency list.
An edge between vertices `u` and `v` with ID `id` is stored as:
```cpp
adj[u].push_back({v, id});
adj[v].push_back({u, id});
```
The original edge list should also be retained:
```cpp
vector<pair<int, int>> edges;
```
The edge ID is the index in this original edge list.

## 2. Structure A — Main Graph Analysis
**Owner: D1**
Structure A performs the main graph decomposition.
It must identify:
1. Articulation points.
2. Bridges.
3. Biconnected components.
It uses iterative DFS and Tarjan's discovery-time and low-link values.

### Expected output
```cpp
struct StructureAResult {
    vector<int> articulationPoints;
    vector<int> bridges;
    vector<vector<int>> biconnectedComponents;
};
```
Here:
- `articulationPoints` contains critical vertex IDs.
- `bridges` contains bridge edge IDs.
- Each biconnected component contains its edge IDs.

### Output Format
- `articulationPoints`: List of articulation vertex IDs.
- `bridges`: List of bridge edge IDs.
- `biconnectedComponents`: Groups of edge IDs belonging to each biconnected component.


### Function Signature
```cpp
StructureAResult analyzeStructureA(
    const Graph& adj,
    int n
);
```

## 3. Structure B — Independent Bridge Detection
**Owner: D2**
Structure B independently finds bridges using:
- DFS tree construction.
- Non-tree edge identification.
- Union-find (DSU) path marking.
- Path skipping to avoid processing already marked tree edges.
It also identifies two-edge-connected components.


### Expected output
```cpp
struct StructureBResult {
    vector<int> bridges;
    vector<vector<int>> twoEdgeConnectedComponents;
};
```
The returned values are edge IDs of bridges.

### Output Format
- `bridges`: List of bridge edge IDs.
- `twoEdgeConnectedComponents`: Groups of vertices belonging to the same two-edge-connected component.
The bridges found by Structure B must agree with Structure A and the Oracle wherever the Oracle finishes.

### Function Signature
```cpp
StructureBResult computeStructureB(
    const Graph& adj,
    int n
);
```
## 4. Oracle — Verification
**Owner: D4**
The Oracle verifies graph connectivity after removing vertices or edges.
It uses BFS to calculate the number of connected components.
It must support:
- Counting connected components in the original graph.
- Counting components after removing a vertex.
- Counting components after removing an edge.

### Input
- `adj`: Graph adjacency list.
- `n`: Number of vertices.
- `removedVertex`: Vertex to ignore, or -1 if none.
- `removedEdge`: Edge ID to ignore, or -1 if none.

### Output Format
- Returns the number of connected components after removal.

### Verification
- An edge is a bridge if removing it increases the number of connected components.
- A vertex is an articulation point if removing it increases the number of connected components.
The Oracle is intended for graphs up to approximately 5000 vertices.

### Function Signature
```cpp
int countComponents(
    const Graph& adj,
    int n,
    int removedVertex = -1,
    int removedEdge = -1
);
```

## 5. Block-Cut Forest — Architect Module

**Owner: D3**
The Architect module constructs a block-cut forest using the biconnected components produced by Structure A.

### Basic idea
The block-cut forest contains two types of nodes:
1. Block nodes: represent biconnected components.
2. Articulation nodes: represent articulation vertices.
An articulation node is connected to every block node containing that articulation vertex.
For a disconnected original graph, the resulting block-cut structure is a forest.

### Expected functionality
The Architect module must:
- Construct the block-cut forest.
- Maintain a mapping from original vertices to their corresponding block or articulation nodes.
- Preprocess the forest for efficient path queries.
- Answer vertex-failure connectivity queries.

### Query
```cpp
bool isDisconnectedAfterFailure(
    int x,
    int y,
    int failedVertex
);
```

The query asks whether `x` and `y` become disconnected when `failedVertex` is removed.

Important cases:
- If `x` or `y` equals `failedVertex`, the query must follow the agreed endpoint-removal convention.
- If the vertices are initially disconnected, they remain disconnected.
- If the failed vertex is not an articulation point and is distinct from both query vertices, its removal does not disconnect two previously connected surviving vertices.
The path-based separation test applies when the failed vertex is an articulation point distinct from both query endpoints.

## 6. Application

**Owner: D3 — Architect**
The application combines the results of Structure A and the block-cut forest.

It must provide:
1. A report of critical vertices.
2. A report of critical edges.
3. Vertex-failure connectivity queries.
4. Efficient query processing using the block-cut forest.
The application must use the common graph representation and module interfaces.

## 7. Integration and Testing Rules

- All modules must use the same vertex numbering and edge IDs.
- All returned bridge IDs must refer to the original edge list.
- Structure A and Structure B must be implemented independently.
- Bridge results must be compared against the Oracle on small test graphs.
- Disconnected graphs must be tested.
- Long-path inputs must be used to test iterative DFS.
- Function signatures must be agreed upon before integration.
- Each member must commit their work to their own branch and use pull requests for integration.



