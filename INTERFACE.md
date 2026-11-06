# Common Graph Interface

## 1. Graph Representation
- The graph is undirected.
- Vertices are numbered from 1 to n.
- Every edge has a unique ID.
- The graph may be disconnected.

## 2. Structure A
- Finds articulation points.
- Finds bridges.
- Finds biconnected components.

## 3. Structure B
- Independently finds bridges.

## 4. Oracle
- Uses BFS to verify connectivity after vertex or edge removal.
- Used for testing on small graphs.

## 5. Application
- Reports critical vertices and edges.
- Answers vertex-failure queries.
- Uses a block-cut forest for efficient queries.

## 6. Integration
- All modules follow the same graph conventions.
- Function signatures and output formats will be agreed upon by the team.
