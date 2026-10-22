"""Compresión Huffman y dos decodificadores implementados por el equipo."""

from pathlib import Path
import pickle

from bitarray import bitarray
from frecuencias import generarFrecuencias
from huffman_base import cargarHuffmanBase, construirHuffman


def comprimirArchivo(entrada, salida):
	"""Guarda encabezado pickle y bits comprimidos; devuelve el encabezado."""
	entrada, salida = Path(entrada), Path(salida)
	if entrada.resolve() == salida.resolve():
		raise ValueError("La salida no puede reemplazar el archivo original.")
	if salida.suffix.lower() != ".huff":
		raise ValueError("El archivo comprimido debe terminar en .huff.")
	text = entrada.read_bytes().decode("utf-8")
	s, freq = generarFrecuencias(entrada)
	codes_dic, root = construirHuffman(s, freq)
	codes = {}
	for c in codes_dic:
		codes[c] = bitarray(codes_dic[c], endian="big")

	bits = bitarray(endian="big")
	for c in text:
		bits.extend(codes[c])
	padding = (8 - len(bits) % 8) % 8
	header = {
		"version": 1,
		"codes_dic": codes_dic,
		"padding": padding,
		"root": root,
		"caracteres": len(text),
		"cantidad_bits": len(bits),
	}
	bits.extend([0] * padding)
	salida.parent.mkdir(parents=True, exist_ok=True)
	with salida.open("wb") as file:
		pickle.dump(header, file, protocol=4)
		bits.tofile(file)
	return header


def leerComprimido(entrada):
	"""Carga un .huff propio y devuelve (encabezado, bits sin padding)."""
	# Node debe existir con el mismo nombre de módulo usado al comprimir.
	cargarHuffmanBase()
	with open(entrada, "rb") as file:
		try:
			header = pickle.load(file)
		except (EOFError, pickle.UnpicklingError) as error:
			raise ValueError("Encabezado .huff incompleto o inválido.") from error
		bits = bitarray(endian="big")
		bits.frombytes(file.read())

	keys = {"version", "codes_dic", "padding", "root", "caracteres", "cantidad_bits"}
	if not isinstance(header, dict) or not keys.issubset(header):
		raise ValueError("Faltan datos en el encabezado .huff.")
	if header["version"] != 1:
		raise ValueError("Versión de .huff no compatible.")
	padding = header["padding"]
	if type(padding) is not int or not 0 <= padding <= 7:
		raise ValueError("Padding inválido.")
	if type(header["cantidad_bits"]) is not int or header["cantidad_bits"] < 0:
		raise ValueError("Cantidad de bits inválida.")
	if type(header["caracteres"]) is not int or header["caracteres"] < 0:
		raise ValueError("Cantidad de caracteres inválida.")
	if len(bits) != header["cantidad_bits"] + padding:
		raise ValueError("El flujo de bits está truncado o tiene bytes adicionales.")
	if padding:
		if any(bits[-padding:]):
			raise ValueError("El relleno debe contener únicamente ceros.")
		del bits[-padding:]
	return header, bits


def decodificarDiccionario(bits, codes_dic):
	"""Reconstruye el texto acumulando bits y consultando códigos inversos."""
	inverse = {}
	for c, code in codes_dic.items():
		if not code or any(bit not in "01" for bit in code) or code in inverse:
			raise ValueError("Diccionario de códigos inválido.")
		inverse[code] = c
	max_length = max((len(code) for code in inverse), default=0)
	text = []
	curr = ""
	for bit in bits:
		curr += "1" if bit else "0"
		if curr in inverse:
			text.append(inverse[curr])
			curr = ""
		elif len(curr) >= max_length:
			raise ValueError("Secuencia de bits sin un código válido.")
	if curr:
		raise ValueError("El flujo termina con un código incompleto.")
	return "".join(text)


def decodificarArbol(bits, root):
	"""Recorre el árbol original: 0 a la izquierda y 1 a la derecha."""
	if root is None:
		if bits:
			raise ValueError("Hay bits, pero no existe un árbol.")
		return ""
	if root.left is None and root.right is None:
		if any(bits):
			raise ValueError("El código de un único símbolo debe ser 0.")
		return root.char * len(bits)
	text = []
	node = root
	for bit in bits:
		node = node.right if bit else node.left
		if node is None:
			raise ValueError("El flujo contiene un recorrido inexistente.")
		if node.left is None and node.right is None:
			text.append(node.char)
			node = root
	if node is not root:
		raise ValueError("El flujo termina antes de alcanzar una hoja.")
	return "".join(text)


def descomprimirArchivo(entrada, salida, metodo="arbol"):
	"""Decodifica un .huff y escribe exactamente los bytes UTF-8 recuperados."""
	entrada, salida = Path(entrada), Path(salida)
	if entrada.resolve() == salida.resolve():
		raise ValueError("La salida no puede reemplazar el archivo comprimido.")
	if metodo not in ("diccionario", "arbol"):
		raise ValueError("El método debe ser diccionario o arbol.")
	header, bits = leerComprimido(entrada)
	if metodo == "diccionario":
		text = decodificarDiccionario(bits, header["codes_dic"])
	else:
		text = decodificarArbol(bits, header["root"])
	if len(text) != header["caracteres"]:
		raise ValueError("El texto recuperado tiene una longitud inesperada.")
	salida.parent.mkdir(parents=True, exist_ok=True)
	salida.write_bytes(text.encode("utf-8"))
	return text
