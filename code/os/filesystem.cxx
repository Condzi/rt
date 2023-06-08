namespace rt {
[[nodiscard]] Buffer
os_read_entire_file_or_panic(char const *path) {
  // @Note: we don't care about leaking resources on error, because system will
  //        close all handles for us.
  check_(path);

  // @Unicode: handle UTF-8 paths
  ::HANDLE file = ::CreateFileA(
                      (LPCSTR)path, 
                      GENERIC_READ, 
                      NULL,
                      0,
                      OPEN_EXISTING,
                      FILE_ATTRIBUTE_NORMAL,
                      NULL
                      );

  if (file == INVALID_HANDLE_VALUE) {
    errf("Failed to open file for reading '%s'.", path);
  }

  ::HANDLE mapping = ::CreateFileMappingA(
                        file,
                        NULL,
                        PAGE_READONLY,
                        0,
                        0,
                        NULL
                        );

  if (mapping == INVALID_HANDLE_VALUE) {
    errf("Failed to create mapping of file '%s'", path);
  }

  void *view = ::MapViewOfFile(
                  mapping, 
                  FILE_MAP_READ, 
                  0, 
                  0, 
                  0
                  );
                  
  if (view == NULL) {
    errf("Failed to map file '%s'", path);
  }
	
  ::LARGE_INTEGER file_sz_raw;
	::GetFileSizeEx(file, &file_sz_raw);
	s64 const file_sz = file_sz_raw.QuadPart;

	void *file_mem = alloc_temp(file_sz);
	::memcpy(file_mem, view, (u64)file_sz);

  ::UnmapViewOfFile(view);
  ::CloseHandle(mapping);
  ::CloseHandle(file);

	return {.count = file_sz, .bytes = (u8*)file_mem};
}

void
os_write_entire_file_or_panic(Buffer content, char const *path) {
  check_(path);
  check_(content.count >= 0);
  check_(content.bytes != NULL);

  // @Unicode: handle UTF-8 paths
  ::HANDLE file = ::CreateFileA(
                      (LPCSTR)path, 
                      GENERIC_WRITE, 
                      FILE_SHARE_READ,
                      0,
                      CREATE_ALWAYS,
                      FILE_ATTRIBUTE_NORMAL,
                      NULL
                      );

  if (file == INVALID_HANDLE_VALUE) {
    errf("Failed to open file for writing: '%s'", path);
  }
  
  ::DWORD const bytes_to_write = (::DWORD)content.count;
  ::DWORD bytes_written = 0;
  ::BOOL const success = ::WriteFile(
                           file, 
                           content.bytes, 
                           bytes_to_write, 
                           &bytes_written, 
                           NULL
                           );

  if (!success) {
    errf("Failed to write to file '%s'", path);
  }

  if (bytes_written != bytes_to_write) {
    errf("Partial write to '%s' (%zu/%zu)", path, bytes_written, bytes_to_write);
  }
  
  ::CloseHandle(file);
}

void
os_append_to_file_or_panic(Buffer content, char const *path) {
  check_(path);
  check_(content.count >= 0);
  check_(content.bytes != NULL);

  // @Unicode: handle UTF-8 paths
  ::HANDLE file = ::CreateFileA(
                      (LPCSTR)path, 
                      FILE_APPEND_DATA, // @Note: the only part that is different from write_entire_file 
                      FILE_SHARE_READ,
                      0,
                      CREATE_ALWAYS,
                      FILE_ATTRIBUTE_NORMAL,
                      NULL
                      );

  if (file == INVALID_HANDLE_VALUE) {
    errf("Failed to open file for appending: '%s'", path);
  }
  
  ::DWORD const bytes_to_write = (::DWORD)content.count;
  ::DWORD bytes_written = 0;
  ::BOOL const success = ::WriteFile(
                           file, 
                           content.bytes, 
                           bytes_to_write, 
                           &bytes_written, 
                           NULL
                           );

  if (!success) {
    errf("Failed to append to file '%s'", path);
  }

  if (bytes_written != bytes_to_write) {
    errf("Partial write (append) to '%s' (%zu/%zu)", path, bytes_written, bytes_to_write);
  }
  
  ::CloseHandle(file);
}

void
os_move_file_or_panic(char const *src, char const *dst) {
  check_(src);
  check_(dst);

  ::BOOL const success = ::MoveFileA(src, dst);

  if (!success) {
    errf("Failed to move file from '%s' to '%s'", src, dst);
  }
}
} // namespace rt