add_library(internet_layer ip_session.c ip_assembler.c session.c)

add_executable(test main.c)
target_link_libraries(test internet_layer -lz)
