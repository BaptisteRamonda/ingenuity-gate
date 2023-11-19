# File list
target_sources(${PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/agents/whiteboard.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/agents/whiteboard.h
)

target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/agents)

