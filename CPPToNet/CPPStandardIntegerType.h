#pragma once

#include "ICPPPrimitiveType.h"
#include "CPPPrimitiveValue.h"


template <typename T>
class CPPStandardIntegerTypeAbstract : public ICPPPrimitiveType
{
public:
	CPPStandardIntegerTypeAbstract(void){};
	virtual ~CPPStandardIntegerTypeAbstract(void){};

	virtual CPPPrimitiveValue OnesComplement(const CPPPrimitiveValue& inValue);
	virtual CPPPrimitiveValue Not(const CPPPrimitiveValue& inValue);
	virtual CPPPrimitiveValue UnaryPlus(const CPPPrimitiveValue& inValue);

	virtual CPPPrimitiveValue ToULongLong(const CPPPrimitiveValue& inValue);
	virtual CPPPrimitiveValue ToLongLong(const CPPPrimitiveValue& inValue);
	virtual CPPPrimitiveValue ToULong(const CPPPrimitiveValue& inValue);
	virtual CPPPrimitiveValue ToLong(const CPPPrimitiveValue& inValue);
	virtual CPPPrimitiveValue ToUnsignedInt(const CPPPrimitiveValue& inValue);
	virtual CPPPrimitiveValue ToInt(const CPPPrimitiveValue& inValue);
	virtual CPPPrimitiveValue ToBool(const CPPPrimitiveValue& inValue);

	virtual CPPPrimitiveValue Multiply(const CPPPrimitiveValue& inLeftValue,const CPPPrimitiveValue& inRightValue);
	virtual CPPPrimitiveValue Divide(const CPPPrimitiveValue& inLeftValue,const CPPPrimitiveValue& inRightValue);
	virtual CPPPrimitiveValue Modulus(const CPPPrimitiveValue& inLeftValue,const CPPPrimitiveValue& inRightValue);
	virtual CPPPrimitiveValue Add(const CPPPrimitiveValue& inLeftValue,const CPPPrimitiveValue& inRightValue);
	virtual CPPPrimitiveValue Subtract(const CPPPrimitiveValue& inLeftValue,const CPPPrimitiveValue& inRightValue);

	virtual CPPPrimitiveValue LeftByteShift(const CPPPrimitiveValue& inLeftValue,const CPPPrimitiveValue& inRightValue);
	virtual CPPPrimitiveValue RightByteShift(const CPPPrimitiveValue& inLeftValue,const CPPPrimitiveValue& inRightValue);

	virtual CPPPrimitiveValue LessThan(const CPPPrimitiveValue& inLeftValue,const CPPPrimitiveValue& inRightValue);
	virtual CPPPrimitiveValue GreaterThan(const CPPPrimitiveValue& inLeftValue,const CPPPrimitiveValue& inRightValue);
	virtual CPPPrimitiveValue LessThanOrEqual(const CPPPrimitiveValue& inLeftValue,const CPPPrimitiveValue& inRightValue);
	virtual CPPPrimitiveValue GreaterThanOrEqual(const CPPPrimitiveValue& inLeftValue,const CPPPrimitiveValue& inRightValue);
	virtual CPPPrimitiveValue Equal(const CPPPrimitiveValue& inLeftValue,const CPPPrimitiveValue& inRightValue);
	virtual CPPPrimitiveValue NotEqual(const CPPPrimitiveValue& inLeftValue,const CPPPrimitiveValue& inRightValue);

	virtual CPPPrimitiveValue BitwiseAnd(const CPPPrimitiveValue& inLeftValue,const CPPPrimitiveValue& inRightValue);
	virtual CPPPrimitiveValue BitwiseOr(const CPPPrimitiveValue& inLeftValue,const CPPPrimitiveValue& inRightValue);
	virtual CPPPrimitiveValue ExclusiveOr(const CPPPrimitiveValue& inLeftValue,const CPPPrimitiveValue& inRightValue);

protected:

	// specific types should implement this method
	virtual CPPPrimitiveValue TypeValue(T inValue) = 0;
	virtual T ValueType(const CPPPrimitiveValue& inValue) = 0;

	CPPPrimitiveValue BoolValue(bool inValue);
};

template <typename T>
CPPPrimitiveValue CPPStandardIntegerTypeAbstract<T>::OnesComplement(const CPPPrimitiveValue& inValue)
{
	return TypeValue(~ValueType(inValue));
}

