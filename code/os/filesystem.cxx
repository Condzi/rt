namespace rt {
[[nodiscard]] Buffer
os_read_entire_file_or_panic(char const *path) {
  // @Note: we don't care about leaking resources on error, because system will
  //        close all handles for us.
  check_(path);

  // @Unicode: handle UTF-8 paths
  ::HANDLE file = ::CreateFileA((LPCSTR)path,
                                GENERIC_READ,
                                NULL,
                                0,
                                OPEN_EXISTING,
                                FILE_ATTRIBUTE_NORMAL,
                                NULL);

  if (file == INVALID_HANDLE_VALUE) {
    errf("Failed to open file for reading '%s'.", path);
  }

  ::HANDLE mapping = ::CreateFileMappingA(file, NULL, PAGE_READONLY, 0, 0, NULL);

  if (mapping == INVALID_HANDLE_VALUE) {
    errf("Failed to create mapping of file '%s'", path);
  }

  void *view = ::MapViewOfFile(mapping, FILE_MAP_READ, 0, 0, 0);

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

  return {.count = file_sz, .bytes = (u8 *)file_mem};
}

void
os_write_entire_file_or_panic(Buffer content, char const *path) {
  check_(path);
  check_(content.count >= 0);
  check_(content.bytes != NULL);

  // @Unicode: handle UTF-8 paths
  ::HANDLE file = ::CreateFileA((LPCSTR)path,
                                GENERIC_WRITE,
                                FILE_SHARE_READ,
                                0,
                                CREATE_ALWAYS,
                                FILE_ATTRIBUTE_NORMAL,
                                NULL);

  if (file == INVALID_HANDLE_VALUE) {
    errf("Failed to open file for writing: '%s'", path);
  }

  ::DWORD const bytes_to_write = (::DWORD)content.count;
  ::DWORD       bytes_written  = 0;
  ::BOOL const  success =
      ::WriteFile(file, content.bytes, bytes_to_write, &bytes_written, NULL);

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
  ::HANDLE file = ::CreateFileA((LPCSTR)path,
                                FILE_APPEND_DATA, // @Note: the only part that is
                                                  // different from write_entire_file
                                FILE_SHARE_READ,
                                0,
                                CREATE_ALWAYS,
                                FILE_ATTRIBUTE_NORMAL,
                                NULL);

  if (file == INVALID_HANDLE_VALUE) {
    errf("Failed to open file for appending: '%s'", path);
  }

  ::DWORD const bytes_to_write = (::DWORD)content.count;
  ::DWORD       bytes_written  = 0;
  ::BOOL const  success =
      ::WriteFile(file, content.bytes, bytes_to_write, &bytes_written, NULL);

  if (!success) {
    errf("Failed to append to file '%s'", path);
  }

  if (bytes_written != bytes_to_write) {
    errf("Partial write (append) to '%s' (%zu/%zu)",
         path,
         bytes_written,
         bytes_to_write);
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

// @Note: we assume/ensure that the paths are *not* ended with \ or /.
struct Path_Cache {
  String cwd;
  String logs;
  String data;
  String shaders;
  String assets;
  String textures;
  String models;
} static gPath_Cache;

void
os_init_filesystem() {
#ifdef HANDMADE_INTERNAL
  // @Unicode
  char run_tree_dir[] = "W:\\pwr\\engineering-thesis\\rt\\run_tree";
  if (0 == ::SetCurrentDirectoryA(run_tree_dir)) {
    logf("!!! Failed to set the CWD to '%s'", run_tree_dir);
  }
#endif

  // Get the CWD
  // @Unicode!!!
  {
    ::DWORD cwd_len = ::GetCurrentDirectoryA(0, NULL);
    char   *cwd     = perm<char>(cwd_len);
    ::DWORD status  = ::GetCurrentDirectoryA(cwd_len, cwd);

    if (status == 0) {
      logf("Failed to get current working directory!\n");
      gPath_Cache.cwd = tprint(".\\");
    } else {
      // Cut off the trailing slash (directory separator)
      if (cwd[cwd_len - 1] == '\\') {
        cwd_len--;
      }

      gPath_Cache.cwd.data  = cwd;
      gPath_Cache.cwd.count = cwd_len - 1; // Remove the '\0'.
    }
  }

  gPath_Cache.logs     = pathf("%c\\logs");
  gPath_Cache.data     = pathf("%c\\data");
  gPath_Cache.shaders  = pathf("%d\\shaders");
  gPath_Cache.assets   = pathf("%c\\assets");
  gPath_Cache.textures = pathf("%a\\textures");
  gPath_Cache.models   = pathf("%a\\models");

  logf("Path cache initialized. Contents:\n");
  logf("\t     cwd='%.*s'\n", (int)gPath_Cache.cwd.count, gPath_Cache.cwd.data);
  logf("\t    logs='%.*s'\n", (int)gPath_Cache.logs.count, gPath_Cache.logs.data);
  logf("\t    data='%.*s'\n", (int)gPath_Cache.data.count, gPath_Cache.data.data);
  logf("\t shaders='%.*s'\n",
       (int)gPath_Cache.shaders.count,
       gPath_Cache.shaders.data);
  logf("\t  assets='%.*s'\n", (int)gPath_Cache.assets.count, gPath_Cache.assets.data);
  logf("\ttextures='%.*s'\n",
       (int)gPath_Cache.textures.count,
       gPath_Cache.textures.data);
  logf("\t  models='%.*s'\n", (int)gPath_Cache.models.count, gPath_Cache.models.data);
}

[[nodiscard]] String
pathf(char const *fmt) {
  check_(fmt != NULL);

  // Pretty naive, but does the work
  // @Unsafe cast
  s32 const      fmt_len = (s32)::strlen(fmt);
  String_Builder sb;
  for (s32 i = 0; i < fmt_len;) {
    if (fmt[i] != '%') {
      // @Unsafe: because `data` is char*, we have to remove the constness.
      char cpy = fmt[i];
      append(sb, String {.count = 1, .data = &cpy});
      i++;
      continue;
    }

    if (i + 1 == fmt_len) break;

    switch (fmt[i + 1]) {
      case 'c': {
        append(sb, gPath_Cache.cwd);
      } break;

      case 'l': {
        append(sb, gPath_Cache.logs);
      } break;

      case 'd': {
        append(sb, gPath_Cache.data);
      } break;

      case 'S': {
        append(sb, gPath_Cache.shaders);
      } break;

      case 'a': {
        append(sb, gPath_Cache.assets);
      } break;

      case 't': {
        append(sb, gPath_Cache.textures);
      } break;

      case 'm': {
        append(sb, gPath_Cache.models);
      } break;

      default: {
        errf("pathf: unknown specifier '%%%c'.", fmt[i + 1]);
      } break;
    }

    i += 2;
  }

  return to_perm_string(sb);
}
} // namespace rt
