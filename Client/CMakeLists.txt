add_executable(GigahrushClient "Client.cpp" "Client.h")
target_link_libraries(GigahrushClient PRIVATE asio)
target_link_libraries(GigahrushClient PRIVATE nlohmann_json)