# CMakeList.txt: CMake-Projekt für "Pong". Schließen Sie die Quelle ein, und definieren Sie
# projektspezifische Logik hier.
#
cmake_minimum_required (VERSION 3.8)

# Aktivieren Sie Hot Reload für MSVC-Compiler, sofern unterstützt.
if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

project ("Pong")

# Fügen Sie der ausführbaren Datei dieses Projekts eine Quelle hinzu.
add_executable (Pong "Pong.cpp" "Pong.h" "Text.h")

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET Pong PROPERTY CXX_STANDARD 20)
endif()

set(LIBDIR "${PROJECT_SOURCE_DIR}/lib")
set(SDL2_LIBRARIES "${LIBDIR}/SDL2.lib;${LIBDIR}/SDL2main.lib;${LIBDIR}/SDL2test.lib;${LIBDIR}/SDL2_ttf.lib")

include_directories(${PROJECT_SOURCE_DIR}/include)
target_link_libraries(Pong ${SDL2_LIBRARIES})

# TODO: Fügen Sie bei Bedarf Tests hinzu, und installieren Sie Ziele.
