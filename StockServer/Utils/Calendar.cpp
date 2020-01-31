#include "Calendar.h"

// 闰年是如何计算的：用年数除以4，整百年除以400, 闰年2月29天，不闰2月28天
CCalendar::CCalendar()
{
    m_iYear       = 0;
    m_iMonth      = 0;
    m_iDay        = 0;
    m_iLeapDay    = 0;
	m_iWeekDay    = 0;

    m_iYearDays          = 365;
    m_arrMonthDays[0]    = 0;
    m_arrMonthDays[1]    = 31;
    m_arrMonthDays[2]    = 28;
    m_arrMonthDays[3]    = 31;
    m_arrMonthDays[4]    = 30;
    m_arrMonthDays[5]    = 31;
    m_arrMonthDays[6]    = 30;
    m_arrMonthDays[7]    = 31;
    m_arrMonthDays[8]    = 31;
    m_arrMonthDays[9]    = 30;
    m_arrMonthDays[10]   = 31;
    m_arrMonthDays[11]   = 30;
    m_arrMonthDays[12]   = 31;

	InitDate(19840127, 5);
	//InitDate(19840101, 0);
}

CCalendar::~CCalendar()
{
}

int CCalendar::InitDate(int iDate, int iWeekDay)
{
    if (false == CheckDate(iDate))
        return 1;

    if (true == CheckLeap(iDate))
    {
        m_iLeapDay = 1;
        m_arrMonthDays[2] = 29;
        m_iYearDays = 366;
    }
    else
    {
        m_iLeapDay = 0;
        m_arrMonthDays[2] = 28;
        m_iYearDays = 365;
    }

    m_iYear   = iDate/10000;
    m_iMonth  = iDate%10000;
    m_iMonth  = m_iMonth/100;
    m_iDay    = iDate%100;
	
	m_iWeekDay  = iWeekDay;
    
}

int CCalendar::AddDay(int iDay)
{
    if (iDay <= 0)
    {
        return 0;
    }
    
    m_iDay += iDay;

    for (; m_iMonth<=12; m_iMonth++)
    {
        if (m_arrMonthDays[m_iMonth] >= m_iDay)
        {
            //break;
            m_iWeekDay = (m_iWeekDay+iDay)%7;
            return (m_iYear*10000 + m_iMonth*100 + m_iDay);
        }
        m_iDay -= m_arrMonthDays[m_iMonth];
    }

    m_iYear++;
    if (true == CheckLeap(m_iYear*10000 + 101))
    {
        m_iLeapDay = 1;
        m_arrMonthDays[2] = 29;
        m_iYearDays = 366;
    }
    else
    {
        m_iLeapDay = 0;
        m_arrMonthDays[2] = 28;
        m_iYearDays = 365;
    }

    
    while (m_iDay > m_iYearDays)
    {
        m_iDay = m_iDay - m_iYearDays;
        m_iYear++;
        if (true == CheckLeap(m_iYear*10000 + 101))
        {
            m_iLeapDay = 1;
            m_arrMonthDays[2] = 29;
            m_iYearDays = 366;
        }
        else
        {
            m_iLeapDay = 0;
            m_arrMonthDays[2] = 28;
            m_iYearDays = 365;
        }

    }

    for (m_iMonth=1; m_iMonth<=12; m_iMonth++)
    {
        if (m_iDay <= m_arrMonthDays[m_iMonth])
        {
            break;
        }
        m_iDay = m_iDay - m_arrMonthDays[m_iMonth];
    }

	m_iWeekDay = (m_iWeekDay+iDay)%7;

    return (m_iYear*10000 + m_iMonth*100 + m_iDay);
}

int CCalendar::SubDay(int iDay)
{
    int iPassDays = 0;

    if (iDay <= 0)
    {
        return 0;
    }
    
    /////////begin: 本日期已过天数//////////////
    for (int i=1; i<m_iMonth; i++)
    {
        iPassDays += m_arrMonthDays[i];
    }
    iPassDays += m_iDay;
	cout<<"iPassDays: "<<iPassDays<<endl;
    ////////end: 本日期已过天数////////////////

    if (iPassDays > iDay)  // 没有跨年
    {
        m_iDay = iPassDays - iDay;
		cout<<"m_iDay: "<<m_iDay<<endl;
        for (m_iMonth=1; m_iMonth<=12; m_iMonth++)
        {
            if (m_iDay <= m_arrMonthDays[m_iMonth])
            {
                break;
            }
            m_iDay -= m_arrMonthDays[m_iMonth];
        }
        return (m_iYear*10000 + m_iMonth*100 + m_iDay);
    }
    iDay -= iPassDays;

    do  // 处理跨年
    {
        m_iYear--;
        if (true == CheckLeap(m_iYear*10000 + 101))
        {
            m_iLeapDay = 1;
            m_arrMonthDays[2] = 29;
            m_iYearDays = 366;
        }
        else
        {
            m_iLeapDay = 0;
            m_arrMonthDays[2] = 28;
            m_iYearDays = 365;
        }

        if (iDay < m_iYearDays)
        {
            break;
        }
        iDay -= m_iYearDays;
    } while (true);

    // 确定年内日期
    iDay = m_iYearDays - iDay;
    m_iDay = iDay;
    for (m_iMonth=1; m_iMonth<=12; m_iMonth++)
    {
        if (m_iDay <= m_arrMonthDays[m_iMonth])
        {
            break;
        }
        m_iDay -= m_arrMonthDays[m_iMonth];
    }

	m_iWeekDay = (7 + m_iWeekDay - iDay%7)%7;
    
    return (m_iYear*10000 + m_iMonth*100 + m_iDay);
}

