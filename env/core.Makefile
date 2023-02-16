#############################################
# 用户设置变量说明
#############################################
# NAME : 模块名称
# BUILD_TARGET_TYPE : 编译目标类型,取值static, shared, exe
# INCDIR : 搜索目录列表(目前前加 -I)
# SRC : 源码文件 (使用相对路径，如果用绝对路径，输出目录嵌套很多层)
# TEST_SRC : 测试源码文件
# OUT_DIR : 生成库或可执行文件的路径
# DYN_LDS_WITH : 需要动态链接的库
# STATIC_LDS_WITH : 需要静态链接的库
# DEPS_TARGET : 扩展依赖对象
#
# ROOT_DIR : 项目根目录
# DEBUG : 是否debug, 取值(y,n)
# PRINT_COMPILER : 是否打印编译信息, 取值(y, n)
# DONE : 结束后的处理
#
# AR : 函数库打包器
# AS : 汇编器
# CC : C编译器
# CXX : C++编译器
# CPP : C预处理器
# LD : 链接器
# ARFLAGS : 函数库打包选项
# ASFLAGS : 汇编选项
# CFLAGS : C编译选项
# CXXFLAGS : C++编译选项
# CPPFLAGS : C预处理选项
# LDFLAGS : 链接选项
# RM : 删除命令，默认是"rm -f"
#############################################
##############################################
#	初始设置build输出目录
##############################################
SUB_OUT_DIR := $(subst $(ROOT_DIR),,$(shell pwd))
BUILD_OUT_PATH := $(ROOT_DIR)/out/objs/$(SUB_OUT_DIR)

# C++文件后缀
CCSUFIX = cpp
# 删除命令
RM = rm -f
ifeq ($(PRINT_COMPILER), y)
Q_ :=
else
Q_ := @
endif

# 编译条件设置
ifeq ($(DEBUG), y)
CFLAGS += -g
CXXFLAGS += -g
else
CFLAGS += -O2
CXXFLAGS += -O2
endif

# C编译选项
CFLAGS += $(INCDIR) 
# C++编译选项
CXXFLAGS += $(INCDIR)
# 编译预处理选项
#CFLAGS += -MMD

# 默认静态库
TARGET := lib$(NAME).a

ifeq ($(findstring shared,$(BUILD_TARGET_TYPE)), shared)
TARGET := lib$(NAME).so
LDFLAGS += -shared -fpic -Wl,--whole-archive 
ifeq ($(OUTPUT_UNDEFINED_ERROR), y)
LDFLAGS += -z defs
endif
endif

ifeq ($(findstring static,$(BUILD_TARGET_TYPE)), static)
TARGET := lib$(NAME).a
endif

ifeq ($(findstring exe,$(BUILD_TARGET_TYPE)), exe)
TARGET := $(NAME)
SRC += $(TEST_SRC)
# 如下选项，编译依赖的动态库也要打开
#  -Wl,--as-needed 选项，链接过程中，链接器会检查所有的依赖库，没有实际被引用的库，不再写入可执行文件头。最终生成的可执行文件头中包含的都是必要的链接库信息。
#  -Wl,--gc-sections，不链接未用函数，减小可执行文件大小
#LDFLAGS += -Wl,--gc-sections -Wl,--as-needed
LDFLAGS += -Wl,--gc-sections
endif

# 编译条件处理
OBJ += $(patsubst %.c,$(BUILD_OUT_PATH)/%.o,$(patsubst %.$(CCSUFIX),$(BUILD_OUT_PATH)/%.o,$(SRC))) 
BIN := $(OUT_DIR)/$(TARGET) 

.PHONY:	all clean distclean tags deps release $(DEPS_TARGET) copy

all: $(DEPS_TARGET) $(BIN) 

## 增加依赖文件
ifeq ($(BUILD_OUT_PATH), $(wildcard $(BUILD_OUT_PATH)))
include $(wildcard $(addsuffix /*.d,$(shell find $(BUILD_OUT_PATH) -type d)))
endif

## 重定义隐含规则
$(BUILD_OUT_PATH)/%.o: %.c
	@echo '<$(CC)>[$(DEBUG)] Compiling object file "$@" ...'
	$(Q_)mkdir -p $(dir $@)
	$(Q_)$(CC) $(CFLAGS) -c -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"

$(BUILD_OUT_PATH)/%.o: %.$(CCSUFIX)
	@echo '<$(CXX)>[$(DEBUG)] Compiling object file "$@" ...'
	$(Q_)mkdir -p $(dir $@)
	$(Q_)$(CXX) $(CXXFLAGS) -c -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"

clean:
	@echo remove all objects
	$(Q_)$(RM) -rf $(BUILD_OUT_PATH) $(BIN) $(CLEAN_OBJ) *.d

distclean:
	@echo remove all objects and deps
	$(Q_)$(RM) -rf $(BUILD_OUT_PATH) $(BIN) $(CLEAN_OBJ) *.d

# Rebuild
rebuild: distclean all

$(BIN): $(OBJ) 
ifeq ($(OUT_DIR), $(wildcard $(OUT_DIR)))
	@echo
else
	mkdir -p $(OUT_DIR)
endif
ifeq ($(findstring exe,$(BUILD_TARGET_TYPE)), exe)
	@echo '<$(LD)>creating binary "$(BIN)"'
	$(Q_)$(LD)  $(LDFLAGS) $(OBJ)  $(DYN_LDS_WITH) $(STATIC_LDS_WITH) -o $(BIN) && chmod a+x $(BIN)
else
ifeq ($(findstring shared,$(BUILD_TARGET_TYPE)), shared)
	@echo '<$(LD)>creating shared lib "$(BIN)"'
	$(Q_)$(LD)  $(LDFLAGS) $(OBJ) $(STATIC_LDS_WITH) $(DYN_LDS_WITH) -Wl,--no-whole-archive -o $(BIN) && chmod a+x $(BIN)
else
	@echo '<$(AR)>creating static lib "$(BIN)"'
	$(Q_)$(AR) rc $@ $^
endif
endif

#编译结束后的处理
	$(STRIP) $(BIN)
	$(DONE)
	@echo '... done'
	@echo
