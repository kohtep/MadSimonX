#pragma once

static const constexpr char *date = __DATE__;

static const constexpr char *mon[12] =
{
	"Jan", "Feb", "Mar", "Apr", "May", "Jun",
	"Jul", "Aug", "Sep", "Oct", "Nov", "Dec"
};

static constexpr int mdays[12] =
{
	31,28,31,30,31,30,31,31,30,31,30,31
};

constexpr bool cxpr_strncmp(char const *str, char const *cmp, int size)
{
    while (size-- > 0)
    {
        if (*str != *cmp)
            return false;

        if (*str == '\0')
            break;

        ++str;
        ++cmp;
    }

    return true;
}

constexpr int cxpr_atoi(const char *str)
{
    int result = 0;

    while (*str && (*str < '0' || *str > '9'))
        ++str;

    while (*str && (*str >= '0' && *str <= '9'))
        result = (*str++ - '0') + result * 10;

    return result;
}

constexpr bool is_leap(int y)
{
    return (y % 4 == 0) && (y % 100 != 0 || y % 400 == 0);
}

constexpr int days_since_1900(int y, int month0, int day)
{
    int days = 0;

    for (int yy = 1900; yy < y; ++yy)
        days += 365 + (is_leap(yy) ? 1 : 0);

    for (int mm = 0; mm < month0; ++mm)
        days += mdays[mm];

    if (month0 > 1 && is_leap(y))
        days += 1;

    days += day - 1;
    return days;
}

constexpr int build_number()
{
    int m = 0;

    for (; m < 12; ++m)
    {
        if (cxpr_strncmp(date, mon[m], 3))
            break;
    }

    const int d = cxpr_atoi(&date[4]);
    const int y = cxpr_atoi(&date[7]);

    const int now = days_since_1900(y, m, d);
    constexpr int base = days_since_1900(2018, 05, 29); // Project creation date

    return now - base;
}