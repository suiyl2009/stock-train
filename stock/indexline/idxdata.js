var idxdata = idxdata || {};
//gdata.m_strUrl = "http://192.168.72.128/Stock";
////begin: m_data in localStorage/////
idxdata.m_data = idxdata.m_data || {};
idxdata.m_data.codes = [];
idxdata.m_data.idx = 0;

idxdata.getAllCodes = function () {
    //{"date": 20190230, "method": "index.list"}
    //{"method": "index.list", "retcode": "0000", "result": [
    //{"Code": "000010", "Name": "上证180", "Exchange": "sh", "Close": "817361", "Date": "20190228"}, 
    //{"Code": "000016", "Name": "上证50", "Exchange": "sh", "Close": "274397", "Date": "20190228"}, 
    //{"Code": "000300", "Name": "沪深300", "Exchange": "sh", "Close": "366937", "Date": "20190228"}, 
    //{"Code": "000903", "Name": "中证100", "Exchange": "sh", "Close": "386307", "Date": "20190228"}, 
    //{"Code": "000905", "Name": "中证500", "Exchange": "sh", "Close": "502529", "Date": "20190228"}, 
    //{"Code": "399001", "Name": "深证成指", "Exchange": "sz", "Close": "903193", "Date": "20190228"}, 
    //{"Code": "399004", "Name": "深证100R", "Exchange": "sz", "Close": "483435", "Date": "20190228"}, 
    //{"Code": "399005", "Name": "中小板指", "Exchange": "sz", "Close": "589369", "Date": "20190228"}, 
    //{"Code": "399006", "Name": "创业板指", "Exchange": "sz", "Close": "153568", "Date": "20190228"}, 
    //{"Code": "399106", "Name": "深证综指", "Exchange": "sz", "Close": "154633", "Date": "20190228"}, 
    //{"Code": "999999", "Name": "上证指数", "Exchange": "sh", "Close": "294095", "Date": "20190228"}
    //]}
    ///////////////begin: list.all/////////////////////////
    idxdata.get();
    var global =conf.get("global");
    var req = { "method": "index.list", "item": ["Code"], "date": global.date };
    function remoteListall(jsonRes) {
        console.log(jsonRes);
        if ('0000' != jsonRes.retcode) {
            console.log('ERROR: { "method": "list.all" } retcode error: ' + jsonRes.retcode);
            return false;
        }

        idxdata.m_data.codes = [];
        idxdata.m_data.idx = 0;
        for (var i = 0; i < jsonRes.result.length; i++) {
            idxdata.m_data.codes.push(jsonRes.result[i].Code);
        }
        console.log('remoteListall: ' + idxdata.m_data.codes);
        idxdata.set();
        idxdata.get();
        return true;
    }
    idxdata.send(req, remoteListall);
    ///////////////end: list.all/////////////////////////
    idxdata.set();
    idxdata.get();

    return true;
}
////end: m_data in localStorage/////

idxdata.set = function () {
    if (window.localStorage) {
        var str_jsonData = JSON.stringify(idxdata.m_data);
        localStorage.setItem('idxdata', str_jsonData); // 存储字符串数据到本地
    }
}

idxdata.get = function () {
    if (window.localStorage) {
        var localStorage = window.localStorage;
        if (null == localStorage.getItem('idxdata')) {
            var str_jsonData = JSON.stringify(idxdata.m_data);
            localStorage.setItem('idxdata', str_jsonData);
        }
        var getLocalData = localStorage.getItem('idxdata');
        var data = JSON.parse(getLocalData);
        idxdata.m_data.codes = data.codes;
        idxdata.m_data.idx = data.idx;
        console.log('idxdata get: ');
        console.log(idxdata.m_data);
        return data;
    }
    else {
        return idxdata.m_data;
    }
}

idxdata.init = function () {
    idxdata.set();
    if (0 >= idxdata.m_data.codes.length) {
        idxdata.getAllCodes();
        console.log("Call idxdata.getAllCodes()");
    }
    console.log("Call idxdata.init");
    idxdata.get();

    return true;
}

idxdata.send = function (jsonreq, func) {
    httpPost(gdata.m_strUrl, jsonreq, func);
}

idxdata.init();
