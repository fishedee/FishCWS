# Introduce
Chinese Word Segmentation System Based On ICTCLAS
Commiuate With Http，Easy To Use in ptyhon,php,js and so on
You Can redefine your own dictory
Base on ICTCLAS，So Support GBK,UTF8,and so on

# Environments
Only Linux x86-64 And Utf8

# Install

## Dependence
sudo apt-get install libevent-dev
sudo apt-get install libjsoncpp-dev

## Compile
make
make install

## Run
sudo fishcws -d -x userdict.txt

# Useage
```bash
curl -d "{\"type\":\"ParagraphProcess\",\"data\":\"我最喜爱的食物是蔓越莓饼干和蔓越莓草莓\"}" 127.0.0.1:1985
```
```bash
{"code":0,"data":["我","最","喜爱","的","食物","是","蔓越莓","饼干","和","蔓越莓","草莓"],"msg":""}
```

# ChangeLog

## V1.0.0
Add ICTCLAS 2015
Support Linux x86-64
Support UTF8 encode
Support HTTP request
