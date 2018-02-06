require("premake", ">=5.0.0-alpha11")

includedirs { "../thirdparty/eigen" }

solution "A"
   location "generated"
   configurations { "Debug", "Release" }
   platforms {"x64"}

project "A"
   kind "ConsoleApp"
   language "C++"
   characterset "MBCS"
   flags {
    "C++14"
   }
   files {
     "**.hpp",
     "**.cpp",
   }
