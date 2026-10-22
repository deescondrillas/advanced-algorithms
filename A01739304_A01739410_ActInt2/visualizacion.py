"""Dibuja el árbol ya construido por el código de clase con Matplotlib."""

from pathlib import Path


def recopilarNodos(root):
	"""Asigna coordenadas a las hojas y centra cada padre sobre sus hijos."""
	nodes, edges = [], []
	leaves = 0

	def visitar(node, depth):
		"""Recorrido recursivo que devuelve el índice del nodo agregado."""
		nonlocal leaves
		index = len(nodes)
		nodes.append({"node": node, "x": 0, "y": -depth})
		children = []
		for child, bit in ((node.left, "0"), (node.right, "1")):
			if child is not None:
				child_index = visitar(child, depth + 1)
				children.append(child_index)
				edges.append((index, child_index, bit))
		if children:
			nodes[index]["x"] = sum(nodes[c]["x"] for c in children) / len(children)
		else:
			nodes[index]["x"] = leaves
			leaves += 1
		return index

	if root is not None:
		visitar(root, 0)
	return nodes, edges


def visualizarArbol(root, salida, titulo="Árbol de Huffman"):
	"""Guarda una imagen PNG; no necesita abrir ventanas ni instalar Graphviz."""
	import matplotlib
	matplotlib.use("Agg")
	import matplotlib.pyplot as plt

	nodes, edges = recopilarNodos(root)
	leaves = sum(n["node"].left is None and n["node"].right is None for n in nodes)
	depth = max((-n["y"] for n in nodes), default=0)
	fig, ax = plt.subplots(figsize=(max(10, leaves * 0.85), max(5, (depth + 2) * 0.9)))
	for parent, child, bit in edges:
		a, b = nodes[parent], nodes[child]
		ax.plot([a["x"], b["x"]], [a["y"], b["y"]], color="#94a3b8", zorder=1)
		ax.text((a["x"] + b["x"]) / 2, (a["y"] + b["y"]) / 2, bit,
			ha="center", va="center", fontsize=9, backgroundcolor="white")
	for item in nodes:
		node = item["node"]
		leaf = node.left is None and node.right is None
		label = f"{node.char!r}\nf={node.data}" if leaf else f"f={node.data}"
		ax.text(item["x"], item["y"], label, ha="center", va="center", fontsize=9,
			bbox={"boxstyle": "round,pad=0.4", "facecolor": "#dcfce7" if leaf else "#dbeafe",
				"edgecolor": "#475569"}, zorder=2)
	if not nodes:
		ax.text(0.5, 0.5, "Texto vacío: no hay árbol", ha="center", transform=ax.transAxes)
	ax.set_title(titulo + "\n0: izquierda · 1: derecha · f: frecuencia", fontsize=14, pad=24)
	ax.set_xlim(-1, max(1, leaves))
	ax.set_ylim(-depth - 0.7, 0.7)
	ax.axis("off")
	fig.tight_layout()
	salida = Path(salida)
	salida.parent.mkdir(parents=True, exist_ok=True)
	fig.savefig(salida, dpi=150, bbox_inches="tight")
	plt.close(fig)
	return salida
