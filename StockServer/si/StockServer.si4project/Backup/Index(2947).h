#ifndef __INDEX_H__
#define __INDEX_H__

class CIndex{
public:
    int list(
        json_t *jReq,
		json_t *&jRes,
		string &strRetCode
		);
    
    int single(
        json_t *jReq,
		json_t *&jRes,
		string &strRetCode
		);
    
    int batch(
        json_t *jReq,
		json_t *&jRes,
		string &strRetCode
		);
    
    int kline(
        json_t *jReq,
		json_t *&jRes,
		string &strRetCode
		);
};

#endif
