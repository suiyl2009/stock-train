###python 3.5

from urllib import parse,request
import json
#textmod={"jsonrpc": "2.0","method":"user.login","params":{"user":"admin","password":"zabbix"},"auth": None,"id":1}
#json串数据使用
#textmod = json.dumps(textmod).encode(encoding='utf-8')
#普通数据使用
#for i in range(0,1000):
#  textmod[i] = i

#textmod = {"method": "list.sub", "sub": "全国地产", "item": ["Code", "Name", "DayDate", "DayClose", "Revenue", "ClosingDate"], "date":20180208}
textmod = {"method": "list.show", "code": ["002367","002372","002415","002434","002440","002444","002449","002450","002456","002460","002464","002466","002468","002477","002493","002503","002508","002509","002517","002532","002534","002555","002563","002572","002589","002595","002597","002601","002636","002701"], "item": ["Code", "Name", "DayDate", "DayClose", "Revenue"]}

#textmod = parse.urlencode(textmod).encode(encoding='utf-8')
textmod = json.dumps(textmod).encode(encoding='utf-8')

print(textmod)
#输出内容:b'{"params": {"user": "admin", "password": "zabbix"}, "auth": null, "method": "user.login", "jsonrpc": "2.0", "id": 1}'
header_dict = {'User-Agent': 'Mozilla/5.0 (Windows NT 6.1; Trident/7.0; rv:11.0) like Gecko',"Content-Type": "application/json"}
url='http://192.168.72.128/Stock'
req = request.Request(url=url,data=textmod,headers=header_dict)
res = request.urlopen(req)
res = res.read()
print(res)
#输出内容:b'{"jsonrpc":"2.0","result":"37d991fd583e91a0cfae6142d8d59d7e","id":1}'
print(res.decode(encoding='utf-8'))
#输出内容:{"jsonrpc":"2.0","result":"37d991fd583e91a0cfae6142d8d59d7e","id":1}

