# Tencent is pleased to support the open source community by making xLua available.
# Copyright (C) 2016 THL A29 Limited, a Tencent company. All rights reserved.
# Licensed under the MIT License (the "License"); you may not use this file except in compliance with the License. You may obtain a copy of the License at
# http://opensource.org/licenses/MIT
# Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the specific language governing permissions and limitations under the License.

cmake_minimum_required(VERSION 2.8)

if ( WIN32 AND NOT CYGWIN AND NOT ( CMAKE_SYSTEM_NAME STREQUAL "WindowsStore" ) AND NOT ANDROID)
	set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} /MT" CACHE STRING "")
	set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} /MTd" CACHE STRING "")
	set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MT" CACHE STRING "")
	set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /MTd" CACHE STRING "")
endif ()

project(XLua)

if ( IOS )
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fembed-bitcode")
        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fembed-bitcode")
endif ()

if ( WIN32 )
    add_definitions(-DWIN32)
    add_definitions(-D_WINDOWS)
    add_definitions(-D_WIN32)
    add_definitions(-D_USRDLL)
    add_definitions(-DUNICODE)
    add_definitions(-D_UNICODE)
    add_definitions(-DWIN32_LEAN_AND_MEAN)
endif ()

option ( UINT_ESPECIALLY "using custom ulong" OFF )
option ( USING_LUAJIT "using luajit" OFF )
option ( GC64 "using gc64" OFF )
option ( LUAC_COMPATIBLE_FORMAT "compatible format" OFF )

#自定义option begin
option ( XLUA_BUILD_WITH_CARES "build with c-ares support" ON )
option ( YASIO_USE_SHARED_PACKET "" ON)

message(STATUS "WINSTORE=${WINSTORE}")
message(STATUS "LINUX=${LINUX}")
message(STATUS "MSVC=${MSVC}")

#使用Cjson
option ( USE_CJSON "using cjson" ON )
#使用7Zip
option ( USE_PC7ZIP "using 7zip" ON )
#使用RapidJson
option ( USE_RAPIDJSON "using RapidJson" ON )
#使用pb
option ( USE_PROTOBUF "using pb" ON )

# Disable c-ares for winstore, because c-ares not support it properly
if(WINSTORE)
    set (XLUA_BUILD_WITH_CARES OFF)
endif()
message(STATUS "XLUA_BUILD_WITH_CARES=${XLUA_BUILD_WITH_CARES}")

#自定义option end

find_path(XLUA_PROJECT_DIR NAMES SConstruct
    PATHS 
    ${CMAKE_SOURCE_DIR}
    NO_DEFAULT_PATH
    )

MARK_AS_ADVANCED(XLUA_PROJECT_DIR)

# 本地代码的存放路径
add_definitions(-DUNITY)
if (USE_CJSON)
add_definitions(-DUSE_CJSON)
endif()

set (SRC_CODE_ROOT ${CMAKE_SOURCE_DIR}/NativeSrc)
list(APPEND THIRDPART_INC
    ${SRC_CODE_ROOT}
    ${SRC_CODE_ROOT}/base
    ${SRC_CODE_ROOT}/scripting
    ${CMAKE_SOURCE_DIR}/external
    ${CMAKE_SOURCE_DIR}/external/thirdparty
)

# tolua++代码根文件夹路径
set(TOLUAPP_51_ROOT "external/tolua/toluapp_51")
set(TOLUAPP_53_ROOT "external/tolua/toluapp_52_53")
if (USING_LUAJIT)
    set(TOLUAPP_ROOT ${TOLUAPP_51_ROOT})    # luajit只支持5.1，所以tolua也用支持5.1的版本
    ADD_DEFINITIONS(-DLUA_COMPAT_5_1)
else ()
    set(TOLUAPP_ROOT ${TOLUAPP_53_ROOT})
endif ()
list(APPEND THIRDPART_INC  ${TOLUAPP_ROOT})

if (USE_PC7ZIP)
    list(APPEND THIRDPART_INC  external/lua-7zip)
    list(APPEND THIRDPART_INC  external/lua-7zip/lzma)
    list(APPEND THIRDPART_INC  external/lua-7zip/lzma/C)
    list(APPEND THIRDPART_INC  external/lua-7zip/src)
endif () 

if (USE_RAPIDJSON)
    list(APPEND THIRDPART_INC  external/lua-rapidjson/include)
    list(APPEND THIRDPART_INC  external/lua-rapidjson/include/rapidjson)
endif ()

