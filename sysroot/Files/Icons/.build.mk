ICONS=$(wildcard sysroot/Files/Icons/*.svg)

ICONS_AT_18PX = $(patsubst sysroot/Files/Icons/%.svg, $(SYSROOT)/Files/Icons/%@18px.png, $(ICONS))
ICONS_AT_24PX = $(patsubst sysroot/Files/Icons/%.svg, $(SYSROOT)/Files/Icons/%@24px.png, $(ICONS))
ICONS_AT_36PX = $(patsubst sysroot/Files/Icons/%.svg, $(SYSROOT)/Files/Icons/%@36px.png, $(ICONS))
ICONS_AT_48PX = $(patsubst sysroot/Files/Icons/%.svg, $(SYSROOT)/Files/Icons/%@48px.png, $(ICONS))

TARGETS += $(ICONS_AT_18PX) $(ICONS_AT_24PX) $(ICONS_AT_36PX) $(ICONS_AT_48PX)

$(SYSROOT)/Files/Icons/%@18px.png: sysroot/Files/Icons/%.svg
	$(DIRECTORY_GUARD)
	@echo [ImageMagick] $(notdir $@)
	@convert -depth 8 -background none -resize 18x18 $< $@

$(SYSROOT)/Files/Icons/%@24px.png: sysroot/Files/Icons/%.svg
	$(DIRECTORY_GUARD)
	@echo [ImageMagick] $(notdir $@)
	@convert -depth 8 -background none -resize 24x24 $< $@

$(SYSROOT)/Files/Icons/%@36px.png: sysroot/Files/Icons/%.svg
	$(DIRECTORY_GUARD)
	@echo [ImageMagick] $(notdir $@)
	@convert -depth 8 -background none -resize 36x36 $< $@

$(SYSROOT)/Files/Icons/%@48px.png: sysroot/Files/Icons/%.svg
	$(DIRECTORY_GUARD)
	@echo [ImageMagick] $(notdir $@)
	@convert -depth 8 -background none -resize 48x48 $< $@