template <typename T>
CPPPrimitiveValue CPPStandardIntegerTypeAbstract<T>::Not(const CPPPrimitiveValue& inValue)
{
	return TypeValue(!ValueType(inValue));
}

template <typename T>
CPPPrimitiveValue CPPStandardIntegerTypeAbstract<T>::UnaryPlus(const CPPPrimitiveValue& inValue)
{
	return TypeValue(ValueType(inValue));
}

template <typename T>
CPPPrimitiveValue CPPStandardIntegerTypeAbstract<T>::ToULongLong(const CPPPrimitiveValue& inValue)
{
	CPPPrimitiveValue value;
	value.mType = eCPPUnsignedLongLong;
	value.mULongLongValue = (unsigned long long)ValueType(inValue);
	return value;
}

template <typename T>
CPPPrimitiveValue CPPStandardIntegerTypeAbstract<T>::ToLongLong(const CPPPrimitiveValue& inValue)
{
	CPPPrimitiveValue value;
	value.mType = eCPPLongLong;
	value.mLongLongValue = (long long)ValueType(inValue);
	return value;
}

template <typename T>
CPPPrimitiveValue CPPStandardIntegerTypeAbstract<T>::ToULong(const CPPPrimitiveValue& inValue)
{
	CPPPrimitiveValue value;
	value.mType = eCPPUnsignedLong;
	value.mULongValue = (unsigned long)ValueType(inValue);
	return value;
}

template <typename T>
CPPPrimitiveValue CPPStandardIntegerTypeAbstract<T>::ToLong(const CPPPrimitiveValue& inValue)
{
	CPPPrimitiveValue value;
	value.mType = eCPPLong;
	value.mLongValue = (long)ValueType(inValue);
	return value;
}

template <typename T>
CPPPrimitiveValue CPPStandardIntegerTypeAbstract<T>::ToUnsignedInt(const CPPPrimitiveValue& inValue)
{
	CPPPrimitiveValue value;
	value.mType = eCPPInt;
	value.mUIntValue = (unsigned)ValueType(inValue);
	return value;
}

template <typename T>
CPPPrimitiveValue CPPStandardIntegerTypeAbstract<T>::ToInt(const CPPPrimitiveValue& inValue)
{
	CPPPrimitiveValue value;
	value.mType = eCPPInt;
	value.mIntValue = (int)ValueType(inValue);
	return value;
}

template <typename T>
CPPPrimitiveValue CPPStandardIntegerTypeAbstract<T>::ToBool(const CPPPrimitiveValue& inValue)
{
	CPPPrimitiveValue value;
	value.mType = eCPPBool;
	value.mBoolValue = ValueType(inValue) != 0;
	return value;	
}

template <typename T>
CPPPrimitiveValue CPPStandardIntegerTypeAbstract<T>::Multiply(const CPPPrimitiveValue& inLeftValue,const CPPPrimitiveValue& inRightValue)
{
	return TypeValue(ValueType(inLeftValue) * ValueType(inRightValue));
}

template <typename T>
CPPPrimitiveValue CPPStandardIntegerTypeAbstract<T>::Divide(const CPPPrimitiveValue& inLeftValue,const CPPPrimitiveValue& inRightValue)
{
	return TypeValue(ValueType(inLeftValue) / ValueType(inRightValue));
}

template <typename T>
CPPPrimitiveValue CPPStandardIntegerTypeAbstract<T>::Modulus(const CPPPrimitiveValue& inLeftValue,const CPPPrimitiveValue& inRightValue)
{
	return TypeValue(ValueType(inLeftValue) % ValueType(inRightValue));
}

template <typename T>
CPPPrimitiveValue CPPStandardIntegerTypeAbstract<T>::Add(const CPPPrimitiveValue& inLeftValue,const CPPPrimitiveValue& inRightValue)
{
	return TypeValue(ValueType(inLeftValue) + ValueType(inRightValue));
}

template <typename T>
CPPPrimitiveValue CPPStandardIntegerTypeAbstract<T>::Subtract(const CPPPrimitiveValue& inLeftValue,const CPPPrimitiveValue& inRightValue)
{
	return TypeValue(ValueType(inLeftValue) - ValueType(inRightValue));
}

