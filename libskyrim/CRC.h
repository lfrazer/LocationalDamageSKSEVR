#pragma once

#include "skse64_common/Relocation.h"


#ifdef SKYRIMVR
#define CALCULATE_CRC32_SIZE_ADDR	0x00C41310
#define CALCULATE_CRC32_32_ADDR		0x00C41370
#define CALCULATE_CRC32_64_ADDR		0x00C413F0
#else
#define CALCULATE_CRC32_SIZE_ADDR	0x00C06490
#define CALCULATE_CRC32_32_ADDR		0x00C064F0
#define CALCULATE_CRC32_64_ADDR		0x00C06570
#endif


template <class Ty>
class CRC32Calculator
{
public:
	inline CRC32Calculator() {}
	inline CRC32Calculator(const Ty &val) {
		operator=(val);
	}

	inline operator UInt32() {
		return m_checksum;
	}

protected:
	template <std::size_t SIZE = sizeof(Ty)>
	inline CRC32Calculator & operator=(const Ty &val) {
		typedef void(*Fn)(UInt32 *, const void *, UInt32);
		RelocAddr<Fn> fn(CALCULATE_CRC32_SIZE_ADDR); // CalculateCRC32_Size

		fn(&m_checksum, &val, SIZE); 
		return *this;
	}

	template <>
	inline CRC32Calculator & operator=<4>(const Ty &val) {
		typedef void(*Fn)(UInt32 *, Ty);
		RelocAddr<Fn> fn(CALCULATE_CRC32_32_ADDR); // CalculateCRC32_32

		fn(&m_checksum, val);
		return *this;
	}

	template <>
	inline CRC32Calculator & operator=<8>(const Ty &val) {
		typedef void(*Fn)(UInt32 *, Ty);
		RelocAddr<Fn> fn(CALCULATE_CRC32_64_ADDR); // CalculateCRC32_64

		fn(&m_checksum, val);
		return *this;
	}

	UInt32	m_checksum;
};

template <class Ty>
inline UInt32 CalcCRC32(const Ty &val) {
	CRC32Calculator<Ty> crc(val);
	return crc;
}
