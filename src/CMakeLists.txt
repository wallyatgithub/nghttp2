#add_subdirectory(third-party)

find_package(OpenSSL 1.0.1)

add_subdirectory(includes)

file(GLOB c_sources *.c)
set_source_files_properties(${c_sources} PROPERTIES
  COMPILE_FLAGS "${WARNCFLAGS}")
file(GLOB cxx_sources *.cc)
set_source_files_properties(${cxx_sources} PROPERTIES
  COMPILE_FLAGS "${WARNCXXFLAGS} ${CXX1XCXXFLAGS}")

include_directories(
  "${CMAKE_CURRENT_SOURCE_DIR}/includes"
  "${CMAKE_CURRENT_SOURCE_DIR}/../third-party"
  "${CMAKE_CURRENT_SOURCE_DIR}/../third-party/llhttp/include"

  ${JEMALLOC_INCLUDE_DIRS}
  ${LIBXML2_INCLUDE_DIRS}
  ${LIBEV_INCLUDE_DIRS}
  ${OPENSSL_INCLUDE_DIRS}
  ${LIBCARES_INCLUDE_DIRS}
  ${JANSSON_INCLUDE_DIRS}
  ${ZLIB_INCLUDE_DIRS}
)

# XXX per-target?
link_libraries(
  nghttp2
  ${JEMALLOC_LIBRARIES}
  ${LIBXML2_LIBRARIES}
  ${LIBEV_LIBRARIES}
  ${OPENSSL_LIBRARIES}
  ${LIBCARES_LIBRARIES}
  ${JANSSON_LIBRARIES}
  ${ZLIB_LIBRARIES}
  ${APP_LIBRARIES}
)

set(NGHTTP2_ASIO_SV_SOURCES
  util.cc http2.cc
  tls.cc
  timegm.c
  asio_common.cc
  asio_io_service_pool.cc
  asio_server_http2.cc
  asio_server_http2_impl.cc
  asio_server.cc
  asio_server_http2_handler.cc
  asio_server_request.cc
  asio_server_request_impl.cc
  asio_server_response.cc
  asio_server_response_impl.cc
  asio_server_stream.cc
  asio_server_serve_mux.cc
  asio_server_request_handler.cc
  asio_server_tls_context.cc
)

add_library(nghttp2_asio_sv SHARED
   ${NGHTTP2_ASIO_SV_SOURCES}
   $<TARGET_OBJECTS:llhttp>
   $<TARGET_OBJECTS:url-parser>
)
target_include_directories(nghttp2_asio_sv PRIVATE
  ${OPENSSL_INCLUDE_DIRS}
  ${Boost_INCLUDE_DIRS}
)
target_include_directories(nghttp2_asio_sv INTERFACE
  "${CMAKE_CURRENT_BINARY_DIR}/../lib/includes"
  "${CMAKE_CURRENT_SOURCE_DIR}/../lib/includes"
  "${CMAKE_CURRENT_SOURCE_DIR}/includes"
)
target_link_libraries(nghttp2_asio_sv
  nghttp2
  ${OPENSSL_LIBRARIES}
  ${Boost_LIBRARIES}
)
set_target_properties(nghttp2_asio_sv PROPERTIES
  VERSION 1.0.0 SOVERSION 1)

install(TARGETS nghttp2_asio_sv
  ARCHIVE DESTINATION "${CMAKE_INSTALL_LIBDIR}"
  LIBRARY DESTINATION "${CMAKE_INSTALL_LIBDIR}"
  RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}")

install(FILES "${CMAKE_CURRENT_BINARY_DIR}/libnghttp2_asio_sv.pc"
  DESTINATION "${CMAKE_INSTALL_LIBDIR}/pkgconfig")
