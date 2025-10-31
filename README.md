# 🧩 Proyecto Final 

## 👩‍💻 Autores

* **Samuel Arango**
* **Alyson Henao** 
* **Samuel Moncada**
* **Emily Cardona**

**Proyecto Final - Sistemas Operativos (2025-2)**  


---

## 📘 Descripción General

Este proyecto implementa una **utilidad de línea de comandos en C++** capaz de **comprimir y descomprimir (pendiente de encriptar y desencriptar) archivos de texto (.txt)** utilizando el **algoritmo de compresión Huffman**, desarrollado completamente **desde cero** y **sin uso de librerías externas**, implementado completamente por nuestro equipo.

El objetivo principal es ofrecer una herramienta eficiente (superando métodos básicos como RLE) y sin pérdida de información, que reduzca significativamente el tamaño de archivos de texto.  
El proyecto cumple con los principios de **eficiencia, concurrencia y manejo directo de archivos** exigidos en la asignatura de Sistemas Operativos. 

**Logros actuales:**
- ✅ Mejora significativa en tiempo de compresión para archivos .txt
- ✅ Implementación del algoritmo más eficiente y recomendado
- ✅ Producto mínimo funcional operativo
- ✅ Capacidad de comprimir y descomprimir archivos .txt
- ✅ Cálculo automático de la tasa de compresión

---

## ⚙️ Funcionamiento

El programa funciona desde la terminal mediante los siguientes comandos:

### 🔹 Compresión
```bash
./app -c -i <archivo_entrada.txt> -o <archivo_salida.hf2>
```

* `-c` → indica compresión
* `-i` → archivo de entrada (.txt)
* `-o` → archivo de salida comprimido (.hf2)

Durante la compresión, el programa:

1. Lee el archivo utilizando **llamadas al sistema POSIX** (`open`, `read`, `write`)
2. Calcula la **frecuencia de cada carácter** del texto
3. Construye el **árbol de Huffman** y asigna códigos binarios más cortos a los caracteres más frecuentes
4. Genera un archivo comprimido en formato **HF2**, que contiene:
   * Encabezado con tamaño original y frecuencias
   * Bitstream codificado con Huffman

**Modo AutoSafe estricto:**
> Solo se genera el archivo comprimido si el resultado **es estrictamente menor** al tamaño original.
> Esto garantiza que la compresión **nunca aumente el tamaño del archivo**.

---

### 🔹 Descompresión

```bash
./app -d -i <archivo_entrada.hf2> -o <archivo_salida.txt>
```

* `-d` → indica descompresión

El programa reconstruye el árbol de Huffman a partir del encabezado y restaura el archivo original **exactamente igual** al texto fuente.

---

## 🚀 Próximos Pasos y Plan de Desarrollo

### 🔐 Implementación de Algoritmos de Cifrado

**Investigación de algoritmos viables:**

| Algoritmo | Complejidad | Ventajas | Estado |
|-----------|-------------|----------|---------|
| **Cifrado Vigenère** | Baja | Simple de implementar, buen punto de partida | ⭐ Prioridad Alta |
| **DES (Data Encryption Standard)** | Media | Estándar histórico, 16 rondas bien documentadas | ⭐ Prioridad Media |
| **AES Simplificado** | Alta | Seguridad moderna, operaciones criptográficas completas | 🔮 Futuro |

**Plan de implementación por fases:**

#### Fase 1: Cifrado Vigenère (aún estamos investigando si vale la pena encriptar por Vigenère o existen mejores posibilidades).
```cpp
// Estructura propuesta
class VigenereCipher {
private:
    std::string key;
    
public:
    void setKey(const std::string& key);
    std::string encrypt(const std::string& text);
    std::string decrypt(const std::string& ciphertext);
};
```

#### Fase 2: DES Simplificado 
- Implementar 8 rondas en lugar de 16 para reducir complejidad
- Operaciones: Permutación inicial, rondas Feistel, permutación final
- Generación de subclaves para cada ronda

#### Fase 3: AES Lite 
- S-box estándar de AES
- Operación ShiftRows simplificada
- MixColumns con operaciones en GF(2⁸)
- 4 rondas en lugar de 10/12/14

### 📄 Soporte para Formatos de Documentos

**Estrategia de implementación:**

#### Fase 1: Análisis de formatos 
- **PDF**: Investigar estructura de archivos PDF y extracción de texto
- **DOCX**: Analizar formato ZIP+XML de documentos Word
- **ODT**: Estructura OpenDocument basada en XML

