#include "SpecialDays.h"

const SpecialDay specialDays[] = {
    {2, 14, "Valentine's", "Happy Valentine's Day! I love you! ❤"},
    {2, 20, "Birthday", "Happy Birthday, my love! You're amazing!"},
    {6, 7, "Birthday", "It's my birthday! Thanks for being here!"}
};

const int SPECIAL_DAY_COUNT = 3;
int currentSpecialDay = -1;

int checkSpecialDay(int month, int day) {
    for (int i = 0; i < SPECIAL_DAY_COUNT; i++) {
        if (specialDays[i].month == month && specialDays[i].day == day) {
            return i;
        }
    }
    return -1;
}
