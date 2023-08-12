#=============================================================================
# Copyright 2005-2011 Kitware, Inc.
# All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions
# are met:
#
# * Redistributions of source code must retain the above copyright
#   notice, this list of conditions and the following disclaimer.
#
# * Redistributions in binary form must reproduce the above copyright
#   notice, this list of conditions and the following disclaimer in the
#   documentation and/or other materials provided with the distribution.
#
# * Neither the name of Kitware, Inc. nor the names of its
#   contributors may be used to endorse or promote products derived
#   from this software without specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
# "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
# LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
# A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
# HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
# SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
# LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
# DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
# THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
# (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
# OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
#=============================================================================

include(CMakeParseArguments)

# redefine this function because of a bug (it originally parse all boost files, and make the build VERY slow)
function(QT6_CREATE_TRANSLATION _qm_files)
    set(options)
    set(oneValueArgs)
    set(multiValueArgs OPTIONS)

    cmake_parse_arguments(_LUPDATE "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})
    set(_lupdate_files ${_LUPDATE_UNPARSED_ARGUMENTS})
    set(_lupdate_options ${_LUPDATE_OPTIONS})

    set(_my_sources)
    set(_my_tsfiles)
    foreach(_file ${_lupdate_files})
        get_filename_component(_ext ${_file} EXT)
        get_filename_component(_abs_FILE ${_file} ABSOLUTE)
        if(_ext MATCHES "ts")
            list(APPEND _my_tsfiles ${_abs_FILE})
        else()
            list(APPEND _my_sources ${_abs_FILE})
        endif()
    endforeach()

    set(_my_temptsfiles)

    foreach(_ts_file ${_my_tsfiles})
        if(_my_sources)
          # make a list file to call lupdate on, so we don't make our commands too
          # long for some systems
          get_filename_component(_ts_name ${_ts_file} NAME)
          set(_ts_lst_file "${CMAKE_CURRENT_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/${_ts_name}_lst_file")
          set(_lst_file_srcs)
          foreach(_lst_file_src ${_my_sources})
              set(_lst_file_srcs "${_lst_file_src}\n${_lst_file_srcs}")
          endforeach()

          #get_directory_property(_inc_DIRS INCLUDE_DIRECTORIES)
          #foreach(_pro_include ${_inc_DIRS})
          #    get_filename_component(_abs_include "${_pro_include}" ABSOLUTE)
          #    set(_lst_file_srcs "-I${_pro_include}\n${_lst_file_srcs}")
          #endforeach()

          file(WRITE ${_ts_lst_file} "${_lst_file_srcs}")
        endif()

        get_filename_component(_ts_nm ${_ts_file} NAME)
        set(_tmpts_file "${CMAKE_CURRENT_BINARY_DIR}/languages/${_ts_nm}")
        list(APPEND _my_temptsfiles ${_tmpts_file})
        get_source_file_property(_qm_output_location ${_ts_file} OUTPUT_LOCATION)

        add_custom_command(OUTPUT ${_tmpts_file}
            COMMAND ${Qt6_LUPDATE_EXECUTABLE}
            ARGS ${_lupdate_options} "@${_ts_lst_file}" -ts ${_ts_file}
            COMMAND ${CMAKE_COMMAND} -E copy ${_ts_file} ${_tmpts_file}
            DEPENDS ${_my_sources}
            BYPRODUCTS ${_tmpts_file} VERBATIM)

        if(_qm_output_location)
            set_property(SOURCE ${_tmpts_file} PROPERTY OUTPUT_LOCATION ${_qm_output_location})
 		endif()

    endforeach()
    qt6_add_translation(${_qm_files} ${_my_temptsfiles})
    set(${_qm_files} ${${_qm_files}} PARENT_SCOPE)
endfunction()
