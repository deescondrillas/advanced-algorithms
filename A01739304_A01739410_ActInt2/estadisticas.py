"""Tablas por archivo/idioma y comparación de los dos decodificadores."""

from pathlib import Path
import csv
from statistics import mean, stdev
from time import perf_counter_ns

from compresion import (comprimirArchivo, descomprimirArchivo, leerComprimido,
	decodificarDiccionario, decodificarArbol)
from frecuencias import generarFrecuencias
from visualizacion import visualizarArbol

IDIOMAS = ("espanol", "frances", "ingles")


def calcularPromedioBits(s, freq, codes_dic):
	"""Calcula sum(frecuencia * longitud de código) / total de símbolos."""
	total = sum(freq)
	if total == 0:
		return 0.0
	bits = sum(freq[i] * len(codes_dic[s[i]]) for i in range(len(s)))
	return bits / total


def evaluarArchivo(entrada, idioma, resultados):
	"""Comprime, recupera por ambos métodos y verifica igualdad de bytes."""
	entrada, resultados = Path(entrada), Path(resultados)
	compressed = resultados / "comprimidos" / idioma / (entrada.stem + ".huff")
	header = comprimirArchivo(entrada, compressed)
	original = entrada.read_bytes()
	for metodo in ("diccionario", "arbol"):
		recovered = resultados / "recuperados" / idioma / metodo / entrada.name
		descomprimirArchivo(compressed, recovered, metodo)
		if recovered.read_bytes() != original:
			raise ValueError(f"La recuperación no coincide: {idioma}/{entrada.name}, {metodo}.")
	s, freq = generarFrecuencias(entrada)
	original_size = len(original)
	compressed_size = compressed.stat().st_size
	return {
		"archivo": entrada.name,
		"idioma": idioma,
		"original_bytes": original_size,
		"comprimido_bytes": compressed_size,
		"factor_compresion": compressed_size / original_size if original_size else None,
		"promedio_bits_simbolo": calcularPromedioBits(s, freq, header["codes_dic"]),
	}


def resumirPorIdioma(rows):
	"""Promedios aritméticos por documento; excluye factores indefinidos."""
	summary = []
	for idioma in IDIOMAS:
		group = [row for row in rows if row["idioma"] == idioma]
		if not group:
			continue
		factors = [row["factor_compresion"] for row in group if row["factor_compresion"] is not None]
		summary.append({
			"idioma": idioma,
			"archivos": len(group),
			"promedio_factor_compresion": mean(factors) if factors else None,
			"promedio_bits_simbolo": mean(row["promedio_bits_simbolo"] for row in group),
		})
	return summary


def medirDecodificacion(entrada, idioma, repeticiones=500):
	"""Mide llamadas completas en memoria; alterna orden y excluye E/S."""
	if repeticiones < 2:
		raise ValueError("Se necesitan al menos dos repeticiones para la desviación estándar.")
	header, bits = leerComprimido(entrada)
	methods = {
		"diccionario": (decodificarDiccionario, header["codes_dic"]),
		"arbol": (decodificarArbol, header["root"]),
	}
	expected = decodificarArbol(bits, header["root"])
	if len(expected) != header["caracteres"]:
		raise ValueError("El texto decodificado tiene una longitud inesperada.")
	# Calentamiento fuera de la medición.
	for function, data in methods.values():
		for _ in range(10):
			if function(bits, data) != expected:
				raise ValueError("Los decodificadores no coinciden.")
	times = {method: [] for method in methods}
	for i in range(repeticiones):
		order = ("diccionario", "arbol") if i % 2 == 0 else ("arbol", "diccionario")
		for method in order:
			function, data = methods[method]
			start = perf_counter_ns()
			text = function(bits, data)
			elapsed = perf_counter_ns() - start
			if text != expected:
				raise ValueError("La decodificación cambió durante la medición.")
			times[method].append(elapsed / 1000)
	return [{
		"archivo": Path(entrada).with_suffix(".txt").name,
		"idioma": idioma,
		"metodo": method,
		"repeticiones": repeticiones,
		"promedio_us": mean(values),
		"desviacion_estandar_us": stdev(values),
	} for method, values in times.items()]


def guardarTabla(rows, salida):
	"""Escribe CSV UTF-8 con BOM para abrir acentos correctamente en Excel."""
	if not rows:
		raise ValueError("No hay filas para guardar.")
	salida = Path(salida)
	salida.parent.mkdir(parents=True, exist_ok=True)
	with salida.open("w", encoding="utf-8-sig", newline="") as file:
		writer = csv.DictWriter(file, fieldnames=list(rows[0]))
		writer.writeheader()
		writer.writerows(rows)
	return salida


def procesarColeccion(textos, resultados, repeticiones=500):
	"""Evalúa todos los .txt por idioma; exige al menos diez en cada carpeta."""
	textos, resultados = Path(textos), Path(resultados)
	if repeticiones < 2:
		raise ValueError("Las repeticiones deben ser al menos dos.")
	groups = {idioma: sorted((textos / idioma).glob("*.txt"),
		key=lambda file: (file.stat().st_size, file.name)) for idioma in IDIOMAS}
	for idioma, files in groups.items():
		if len(files) < 10:
			raise ValueError(f"Se requieren 10 textos en {idioma}; se encontraron {len(files)}.")
	rows, timings = [], []
	for idioma, files in groups.items():
		for file in files:
			row = evaluarArchivo(file, idioma, resultados)
			rows.append(row)
			print(f"Verificado por ambos métodos: {idioma}/{file.name}")
		# El mayor archivo ofrece una medición menos dominada por llamadas breves.
		sample = max(files, key=lambda file: file.stat().st_size)
		compressed = resultados / "comprimidos" / idioma / (sample.stem + ".huff")
		timings.extend(medirDecodificacion(compressed, idioma, repeticiones))
		# El texto más corto produce una imagen manejable para la demostración.
		visual_sample = min(files, key=lambda file: file.stat().st_size)
		header, _ = leerComprimido(resultados / "comprimidos" / idioma / (visual_sample.stem + ".huff"))
		visualizarArbol(header["root"], resultados / "arboles" / (idioma + ".png"),
			f"Huffman · {idioma} · {visual_sample.name}")
	summary = resumirPorIdioma(rows)
	guardarTabla(rows, resultados / "estadisticas_archivos.csv")
	guardarTabla(summary, resultados / "estadisticas_idiomas.csv")
	guardarTabla(timings, resultados / "tiempos_decodificacion.csv")
	return rows, summary, timings
