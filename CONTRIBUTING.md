# Contributing to MIDI2LR

Looking to contribute something to MIDI2LR? **Here's how you can help.**

## Reporting issues

On Github, we only accept issues that are bug reports or feature requests. Bugs must be isolated and reproducible problems that we can fix within the MIDI2LR code. Lightroom bugs and questions/concerns about the MIDI2LR program may be discussed in our [MIDI2LR Group Page](https://groups.google.com/forum/#!forum/midi2lr). Please read the following guidelines before opening any issue.

1. **Search for existing issues.** We get duplicate issues, and you'd help us out a lot by first checking if someone else has reported the same issue. Moreover, the issue may have already been resolved with a fix available. Also check out the [*known issues* section](https://github.com/rsjaffe/MIDI2LR/wiki#faqknown-issues) of the wiki.
2. **Create an isolated and reproducible test case.** Be sure the problem exists in MIDI2LR's code with a description of the steps to replicate the bug that should be included in each bug report.
3. **Share as much information as possible.** Include operating system and version, Lightroom version, version of MIDI2LR, etc. where appropriate. Also include steps to reproduce the bug.

## Key branches

- `master` is the latest, deployed version.
- `gh-pages` is the hosted docs (not to be used for pull requests).
- `develop` is the official work in progress branch for the next release.

## Pull requests

- Submit pull requests against the `develop` branch for easier merging.

## Coding standards
- For C++ and Objective-C++ files, use the formatting rules in `.clang-format`.
- Comments are C-style: /* comment here */, even for single-line comments. The exceptions are comments placed automatically by programs (e.g., static analyzer skip-error comments).
- Names and order of includes in C++ files follows the [Google C++ Style Guide](https://google.github.io/styleguide/cppguide.html#Names_and_Order_of_Includes).
- Use braced initialization whenever possible (which means almost always).
- Naming rules in C++:
  - UpperCamelCase: Classes, Structs, Enums, Unions, Template parameters, Global functions, Class and struct methods, Typedefs, Properties
  - all_lower: Parameters, Local variables, Global variables, Class and struct public fields, Union members, Namespaces
  - all_lower_: Protected and private class and struct fields
  - kUpperCamelCase: Enum members, Global constants, Other constants
  - ALL_UPPER: Macros
- For Lua files, emulate the formatting in current files. Avoid adding code to the message loops in Client.lua unless absolutely necessary, as those changes can slow down Lightroom
- If adding something that is stored in the Preferences, there are four functions needed: StartDialog, EndDialog, Loaded..., UseDefaults.... The Loaded... and UseDefaults... functions must be in *Init.lua*, exported, and used in the LoadedAll and UseDefaultsAll functions in *Init.lua*.

## Typical process for adding new types of actions
- **If storage in Preferences is needed or if the function requires new extensive code**, write new .lua file with specialized code.
- **If using Preferences**, add *Loaded* and *Default* methods to *Init.lua* to handle preference file loading. Add menu building code to the .lua file for these actions and integrate into *Options.lua*.
- Integrate commands into Client.lua and **require** new lua file (if new lua file written).
- Add actions to *Database.lua* and regenerate the database in Lightroom by deleting the existing MenuTrans.xml and running Lightroom.
- Using the *Build files (development use only)* option in the MIDI2LR menu generate .md file for the wiki.

## Versioning

Version number is in the style *major.minor.maintenance.build* and needs to be updated in four places:

- The Projucer program: project version property 
- Info.lua: VERSION
- Midi2lr.Doxyfile: PROJECT_NUMBER
- Installer/MIDI2LR.xml: \<version\>\</version\>

## Notarizing for MacOS

Both the app and the installer need to be notarized for MacOS distribution. Use XCode to notarize and staple application. To notarize and staple installer, run (substitute correct username, password, and MIDI2LR version number)
`xcrun altool --notarize-app --primary-bundle-id "com.rsjaffe.MIDI2LR.installer" --username "InsertUsernameHere" --password "InsertPasswordHere" --file MIDI2LR-W.X.Y.Z-osx-installer.dmg`

After successful notarization, run (substitute correct MIDI2LR version number)
`xcrun stapler staple MIDI2LR-W.X.Y.Z-osx-installer.dmg`

## License

By contributing your code, you agree to license your contribution under the terms of the GPLv3: https://github.com/rsjaffe/MIDI2LR/blob/master/LICENSE.txt.
