# Makefile for RClient
# Copyright 2014 FactSet Research Systems Inc.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#
#
# 'make' to build executable
# 'make DEBUG=1' to build debug version
# 'make clean' to remove all constructed files
#

CXX=g++
CXXFLAGS=-Wall -Werror $(if $(DEBUG),-O0 -g,-O1)

RCLIENT=	endian_converter.cpp \
		network_error.cpp \
		network_manager.cpp \
		qap1_header.cpp \
		rclient.cpp \
		rexp.cpp \
		rexp_double.cpp \
		rexp_integer.cpp \
		rexp_null.cpp \
		rexp_string.cpp \
		rexp_vector.cpp \
		rpacket.cpp \
		rpacket_entry_0103.cpp

DEMO= demo.o
EXECUTABLE=demo
OBJECTS= $(RCLIENT:.cpp=.o)

.PHONY : all
all: $(EXECUTABLE)

$(EXECUTABLE):  $(OBJECTS) $(DEMO)
	$(CXX) $(DEMO) $(OBJECTS) -o $@

.PHONY : clean
clean:
	$(RM) $(DEMO) $(OBJECTS) $(EXECUTABLE) *~