if (USE_PROTOBUF)
    list(APPEND THIRDPART_INC  external/lua-protobuf)
endif () 

# begin yasio
# yasio strong recommend use c++17 with sol bindings
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)
set (YASIO_SOURCES
  yasio/yasio/xxsocket.cpp
  yasio/yasio/io_service.cpp
  yasio/yasio/bindings/lyasio.cpp
  yasio/yasio/bindings/yasio_ni.cpp
  yasio/3rdparty/kcp/ikcp.c
)
if (ANDROID)
  list(APPEND THIRDPART_LIB "log")
endif()
list(APPEND THIRDPART_INC yasio yasio/3rdparty yasio/3rdparty/kcp)
list(APPEND THIRDPART_SRC ${YASIO_SOURCES})

set_property(
    SOURCE ${YASIO_SOURCES}
    APPEND
    PROPERTY COMPILE_DEFINITIONS
    LUA_COMPAT_APIINTCASTS LUA_LIB _CRT_SECURE_NO_WARNINGS _WINSOCK_DEPRECATED_NO_WARNINGS
    YASIO_ENABLE_KCP=1
    YASIO_ENABLE_HALF_FLOAT=1
)
if (MSVC)
    set_property(SOURCE yasio/yasio/bindings/lyasio.cpp APPEND PROPERTY COMPILE_OPTIONS "/bigobj")
endif()
# end yasio

# end tolua++

if (NOT LUA_VERSION)
    set(LUA_VERSION "5.3.5")
endif()

set(LUA_SRC_PATH lua-${LUA_VERSION}/src)
set(LUAJIT_SRC_PATH luajit-2.1.0b3/src)

if (USING_LUAJIT)
	
    if (APPLE AND NOT IOS AND NOT ANDROID)
	    include_directories(
		  ${CMAKE_SOURCE_DIR}
		  lua-5.1.5/src
		  lua-5.1.5/etc
		  ${CMAKE_CURRENT_BINARY_DIR}
		  ${THIRDPART_INC}
	    )
	    set ( LUA_CORE lua-5.1.5/src/lapi.c lua-5.1.5/src/lcode.c lua-5.1.5/src/ldebug.c lua-5.1.5/src/ldo.c lua-5.1.5/src/ldump.c lua-5.1.5/src/lfunc.c 
          lua-5.1.5/src/lgc.c lua-5.1.5/src/llex.c lua-5.1.5/src/lmem.c lua-5.1.5/src/lobject.c lua-5.1.5/src/lopcodes.c lua-5.1.5/src/lparser.c lua-5.1.5/src/lstate.c 
          lua-5.1.5/src/lstring.c lua-5.1.5/src/ltable.c lua-5.1.5/src/ltm.c lua-5.1.5/src/lundump.c lua-5.1.5/src/lvm.c lua-5.1.5/src/lzio.c lua-5.1.5/src/lauxlib.c 
          lua-5.1.5/src/lbaselib.c lua-5.1.5/src/ldblib.c lua-5.1.5/src/liolib.c lua-5.1.5/src/lmathlib.c lua-5.1.5/src/loslib.c lua-5.1.5/src/ltablib.c 
          lua-5.1.5/src/lstrlib.c lua-5.1.5/src/linit.c lua-5.1.5/src/loadlib.c)
	else ()
	    include_directories(
		  ${CMAKE_SOURCE_DIR}
		  ${LUAJIT_SRC_PATH}
		  ${CMAKE_CURRENT_BINARY_DIR}
		  ${THIRDPART_INC}
	    )

	    set ( LUA_CORE )
	    set_property( SOURCE xlua.c APPEND PROPERTY COMPILE_DEFINITIONS USING_LUAJIT )
    endif ()
	set ( LUA_LIB )
