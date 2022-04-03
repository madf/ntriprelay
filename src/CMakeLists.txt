configure_file ( version.h.in version.h ESCAPE_QUOTES @ONLY )

file ( GLOB CPP_FILES main.cpp relay.cpp server.cpp client.cpp connection.cpp settings.cpp logger.cpp log_writer.cpp base64.cpp authenticator.cpp )

set ( THREADS_PREFER_PTHREAD_FLAG ON )
find_package ( Threads REQUIRED )

if ( TIDY )
    find_program( CLANG_TIDY_EXE NAMES "clang-tidy" DOC "Path to clang-tidy executable" )
    if ( NOT CLANG_TIDY_EXE )
        message( STATUS "clang-tidy not found." )
    else ()
        message( STATUS "clang-tidy found: ${CLANG_TIDY_EXE}" )
        set( DO_CLANG_TIDY "${CLANG_TIDY_EXE}"
             "-checks=-*,\
clang-analyzer-*,\
misc-*,\
modernize-*,\
performance-*,\
portability-*,\
readability-*,\
-modernize-avoid-bind,\
-modernize-use-trailing-return-type,\
-readability-braces-around-statements" )
    endif ()
endif ()

if ( IWYU )
    find_program( INCLUDE_WHAT_YOU_USE_EXE NAMES "include-what-you-use" DOC "Path to include-what-you-use executable" )
    if ( NOT INCLUDE_WHAT_YOU_USE_EXE )
        message( STATUS "include-what-you-use not found." )
    else ()
        message( STATUS "include-what-you-use found: ${INCLUDE_WHAT_YOU_USE_EXE}" )
        set( DO_INCLUDE_WHAT_YOU_USE "${INCLUDE_WHAT_YOU_USE_EXE}" )
    endif ()
endif ()

set ( CMAKE_INCLUDE_CURRENT_DIR ON )

add_executable ( ${PROJECT_NAME} ${CPP_FILES} )

target_link_libraries ( ${PROJECT_NAME} Boost::boost Boost::system Boost::program_options OpenSSL::Crypto Threads::Threads )

if ( CLANG_TIDY_EXE )
    set_target_properties ( ${PROJECT_NAME} PROPERTIES CXX_CLANG_TIDY "${DO_CLANG_TIDY}" )
endif ()
if ( INCLUDE_WHAT_YOU_USE_EXE )
    set_target_properties ( ${PROJECT_NAME} PROPERTIES CXX_INCLUDE_WHAT_YOU_USE "${DO_INCLUDE_WHAT_YOU_USE}" )
endif ()
