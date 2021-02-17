__kernel void functionAdd (__global const int* A, __global const int* B, __global int* C) {
    int id = get_global_id (0);
    C[id] = A[id] + B[id];
}