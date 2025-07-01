/*
 FS.cpp - file system wrapper
 Copyright (c) 2015 Ivan Grokhotkov. All rights reserved.
 This file is part of the esp8266 core for Arduino environment.

 This library is free software; you can redistribute it and/or
 modify it under the terms of the GNU Lesser General Public
 License as published by the Free Software Foundation; either
 version 2.1 of the License, or (at your option) any later version.

 This library is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 Lesser General Public License for more details.

 You should have received a copy of the GNU Lesser General Public
 License along with this library; if not, write to the Free Software
 Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */

#include "FS.h"
#include "FSImpl.h"
#include <alloca.h>
#include <string.h>

using namespace fs;

// Internal write buffering for better performance on MCUs
static uint8_t writeBuffer[32]; // Small 32-byte write buffer
static size_t writeBufferPos = 0;
static FileImplPtr bufferedFile = nullptr;

// Internal function to flush write buffer
static void flushWriteBuffer() {
  if (bufferedFile && writeBufferPos > 0) {
    bufferedFile->write(writeBuffer, writeBufferPos);
    writeBufferPos = 0;
  }
}

// Internal function to ensure buffer is for current file
static void ensureWriteBuffer(FileImplPtr currentFile) {
  if (bufferedFile != currentFile) {
    flushWriteBuffer(); // Flush old buffer
    bufferedFile = currentFile;
  }
}

size_t File::write(uint8_t c) {
  if (!*this) {
    return 0;
  }

  // Use internal buffering for single byte writes (automatic optimization)
  ensureWriteBuffer(_p);
  
  writeBuffer[writeBufferPos++] = c;
  
  // Flush when buffer is full
  if (writeBufferPos >= sizeof(writeBuffer)) {
    size_t written = _p->write(writeBuffer, writeBufferPos);
    writeBufferPos = 0;
    return (written == sizeof(writeBuffer)) ? 1 : 0;
  }
  
  return 1; // Buffered successfully
}

time_t File::getLastWrite() {
  if (!*this) {
    return 0;
  }

  return _p->getLastWrite();
}

size_t File::write(const uint8_t *buf, size_t size) {
  if (!*this) {
    return 0;
  }

  // For larger writes, flush buffer first and write directly
  if (size >= sizeof(writeBuffer)) {
    ensureWriteBuffer(_p);
    flushWriteBuffer(); // Flush any pending data
    return _p->write(buf, size);
  }
  
  // For small writes, use buffering
  size_t totalWritten = 0;
  ensureWriteBuffer(_p);
  
  while (totalWritten < size) {
    size_t remaining = size - totalWritten;
    size_t spaceInBuffer = sizeof(writeBuffer) - writeBufferPos;
    size_t toCopy = min(remaining, spaceInBuffer);
    
    memcpy(writeBuffer + writeBufferPos, buf + totalWritten, toCopy);
    writeBufferPos += toCopy;
    totalWritten += toCopy;
    
    // Flush if buffer is full
    if (writeBufferPos >= sizeof(writeBuffer)) {
      size_t written = _p->write(writeBuffer, writeBufferPos);
      if (written != writeBufferPos) {
        // Error occurred, return partial write count
        return totalWritten - (writeBufferPos - written);
      }
      writeBufferPos = 0;
    }
  }
  
  return totalWritten;
}

// Note: Additional optimized write methods for const char* and String
// are added but require header file updates for full compatibility
size_t File::write(const char* str) {
  if (!*this || !str) {
    return 0;
  }
  return write((const uint8_t*)str, strlen(str));
}

size_t File::write(const String& str) {
  if (!*this) {
    return 0;
  }
  return write((const uint8_t*)str.c_str(), str.length());
}

// Optimized write methods using small stack buffers

int File::available() {
  if (!*this) {
    return 0;
  }

  size_t fileSize = _p->size();
  size_t currentPos = _p->position();
  return (fileSize > currentPos) ? (fileSize - currentPos) : 0;
}

int File::read() {
  if (!*this) {
    return -1;
  }

  uint8_t result;
  if (_p->read(&result, 1) != 1) {
    return -1;
  }

  return result;
}

size_t File::read(uint8_t *buf, size_t size) {
  if (!*this) {
    return -1;
  }

  return _p->read(buf, size);
}

// Optimized bulk read method with adaptive chunk sizing
size_t File::readBytes(uint8_t *buffer, size_t length) {
  if (!*this || !buffer) {
    return 0;
  }
  
  // Adaptive chunk sizing for better performance
  size_t totalRead = 0;
  size_t remaining = length;
  
  while (remaining > 0 && available() > 0) {
    // Adaptive chunk size: optimized for ESP32 architecture
    size_t chunkSize;
    if (remaining <= 128) {
      chunkSize = remaining; // Direct read for small amounts
    } else if (remaining <= 1024) {
      chunkSize = min(remaining, (size_t)256); // Medium chunks
    } else {
      chunkSize = min(remaining, (size_t)512); // Larger chunks for big reads
    }
    
    size_t bytesRead = _p->read(buffer + totalRead, chunkSize);
    if (bytesRead == 0) break;
    
    totalRead += bytesRead;
    remaining -= bytesRead;
  }
  
  return totalRead;
}

