add_executable (GigahrushServer "Server.cpp" "Server.h")
target_link_libraries(GigahrushServer PRIVATE asio)
target_link_libraries(GigahrushServer PRIVATE nlohmann_json)