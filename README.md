# Archivator Zip/Unzip

This is a simple command-line archivator that can compress and decompress files using the zlib library.

## Features

* Compress and decompress files.
* Supports basic options:
    * `-v` (verbose):  Show more information during compression/decompression.
    * `-b` (blocksize): Define the block size (default is 16 KB).

## Usage

### Zip

```
zip [-v] [-b BLOCKSIZE] <input_file> <output_file>
```

* **-v**: Enables verbose mode, showing progress information.
* **-b**: Specifies the block size for compression (in bytes).  The default is 16 KB.

### Unzip

```
unzip [-v] <archive_file> <output_file>
```

* **-v**: Enables verbose mode, showing progress information.

## Examples

* **Compress a file:**
  ```
  zip -v -b 32768 myfile.txt myfile.zip
  ```
* **Decompress a file:**
  ```
  unzip -v myfile.zip myfile.txt
  ```

## Building

1. **Prerequisites:**
   * GCC compiler
   * Zlib library

2. **Options:**
   * `OUTPUT_DIR`: Directory, where will be result of building
   * `BINARY_DIR`: Directory, where will be result binaries
   * `CC`: C compiler(gcc by default)
   * `CXX`: C++ compiler(g++ by default)
   * `ARGP_LIB_DIRS`: Directory, where argp library placed in format -L(direcory)
   * `Z_LIB_DIRS`: Directory, where zlib library placed in format -L(direcory)
   * `ARGP_INCLUDE_DIRS`: Directory, where argp include files placed in format -L(direcory)
   * `Z_INCLUDE_DIRS`: Directory, where zlib include files placed in format -L(direcory)
   * `sanitize`: If defined builds with sinitizer options

3. **Build:**
   ```bash
   make
   ```

4. **Run:**
   * `./zip` for compression
   * `./unzip` for decompression

## Testing
   **Run the tests:**
   ```bash
   make tests
   ```

## Coverage

1. **Generate coverage report:**
   ```bash
   make coverage
   ```
2. **Open the report:**
   * `out/index.html`

## Analyzing

1. **Generate coverage report:**
   ```bash
   make analyze
   ```
2. **Open the report:**
   * `out/static-anlyze.txt`

## Security
   **Функции-антиотдладчики**
   1. SEH_handler - выполняется в случае получения SIGTRAP, который вызывается, когда флаг trap установлен
   2. is_ptraced_proc - проверяет /proc/self/status на запись трассировщика
   3. is_ptraced_syscall - проверяет, соовпадают ли родительский PID, полученный с помощью getppid и полученный с помощью прямого системного вызова
   
   **Опции компилятора**
   1. -fstack-protector-strong - добавляет канарейку во все функции, в которых происходит присваивание локальной переменной
   2. -fPIE - рандомизирует расположение секций в памяти, что усложняет злоумышленнику нахождение необходимой информации