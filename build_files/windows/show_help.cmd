echo.
echo Convenience targets
echo - release ^(identical to the official blender.org builds^)
echo - full ^(same as release minus the cuda kernels^)
echo - lite
echo - headless
echo - cycles
echo - bpy
echo.
echo Utilities ^(not associated with building^)
echo - clean ^(Target must be set^)
echo - update
echo - nobuild ^(only generate project files^)
echo - showhash ^(Show git hashes of source tree^)
echo.
echo Configuration options
echo - verbose ^(enable diagnostic output during configuration^)
echo - with_tests ^(enable building unit tests^)
echo - nobuildinfo ^(disable buildinfo^)
echo - noge ^(disable building game engine and player^)
echo - debug ^(Build an unoptimized debuggable build^)
echo - packagename [newname] ^(override default cpack package name^)
echo - builddir [newdir] ^(override default build folder^)
echo - 2017 ^(build with visual studio 2017^)
echo - 2017pre ^(build with visual studio 2017 pre-release^)
echo - 2017b ^(build with visual studio 2017 Build Tools^)
echo - 2019 ^(build with visual studio 2019^)
echo - 2019pre ^(build with visual studio 2019 pre-release^)
echo - 2019b ^(build with visual studio 2019 Build Tools^)

echo.
echo Experimental options
echo - clang ^(enable building with clang^)
echo - asan ^(enable asan when building with clang^)
echo - ninja ^(enable building with ninja instead of msbuild^)
echo.
