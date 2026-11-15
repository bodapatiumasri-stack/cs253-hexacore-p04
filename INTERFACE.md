# Common Graph Interface

### Graph Conventions

- The graph is undirected.
- Vertices are numbered from `1` to `n`.
- `adj` contains `n + 1` entries; index `0` is unused.
- Edge IDs are numbered from `0` to `m - 1`.
- `edges[id] = {u, v}` stores the endpoints of edge `id`.
- Each non-self-loop edge is stored in both endpoint adjacency lists:
  - `{v, id}` in `adj[u]`
  - `{u, id}` in `adj[v]`
- Parallel edges are allowed and are distinguished by their edge IDs.
- The graph may be disconnected.
- Self-loops are allowed and are stored in the original edge list. A self-loop is stored only once in the corresponding adjacency list.

### Self-Loop Conventions

All modules must handle self-loops consistently.

- A self-loop does not change the number of connected components.
- A self-loop is never a bridge.
- A self-loop does not create an articulation point.
- Self-loops are ignored when determining bridges and two-edge-connected components.
- Self-loops are excluded from the biconnected-component decomposition.
- A vertex that has only self-loops and no other incident edges is treated as an isolated vertex for the block-cut forest.
- Self-loops must not cause duplicate traversal or incorrect component counts.

---

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
It must ignore self-loops for articulation point,bridge,and biconnected-component calculations.
It must distinguish parallel edges using edge Ids.

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
It must distinguish parallel edges using edge IDs.
It must ignore self-loops when determining bridges and two-edge-connected components.

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

### Expected output
```cpp
struct OracleResult {
     int originalComponentCount;
    vector<int> articulationPoints;
    vector<int> bridges;
    vector<int> componentsAfterVertexRemoval;
    vector<int> componentsAfterEdgeRemoval;
};

```

### Output Format
- originalComponentCount: Number of connected components in the original graph.
- articulationPoints: Vertices whose removal increases the number of connected components.
- bridges: Edges whose removal increases the number of connected components.
- componentsAfterVertexRemoval[v]: Number of connected components after removing vertex v.
- componentsAfterEdgeRemoval[id]: Number of connected components after removing edge id.

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

