"""
 * Descripción: Implementación de función complementaria conteo de caracteres.
 * Autor: Sandra E. Barajas Montiel
 * Última modificación: Octavio & Franco | 2026-10-12
"""

from pathlib import Path
import runpy
import sys


# Genera las listas de caracteres y frecuencias para huffmanCodes.
def generarFrecuencias(file_name):
	"""Lee un archivo UTF-8 y devuelve (s, freq) en el mismo orden."""
	freq_dic = {}
	with open(file_name, "r", encoding="utf-8", newline="") as file:
		text = file.read()

	# Se conservan mayúsculas, acentos, espacios y saltos de línea.
	for c in text:
		if c in freq_dic:
			freq_dic[c] += 1
		else:
			freq_dic[c] = 1

	s = list(freq_dic.keys())
	freq = list(freq_dic.values())
	return s, freq


if __name__ == "__main__":
	base_dir = Path(__file__).resolve().parent
	file_name = sys.argv[1] if len(sys.argv) > 1 else base_dir / "texto_prueba.txt"
	s, freq = generarFrecuencias(file_name)
	if not s:
		raise SystemExit("El archivo está vacío; no se puede construir el árbol.")

	print(f"Archivo: {Path(file_name).name}")
	print(f"Total de caracteres: {sum(freq)}")
	print("\nFrecuencias de caracteres:")
	for i in range(len(s)):
		print(f"{s[i]!r} = {freq[i]}")

	base = runpy.run_path(str(base_dir / "05_uffman.py"))
	codes_dic, root = base["huffmanCodes"](s, freq)
	print("\nÁrbol de Huffman:")
	print(root)

	codes_dic = dict(sorted(codes_dic.items()))
	print("Códigos de Huffman:")
	for c in codes_dic:
		print(f"{c!r}={codes_dic[c]}")
