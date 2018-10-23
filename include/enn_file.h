/**
* Copyright (c) By zengqh.
*
* This program is just for fun or demo, in the hope that it  
* will be useful, you can redistribute it and/or modify freely.
*
* Time: 2013/02/18
* File: file.h
* Blog: http://www.cnblogs.com/zengqh/
**/

#pragma once

#include <enn_deserializer.h>
#include <enn_serializer.h>

enum FileMode
{
	FILE_READ = 0,
	FILE_WRITE,
	FILE_READWRITE
};

namespace enn
{
	class File : public Deserializer, public Serializer
	{
	public:
		File();
		File(const enn::String& fileName, FileMode mode = FILE_READ);
		~File();

		/// Read bytes from the file. Return number of bytes actually read.
		virtual unsigned Read(void* dest, unsigned size);

		/// Set position from the beginning of the file.
		virtual unsigned Seek(unsigned position);

		/// Write bytes to the file. Return number of bytes actually written.
		virtual unsigned Write(const void* data, unsigned size);

		/// Open a filesystem file. Return true if successful.
		bool Open(const enn::String& fileName, FileMode mode = FILE_READ);

		/// Close the file.
		void Close();
		/// Flush any buffered output to the file.
		void Flush();

		/// Return the open mode.
		FileMode GetMode() const { return mode_; }
		/// Return whether is open.
		bool IsOpen() const { return handle_ != 0; }
		/// Return the file handle.
		void* GetHandle() const { return handle_; }

		const enn::String& GetName() const { return fileName_; }

	protected:
		/// File name.
		enn::String fileName_;
		/// Open mode.
		FileMode mode_;
		/// File handle.
		void* handle_;
	};
}