else ()
	set ( LUA_IDSIZE 120 CACHE STRING "gives the maximum size for the description of the source." )

	configure_file ( ${LUA_SRC_PATH}/luaconf.h.in ${CMAKE_CURRENT_BINARY_DIR}/luaconf.h )

	include_directories(
	    ${CMAKE_SOURCE_DIR}
		${LUA_SRC_PATH}
		${CMAKE_CURRENT_BINARY_DIR}
		${THIRDPART_INC}
	)

    aux_source_directory(${LUA_SRC_PATH} LUA_CORE)
    list(REMOVE_ITEM LUA_CORE ${LUA_SRC_PATH}/lua.c ${LUA_SRC_PATH}/luac.c)
	set ( LUA_LIB )

	# set ( LUA_CORE ${LUA_SRC_PATH}/lapi.c ${LUA_SRC_PATH}/lcode.c ${LUA_SRC_PATH}/lctype.c ${LUA_SRC_PATH}/ldebug.c ${LUA_SRC_PATH}/ldo.c ${LUA_SRC_PATH}/ldump.c
	#   ${LUA_SRC_PATH}/lfunc.c ${LUA_SRC_PATH}/lgc.c ${LUA_SRC_PATH}/llex.c ${LUA_SRC_PATH}/lmem.c ${LUA_SRC_PATH}/lobject.c ${LUA_SRC_PATH}/lopcodes.c ${LUA_SRC_PATH}/lparser.c
	#   ${LUA_SRC_PATH}/lstate.c ${LUA_SRC_PATH}/lstring.c ${LUA_SRC_PATH}/ltable.c ${LUA_SRC_PATH}/ltm.c ${LUA_SRC_PATH}/lundump.c ${LUA_SRC_PATH}/lvm.c ${LUA_SRC_PATH}/lzio.c )
	# set ( LUA_LIB ${LUA_SRC_PATH}/lauxlib.c ${LUA_SRC_PATH}/lbaselib.c ${LUA_SRC_PATH}/lbitlib.c ${LUA_SRC_PATH}/lcorolib.c ${LUA_SRC_PATH}/ldblib.c
	#   ${LUA_SRC_PATH}/liolib.c ${LUA_SRC_PATH}/lmathlib.c ${LUA_SRC_PATH}/loslib.c ${LUA_SRC_PATH}/lstrlib.c ${LUA_SRC_PATH}/ltablib.c ${LUA_SRC_PATH}/linit.c
	#   ${LUA_SRC_PATH}/lutf8lib.c ${LUA_SRC_PATH}/loadlib.c )
endif ()

set ( LUA_SOCKET
    luasocket/auxiliar.c
    luasocket/buffer.c
    luasocket/except.c
    luasocket/inet.c
    luasocket/io.c
    luasocket/luasocket.c
    luasocket/mime.c
    luasocket/options.c
    luasocket/select.c
    luasocket/tcp.c
    luasocket/timeout.c
    luasocket/udp.c 
)

if ( WIN32 AND NOT CYGWIN )
    list (APPEND LUA_SOCKET luasocket/wsocket.c)
else ( )
    list (APPEND LUA_SOCKET luasocket/usocket.c)
endif ( )

set(LUA_EXTENSIONS_SRC)


# 自定义模块option

if (USING_LUAJIT)
    add_definitions(-DUSING_LUAJIT)
    add_definitions(-DLUAJIT_2_1_0_b3)
endif()

if (USE_PC7ZIP)
    add_definitions(-DUSE_PC7ZIP)
endif()

if (USE_RAPIDJSON)
    add_definitions(-DUSE_RAPIDJSON)
endif()

if (USE_PROTOBUF)
    add_definitions(-DUSE_PROTOBUF)
endif()
# 自定义模块optionEnd

#自定义 cpp
# begin tolua
list(APPEND LUA_EXTENSIONS_SRC 
    ${TOLUAPP_ROOT}/tolua_event.c
    ${TOLUAPP_ROOT}/tolua_is.c
    ${TOLUAPP_ROOT}/tolua_map.c
    ${TOLUAPP_ROOT}/tolua_push.c
    ${TOLUAPP_ROOT}/tolua_to.c
)
# end tolua

list(APPEND LUA_EXTENSIONS_SRC
    ${SRC_CODE_ROOT}/PerformFunctions.cpp
    ${SRC_CODE_ROOT}/UnityLuaOpen.cpp
    # ${SRC_CODE_ROOT}/FileUtils.cpp
)

# begin cocos_code
list(APPEND LUA_EXTENSIONS_SRC
    ${SRC_CODE_ROOT}/scripting/lua-bindings/manual/LuaBasicConversions.cpp
    ${SRC_CODE_ROOT}/scripting/lua-bindings/manual/pc_tolua_fix.c
    ${SRC_CODE_ROOT}/scripting/lua-bindings/manual/tolua_fix.cpp
    ${SRC_CODE_ROOT}/scripting/lua-bindings/manual/LuaStack.c
)
# end cocos_code

if (USE_CJSON)
    # begin cjson
    list(APPEND LUA_EXTENSIONS_SRC
        external/lua-cjson/fpconv.c
        external/lua-cjson/lua_cjson.c
        external/lua-cjson/strbuf.c
    )
    list(APPEND THIRDPART_INC  external/lua-cjson)
    # end cjson
