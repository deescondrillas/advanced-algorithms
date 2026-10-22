"""Carga del algoritmo de clase y adaptación de los casos especiales."""

from pathlib import Path
import importlib.util
import sys


def cargarHuffmanBase():
	"""Devuelve el módulo original, registrado con un nombre estable para pickle."""
	name = "_huffman_actividad_base"
	if name not in sys.modules:
		file_name = Path(__file__).resolve().parent / "05_uffman.py"
		spec = importlib.util.spec_from_file_location(name, file_name)
		base = importlib.util.module_from_spec(spec)
		sys.modules[name] = base
		try:
			spec.loader.exec_module(base)
		except Exception:
			del sys.modules[name]
			raise
	return sys.modules[name]


def construirHuffman(s, freq):
	"""Usa huffmanCodes de clase; contempla vacío y un solo símbolo."""
	if not s:
		return {}, None
	base = cargarHuffmanBase()
	codes_dic, root = base.huffmanCodes(s, freq)
	# La base asigna "" a una raíz que es hoja. Usamos un bit por aparición.
	if len(s) == 1:
		codes_dic[s[0]] = "0"
	return codes_dic, root
