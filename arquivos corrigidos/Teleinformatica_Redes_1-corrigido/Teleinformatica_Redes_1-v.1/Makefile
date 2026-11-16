# Compilador e flags
CXX      = g++
INCS     = -I. -IcamadaFisica -IcamadaEnlace -Itesting
CXXFLAGS = -Wall -std=c++17 $(INCS)

BUILD_DIR = build

# --- fontes comuns (camadas + simulador, sem GTK) ---
CORE_SRCS = camadaFisica/camadaFisica.cpp \
            camadaFisica/camadaFisicaReceptor.cpp \
            camadaFisica/camadaFisicaTransmissor.cpp \
            camadaEnlace/camadaEnlace.cpp \
            simulador.cpp

# --- programa 1: teste da camada fisica ---
FISICA_SRCS = main.cpp testing/testeCamadaFisica.cpp \
              camadaFisica/camadaFisica.cpp \
              camadaFisica/camadaFisicaReceptor.cpp \
              camadaFisica/camadaFisicaTransmissor.cpp

# --- programa 2: teste TX->canal->RX pelo terminal ---
CLI_SRCS = testing/teste_simulador.cpp $(CORE_SRCS)

# --- programa 3: interface grafica (gtkmm 3) ---
GUI_SRCS   = main_gui.cpp $(CORE_SRCS)
GTK_CFLAGS = $(shell pkg-config --cflags gtkmm-3.0 2>/dev/null)
GTK_LIBS   = $(shell pkg-config --libs gtkmm-3.0 2>/dev/null)

obj = $(1:%.cpp=$(BUILD_DIR)/%.o)

all: programa_fisica teste_simulador

programa_fisica: $(call obj,$(FISICA_SRCS))
	$(CXX) $^ -o $@

teste_simulador: $(call obj,$(CLI_SRCS))
	$(CXX) $^ -o $@

# make gui  (precisa de: sudo apt install libgtkmm-3.0-dev pkg-config)
gui: $(call obj,$(GUI_SRCS))
	$(CXX) $^ -o simulador_gui $(GTK_LIBS)

# main_gui.cpp e grafico.hpp precisam dos headers do gtkmm
$(BUILD_DIR)/main_gui.o: main_gui.cpp grafico.hpp simulador.hpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) $(GTK_CFLAGS) -c $< -o $@

$(BUILD_DIR)/%.o: %.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

test: teste_simulador
	./teste_simulador

clean:
	rm -rf $(BUILD_DIR) programa_fisica teste_simulador simulador_gui

.PHONY: all gui test clean