endif()

if (USE_PC7ZIP)
    # begin 7zip
    list(APPEND LUA_EXTENSIONS_SRC
        external/lua-7zip/src/PCZipExtract.cpp
        external/lua-7zip/src/lua_app_7zip.cpp
        external/lua-7zip/lzma/C7z.cpp
        external/lua-7zip/lzma/7zMain.c
        external/lua-7zip/lzma/C/Lzma2Dec.c
        external/lua-7zip/lzma/C/Ppmd7Dec.c
        external/lua-7zip/lzma/C/XzDec.c
        external/lua-7zip/lzma/C/Bcj2.c
        external/lua-7zip/lzma/C/LzFind.c
        external/lua-7zip/lzma/C/Ppmd7.c
        external/lua-7zip/lzma/C/Bra.c
        external/lua-7zip/lzma/C/7zIn.c
        external/lua-7zip/lzma/C/Bra86.c
        external/lua-7zip/lzma/C/7zAlloc.c
        external/lua-7zip/lzma/C/XzIn.c
        external/lua-7zip/lzma/C/Alloc.c
        external/lua-7zip/lzma/C/LzmaDec.c
        external/lua-7zip/lzma/C/Ppmd7Enc.c
        external/lua-7zip/lzma/C/7zDec.c
        external/lua-7zip/lzma/C/7zBuf2.c
        external/lua-7zip/lzma/C/XzCrc64.c
        external/lua-7zip/lzma/C/BraIA64.c
        external/lua-7zip/lzma/C/Lzma86Dec.c
        external/lua-7zip/lzma/C/7zStream.c
        external/lua-7zip/lzma/C/Sha256.c
        external/lua-7zip/lzma/C/Delta.c
        external/lua-7zip/lzma/C/7zFile.c
        external/lua-7zip/lzma/C/7zCrc.c
        external/lua-7zip/lzma/C/CpuArch.c
        external/lua-7zip/lzma/C/Xz.c
        external/lua-7zip/lzma/C/7zCrcOpt.c
        external/lua-7zip/lzma/C/7zBuf.c
    )
    # end 7zip
endif()

if (USE_RAPIDJSON)
    #begin lua-rapidjson
    list(APPEND LUA_EXTENSIONS_SRC
        external/lua-rapidjson/source/rapidjson.cpp
        external/lua-rapidjson/source/Document.cpp
        external/lua-rapidjson/source/Schema.cpp
        external/lua-rapidjson/source/values.cpp
    )
 endif()

 if (USE_PROTOBUF)
    # list(APPEND THIRDPART_INC  external/lua-protobuf/pb.c)
    list(APPEND LUA_EXTENSIONS_SRC
        external/lua-protobuf/pb.c
    )
endif () 
#自定义 cpp end

set_property(
    SOURCE ${LUA_EXTENSIONS_SRC}
    APPEND
    PROPERTY COMPILE_DEFINITIONS
    LUA_LIB
)
list(APPEND THIRDPART_SRC ${LUA_EXTENSIONS_SRC})


set ( XLUA_CORE
    i64lib.c
    xlua.c
)

if (NOT USING_LUAJIT)
    list (APPEND XLUA_CORE memory_leak_checker.c)
endif ( )

macro(source_group_by_dir proj_dir source_files)
    if(MSVC OR APPLE)
        get_filename_component(sgbd_cur_dir ${proj_dir} ABSOLUTE)
        foreach(sgbd_file ${${source_files}})
			get_filename_component(sgbd_abs_file ${sgbd_file} ABSOLUTE)
            file(RELATIVE_PATH sgbd_fpath ${sgbd_cur_dir} ${sgbd_abs_file})
            string(REGEX REPLACE "\(.*\)/.*" \\1 sgbd_group_name ${sgbd_fpath})
            string(COMPARE EQUAL ${sgbd_fpath} ${sgbd_group_name} sgbd_nogroup)
            string(REPLACE "/" "\\" sgbd_group_name ${sgbd_group_name})
            if(sgbd_nogroup)
                set(sgbd_group_name "\\")
            endif(sgbd_nogroup)
            source_group(${sgbd_group_name} FILES ${sgbd_file})
        endforeach(sgbd_file)
    endif(MSVC OR APPLE)
endmacro(source_group_by_dir)

