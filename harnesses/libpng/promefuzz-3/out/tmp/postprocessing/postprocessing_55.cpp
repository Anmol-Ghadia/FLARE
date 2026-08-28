#include <cstdint>
#include <cstddef>
#include <cstdio>
#include <cstdlib>
#include <png.h>

static void PngErrorFn(png_structp png_ptr, png_const_charp)
{
  longjmp(png_jmpbuf(png_ptr), 1);
}

static void PngWarningFn(png_structp, png_const_charp)
{
}

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size)
{
  FILE *fp = std::tmpfile();
  if (fp == nullptr)
    return 0;

  if (Size > 0)
    std::fwrite(Data, 1, Size, fp);
  std::rewind(fp);

  png_structp png_ptr =
      png_create_read_struct(PNG_LIBPNG_VER_STRING, nullptr, PngErrorFn, PngWarningFn);
  if (png_ptr == nullptr)
  {
    std::fclose(fp);
    return 0;
  }

  png_infop info_ptr = png_create_info_struct(png_ptr);
  if (info_ptr == nullptr)
  {
    png_destroy_read_struct(&png_ptr, nullptr, nullptr);
    std::fclose(fp);
    return 0;
  }

  png_infop end_info = png_create_info_struct(png_ptr);
  if (end_info == nullptr)
  {
    png_destroy_read_struct(&png_ptr, &info_ptr, nullptr);
    std::fclose(fp);
    return 0;
  }

  if (setjmp(png_jmpbuf(png_ptr)))
  {
    png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
    std::fclose(fp);
    return 0;
  }

  png_init_io(png_ptr, fp);

  if (Size < 8 || png_sig_cmp(const_cast<png_bytep>(reinterpret_cast<const png_byte*>(Data)), 0, 8) != 0)
  {
    png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
    std::fclose(fp);
    return 0;
  }

  png_set_sig_bytes(png_ptr, 0);
  png_read_info(png_ptr, info_ptr);

  png_uint_32 flags_to_check[] = {
      PNG_INFO_pCAL,
      PNG_INFO_tIME,
      PNG_INFO_iCCP,
      PNG_INFO_eXIf,
      PNG_INFO_PLTE,
      PNG_INFO_tRNS,
      PNG_INFO_gAMA,
      PNG_INFO_sBIT
  };

  for (size_t i = 0; i < sizeof(flags_to_check) / sizeof(flags_to_check[0]); ++i)
  {
    (void)png_get_valid(png_ptr, info_ptr, flags_to_check[i]);
  }

  png_charp purpose = nullptr;
  png_int_32 X0 = 0;
  png_int_32 X1 = 0;
  int type = 0;
  int nparams = 0;
  png_charp units = nullptr;
  png_charpp params = nullptr;
  (void)png_get_pCAL(png_ptr, info_ptr, &purpose, &X0, &X1, &type, &nparams, &units, &params);

  png_timep mod_time = nullptr;
  (void)png_get_tIME(png_ptr, info_ptr, &mod_time);

  png_charp iccp_name = nullptr;
  int compression_type = 0;
  png_bytep profile = nullptr;
  png_uint_32 proflen = 0;
  (void)png_get_iCCP(png_ptr, info_ptr, &iccp_name, &compression_type, &profile, &proflen);

  png_uint_32 num_exif = 0;
  png_bytep exif = nullptr;
  (void)png_get_eXIf_1(png_ptr, info_ptr, &num_exif, &exif);

  (void)png_get_valid(png_ptr, info_ptr, PNG_INFO_pCAL | PNG_INFO_tIME);
  (void)png_get_valid(png_ptr, info_ptr, PNG_INFO_iCCP | PNG_INFO_eXIf);

  png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
  std::fclose(fp);
  return 0;
}