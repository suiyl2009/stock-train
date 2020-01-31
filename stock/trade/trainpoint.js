function CTrainTrade(){
    this.m_strType = "";
    this.m_strCode = "";
    this.m_iNum = 0;
    this.m_iPrice = 0;
    this.m_iTax = 0;
    this.m_iDate = 0;
};

function CTrainHold(){
    this.m_strCode = "";
    this.m_iNum = 0;
    this.m_iPrice = 0;
};

function CTrainInfo(){
    this.sessionid="";
    this.m_iBeginDate = 0;
    this.m_iEndDate = 0;
    this.m_iBeginFund = 0;
    this.m_iEndFund = 0;
    this.m_iTaxRate = 0;
    this.m_vecTrade = [];
    this.m_vecHold = [];
    this.m_currIdx = -1;
    this.m_objClickRect = null;

    this.setCurrIdxByCode = function (code) {
        this.m_currIdx = -1;
        for (var i = 0; i < this.m_vecHold.length; i++) {
            if (code == this.m_vecHold[i].code) {
                this.m_currIdx = i;
                break;
            }
        }
        return this.m_currIdx;
    }

    this.getBuyNum = function(code, price){
        if ((0 <= this.m_currIdx) && (code != this.m_vecHold[this.m_currIdx].code)) {
            return -1;
        }
        var num = Math.floor(this.m_iEndFund / price);
        num = Math.floor(num / 100) * 100;
        return num;
    }

    /*this.buy = function(code, price, date, num){
        if ((0 <= this.m_currIdx) && (code != this.m_vecHold[this.m_currIdx].m_strCode)) {
            return -1;
        }
        var maxNum = Math.floor(this.m_iEndFund / price / 100);
        maxNum = Math.floor(maxNum / 100) * 100;
        if (num > maxNum) {
            return 0;
        }
        this.m_iEndFund -= (price * num);
        if (0 > this.m_currIdx) {
            var objHold = new CTrainHold;
            objHold.m_strCode = code;
            objHold.m_iPrice = price;
            objHold.m_iNum = num;
            this.m_vecHold.push(objHold);
        }
        else {
            this.m_vecHold[this.m_currIdx].m_iPrice = (this.m_vecHold[this.m_currIdx].m_iPrice * this.m_vecHold[this.m_currIdx].m_iNum
                + price * num) / (this.m_vecHold[this.m_currIdx].m_iNum + num);
            this.m_vecHold[this.m_currIdx].m_iPrice = Math.round(this.m_vecHold[this.m_currIdx].m_iPrice);
            this.m_vecHold[this.m_currIdx].m_iNum += num;
        }
        this.m_iEndDate = date;

        // Ìí¼Ótrade

        return num;
    }*/

    this.buy = function (sessionid, code, price, date, num) {
        if ((0 <= this.m_currIdx) && (code != this.m_vecHold[this.m_currIdx].code)) {
            return -1;
        }
        var maxNum = Math.floor(this.m_iEndFund / price);
        maxNum = Math.floor(maxNum / 100) * 100;
        if (num > maxNum) {
            return 0;
        }

        var req =
           {
               "method": "ktrain.buy",
               "session": sessionid,
               "code": code,
               "num": num,
               "price": price,
               "date": date
           };
        console.log('ktrain.search: ' + JSON.stringify(req));
        var retcode = "0000";
        var beginDate = 0;
        var endDate = 0;
        var beginFund = 0;
        var endFund = 0;
        var taxRate = 0;
        var vecTrade = [];
        var vecHold = [];
        //var currIdx = -1;
        function remoteTrainSearch(objJson) {
            if (objJson.retcode != '0000') {
                console.log("ERROR: retcode[" + objJson.retcode + "] != 0000");
                retcode = objJson.retcode;
                return false;
            }
            retcode = objJson.retcode;
            console.log(objJson);
            //username = objJson.result.name;
            //session = objJson.result.session;
            beginDate = objJson.result.begindate;
            beginFund = objJson.result.beginfund;
            endDate = objJson.result.enddate;
            endFund = objJson.result.endfund;
            taxRate = objJson.result.taxrate;
            vecTrade = objJson.result.operate;
            vecHold = objJson.result.hold;

            return true;
        }
        gdata.send(req, remoteTrainSearch);

        if ('0000' != retcode) {
            console.log('train.search error retcode: ' + retcode);
            return -1;
        }

        this.sessionid = sessionid;
        this.m_iBeginDate = beginDate;
        this.m_iEndDate = endDate;
        this.m_iBeginFund = beginFund;
        this.m_iEndFund = endFund;
        this.m_iTaxRate = taxRate;
        this.m_vecTrade = vecTrade;
        this.m_vecHold = vecHold;
        this.m_currIdx = -1;

        return num;
    }

    this.getSaleNum = function(code){
        if ((0 > this.m_currIdx) || (code != this.m_vecHold[this.m_currIdx].code)) {
            console.log("this.m_currIdx: " + this.m_currIdx + ", code: " + code + ", this.m_vecHold.length: "
                + this.m_vecHold.length + ", hold code: " + this.m_vecHold[this.m_currIdx].code);
            return -1;
        }
        var num = this.m_vecHold[this.m_currIdx].num;
        num = Math.floor(num / 100) * 100;
        return num;
    }

    /*this.sale = function(code, price, date, num){
        if (code != this.m_vecHold[this.m_currIdx].m_strCode) {
            return -1;
        }
        if (this.m_vecHold[this.m_currIdx].m_iNum < num) {
            return 0;
        }
        this.m_iEndFund += price * num;
        if (this.m_vecHold[this.m_currIdx].m_iNum == num) {
            this.m_vecHold.splice(this.m_currIdx, 1);
        }
        else {
            this.m_vecHold[this.m_currIdx].m_iPrice = (this.m_vecHold[this.m_currIdx].m_iPrice * this.m_vecHold[this.m_currIdx].m_iNum
                - price * num) / (this.m_vecHold[this.m_currIdx].m_iNum - num);
            this.m_vecHold[this.m_currIdx].m_iPrice = Math.round(this.m_vecHold[this.m_currIdx].m_iPrice);
            this.m_vecHold[this.m_currIdx].m_iNum -= num;
        }
        this.m_iEndDate = date;

        // Ìí¼Ótrade

        return num;
    }*/

    this.sale = function (sessionid, code, price, date, num) {
        if (code != this.m_vecHold[this.m_currIdx].code) {
            return -1;
        }
        if (this.m_vecHold[this.m_currIdx].num < num) {
            return 0;
        }

        var req =
           {
               "method": "ktrain.sale",
               "session": sessionid,
               "code": code,
               "num": num,
               "price": price,
               "date": date
           };
        console.log('ktrain.search: ' + JSON.stringify(req));
        var retcode = "0000";
        var beginDate = 0;
        var endDate = 0;
        var beginFund = 0;
        var endFund = 0;
        var taxRate = 0;
        var vecTrade = [];
        var vecHold = [];
        //var currIdx = -1;
        function remoteTrainSearch(objJson) {
            if (objJson.retcode != '0000') {
                console.log("ERROR: retcode[" + objJson.retcode + "] != 0000");
                retcode = objJson.retcode;
                return false;
            }
            retcode = objJson.retcode;
            console.log(objJson);
            //username = objJson.result.name;
            //session = objJson.result.session;
            beginDate = objJson.result.begindate;
            beginFund = objJson.result.beginfund;
            endDate = objJson.result.enddate;
            endFund = objJson.result.endfund;
            taxRate = objJson.result.taxrate;
            vecTrade = objJson.result.operate;
            vecHold = objJson.result.hold;

            return true;
        }
        gdata.send(req, remoteTrainSearch);

        if ('0000' != retcode) {
            console.log('train.search error retcode: ' + retcode);
            return -1;
        }

        this.sessionid = sessionid;
        this.m_iBeginDate = beginDate;
        this.m_iEndDate = endDate;
        this.m_iBeginFund = beginFund;
        this.m_iEndFund = endFund;
        this.m_iTaxRate = taxRate;
        this.m_vecTrade = vecTrade;
        this.m_vecHold = vecHold;
        this.m_currIdx = -1;

        return num;
    }

    this.get = function (sessionid) {
        //this.sessionid = kline.m_user.sessionid;
        this.sessionid = "";
        var req =
           {
               "method": "ktrain.search",
               "session": sessionid,
               "current": 1,
               "createtime": "0"
           };
        console.log('ktrain.search: ' + JSON.stringify(req));
        var retcode = "0000";
        var beginDate = 0;
        var endDate = 0;
        var beginFund = 0;
        var endFund = 0;
        var taxRate = 0;
        var vecTrade = [];
        var vecHold = [];
        //var currIdx = -1;
        function remoteTrainSearch(objJson) {
            if (objJson.retcode != '0000') {
                console.log("ERROR: retcode[" + objJson.retcode + "] != 0000");
                retcode = objJson.retcode;
                return false;
            }
            retcode = objJson.retcode;
            console.log(objJson);
            //username = objJson.result.name;
            //session = objJson.result.session;
            beginDate = objJson.result.begindate;
            beginFund = objJson.result.beginfund;
            endDate   = objJson.result.enddate;
            endFund   = objJson.result.endfund;
            taxRate   = objJson.result.taxrate;
            vecTrade  = objJson.result.operate;
            vecHold   = objJson.result.hold;

            return true;
        }
        gdata.send(req, remoteTrainSearch);

        if ('0000' != retcode) {
            console.log('train.search error retcode: ' + retcode);
            return false;
        }
        if (0 == beginDate || 0 == beginFund) {
            return true;
        }
        this.sessionid = sessionid;
        this.m_iBeginDate = beginDate;
        this.m_iEndDate = endDate;
        this.m_iBeginFund = beginFund;
        this.m_iEndFund = endFund;
        this.m_iTaxRate = taxRate;
        this.m_vecTrade = vecTrade;
        this.m_vecHold = vecHold;
        this.m_currIdx = -1;
        return true;
    }

    this.filter = function (strCode) {
        var arrHold = [];
        this.m_vecTrade.length = 0;
        for (var i = 0; i < this.m_vecHold.length; i++) {
            if (this.m_vecHold[i].code == strCode) {
                arrHold.push(this.m_vecHold[i]);
            }
        }
        this.m_vecHold = arrHold.slice(0);
        if (0 < this.m_vecHold.length) {
            this.m_currIdx = 0;
        }
        else {
            this.m_currIdx = -1;
        }
        return true;
    }
};