# 🧩 Proyecto Final - Compresor Huffman (Sistemas Operativos)

## 👩‍💻 Autores

* **Samuel Arango**
* **Alyson Henao**
* **Samuel Moncada**
* **Emily Cardona**

**Proyecto Final - Sistemas Operativos (2025-2)**
**Programa de Ingeniería de Sistemas**


## 📘 Descripción General

Este proyecto implementa una **utilidad de línea de comandos en C++** capaz de **comprimir y descomprimir archivos de texto (.txt)** utilizando el **algoritmo de compresión Huffman**, desarrollado completamente **desde cero** y **sin uso de librerías externas**.

El objetivo principal es ofrecer una herramienta eficiente y sin pérdida de información, que reduzca significativamente el tamaño de archivos de texto.  
El proyecto cumple con los principios de **eficiencia, concurrencia y manejo directo de archivos** exigidos en la asignatura de Sistemas Operativos.

---

## ⚙️ Funcionamiento

El programa funciona desde la terminal mediante los siguientes comandos:

### 🔹 Compresión
```bash
./app -c -i <archivo_entrada.txt> -o <archivo_salida.hf2>
```

* `-c` → indica compresión.
* `-i` → archivo de entrada (.txt).
* `-o` → archivo de salida comprimido (.hf2).

Durante la compresión, el programa:

1. Lee el archivo utilizando **llamadas al sistema POSIX** (`open`, `read`, `write`).
2. Calcula la **frecuencia de cada carácter** del texto.
3. Construye el **árbol de Huffman** y asigna códigos binarios más cortos a los caracteres más frecuentes.
4. Genera un archivo comprimido en formato **HF2**, que contiene:

   * Encabezado con tamaño original y frecuencias.
   * Bitstream codificado con Huffman.

Además, incluye un modo **AutoSafe estricto**:

> Solo se genera el archivo comprimido si el resultado **es estrictamente menor** al tamaño original.
> Esto garantiza que la compresión **nunca aumente el tamaño del archivo**.

---

### 🔹 Descompresión

```bash
./app -d -i <archivo_entrada.hf2> -o <archivo_salida.txt>
```

* `-d` → indica descompresión.
  El programa reconstruye el árbol de Huffman a partir del encabezado y restaura el archivo original **exactamente igual** al texto fuente.

---

## 🧠 Algoritmo de Compresión: **Huffman**

El **algoritmo de Huffman** es uno de los métodos de compresión sin pérdida más eficientes conocidos.
Su principio se basa en la **codificación de longitud variable**, donde los símbolos más frecuentes usan menos bits, logrando una mejor relación de compresión.

### 🔬 Ventajas sobre otros algoritmos

| Algoritmo                     | Tipo        | Ventajas                                                                | Desventajas                                                   |
| ----------------------------- | ----------- | ----------------------------------------------------------------------- | ------------------------------------------------------------- |
| **Huffman**                   | Sin pérdida | Óptimo para textos y datos con patrones repetitivos. Estructura simple. | Requiere leer todo el archivo para calcular frecuencias.      |
| **RLE (Run-Length Encoding)** | Sin pérdida | Muy simple y rápido.                                                    | Ineficiente si el archivo no tiene repeticiones consecutivas. |
| **LZW (Lempel–Ziv–Welch)**    | Sin pérdida | Compresión adaptable sin preprocesar frecuencias.                       | Más complejo de implementar; tablas dinámicas.                |

➡️ **Conclusión:** Huffman logra **mejor balance entre eficiencia, simplicidad y compresión real** en textos extensos.
En pruebas realizadas, se obtuvieron reducciones de **hasta 60 % del tamaño original** en archivos de texto con contenido repetitivo.

---

## 🧾 Tipos de Archivos Admitidos

Actualmente el programa soporta:

| Tipo   | Estado         | Descripción                                      |
| ------ | -------------- | ------------------------------------------------ |
| `.txt` | ✅ Implementado | Archivos de texto plano con cualquier contenido. |

### 🔮 Próximas versiones

* **`.pdf`** → Se planea añadir compatibilidad para comprimir documentos PDF, aplicando la misma política *AutoSafe* (solo se comprime si realmente reduce el tamaño).
* **Directorios** → Procesamiento concurrente de múltiples archivos.
* **Cifrado** → Posibilidad de aplicar encriptación simétrica (Vigenère / AES-lite) tras la compresión.

---

## 🧰 Tecnologías y Librerías

* **Lenguaje:** C++17
* **Compilador:** GCC (g++)
* **E/S de bajo nivel:** llamadas POSIX (`open`, `read`, `write`, `close`, `stat`, `chmod`).
* **Sin dependencias externas.**

---

## 🧪 Ejemplo de uso

```bash
# Compilar
g++ -std=c++17 -O2 cli.cpp file_util.cpp huffman.cpp -o app

# Comprimir un archivo
./app -c -i prueba.txt -o prueba.hf2

# Descomprimirlo
./app -d -i prueba.hf2 -o salida.txt

# Verificar igualdad
diff prueba.txt salida.txt && echo "Archivos iguales ✅"
```

**Salida esperada:**

```
OK: comprimido 732286 / 1300000 bytes (TC=0.56, reduccion=43.7%)
OK: descomprimido
Archivos iguales ✅
```

---

## 📊 Resultados obtenidos

| Archivo                | Tamaño Original | Tamaño Comprimido | Reducción |
| ---------------------- | --------------- | ----------------- | --------- |
| `prueba.txt`           | 1.3 MB          | 716 KB            | 43.7 %    |
| `texto_repetitivo.txt` | 2.1 MB          | 380 KB            | 82 %      |

---


---

## 🧭 Licencia

Proyecto académico de uso libre con fines educativos.
