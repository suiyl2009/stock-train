#include "Index.h"

int CIndex::list(
    json_t *jReq,
	json_t *&jRes,
	string &strRetCode
	);

int CIndex::single(
    json_t *jReq,
	json_t *&jRes,
	string &strRetCode
	);

int CIndex::batch(
    json_t *jReq,
	json_t *&jRes,
	string &strRetCode
	);

int CIndex::kline(
    json_t *jReq,
	json_t *&jRes,
	string &strRetCode
	);

