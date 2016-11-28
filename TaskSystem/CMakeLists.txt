# Really simple build configuration

cmake_minimum_required(VERSION 2.8.12)

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
)


set(SOURCES
  TaskTest.c
  TextMsg/TextMsg.c 
  BarMsg/BarMsg.c 
  System/System.c 
  PapaTask/PapaTask.c 
  FooTask/FooTask.c 
  MsgQueue/MsgQ.c
  BazTask/BazTask.c
)

add_executable(test.exe ${SOURCES})
target_link_libraries(test.exe  pthread)


                 
                  
                  