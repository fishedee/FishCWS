# Introduce
* Chinese Word Segmentation System Based On ICTCLAS
* Chinese Word Pinyin System
* Commiuate With Http，Easy To Use in ptyhon,php,js and so on
* You Can redefine your own dictory
* Base on ICTCLAS，So Support GBK,UTF8,and so on

# Environments
* Only Linux x86-64 And Utf8

# Install

## Dependence
* sudo apt-get install libevent-dev
* sudo apt-get install libjsoncpp-dev

## Compile
* make
* make install

## Run
* sudo fishcws -d -x userdict.txt

# Useage
## getSegment
```bash
curl -d "{\"type\":\"getSegment\",\"data\":\"我最喜爱的食物是蔓越莓饼干和蔓越莓草莓\"}" 127.0.0.1:1985
```
```bash
{"code":0,"data":[{"pos":"rr","weight":6305,"word":"我"},{"pos":"d","weight":7002,"word":"最"},{"pos":"v","weight":10500,"word":"喜爱"},{"pos":"ude1","weight":3020,"word":"的"},{"pos":"n","weight":11464,"word":"食物"},{"pos":"vshi","weight":4710,"word":"是"},{"pos":"n","weight":18804,"word":"蔓越莓"},{"pos":"n","weight":13166,"word":"饼干"},{"pos":"cc","weight":4618,"word":"和"},{"pos":"n","weight":18804,"word":"蔓越莓"},{"pos":"n","weight":11979,"word":"草莓"}],"msg":""}
```
## getPinyin
```bash
curl -d "{\"type\":\"getPinyin\",\"data\":\"中华人民共和国\"}" localhost:1985
```
```bash
{"code":0,"data":[{"pinyin":"zhong","word":"中"},{"pinyin":"hua","word":"华"},{"pinyin":"ren","word":"人"},{"pinyin":"min","word":"民"},{"pinyin":"gong","word":"共"},{"pinyin":"he","word":"和"},{"pinyin":"guo","word":"国"}],"msg":""}
```

# ChangeLog

## V1.1.0
* Add Pinyin
* Add Word Weight Output

## V1.0.0
* Add ICTCLAS 2015
* Support Linux x86-64
* Support UTF8 encode
* Support HTTP request