#### Fase 2: Extracción de contenido 
```cpp
class DocumentProcessor {
public:
    std::string extractTextFromPDF(const std::string& filename);
    std::string extractTextFromDOCX(const std::string& filename);
    std::string extractTextFromODT(const std::string& filename);
};
```

#### Fase 3: Integración con Huffman
- Compresión del texto extraído
- Preservación de metadatos esenciales
- Reconstrucción del formato original después de descompresión

### 🔄 Flujo de Trabajo Propuesto

```
Archivo Entrada → Detección Formato → Extracción Texto → [Cifrado] → Compresión Huffman → Archivo .hf2
```

```
Archivo .hf2 → Descompresión Huffman → [Descifrado] → Reconstrucción Formato → Archivo Original
```

### 🎯 Objetivos Específicos

**Fase 1**
- [ ] Investigación completa de algoritmos de cifrado
- [ ] Implementación de Vigenère básico
- [ ] Análisis de estructuras de PDF y DOCX

**Fase 2**
- [ ] Integración cifrado en pipeline de compresión
- [ ] Desarrollar extractor de texto para PDF
- [ ] Pruebas de compatibilidad con formatos mixtos

**Fase 3**
- [ ] Implementación de DES simplificado
- [ ] Soporte completo para DOCX
- [ ] Optimización de rendimiento

**Fase 4**
- [ ] Refinamiento y pruebas exhaustivas
- [ ] Documentación completa
- [ ] Preparación para entrega final

---

## 🧠 Algoritmo de Compresión: **Huffman**

El **algoritmo de Huffman** es uno de los métodos de compresión sin pérdida más eficientes conocidos. Su principio se basa en la **codificación de longitud variable**, donde los símbolos más frecuentes usan menos bits, logrando una mejor relación de compresión.

### 🔬 Ventajas sobre otros algoritmos

| Algoritmo                     | Tipo        | Ventajas                                                                | Desventajas                                                   |
| ----------------------------- | ----------- | ----------------------------------------------------------------------- | ------------------------------------------------------------- |
| **Huffman**                   | Sin pérdida | Óptimo para textos y datos con patrones repetitivos. Estructura simple. | Requiere leer todo el archivo para calcular frecuencias.      |
| **RLE (Run-Length Encoding)** | Sin pérdida | Muy simple y rápido.                                                    | Ineficiente si el archivo no tiene repeticiones consecutivas. |
| **LZW (Lempel–Ziv–Welch)**    | Sin pérdida | Compresión adaptable sin preprocesar frecuencias.                       | Más complejo de implementar; tablas dinámicas.                |

➡️ **Conclusión:** Huffman logra **mejor balance entre eficiencia, simplicidad y compresión real** en textos extensos. En pruebas realizadas, se obtuvieron reducciones de **hasta 60% del tamaño original** en archivos de texto con contenido repetitivo.

---

## 🧾 Tipos de Archivos Admitidos

### ✅ Actualmente Implementado
| Tipo   | Estado         | Descripción                                      |
| ------ | -------------- | ------------------------------------------------ |
| `.txt` | ✅ Implementado | Archivos de texto plano con cualquier contenido. |

### 🔄 En Desarrollo
| Tipo   | Prioridad | Estado        | Descripción |
|--------|-----------|---------------|-------------|
| `.pdf` | Alta      | 🔄 En proceso | Documentos PDF |
| `.docx`| Alta      | 🔄 En proceso | Documentos Word |
| `.odt` | Media     | 🕐 Planeado   | Documentos OpenOffice |

### 🔮 Futuras Versiones
* **Cifrado integrado** → Vigenère, DES simplificado, o AES-lite
* **Procesamiento por lotes** → Directorios completos
* **Interfaz gráfica** → Versión con UI para usuarios finales

---

## 🧰 Tecnologías y Librerías

* **Lenguaje:** C++17
* **Compilador:** GCC (g++)
* **E/S de bajo nivel:** llamadas POSIX (`open`, `read`, `write`, `close`, `stat`, `chmod`)
* **Sin dependencias externas**
* **Próximas:** Implementación propia de algoritmos criptográficos

---

## 🧪 Ejemplo de uso

```bash
# Compilar
g++ -std=c++17 -O2 cli.cpp file_util.cpp huffman.cpp -o app

# Comprimir un archivo
./app -c -i prueba.txt -o prueba.hf2

# Comprimir con cifrado (futuro)
./app -c -i documento.pdf --encrypt vigenere -k "clave-secreta" -o documento_secure.hf2

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


**Próximo Hito:** Implementación de cifrado Vigenère + soporte PDF

*Última actualización: Marzo 2025*
