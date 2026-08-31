#ifndef FILTERS_H
#define FILTERS_H

#include <Arduino.h>

class EMAFilter {
private:
    float alpha;
    float currentVal;
    bool initialized;
public:
    EMAFilter(float alpha = 0.1) : alpha(alpha), currentVal(0), initialized(false) {}
    float filter(float newVal) {
        if (!initialized) {
            currentVal = newVal;
            initialized = true;
        } else {
            currentVal = (alpha * newVal) + ((1.0 - alpha) * currentVal);
        }
        return currentVal;
    }
    float get() const { return currentVal; }
};

class Median5Filter {
private:
    int buffer[5];
    uint8_t index;
    bool filled;
    
    void sort(int a[], int size) {
        for(int i=0; i<(size-1); i++) {
            for(int o=0; o<(size-(i+1)); o++) {
                if(a[o] > a[o+1]) {
                    int t = a[o];
                    a[o] = a[o+1];
                    a[o+1] = t;
                }
            }
        }
    }

public:
    Median5Filter() : index(0), filled(false) {
        for(int i=0; i<5; i++) buffer[i] = 0;
    }
    
    int filter(int newVal) {
        buffer[index] = newVal;
        index++;
        if (index >= 5) {
            index = 0;
            filled = true;
        }
        
        if (!filled) return newVal; // Pass through until filled
        
        int sorted[5];
        for(int i=0; i<5; i++) sorted[i] = buffer[i];
        sort(sorted, 5);
        return sorted[2]; // Return middle element
    }
};

#endif // FILTERS_H
