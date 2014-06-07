# About this project
This utility was created to sign "Lunar Magic" executable for my translation project (Which has been discontinued for a while).

This should work with recent version (~2.2.x), 

Note: This is my first c++ project, some code may not readable as you expected QwQ

## Tools used to analyses this thingy
* IDA
* OllyDbg
* Bless Hex Editor

## About the 2 versions
I was planning to make the py3 version, then while testing it took too long. So I ported to c++.

py3 version is still usable, just a bit slower.

## Command line usage
```bash
./LunarMagicSign "Lunar Magic CHS.exe" "Lunar Magic CHS Signed.exe"
```
.. or, using the py3 version
```bash
python3 py3-signer "Lunar Magic CHS.exe" "Lunar Magic CHS Signed.exe"
```
