## 目录/usr/local/commlib/视频公共库

SPP_VER_3.0.1 = true
COMM_CACHE_2.0 = true
include /usr/local/commlib/makefile.comm
include ../makefile.comm

##重要说明
##说明：redis库使用的是视频公共库 $(INC_REDISAPI) 、$(LIB_REDISAPI)，只用到了里面的hiredis库，redis_api这个并没有用到

INC += -I. \
	$(INC_JCE_PROTOCOL)\
	$(INC_PROJ_COMM)\
	$(INC_COMM_SPPTASK)\
	$(INC_JCE)\
	$(INC_SPP)\
	$(INC_COMM_CACHE) \
	$(INC_BASELIB)\
	$(INC_CONFIG)\
	$(INC_VIDEOPACKET)\
	$(INC_CSPACKET)\
	$(INC_PACKET)\
	$(INC_L5)\
	$(INC_NAMEAPI)\
	$(INC_ITIL_MCALL)\
	$(INC_UNION)\
	$(INC_PROTOBUF)\
	$(INC_UDP_DLB)\
	$(INC_REDISAPI)\
	$(INC_UNIFIED_CONFIG)\
	$(INC_JSON)\
	$(INC_UNIFIED_CONFIGWUJI) \
	$(INC_BOOST) \


LIB += \
	$(LIB_COMM_CACHE) \
	$(LIB_COMM_OI)\
	$(LIB_CONFIG)\
	$(LIB_VIDEOPACKET)\
	$(LIB_CSPACKET)\
	$(LIB_PACKET)\
	$(LIB_L5)\
	$(LIB_NAMEAPI)\
	$(LIB_ITIL_MCALL)\
	$(LIB_UNION)\
	$(LIB_PROTOBUF)\
	$(LIB_UDP_DLB)\
	$(LIB_REDISAPI)\
	$(LIB_UNIFIED_CONFIG)\
	$(LIB_JSON)\
	$(LIB_UNIFIED_CONFIGWUJI) \
	$(LIB_BOOST) \


LIB += -ldl -lrt -lz -lpthread

##新编译机gcc版本较高，在老版本的tlinux机器上存在glibc兼容问题，这里主要为了做兼容
LIB += $(LIB_WRAP_MEMCPY)

CC  = gcc
CXX = g++
MYFLAGS += -MMD -Wno-deprecated -pipe -O3 -D_GNU_SOURCE -D_REENTRANT -shared -fPIC 
#MYFLAGS += -MMD -Wno-deprecated -pipe -g -D_GNU_SOURCE -D_REENTRANT -shared -fPIC

SRC_DIR = ./
OBJ_DIR = ./obj/
TAR_DIR = ./lib/
TARGET = $(TAR_DIR)libdoki_square_new.so

OBJ  += $(patsubst ${SRC_DIR}%.cpp,${OBJ_DIR}%.o,$(wildcard ${SRC_DIR}*.cpp))
OBJ  += $(patsubst ${SRC_DIR}%.c,${OBJ_DIR}%.o,$(wildcard ${SRC_DIR}*.c))

####other--commSppTask
OBJ  += $(patsubst ${PATH_COMM_SPPTASK}/%.cpp,${OBJ_DIR}%.o,$(wildcard ${PATH_COMM_SPPTASK}/*.cpp))
# OBJ  += $(OBJ_DIR)http_tool.o
# OBJ  += $(OBJ_DIR)task_base.o
# OBJ  += $(OBJ_DIR)task_operate_uniondata.o
# OBJ  += $(OBJ_DIR)task_operate_redis.o
# OBJ  += $(OBJ_DIR)task_operate_httpdata.o

####other--protobuf
#OBJ += $(patsubst ${PATH_PB_PROTOCOL}/%.pb.cc,${OBJ_DIR}%.o,$(wildcard ${PATH_PB_PROTOCOL}/*.pb.cc))

.PHONY: all clean init

all : init $(TARGET)
	ldd -r $(TARGET)
	md5sum $(TARGET)
	ls -l  $(TAR_DIR)
	@echo -e "\033[1;32m\nSuccess!\033[0m"

$(TARGET): $(OBJ)
	@echo -e "\033[1;33m\nLinking $< ==> $@\033[0m"
	$(CXX) $(MYFLAGS) $^ -o $@ $(INC) $(LIB)

$(OBJ_DIR)%.o: $(SRC_DIR)%.cpp
	@echo -e "\033[1;33m\nCompiling $< ==> $@\033[0m"
	$(CXX) $(MYFLAGS) -c -o $@ $(INC) $<

$(OBJ_DIR)%.o: $(SRC_DIR)%.c
	@echo -e "\033[1;33m\nCompiling $< ==> $@\033[0m"
	$(CC) $(MYFLAGS) -c -o $@ $(INC) $<

$(OBJ_DIR)%.o: $(PATH_COMM_SPPTASK)/%.cpp
	@echo -e "\033[1;33m\nCompiling $< ==> $@\033[0m"
	$(CC) $(MYFLAGS) -c -o $@ $(INC) $<

# $(OBJ_DIR)%.o: $(PATH_PB_PROTOCOL)/%.pb.cc
# 	@echo -e "\033[1;33m\nCompiling $< ==> $@\033[0m"
# 	$(CC) $(MYFLAGS) -c -o $@ $(INC) $<

init:
	mkdir -p $(sort ./ $(TAR_DIR) $(SRC_DIR) $(OBJ_DIR))

clean:
	rm -f $(OBJ) $(TARGET)
