

# 🕵️‍♂️ Sistema de Compresión y Encriptación 

Proyecto académico que implementa un sistema completo de **compresión sin pérdida** mediante *Huffman* y un sistema de **encriptación reversible** basado en un *cifrado tipo stream* con XOR.
El programa incluye un **menú interactivo**, genera archivos automáticamente y verifica la integridad de cada proceso.

---

## 👥 Integrantes del equipo

* **Samuel Arango Echeverri** 
* **Emily Cardona** 
* **Samuel Moncada** 
* **Alyson Henao** 

---

## 🧱 Arquitectura general del proyecto

El sistema está compuesto por varios módulos en C++:

* **cli.cpp**
  Maneja el menú interactivo, la lógica del usuario, la secuencia de operaciones y comparaciones byte a byte.

* **huffman.cpp / huffman.hpp**
  Implementa el algoritmo de compresión *Huffman* y el formato propio **HF2**
  (cabecera + frecuencias + bitstream).

* **crypto.cpp / crypto.hpp**
  Implementa un cifrado tipo *stream cipher* basado en:

  * Hash FNV para generar semilla.
  * Generador lineal congruencial (LCG).
  * XOR entre datos originales y keystream.

* **file_util.cpp / file_util.hpp**
  Funciones de lectura y escritura **binaria**, manejo de permisos y utilidades del sistema de archivos.

---

## 📂 Archivos generados automáticamente

El sistema trabaja con los siguientes archivos:

| Archivo                  | Descripción                                                     |
| ------------------------ | --------------------------------------------------------------- |
| `base.txt`               | Archivo original ingresado por el usuario.                      |
| `textocomprimido.txt`    | Archivo comprimido usando Huffman (formato HF2).                |
| `textodescomprimido.txt` | Salida de descomprimir HF2. Se compara con el archivo original. |
| `textoencriptado.txt`    | Archivo encriptado usando XOR + keystream.                      |
| `textodesencriptado.txt` | Archivo desencriptado y comparado con `base.txt`.               |

El sistema confirma automáticamente si el archivo recuperado es **idéntico** al original.

---

## ⚙️ Compilación

Ejecutar en la carpeta del proyecto:

```bash
g++ -std=c++17 -O2 cli.cpp huffman.cpp file_util.cpp crypto.cpp -o app
```

---

## ▶️ Ejecución del programa

```bash
./app
```

El menú aparece así:

```
=============================================
  Sistema de compresion y encriptacion 007
  Archivo base: base.txt (XXXXX bytes)
=============================================
Selecciona la operacion a realizar:
  -c  Comprimir base.txt -> textocomprimido.txt
  -d  Descomprimir textocomprimido.txt -> textodescomprimido.txt
         (y comparar con base.txt)
  -e  Encriptar base.txt -> textoencriptado.txt
  -u  Desencriptar textoencriptado.txt -> textodesencriptado.txt
         (y comparar con base.txt)
  -s  Salir
```

Después de cada operación, el programa regresa al menú principal.

---

## 🧩 Algoritmos utilizados

### 🔹 Compresión (Huffman + HF2)

La compresión incluye:

1. Conteo de frecuencias de cada byte.
2. Construcción del árbol Huffman.
3. Generación del bitstream comprimido.
4. Empaquetado en formato **HF2** con:

   * Magic `HF2`.
   * Método (1 = Huffman).
   * Tamaño original.
   * Frecuencias (256 enteros de 32 bits).
   * Bitstream comprimido.

La compresión es **sin pérdida**.
La descompresión reconstruye los datos **exactamente** como estaban.

El sistema muestra también la **tasa de compresión**:

```
Tasa de compresion (TC): X.XXXXX | Reduccion: YY.YY%
```

---

### 🔹 Encriptación (XOR + keystream pseudoaleatorio)

1. El usuario ingresa una contraseña.
2. Se genera una semilla mediante hash tipo FNV-1a.
3. Con la semilla se alimenta un **LCG (Linear Congruential Generator)**.
4. El LCG genera un flujo de bytes pseudoaleatorios (keystream).
5. Cada byte del archivo original se encripta así:

```
C[i] = P[i] XOR K[i]
```

### 🔹 Desencriptación

Basta aplicar XOR nuevamente:

```
P[i] = (P[i] XOR K[i]) XOR K[i]
```

Este algoritmo es **100% reversible**, seguro para uso académico y muy eficiente.

---

## 🔍 Verificación automática de integridad

Luego de descomprimir y desencriptar, el programa compara **byte a byte** contra `base.txt`.

Ejemplo de salida:

```
Verificacion: El archivo DESCOMPRIMIDO es IDENTICO a base.txt
```

```
Verificacion: El archivo DESENCRIPTADO es IDENTICO a base.txt
```

Esto prueba:

* Que la compresión es *sin pérdida*.
* Que la encriptación es totalmente *reversible*.

---

## 🚀 Conclusión

El sistema combina:

* Un algoritmo de compresión clásico (Huffman).
* Un cifrado reversible tipo stream (XOR + LCG).
* Manejo binario seguro de archivos.
* Menú interactivo intuitivo.
* Verificación automática de integridad.

Es un proyecto modular, claro y completamente funcional para la presentación académica.

---

