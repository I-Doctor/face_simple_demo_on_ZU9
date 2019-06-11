# face_simple_demo_on_ZU9
A Simple Face Recognition Application on ZU9 FPGA with DPU

## requirements
gcc
g++
opencv

## run
run 'make' to compile three program: process, run, judge
run 'process' to generate two input bin file in data/ from two pictures in picture/
run 'run' to copy weight and instruction in weights/ onto the DPU, then copy data and set DPU to run, so that we can get two output bin file in data/ .
run 'judge' to read two output bin file in data/ and calculate the cosine distance to judge whether the two pictures belong to the same person.

## customize
change picture names in process.cpp to process different pictures.
