Promise.all([
  fetch('graph.json').then(res => res.json()),
  fetch('search.json').then(res => res.json())
])
.then(([fullGraph, traversal]) => {
  // Track which nodes and edges were visited/traversed
  const visitedSet = new Set(traversal.nodes.map(n => n.id));
  const traversedSet = new Set(
    traversal.links.filter(l => l.traversed)
                   .map(l => `${l.source}->${l.target}`)
  );

  // Mark visited nodes
  fullGraph.nodes.forEach(node => {
    node.visited = visitedSet.has(node.id);
  });

  // Mark traversed links
  fullGraph.links.forEach(link => {
    const key = `${link.source}->${link.target}`;
    link.traversed = traversedSet.has(key);
  });

  const Graph = ForceGraph3D()(document.getElementById('3d-graph'))
    .graphData(fullGraph)
    .backgroundColor('#111')

    // Node color based on group + visited status
    .nodeColor(node => {
      if (node.id === "ORIGINAL") return 'rgba(11, 44, 190, 1)';

      const colors = {
        "Safe":       node.visited ? 'rgba(29, 173, 31, 1)'  : 'rgba(29, 173, 31, 0.25)',
        "Mild":       node.visited ? 'rgba(225, 229, 21, 1)' : 'rgba(225, 229, 21, 0.25)',
        "Borderline": node.visited ? 'rgba(250, 151, 3, 1)'  : 'rgba(250, 151, 3, 0.25)',
        "Risky":      node.visited ? 'rgba(255, 103, 2, 1)'  : 'rgba(255, 103, 2, 0.25)',
        "Avoid":      node.visited ? 'rgba(231, 79, 8, 1)'   : 'rgba(231, 79, 8, 0.25)',
        "Extreme":    node.visited ? 'rgba(248, 5, 5, 1)'    : 'rgba(248, 5, 5, 0.25)',
      };

      const group = (node.group || '').replace(/[^a-zA-Z]/g, '');
      return colors[group] || (node.visited ? '#aaa' : 'rgba(170,170,170,1)');
    })

    // Node size by group, larger for more severe content
    .nodeVal(node => {
      if (node.id === "ORIGINAL") return 20;

      const groupSizes = {
        Safe: 5, Mild: 6, Borderline: 7,
        Risky: 8, Avoid: 10, Extreme: 12
      };

      const group = (node.group || '').replace(/[^a-zA-Z]/g, '');
      return groupSizes[group] || 4;
    })

    // Node label (title only)
    .nodeLabel(node => `${node.title || node.id}`)

    // Traversed edge styling
    .linkColor(link => link.traversed ? 'rgba(231, 94, 208, 1)' : 'rgba(252, 252, 252, .5)')
    .linkWidth(link => link.traversed ? 4 : 0.5)
    .linkOpacity(0.2)
    .linkCurvature(link => link.traversed ? 0.1 : 0)

    // Traversed arrow styling
    .linkDirectionalArrowLength(link => link.traversed ? 10 : 0)
    .linkDirectionalArrowRelPos(1)
    .linkDirectionalArrowColor(link => link.traversed ? 'magenta' : null)

    // Layout forces
    .linkDirectionalParticles(0)
    .d3Force('charge', d3.forceManyBody().strength(-150))
    .d3Force('link').distance(1000);

  // Keep graph responsive on window resize
  window.addEventListener('resize', () => {
    Graph.width(window.innerWidth);
    Graph.height(window.innerHeight);
  });

  // Center camera on the start node
  document.getElementById('center-start-btn').addEventListener('click', () => {
    const start = fullGraph.nodes.find(n => n.id === "ORIGINAL");
    if (!start) return;

    const distance = 800;
    const distRatio = 1 + distance / Math.hypot(start.x, start.y, start.z);

    Graph.cameraPosition(
      { x: start.x * distRatio, y: start.y * distRatio, z: start.z * distRatio },
      start,
      3000
    );
  });
});