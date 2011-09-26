
BUILD = debug
MODE = http
PORT = 19022
ADDRESS = 127.0.0.1

.SECONDEXPANSION:

EXE_FILE = thechess-$(MODE).wt
EXE = ./$(BUILD)/$(EXE_FILE)
WT_CONFIG = $(BUILD)/wt_config_$(MODE).xml
EXE_PATH = $(EXE)
APPROOT = .
DOCROOT_PARENT = .
FCGI_RUN_DIR = $(BUILD)/run
PID_FILE = $(BUILD)/thechess-$(MODE).pid
SOCKET = $(BUILD)/thechess-$(MODE).socket
DOCROOT = $(DOCROOT_PARENT)/files

ifeq ($(BUILD), release)
NOOBJECTS = TRUE
endif
ifeq ($(MAKECMDGOALS), doc)
NOOBJECTS = TRUE
endif
ifeq ($(MAKECMDGOALS), style)
NOOBJECTS = TRUE
endif

CXX = ccache g++
LINK = g++
LIBS += -lcrypt -lpthread
LIBS += -lboost_signals -lboost_regex -lboost_system -lboost_thread
LIBS += -lwt -lwtdbo -lwtdbosqlite3 -lwtdbopostgres
LIBS += -lwt$(MODE)
CXXFLAGS += -pipe -Wall -W
CXXFLAGS += -I$(BUILD) -Isrc
ifeq ($(BUILD), debug)
CXXFLAGS += -g -O0
CXXFLAGS += -DRUN_TESTS
LFLAGS += -O0
else
CXXFLAGS += -O3
LFLAGS += -O3
ifeq (,$(NOOBJECTS))
CXXFLAGS += -flto
LFLAGS += -flto
endif
endif

sources = $(wildcard src/*.cpp) $(wildcard src/*/*.cpp)
headers = $(wildcard src/*.hpp) $(wildcard src/*/*.hpp)
ifeq (,$(NOOBJECTS))
objects = $(subst src/,$(BUILD)/,$(sources:.cpp=.o))
makefiles = $(objects:.o=.d)
tosource = src/$*.cpp
precompiled = $(BUILD)/model/all.hpp.gch
toheader = src/$*.hpp
endif
CHESS_OBJECTS = $(filter $(BUILD)/chess/%,$(objects))

ifeq ($(MODE), http)
RUN_COMMAND = WT_CONFIG_XML=$(WT_CONFIG) $(DEBUGGER) $(EXE_PATH) --http-address=$(ADDRESS) --http-port=$(PORT) \
			  --docroot="$(DOCROOT)/;/resources,/img,/js,/css,/tinymce,/favicon.ico" -p $(PID_FILE)
else
RUN_COMMAND = WT_CONFIG_XML=$(WT_CONFIG) spawn-fcgi -n -f $(EXE_PATH) -P $(PID_FILE)
ifeq (,$(SOCKET))
RUN_COMMAND += -a $(ADDRESS) -p $(PORT)
else
RUN_COMMAND += -s $(SOCKET)
endif
endif

build: $$(EXE)

chess: $$(CHESS_OBJECTS)

check: locales

.PHONY: dep
dep:
	apt-get install spawn-fcgi `apt-cache depends witty| awk '{$$1 = ""; print $$0}'`

.PHONY: build-dep
build-dep:
	apt-get install g++ ccache upx inkscape doxygen texlive graphviz \
		libwt-dev libwtdbo-dev libwtdbopostgres-dev libwtdbosqlite-dev libwtfcgi-dev libwthttp-dev

include $(makefiles)

$(BUILD)/%.d: $$(tosource)
	mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) $< -MM | sed 's,.\+\.o[ :]*,$(@:.d=.o) $@ : ,' > $@;

$(EXE): $$(sources) $$(headers) $$(makefiles) $$(objects)
	mkdir -p $(dir $@)
ifeq (,$(NOOBJECTS))
	$(LINK) $(LFLAGS) $(LIBS) $(objects) -o $@
else
	$(CXX) $(LFLAGS) $(LIBS) $(CXXFLAGS) \
		$(addprefix -include ,$(filter-out $(firstword $(sources)),$(sources))) \
		$(firstword $(sources)) -o $@
endif
ifeq ($(BUILD), release)
	upx -9 $@
endif

$(BUILD)/%.o: $$(precompiled)
	$(CXX) -c $(CXXFLAGS) $(tosource) -o $@

.SECONDARY: $$(precompiled)

$(BUILD)/%.hpp.gch:
	$(CXX) $(CXXFLAGS) $(toheader) -o $@

run: $(EXE) $$(WT_CONFIG)
	$(RUN_COMMAND)

.PHONY: doc
doc:
	doxygen

.PHONY: style
style:
	astyle --style=java --add-brackets --lineend=linux --align-pointer=type --align-reference=type \
		--delete-empty-lines --convert-tabs --pad-header --pad-oper --unpad-paren \
		--indent-col1-comments \
		--quiet --recursive '*.?pp'
	sed '/^$$/N;/^\n$$/D' -i `find src -name '*.?pp'`
	sed '/define/!s/\(FOREACH\|THECHESS_\w\+\)(/\1 (/' -i `find src -name '*.?pp'`
	sed '/\(FOREACH\|THECHESS_\)/s/\(\w\) \([*&]\)/\1\2/' -i `find src -name '*.?pp'`
	sed '$$s@\(.\+\)@\1\n@g' -i `find -name '*.?pp'`

.SECONDARY: $(WT_CONFIG)
$(WT_CONFIG): /etc/wt/wt_config.xml Makefile
	mkdir -p $(dir $@)
	cp --backup=numbered $< $@
	chmod 660 $@
ifeq ($(MODE), fcgi)
	mkdir -p $(FCGI_RUN_DIR)
	sed 's@$(FCGI_RUN_DIR_ORIGINAL)@$(FCGI_RUN_DIR)@' -i $@
	sed 's@<num-threads>1</num-threads>@<num-threads>2</num-threads>@' -i $@
endif
	sed 's@<log-file></log-file>@<log-file>$(LOG_FILE)</log-file>@' -i $@
	sed 's@<behind-reverse-proxy>false</behind-reverse-proxy>@<behind-reverse-proxy>true</behind-reverse-proxy>@' -i $@
	sed 's@</properties>@<property name="approot">$(APPROOT)</property>\
		<property name="tinyMCEBaseURL">tinymce/</property>\
		</properties>@' -i $@

.SECONDARY: wt.xml
wt.xml:
	wget -O $@ http://redmine.emweb.be/projects/wt/repository/revisions/master/raw/src/xml/wt.xml

.PHONY: locales
locales: wt.xml
	./locales_test.py --wt=$<
