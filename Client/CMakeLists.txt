set(SOURCES
	Client.cpp
	main.cpp
)

set(HEADERS
	Client.h
)

add_executable(GigahrushClient ${SOURCES} ${HEADERS})

set(aud_dir "${CMAKE_SOURCE_DIR}/thirdparty/audiere-1.9.4-win32/")
target_link_libraries(GigahrushClient PRIVATE "${aud_dir}/lib/audiere.lib")
target_include_directories(GigahrushClient PRIVATE "${aud_dir}/include")
target_link_libraries(GigahrushClient PRIVATE asio)
target_link_libraries(GigahrushClient PRIVATE nlohmann_json)
target_link_libraries(GigahrushClient PRIVATE ${aud_dir}/lib/audiere.lib)

#target_compile_options(GigahrushClient PRIVATE
#    $<$<CXX_COMPILER_ID:MSVC>:/utf-8>
#)