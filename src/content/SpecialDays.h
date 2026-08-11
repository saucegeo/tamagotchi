#ifndef SPECIAL_DAYS_H
#define SPECIAL_DAYS_H

struct SpecialDay {
    int month;
    int day;
    const char* name;
    const char* message;
};

int checkSpecialDay(int month, int day);

extern const SpecialDay specialDays[];
extern const int SPECIAL_DAY_COUNT;
extern int currentSpecialDay;

#endif
