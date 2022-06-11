# Ray Tracing

A simple ray tracing renderer with CPU

## Reference

- Peter Shirley. Ray Tracing in One Weekend https://raytracing.github.io/books/RayTracingInOneWeekend.html
  
## Dependencies

- GLM for linear algebra https://github.com/g-truc/glm
- Tinyobjloader for loading meshes https://github.com/tinyobjloader/tinyobjloader
- STB for image IO https://github.com/nothings/stb

## Compile

Clone the project

```shell
git clone --recursive https://github.com/LuniumLuk/raytracing.git
```

Compile with Makefile

```shell
make
```

or Compile with C++ compiler, since this project has only one .cpp file

```shell
g++ -std=c++11 -Isrc -o main -c src/main.cpp
```