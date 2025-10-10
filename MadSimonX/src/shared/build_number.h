#pragma once

static const constexpr char *date = __DATE__;
static const constexpr char *mon[12] = { "Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec" };
static const constexpr char mond[12] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };

constexpr bool cxpr_strncmp(char const* str, char const* cmp, int size)
{
	while (size-- || *str || *cmp)
	{
		auto c1 = *str;
		auto c2 = *cmp;

		if (c1 != c2)
			return false;

		str++;
		cmp++;
	}

	return true;
}

constexpr int cxpr_atoi(const char* str)
{
	int result = 0;
	while (*str && (*str >= '0' && *str <= '9'))
		result = (*str++ - '0') + result * 10;
	return result;
}

constexpr int build_number()
{
	int m = 0;
	int d = 0;
	int y = 0;

	for (m = 0; m < 11; m++)
	{
		const char *dat = &date[0];
		const char *month = mon[m];

		if (cxpr_strncmp(dat, month, 3))
			break;
		d += mond[m];
	}

	d += cxpr_atoi(&date[4]) - 1;
	y = cxpr_atoi(&date[7]) - 1900;
	int b = d + (int)((y - 1) * 365.25);

	if (((y % 4) == 0) && m > 1) {
		b += 1;
	}

	b -= 43058; // 29.06.2018

	return b;
}

constexpr const int BUILD_NUM = build_number();