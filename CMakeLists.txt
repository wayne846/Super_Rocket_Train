cmake_minimum_required(VERSION 2.8)

project(RollerCoasters)
set(SRC_DIR ${PROJECT_SOURCE_DIR}/src/)
set(INCLUDE_DIR ${PROJECT_SOURCE_DIR}/include/)
set(LIB_DIR ${PROJECT_SOURCE_DIR}/lib/)

set(SRC_SHADER 
    ${PROJECT_SOURCE_DIR}/assets/shaders/simpleObject.vert
    ${PROJECT_SOURCE_DIR}/assets/shaders/simpleObject.frag
    ${PROJECT_SOURCE_DIR}/assets/shaders/instanceObject.vert
    ${PROJECT_SOURCE_DIR}/assets/shaders/smoke.vert
    ${PROJECT_SOURCE_DIR}/assets/shaders/smoke.frag
    ${PROJECT_SOURCE_DIR}/assets/shaders/water.vert
    ${PROJECT_SOURCE_DIR}/assets/shaders/water.frag
    ${PROJECT_SOURCE_DIR}/assets/shaders/model_loading.vert
    ${PROJECT_SOURCE_DIR}/assets/shaders/model_loading.frag
    ${PROJECT_SOURCE_DIR}/assets/shaders/particle.vert
    ${PROJECT_SOURCE_DIR}/assets/shaders/particle.frag
    ${PROJECT_SOURCE_DIR}/assets/shaders/ellipticalParticle.vert
    ${PROJECT_SOURCE_DIR}/assets/shaders/ellipticalParticle.frag
    ${PROJECT_SOURCE_DIR}/assets/shaders/frame.vert
    ${PROJECT_SOURCE_DIR}/assets/shaders/frame.frag
    ${PROJECT_SOURCE_DIR}/assets/shaders/whiteLine.frag
    ${PROJECT_SOURCE_DIR}/assets/shaders/whiteLine.vert
    ${PROJECT_SOURCE_DIR}/assets/shaders/drill.vert
    ${PROJECT_SOURCE_DIR}/assets/shaders/drill.frag
    ${PROJECT_SOURCE_DIR}/assets/shaders/speedBg.vert
    ${PROJECT_SOURCE_DIR}/assets/shaders/speedBg.frag
    ${PROJECT_SOURCE_DIR}/assets/shaders/instanceObjectShadow.vert
    ${PROJECT_SOURCE_DIR}/assets/shaders/simpleObjectShadow.frag
) 

set(SRC_RENDER_UNIT
    ${SRC_DIR}RenderUnit/Shader.h
    ${SRC_DIR}RenderUnit/RenderStructure.h
    ${SRC_DIR}RenderUnit/RenderStructure.cpp
    ${SRC_DIR}RenderUnit/InstanceDrawer.h
    ${SRC_DIR}RenderUnit/InstanceDrawer.cpp
    ${SRC_DIR}RenderUnit/ParticleSystem.h
    ${SRC_DIR}RenderUnit/ParticleSystem.cpp
)

include_directories(${INCLUDE_DIR})
include_directories(${INCLUDE_DIR}glad4.6/include/)
include_directories(${INCLUDE_DIR}glm-0.9.8.5/glm/)

add_Definitions("-D_XKEYCHECK_H")
add_definitions(-DPROJECT_DIR="${PROJECT_SOURCE_DIR}")

add_executable(RollerCoasters
    ${SRC_DIR}CallBacks.h
    ${SRC_DIR}CallBacks.cpp
    ${SRC_DIR}ControlPoint.h
    ${SRC_DIR}ControlPoint.cpp
    ${SRC_DIR}main.cpp
    ${SRC_DIR}Object.h
    ${SRC_DIR}Track.h
    ${SRC_DIR}Track.cpp
    ${SRC_DIR}TrainView.h
    ${SRC_DIR}TrainView.cpp
    ${SRC_DIR}TrainWindow.h
    ${SRC_DIR}TrainWindow.cpp
    ${SRC_DIR}MathHelper.h
    ${SRC_DIR}MathHelper.cpp
    ${SRC_DIR}EntityStructure.h
    ${SRC_DIR}EntityStructure.cpp
    ${SRC_DIR}FreeCamera.h
    ${SRC_DIR}FreeCamera.cpp
    ${INCLUDE_DIR}glad4.6/src/glad.c

    ${SRC_DIR}SoundBuffer.h
    ${SRC_DIR}SoundBuffer.cpp
    ${SRC_DIR}SoundDevice.h
    ${SRC_DIR}SoundDevice.cpp
    ${SRC_DIR}SoundSource.h
    ${SRC_DIR}SoundSource.cpp

    ${SRC_SHADER}
    ${SRC_RENDER_UNIT}
)

