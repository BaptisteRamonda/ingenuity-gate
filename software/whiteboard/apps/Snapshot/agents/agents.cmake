# File list
target_sources(${PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/agents/snapshot.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/agents/snapshot.h
)

target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/agents)

