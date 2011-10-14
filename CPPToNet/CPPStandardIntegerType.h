#pragma once

#include "ICPPType.h"
#include "CPPValue.h"


template <typename T>
class CPPStandardIntegerTypeAbstract : public ICPPType
{
public:
	CPPStandardIntegerTypeAbstract(void){};
	virtual ~CPPStandardIntegerTypeAbstract(void){};

	virtual CPPValue OnesComplement(const CPPValue& inValue);
	virtual CPPValue Not(const CPPValue& inValue);
	virtual CPPValue UnaryPlus(const CPPValue& inValue);

	virtual CPPValue ToULongLong(const CPPValue& inValue);
	virtual CPPValue ToLongLong(const CPPValue& inValue);
	virtual CPPValue ToULong(const CPPValue& inValue);
	virtual CPPValue ToLong(const CPPValue& inValue);
	virtual CPPValue ToUnsignedInt(const CPPValue& inValue);
	virtual CPPValue ToInt(const CPPValue& inValue);
	virtual CPPValue ToBool(const CPPValue& inValue);

	virtual CPPValue Multiply(const CPPValue& inLeftValue,const CPPValue& inRightValue);
	virtual CPPValue Divide(const CPPValue& inLeftValue,const CPPValue& inRightValue);
	virtual CPPValue Modulus(const CPPValue& inLeftValue,const CPPValue& inRightValue);
	virtual CPPValue Add(const CPPValue& inLeftValue,const CPPValue& inRightValue);
	virtual CPPValue Subtract(const CPPValue& inLeftValue,const CPPValue& inRightValue);

	virtual CPPValue LeftByteShift(const CPPValue& inLeftValue,const CPPValue& inRightValue);
	virtual CPPValue RightByteShift(const CPPValue& inLeftValue,const CPPValue& inRightValue);

	virtual CPPValue LessThan(const CPPValue& inLeftValue,const CPPValue& inRightValue);
	virtual CPPValue GreaterThan(const CPPValue& inLeftValue,const CPPValue& inRightValue);
	virtual CPPValue LessThanOrEqual(const CPPValue& inLeftValue,const CPPValue& inRightValue);
	virtual CPPValue GreaterThanOrEqual(const CPPValue& inLeftValue,const CPPValue& inRightValue);
	virtual CPPValue Equal(const CPPValue& inLeftValue,const CPPValue& inRightValue);
	virtual CPPValue NotEqual(const CPPValue& inLeftValue,const CPPValue& inRightValue);

	virtual CPPValue BitwiseAnd(const CPPValue& inLeftValue,const CPPValue& inRightValue);
	virtual CPPValue BitwiseOr(const CPPValue& inLeftValue,const CPPValue& inRightValue);
	virtual CPPValue ExclusiveOr(const CPPValue& inLeftValue,const CPPValue& inRightValue);

protected:

	// specific types should implement this method
	virtual CPPValue TypeValue(T inValue) = 0;
	virtual T ValueType(const CPPValue& inValue) = 0;

	CPPValue BoolValue(bool inValue);
};

template <typename T>
CPPValue CPPStandardIntegerTypeAbstract<T>::OnesComplement(const CPPValue& inValue)
{
	return TypeValue(~ValueType(inValue));
}

template <typename T>
CPPValue CPPStandardIntegerTypeAbstract<T>::Not(const CPPValue& inValue)
{
	return TypeValue(!ValueType(inValue));
}

template <typename T>
CPPValue CPPStandardIntegerTypeAbstract<T>::UnaryPlus(const CPPValue& inValue)
{
	return TypeValue(ValueType(inValue));
}

template <typename T>
CPPValue CPPStandardIntegerTypeAbstract<T>::ToULongLong(const CPPValue& inValue)
{
	CPPValue value;
	value.mType = eCPPUnsignedLongLong;
	value.mULongLongValue = (unsigned long long)ValueType(inValue);
	return value;
}

template <typename T>
CPPValue CPPStandardIntegerTypeAbstract<T>::ToLongLong(const CPPValue& inValue)
{
	CPPValue value;
	value.mType = eCPPLongLong;
	value.mLongLongValue = (long long)ValueType(inValue);
	return value;
}

template <typename T>
CPPValue CPPStandardIntegerTypeAbstract<T>::ToULong(const CPPValue& inValue)
{
	CPPValue value;
	value.mType = eCPPUnsignedLong;
	value.mULongValue = (unsigned long)ValueType(inValue);
	return value;
}

template <typename T>
CPPValue CPPStandardIntegerTypeAbstract<T>::ToLong(const CPPValue& inValue)
{
	CPPValue value;
	value.mType = eCPPLong;
	value.mLongValue = (long)ValueType(inValue);
	return value;
}

template <typename T>
CPPValue CPPStandardIntegerTypeAbstract<T>::ToUnsignedInt(const CPPValue& inValue)
{
	CPPValue value;
	value.mType = eCPPInt;
	value.mUIntValue = (unsigned)ValueType(inValue);
	return value;
}

template <typename T>
CPPValue CPPStandardIntegerTypeAbstract<T>::ToInt(const CPPValue& inValue)
{
	CPPValue value;
	value.mType = eCPPInt;
	value.mIntValue = (int)ValueType(inValue);
	return value;
}

