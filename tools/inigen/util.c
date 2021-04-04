#include <stdlib.h>
#include <string.h>
#include "util.h"

int msort_r(void * data, size_t count, size_t size, cmpfun cmp, void * buffer) {
    /*
     * Out-of-place mergesort (stable sort)
     * Returns 1 on success, 0 on failure
     */
    void * leftPtr;
    void * rightPtr;
    void * leftEnd;
    void * rightEnd;
    int i;

    switch (count) {
        case 0:
            // Should never be here
            return 0;

        case 1:
            // Nothing to do here
            break;

        case 2:
            // Swap the two entries if the right one compares higher.
            if (cmp(data, data + size) > 0) {
                memcpy(buffer, data, size);
                memcpy(data, data + size, size);
                memcpy(data + size, buffer, size);
            }
            break;
        default:
            // Merge sort out-of-place.
            leftPtr = data;
            leftEnd = rightPtr = data + count / 2 * size;
            rightEnd = data + count * size;

            // Sort the left half
            if (!msort_r(leftPtr, count / 2, size, cmp, buffer))
                return 0;

            // Sort the right half
            if (!msort_r(rightPtr, count / 2 + (count & 1), size, cmp, buffer))
                return 0;

            // Merge the sorted halves out of place
            i = 0;
            do {
                if (cmp(leftPtr, rightPtr) <= 0) {
                    memcpy(buffer + i * size, leftPtr, size);
                    leftPtr += size;
                } else {
                    memcpy(buffer + i * size, rightPtr, size);
                    rightPtr += size;
                }

            } while (++i < count && leftPtr < leftEnd && rightPtr < rightEnd);

            // Copy the remainder
            if (i < count) {
                if (leftPtr < leftEnd) {
                    memcpy(buffer + i * size, leftPtr, leftEnd - leftPtr);
                }
                else {
                    memcpy(buffer + i * size, rightPtr, rightEnd - rightPtr);
                }
            }

            // Copy the merged data back
            memcpy(data, buffer, count * size);
            break;
    }

    return 1;
}

int msort(void * data, size_t count, size_t size, cmpfun cmp) {
    void * buffer = malloc(count * size);
    if (buffer == NULL) return 0;
    int result = msort_r(data, count, size, cmp, buffer);
    free(buffer);
    return result;
}
