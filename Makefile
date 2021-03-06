NAME = ft_vox
CC = clang++

LIB_GLFW_NAME = .glfw-3.2.1-mojave-fix# remove `-mojave-fix` for macs at school, fix : https://github.com/glfw/glfw/pull/1346
LIB_GLAD_NAME = .glad
LIB_GLM_NAME = .glm
LIB_ASSIMP_NAME = .assimp-4.1.0
LIB_TBB_NAME = .tbb

SRC_PATH = ./src/
OBJ_PATH = ./obj/
LIB_PATH = $(HOME)/
INC_PATH = ./include/ \
		   ./lib/ \
		   $(LIB_PATH)$(LIB_GLFW_NAME)/include/ \
		   $(LIB_PATH)$(LIB_GLAD_NAME)/include/ \
		   $(LIB_PATH)$(LIB_GLM_NAME)/ \
		   $(LIB_PATH)$(LIB_ASSIMP_NAME)/include/ \
		   $(LIB_PATH)$(LIB_FNS_NAME)/ \

CC_FLGS = -std=c++11 -Ofast
CC_LIBS = -lassimp -lglfw3 -framework AppKit -framework OpenGL -framework IOKit -framework CoreVideo

SRC_NAME = main.cpp PostProcess.cpp Light.cpp Cubemap.cpp Terrain.cpp Chunk.cpp \
		   Camera.cpp Controller.cpp Env.cpp Renderer.cpp Shader.cpp utils.cpp
OBJ_NAME = $(SRC_NAME:.cpp=.o)

SRC = $(addprefix $(SRC_PATH), $(SRC_NAME))
OBJ = $(addprefix $(OBJ_PATH), $(OBJ_NAME))
INC = $(addprefix -I,$(INC_PATH))
LIB_GLFW = -L $(LIB_PATH)$(LIB_GLFW_NAME)/src
LIB_GLAD = $(LIB_PATH)$(LIB_GLAD_NAME)/src/glad.c
LIB_ASSIMP = -L $(LIB_PATH)$(LIB_ASSIMP_NAME)/lib

all: $(NAME)

$(NAME): $(OBJ)
	$(CC) $(CC_FLGS) $(LIB_GLFW) $(LIB_GLAD) $(LIB_ASSIMP) $(INC) $(OBJ) $(CC_LIBS) -o $(NAME)

$(OBJ_PATH)%.o: $(SRC_PATH)%.cpp
	mkdir -p $(OBJ_PATH)
	$(CC) $(CC_FLGS) $(INC) -o $@ -c $<

clean:
	rm -fv $(OBJ)
	rm -rf $(OBJ_PATH)

fclean: clean
	rm -fv $(NAME)

re: fclean all
