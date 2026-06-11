# UB Sort Labs - Polyglot Performance Profiler Backend

UB Sort Labs is a high-performance, polyglot sorting algorithm analyzer and performance profiler. The backend orchestrates the compilation, execution, and metric-gathering of sorting algorithms written in 7 different programming languages. By tracking execution speed, raw comparison operations, and array swaps, it provides deep statistical comparisons of language runtimes and algorithmic complexity.

---

## 🚀 Polyglot Engine Architecture

Each of the 7 languages executes sorting algorithms natively. The backend coordinates inputs, maps them to command-line parameters, invokes the respective runtime or compiled binary, and parses the standardized JSON stdout stream.

### 1. C (`SortingC`)
- **Compilation**: Compiled using `gcc` with `-O3` optimizations and linked against the standard math library (`-lm`).
- **Characteristics**: Low-level memory access, near-zero runtime overhead. Provides the raw benchmark baseline.

### 2. C++ (`SortingCpp`)
- **Compilation**: Compiled using `g++` with `-O3` optimizations.
- **Characteristics**: Object-oriented implementation using modern standard library optimizations.

### 3. Go (`SortingGo`)
- **Compilation**: Statically compiled binary built with Go 1.21.
- **Characteristics**: Utilizes native Go high-resolution monotonic timer (`time.Now()`) to record performance logs.

### 4. Rust (`SortingRust`)
- **Compilation**: Cargo project built with maximum compiler optimization flags (`cargo build --release` with `opt-level = 3`).
- **Characteristics**: Safe memory access guarantees without a garbage collector. Extremely fast performance comparable to raw C.

### 5. C# (`SortingCSharp`)
- **Compilation**: Published as a self-contained, native single-file executable using the `.NET 8.0 SDK` targeted for `linux-x64` runtimes.
- **Characteristics**: Utilizes `System.Diagnostics.Stopwatch` for high-precision time calculations.

### 6. Java (`SortingJava.jar`)
- **Runtime**: Executed via a pre-compiled JAR archive running on the OpenJDK 17 headless runtime.
- **Characteristics**: Benchmark captures standard JVM execution patterns.

### 7. Python (`sorting.py`)
- **Runtime**: Executed script-style using the standard Python 3.10+ interpreter.
- **Characteristics**: Interpreted language baseline, benchmarking runtime overhead vs compiled languages.

---

## 📊 Supported Sorting Algorithms

The profiler supports **11 different algorithms** with varying average-case time complexities:
- **Comparison-Based ($O(n^2)$)**: Bubble Sort, Selection Sort, Insertion Sort
- **Comparison-Based ($O(n \log n)$)**: Merge Sort, Quick Sort, Heap Sort, Tim Sort, Intro Sort
- **Non-Comparison Based**: Counting Sort ($O(n+k)$), Radix Sort ($O(nk)$), Bucket Sort ($O(n+k)$)

---

## 🛠️ API Reference & JSON Schemas

### 1. Health Check
Checks if the Express server is up and running.
- **Endpoint**: `GET /api/health`
- **Response**:
  ```json
  {
    "status": "ok",
    "message": "Sorting Algorithm Backend is running"
  }
  ```

### 2. System Capabilities Discovery
Lists all supported programming languages and algorithms registered in the profiler.
- **Endpoint**: `GET /api/languages`
- **Response**:
  ```json
  {
    "languages": ["c", "cpp", "python", "go", "rust", "csharp", "java"],
    "algorithms": ["bubble", "selection", "insertion", "merge", "quick", "heap", "counting", "radix", "bucket", "tim", "intro"]
  }
  ```

### 3. Run Sort Execution
Executes a selected algorithm in a specific language using the provided input elements.
- **Endpoint**: `POST /api/sort/:language/:algorithm`
- **Request Body**:
  ```json
  {
    "elements": [23, 5, 89, 45, 12]
  }
  ```
- **Response**:
  ```json
  {
    "success": true,
    "language": "rust",
    "data": {
      "algorithm": "quick",
      "n": 5,
      "input": [23, 5, 89, 45, 12],
      "output": [5, 12, 23, 45, 89],
      "actual_time_ms": 0.00412,
      "comparisons": 7,
      "swaps": 3,
      "is_sorted": true
    }
  }
  ```

