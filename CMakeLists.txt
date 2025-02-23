cmake_minimum_required(VERSION 3.22)

project(
	Graph
)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_executable(
	graph
	"src/Main.cpp"
	"src/Objects/Node.cpp"
	"src/Objects/Edge.cpp"
	"src/Objects/Object.cpp"
	"src/Objects/ObjectManager.cpp"
	"src/Utils.cpp"
	"src/ImGuiExtra.cpp"
	"src/ImmersiveDarkMode.cpp"
)

target_include_directories(graph PRIVATE "include/")

add_executable(
	test
	"src/Test.cpp"
)

if (WIN32)
	find_package(ImGui-SFML CONFIG REQUIRED)
	add_compile_definitions(GRAPH_WINDOWS)
else ()
	add_subdirectory(external/ImGUI-SFML)
endif ()

target_link_libraries(graph PRIVATE ImGui-SFML::ImGui-SFML)
target_link_libraries(test  PRIVATE ImGui-SFML::ImGui-SFML)