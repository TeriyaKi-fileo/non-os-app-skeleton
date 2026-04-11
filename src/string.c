#include "string.h"

void num2str(int num, unsigned char* dist) {
    int index = 0;
    int work, p, i;
    work = num;
    dist[0] = 0;
    for(;;) {
        p = work - ((work / 10) * 10);
        work = work / 10;
        if (p > 0 || work > 0) {
            for (i = index; i >= 0; i--) {
                dist[i+1] = dist[i];
            }
            dist[0] = '0' + p;
            index ++;
        } else {
            break;
        }
    }
    if (num == 0) {
        dist[0] = '0';
        dist[1] = 0;
    }
}
