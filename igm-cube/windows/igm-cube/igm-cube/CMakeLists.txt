# CMakeList.txt: proyecto de CMake para OpenGLTest, incluya el origen y defina
# la lógica específica del proyecto aquí.
#
cmake_minimum_required (VERSION 3.8)

# Agregue un origen al ejecutable de este proyecto.
add_executable (igm-cube "spinningcube.cpp" "stb_image.h")

# Set the path to the GLEW directory
set(GLEW_DIR ${CMAKE_SOURCE_DIR}/thirdParty/GLEW)
# Specify the path to GLEW.lib
set(GLEW_LIBRARY ${GLEW_DIR}/lib/Release/x64/glew32.lib)

set(GLFW_DIR ${CMAKE_SOURCE_DIR}/thirdParty/GLFW)
# Specify the path to GLFW.lib
set(GLFW_LIBRARY ${GLFW_DIR}/lib/glfw3.lib)

set(GLM_DIR ${CMAKE_SOURCE_DIR}/thirdParty/GLM)
# Specify the path to GLM.lib
set(GLM_LIBRARY ${CMAKE_SOURCE_DIR}/thirdParty/GLM/GLM/lib/glm.lib)

# Set the path to the GLEW directory
set(GLEW_INCLUDE_DIR ${GLEW_DIR}/include)

set(GLFW_INCLUDE_DIR ${GLFW_DIR}/include)

set(RESOURCES_DIR ${CMAKE_SOURCE_DIR}/resources)

target_compile_definitions(igm-cube PRIVATE RESOURCES_DIR="${RESOURCES_DIR}")

# Include GLEW GLFW headers
include_directories(${GLEW_INCLUDE_DIR} ${GLFW_INCLUDE_DIR} ${GLM_DIR})

# Link GLEW GLFW libraries
target_link_libraries(igm-cube PRIVATE ${GLEW_LIBRARY} ${GLFW_LIBRARY} ${GLM_LIBRARY} opengl32)

# Manually specify the GLEW library
find_library(GLEW_LIBRARY NAMES glew32 PATHS ${GLEW_DIR}/lib/Release/x64 NO_DEFAULT_PATH)

if (NOT GLEW_LIBRARY)
  message(FATAL_ERROR "GLEW library not found. Set the correct path to GLEW library.")
endif()


# Specify additional DLLs
set(ADDITIONAL_DLLS
    "${GLEW_DIR}/bin/x64/glew32.dll"
    "${GLFW_DIR}/lib/glfw3.dll"
)

# Copy DLLs to the build directory after compilation
foreach(DLL ${ADDITIONAL_DLLS})
    add_custom_command(TARGET igm-cube POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${DLL}"
            $<TARGET_FILE_DIR:igm-cube>)
endforeach()

# TODO: Agregue pruebas y destinos de instalación si es necesario.