#!/usr/bin/env python3

import sys, os, mmap, shutil, codecs, time

# My custom lib.
import calc

print('''
    Lunar Magic Signer By Jixun
    Version 1.0 Use at your own risk.

    Lunar Magic was created by FuSoYa:
    http://fusoya.eludevisibility.org/lm/

    Usage: python3 sign <input> <output> [mud]
''')


def ByteToHex(byteStr):
    return (codecs.getencoder('hex_codec')(byteStr)[0]).decode('utf-8')


if (len(sys.argv) - 1 < 2):
	print ('Error: not enough argument.')
	exit (1)

if not os.path.isfile (sys.argv[1]):
	print ('Input file not exist.')
	exit (2)

try:
	fOut = open (sys.argv[2], 'w+b')
	fOut.close ()
except:
	print ('Can\'t open output file.')
	exit (3)

if sys.argv[1] != sys.argv[2]:
	shutil.copyfile (sys.argv[1], sys.argv[2])

hFile = os.open(sys.argv[2], os.O_RDWR)
mFile = mmap.mmap(hFile, 0)

creticalCode = b'\xb8\xfc\xff\xff\x33\xf6\xbf'
creticalPos  = mFile.find (creticalCode)

if creticalPos == -1:
	print ('Critical code not found')
	exit (4)

if mFile.find (creticalCode, creticalPos + 1) != -1:
	print ('Multiple critical code found!')
	exit (5)

# The Magical number!
# Used to cancel out the calculation (i.e. = 0)
nop = b'\x4A\x31\x4A\x31' # J1J1
mFile.seek  (creticalPos + 7)
mFile.write (nop)
mFile.seek  (4, os.SEEK_CUR)
mFile.write (nop)
mFile.seek  (0)

extraInfo = b''
if len(sys.argv) > 3 and os.path.isfile (sys.argv[3]):
	with open (sys.argv[3], 'rb') as file:
		os.lseek (hFile, 0, os.SEEK_END)
		extraInfo = file.read()

startTime = time.time()
print ('Calculating new sign...')
newHash  = calc.calc (mFile.read() + extraInfo)
hashPair = calc.getByte(calc.hashPair (newHash))
newHash  = calc.getByte(newHash)
extraBit = calc.repealGen (newHash + hashPair)
print ('Position \t %#010x,  %#010x' % (creticalPos + 7, creticalPos + 7 + 4))
print ('Signature\t 0x%s, ~0x%s' % (ByteToHex(newHash), ByteToHex(hashPair)))
print ('File pad.\t 0x%s' % ByteToHex(extraBit))
print ('Time escaped\t %.0fms' % ((time.time() - startTime) * 1000))

# Append additional info to EOF
os.lseek (hFile, 0, os.SEEK_END)

# Extra bit is always 8-byte
# hence put this in the front.
os.write (hFile, extraBit)
os.write (hFile, extraInfo)

# Fill in the new hash
mFile.seek  (creticalPos + 7)
mFile.write (newHash)
mFile.seek  (4, os.SEEK_CUR)
mFile.write (hashPair)

os.close (hFile)