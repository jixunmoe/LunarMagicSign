revOdd =  [0] * 256
revEven = [0] * 256


# Force [& 0xFF] to ensure it's a byte.
def unsigned32 (i): return i & 0xffffffff
def odd   (i): return 0xFF & (i << 0x2 | i >> 0x6) ^ 0x29
def even  (i): return 0xFF & (i << 0x5 | i >> 0x3) ^ 0x26
def rOdd  (i): return revOdd [i]
def rEven (i): return revEven[i]

for i in range (0x00, 0xFF + 1):
	revOdd  [odd(i)]  = i
	revEven [even(i)] = i

# print ('%d~%d %d~%d' % (rOdd(0), rOdd(0xFF), rEven(0), rEven(0xFF)))

def calc (hFile):
	isOdd = False
	result = 0
	for i in range(0, len(hFile)):
		if (isOdd):
			result += odd (hFile[i])
		else:
			result -= even (hFile[i])
		isOdd = not isOdd

	return result

def getByte (src):
	return unsigned32(src).to_bytes(4, byteorder='little')

def repealGen (src):
	ret = bytearray(len(src))
	for i in range (0, int(len (src) / 2)):
		ret [i * 2]     = rEven(odd(src [i * 2]))
		ret [i * 2 + 1] = rOdd(even(src [i * 2 + 1]))
	return bytes(ret)

def hashPair (oldHash):
	return ~0 - oldHash;