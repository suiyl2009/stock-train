#ifndef __CALENDAR_H__
#define __CALENDAR_H__

#include <iostream>

using namespace std;

// 闰年是如何计算的：用年数除以4，整百年除以400, 闰年2月29天，不闰2月28天
class CCalendar
{
public:
    CCalendar();
    ~CCalendar();
    
    int InitDate(int iDate, int iWeekDay);
    int AddDay(int iDay);
    int SubDay(int iDay);
    int IntervalDays(int iDate);
    bool CheckDate(int iDate);
    bool CheckLeap(int iDate);
    int GetDate();

    // 当前日期
    int m_iYear;
    int m_iMonth;
    int m_iDay;
    int m_iLeapDay;
	int m_iWeekDay;

    // 常数信息，都是非闰时的天数
    int m_iYearDays;
    int m_arrMonthDays[13]; // 0下标备用
};

#endif
