#ifndef HISTORY_H
#define HISTORY_H

#define HISTORY_MAX 256

typedef struct {
    double m8[HISTORY_MAX];
    double m9[HISTORY_MAX];
    double m10[HISTORY_MAX];
    int count;
} History;

void history_reset(History *h);
void history_append(History *h, double m8, double m9, double m10);

#endif
