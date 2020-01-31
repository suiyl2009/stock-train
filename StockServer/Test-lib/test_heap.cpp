//////////////////begin: leap sort/////////////////////////////////////
#include <iostream>
#include <vector>

using namespace std;

class Data
{
public:
  int m_iKey;
  int compare(Data objData)
  {
    if (m_iKey > objData.m_iKey)
      return 1;
    else if (m_iKey < objData.m_iKey)
      return -1;
    else
      return 0;
  }
}; 

//?????
template <class T>
void MaxHeapFixDown(vector<T> &vecData, int nLow, int nHigh){
    int j = 2*nLow+1;
    T top = vecData[nLow];
    while(j<nHigh){
        if(j+1<nHigh&&vecData[j].compare(vecData[j+1])<0)
            ++j;
        if(top.compare(vecData[j])>=0)
            break;
        else{
            vecData[nLow]=vecData[j];
            ;
            nLow=j;
            j=2*nLow+1;
        }
    }
    vecData[nLow]=top;
}

//???,????
template <class T>
void HeapSortAsc(vector<T> &vecData, int nTopCnt){
    for(int i= vecData.size()/2-1;i>=0;i--)
        MaxHeapFixDown(vecData, i, vecData.size());
    for(int i=vecData.size()-1;(i>=1)&&(vecData.size()-i<=nTopCnt);i--){
        swap(vecData[i],vecData[0]);
        MaxHeapFixDown(vecData,0,i);
    }
}
//////////////////end: leap sort////////////////////////////////////


int main()
{
  Data objData;
  vector<Data> vecData;

  objData.m_iKey = 5;
  vecData.push_back(objData);
  objData.m_iKey = 3;
  vecData.push_back(objData);
  objData.m_iKey = 6;
  vecData.push_back(objData);
  objData.m_iKey = 10;
  vecData.push_back(objData);
  objData.m_iKey = 2;
  vecData.push_back(objData);
  objData.m_iKey = 9;
  vecData.push_back(objData);

  int iHigh = vecData.size();
  HeapSortAsc(vecData, iHigh);

  for (int i=0; i<vecData.size(); i++)
  {
    cout<<" "<<vecData[i].m_iKey;
  }

  return 0;
}
