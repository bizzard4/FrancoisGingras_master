# Build test cases

add_executable(system_test system_test.c)
target_link_libraries(system_test TaskSystem)

add_executable(unbounded_msgq_test unbounded_msgq_test.c)
target_link_libraries(unbounded_msgq_test TaskSystem)

add_executable(simple_task_test simple_task_test.c)
target_link_libraries(simple_task_test TaskSystem)

add_executable(producer_consumer_test producer_consumer_test.c)
target_link_libraries(producer_consumer_test TaskSystem)

add_executable(loop_task_test loop_task_test.c)
target_link_libraries(loop_task_test TaskSystem)

add_executable(direct_receive_test direct_receive_test.c)
target_link_libraries(direct_receive_test TaskSystem)

add_executable(direct_miss_test direct_miss_test.c)
target_link_libraries(direct_miss_test TaskSystem)

add_executable(samplesort_test samplesort_test.c)
target_link_libraries(samplesort_test TaskSystem)

enable_testing()
add_test(system_test system_test)
add_test(unbounded_msgq_test unbounded_msgq_test)
add_test(simple_task_test simple_task_test)
add_test(producer_consumer_test producer_consumer_test)
add_test(loop_task_test loop_task_test)
add_test(direct_receive_test direct_receive_test)
add_test(direct_miss_test direct_miss_test)
add_test(samplesort_test samplesort_test)
