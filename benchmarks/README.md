# BENCHMARKS

## Подготовьте необходимые директории

```bash
mkdir -p benchmarks/{data,results/{hollywood-2009,indochina-2004,mycielskian17,web-Google,web-Stanford}}
```

## Загрузите следующие матрицы в формате Matrix Market и поместите их в директорию "benchmarks/data/"

* [web-Google](https://suitesparse-collection-website.herokuapp.com/SNAP/web-Google)
* [web-Stanford](https://suitesparse-collection-website.herokuapp.com/SNAP/web-Stanford)
* [indochina-2004](https://suitesparse-collection-website.herokuapp.com/LAW/indochina-2004)
* [hollywood-2009](https://suitesparse-collection-website.herokuapp.com/LAW/hollywood-2009)
* [mycielskian17](https://suitesparse-collection-website.herokuapp.com/Mycielski/mycielskian17)
  
## Сборка и запуск

```bash
mkdir build && cd build
cmake -DCMAKE_BUILD_TYPE=Release -DBUILD_BENCHMARKS=ON ..
make -j$(nproc)
cd ..
./build/benchmarks/benchmark
```
