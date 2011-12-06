import sys
import re
import cgi

def padL(s, p, pc=" "):
	return pc*(p - len(s)) + s

def padR(s, p, pc= " "):
	return s + pc*(p - len(s))

DEC_NONE		= 0
DEC_IMM_JMP		= 1

def mkInstruction(addr, bytes, opcode, operands, comment):
	global DEC_NONE
	global DEC_IMM_JMP

	decoding = (DEC_NONE,)
	if opcode[0] == "j":
		m = re.match(r'0x([A-Fa-f0-9]+)', operands)
		if m:
			decoding = (DEC_IMM_JMP, addr, int(m.group(1), 16))

	return (addr, bytes, opcode, operands, comment, decoding)

def methodNameKey(class_, name):
	return "%s %s" % (class_, name)

class JmpSet:
	def __init__(self):
		self.srcJmps = {}
		self.dstJmps = {}
		self.methods = {}
		self.methodName = None

	def _jmp(self, src, dst):
		self.srcJmps[src] = [dst]

		if not self.dstJmps.has_key(dst):
			self.dstJmps[dst] = []
		self.dstJmps[dst].append(src)

	def startMethod(self, class_, name):
		self.methodName = methodNameKey(class_, name)

	def endMethod(self):
		self.methods[self.methodName] = (self.srcJmps, self.dstJmps)
		self.srcJmps = {}
		self.dstJmps = {}
	
	def getSrcJmps(self, method, addr):
		if not self.methods.has_key(method):
			return []
		(srcJmps, _) = self.methods[method]
		if not srcJmps.has_key(addr):
			return []

		return srcJmps[addr]

	def getDstJmps(self, method, addr):
		if not self.methods.has_key(method):
			return []
		(_, dstJmps) = self.methods[method]
		if not dstJmps.has_key(addr):
			return []

		return dstJmps[addr]

	def instruction(self, instruction):
		global DEC_IMM_JMP
		(addr, bytes, opcode, operands, comment, decoding) = instruction
		if (decoding[0] == DEC_IMM_JMP):
			(_, src, dst) = decoding
			self._jmp(src, dst)

def isJoinArrow(arrow, addr):
	if type(arrow) != type(None):
		(src, dst, jDst) = arrow
		if dst == addr or src == addr:
			return True
	return False

import random

def addArrow(arrows, src, dst, jDst):
	for i in range(len(arrows)):
		if type(arrows[i]) != type(None):
			continue

		arrows[i] = (src, dst, jDst)
		break;
	else:
		arrows.append((src, dst, jDst))

class TxtFmt:
	def __init__(self, f, jmps):
		self.f = f
		self.jmps = jmps
		self.methodName = None
		self.arrows = None
		self.txtCols = 79
		self.pc = " "

	def startMethod(self, class_, name):
		self.f.write("%s %s\n" % (class_, name))
		self.f.write("-" * self.txtCols + "\n")
		self.methodName = methodNameKey(class_, name)
		self.arrows = []

	def endMethod(self):
		self.methodName = None
		self.arrows = []
		self.f.write("-" * self.txtCols + "\n\n")

	def instruction(self, instruction):
		(addr, _, opcode, operands, comment, decoding) = instruction

		self.f.write(padR("%x" % addr, 8, self.pc))
		self.f.write(padL(opcode, 8, self.pc))
		self.f.write(padL(operands, 30, self.pc))
		self.f.write(" ")

		if (self.pc == " "):
			self.pc = "-"
		else:
			self.pc = " "

		horz = ""
		horzLine = 0

		srcJmps = self.jmps.getSrcJmps(self.methodName, addr)
		dstJmps = self.jmps.getDstJmps(self.methodName, addr)
		for dst in srcJmps:
			horzLine = 1
			horz = "o"
		for src in dstJmps:
			horzLine = 1
			horz = "<"

		self.f.write(padL(horz, 1))

		for dst in srcJmps:
			if dst > addr:
				addArrow(self.arrows, addr, dst, dst)
		for src in dstJmps:
			if src > addr:
				addArrow(self.arrows, addr, src, addr)

		lastJoinArrow = -1
		for (i, arrow) in zip(range(len(self.arrows)), self.arrows):
			if isJoinArrow(arrow, addr):
				lastJoinArrow = i

		for (i, arrow) in zip(range(len(self.arrows)), self.arrows):
			join = "-"
			right = "-"

			if isJoinArrow(arrow, addr):
				join = "+"

			if i > lastJoinArrow:
				horzLine = 0
			if i == lastJoinArrow:
				right = " "
				(src, dst, jDst) = arrow
				if dst > addr:
					join = "."
				else:
					join = "'"
				
			if horzLine:
				self.f.write("-%s%s" % (join, right))
				continue

			if type(arrow) == type(None):
				self.f.write("   ")
				continue

			(src, dst, jDst) = arrow
			self.f.write(" | ")

		arrows = []
		for arrow in self.arrows:
			if type(arrow) == type(None):
				arrows.append(None)
				continue

			(src, dst, jDst) = arrow
			if dst != addr:
				arrows.append((src, dst, jDst))
			else:
				arrows.append(None)
		self.arrows = arrows

		self.f.write(padL(comment, 3, " "))

		self.f.write("\n")

def pretty(f, fmt, fr):
	methR = re.compile(r'(\+|-)\(.*?\)\[(\w+)\s*([A-Za-z0-9_:]+)\]')
	lineR = re.compile(r'\s*\+\d+\s+([A-Fa-f0-9]+)\s+([A-Fa-f0-9]+)\s+(\w+)\s*(\S*)\s*(\S?.*)')

	ST_START	= 0
	ST_METH		= 1

	state = ST_START

	for line in f:
		line = line.strip()
		if state == ST_START:
			m = methR.match(line)
			if m:
				if fr.match(m.group(3)):
					state = ST_METH
					fmt.startMethod(m.group(2), m.group(3))
		elif state == ST_METH:
			m = lineR.match(line)
			if line == "":
				state = ST_START
				fmt.endMethod()
			elif m:
				fmt.instruction(mkInstruction(int(m.group(1), 16), m.group(2), m.group(3), m.group(4), m.group(5)))
			else:
				sys.stderr.write("Failed to match %s" % line)
				sys.exit(0)

	if state == ST_METH:
		fmt.endMethod()

def main(argv):
	if len(argv) != 3:
		sys.stderr.write("Usage:\n%s FILE.OTX METHOD_REGEX\n" % argv[0])
		return 1

	fn = argv[1]

	try:
		fr = re.compile(argv[2])
	except re.error, x:
		sys.stderr.write("Error compiling regex: %s\n" % x)
		return 1

	try:
		f = open(fn, "rb")
		jmps = JmpSet()
		pretty(f, jmps, fr)
		f.close()
	except IOError, x:
		sys.stderr.write("IO error on %s: %s\n" % (fn, x))
		return 1

	try:
		f = open(fn, "rb")
		fmt = TxtFmt(sys.stdout, jmps)
		pretty(f, fmt, fr)
		f.close()
	except IOError, x:
		sys.stderr.write("IO error on %s: %s\n" % (fn, x))
		return 1

	return 0

sys.exit(main(sys.argv))
