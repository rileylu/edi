#pragma once

#include "noncopyable.h"
#include <string>
#include <array>
namespace buffer
{
	const size_t SMALL = 4000;
	const size_t BIG = 4000 * 1000;

	template<size_t N>
	class FixedBuffer :Noncopyable
	{
	public:
		using Iterator = typename std::array<char, N>::iterator;
		using CIterator = typename std::array<char, N>::const_iterator;
		FixedBuffer() :
			cur_(data_.data())
		{}
		~FixedBuffer() = default;
		void append(const std::string& str)
		{
			if (avail() > str.length())
			{
				std::copy(str.cbegin(), str.cend(), cur_);
				cur_ += str.length();
			}
		}
		void append(std::string&& str)
		{
			if (avail() > str.length())
			{
				std::move(str.begin(), str.end(), cur_);
				cur_ += str.length();
				str.clear();
			}
		}
		size_t avail() const
		{
			return data_.cend() - cur_;
		}

		Iterator current()
		{
			return cur_;
		}
		CIterator current() const
		{
			return cur_;
		}

		void add(size_t len)
		{
			cur_ += len;
		}
		void reset()
		{
			cur_ = data_.begin();
		}
		void bzero()
		{
			data_.fill(0);
		}
		size_t length() const
		{
			cur_ - data_.begin();
		}

		std::string toString() const
		{
			return std::string(data_.begin(), data_.begin()+length());
		}

	private:
		std::array<char, N> data_;
		Iterator cur_;
	};
}
