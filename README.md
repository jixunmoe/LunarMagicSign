# About this project
This utility was created to sign "Lunar Magic" executable for my translation project (Which has been discontinued for a while).

This should work with recent version (~2.2.x), 

Completely rewrite in PURE C! Since the cpp version didn't work as I expected :<

## Tools used to analyses this thingy
* IDA
* OllyDbg
* Hex Editor

## About the 2 versions
At the beginning, I only did the py3 version, but it seems too slow to sign the file.

So I ported to C++. Since C++ version didn't work well, I re-wrote the whole thing in PURE C.

py3 version is still usable, just signs the file bit slower.

## Command line usage
```bash
./lmSign "Lunar Magic CHS.exe" "Lunar Magic CHS Signed.exe"
```
.. or, using the py3 version
```bash
python3 py3-signer "Lunar Magic CHS.exe" "Lunar Magic CHS Signed.exe"
```
