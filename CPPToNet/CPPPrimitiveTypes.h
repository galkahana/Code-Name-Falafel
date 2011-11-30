#pragma once


// These are the CPP types interesting for the library. 
enum ECPPPrimitiveType
{
	eCPPBool = 0,
	eCPPChar,
	eCPPUnsignedChar,
	eCPPShort,
	eCPPUnsignedShort,
	eCPPInt,
	eCPPUnsigned,
	eCPPWChar,
	eCPPLong,
	eCPPUnsignedLong,
	eCPPLongLong,
	eCPPUnsignedLongLong,
	eCPPVoid,
	eCPPFloat,
	eCPPDouble,
	eCPPLongDouble,
	eCPPPrimitivesMax
};

static const char* scPrimitivesLabels[] = 
{
	"bool",
	"char",
	"unsigned char",
	"short",
	"unsigned short",
	"int",
	"unsigned",
	"wchar_t",
	"long",
	"unsigned long",
	"long long",
	"unsigned long long",
	"void",
	"float",
	"double",
	"long double"
};

