fetch('graph.json')
  .then(res => res.json())
  .then(data => {
    console.log("[DEBUG] graph.json loaded:", data);

    const Graph = ForceGraph3D()(document.getElementById('3d-graph'))
      .graphData(data)
      .nodeLabel(node => `${node.title || node.id}\nFlags: ${node.flags}`)
      .nodeAutoColorBy('flags')
      .nodeOpacity(0.85)
      .nodeVal(node => 2 + (node.flags || 0) * 0.3)
      .backgroundColor('#000');

    // ✅ Responsive resizing
    window.addEventListener('resize', () => {
      Graph.width(window.innerWidth);
      Graph.height(window.innerHeight);
    });
  });