### 4. Cross-Language Benchmarking
Runs a single algorithm across **all 7 languages in parallel** using the exact same input array to compare performance metrics.
- **Endpoint**: `POST /api/compare/:algorithm`
- **Request Body**:
  ```json
  {
    "elements": [45, 12, 89, 3, 11]
  }
  ```
- **Response**:
  ```json
  {
    "success": true,
    "algorithm": "bubble",
    "results": {
      "c": {
        "algorithm": "bubble",
        "n": 5,
        "input": [45, 12, 89, 3, 11],
        "output": [3, 11, 12, 45, 89],
        "actual_time_ms": 0.00098,
        "comparisons": 10,
        "swaps": 7,
        "is_sorted": true
      },
      "python": {
        "algorithm": "bubble",
        "n": 5,
        "input": [45, 12, 89, 3, 11],
        "output": [3, 11, 12, 45, 89],
        "actual_time_ms": 0.0456,
        "comparisons": 10,
        "swaps": 7,
        "is_sorted": true
      }
      // Results for cpp, go, rust, csharp, and java included...
    }
  }
  ```

### 5. Cross-Algorithm Benchmarking
Runs **all 11 algorithms** in a selected language in parallel using the exact same input array to compare complexity metrics.
- **Endpoint**: `POST /api/algorithms/:language`
- **Request Body**:
  ```json
  {
    "elements": [45, 12, 89, 3, 11]
  }
  ```
- **Response**:
  ```json
  {
    "success": true,
    "language": "go",
    "results": {
      "bubble": { "actual_time_ms": 0.0012, "comparisons": 10, "swaps": 7, "is_sorted": true },
      "quick": { "actual_time_ms": 0.0034, "comparisons": 6, "swaps": 2, "is_sorted": true }
      // Results for insertion, selection, merge, heap, counting, radix, bucket, tim, intro included...
    }
  }
  ```

---

## 🐳 Docker Multi-Stage Build Pipeline

To build and compile code across GCC, Golang, Rust, and .NET, the `Dockerfile` implements a 5-stage build environment. This ensures that heavy build dependencies (compilers, build caches) are completely discarded, resulting in a minimal, lightweight production image containing only compiled binaries and the Node.js runner.

1. **`cpp-builder` stage**: Pulls `gcc:12`, copies C/C++ source code, compiles optimized C/C++ binaries.
2. **`go-builder` stage**: Pulls `golang:1.21`, copies Go project, builds optimized static Go binary.
3. **`rust-builder` stage**: Pulls `rust:1.75`, copies Rust project, runs optimized Cargo release compilation.
4. **`csharp-builder` stage**: Pulls `mcr.microsoft.com/dotnet/sdk:8.0`, compiles and publishes native standalone single-file binary.
5. **Runner stage (`node:20-slim`)**:
   - Installs runtime components `python3` and `openjdk-17-jre-headless`.
   - Copies production NPM dependencies (`npm ci --omit=dev`).
   - Copies `server.js` and static assets (`SortingJava.jar`, `sorting.py`).
   - Copies compiled compiled binaries (`SortingC`, `SortingCpp`, `SortingGo`, `SortingRust`, `SortingCSharp`) from the builders.

---

## 🗄️ Database Fallback Management
The backend utilizes Mongoose to interface with MongoDB.
- **Connection**: Attempts to connect using the environment variable `MONGODB_URI`.
- **Fault-Tolerance**: If `MONGODB_URI` is missing or fails, the backend prints a connection warning but does not crash. It disables write/read operations to the history statistics and remains fully operational in database-less mode.

---

## 🌐 Production Deployment
- **Provider**: Render (Web Service)
- **Deployment Strategy**: Docker Runtime (automatically triggers the multi-stage build using the repo's `Dockerfile`)
- **Environment Settings**: 
  - `NODE_ENV` = `production`
  - `PORT` = `3000` (exposed port)
- **Live URL**: https://ub-sort-lab-backend.onrender.com
