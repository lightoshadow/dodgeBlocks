BUILD_DIR = build

all: $(BUILD_DIR) $(BUILD_DIR)/main.gb 

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(BUILD_DIR)/main.gb: main.o ballSprite.o textTiles.o 
	../gbdk/bin/lcc -Wa-l -Wl-m -Wl-j -o $(BUILD_DIR)/main.gb main.o ballSprite.o textTiles.o 
	rm -f *.asm *.lst *.ihx *.sym *.o

main.o: main.c
	../gbdk/bin/lcc -Wa-l -Wl-m -Wl-j -c -o main.o main.c

ballSprite.o: ballSprite.c
	../gbdk/bin/lcc -Wa-l -Wl-m -Wl-j -c -o ballSprite.o ballSprite.c

textTiles.o: textTiles.c
	../gbdk/bin/lcc -Wa-l -Wl-m -Wl-j -c -o textTiles.o textTiles.c

clean:
	rm -f $(BUILD_DIR)/*.gb $(BUILD_DIR)/*.ihx $(BUILD_DIR)/*.map $(BUILD_DIR)/*.noi $(BUILD_DIR)/*.sav
	rm -f *.asm *.lst *.sym *.o
	rmdir $(BUILD_DIR) 2>/dev/null || true

.PHONY: all clean
