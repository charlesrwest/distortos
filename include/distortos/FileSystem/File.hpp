/**
 * \file
 * \brief File class header
 *
 * \author Copyright (C) 2018 Kamil Szczygiel http://www.distortec.com http://www.freddiechopin.info
 *
 * \par License
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
 * distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef INCLUDE_DISTORTOS_FILESYSTEM_FILE_HPP_
#define INCLUDE_DISTORTOS_FILESYSTEM_FILE_HPP_

#include <sys/stat.h>

#include <utility>

namespace distortos
{

/**
 * File class is an interface for a file.
 *
 * \ingroup fileSystem
 */

class File
{
public:

	/// seek() mode of operation
	enum class Whence
	{
		/// set file offset to provided argument, similar to `SEEK_SET`
		beginning,
		/// set file offset to its current value plus provided argument, similar to `SEEK_CUR`
		current,
		/// set file offset to the size of the file plus provided argument, similar to `SEEK_END`
		end,
	};

	/**
	 * \brief File's destructor
	 */

	virtual ~File() = default;

	/**
	 * \brief Closes file.
	 *
	 * Similar to [close()](http://pubs.opengroup.org/onlinepubs/9699919799/functions/close.html)
	 *
	 * \return 0 on success, error code otherwise:
	 * - EBADF - the file is already closed;
	 */

	virtual int close() = 0;

	/**
	 * \brief Returns current file offset.
	 *
	 * Similar to [ftello()](http://pubs.opengroup.org/onlinepubs/9699919799/functions/ftell.html)
	 *
	 * \return pair with return code (0 on success, error code otherwise) and current file offset, bytes; error codes:
	 * - EBADF - the file is not opened;
	 */

	virtual std::pair<int, off_t> getPosition() = 0;

	/**
	 * \brief Returns size of file.
	 *
	 * \return pair with return code (0 on success, error code otherwise) and size of file, bytes; error codes:
	 * - EBADF - the file is not opened;
	 */

	virtual std::pair<int, off_t> getSize() = 0;

	/**
	 * \brief Returns status of file.
	 *
	 * Similar to [fstat()](http://pubs.opengroup.org/onlinepubs/9699919799/functions/fstat.html)
	 *
	 * \return pair with return code (0 on success, error code otherwise) and status of file in `stat` struct; error
	 * codes:
	 * - EBADF - the file is not opened;
	 */

	virtual std::pair<int, struct stat> getStatus() = 0;

	/**
	 * \brief Tells whether the file is a terminal.
	 *
	 * Similar to [isatty()](http://pubs.opengroup.org/onlinepubs/9699919799/functions/isatty.html)
	 *
	 * \return pair with return code (0 on success, error code otherwise) and bool telling whether the file is a
	 * terminal (true) or not (false); error codes:
	 * - EBADF - the file is not opened;
	 */

	virtual std::pair<int, bool> isATerminal() = 0;

	/**
	 * \brief Locks the file for exclusive use by current thread.
	 *
	 * When the object is locked, any call to any member function from other thread will be blocked until the object is
	 * unlocked. Locking is optional, but may be useful when more than one operation must be done atomically.
	 *
	 * \note Locks are recursive.
	 *
	 * \warning This function must not be called from interrupt context!
	 *
	 * \return 0 on success, error code otherwise:
	 * - EAGAIN - the lock could not be acquired because the maximum number of recursive locks for file has been
	 * exceeded;
	 */

	virtual int lock() = 0;

	/**
	 * \brief Reads data from file.
	 *
	 * Similar to [read()](http://pubs.opengroup.org/onlinepubs/9699919799/functions/read.html)
	 *
	 * \param [out] buffer is the buffer into which the data will be read
	 * \param [in] size is the size of \a buffer, bytes
	 *
	 * \return pair with return code (0 on success, error code otherwise) and number of read bytes (valid even when
	 * error code is returned); error codes:
	 * - EBADF - the file is not opened or opened only for writing;
	 * - EINVAL - \a buffer is not valid;
	 */

	virtual std::pair<int, size_t> read(void* buffer, size_t size) = 0;

	/**
	 * \brief Resets current file offset.
	 *
	 * Similar to [rewind()](http://pubs.opengroup.org/onlinepubs/9699919799/functions/rewind.html)
	 *
	 * \return 0 on success, error code otherwise:
	 * - EBADF - the file is not opened;
	 */

	virtual int rewind() = 0;

	/**
	 * \brief Moves file offset.
	 *
	 * Similar to [lseek()](http://pubs.opengroup.org/onlinepubs/9699919799/functions/lseek.html)
	 *
	 * \param [in] whence selects the mode of operation: `Whence::beginning` will set file offset to \a offset,
	 * `Whence::current` will set file offset to its current value plus \a offset, `Whence::end` will set file offset to
	 * the size of the file plus \a offset
	 * \param [in] offset is the value of offset, bytes
	 *
	 * \return pair with return code (0 on success, error code otherwise) and current file offset, bytes; error codes:
	 * - EBADF - the file is not opened;
	 * - EINVAL - resulting file offset would be negative;
	 */

	virtual std::pair<int, off_t> seek(Whence whence, off_t offset) = 0;

	/**
	 * \brief Synchronizes state of a file, ensuring all cached writes are finished.
	 *
	 * Similar to [fsync()](http://pubs.opengroup.org/onlinepubs/9699919799/functions/fsync.html)
	 *
	 * \return 0 on success, error code otherwise:
	 * - EBADF - the file is not opened;
	 */

	virtual int synchronize() = 0;

	/**
	 * \brief Unlocks the file which was previously locked by current thread.
	 *
	 * \note Locks are recursive.
	 *
	 * \warning This function must not be called from interrupt context!
	 *
	 * \return 0 on success, error code otherwise:
	 * - EPERM - current thread did not lock the file;
	 */

	virtual int unlock() = 0;

	/**
	 * \brief Writes data to file.
	 *
	 * Similar to [write()](http://pubs.opengroup.org/onlinepubs/9699919799/functions/write.html)
	 *
	 * \param [in] buffer is the buffer with data that will be written
	 * \param [in] size is the size of \a buffer, bytes
	 *
	 * \return pair with return code (0 on success, error code otherwise) and number of written bytes (valid even when
	 * error code is returned); error codes:
	 * - EBADF - the file is not opened or opened only for reading;
	 * - EINVAL - \a buffer is not valid;
	 * - ENOSPC - no space left on the device containing the file;
	 */

	virtual std::pair<int, size_t> write(const void* buffer, size_t size) = 0;

	File() = default;
	File(const File&) = delete;
	File& operator=(const File&) = delete;
};

}	// namespace distortos

#endif	// INCLUDE_DISTORTOS_FILESYSTEM_FILE_HPP_
