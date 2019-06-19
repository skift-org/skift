FONTS_JSON  = ${wildcard font/*.json}
FONTS_PNG   = ${wildcard font/*.png}
FONTS_GLYPH = ${patsubst %.png,%.glyph,$(FONTS_PNG)}

all: $(FONTS_GLYPH)

clean:
	rm -f $(FONTS_GLYPH)

install: all
	# install fonts
	mkdir -p $(SYSROOT)/res/
	mkdir -p $(SYSROOT)/res/font/
	cp -a $(FONTS_GLYPH) $(SYSROOT)/res/font/
	cp -a $(FONTS_PNG) $(SYSROOT)/res/font/

	# install mouse cursors
	cp -a mouse/ $(SYSROOT)/res

	# install wallpapers
	cp -a wallpaper/ $(SYSROOT)/res

.PHONY: all clean install

font/%.glyph: font/%.json
	fontcompiler.py $^ $@