source_group_by_dir(${CMAKE_CURRENT_SOURCE_DIR} LUA_SOCKET)
source_group_by_dir(${CMAKE_CURRENT_SOURCE_DIR} LUA_CORE)
source_group_by_dir(${CMAKE_CURRENT_SOURCE_DIR} LUA_LIB)
source_group_by_dir(${CMAKE_CURRENT_SOURCE_DIR} XLUA_CORE)

if (APPLE)
    if (IOS)
        ADD_DEFINITIONS(-DLUA_USE_IOS) # Despite claiming to be ISO C, iOS does not implement 'system'.(iOS11)
        set(CMAKE_OSX_ARCHITECTURES "$(ARCHS_STANDARD)")
        add_library(xlua STATIC
           ${LUA_CORE}
           ${LUA_LIB}
           ${LUA_SOCKET}
           ${XLUA_CORE}
           ${THIRDPART_SRC}
        )
		set_xcode_property (xlua IPHONEOS_DEPLOYMENT_TARGET "7.0" "all")
    else ()
        ADD_DEFINITIONS(-DLUA_USE_MACOSX) #osx platform emmylua debugger must have this option or can not load cpath
        if (BUILD_SILICON)
           set(CMAKE_OSX_ARCHITECTURES arm64)

            add_library(xlua SHARED
                ${LUA_CORE}
                ${LUA_LIB}
                ${LUA_SOCKET}
                ${XLUA_CORE}
                ${THIRDPART_SRC}
            )
        else ()
            set(CMAKE_OSX_ARCHITECTURES "$(ARCHS_STANDARD_64_BIT)")
            add_library(xlua MODULE
                ${LUA_CORE}
                ${LUA_LIB}
                ${LUA_SOCKET}
                ${XLUA_CORE}
                ${THIRDPART_SRC}
            )
            set_target_properties ( xlua PROPERTIES BUNDLE TRUE )
            #set_target_properties ( xlua PROPERTIES FRAMEWORK TRUE )
            #set_target_properties ( xlua PROPERTIES MACOSX_RPATH TRUE )
        endif ()
    endif ()
elseif ("${CMAKE_SYSTEM_NAME}" STREQUAL "Switch")
    add_library(xlua STATIC
        ${LUA_CORE}
        ${LUA_LIB}
        ${XLUA_CORE}
        ${THIRDPART_SRC}
    )
    target_compile_options(xlua PRIVATE -m64 -mcpu=cortex-a57+fp+simd+crypto+crc -fno-common -fno-short-enums -ffunction-sections -fdata-sections -fPIC -fms-extensions)
else ( )
    add_library(xlua SHARED
        ${LUA_CORE}
        ${LUA_LIB}
        ${LUA_SOCKET}
        ${XLUA_CORE}
        ${THIRDPART_SRC}
    )
endif ( )

if ( WIN32 AND NOT CYGWIN )
    target_compile_definitions (xlua PRIVATE LUA_BUILD_AS_DLL)
endif ( )

if (GC64 AND USING_LUAJIT)
    target_compile_definitions (xlua PRIVATE LUAJIT_ENABLE_GC64)
endif ()

if (LUAC_COMPATIBLE_FORMAT AND NOT USING_LUAJIT)
    target_compile_definitions (xlua PRIVATE LUAC_COMPATIBLE_FORMAT)
endif ()

set_property(
	SOURCE ${LUA_SOCKET}
	APPEND
	PROPERTY COMPILE_DEFINITIONS
	LUA_COMPAT_APIINTCASTS LUA_LIB _CRT_SECURE_NO_WARNINGS _WINSOCK_DEPRECATED_NO_WARNINGS
)

if(UINT_ESPECIALLY)
    ADD_DEFINITIONS(-DUINT_ESPECIALLY)
endif()
	
if ( WIN32 AND NOT CYGWIN )
    if (USING_LUAJIT)
        target_link_libraries(xlua
            ${CMAKE_SOURCE_DIR}/${LUAJIT_SRC_PATH}/lua51.lib
            ws2_32
            ${THIRDPART_LIB} 
        )
    else ()
        target_link_libraries(xlua
            ws2_32
            ${THIRDPART_LIB} 
        )
    endif()
else ( )
    if (USING_LUAJIT AND NOT APPLE)
		target_link_libraries(xlua
			${CMAKE_SOURCE_DIR}/${LUAJIT_SRC_PATH}/libluajit.a
			m
            ${THIRDPART_LIB} 
		)
    else ()
        target_link_libraries(xlua
            m
            ${THIRDPART_LIB} 
        )
    endif()
endif ( )
