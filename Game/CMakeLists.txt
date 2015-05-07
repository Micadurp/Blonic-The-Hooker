# CMake settings
cmake_minimum_required(VERSION 3.1)

# Root project settings
set(PROJECT_NAME Game)
project(${PROJECT_NAME})


# Set the files used in the target
file(GLOB_RECURSE HEADERS Headers/ *.h)
file(GLOB_RECURSE VS Headers/ *VertexShader.hlsl)
file(GLOB_RECURSE GS Headers/ *GeometryShader.hlsl)
file(GLOB_RECURSE PS Headers/ *PixelShader.hlsl)
file(GLOB_RECURSE SOURCES Source/ *.cpp)
include_directories(Include)

create_source_group("Header Files" "${CMAKE_CURRENT_SOURCE_DIR}/Include" ${HEADERS} ${VS} ${GS} ${PS})
create_source_group("Source Files" "${CMAKE_CURRENT_SOURCE_DIR}/Source" ${SOURCES})
set(LIBRARIES ${EXTERNAL_LIBRARIES})
link_directories(${OUTPUT_FOLDER}) # lib files

SET_SOURCE_FILES_PROPERTIES( ${VS} PROPERTIES VS_SHADER_TYPE Vertex VS_SHADER_MODEL 5.0 VS_SHADER_ENTRYPOINT VS_main )
SET_SOURCE_FILES_PROPERTIES( ${GS} PROPERTIES VS_SHADER_TYPE Geometry VS_SHADER_MODEL 5.0 VS_SHADER_ENTRYPOINT GS_main )
SET_SOURCE_FILES_PROPERTIES( ${PS} PROPERTIES VS_SHADER_TYPE Pixel VS_SHADER_MODEL 5.0 VS_SHADER_ENTRYPOINT PS_main )

# Add the target
add_executable(${PROJECT_NAME} ${HEADERS} ${VS} ${GS} ${PS} ${SOURCES})
target_link_libraries(${PROJECT_NAME} ${LIBRARIES})

# Set SUBSYSTEM:CONSOLE
set_target_properties(${PROJECT_NAME} PROPERTIES LINK_FLAGS "/SUBSYSTEM:WINDOWS")