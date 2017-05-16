# Build task system as a library

set(SOURCES
  System/System.c 
  MsgQueue/CircularMsgQ.c
  Messages/TextMsg/TextMsg.c 
  Messages/BarMsg/BarMsg.c 
  Messages/TopologyMsg/TopologyMsg.c
  Messages/IntArrayMsg/IntArrayMsg.c
  Messages/DoneMsg/DoneMsg.c
  Messages/RefIntArrayMsg/RefIntArrayMsg.c
  Messages/RefTwoDimIntArrayMsg/RefTwoDimIntArrayMsg.c
  Messages/RequestMsg/RequestMsg.c
  Messages/ResponseMsg/ResponseMsg.c
  Tasks/PapaTask/PapaTask.c 
  Tasks/FooTask/FooTask.c 
  Tasks/BazTask/BazTask.c
  Tasks/SampleSortTask/SampleSortTask.c
  Tasks/BucketTask/BucketTask.c
  Tasks/DatabaseTask/DatabaseTask.c
  Tasks/ClientTask/ClientTask.c
  TimeHelper.c
)

add_compile_options(-g)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c11")

add_library(TaskSystem ${SOURCES})
target_link_libraries(TaskSystem pthread)
target_link_libraries(TaskSystem -lrt)

target_include_directories(TaskSystem PUBLIC ${PROJECT_INCLUDE_DIR})
