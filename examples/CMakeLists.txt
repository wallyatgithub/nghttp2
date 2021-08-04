file(GLOB c_sources *.c)
set_source_files_properties(${c_sources} PROPERTIES
COMPILE_FLAGS "${WARNCFLAGS}")
file(GLOB cxx_sources *.cc)
set_source_files_properties(${cxx_sources} PROPERTIES
COMPILE_FLAGS "${WARNCXXFLAGS} ${CXX1XCXXFLAGS}")

include_directories(
${CMAKE_CURRENT_SOURCE_DIR}
"${CMAKE_CURRENT_SOURCE_DIR}/../third-party"
"${CMAKE_CURRENT_SOURCE_DIR}/../third-party/llhttp/include"
"${CMAKE_CURRENT_SOURCE_DIR}/../third-party/StaticJSON/include"
${LIBEVENT_INCLUDE_DIRS}
${OPENSSL_INCLUDE_DIRS}
)

link_libraries(
nghttp2
${LIBEVENT_OPENSSL_LIBRARIES}
${OPENSSL_LIBRARIES}
${APP_LIBRARIES}
)

add_executable(asio-sv asio-sv.cc $<TARGET_OBJECTS:llhttp>
$<TARGET_OBJECTS:url-parser> $<TARGET_OBJECTS:staticjson>
)
target_include_directories(asio-sv PRIVATE
${OPENSSL_INCLUDE_DIRS}
${Boost_INCLUDE_DIRS}
)
target_link_libraries(asio-sv
nghttp2
nghttp2_asio_sv
${JEMALLOC_LIBRARIES}
${OPENSSL_LIBRARIES}
${Boost_LIBRARIES}
${APP_LIBRARIES}
)

