# Build test cases

add_executable(system_test system_test.c)
target_link_libraries(system_test TaskSystem)

add_executable(simple_task_test simple_task_test.c)
target_link_libraries(simple_task_test TaskSystem)

add_executable(samplesort_test samplesort_test.c)
target_link_libraries(samplesort_test TaskSystem)

add_executable(ref_intarray_test ref_intarray_test.c)
target_link_libraries(ref_intarray_test TaskSystem)

add_executable(db_server_test db_server_test.c)
target_link_libraries(db_server_test TaskSystem)

add_executable(db_client_test db_client_test.c)
target_link_libraries(db_client_test TaskSystem)

add_executable(circular_msgq_test circular_msgq_test.c)
target_link_libraries(circular_msgq_test TaskSystem)

add_executable(wt_start_test wt_start_test.c)
target_link_libraries(wt_start_test TaskSystem)

add_executable(wt_client_test wt_client_test.c)
target_link_libraries(wt_client_test TaskSystem)

enable_testing()
add_test(system_test system_test)
add_test(simple_task_test simple_task_test)
add_test(samplesort_test samplesort_test)
add_test(ref_intarray_test ref_intarray_test)
add_test(db_server_test db_server_test)
add_test(db_client_test db_client_test)
add_test(circular_msgq_test circular_msgq_test)