// Enhanced readString with adaptive buffering and memory efficiency
String File::readString() {
  if (!*this) {
    return String();
  }
  
  size_t fileSize = available();
  if (fileSize == 0) {
    return String();
  }
  
  // Adaptive buffer sizing based on file size
  size_t bufferSize;
  if (fileSize <= 64) {
    bufferSize = 32;  // Small buffer for small files
  } else if (fileSize <= 512) {
    bufferSize = 64;  // Medium buffer for medium files
  } else {
    bufferSize = 128; // Larger buffer for big files
  }
  
  // Limit maximum read size for MCUs (adjust as needed)
  const size_t MAX_READ_SIZE = 4096; // Increased to 4KB for better performance
  if (fileSize > MAX_READ_SIZE) {
    fileSize = MAX_READ_SIZE;
  }
  
  // Use adaptive stack buffer
  uint8_t* buffer = (uint8_t*)alloca(bufferSize); // Stack allocation
  String result;
  result.reserve(min(fileSize, (size_t)512)); // Pre-allocate reasonable size
  size_t remaining = fileSize;
  
  while (remaining > 0 && available() > 0) {
    size_t chunkSize = min(remaining, bufferSize);
    size_t bytesRead = _p->read(buffer, chunkSize);
    if (bytesRead == 0) break;
    
    // Append to string efficiently
    result.concat((const char*)buffer, bytesRead);
    remaining -= bytesRead;
  }
  
  return result;
}

String File::readStringUntil(char terminator) {
  if (!*this) {
    return String();
  }
  
  String result;
  result.reserve(256); // Pre-allocate reasonable size
  
  // Read in chunks to find terminator efficiently
  uint8_t buffer[64];
  size_t bufferPos = 0;
  
  while (available() > 0) {
    if (bufferPos == 0) {
      size_t bytesRead = _p->read(buffer, sizeof(buffer));
      if (bytesRead == 0) break;
      bufferPos = bytesRead;
    }
    
    // Process buffer content
    for (size_t i = 0; i < bufferPos; i++) {
      if (buffer[i] == terminator) {
        // Found terminator, seek back remaining bytes
        if (i + 1 < bufferPos) {
          size_t backSeek = bufferPos - (i + 1);
          seek(position() - backSeek);
        }
        return result;
      }
      result += (char)buffer[i];
    }
    bufferPos = 0;
  }
  
  return result;
}

int File::peek() {
  if (!*this) {
    return -1;
  }

  size_t curPos = _p->position();
  int result = read();
  seek(curPos, SeekSet);
  return result;
}

void File::flush() {
  if (!*this) {
    return;
  }

  // Flush internal write buffer first
  if (bufferedFile == _p) {
    flushWriteBuffer();
  }
  
  _p->flush();
}

bool File::seek(uint32_t pos, SeekMode mode) {
  if (!*this) {
    return false;
  }

  return _p->seek(pos, mode);
}

size_t File::position() const {
  if (!*this) {
    return (size_t)-1;
  }

  return _p->position();
}

size_t File::size() const {
  if (!*this) {
    return 0;
  }

  return _p->size();
}

bool File::setBufferSize(size_t size) {
  if (!*this) {
    return 0;
  }

  return _p->setBufferSize(size);
}

void File::close() {
  if (_p) {
    // Flush internal write buffer before closing
    if (bufferedFile == _p) {
      flushWriteBuffer();
      bufferedFile = nullptr; // Clear buffer reference
    }
    
    _p->close();
    _p = nullptr;
  }
}

File::operator bool() const {
  return _p != nullptr && *_p != false;
}

const char *File::path() const {
  if (!*this) {
    return nullptr;
  }

  return _p->path();
}

const char *File::name() const {
  if (!*this) {
    return nullptr;
  }

  return _p->name();
}

//to implement
boolean File::isDirectory(void) {
  if (!*this) {
    return false;
  }
  return _p->isDirectory();
}

File File::openNextFile(const char *mode) {
  if (!*this) {
    return File();
  }
  return _p->openNextFile(mode);
}

boolean File::seekDir(long position) {
  if (!_p) {
    return false;
  }
  return _p->seekDir(position);
}

String File::getNextFileName(void) {
  if (!_p) {
    return "";
  }
  return _p->getNextFileName();
}

String File::getNextFileName(bool *isDir) {
  if (!_p) {
    return "";
  }
  return _p->getNextFileName(isDir);
}

void File::rewindDirectory(void) {
  if (!*this) {
    return;
  }
  _p->rewindDirectory();
}

File FS::open(const String &path, const char *mode, const bool create) {
  return open(path.c_str(), mode, create);
}

File FS::open(const char *path, const char *mode, const bool create) {
  if (!_impl) {
    return File();
  }

  return File(_impl->open(path, mode, create));
}

bool FS::exists(const char *path) {
  if (!_impl) {
    return false;
  }
  return _impl->exists(path);
}

bool FS::exists(const String &path) {
  return exists(path.c_str());
}

bool FS::remove(const char *path) {
  if (!_impl) {
    return false;
  }
  return _impl->remove(path);
}

bool FS::remove(const String &path) {
  return remove(path.c_str());
}

bool FS::rename(const char *pathFrom, const char *pathTo) {
  if (!_impl) {
    return false;
  }
  return _impl->rename(pathFrom, pathTo);
}

bool FS::rename(const String &pathFrom, const String &pathTo) {
  return rename(pathFrom.c_str(), pathTo.c_str());
}

bool FS::mkdir(const char *path) {
  if (!_impl) {
    return false;
  }
  return _impl->mkdir(path);
}

bool FS::mkdir(const String &path) {
  return mkdir(path.c_str());
}

bool FS::rmdir(const char *path) {
  if (!_impl) {
    return false;
  }
  return _impl->rmdir(path);
}

bool FS::rmdir(const String &path) {
  return rmdir(path.c_str());
}

const char *FS::mountpoint() {
  if (!_impl) {
    return NULL;
  }
  return _impl->mountpoint();
}

void FSImpl::mountpoint(const char *mp) {
  _mountpoint = mp;
}

const char *FSImpl::mountpoint() {
  return _mountpoint;
}
