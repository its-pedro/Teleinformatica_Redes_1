# Compilador e flags de otimização/aviso
CXX = g++
CXXFLAGS = -Wall -std=c++17 -IcamadaFisica -Itesting

# Diretórios
SRC_DIR = .
BUILD_DIR = build

# Arquivos fonte do projeto espalhados nas pastas
SRCS = main.cpp \
        camadaFisica/camadaFisica.cpp \
        camadaFisica/camadaFisicaReceptor.cpp \
        camadaFisica/camadaFisicaTransmissor.cpp \
	    testing/testeCamadaFisica.cpp

# Mapeia os arquivos fonte para arquivos objeto (.o) dentro da pasta build
OBJS = $(SRCS:%.cpp=$(BUILD_DIR)/%.o)

# Nome do executável final
TARGET = programa_fisica

# Regra principal
all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(OBJS) -o $(TARGET)

# Regra genérica para compilar qualquer .cpp mantendo a estrutura de subpastas no build
$(BUILD_DIR)/%.o: %.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Limpeza dos binários gerados
clean:
	rm -rf $(BUILD_DIR) $(TARGET)

.PHONY: all clean