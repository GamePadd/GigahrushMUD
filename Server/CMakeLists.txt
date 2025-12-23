set(SOURCES 
	Server.cpp
	Session.cpp
	main.cpp
	Game/Game.cpp
	Game/JsonParser.cpp
	Game/GameStructures.cpp
)

set(HEADERS 
	Server.h
	Session.h
	Game/Game.h
	Game/JsonParser.h
	Game/GameStructures.h
)

add_executable (GigahrushServer ${SOURCES} ${HEADERS})
target_link_libraries(GigahrushServer PRIVATE asio)
target_link_libraries(GigahrushServer PRIVATE nlohmann_json)