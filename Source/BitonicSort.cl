typedef int T;

__kernel void swap (T* lhs, T* rhs) {
    T temp = rhs;
    rhs = lhs;
    lhs = temp;
    
}

__kernel void bitonic_sort (__global T* data, const int stage, const int pass) {
    printf ("hi there");
    int threadId = get_global_id (0);
    int pairDistance = 1 << (stage - pass);
    int blockSize = 2 * pairDistance;

    int leftId = (threadId & (pairDistance - 1)) + (threadId >> (stage - pass)) * blockSize;
    int rightId = leftId + pairDistance;

    T left = data[leftId];
    T right = data[rightId];

    if (sameDirection) {
        swap (&left, &right);
    }

    data[leftId] = (left < right : left : right);
    data[rightId] = (left < right : right : left);
    
}