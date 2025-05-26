# CC_Generator

Generate a bunch of random credit card numbers quickly and easily in your browser using WebAssembly.

## Description

CC_Generator is a lightweight tool that generates random credit card numbers fast. It uses a C++ backend compiled to WebAssembly via Emscripten for performance, and a GUI built with Dear ImGui for ease of use.

## Installation

1. **Prerequisites:**

   - Install [Emscripten SDK](https://emscripten.org/docs/getting_started/downloads.html).
   - Make sure to activate Emscripten environment by running:
     ```bash
     source /path/to/emsdk/emsdk_env.sh
     ```

2. **Clone this repository:**
   ```bash
   git clone https://github.com/ItaiShek/CC_Generator.git
   cd CC_Generator
   ```

3. **Prepare the database:**
    ```bash
    cp /path/to/Cards.db build/
    ```

4. **Building from source**
    ```bash
    mkdir build
    cd build
    emcmake cmake ..
    cmake --build .
    ```

## Running
Open `index.html` in a web browser that supports WebAssembly and WebGL2. You can serve the directory using a simple HTTP server:

```bash
cd build/CC_Generator
python3 -m http.server 8080
```
Then navigate to http://localhost:8080/index.html

## Performance Note

While the WebAssembly (WASM) version runs directly in the browser and is very convenient, it is significantly slower than the native desktop application version. This is due to the inherent overhead of running in a browser environment and limitations of the WebAssembly runtime compared to a compiled native binary.

For heavy usage or maximum speed, I recommend using the desktop version.