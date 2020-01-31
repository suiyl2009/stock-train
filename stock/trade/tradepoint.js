function CTradeItem()
{
    this.m_nTrade = 0;
    this.m_nCount = 0;
    this.m_nDate = 0;
};

function CTradeData()
{
    this.m_strCode = '';
    this.m_nBeginDate = 0;
    this.m_nEndDate = 0;
    this.m_mapItem = {}; // map<int, CTradeItem> 
};

var ViewTab = ViewTab || {};
ViewTab.line = 0;
ViewTab.cond = 1;
ViewTab.quota = 2;
ViewTab.train = 3;

var ETradePointType = ETradePointType || {};
ETradePointType.eTradePointNone    = 0;
ETradePointType.eTradePointInsert  = 1;
ETradePointType.eTradePointDelete  = 2;
ETradePointType.eTradePointSearch  = 3;
ETradePointType.eTradePointStat    = 4;

function CTradePoint()
{
    this.m_nBeginDate = 0;
    this.m_nEndDate = 0;
    this.m_mapData = {};  // map<string, CTradeData> 
    this.m_eType = ETradePointType.eTradePointNone;  // ETradePointType

    /*
    1.point.insert
    请求字段列表
    字段名称	值类型	备注
    method	字符串point.insert
    userid	长整数	用户编号
    code	字符串	股票代码
    trade	整数	sale, buy-》1,2
    number	整数	第几买卖点
    date	整数	买卖点日期

    响应字段列表
    字段名称	值类型	备注
    method	同上	同上
    retcode	字符串	错误码，成功为0000
    result	{“code”: “xxx”, “begindate”: xxxx, “enddate”: xxx, 
    “point”: [{“trade”: xxx, “number”: xxx, “count”: xxx, “date”: xxx}, {}, …]}	响应字段值。
    */
    this.Insert = function (
		strSessionid,
		strCode,
		nTrade,
		nDate
		) {
        var bRet = true;
        var req =
        {
            "method": "point.insert",
            "session": strSessionid,
            "code": strCode,
            "trade": nTrade,
            "date": nDate
        };
        console.log('point.insert: ' + JSON.stringify(req));
        function remoteInsert(objJson) {
            if (objJson.retcode != '0000') {
                console.log("ERROR: retcode[" + objJson.retcode + "] != 0000");
                bRet = false;
                return bRet;
            }

            this.m_nBeginDate = objJson.result.begindata;
            this.m_nEndDate = objJson.result.enddate;
            this.m_mapData = {};
            this.m_mapData[objJson.result.code] = objJson.result.point;
            this.m_eType = ETradePointType.eTradePointInsert;  // ETradePointType

            return bRet;
        }
        gdata.send(req, remoteInsert);

        return bRet;
    };

    this.Delete = function (
		strSessionid,
		strCode,
		nTrade,
		nDate
		) {
        var bRet = true;
        var req =
        {
            "method": "point.delete",
            "session": strSessionid,
            "code": strCode,
            "trade": nTrade,
            "date": nDate
        };
        console.log('point.delete: ' + JSON.stringify(req));
        var result = {};
        function remoteDelete(objJson) {
            if (objJson.retcode != '0000') {
                console.log("ERROR: retcode[" + objJson.retcode + "] != 0000");
                bRet = false;
                return bRet;
            }

            result.m_nBeginDate = objJson.result.begindate;
            result.m_nEndDate = objJson.result.enddate;
            result.m_mapData = {};
            result.m_mapData[objJson.result.code] = objJson.result.point;
            result.m_eType = ETradePointType.eTradePointDelete;  // ETradePointType

            return bRet;
        }
        gdata.send(req, remoteDelete);

        this.m_nBeginDate = result.m_nBeginDate;
        this.m_nEndDate = result.m_nEndDate;
        this.m_mapData = result.m_mapData;
        this.m_eType = result.m_eType;

        return bRet;
    };

    /*
    请求字段列表
    字段名称	值类型	备注
    method	字符串point.search
    userid	长整数	用户编号
    code	字符串列表{}	股票代码列表
    cycle	字符串	day, week, month, year


    响应字段列表
    字段名称	值类型	备注
    method	同上	同上
    retcode	字符串	错误码，成功为0000
    result	{“begindate”: xxxx, “enddate”: xxx, 
    “trade”: [{“code”: “xxx”, “begindate”: xxxx, “enddate”: xxx, 
    “point”: [{“trade”: xxx, “number”: xxx, “count”: xxx, “date”: xxx}, 
    {}, …]}, {}, …], “date”: [xxx, xxx, …]}	响应字段值。
    */
    this.Search = function (
		strSessionid,
		vecCode,
		strCycle,
		nDate
		) {

        var bRet = true;
        var req =
        {
            "method": "point.search",
            "session": strSessionid,
            "code": vecCode,
            "cycle": strCycle,
            "date": nDate
        };
        console.log('point.search: ' + JSON.stringify(req));
        var result = {};
        function remoteSearch(objJson) {
            bRet = true;
            if (objJson.retcode != '0000') {
                console.log("ERROR: retcode[" + objJson.retcode + "] != 0000");
                bRet = false;
                return bRet;
            }
            console.log('point.search objJson: ');
            console.log(objJson);
            result.m_nBeginDate = objJson.result.begindate;
            result.m_nEndDate = objJson.result.enddate;
            result.m_mapData = {};
            for (var i = 0; i < objJson.result.trade.length; i++) {
                result.m_mapData[objJson.result.trade[i].code] = objJson.result.trade[i];
            }
            result.m_eType = ETradePointType.eTradePointSearch;  // ETradePointType
            //console.log(result.m_mapData);
            return bRet;
        }
        gdata.send(req, remoteSearch);
        //console.log(result.m_mapData);
        this.m_nBeginDate = result.m_nBeginDate;
        this.m_nEndDate = result.m_nEndDate;
        this.m_mapData = result.m_mapData;

        return bRet;
    };

    this.Statistics = function (
		strSessionid,
		vecCode,
		strCycle
		) {

        var bRet = true;
        var req =
        {
            "method": "point.statistics",
            "session": strSessionid,
            "code": vecCode,
            "cycle": strCycle
        };
        console.log('point.statistics: ' + JSON.stringify(req));
        function remoteInsert(objJson) {
            if (objJson.retcode != '0000') {
                console.log("ERROR: retcode[" + objJson.retcode + "] != 0000");
                bRet = false;
                return bRet;
            }

            this.m_nBeginDate = objJson.result.begindata;
            this.m_nEndDate = objJson.result.enddate;
            this.m_mapData.clear();
            for (var i=0; i<objJson.result.trade.length; i++){
                this.m_mapData[objJson.result.trade[i].code] = objJson.result.trade[i];
            }
            this.m_eType = ETradePointType.eTradePointStat;  // ETradePointType

            return bRet;
        }
        gdata.send(req, remoteInsert);

        return bRet;
    };
};