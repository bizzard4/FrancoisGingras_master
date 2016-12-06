# Really simple build configuration

cmake_minimum_required(VERSION 2.8.7)

project(TaskSystem C)

# User-defined options
option (TS_TESTS "Build and run task system tests" ON)

add_compile_options(-g)

include_directories(
  ${CMAKE_SOURCE_DIR} 
  System
  BarMsg
  FooTask
  MsgQueue
  Message
  TextMsg
  Errors
  PapaTask
  BazTask
  DirectSendTask
  DirectReceiveTask
)

set(SOURCES
  TextMsg/TextMsg.c 
  BarMsg/BarMsg.c 
  System/System.c 
  PapaTask/PapaTask.c 
  FooTask/FooTask.c 
  MsgQueue/MsgQ.c
  BazTask/BazTask.c
  DirectSendTask/DirectSendTask.c
  DirectReceiveTask/DirectReceiveTask.c
)

add_executable(task_system TaskTest.c ${SOURCES})
target_link_libraries(task_system pthread)

# Build unit tests
if (TS_TESTS)
  enable_testing()
  set(all_tests "")

  macro (add_tasksystem_test NAME TIMEOUT)
    list (APPEND all_tests ${NAME})
    add_executable (${NAME} Tests/${NAME}.c ${SOURCES})
    target_link_libraries(${NAME} pthread)
    add_test (NAME ${NAME} COMMAND ${NAME})
    set_tests_properties (${NAME} PROPERTIES TIMEOUT ${TIMEOUT})
  endmacro (add_tasksystem_test)

  add_tasksystem_test(system_test 5)
  add_tasksystem_test(simple_task_test 5)
  add_tasksystem_test(direct_receive_test 5)
endif ()


                 
                  
                  