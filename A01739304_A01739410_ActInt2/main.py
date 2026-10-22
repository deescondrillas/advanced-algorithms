"""Punto de entrada del problema 1. Ejecutar con --help para ver opciones."""

from pathlib import Path
import argparse
import sys

from compresion import comprimirArchivo, descomprimirArchivo, leerComprimido
from estadisticas import procesarColeccion
from visualizacion import visualizarArbol


def main():
	"""Interpreta la acción solicitada y llama al módulo correspondiente."""
	base_dir = Path(__file__).resolve().parent
	parser = argparse.ArgumentParser(description="Actividad Integradora 2 · Huffman")
	actions = parser.add_subparsers(dest="accion", required=True)

	compress = actions.add_parser("comprimir", help="Convertir un texto UTF-8 a .huff")
	compress.add_argument("entrada", type=Path)
	compress.add_argument("salida", type=Path)
	decompress = actions.add_parser("descomprimir", help="Recuperar el texto de un .huff")
	decompress.add_argument("entrada", type=Path)
	decompress.add_argument("salida", type=Path)
	decompress.add_argument("--metodo", choices=("diccionario", "arbol"), default="arbol")
	plot = actions.add_parser("arbol", help="Guardar una imagen del árbol de un .huff")
	plot.add_argument("entrada", type=Path)
	plot.add_argument("salida", type=Path)
	all_files = actions.add_parser("evaluar", help="Procesar la colección y generar las tablas")
	all_files.add_argument("--textos", type=Path, default=base_dir / "textos")
	all_files.add_argument("--resultados", type=Path, default=base_dir / "resultados")
	all_files.add_argument("--repeticiones", type=int, default=500)
	args = parser.parse_args()
	try:
		if args.accion == "comprimir":
			header = comprimirArchivo(args.entrada, args.salida)
			print(f"Comprimido: {args.salida}")
			print(f"Caracteres: {header['caracteres']} | Bits: {header['cantidad_bits']} | Padding: {header['padding']}")
		elif args.accion == "descomprimir":
			descomprimirArchivo(args.entrada, args.salida, args.metodo)
			print(f"Recuperado mediante {args.metodo}: {args.salida}")
		elif args.accion == "arbol":
			header, _ = leerComprimido(args.entrada)
			visualizarArbol(header["root"], args.salida, f"Huffman · {args.entrada.name}")
			print(f"Imagen guardada: {args.salida}")
		else:
			rows, _, _ = procesarColeccion(args.textos, args.resultados, args.repeticiones)
			print(f"\nTerminó la evaluación de {len(rows)} archivos. Resultados: {args.resultados}")
	except (OSError, ValueError) as error:
		print(f"Error: {error}", file=sys.stderr)
		return 1
	return 0


if __name__ == "__main__":
	raise SystemExit(main())
