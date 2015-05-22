var retCode = ((function () {
var fs = require('fs');

console.info (function(){/*
/ ******************************************
  *   lm-patcher v1.01   [ For: LM 2.32 ]  *
  *        GitHub [ http://git.io/vTo7o ]  *
  *                                        *
  *  Usage:                                *
  *  node lm-patcher.js -i <input>         *
  *                                        *
  *  Required:                             *
  *  -i    Input file                      *
  *                                        *
  *  ** Make backup before you patch it!   *
  ****************************************** /
*/}.toString().slice(15,-4));

var argv = (function (argv) {
	var r = {};

	var key = '_';

	for (var i = 2, t; i < argv.length; i++) {
		if (argv[i][0] == '-') {
			key = argv[i].slice(1);
			r[key] = false;
		} else {
			r[key] = argv[i];
		}
	}
	
	return r;
})(process.argv);

var target = argv.i;
if (!target) return ;
var bufFile;
try {
	bufFile = fs.readFileSync(target);
} catch (err) {
	console.error('Unable to open target: %s', err.message);
	return 1;
}

function __ROL__ (x, n) {
	return 0xFF & ((x << n) | (x >> (8 - n)));
}

function __ROR__ (x, n) {
	return 0xFF & ((x >> n) | (x << (8 - n)));
}

function unsigned_byte (x) {
	if (x < 0)    x += 0xFF + 1;
	if (x > 0xFF) x -= 0xFF + 1; 
	return x;
}

function unsigned_int (x) {
	if (x < 0)          x += 0xFFFFFFFF + 1;
	if (x > 0xFFFFFFFF) x -= 0xFFFFFFFF + 1;
	return x;
}

function signed_int (x) {
	if (x & 0x80000000) {
		x -= 0xFFFFFFFF + 1
	}
	
	return x;
}

function unsigned_to_buf (x) {
	var buf = new Buffer(4);
	buf.writeUIntLE(unsigned_int(x), 0, 4);
	return buf;
}

function sign_rev_2 (x) {
	x = unsigned_byte(x ^ 0xEA + 2);
	return __ROR__(x, 7);
}

function sign_rev_1 (x) {
	return __ROR__(x ^ 0x19, 4);
}

function sign_rev_0 (x) {
	return __ROR__(x ^ 0x41, 2);
}

// Calculate Buffer from a hash.
function calcBuf (bufFile, hash) {
	var theByte, round;
	var nLength = bufFile.length;

	for(var i = 0; i<nLength; i++) {
		theByte = bufFile[i];

		if (i & 2) {
			round = unsigned_byte(__ROL__(theByte, 7) - 2);
			hash += round ^ 0xEA;
		} else if (i & 1) {
			round = __ROL__(theByte, 4);
			hash -= round ^ 0x19;
		} else {
			round = __ROL__(theByte, 2);
			hash += round ^ 0x41;
		}
		
		hash = unsigned_int(hash);
	}
	return hash;
}

// Distribute integer to bytes.
function distroBytes (x, n) {
	var arr = new Array(n);
	
	var i = 0;
	while (x > 255) {
		x -= 255;
		arr[i] = 255;
		i++;
	}
	
	for (; i < n; i++) {
		arr[i] = x;
		x = 0;
	}
	
	return arr;
}

var hash = calcBuf(bufFile, 16);
var hashFromFile = bufFile.readIntLE(bufFile.length - 16, 4);
var bPerformPatch = hashFromFile != hash;

console.info('Hash: %s vs %s, %s',
	hash.toString(16), hashFromFile.toString(16),
	bPerformPatch ? 'need patch!' : 'no need to patch.'
);

if (bufFile.length % 4 != 0) {
	bPerformPatch = false;
	console.error('ERROR: Please align exe to 4 bytes.');
	return 2;
}

if (!bPerformPatch) return 9;

var fd = fs.createWriteStream(target, {flags: 'a'});
var revHash = 0xFFFFFFFF - hash;

var b1 = unsigned_to_buf(hash   );
var b2 = unsigned_to_buf(revHash);
var valFix = signed_int(calcBuf(b2, calcBuf(b1, 0)));
// console.info('To be fixed: ', valFix);

var bfix = new Buffer(4);
bfix[0] = sign_rev_0(0);
bfix[1] = sign_rev_1(0xFF);
bfix[2] = sign_rev_2(0);
bfix[3] = sign_rev_2(0);
var tmpFixBuf = bfix.toString('base64');
var numBytes = 16;
while (valFix > 510) { // 0xFF * 2
	valFix -= 0xFF;
	numBytes += 4;
	fd.write(tmpFixBuf, 'base64');
}

fd.write(b1.toString('base64'), 'base64');
fd.write(b2.toString('base64'), 'base64');

// 计算需要的值
var bytes ;
if (valFix >= 0) {
	bytes = distroBytes(valFix, 2);
	b1[0] = b2[0] = sign_rev_0(0);
	
	b1[1] = sign_rev_1(bytes[0]);
	b2[1] = sign_rev_1(bytes[1]);
	
	b1[2] = b2[2] = sign_rev_2(0);
	b1[3] = b2[3] = sign_rev_2(0);
} else {
	bytes = distroBytes(-valFix, 6);
	b1[1] = b2[1] = sign_rev_1(0);
	
	b1[0] = sign_rev_0(bytes[0]);
	b2[0] = sign_rev_0(bytes[1]);
	
	b1[2] = sign_rev_2(bytes[2]);
	b2[2] = sign_rev_2(bytes[3]);
	
	b1[3] = sign_rev_2(bytes[4]);
	b2[3] = sign_rev_2(bytes[5]);
}
// valFix = signed_int(calcBuf(b2, calcBuf(b1, 0)));
// console.info('After fix: ', valFix);

fd.write(b1.toString('base64'), 'base64');
fd.write(b2.toString('base64'), 'base64');
fd.end(null, null, function () {
	console.info('File patched, %s bytes written.', numBytes);
});

})());

if (retCode) process.exit (retCode);
