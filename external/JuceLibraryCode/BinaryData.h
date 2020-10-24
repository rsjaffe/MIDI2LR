/* =========================================================================================

   This is an auto-generated file: Any edits you make may be overwritten!

*/

#pragma once

namespace BinaryData
{
    extern const char*   NotoSansMediumplusThai_ttf;
    const int            NotoSansMediumplusThai_ttfSize = 562900;

    extern const char*   NotoSansJPMedium_otf;
    const int            NotoSansJPMedium_otfSize = 4495840;

    extern const char*   NotoSansKRMedium_otf;
    const int            NotoSansKRMedium_otfSize = 4847684;

    extern const char*   NotoSansSCMedium_otf;
    const int            NotoSansSCMedium_otfSize = 8363604;

    extern const char*   NotoSansTCMedium_otf;
    const int            NotoSansTCMedium_otfSize = 5925492;

    extern const char*   MIDI2LR_png;
    const int            MIDI2LR_pngSize = 6750;

    // Number of elements in the namedResourceList and originalFileNames arrays.
    const int namedResourceListSize = 6;

    // Points to the start of a list of resource names.
    extern const char* namedResourceList[];

    // Points to the start of a list of resource filenames.
    extern const char* originalFilenames[];

    // If you provide the name of one of the binary resource variables above, this function will
    // return the corresponding data and its size (or a null pointer if the name isn't found).
    const char* getNamedResource (const char* resourceNameUTF8, int& dataSizeInBytes);

    // If you provide the name of one of the binary resource variables above, this function will
    // return the corresponding original, non-mangled filename (or a null pointer if the name isn't found).
    const char* getNamedResourceOriginalFilename (const char* resourceNameUTF8);
}
