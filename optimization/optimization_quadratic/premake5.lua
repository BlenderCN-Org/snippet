require("premake", ">=5.0.0-alpha11")

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

   sysincludedirs {
     "../thirdparty/eigen/",
     "C:/Users/ozawa/Desktop/octave-4.2.1/include/octave-4.2.1/"
   }
