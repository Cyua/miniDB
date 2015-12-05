WICKYDB = wickydb
WICKY = bin/main.o bin/WickyEngine.o bin/WickyFile.o bin/Condition.o
BUFFER = bin/BufferManager.o bin/block.o
CATALOG = bin/CatalogManager.o bin/Schema.o
INDEX = bin/Index.o bin/Key.o bin/IndexManager.o bin/Node.o
INTERPRETOR = bin/Parser.o bin/Expression.o bin/Optimizer.o bin/Plan.o bin/lex.yy.o bin/y.tab.o
#ALOGRITHM = bin/BTree.o
RECORD = bin/RecordManager.o bin/Table.o
LIB = lib/libfl.a lib/liby.a

RM 		= rm
CC		= g++
SRC		= src

wickydb.exe: $(WICKY) $(BUFFER) $(CATALOG) $(INDEX) $(INTERPRETOR) $(ALOGRITHM) $(RECORD)
	g++ -o $(WICKYDB) \
		$(WICKY) $(BUFFER) $(CATALOG) $(INDEX) $(INTERPRETOR) $(ALOGRITHM) $(RECORD)
		
###################WICKY###############
bin/main.o: src/main.cpp \
	include/WickyEngine.h \
	include/CatalogManager.h
	g++ -c -o bin/main.o src/main.cpp -Iinclude
	
bin/WickyEngine.o: src/Wicky/WickyEngine.cpp include/WickyEngine.h \
	include/Table.h \
	include/Condition.h \
	include/BufferManager.h \
	include/WickyFile.h 
	g++ -c -o bin/WickyEngine.o src/Wicky/WickyEngine.cpp -Iinclude
	 
bin/WickyFile.o: src/Wicky/WickyFile.cpp include/WickyFile.h
	g++ -c -o bin/WickyFile.o src/Wicky/WickyFile.cpp -Iinclude
	
bin/Condition.o: src/Wicky/Condition.cpp include/Condition.h
	g++ -c -o bin/Condition.o src/Wicky/Condition.cpp -Iinclude
	
###################BUFFER##############
bin/BufferManager.o: src/Buffer/BufferManager.cpp include/BufferManager.h \
	include/WickyFile.h \
	include/Schema.h \
	include/Block.h
	g++ -c -o bin/BufferManager.o src/Buffer/BufferManager.cpp -Iinclude

bin/block.o: src/Buffer/Block.cpp include/Block.h \
	include/WickyFile.h \
	include/BufferManager.h
	g++ -c -o bin/Block.o src/Buffer/Block.cpp -Iinclude

###################Catalog#############
bin/CatalogManager.o: src/Catalog/CatalogManager.cpp include/CatalogManager.h \
	include/Schema.h \
	include/BufferManager.h
	g++ -c -o bin/CatalogManager.o src/Catalog/CatalogManager.cpp -Iinclude

bin/Schema.o: src/Catalog/Schema.cpp include/Schema.h \
	include/CatalogManager.h
	g++ -c -o bin/Schema.o src/Catalog/Schema.cpp -Iinclude
	
###################Index###############
bin/Index.o: src/Index/Index.cpp include/Index.h \
	include/Key.h \
	include/Schema.h \
	include/BufferManager.h \
	include/Node.h \
	include/Block.h
	g++ -c -o bin/Index.o src/Index/Index.cpp -Iinclude
	
bin/Key.o: src/Index/Key.cpp include/Key.h
	g++ -c -o bin/Key.o src/Index/Key.cpp -Iinclude
	
bin/Node.o: src/Index/Node.cpp include/Node.h \
	include/Index.h \
	include/Block.h
	g++ -c -o bin/Node.o src/Index/Node.cpp -Iinclude
	
bin/IndexManager.o: src/Index/IndexManager.cpp include/IndexManager.h \
	include/Schema.h \
	include/Index.h
	g++ -c -o bin/IndexManager.o src/Index/IndexManager.cpp -Iinclude
	
###################Interpretor: Sql####
bin/Evaluator.o: src/Interpretor/Evaluator.cpp include/Evaluator.h \
	include/Plan.h \	
	g++ -c -o bin/Evaluator.o src/Interpretor/Evaluator.cpp -Iinclude
	
bin/Expression.o: src/Interpretor/Expression.cpp include/Expression.h
	g++ -c -o bin/Expression.o src/Interpretor/Expression.cpp -Iinclude
	
bin/Optimizer.o: src/Interpretor/Optimizer.cpp include/Optimizer.h
	g++ -c -o bin/Optimizer.o src/Interpretor/Optimizer.cpp -Iinclude
	
bin/Parser.o: src/Interpretor/Parser.cpp include/Parser.h \
	include/Expression.h \
	src/Interpretor/lex.yy.c \
	src/Interpretor/y.tab.c \
	include/y.tab.h
	g++ -c -o bin/Parser.o src/Interpretor/Parser.cpp -Iinclude
	
bin/Plan.o: src/Interpretor/Plan.cpp include/Plan.h
	g++ -c -o bin/Plan.o src/Interpretor/Plan.cpp -Iinclude
	
bin/lex.yy.o:src/Interpretor/lex.yy.c \
	include/Parser.h \
	include/y.tab.h
	g++ -c -o bin/lex.yy.o src/Interpretor/lex.yy.c -Iinclude
	 
bin/y.tab.o:src/Interpretor/y.tab.c include/y.tab.h \
	include/stack.hh \
	include/location.hh \
	include/position.hh
	g++ -c -o bin/y.tab.o src/Interpretor/y.tab.c -Iinclude
	
###################Algorithm###########
	
###################Record##############
bin/RecordManager.o:src/Record/RecordManager.cpp include/RecordManager.h \
	include/Table.h
	g++ -c -o bin/RecordManager.o src/Record/RecordManager.cpp -Iinclude
	
bin/Table.o:src/Record/Table.cpp include/Table.h
	g++ -c -o bin/Table.o src/Record/Table.cpp -Iinclude

###################Sql#################
src/Interpretor/lex.yy.c: src/Interpretor/SqlScanner.l \
	src/Interpretor/y.tab.c \
	include/Parser.h \
	include/y.tab.h	
	flex src/Interpretor/SqlScanner.l
	mv lex.yy.c src/Interpretor/lex.yy.c
	
src/Interpretor/y.tab.c: src/Interpretor/SqlParser.y \
	include/Parser.h \
	include/Condition.h
	bison -y src/Interpretor/SqlParser.y	
	mv y.tab.c src/Interpretor/y.tab.c
	mv y.tab.h include/y.tab.h	
	mv location.hh include/location.hh
	mv position.hh include/position.hh
	mv stack.hh include/stack.hh

.PHONY: clean
clean:
	$(RM) bin/*.o $(WICKYDB) $(WICKYDB).exe