template <typename T>
CPPValue CPPStandardIntegerTypeAbstract<T>::ToBool(const CPPValue& inValue)
{
	CPPValue value;
	value.mType = eCPPBool;
	value.mBoolValue = ValueType(inValue) != 0;
	return value;	
}

template <typename T>
CPPValue CPPStandardIntegerTypeAbstract<T>::Multiply(const CPPValue& inLeftValue,const CPPValue& inRightValue)
{
	return TypeValue(ValueType(inLeftValue) * ValueType(inRightValue));
}

template <typename T>
CPPValue CPPStandardIntegerTypeAbstract<T>::Divide(const CPPValue& inLeftValue,const CPPValue& inRightValue)
{
	return TypeValue(ValueType(inLeftValue) / ValueType(inRightValue));
}

template <typename T>
CPPValue CPPStandardIntegerTypeAbstract<T>::Modulus(const CPPValue& inLeftValue,const CPPValue& inRightValue)
{
	return TypeValue(ValueType(inLeftValue) % ValueType(inRightValue));
}

template <typename T>
CPPValue CPPStandardIntegerTypeAbstract<T>::Add(const CPPValue& inLeftValue,const CPPValue& inRightValue)
{
	return TypeValue(ValueType(inLeftValue) + ValueType(inRightValue));
}

template <typename T>
CPPValue CPPStandardIntegerTypeAbstract<T>::Subtract(const CPPValue& inLeftValue,const CPPValue& inRightValue)
{
	return TypeValue(ValueType(inLeftValue) - ValueType(inRightValue));
}

template <typename T>
CPPValue CPPStandardIntegerTypeAbstract<T>::LeftByteShift(const CPPValue& inLeftValue,const CPPValue& inRightValue)
{
	return TypeValue(ValueType(inLeftValue)<<inRightValue.mULongLongValue);
}

template <typename T>
CPPValue CPPStandardIntegerTypeAbstract<T>::RightByteShift(const CPPValue& inLeftValue,const CPPValue& inRightValue)
{
	return TypeValue(ValueType(inLeftValue)>>inRightValue.mULongLongValue);
}

template <typename T>
CPPValue CPPStandardIntegerTypeAbstract<T>::LessThan(const CPPValue& inLeftValue,const CPPValue& inRightValue)
{
	return BoolValue(ValueType(inLeftValue) < ValueType(inRightValue));
}

template <typename T>
CPPValue CPPStandardIntegerTypeAbstract<T>::GreaterThan(const CPPValue& inLeftValue,const CPPValue& inRightValue)
{
	return BoolValue(ValueType(inLeftValue) > ValueType(inRightValue));
}

template <typename T>
CPPValue CPPStandardIntegerTypeAbstract<T>::LessThanOrEqual(const CPPValue& inLeftValue,const CPPValue& inRightValue)
{
	return BoolValue(ValueType(inLeftValue) <= ValueType(inRightValue));
}

template <typename T>
CPPValue CPPStandardIntegerTypeAbstract<T>::GreaterThanOrEqual(const CPPValue& inLeftValue,const CPPValue& inRightValue)
{
	return BoolValue(ValueType(inLeftValue) >= ValueType(inRightValue));
}

template <typename T>
CPPValue CPPStandardIntegerTypeAbstract<T>::Equal(const CPPValue& inLeftValue,const CPPValue& inRightValue)
{
	return BoolValue(ValueType(inLeftValue) == ValueType(inRightValue));
}

template <typename T>
CPPValue CPPStandardIntegerTypeAbstract<T>::NotEqual(const CPPValue& inLeftValue,const CPPValue& inRightValue)
{
	return BoolValue(ValueType(inLeftValue) != ValueType(inRightValue));
}

template <typename T>
CPPValue CPPStandardIntegerTypeAbstract<T>::BoolValue(bool inValue)
{
	CPPValue value;
	value.mType = eCPPBool;
	value.mBoolValue = inValue;
	return value;
}

template <typename T>
CPPValue CPPStandardIntegerTypeAbstract<T>::BitwiseAnd(const CPPValue& inLeftValue,const CPPValue& inRightValue)
{
	return TypeValue(ValueType(inLeftValue) & ValueType(inRightValue));
}

template <typename T>
CPPValue CPPStandardIntegerTypeAbstract<T>::BitwiseOr(const CPPValue& inLeftValue,const CPPValue& inRightValue)
{
	return TypeValue(ValueType(inLeftValue) | ValueType(inRightValue));
}

template <typename T>
CPPValue CPPStandardIntegerTypeAbstract<T>::ExclusiveOr(const CPPValue& inLeftValue,const CPPValue& inRightValue)
{
	return TypeValue(ValueType(inLeftValue) ^ ValueType(inRightValue));
}

// using a combo of default params and specialization to implement signed parametrs casting when required, and avoid it when not

template <typename T,typename SignedT=T>
class CPPStandardIntegerType : public CPPStandardIntegerTypeAbstract<T>
{
public:
	virtual CPPValue UnaryMinus(const CPPValue& inValue)
	{
		return TypeValue(-(SignedT)ValueType(inValue));
	}
};

template <typename T>
class CPPStandardIntegerType<T,T> : public CPPStandardIntegerTypeAbstract<T>
{
public:
	virtual CPPValue UnaryMinus(const CPPValue& inValue)
	{
		return TypeValue(-ValueType(inValue));
	}
};