source_group("shaders" FILES ${SRC_SHADER})
source_group("RenderUnit" FILES ${SRC_RENDER_UNIT})

add_library(Utilities 
    ${SRC_DIR}Utilities/3DUtils.h
    ${SRC_DIR}Utilities/3DUtils.cpp
    ${SRC_DIR}Utilities/ArcBallCam.h
    ${SRC_DIR}Utilities/ArcBallCam.cpp
    ${SRC_DIR}Utilities/Pnt3f.h
    ${SRC_DIR}Utilities/Pnt3f.cpp)

target_link_libraries(RollerCoasters 
    debug ${LIB_DIR}Debug/fltk_formsd.lib      optimized ${LIB_DIR}Release/fltk_forms.lib
    debug ${LIB_DIR}Debug/fltk_gld.lib         optimized ${LIB_DIR}Release/fltk_gl.lib
    debug ${LIB_DIR}Debug/fltk_imagesd.lib     optimized ${LIB_DIR}Release/fltk_images.lib
    debug ${LIB_DIR}Debug/fltk_jpegd.lib       optimized ${LIB_DIR}Release/fltk_jpeg.lib
    debug ${LIB_DIR}Debug/fltk_pngd.lib        optimized ${LIB_DIR}Release/fltk_png.lib
    debug ${LIB_DIR}Debug/fltk_zd.lib          optimized ${LIB_DIR}Release/fltk_z.lib
    debug ${LIB_DIR}Debug/fltkd.lib            optimized ${LIB_DIR}Release/fltk.lib
    debug ${LIB_DIR}Debug/assimp-vc142-mtd.lib           optimized ${LIB_DIR}Release/assimp-vc142-mtd.lib)

target_link_libraries(RollerCoasters 
    ${LIB_DIR}OpenGL32.lib
    ${LIB_DIR}glu32.lib
    ${LIB_DIR}OpenAL32.lib
    ${LIB_DIR}sndfile.lib)

target_link_libraries(RollerCoasters Utilities)

# 需要複製到執行檔路徑下的dll
set(DLL_SOURCE_PATHS
    ${LIB_DIR}dll/OpenAL32.dll
    ${LIB_DIR}dll/sndfile.dll
)
# 每次編譯完成後，把dll複製到執行檔所在資料夾(debug/release)
foreach(DLL_PATH ${DLL_SOURCE_PATHS})
    add_custom_command(TARGET RollerCoasters POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${DLL_PATH}
        ${CMAKE_CURRENT_BINARY_DIR}/$(Configuration)
    )
endforeach()

set(ASSETS_SOURCE_PATHS
    ${PROJECT_SOURCE_DIR}/assets
)

# 每次編譯完成後，複製資料夾到執行檔所在資料夾(debug/release)
foreach(SOURCE_DIR ${ASSETS_SOURCE_PATHS})
	get_filename_component(SOURCE_DIR_NAME ${SOURCE_DIR} NAME)
	add_custom_command(TARGET RollerCoasters POST_BUILD
		COMMAND ${CMAKE_COMMAND} -E copy_directory 
		${SOURCE_DIR}
		${CMAKE_CURRENT_BINARY_DIR}/$(Configuration)/${SOURCE_DIR_NAME}
	)
endforeach()

# 設定visual studio在編輯情況下的工作目錄與輸出目錄相同
set_target_properties(RollerCoasters PROPERTIES
    VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/$(Configuration)"
)
    
# 設定起始專案
set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT RollerCoasters)