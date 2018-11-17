require("premake", ">=5.0.0-alpha11")

includedirs {
  "../../thirdparty/matplotlib-cpp",
  "C:/Python27/include"
}

libdirs {
  "C:/Python27/libs"
}

links {
  "python27.lib"
}


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
