cmake -DCMAKE_BUILD_TYPE=Release -Brelease -H. && make -j4 -Crelease && ./release/RayTracer.app && open ./result.ppm
