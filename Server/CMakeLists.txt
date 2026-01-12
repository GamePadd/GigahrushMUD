set(SOURCES 
	Server.cpp
	Session.cpp
	main.cpp
	Game/Game.cpp
	Game/JsonParser.cpp
	Game/GameStructures.cpp
	Game/CommandHandler.cpp
	utils.cpp
)

set(HEADERS 
	Server.h
	Session.h
	Game/Game.h
	Game/JsonParser.h
	Game/GameStructures.h
	Game/CommandHandler.h
	utils.h
)

add_executable (GigahrushServer ${SOURCES} ${HEADERS})
target_link_libraries(GigahrushServer PRIVATE asio)
target_link_libraries(GigahrushServer PRIVATE nlohmann_json)

target_compile_options(GigahrushServer PRIVATE
    $<$<CXX_COMPILER_ID:MSVC>:/utf-8>
)