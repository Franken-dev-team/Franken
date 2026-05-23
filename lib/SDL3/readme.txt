Select one of the three folders that corresponds to your operating system (OS) and copy SDL3.dll and paste it next to your .exe.

One Important Note: You must change the link in CMakeLists if your operating system is not x64. Change it in the section

if(WIN32)
target_link_libraries(CluxeEditor PRIVATE "${CMAKE_SOURCE_DIR}/lib/x64/SDL3.lib")

It's located at line 46 of CMakeLists.txt.