int CCalendar::IntervalDays(int iDate)
{
    if (false == CheckDate(iDate))
    {
        return 0;
    }
    
    int iYear = 0;
    int iMonth = 0;
    int iDay = 0;
    int iLeapDay = 0;
    int iInterval1 = 0;
    int iInterval2 = 0;
    int iInterval3 = 0;
    bool bConvert = false;
	int iWeekDay = 0;
    int iMyDate = GetDate();
    if (iDate == iMyDate)
        return 0;

    if (iDate < iMyDate)
    {
        iWeekDay = m_iWeekDay;
        InitDate(iDate, 0);
        iDate = iMyDate;
        iMyDate = GetDate();
        bConvert = true;
    }

    iYear = iDate/10000;
    iMonth = iDate/100;
    iMonth = iMonth%100;
    iDay = iDate%100;
    if (true == CheckLeap(iDate))
        iLeapDay = 1;

    if (iYear > m_iYear) //加闰
    {
        iInterval1 = (iYear-m_iYear-1)*365;
        iInterval1 += (iYear-m_iYear-1)/4;
        int iMod = (iYear-m_iYear-1)%4;
        for (int i=0; i<iMod; i++)
        {
            if (0 == (m_iYear+1+i)%4)
                iInterval1++;
        }
    }
    cout<<"iInterval1: "<<iInterval1<<endl;
    for (int i = (m_iYear+1)/100+1; i<=(iYear-1)/100; i++) // 废闰
    {
        if (0 != i%4)
            iInterval1--;
    }
	cout<<"iInterval1: "<<iInterval1<<endl;

    /////////begin: iDate 已过天数//////////////
    for (int i=1; i<iMonth; i++)
    {
        iInterval2 += m_arrMonthDays[i];
    }
    iInterval2 += iDay;
    if ((iMonth > 2) && (iLeapDay != m_iLeapDay))
    {
        if (iLeapDay == 1)
            iInterval2++;
        else
            iInterval2--;
    }
	cout<<"iInterval2: "<<iInterval2<<endl;
    /////////end: iDate 已过天数///////////////

    /////////begin: iMyDate 已过天数//////////////
    for (int i=1; i<m_iMonth; i++)
    {
        iInterval3 += m_arrMonthDays[i];
    }
    iInterval3 += m_iDay;
    ////////end: iMyDate 已过天数////////////////

    if (m_iYear == iYear)
	{
	    iInterval2 = iInterval2 - iInterval3;
		iInterval3 = 0;
	}
	else
	{
		iInterval3 = m_iYearDays - iInterval3;  // iMyDate 剩余天数
	}
    cout<<"iInterval3: "<<iInterval3<<endl;

    if (true == bConvert)
    {
        InitDate(iDate, iWeekDay);
        return (iInterval1 + iInterval2 + iInterval3);
    }

    return ((iInterval1 + iInterval2 + iInterval3) - 2*(iInterval1 + iInterval2 + iInterval3));
}

bool CCalendar::CheckDate(int iDate)
{
    int iYear = iDate/10000;
    int iMonth = iDate%10000;
    iMonth = iMonth/100;
    int iDay = iDate%100;

    if ((1000 > iYear) || (9999 < iYear))
        return false;

    int iLeapDay = 0;
    if (true == CheckLeap(iDate))
        iLeapDay = 1;

    if ((0>= iMonth) || (13 <= iMonth))
        return false;

    if (0 >= iDay)
        return false;

    if (2 == iMonth)
    {
        if ((0 == iLeapDay) && (iDay > 28))
            return false;
        if ((1 == iLeapDay) && (iDay > 29))
            return false;
    }
    else
    {
        if (iDay > m_arrMonthDays[iMonth])
            return false;
    }

    return true;
}

bool CCalendar::CheckLeap(int iDate)
{
    int iYear = iDate/10000;

    if ((0 != iYear%100) && (0 == iYear%4))
    {
        return true;
    }

    if ((0 == iYear%100) && (0 == iYear%400))
    {
        return true;
    }

    return false;
}

int CCalendar::GetDate()
{
    return (m_iYear*10000 + m_iMonth*100 + m_iDay);
}

