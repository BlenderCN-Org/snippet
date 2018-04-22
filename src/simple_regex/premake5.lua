require("premake", ">=5.0.0-alpha11")

solution "A"
   location "generated"
   configurations { "Debug", "Release" }
   platforms {"x64"}

project "A"
   kind "ConsoleApp"
   language "C++"
   characterset "MBCS"
   files {
     "**.hpp",
     "**.cpp",
   }
   flags {"C++11"}
