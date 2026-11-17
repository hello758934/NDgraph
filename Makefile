# Optional official Ndless SDK build. TNS Tool Portable uses .tnsproject.json.
GCC = nspire-gcc
LD = nspire-ld
GENZEHN = genzehn
CFLAGS = -Wall -Wextra -marm -Os -std=c99
LDFLAGS =
OBJS = ndgraph_ndless.o ndgraph_expr.o ndgraph_features.o complex_math.o
.PHONY: all test clean
all: NDgraph.tns
%.o: %.c ndgraph_expr.h ndgraph_features.h complex_math.h
	$(GCC) $(CFLAGS) -c $< -o $@
NDgraph.elf: $(OBJS)
	$(LD) $(OBJS) -o $@ $(LDFLAGS)
NDgraph.tns: NDgraph.elf
	$(GENZEHN) --input NDgraph.elf --output NDgraph.zehn --name "NDgraph"
	make-prg NDgraph.zehn NDgraph.tns
	rm -f NDgraph.zehn
test:
	gcc -std=c99 -O2 -Wall -Wextra -Werror -pedantic ndgraph_expr.c complex_math.c test_ndgraph.c -lm -o test_ndgraph
	./test_ndgraph
	gcc -std=c99 -O2 -Wall -Wextra -Werror -pedantic complex_math.c test_complex.c -lm -o test_complex
	./test_complex
	gcc -std=c99 -O2 -Wall -Wextra -Werror -pedantic ndgraph_expr.c ndgraph_features.c complex_math.c test_features.c -lm -o test_features
	./test_features
	gcc -std=c99 -O2 -Wall -Wextra -Werror -pedantic ndgraph_expr.c complex_math.c test_angles.c -lm -o test_angles
	./test_angles
clean:
	rm -f $(OBJS) NDgraph.elf NDgraph.zehn NDgraph.tns test_ndgraph test_complex test_features test_angles
