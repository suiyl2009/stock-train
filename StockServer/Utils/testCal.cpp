#include "Calendar.h"

int main()
{

    CCalendar objCal;
	int iTestDate = 17821231;

	int iInterval = objCal.IntervalDays(iTestDate);
	//iInterval = iInterval - 2*iInterval;
    cout<<"iInterval: "<<iInterval<<endl;
    
	//objCal.AddDay(iInterval);
	objCal.SubDay(iInterval);

	cout<<"iInterval: "<<iInterval<<", iTestDate: "<<iTestDate<<", objCal: "<<objCal.GetDate()<<endl;

	CCalendar objCal2;
	int iTestDate2 = 20681231;

	int iInterval2 = objCal2.IntervalDays(iTestDate2);
	iInterval2 = iInterval2 - 2*iInterval2;
    cout<<"iInterval2: "<<iInterval2<<endl;
    
	objCal2.AddDay(iInterval2);

	cout<<"iInterval2: "<<iInterval2<<", iTestDate2: "<<iTestDate2<<", objCal2: "<<objCal2.GetDate()<<endl;

    CCalendar objCal3;
    int iTestDate3 = 20151230;
    int iInterval3 = objCal3.IntervalDays(iTestDate3);
    iInterval3 = iInterval3 - 2*iInterval3;
    cout<<"iInterval3: "<<iInterval3<<endl;
    objCal3.AddDay(iInterval3);
    cout<<"objCal3: "<<objCal3.GetDate()<<", weekday: "<<objCal3.m_iWeekDay<<endl;
    objCal3.AddDay(1);
    cout<<"objCal3: "<<objCal3.GetDate()<<", weekday: "<<objCal3.m_iWeekDay<<endl;
    objCal3.AddDay(1);
    cout<<"objCal3: "<<objCal3.GetDate()<<", weekday: "<<objCal3.m_iWeekDay<<endl;
    
    return 0;
}