OracleResult runOracle(const Graph& graph);
```
- A vertex is an articulation point if:
  componentsAfterVertexRemoval[v] > originalComponentCount
- An edge is a bridge if:
  componentsAfterEdgeRemoval[id] > originalComponentCount
- Must correctly handle disconnected graphs, parallel edges, and self-loops.
- Removing a self-loop must not change the component count.
- Must not use Structure A or Structure B internally.

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

### Expected output
```cpp
struct BlockCutForest {
    int nodeCount = 0;
    vector<vector<int>> adj;
    vector<int> vertexToNode;
    vector<int> articulationNode;
    vector<int> treeId;
    vector<int> tin;
    vector<int> tout;
    vector<int> depth;
    vector<vector<int>> up;
}; 
```

### Output format

- nodeCount:Total number of nodes in the block-cut forest
- adj:Adjacency list representing connections between block and articulation nodes
- vertexToNode:Maps each original graph vertex to its corresponding forest node
- articulationNode:Maps each articulation vertex to its corresponding articulation node;    -1 for non-articulation vertices
- treeId:Identifies which tree of the forest each node belongs to
- tin:DFS entry time of each forest node
- tout:DFS exit time of each forest node
- depth:Depth of each forest node in its tree
- up:Binary lifting table storing ancestors of each forest node

Conventions:
- Original graph vertices are numbered from 1 to n.
- Block-cut forest nodes are numbered from 0 to nodeCount - 1.
- Block nodes and articulation nodes must have distinct forest IDs.
- Every articulation node must be connected to all block nodes containing that articulation vertex.
- Every original vertex must have a valid mapping in vertexToNode.
- Isolated vertices must be represented as standalone forest nodes.
- The forest may contain multiple disconnected trees.
- Self-loops must not create blocks or articulation nodes.

### Function signature — Building block-cut forest
```cpp
BlockCutForest buildBlockCutForest(
    const Graph& graph,
    const StructureAResult& result
); 
```
Input
- graph: Original graph adjacency list.
- result: Output of Structure A containing articulation points and biconnected components.

Expected output
A BlockCutForest containing:
- All block nodes.
- All articulation nodes.
- Connections between blocks and articulation nodes.
- Mapping from original vertices to forest nodes.
- DFS preprocessing information required for efficient queries.

Function responsibilities
1. Read articulation points from Structure A.
2. Read biconnected components from Structure A.
3. Create one block node for each biconnected component.
4. Create one articulation node for each articulation vertex.
5. Connect articulation nodes to all blocks containing them.
6. Assign forest nodes to original vertices.
7. Handle isolated vertices.
8. Construct the block-cut forest for disconnected graphs.
9. Preprocess each tree using DFS.
10. Build the binary lifting table for LCA queries.



### Function signature — Vertex-failure connectivity query
```cpp
bool isDisconnectedAfterFailure(
    const BlockCutForest& forest,
    int x,
    int y,
    int failedVertex
);
```

Input
- forest: Previously constructed block-cut forest.
- x: First original graph vertex.
- y: Second original graph vertex.
- failedVertex: Vertex assumed to have failed.

Expected output
A boolean value:
- true: Vertices x and y become disconnected after removing failedVertex.
- false: Vertices x and y remain connected after removing failedVertex.
Output format
Return value	Meaning
true	x and y are disconnected after vertex failure
false	x and y remain connected after vertex failure

Query conventions
- If failedVertex == x or failedVertex == y, return true.
- If x and y are already in different connected components, return true.
- If x == y and that vertex is not removed, return false.
- If failedVertex is not an articulation point and differs from both endpoints, return false.
- Otherwise, check whether the articulation node corresponding to failedVertex lies on the block-cut forest path between the nodes representing x and y.
- If the articulation node lies on that path, return true.
- Otherwise, return false.
Implementation requirement
- Use the preprocessed block-cut forest.
- Use DFS entry/exit times and binary lifting for efficient path queries.
- Do not rebuild the block-cut forest for every query

## 6. Application

**Owner: D3 — Architect**
The application combines the results of Structure A, Structure B, the Oracle and the block-cut forest to provide a graph robustness analysis.
Expected functionality
The application must provide:
1. Critical Vertex Report
   - Display articulation points.
   - Display the number of connected components after removing each vertex.
   - Rank vertices according to their impact on graph connectivity.
2. Critical Edge Report
   - Display bridges identified by Structure A and Structure B.
   - Compare bridge results with the Oracle wherever the Oracle finishes.
   - Report whether the bridge detection methods agree.
3. Vertex-Failure Connectivity Queries
   - Accept two vertices x and y.
   - Accept a failed vertex v.
   - Determine whether x and y become disconnected after removing v.
4. Efficient Query Processing
   - Use the preprocessed block-cut forest.
   - Use DFS entry/exit times and binary lifting for path queries.
   - Avoid rebuilding the block-cut forest for every query.

### Expected output
```cpp
struct CriticalNode {
    int vertex;
    int componentsAfterRemoval;
    int additionalComponents;
};
```
### Output format
- vertex: Original graph vertex ID.
- componentsAfterRemoval: Number of connected components after removing that vertex.
- additionalComponents: Increase in connected components compared with the original graph.

Calculation
\[
additionalComponents =
componentsAfterRemoval - originalComponentCount
\]

### Function signature
```cpp
vector<CriticalNode> rankCriticalNodes(
    const OracleResult& oracleResult
);
```
Input
- oracleResult: Output of the Oracle containing the original component count and component counts after vertex removal.

Expected output
A vector of CriticalNode structures containing all original graph vertices ranked according to their impact on graph connectivity.

Ranking rules
- Use componentsAfterVertexRemoval from the Oracle.
- Calculate additionalComponents for every vertex.
- Sort vertices in descending order of additionalComponents.
- Vertices causing the greatest increase in connected components must appear first.
- For equal values, sort by ascending vertex ID.

## 7. Integration and Testing Rules

- All modules must use the same vertex numbering and edge IDs.
- All returned bridge IDs must refer to the original edge list.
- Structure A and Structure B must be implemented independently.
- Bridge results must be compared against the Oracle on small test graphs.
- Disconnected graphs must be tested.
- Long-path inputs must be used to test iterative DFS.
- Function signatures must be agreed upon before integration.
- Each member must commit their work to their own branch and use pull requests for integration.



