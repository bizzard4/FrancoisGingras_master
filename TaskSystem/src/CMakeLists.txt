# Build task system as a library

set(SOURCES
  System/System.c 
  MsgQueue/UnboundedMsgQ.c
  Messages/TextMsg/TextMsg.c 
  Messages/BarMsg/BarMsg.c 
  Messages/ProduceMsg/ProduceMsg.c
  Tasks/PapaTask/PapaTask.c 
  Tasks/FooTask/FooTask.c 
  Tasks/BazTask/BazTask.c
  Tasks/DirectSendTask/DirectSendTask.c
  Tasks/DirectReceiveTask/DirectReceiveTask.c
  Tasks/DirectMissSendTask/DirectMissSendTask.c
  Tasks/DirectMissReceiveTask/DirectMissReceiveTask.c
  Tasks/LoopSendTask/LoopSendTask.c
  Tasks/LoopReceiveTask/LoopReceiveTask.c
  Tasks/ConsumerTask/ConsumerTask.c
  Tasks/ProducerTask/ProducerTask.c
)

add_compile_options(-g)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c11")

add_library(TaskSystem ${SOURCES})
target_link_libraries(TaskSystem pthread)

target_include_directories(TaskSystem PUBLIC ${PROJECT_INCLUDE_DIR})