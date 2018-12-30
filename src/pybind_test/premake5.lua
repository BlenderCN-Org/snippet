require("premake", ">=5.0.0-alpha11")

solution "example"
   location "generated"
   configurations { "Debug", "Release" }
   platforms {"x64"}

project "example"
   kind "SharedLib"
   targetextension ".pyd"
   language "C++"
   characterset "MBCS"
   includedirs{"$(PYTHON_DIR)/include"}
   libdirs{"$(PYTHON_DIR)/libs"}
   files {
     "**.hpp",
     "**.cpp",
	 "**.lua",
   }