template <typename T>
CPPPrimitiveValue CPPStandardIntegerTypeAbstract<T>::LeftByteShift(const CPPPrimitiveValue& inLeftValue,const CPPPrimitiveValue& inRightValue)
{
	return TypeValue(ValueType(inLeftValue)<<inRightValue.mULongLongValue);
}

template <typename T>
CPPPrimitiveValue CPPStandardIntegerTypeAbstract<T>::RightByteShift(const CPPPrimitiveValue& inLeftValue,const CPPPrimitiveValue& inRightValue)
{
	return TypeValue(ValueType(inLeftValue)>>inRightValue.mULongLongValue);
}

template <typename T>
CPPPrimitiveValue CPPStandardIntegerTypeAbstract<T>::LessThan(const CPPPrimitiveValue& inLeftValue,const CPPPrimitiveValue& inRightValue)
{
	return BoolValue(ValueType(inLeftValue) < ValueType(inRightValue));
}

template <typename T>
CPPPrimitiveValue CPPStandardIntegerTypeAbstract<T>::GreaterThan(const CPPPrimitiveValue& inLeftValue,const CPPPrimitiveValue& inRightValue)
{
	return BoolValue(ValueType(inLeftValue) > ValueType(inRightValue));
}

template <typename T>
CPPPrimitiveValue CPPStandardIntegerTypeAbstract<T>::LessThanOrEqual(const CPPPrimitiveValue& inLeftValue,const CPPPrimitiveValue& inRightValue)
{
	return BoolValue(ValueType(inLeftValue) <= ValueType(inRightValue));
}

template <typename T>
CPPPrimitiveValue CPPStandardIntegerTypeAbstract<T>::GreaterThanOrEqual(const CPPPrimitiveValue& inLeftValue,const CPPPrimitiveValue& inRightValue)
{
	return BoolValue(ValueType(inLeftValue) >= ValueType(inRightValue));
}

template <typename T>
CPPPrimitiveValue CPPStandardIntegerTypeAbstract<T>::Equal(const CPPPrimitiveValue& inLeftValue,const CPPPrimitiveValue& inRightValue)
{
	return BoolValue(ValueType(inLeftValue) == ValueType(inRightValue));
}

template <typename T>
CPPPrimitiveValue CPPStandardIntegerTypeAbstract<T>::NotEqual(const CPPPrimitiveValue& inLeftValue,const CPPPrimitiveValue& inRightValue)
{
	return BoolValue(ValueType(inLeftValue) != ValueType(inRightValue));
}

template <typename T>
CPPPrimitiveValue CPPStandardIntegerTypeAbstract<T>::BoolValue(bool inValue)
{
	CPPPrimitiveValue value;
	value.mType = eCPPBool;
	value.mBoolValue = inValue;
	return value;
}

template <typename T>
CPPPrimitiveValue CPPStandardIntegerTypeAbstract<T>::BitwiseAnd(const CPPPrimitiveValue& inLeftValue,const CPPPrimitiveValue& inRightValue)
{
	return TypeValue(ValueType(inLeftValue) & ValueType(inRightValue));
}

template <typename T>
CPPPrimitiveValue CPPStandardIntegerTypeAbstract<T>::BitwiseOr(const CPPPrimitiveValue& inLeftValue,const CPPPrimitiveValue& inRightValue)
{
	return TypeValue(ValueType(inLeftValue) | ValueType(inRightValue));
}

template <typename T>
CPPPrimitiveValue CPPStandardIntegerTypeAbstract<T>::ExclusiveOr(const CPPPrimitiveValue& inLeftValue,const CPPPrimitiveValue& inRightValue)
{
	return TypeValue(ValueType(inLeftValue) ^ ValueType(inRightValue));
}

// using a combo of default params and specialization to implement signed parametrs casting when required, and avoid it when not

template <typename T,typename SignedT=T>
class CPPStandardIntegerType : public CPPStandardIntegerTypeAbstract<T>
{
public:
	virtual CPPPrimitiveValue UnaryMinus(const CPPPrimitiveValue& inValue)
	{
		return TypeValue(-(SignedT)ValueType(inValue));
	}
};

template <typename T>
class CPPStandardIntegerType<T,T> : public CPPStandardIntegerTypeAbstract<T>
{
public:
	virtual CPPPrimitiveValue UnaryMinus(const CPPPrimitiveValue& inValue)
	{
		return TypeValue(-ValueType(inValue));
	}
};


