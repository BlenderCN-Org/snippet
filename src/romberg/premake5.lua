require("premake", ">=5.0.0-alpha12")

includedirs { "../thirdparty/eigen" }

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
   
   filter "configurations:Release"
     optimize "Speed"
