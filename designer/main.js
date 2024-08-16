class NodeEditDialog {
	constructor(container)
	{
		this.container = container;
	}
	
	open(node, outboundConnections, possibleConnections)
	{
		this.container.innerHTML = "";
		this.container.style.display = "";
		
		var opts = "";
		for (var possibleConnection in possibleConnections)
		{
			opts += "<option value=\"" + possibleConnection + "\">" + possibleConnection + "</option>";
		}
		this.container.innerHTML += "<select>" + opts + "</select>";
	}
	
	close()
	{
		this.container.style.display = "none";
	}
}

var nodeEditDialog = new NodeEditDialog(document.getElementById("dialog"));

function getSetDifference(set1, set2) {
	console.log(set1);
	console.log(set2);
	return set1.filter(item => !set2.includes(item));
}

class FSM {
	constructor(container) {
		this.graph = new graphology.Graph();
		this.sigma = new Sigma.Sigma(
			this.graph,
			container,
			{
				renderEdgeLabels: true,
				renderLabels: true,
				enableEdgeClickEvents: true,
				enableEdgeWheelEvents: true,
				enableEdgeHoverEvents: true,
				allowInvalidContainer: true,
				stagePadding: 0
			});
		
		this.sigma.on("clickStage", this.handleCanvasClicked);
		this.sigma.on("clickNode", this.handleNodeClicked);
	}
	
	runNoverlap()
	{
		graphologyLibrary.layoutNoverlap.assign(this.graph, {scale: 1});
	}
	
	handleCanvasClicked(e)
	{
		let name = prompt("Enter node name", "");
		if (name == null || name == "") return;
		this.graph.addNode(name, { x: 50, y: 50, label: name, size: 20 });
		graphologyLibrary.layoutNoverlap.assign(this.graph, {scale: 1});
	}
	
	handleNodeClicked(e)
	{
		let target = prompt("Enter target node name", "");
		if (target == null || target == "") return;
		this.graph.addDirectedEdge(e.node, target, { size: 5, type: "arrow", label: e.node + " --> " + target, forceLabel: true});
		graphologyLibrary.layoutNoverlap.assign(this.graph, {scale: 1});
		
		//var outbound = this.graph.outboundNeighbors(e.node);
		//nodeEditDialog.open(e.node, outbound, getSetDifference(this.graph.nodes(), outbound));
	}
	
	getOutboundNodes(node)
	{
		this.graph.outboundNeighbors(node);
	}
	
	getAll
};

var fsm = new FSM(document.getElementById("container"));