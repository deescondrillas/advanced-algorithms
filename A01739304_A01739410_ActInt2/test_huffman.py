"""Pruebas reproducibles de recuperación exacta y casos límite."""

from pathlib import Path
from tempfile import TemporaryDirectory
import random
import subprocess
import sys
import unittest

from bitarray import bitarray
from compresion import (comprimirArchivo, descomprimirArchivo, leerComprimido,
	decodificarDiccionario, decodificarArbol)
from frecuencias import generarFrecuencias
from huffman_base import construirHuffman
from estadisticas import calcularPromedioBits, resumirPorIdioma, medirDecodificacion
from visualizacion import recopilarNodos


class PruebasHuffman(unittest.TestCase):
	@classmethod
	def setUpClass(cls):
		cls.base = Path(__file__).resolve().parent
		cls.temporary = TemporaryDirectory(prefix="pruebas_", dir=cls.base)
		cls.folder = Path(cls.temporary.name).resolve()
		# Verifica el destino antes de que TemporaryDirectory borre sus archivos.
		assert cls.folder.parent == cls.base
		cls.addClassCleanup(cls.temporary.cleanup)

	def comprobarRecuperacion(self, content):
		original = self.folder / "original.txt"
		compressed = self.folder / "original.huff"
		original.write_bytes(content)
		header = comprimirArchivo(original, compressed)
		loaded, bits = leerComprimido(compressed)
		self.assertEqual(len(bits), header["cantidad_bits"])
		self.assertEqual(loaded["padding"], header["padding"])
		for method in ("diccionario", "arbol"):
			recovered = self.folder / (method + ".txt")
			descomprimirArchivo(compressed, recovered, method)
			self.assertEqual(recovered.read_bytes(), content)
		return header

	def test_frecuencias_unicode_y_saltos(self):
		file = self.folder / "frecuencias.txt"
		file.write_bytes("áA á\r\n".encode("utf-8"))
		s, freq = generarFrecuencias(file)
		self.assertEqual(dict(zip(s, freq)), {"á": 2, "A": 1, " ": 1, "\r": 1, "\n": 1})

	def test_vacio(self):
		header = self.comprobarRecuperacion(b"")
		self.assertIsNone(header["root"])
		self.assertEqual(header["codes_dic"], {})
		self.assertEqual(header["padding"], 0)

	def test_simbolo_unico_y_todos_los_paddings(self):
		paddings = set()
		for n in range(1, 9):
			with self.subTest(repeticiones=n):
				header = self.comprobarRecuperacion(("ñ" * n).encode("utf-8"))
				self.assertEqual(header["codes_dic"], {"ñ": "0"})
				paddings.add(header["padding"])
		self.assertEqual(paddings, set(range(8)))

	def test_preserva_utf8_bom_crlf_y_emoji(self):
		self.comprobarRecuperacion("\ufeffEspañol: árbol.\r\nFrançais: été.\nEnglish: hello! 🐢\rfin".encode("utf-8"))

	def test_recuperacion_aleatoria_reproducible(self):
		rng = random.Random(21)
		for n in (2, 7, 50, 500, 2000):
			with self.subTest(longitud=n):
				text = "".join(rng.choice("abcde áéñçœ\n\r\t🙂") for _ in range(n))
				self.comprobarRecuperacion(text.encode("utf-8"))

	def test_promedio_ponderado(self):
		self.assertAlmostEqual(calcularPromedioBits(["a", "b", "c"], [5, 2, 1],
			{"a": "0", "b": "10", "c": "11"}), 11 / 8)
		self.assertEqual(calcularPromedioBits([], [], {}), 0)

	def test_resumen_promedia_documentos(self):
		rows = [
			{"idioma": "espanol", "factor_compresion": 2, "promedio_bits_simbolo": 3},
			{"idioma": "espanol", "factor_compresion": 4, "promedio_bits_simbolo": 5},
		]
		row = resumirPorIdioma(rows)[0]
		self.assertEqual(row["promedio_factor_compresion"], 3)
		self.assertEqual(row["promedio_bits_simbolo"], 4)

	def test_rechaza_flujo_truncado_y_bytes_extra(self):
		self.comprobarRecuperacion(b"abracadabra")
		original = (self.folder / "original.huff").read_bytes()
		file = self.folder / "danado.huff"
		for content in (original[:-1], original + b"\x00"):
			file.write_bytes(content)
			with self.assertRaises(ValueError):
				leerComprimido(file)

	def test_rechaza_padding_no_cero(self):
		self.comprobarRecuperacion(b"a")
		file = self.folder / "original.huff"
		content = bytearray(file.read_bytes())
		content[-1] |= 1
		file.write_bytes(content)
		with self.assertRaises(ValueError):
			leerComprimido(file)

	def test_rechaza_codigos_incompletos(self):
		with self.assertRaises(ValueError):
			decodificarDiccionario(bitarray("1"), {"a": "0", "b": "10", "c": "11"})
		codes, root = construirHuffman(["a", "b", "c"], [5, 2, 1])
		longest = max(codes.values(), key=len)
		with self.assertRaises(ValueError):
			decodificarArbol(bitarray(longest[:-1]), root)

	def test_evitar_reemplazar_entrada(self):
		file = self.folder / "protegido.txt"
		file.write_bytes(b"abc")
		with self.assertRaises(ValueError):
			comprimirArchivo(file, file)
		self.assertEqual(file.read_bytes(), b"abc")

	def test_pickle_en_proceso_nuevo_ambos_metodos(self):
		self.comprobarRecuperacion("Otro proceso: café, été, tea.\r\n".encode("utf-8"))
		for method in ("arbol", "diccionario"):
			output = self.folder / ("proceso_" + method + ".txt")
			result = subprocess.run([sys.executable, "-B", "-X", "utf8", str(self.base / "main.py"),
				"descomprimir", str(self.folder / "original.huff"), str(output), "--metodo", method],
				capture_output=True, text=True, encoding="utf-8", cwd=self.folder)
			self.assertEqual(result.returncode, 0, result.stderr)
			self.assertEqual(output.read_bytes(), (self.folder / "original.txt").read_bytes())

	def test_coordenadas_arbol_conservan_estructura(self):
		_, root = construirHuffman(["a", "b", "c"], [5, 2, 1])
		nodes, edges = recopilarNodos(root)
		self.assertEqual(len(nodes), 5)
		self.assertEqual(len(edges), 4)
		for parent, child, bit in edges:
			expected = nodes[parent]["node"].left if bit == "0" else nodes[parent]["node"].right
			self.assertIs(nodes[child]["node"], expected)
			self.assertLess(nodes[child]["y"], nodes[parent]["y"])

	def test_medicion_devuelve_ambos_metodos(self):
		self.comprobarRecuperacion(b"abracadabra")
		rows = medirDecodificacion(self.folder / "original.huff", "espanol", 3)
		self.assertEqual({row["metodo"] for row in rows}, {"arbol", "diccionario"})
		for row in rows:
			self.assertEqual(row["repeticiones"], 3)
			self.assertGreater(row["promedio_us"], 0)
			self.assertGreaterEqual(row["desviacion_estandar_us"], 0)


if __name__ == "__main__":
	unittest.main(verbosity=2)
