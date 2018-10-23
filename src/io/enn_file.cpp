/**
* Copyright (c) By zengqh.
*
* This program is just for fun or demo, in the hope that it  
* will be useful, you can redistribute it and/or modify freely.
*
* Time: 2013/02/19
* File: file.cpp
**/

#include <enn_file.h>

namespace enn
{
	static const char* openMode[] =
	{
		"rb",
		"wb",
		"w+b"
	};

	File::File()
		: mode_(FILE_READ)
		, handle_(0)
	{

	}

	File::File(const enn::String& fileName, FileMode mode)
	{
		Open(fileName, mode);
	}

	File::~File()
	{
		Close();
	}

	/// Read bytes from the file. Return number of bytes actually read.
	unsigned File::Read(void* dest, unsigned size)
	{
		if (mode_ == FILE_WRITE)
		{
			return 0;
		}

		if (size + position_ > size_)
			size = size_ - position_;
		if (!size) return 0;

		if (!handle_)
		{
			return 0;
		}

		size_t ret = fread(dest, size, 1, (FILE*)handle_);
		if (ret != 1)
		{
			// Return to the position where the read began
			fseek((FILE*)handle_, position_, SEEK_SET);
			return 0;
		}

		position_ += size;
		return size;
	}

	/// Set position from the beginning of the file.
	unsigned File::Seek(unsigned position)
	{
		if (mode_ == FILE_READ && position > size_) position = size_;

		if (!handle_)
		{
			return 0;
		}

		fseek((FILE*)handle_, position, SEEK_SET);
		position_ = position;
		return position_;
	}

	/// Write bytes to the file. Return number of bytes actually written.
	unsigned File::Write(const void* data, unsigned size)
	{
		if (mode_ == FILE_READ)
		{
			return 0;
		}

		if (!handle_)
		{
			return 0;
		}

		if (!size)
			return 0;

		if (fwrite(data, size, 1, (FILE*)handle_) != 1)
		{
			// Return to the position where the write began
			fseek((FILE*)handle_, position_, SEEK_SET);
			return 0;
		}

		position_ += size;
		if (position_ > size_)
			size_ = position_;

		return size;
	}

	/// Open a filesystem file. Return true if successful.
	bool File::Open(const enn::String& fileName, FileMode mode)
	{
		handle_ = fopen(fileName.c_str(), openMode[mode]);

		if (!handle_)
		{
			return false;
		}

		fileName_ = fileName;
		mode_ = mode;
		position_ = 0;

		fseek((FILE*)handle_, 0, SEEK_END);
		size_ = ftell((FILE*)handle_);
		fseek((FILE*)handle_, 0, SEEK_SET);
		return true;
	}

	/// Close the file.
	void File::Close()
	{
		if (handle_)
		{
			fclose((FILE*)handle_);
			handle_ = 0;
			position_ = 0;
			size_ = 0;
		}
	}
	/// Flush any buffered output to the file.
	void File::Flush()
	{
		if (handle_)
			fflush((FILE*)handle_);
	}
}